
#include "GeoWFS.h"
#include <osgEarth/URI>
#include <osgEarth/XmlUtils>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>

namespace GeoGlobe {
	namespace GeoVector
	{

	GEOWFSFeatureType::GEOWFSFeatureType():
		_tiled(false),
		_maxLevel(0),
		_firstLevel(0)
	{
		//_maxLevel = 20;
		//_firstLevel = 0;
	}

	GEOWFSFeatureType* GEOWFSCapabilities::getFeatureTypeByName(const std::string& name)
	{
		for (GEOFeatureTypeList::iterator itr = _featureTypes.begin(); itr != _featureTypes.end(); ++itr)
		{
			if (osgDB::equalCaseInsensitive(itr->get()->getName(), name)) return itr->get();
		}
		return NULL;
	}

    //从服务器或者本地文件中读取
    GEOWFSCapabilities* GEOWFSCapabilitiesReader::read( const osgEarth::URI& location,std::string filename,const osgDB::Options* dbOptions)
	{
		GEOWFSCapabilities *caps = NULL;
		if (osgDB::fileExists(filename))
		{
			std::ifstream fin(filename.c_str());
			caps = read(fin);
		}
		else if( osgDB::containsServerAddress( location.full()))
		{
			osgEarth::ReadResult rr = osgEarth::URI(location).readString();
			if (rr.succeeded())
			{
				if (!osgDB::fileExists(osgDB::getFilePath(filename)) )
					osgDB::makeDirectoryForFile(filename);

				//若文件不存在则将该文件缓存
				const std::string& buffer = rr.getString();
				std::ofstream fout;
				fout.open(filename.c_str(),std::ios::out|std::ios::binary);
				fout.write(buffer.c_str(),buffer.size());
				fout.close();
				std::istringstream in( buffer);
				caps = read(in);
			}
		}
		return caps;
	}

	GEOWFSCapabilities* GEOWFSCapabilitiesReader::read(const osgEarth::URI& location, const osgDB::Options* dbOptions)
	{
		GEOWFSCapabilities *caps = NULL;
		if (osgDB::containsServerAddress(location.full()))
		{
			osgEarth::ReadResult rr = osgEarth::URI(location).readString();
			if (rr.succeeded())
			{
				std::istringstream in(rr.getString());
				caps = read(in);
			}
		}
		return caps;
	}

    //以流的方式读取GEOWFSCapabilities
    GEOWFSCapabilities* GEOWFSCapabilitiesReader::read( std::istream &in )
	{
		osg::ref_ptr<osgEarth::XmlDocument> doc = osgEarth::XmlDocument::load( in );
		if (!doc.valid() || doc->getChildren().empty())
		{
			OE_NOTICE << "Failed to load Capabilities " << std::endl;
			return 0;
		}
		osg::ref_ptr<GEOWFSCapabilities> capabilities = new GEOWFSCapabilities;
		//Get the Capabilities version
		osg::ref_ptr<osgEarth::XmlElement> e_root = static_cast<osgEarth::XmlElement*>(doc->getChildren()[0].get());
		capabilities->setVersion( e_root->getAttr(ATTR_VERSION ) );

		osg::ref_ptr<osgEarth::XmlElement> e_service = e_root->getSubElement( ELEM_SERVICE );
		if (!e_service.valid())
		{
			OE_NOTICE << "Could not find Service element" << std::endl;
			return 0;
		}

		//Read the parameters from the Service block
		capabilities->setName( e_service->getSubElementText(ELEM_NAME ) );
		capabilities->setAbstract( e_service->getSubElementText( ELEM_ABSTRACT ) );
		capabilities->setTitle( e_service->getSubElementText( ELEM_TITLE ) );    

		//Read all the feature types    
		osg::ref_ptr<osgEarth::XmlElement> e_feature_types = e_root->getSubElement( ELEM_FEATURETYPELIST );
		if (e_feature_types.valid())
		{
			osgEarth::XmlNodeList featureTypes = e_feature_types->getSubElements( ELEM_FEATURETYPE );
			for(osgEarth::XmlNodeList::const_iterator itr = featureTypes.begin(); itr != featureTypes.end(); itr++ )
			{
				osgEarth::XmlElement* e_featureType = static_cast<osgEarth::XmlElement*>( itr->get() );
				GEOWFSFeatureType* featureType = new GEOWFSFeatureType();
				featureType->setName( e_featureType->getSubElementText( ELEM_NAME ) );
				featureType->setTitle( e_featureType->getSubElementText( ELEM_TITLE ) );
				featureType->setOriginSRS(e_featureType->getSubElementText(ELEM_SRS));
				featureType->setAbstract( e_featureType->getSubElementText( ELEM_ABSTRACT ) );

				//NOTE:  TILED and MAXLEVEL aren't part of the WFS spec, these are enhancements to our server for tiled WFS access
				std::string tiledStr = e_featureType->getSubElementText(ELEM_TILED);
				if (tiledStr.compare("") != 0)
				{
					featureType->setTiled(osgEarth::as<bool>(tiledStr, false) );
				}

				std::string maxLevelStr = e_featureType->getSubElementText(ELEM_MAXLEVEL);
				if (maxLevelStr.compare("") != 0)
				{
					featureType->setMaxLevel(osgEarth::as<int>(maxLevelStr, -1));
				}

				std::string firstLevelStr = e_featureType->getSubElementText(ELEM_FIRSTLEVEL);
				if (firstLevelStr.compare("") != 0)
				{
					featureType->setFirstLevel(osgEarth::as<int>(firstLevelStr, 0));
				}

				// Read the SRS            
				std::string srsText = e_featureType->getSubElementText(ELEM_SRS);
				if (srsText.compare("") != 0)
				{
					srsText = "wgs84";
					featureType->setSRS( srsText ); 
				}

				osg::ref_ptr<osgEarth::XmlElement> e_bb = e_featureType->getSubElement( ELEM_LATLONGBOUNDINGBOX );
				if (e_bb.valid())
				{
					double minX, minY, maxX, maxY;
					minX = osgEarth::as<double>(e_bb->getAttr( ATTR_MINX ), 0);
					minY = osgEarth::as<double>(e_bb->getAttr( ATTR_MINY ), 0);
					maxX = osgEarth::as<double>(e_bb->getAttr( ATTR_MAXX ), 0);
					maxY = osgEarth::as<double>(e_bb->getAttr( ATTR_MAXY ), 0);
					featureType->setExtent(osgEarth::GeoExtent( osgEarth::SpatialReference::create( srsText ), minX, minY, maxX, maxY) );
				}                       

				capabilities->getFeatureTypes().push_back( featureType );
			}        
		}

		return capabilities.release();
	}
	} 
} // namespace GeoGlobe:GeoVector