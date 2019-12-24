#include "base/Service.h"
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgEarth/IOTypes>
#include <osgEarth/URI>

namespace osgEarth { namespace Util
{

	WMTSCapabilities::WMTSCapabilities()
	{

	};

	WMTSCapabilities::~WMTSCapabilities()
	{

	};

	WMTSCapabilities* WMTSCapabilitiesReader::read(const std::string &location, const osgDB::ReaderWriter::Options *options)
	{
		WMTSCapabilities *caps = NULL;
		if (osgDB::containsServerAddress(location))
		{
			ReadResult rr = osgEarth::URI(location).readString(options);
			if (rr.succeeded())
			{
				std::istringstream in(rr.getString());
				caps = read(in);
			}
		}
		else
		{
			if ((osgDB::fileExists(location)) && (osgDB::fileType(location) == osgDB::REGULAR_FILE))
			{
				std::ifstream in(location.c_str());
				caps = read(in);
			}
		}
		return caps;
	}

	WMTSCapabilities* WMTSCapabilitiesReader::read(const std::string &url, const std::string &location, const osgDB::ReaderWriter::Options* options)
	{
		WMTSCapabilities *caps = NULL;
		if (osgDB::fileExists(location))
		{
			std::ifstream in(location.c_str(), std::ios::binary);
			caps = read(in);
		}
		else
		{
			ReadResult rr = URI(url).readString(options);
			if (rr.succeeded())
			{
				std::string buf = rr.getString();
				std::istringstream in(buf);
				caps = read(in);

				osgDB::makeDirectoryForFile(location);
				std::ofstream fout;
				fout.open(location.c_str(), std::ios::out | std::ios::binary);
				fout.write(buf.c_str(), buf.size());
				fout.close();
			}
		}

		return caps;
	 }
		
		WMTSCapabilities* WMTSCapabilitiesReader::read( std::istream &in )
		{
			Config doc;
			doc.fromXML( in );
			if (doc.empty() || doc.children().empty())
			{
				OE_NOTICE << "Failed to load Capabilities " << std::endl;
				return 0;
			}
			osg::ref_ptr<WMTSCapabilities> capabilities = new WMTSCapabilities;
			//std::vector<std::string> layer;
			std::vector<std::string> style;
			std::vector<std::string> format;
			//std::vector<std::string> tileMatrixSet;
			//std::vector<std::string> tileMatrix;
			std::string srs;
			const Config contents = doc.child("capabilities").child("contents");	
			const ConfigSet layers ;
			std::vector<wmtsLayer> mLayer;
			std::vector<TileMatrixSet> mTileMatrixSet;
			for( ConfigSet::const_iterator i = contents.children().begin(); i != contents.children().end(); ++i )
			{
				const osgEarth::Config& conf = *i;
				if(conf.key()=="layer")
				{
					wmtsLayer layer;
					layer.Title = conf.child("ows:title").value();
					layer.Abstract = conf.child("ows:abstract").value();
					layer.ID = conf.child("ows:identifier").value();
					if(conf.child("style").hasChild("ows:identifier"))
						layer.style = conf.child("style").child("ows:identifier").value();
					else
						layer.style = conf.child("style").value();
					if(conf.child("format").hasChild("ows:identifier"))
						layer.format = conf.child("format").child("ows:identifier").value();
					else
						layer.format = conf.child("format").value();
					layer.TileMatrixSetLink = conf.child("tilematrixsetlink").child("tilematrixset").value();

					std::string lowerConner = conf.child("ows:wgs84boundingbox").child("ows:lowercorner").value();
					std::string upperConner = conf.child("ows:wgs84boundingbox").child("ows:uppercorner").value();
					std::string str1,str2;
					int pos = lowerConner.find(' ',0);
					str1 = lowerConner.substr(0,pos);
					str2 = lowerConner.substr(pos,lowerConner.size()-pos);
					layer.wgs84bound.west = atof(str1.c_str());
					layer.wgs84bound.south = atof(str2.c_str());

					pos = upperConner.find(' ',0);
					str1.clear();str2.clear();
					str1 = upperConner.substr(0,pos);
					str2 = upperConner.substr(pos,upperConner.size()-pos);
					layer.wgs84bound.east = atof(str1.c_str());
					layer.wgs84bound.north = atof(str2.c_str());

					lowerConner = conf.child("ows:boundingbox").child("ows:lowercorner").value();
					upperConner = conf.child("ows:boundingbox").child("ows:uppercorner").value();
					pos = lowerConner.find(' ',0);
					str1.clear();str2.clear();
					str1 = lowerConner.substr(0,pos);
					str2 = lowerConner.substr(pos,lowerConner.size()-pos);
					layer.bound.west = atof(str1.c_str());
					layer.bound.south = atof(str2.c_str());

					pos = upperConner.find(' ',0);
					str1.clear();str2.clear();
					str1 = upperConner.substr(0,pos);
					str2 = upperConner.substr(pos,upperConner.size()-pos);
					layer.bound.east = atof(str1.c_str());
					layer.bound.north = atof(str2.c_str());

					mLayer.push_back(layer);
				}

				if(conf.key()=="tilematrixset")
				{
					TileMatrixSet tileMatrixset;
					tileMatrixset.ID = conf.child("ows:identifier").value();
					tileMatrixset.SupportedCRS = conf.child("ows:supportedcrs").value();
					for( ConfigSet::const_iterator j = conf.children().begin(); j != conf.children().end(); ++j )
					{
						const osgEarth::Config& matrix = *j;
						if(matrix.key()=="tilematrix")
						{
							TileMatrix tileMatrix;
							tileMatrix.ID = matrix.child("ows:identifier").value();
							tileMatrix.ScaleDenominator = atof(matrix.child("scaledenominator").value().c_str());
							std::string topleft = matrix.child("topleftcorner").value();
							std::stringstream ss(topleft);
							ss >>  tileMatrix.north >>  tileMatrix.west;
							tileMatrix.TileWidth = atoi(matrix.child("tilewidth").value().c_str());
							tileMatrix.TileHeight = atoi(matrix.child("tileheight").value().c_str());
							tileMatrix.MatrixWidth = atoi(matrix.child("matrixwidth").value().c_str());
							tileMatrix.MatrixHeight = atoi(matrix.child("matrixheight").value().c_str());
							tileMatrixset.TileMatrixs.push_back(tileMatrix);
						}				 
					}

					mTileMatrixSet.push_back(tileMatrixset);
				}
			}
			capabilities->setLayer(mLayer);
			capabilities->setMatrixSet(mTileMatrixSet);

			return capabilities.release();
		}

