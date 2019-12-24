#include "vector/FeatureSourceGeoWFS.h"
#include <osgEarth/XmlUtils>
#include <osgEarthFeatures/FeatureCursor>

namespace GeoGlobe
{
	namespace GeoVector
	{
				//构造函数，传入参数GEOWFSFeatureOptions，矢量抬高高度与透明度
		GEOWFSFeatureSource::GEOWFSFeatureSource(const GEOWFSFeatureOptions& options ) :
			FeatureSource( options ),
			_options     ( options )
		{      
			m_OriginSRS = "";
			_xmlPath = _options.cachePath().get() + "/" +_options.typeName().get() + ".xml";
			if ( _options.transparent().isSet())
			{
				_transparent = _options.transparent().value();
			}
			else
			{
				_transparent = 1.0;
			}
			if (_options.height().isSet())
			{
				_geoHeight = _options.height().value();
			}
			else
			{
				_geoHeight=0.0;
			}
		}

		/** Destruct the object, cleaning up and OGR handles. */
		GEOWFSFeatureSource::~GEOWFSFeatureSource()
		{               
		}

		//FeatureSource的初始化类，在加载数据之前调用
		Status GEOWFSFeatureSource::initialize(const osgDB::Options* readOptions)
		{
			// initialize the base class
			//FeatureSource::initialize(readOptions);

			// store a reference to the read options so we can pass them along to
			// later requests.
			_dbOptions = readOptions;

			// parse the WFS capabilities URL
			std::string capUrl;
			if (_options.url().isSet())
			{
				char sep = _options.url()->full().find_first_of('?') == std::string::npos ? '?' : '&';

				capUrl =
					_options.url()->full() +
					sep +
					"SERVICE=WFS&VERSION=1.0.0&REQUEST=GetCapabilities";
			}

			// read the WFS capabilities:
			_capabilities = GEOWFSCapabilitiesReader::read(capUrl, _dbOptions.get());
			if (!_capabilities.valid())
			{
				return Status::Error(Status::ResourceUnavailable, Stringify() << "Failed to read WFS GetCapabilities from \"" << capUrl << "\"");
			}
			else
			{
				OE_INFO << "[osgEarth::WFS] Got capabilities from " << capUrl << std::endl;
			}

			// establish a feature profile
			FeatureProfile* fp = 0L;

			//Find the feature type by name
			osg::ref_ptr<GEOWFSFeatureType > featureType = _capabilities->getFeatureTypeByName(_options.typeName().get());
			if (featureType.valid())
			{
				m_OriginSRS = featureType->getOriginSRS();

				if (featureType->getExtent().isValid())
				{
					fp = new FeatureProfile(featureType->getExtent());

					bool disableTiling = _options.disableTiling().isSetTo(true);

					if (featureType->getTiled() && !disableTiling)
					{
						fp->setTiled(true);
						fp->setFirstLevel(featureType->getFirstLevel());
						fp->setMaxLevel(featureType->getMaxLevel());
						fp->setProfile(osgEarth::Profile::create(
							osgEarth::SpatialReference::create("epsg:4326"),
							featureType->getExtent().xMin(), featureType->getExtent().yMin(),
							featureType->getExtent().xMax(), featureType->getExtent().yMax(),
							1, 1));
					}
				}
			}

			// if nothing else, fall back on a global geodetic feature profile.
			if (!fp)
			{
				fp = new FeatureProfile(GeoExtent(SpatialReference::create("epsg:4326"), -180, -90, 180, 90));
			}

			if (_options.geoInterp().isSet())
			{
				fp->geoInterp() = _options.geoInterp().get();
			}

			_featureProfile = fp;
			setFeatureProfile(fp);

			return Status::OK();
		}

