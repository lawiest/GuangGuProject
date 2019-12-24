/* -*-c++-*- */
/* osgEarth - Dynamic map generation toolkit for OpenSceneGraph
 * Copyright 2008-2014 Pelican Mapping
 * http://osgearth.org
 *
 * osgEarth is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include <osgEarth/TileSource>
#include <osgEarth/ImageToHeightFieldConverter>
#include <osgEarth/Registry>
#include <osgEarth/TimeControl>
#include <osgEarth/XmlUtils>
#include <osgEarth/ImageUtils>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osg/ImageSequence>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <iomanip>

#include "GEOWMTSOptions.h"
#include <Tool/Cache.h>

#define LC "[WMS] "


using namespace osgEarth::Util;
using namespace GeoGlobe;
using namespace GeoGlobe::Tool;
using namespace osgEarth;




//----------------------------------------------------------------------------

class GEOWMTSSource : public osgEarth::TileSource
{
public:
	GEOWMTSSource( const osgEarth::TileSourceOptions& options ) : TileSource( options ), _options(options)
    {
		if (_options.cachePath().isSet()&&_options.layers().isSet())
		{
			_cacheSqlite = _options.cachePath().value()+"/"+_options.layers().value()+".tile";
		}
		else
		{
			_cacheSqlite = "tempt.tile";
		}
		_cache=new GeoGlobe::Tool::SQLiteCache(_cacheSqlite, eImageCache);
		_cache->createImageCache();
    }
	std::string wmtsstringRight(const std::string& input,const int num)
	{
		std::string output(input);
		int m_temnum=output.size()-num;
		return output.substr(m_temnum,num);
	}
    /** override */
    Status initialize( const osgDB::Options* dbOptions )
    {
		if ((!_options.layers().isSet())||(!_options.url().isSet()))
		{
			return Status::Error( "Unable to get data." );
		}

        osg::ref_ptr<const Profile> result;

		std::string wmtsFormatToUse = _options.wmtsFormat().value();

        char sep = _options.url()->full().find_first_of('?') == std::string::npos? '?' : '&';

		//Try to read the WMS capabilities and write cache
		osg::ref_ptr<WMTSCapabilities> capabilities=NULL;
		osgEarth::URI capUrl = _options.capabilitiesUrl().value();
		if ( capUrl.empty() )
		{
			capUrl = URI(
				_options.url()->full() + 
				sep + 
				std::string("SERVICE=WMTS") +
				std::string("&VERSION=") + _options.wmtsVersion().value() +
				std::string("&REQUEST=GetCapabilities") );
		}
		if (_options.cachePath().isSet()&&_options.layers().isSet())
		{
			std::string capCachePath = _options.cachePath().value()+"/"+"Cap-"+_options.layers().value()+".xml";
			
			capabilities = WMTSCapabilitiesReader::read(capUrl.full(),capCachePath,dbOptions );
		}
		else
			capabilities = WMTSCapabilitiesReader::read(capUrl.full(),dbOptions );
        
        if ( !capabilities.valid() )
        {
            return Status::Error( "Unable to read WMTS GetCapabilities." );
        }
        else
        {
            OE_INFO << "[osgEarth::WMTS] Got capabilities from " << capUrl.full() << std::endl;
        }
		int layerIndex=0;
		int matrixsetIndex=0;
		if (_options.layers().isSet())//ture,judge at first
		{
			for(int i=0;i<capabilities->getLayer().size();i++)
			{
				if (_options.layers() == capabilities->getLayerAt(i).Title)
				{
					layerIndex=i;
					_formatToUse = capabilities->getLayerAt(i).format;
					std::string link = capabilities->getLayerAt(i).TileMatrixSetLink;
					for(int j=0;j<capabilities->getMatrixSet().size();j++)
					{
						if(link==capabilities->getMatrixSetAt(j).ID)
						{
							matrixsetIndex=j;
							_srsToUse = capabilities->getMatrixSetAt(j).SupportedCRS;
							break;
						}
					}
					break;
				}
			}//for layer
			if ( _formatToUse.empty() )
				_formatToUse = "image/png";
			//////////////////////////////////////////////////get profile
			if ( _srsToUse.empty())
			{
				_srsToUse = "wgs84";
			}
			else
			{
				int pos=_srsToUse.find(":EPSG",0);
				_srsToUse=wmtsstringRight(_srsToUse,(_srsToUse.size()-pos-1));
				_srsToUse = osgDB::convertToLowerCase(_srsToUse);
			}
			osg::ref_ptr<osgEarth::SpatialReference> wmts_srs = osgEarth::SpatialReference::create( _srsToUse );
			//////////////////////////////////////////////////////////////get the min and maxlevel,to the dataextent
			int levnum=capabilities->getMatrixSetAt(matrixsetIndex).TileMatrixs.size();
			int minlevel=100,maxlevel=0;
			for (int i =0;i<levnum;i++)
			{
				int lev=atoi(capabilities->getMatrixSetAt(matrixsetIndex).getTileMatrixAt(i).ID.c_str());
				if (lev<minlevel)
				{
					minlevel=lev;
				}
				if (lev>maxlevel)
				{
					maxlevel=lev;
				}
			}
			double minx, miny, maxx, maxy;                
			minx = miny = maxx = maxy = 0;
			minx = capabilities->getLayerAt(layerIndex).wgs84bound.west;
			maxx = capabilities->getLayerAt(layerIndex).wgs84bound.east;
			miny = capabilities->getLayerAt(layerIndex).wgs84bound.south;
			maxy = capabilities->getLayerAt(layerIndex).wgs84bound.north;
			
			//If we still don't have any extents, just default to global geodetic.
			if (!result.valid() && minx == 0 && miny == 0 && maxx == 0 && maxy == 0)
			{
				result = osgEarth::Registry::instance()->getGlobalGeodeticProfile();
			}
		
			if (!result.valid())
			{
				result = osgEarth::Profile::create( _srsToUse, minx, miny, maxx, maxy );
			}
			if ( !result.valid())
			{
				result = osgEarth::Registry::instance()->getGlobalGeodeticProfile();
			}
			//Add the layer extents to the list of valid areas
			if (minx != 0 || maxx != 0 || miny != 0 || maxy != 0)
			{
			 osgEarth::GeoExtent extent( result->getSRS(), minx, miny, maxx, maxy);
				getDataExtents().push_back(osgEarth::DataExtent(extent, minlevel-1,maxlevel-1) );
			}
			////////////////////////////////////////////////////////
			std::stringstream buf;
			buf
				<< std::fixed << _options.url()->full() << sep
				<< "SERVICE=WMTS"
				<< "&VERSION=" << _options.wmtsVersion().value()
				<< "&REQUEST=GetTile"
				<< "&LAYER=" <<_options.layers().value()
				<< "&FORMAT=" << ( wmtsFormatToUse.empty() ?  _formatToUse : std::string("image/") +  wmtsFormatToUse )
				//<< "&FORMAT=" << _formatToUse
				<< "&TILEMATRIXSET=" << capabilities->getMatrixSetAt(matrixsetIndex).ID;
			_prototype = buf.str();

		}//if (_options.layers().isSet())
		
        // Use the override profile if one is passed in.
        if ( getProfile() == 0L )
        {
            setProfile( result.get() );
        }

        if ( getProfile() )
        {
            OE_NOTICE << "[osgEarth::WMS] Profile=" << getProfile()->toString() << std::endl;

            // set up the cache options properly for a TileSource.
            _dbOptions = osgEarth::Registry::instance()->cloneOrCreateOptions( dbOptions );

            return STATUS_OK;
        }
        else
        {
            return Status::Error( "Unable to establish profile" );
        }
    }