		WMTSCapabilitiesReader::WMTSCapabilitiesReader()
		{
		}

		WMTSCapabilitiesReader::WMTSCapabilitiesReader(const WMTSCapabilitiesReader &cr)
		{
		}

        /** dtor */
        WMTSCapabilitiesReader::~WMTSCapabilitiesReader() 
		{ 
		}

		GeoGlobeTileCapabilities* GeoGlobeTileCapabilitiesReader::read( const std::string &location, const osgDB::ReaderWriter::Options *options)
		{
			GeoGlobeTileCapabilities *caps = NULL;
			if ( osgDB::containsServerAddress( location ) )
			{
				ReadResult rr = URI(location).readString( options );
				if ( rr.succeeded() )
				{
					std::istringstream in( rr.getString() );
					caps = read( in );
				}
			}
			else
			{
				if ((osgDB::fileExists(location)) && (osgDB::fileType(location) == osgDB::REGULAR_FILE))
				{
					std::ifstream in( location.c_str() );
					caps = read( in );
				}
			}

			return caps;
		}

		GeoGlobeTileCapabilities* GeoGlobeTileCapabilitiesReader::read(const std::string &url, const std::string &location, const osgDB::ReaderWriter::Options* options)
		{
			GeoGlobeTileCapabilities *caps = NULL;
			if (osgDB::fileExists(location))
			{
				std::ifstream in(location.c_str(),std::ios::binary);
				caps = read( in );
			}
			else
			{
				ReadResult rr = URI(url).readString( options );
				if ( rr.succeeded() )
				{
					std::string buf = rr.getString();
					std::istringstream in(buf);
					caps = read( in );

					osgDB::makeDirectoryForFile(location);
					std::ofstream fout;
					fout.open(location.c_str(), std::ios::out | std::ios::binary);        
					fout.write(buf.c_str(), buf.size());        
					fout.close();
				}
			}
			return caps;
		}

		GeoGlobeTileCapabilities* GeoGlobeTileCapabilitiesReader::read( std::istream &in )
		{
			Config doc;
			doc.fromXML( in );
			if (doc.empty() || doc.children().empty())
			{
				OE_NOTICE << "Failed to load Capabilities " << std::endl;
				return 0;
			}
			osg::ref_ptr<GeoGlobeTileCapabilities> capabilities = new GeoGlobeTileCapabilities;
			capabilities->setName(doc.child("getcapabilitiesresponse").child("servicecapabilities").child("name").value());
			capabilities->setVersion(doc.child("getcapabilitiesresponse").child("servicecapabilities").child("version").value());
			/*capabilities.setName(doc.child("GetCapabilitiesResponse").child("name").value());
			capabilities.setVersion(doc.child("GetCapabilitiesResponse").child("version").value())*/

			const Config capdata=doc.child("getcapabilitiesresponse").child("servicecapabilities").child("data");
			std::vector<TileMetaData> tdatas;
			for (ConfigSet::const_iterator i=capdata.children().begin(); i != capdata.children().end(); ++i)
			{
				const osgEarth::Config& cftiledata=*i;
				if(cftiledata.key()=="tiledata")
				{
					TileMetaData tdata;
					tdata.tile=cftiledata.child("tile").value();
					tdata.crs=cftiledata.child("crs").value();

					tdata.bbox.west=atof(cftiledata.child("boundbox").child("minx").value().c_str());
					tdata.bbox.east=atof(cftiledata.child("boundbox").child("maxx").value().c_str());
					tdata.bbox.south=atof(cftiledata.child("boundbox").child("miny").value().c_str());
					tdata.bbox.north=atof(cftiledata.child("boundbox").child("maxy").value().c_str());
					
					tdata.LevelZeroTileSizeX=atof(cftiledata.child("levelzerotilesizex").value().c_str());
					tdata.LevelZeroTileSizeY=atof(cftiledata.child("levelzerotilesizey").value().c_str());

					tdata.toplevel=atoi(cftiledata.child("toplevel").value().c_str());
					tdata.bottomlevel=atoi(cftiledata.child("bottomlevel").value().c_str());

					tdata.tilepixelsX=atoi(cftiledata.child("tilepixelsx").value().c_str());
					tdata.tilepixelsY=atoi(cftiledata.child("tilepixelsy").value().c_str());

					if (cftiledata.hasChild("demdatatype"))
					{
						tdata.DEMDataType=cftiledata.child("demdatatype").value();
					}

					tdatas.push_back(tdata);
				}
			}

			capabilities->setTileMetaData(tdatas);
			return capabilities.release();
		}

} } // namespace osgEarth::Util