		//创建要素的概括类，包括其数据范围，空间坐标系等
		const FeatureProfile* GEOWFSFeatureSource::createFeatureProfile()
		{
			if ( !_featureProfile.valid() )
			{
				static Threading::Mutex s_mutex;
				Threading::ScopedMutexLock lock(s_mutex);
            
				if ( !_featureProfile.valid() )
				{
					FeatureProfile* result = 0L;

					if (_options._dataExtent.isValid())
					{
							result = new FeatureProfile(_options._dataExtent);
					}
					//if (_capabilities.valid())
					//{
					//    //Find the feature type by name
					//    osg::ref_ptr< GEOWFSFeatureType > featureType = _capabilities->getFeatureTypeByName( _options.typeName().get() );
					//    if (featureType.valid())
					//    {
					//        if (featureType->getExtent().isValid())
					//        {
							
					//            result = new FeatureProfile(featureType->getExtent());

					//            bool disableTiling = _options.disableTiling().isSet() && *_options.disableTiling();

					//            if (featureType->getTiled() && !disableTiling)
					//            {                        
					//                result->setTiled( true );
					//                result->setFirstLevel( featureType->getFirstLevel() );
					//                result->setMaxLevel( featureType->getMaxLevel() );
					//                result->setProfile( osgEarth::Profile::create(osgEarth::SpatialReference::create("epsg:4326"), featureType->getExtent().xMin(), featureType->getExtent().yMin(), featureType->getExtent().xMax(), featureType->getExtent().yMax(), 1, 1) );
					//            }
					//        }
					//    }
					//}

					if (!result)
					{
						result = new FeatureProfile(GeoExtent(SpatialReference::create( "epsg:4326" ), -180, -90, 180, 90));
					}
                
					_featureProfile = result;
				}
			}

			return _featureProfile.get();
		}

		FeatureProfile* GEOWFSFeatureSource::getFeatureProfile()
		{
			if ( !_featureProfile.valid() )
			{
				createFeatureProfile();
			}
			return _featureProfile.get();
		}

		bool GEOWFSFeatureSource::getFeatures( const std::string& buffer,FeatureList& features )
		{
			GEO_SCOPED_LOCK;    
			std::stringstream buf(buffer);

			osg::ref_ptr<osgEarth::XmlDocument> doc = osgEarth::XmlDocument::load(buf);
			osg::ref_ptr<osgEarth::XmlElement> e_root = static_cast<osgEarth::XmlElement*>(doc->getChildren()[0].get());

			GEOWFSFeatureClass* geoFeaClass = new GEOWFSFeatureClass(e_root);
			geoFeaClass->m_vecOrigin = osg::Vec3d(_options.OriginX().get(),_options.OriginY().get(),_options.OriginZ().get());
			geoFeaClass->m_OriginSRS = m_OriginSRS;
			geoFeaClass->createFeaturesfromGML(_featureProfile->getSRS(),features);
			return true;
		}

		bool GEOWFSFeatureSource::getFeatures( const std::string& buffer,FeatureList& features,float height)
		{
			GEO_SCOPED_LOCK;    
			std::stringstream buf(buffer);

			osg::ref_ptr<XmlDocument> doc = XmlDocument::load(buf);
			osg::ref_ptr<XmlElement> e_root = static_cast<XmlElement*>(doc->getChildren()[0].get());

			GEOWFSFeatureClass* geoFeaClass = new GEOWFSFeatureClass(e_root);
			geoFeaClass->m_vecOrigin = osg::Vec3d(_options.OriginX().get(), _options.OriginY().get(), _options.OriginZ().get());
			geoFeaClass->m_OriginSRS = m_OriginSRS;
			geoFeaClass->createFeaturesfromGML(_featureProfile->getSRS(),height,features);
			return true;
		}