public:

	std::string createImageTileFileName(int lev,int col,int row)
	{

		std::stringstream ss;
		ss<<lev<<"/"<<col<<"/"<<row<<".1.osgearth_engine_mp_tile_image";

		return ss.str();
	}
	osg::Image* readImagefromSqlite(const osgEarth::TileKey& key, osgEarth::ProgressCallback* progress )
	{
		osg::ref_ptr<osg::Image> image=NULL;

		unsigned int lev,r,c;
		lev=key.getLevelOfDetail()+1;
		key.getTileXY(c,r);

		int tsize=0;
		int tiletype=0;
		char* tileData=NULL;

		std::string tilefn=createImageTileFileName(lev,c,r);

		if (_cache->readDataFromCache(tilefn,&tileData,tsize,tiletype))
		{
			std::string exname;
			switch(tiletype)
			{
			case 0:
				exname="jpg";
				break;
			case 3:
				exname="png";
				break;
			case 8:
				exname="tiff";
				break;
			default:
				return NULL;
			}
			std::string imgsrc;
			imgsrc.assign(tileData,tsize);
			if (imgsrc.c_str()[1]=='P'&&imgsrc.c_str()[2]=='N'&&imgsrc.c_str()[3]=='G')//png
				//if (dl.data[1]=='P'&&dl.data[2]=='N'&&dl.data[3]=='G')
			{
				exname="png";
			}
			if((imgsrc.c_str()[6]=='J'&&imgsrc.c_str()[7]=='F'&&imgsrc.c_str()[8]=='I'&&imgsrc.c_str()[9]=='F')||(imgsrc.c_str()[0]==-1&&imgsrc.c_str()[1]==-40&&imgsrc.c_str()[2]==-1)) //if (tileData[0]==-1&&tileData[1]==-40&&tileData[2]==-1)//jpg
				//if((dl.data[6]=='J'&&dl.data[7]=='F'&&dl.data[8]=='I'&&dl.data[9]=='F')||(dl.data[0]==-1&&dl.data[1]==-40&&dl.data[2]==-1))
			{
				exname="jpg";
			}
			std::stringstream ss;
			ss<<imgsrc;
			osg::ref_ptr<osgDB::ReaderWriter> reader = osgDB::Registry::instance()->getReaderWriterForExtension(exname);
			if (reader.valid())
			{
				image=(reader->readImage(ss)).getImage();	
			}//if reader

		}//if read successfully
		if (tileData!=NULL)
		{
			delete[] tileData;
			tileData=NULL;
		}
		return image.release();
	}
	osg::Image* readImagefromServiceOrCache(const osgEarth::TileKey& key,osgEarth::ProgressCallback* progress)
	{
		osg::ref_ptr<osg::Image> image=NULL;

		image=readImagefromSqlite(key,progress);

		if(!image.valid())//缓存中不存在
		{
			//缓存中不存在则读取uri并存入缓存
			unsigned int lev,r,c;
			lev=key.getLevelOfDetail()+1;
			key.getTileXY(c,r);

			std::string image_uri=createGeoWMTSURI(key);
			//////////////////////////////////////download image///////////////////

			//Download dl;
			osgEarth::ReadResult rr= osgEarth::URI(image_uri).readString();
			if (rr.succeeded())
				//if (dl.downloadData(image_uri))
			{
				std::string imgsrc=rr.getString();
				long imglen=imgsrc.size();

				////////////////judge the format of image
				int tiletype=100;//unknown format
				std::string exname;
				if (imgsrc.c_str()[1]=='P'&&imgsrc.c_str()[2]=='N'&&imgsrc.c_str()[3]=='G')//png
					//if (dl.data[1]=='P'&&dl.data[2]=='N'&&dl.data[3]=='G')
				{
					tiletype=3;
					exname="png";
				}
				if((imgsrc.c_str()[6]=='J'&&imgsrc.c_str()[7]=='F'&&imgsrc.c_str()[8]=='I'&&imgsrc.c_str()[9]=='F')||(imgsrc.c_str()[0]==-1&&imgsrc.c_str()[1]==-40&&imgsrc.c_str()[2]==-1)) //if (tileData[0]==-1&&tileData[1]==-40&&tileData[2]==-1)//jpg
					//if((dl.data[6]=='J'&&dl.data[7]=='F'&&dl.data[8]=='I'&&dl.data[9]=='F')||(dl.data[0]==-1&&dl.data[1]==-40&&dl.data[2]==-1))
				{
					tiletype=0;
					exname="jpg";
				}
				//////////////////////////
				//std::string imgsrc;
				//imgsrc.assign(dl.data, dl.size);
				std::stringstream ss;
				ss<<imgsrc;
				osg::ref_ptr<osgDB::ReaderWriter> reader = osgDB::Registry::instance()->getReaderWriterForExtension(exname);
				if(reader.valid())
				{
					image=(reader->readImage(ss)).getImage();
					if (image.valid())///////////////////////download successfully
					{
						/////////////////////////////write into sqlite////////////////////////////////////////
						std::string tilefn=createImageTileFileName(lev,c,r);
						if (_cache->writeDataToCache(tilefn,imgsrc.size(),tiletype,(char*)(imgsrc.c_str())))
						{
							_cache->commitCache(tilefn);
						}
						//////////////////////////////////write into sqlite///////////////////////////////////////////
					}//if image valid
				}//if reader valid
			}//if connect
		}//if no cache
		return image.release();
	}
    osg::Image* createImage( const osgEarth::TileKey& key, osgEarth::ProgressCallback* progress )
    {
        osg::ref_ptr<osg::Image> image=NULL;
		image=readImagefromServiceOrCache(key,progress);
        return image.release();
    }

    std::string createGeoWMTSURI( const osgEarth::TileKey& key ) const
    {
		std::string url=_prototype;

		int key_X=key.getTileX();
		int key_Y=key.getTileY();
		int key_L=key.getLevelOfDetail()+1;

		std::ostringstream buf;
		
		buf<<"&TILEMATRIX="<<key_L<<"&TILEROW="<<key_Y<<"&TILECOL="<<key_X;
		
		url+=buf.str();
		return url;
    }

private:
    const GEOWMTSOptions                 _options;

	std::string                      _cacheSqlite;
	osg::ref_ptr<GeoGlobe::Tool::SQLiteCache> _cache;

    std::string                      _formatToUse;
    std::string                      _srsToUse;
    osg::ref_ptr<const osgEarth::Profile>      _profile;
    std::string                      _prototype;
	osg::ref_ptr<osgDB::Options>     _dbOptions;
};


class GEOWMTSSourceFactory : public osgEarth::TileSourceDriver
{
    public:
        GEOWMTSSourceFactory() {}

        virtual const char* className()
        {
            return "GEOWMTS Reader";
        }
        
        virtual bool acceptsExtension(const std::string& extension) const
        {
            return osgDB::equalCaseInsensitive( extension, "osgearth_geowmts" );
        }

        virtual ReadResult readObject(const std::string& file_name, const Options* opt) const
        {
            std::string ext = osgDB::getFileExtension( file_name );
            if ( !acceptsExtension( ext ) )
            {
                return ReadResult::FILE_NOT_HANDLED;
            }

            return new GEOWMTSSource( getTileSourceOptions(opt) );
        }
};

REGISTER_OSGPLUGIN(osgearth_geowmts, GEOWMTSSourceFactory)