		//根据要素图层的名称获取请求要素数据的url
		std::string GEOWFSFeatureSource::createAllURL()
		{
			std::stringstream buf;
			buf << _options.url()->full() << "?SERVICE=WFS&VERSION=1.0.0&REQUEST=GetFeature";
			buf << "&TYPENAME=" << _options.typeName().get();
			return buf.str();
		}
		//根据一定的经纬度范围获取要素的url
		std::string GEOWFSFeatureSource::createBoundURL(osgEarth::Bounds bds)
		{
			std::stringstream buf;
			buf << _options.url()->full() << "?SERVICE=WFS&VERSION=1.0.0&REQUEST=GetFeature";
			buf << "&TYPENAME=" << _options.typeName().get();

			buf << "&BBOX=" << std::setprecision(16)
				<< bds.xMin()<< ","
				<< bds.yMin()<< ","
				<< bds.xMax()<< ","
				<< bds.yMax();
			return buf.str();
		}
		//根据要素的oid获取要素的url
		std::string GEOWFSFeatureSource::createOidURL(int oid)
		{
			std::stringstream buf;
			buf << _options.url()->full() << "?VERSION=1.0.0&REQUEST=GetFeature";
			buf << "&FeatureID=" << _options.typeName().get()<<"."<<oid;

			return buf.str();
		}
		//根据请求保存文件
		void GEOWFSFeatureSource::saveResponse(const std::string buffer, const std::string& filename)
		{
			if (!osgDB::fileExists(osgDB::getFilePath(filename)) )
				osgDB::makeDirectoryForFile(filename);

			std::ofstream fout;
			fout.open(filename.c_str(), std::ios::out | std::ios::binary);
			fout.write(buffer.c_str(), buffer.size());
			fout.close();
		}
		bool GEOWFSFeatureSource::getFeatures(std::istream& buffer,FeatureList& features )
		{
			//GEO_SCOPED_LOCK;    
			osg::ref_ptr<XmlDocument> doc = XmlDocument::load(buffer);


			if(NULL==doc)
				return false;

			osg::ref_ptr<XmlElement> e_root = static_cast<XmlElement*>(doc->getChildren()[0].get());

			GEOWFSFeatureClass* geoFeaClass = new GEOWFSFeatureClass(e_root);
			geoFeaClass->m_vecOrigin = osg::Vec3d(_options.OriginX().get(), _options.OriginY().get(), _options.OriginZ().get());
			geoFeaClass->m_OriginSRS = m_OriginSRS;
			geoFeaClass->createFeaturesfromGML(_featureProfile->getSRS(),features);
			return true;
		}

		bool GEOWFSFeatureSource::getFeatures(std::istream& buffer,FeatureList& features ,float height)
		{
			//GEO_SCOPED_LOCK;    
			osg::ref_ptr<XmlDocument> doc = XmlDocument::load(buffer);

			if(NULL==doc)
				return false;

			osg::ref_ptr<XmlElement> e_root = static_cast<XmlElement*>(doc->getChildren()[0].get());

			GEOWFSFeatureClass* geoFeaClass = new GEOWFSFeatureClass(e_root);
			geoFeaClass->m_vecOrigin = osg::Vec3d(_options.OriginX().get(), _options.OriginY().get(), _options.OriginZ().get());
			geoFeaClass->m_OriginSRS = m_OriginSRS;
			geoFeaClass->createFeaturesfromGML(_featureProfile->getSRS(),height,features);
			return true;
		}

		bool GEOWFSFeatureSource::getFeatures(std::istream& buffer, FeatureList& features, float transparent, float height)
		{
			GEO_SCOPED_LOCK;    
			osg::ref_ptr<XmlDocument> doc = XmlDocument::load(buffer);

			if(NULL==doc)
				return false;

			osg::ref_ptr<XmlElement> e_root = static_cast<XmlElement*>(doc->getChildren()[0].get());

			GEOWFSFeatureClass* geoFeaClass = new GEOWFSFeatureClass(e_root);
			geoFeaClass->m_vecOrigin = osg::Vec3d(_options.OriginX().get(), _options.OriginY().get(), _options.OriginZ().get());
			geoFeaClass->m_OriginSRS = m_OriginSRS;
			geoFeaClass->m_Color = osg::Vec4d(_options.ColorR().get()/255.0, _options.ColorG().get() / 255.0, _options.ColorB().get() / 255.0, _options.ColorA().get() / 255.0);
			geoFeaClass->m_nsize = _options.Size().get();
			geoFeaClass->m_strFontFile = _options.FontFile().get();
			geoFeaClass->m_shadowColor = osg::Vec4d(_options.ShaowColorR().get() / 255.0, _options.ShaowColorG().get() / 255.0, _options.ShaowColorB().get() / 255.0, _options.ShaowColorA().get() / 255.0);
			geoFeaClass->m_eShadowPos = _options.ShadowPos().get();
			geoFeaClass->m_dbOffset = _options.ShaowOffset().get();
			geoFeaClass->createFeaturesfromGML(_featureProfile->getSRS(),features,transparent,height);
			return true;
		}

		//直接创建对Feature的读取类，将xml文件中的所有要素读入内存中
		FeatureCursor* GEOWFSFeatureSource::createFeatureCursor( const Symbology::Query& query, ProgressCallback* progress)
		{
			FeatureCursor* result = 0L;

			if (!osgDB::fileExists(_xmlPath))
			{
				std::string url = createAllURL();        

				// check the blacklist:
				if ( Registry::instance()->isBlacklisted(url) )
					return 0L;

				std::string strUrlNew = CodeHelp::String_To_UTF8(url);

				URI uri(strUrlNew);

				// read the data:
				ReadResult r = uri.readString( _dbOptions.get() );

				const std::string& buf = r.getString();

				saveResponse(buf,_xmlPath);
			}
				
			std::ifstream fin(_xmlPath.c_str(),ios::binary);
			bool dataOK = false;
			FeatureList features;
			if (!fin.eof())
			{
				dataOK = getFeatures( fin,features,_transparent,_geoHeight);
			}
			fin.close();
			if ( dataOK )
			{
				OE_DEBUG << LC << "Read " << features.size() << " features" << std::endl;
			}

			//If we have any filters, process them here before the cursor is created
			if (getFilters() && !getFilters()->empty() && !features.empty())
			{
				// preprocess the features using the filter list:
				if (features.size() > 0)
				{
					FilterContext cx;
					cx.setProfile(getFeatureProfile());

					for (FeatureFilterChain::const_iterator i = getFilters()->begin(); i != getFilters()->end(); ++i)
					{
						FeatureFilter* filter = i->get();
						cx = filter->push(features, cx);
					}
				}
			}

			// If we have any features and we have an fid attribute, override the fid of the features
			if (_options.fidAttribute().isSet())
			{
				for (FeatureList::iterator itr = features.begin(); itr != features.end(); ++itr)
				{
					std::string attr = itr->get()->getString(_options.fidAttribute().get());
					FeatureID fid = as<long>(attr, 0);
					itr->get()->setFID(fid);
				}
			}

			//result = new FeatureListCursor(features);
			result = dataOK ? new FeatureListCursor( features ) : 0L;

			if ( !result )
				Registry::instance()->blacklist(_xmlPath);

			return result;
		}

		/**
		* Gets the Feature with the given FID
		* @returns
		*     The Feature with the given FID or NULL if not found.
		*/
		Feature* GEOWFSFeatureSource::getFeature( FeatureID fid )
		{
			return 0;
		}

		bool GEOWFSFeatureSource::isWritable() const
		{
			return false;
		}

		const FeatureSchema&  GEOWFSFeatureSource::getSchema() const
		{
			//TODO:  Populate the schema from the DescribeFeatureType call
			return _schema;
		}

		osgEarth::Symbology::Geometry::Type GEOWFSFeatureSource::getGeometryType() const
		{
			return osgEarth::Symbology::Geometry::TYPE_UNKNOWN;
		}



		//设置读取WFS服务的吉奥要素类的驱动器，由osgEarth调用加载要素类
		class WFSFeatureSourceFactory : public FeatureSourceDriver
		{
		public:
			WFSFeatureSourceFactory()
			{
				supportsExtension( "osgearth_feature_geowfs", "geoWFS feature driver for osgEarth" );
			}

			virtual const char* className()
			{
				return "geoWFS Feature Reader";
			}

			virtual ReadResult readObject(const std::string& file_name, const Options* options) const
			{
				if ( !acceptsExtension(osgDB::getLowerCaseFileExtension( file_name )))
					return ReadResult::FILE_NOT_HANDLED;

				return ReadResult( new GEOWFSFeatureSource( getFeatureSourceOptions(options) ) );
			}
		};

		REGISTER_OSGPLUGIN(osgearth_feature_geowfs, WFSFeatureSourceFactory)
	}
}