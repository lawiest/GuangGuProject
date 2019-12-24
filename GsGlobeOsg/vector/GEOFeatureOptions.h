#ifndef OSGEARTH_DRIVER_GEO_FEATURE_SOURCE_OPTIONS
#define OSGEARTH_DRIVER_GEO_FEATURE_SOURCE_OPTIONS 1

#include<osgEarth/Common>
#include<osgEarthFeatures/FeatureSource>
#include <osgEarthFeatures/FeatureDisplayLayout>
#include <osgEarthSymbology/Style>
#include <osgEarthSymbology/StyleSheet>
#include <osgEarthSymbology/StyleSelector>
#include "sqlite3.h"
#include "Geo_Geometry_Common.h"
namespace GeoGlobe {
	namespace GeoVector 
	{
		
		using namespace osgEarth::Features;
		using namespace osgEarth::Symbology;

		//吉奥矢量要素参数类
		class GEOFeatureOptions : public FeatureSourceOptions // NO EXPORT; header only
		{
		public:
			osgEarth::optional<osgEarth::URI>& url() { return _url; }
			const osgEarth::optional<osgEarth::URI>& url() const { return _url; }

			osgEarth::optional<std::string>& connection() { return _connection; }
			const osgEarth::optional<std::string>& connection() const { return _connection; }

			osgEarth::optional<std::string>& geoDriver() { return _geoDriver; }
			const osgEarth::optional<std::string>& geoDriver() const { return _geoDriver; }

			osgEarth::optional<bool>& buildSpatialIndex() { return _buildSpatialIndex; }
			const osgEarth::optional<bool>& buildSpatialIndex() const { return _buildSpatialIndex; }

			osgEarth::optional<bool>& forceRebuildSpatialIndex() { return _forceRebuildSpatialIndex; }
			const osgEarth::optional<bool>& forceRebuildSpatialIndex() const { return _forceRebuildSpatialIndex; }

			optional<Config>& geometryConfig() { return _geometryConf; }
			const optional<Config>& geometryConfig() const { return _geometryConf; }

			osgEarth::optional<std::string>& geometryUrl()
			{
				return _geometryUrl;
			}

			const osgEarth::optional<std::string>& geometryUrl() const
			{ 
				return _geometryUrl;
			}

			osgEarth::optional<std::string>& layer() { return _layer; }
			const osgEarth::optional<std::string>& layer() const { return _layer; }

			// does not serialize
			osg::ref_ptr<Symbology::Geometry>& geometry() 
			{ 
				return _geometry;
			}

			const osg::ref_ptr<Symbology::Geometry>& geometry() const 
			{ 
				return _geometry; 
			}

			optional<float>& height() {return _geoHeight;}
			const optional<float>& height()const {return _geoHeight;}



		public:
			GEOFeatureOptions( const ConfigOptions& opt =ConfigOptions() ) : 
			FeatureSourceOptions( opt ) ,
			_geoHeight(0.0)
			{
				setDriver( "geoStar" );
				fromConfig( _conf );

				m_Xmin = -180;
				m_Xmax = 190;
				m_Ymin = -90;
				m_Ymax = 90;
			}

			virtual ~GEOFeatureOptions() { }

		public:
			Config getConfig() const {
				Config conf = FeatureSourceOptions::getConfig();
				conf.set( "url", _url );
				conf.set( "connection", _connection );
				conf.set( "geo_driver", _geoDriver );
				conf.set( "build_spatial_index", _buildSpatialIndex );
				conf.set( "force_rebuild_spatial_index", _forceRebuildSpatialIndex );
				conf.set( "geometry", _geometryConf );
				conf.set( "geometry_url", _geometryUrl );
				conf.set( "layer", _layer );
				conf.set( "height", _geoHeight );
				conf.setNonSerializable( "GEOFeatureOptions::geometry", _geometry.get() );
				return conf;
			}

		protected:
			void mergeConfig( const osgEarth::Config& conf ) {
				FeatureSourceOptions::mergeConfig( conf );
				fromConfig( conf );
			}

		private:
			void fromConfig( const osgEarth::Config& conf ) {
				conf.get( "url", _url );
				conf.get( "connection", _connection );
				conf.get( "geo_driver", _geoDriver );
				conf.get( "build_spatial_index", _buildSpatialIndex );
				conf.get( "force_rebuild_spatial_index", _forceRebuildSpatialIndex );
				conf.get( "geometry", _geometryConf );
				conf.get( "geometry_url", _geometryUrl );
				conf.get( "layer", _layer);
				conf.get( "height", _geoHeight);
				_geometry = conf.getNonSerializable<Symbology::Geometry>( "GEOFeatureOptions::geometry" );
			}

			osgEarth::optional<osgEarth::URI>                     _url;

			optional<float>                   _geoHeight;

			osgEarth::optional<std::string>             _connection;
			osgEarth::optional<std::string>             _geoDriver;
			optional<bool>                    _buildSpatialIndex;
			optional<bool>                    _forceRebuildSpatialIndex;
			optional<Config>                  _geometryConf;
			optional<Config>                  _geometryProfileConf;
			osgEarth::optional<std::string>             _geometryUrl;
			osgEarth::optional<std::string>             _layer;
			
			
			osg::ref_ptr<Symbology::Geometry> _geometry;
			

			//////////////////////////////////////根据URL创建layout与style，传给FeatureGeomModelOptions
		public:
			osg::Vec2d                        _center;
			double m_Xmin, m_Xmax, m_Ymin, m_Ymax;
		public:
			//获取要素图层的中心点
			bool getCenter(std::string dbPath)
			{
				double xmin,xmax,ymin,ymax;
				sqlite3* m_pSqliteDB;
				sqlite3_open(dbPath.c_str(),&m_pSqliteDB);
				if (! m_pSqliteDB) return false;

				sqlite3_stmt* stmt;

				std::string sql;

				sql="select value from metatable where name='XMin'";
				if (SQLITE_OK==sqlite3_prepare(m_pSqliteDB,sql.c_str(),-1,&stmt,0))
				{
					while(sqlite3_step(stmt)==SQLITE_ROW)
					{
						xmin=sqlite3_column_double(stmt,0);
					}
				}

				sql="select value from metatable where name='XMax'";
				if (SQLITE_OK==sqlite3_prepare(m_pSqliteDB,sql.c_str(),-1,&stmt,0))
				{
					while(sqlite3_step(stmt)==SQLITE_ROW)
					{
						xmax=sqlite3_column_double(stmt,0);
					}
				}

				sql="select value from metatable where name='YMin'";
				if (SQLITE_OK==sqlite3_prepare(m_pSqliteDB,sql.c_str(),-1,&stmt,0))
				{
					while(sqlite3_step(stmt)==SQLITE_ROW)
					{
						ymin=sqlite3_column_double(stmt,0);
					}
				}

				sql="select value from metatable where name='YMax'";
				if (SQLITE_OK==sqlite3_prepare(m_pSqliteDB,sql.c_str(),-1,&stmt,0))
				{
					while(sqlite3_step(stmt)==SQLITE_ROW)
					{
						ymax=sqlite3_column_double(stmt,0);
					}
				}

				osg::ref_ptr<SpatialReference> srs =SpatialReference::get("wgs84");
			 osgEarth::GeoExtent extent(srs.get(),xmin,ymin,xmax,ymax);
				//get radius from ecef,what is ecef?
				extent.getCentroid( _center.x(), _center.y() );
			}

			//获取要素图层的要素个数、类型，以及对应的经纬度范围的半径
			bool getSumandRadius(std::string dbPath,int& sum,double& radius,int& type)
			{
				sqlite3* m_pSqliteDB;

				sqlite3_open(dbPath.c_str(),&m_pSqliteDB);

				if (! m_pSqliteDB)
					return false;

				sqlite3_stmt* stmt;

				std::string sql = "select count(*) from datatable";

				if (SQLITE_OK==sqlite3_prepare(m_pSqliteDB,sql.c_str(),-1,&stmt,0))
				{
					while(sqlite3_step(stmt)==SQLITE_ROW)
					{
						sum=sqlite3_column_int(stmt,0);
					}
				}

				sql="select value from metatable where name='GeometryType'";
				if (SQLITE_OK==sqlite3_prepare(m_pSqliteDB,sql.c_str(),-1,&stmt,0))
				{
					while(sqlite3_step(stmt)==SQLITE_ROW)
					{
						type=sqlite3_column_int(stmt,0);
					}
				}

				sql="select value from metatable where name='XMin'";
				if (SQLITE_OK==sqlite3_prepare(m_pSqliteDB,sql.c_str(),-1,&stmt,0))
				{
					while(sqlite3_step(stmt)==SQLITE_ROW)
					{
						m_Xmin=sqlite3_column_double(stmt,0);
					}
				}

				sql="select value from metatable where name='XMax'";
				if (SQLITE_OK==sqlite3_prepare(m_pSqliteDB,sql.c_str(),-1,&stmt,0))
				{
					while(sqlite3_step(stmt)==SQLITE_ROW)
					{
						m_Xmax=sqlite3_column_double(stmt,0);
					}
				}

				sql="select value from metatable where name='YMin'";
				if (SQLITE_OK==sqlite3_prepare(m_pSqliteDB,sql.c_str(),-1,&stmt,0))
				{
					while(sqlite3_step(stmt)==SQLITE_ROW)
					{
					   m_Ymin=sqlite3_column_double(stmt,0);
					}
				}

				sql="select value from metatable where name='YMax'";
				if (SQLITE_OK==sqlite3_prepare(m_pSqliteDB,sql.c_str(),-1,&stmt,0))
				{
					while(sqlite3_step(stmt)==SQLITE_ROW)
					{
						m_Ymax=sqlite3_column_double(stmt,0);
					}
				}

				osg::ref_ptr<SpatialReference> srs =SpatialReference::get("wgs84");
			 osgEarth::GeoExtent extent(srs.get(),m_Xmin,m_Ymin,m_Xmax,m_Ymax);
				//get radius from ecef,what is ecef?
				osg::Vec3d center, corner;
				extent.getCentroid( center.x(), center.y() );
				corner.x()=m_Xmin;
				corner.y()=m_Ymin;
				const SpatialReference* ecefSRS = extent.getSRS()->getECEF();
				extent.getSRS()->transform( center, ecefSRS, center );
				extent.getSRS()->transform( corner, ecefSRS, corner );
				radius=(center-corner).length();
				/*radius=extent.bounds().radius();*///this way is wrong
			   return true;

			}

			//对矢量要素进行分层处理，要素的特征style设为默认值
			bool createLayoutandStyle(osgEarth::Features::FeatureDisplayLayout& layout,osgEarth::Symbology::StyleSheet* stys)
			{
				layout.tileSizeFactor()=10.0;
				layout.maxRange()=1e10;

				//the param need to create the layout
				/*levels,featurecount(sum),the dataset extent(radius),the levelRange for every level,selector and its select sql*/
				int sum,type;
				double radius;
				
				getSumandRadius(_url.get().full(),sum,radius,type);//sum=21429,radius=23082
				GeoGlobe::GeoVector::geoGEOMETRYTYPE geoType=(GeoGlobe::GeoVector::geoGEOMETRYTYPE)type;

				int levels=4;//default is 4
				int interval=1;
				if (sum<250)
				{
					levels=1;
				}
				if (radius>1e6)//radius determine levels
				{
					levels=1;
				}

				double levelRange=radius*layout.tileSizeFactor().get();

				std::string buf;
				StyleSelectorList sselectorlist;
				
				for (int i=0;i<levels;i++)//对每一层要素确定该层的要素成员及其符号特征
				{
					StyleSelector selector;

					std::stringstream nbuf;
					nbuf<<i;
					selector.name()=nbuf.str();

					selector.styleName()="style";

					osgEarth::Symbology::Query query=osgEarth::Symbology::Query();
					std::stringstream temp,ntemp;

					if (levels==1)//if only one layer,get all vectors
					{
						interval=1;
					}
					else
					{
						interval=sum/(pow(4.0,i+4));
						if (interval<1)
						{
							interval=1;
						}
					}
					
					if (i==0)
					{
						temp<<"oid%"<<interval<<"==0";
						query.expression()=temp.str();
						ntemp<<"oid%"<<interval<<"!=0";
						buf=ntemp.str();
					}
					else if(i==levels-1)
					{
						query.expression()=buf;
						
					}
					else
					{
						temp<<"oid%"<<interval<<"==0";
						query.expression()=buf+" and "+temp.str();
						ntemp<<"oid%"<<interval<<"!=0";
						buf=buf+" and "+ntemp.str();
					}
					selector.query()=query;

					if (i==0)
					{
						layout.addLevel(FeatureLevel(0.0,20000000,selector.name()));//make sure the lod0
					}
					else
					{
						if (i==levels-1)
						{
							layout.addLevel(FeatureLevel(0.0,levelRange/pow(2.0,i+1)-100,selector.name()));
						}
						else
						{
							layout.addLevel(FeatureLevel(0.0,levelRange/pow(2.0,i)-100,selector.name()));
						}
					}

					sselectorlist.push_back(selector);
				}//for
				//layout.addLevel(FeatureLevel(0.0,0.10));
				Style style;
				style.setName("style");
				LineSymbol* ls = style.getOrCreateSymbol<LineSymbol>();
				ls->stroke()->color() = Color::Yellow;
				ls->stroke()->width() = 2.0f;
				if (geoType==GeoGlobe::GeoVector::GEO_GEOMETRY_POINT)
				{
					style.getOrCreate<PointSymbol>()->size()=3.0;
					style.getOrCreate<PointSymbol>()->fill()->color()=Color(0.0,0.0,1.0,1.0);
				}
				if (geoType==GeoGlobe::GeoVector::GEO_GEOMETRY_POLYGON)
				{
					//style.getOrCreate<PolygonSymbol>()->fill()->color()=Color(Color::Cyan, 0.5);
				}
				
				//style.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
				//style.getOrCreate<AltitudeSymbol>()->technique() = AltitudeSymbol::TECHNIQUE_DRAPE;
				style.getOrCreate<RenderSymbol>()->depthTest()=false;
				
				stys->addStyle(style);
				stys->selectors()=sselectorlist;

				return true;
			}

			//对矢量要素进行分层处理，要素的特征style使用已设定的值
			bool createLayoutandStyle(osgEarth::Features::FeatureDisplayLayout& layout,osgEarth::Symbology::Style& osgStyle,osgEarth::Symbology::StyleSheet* stys)
			{
				layout.tileSizeFactor()=10.0;
				layout.maxRange()=1e10;

				//the param need to create the layout
				/*levels,featurecount(sum),the dataset extent(radius),the levelRange for every level,selector and its select sql*/
				int sum,type;
				double radius;

				getSumandRadius(_url.get().full(),sum,radius,type);//sum=21429,radius=23082
				GeoGlobe::GeoVector::geoGEOMETRYTYPE geoType=(GeoGlobe::GeoVector::geoGEOMETRYTYPE)type;

				int levels=4;//default is 4
				int interval=1;
				if (sum<250)
				{
					levels=1;
				}
				if (radius>1e6)//radius determine levels
				{
					levels=1;
				}

				double levelRange=radius*layout.tileSizeFactor().get();

				std::string buf;
				StyleSelectorList sselectorlist;

				for (int i=0;i<levels;i++)//create selector and layout
				{
					StyleSelector selector;

					std::stringstream nbuf;
					nbuf<<i;
					selector.name()=nbuf.str();

					selector.styleName()="style";

					osgEarth::Symbology::Query query=osgEarth::Symbology::Query();
					std::stringstream temp,ntemp;

					if (levels==1)//if only one layer,get all vectors
					{
						interval=1;
					}
					else
					{
						interval=sum/(pow(4.0,i+4));
						if (interval<1)
						{
							interval=1;
						}
					}


					if (i==0)
					{
						temp<<"oid%"<<interval<<"==0";
						query.expression()=temp.str();
						ntemp<<"oid%"<<interval<<"!=0";
						buf=ntemp.str();
					}
					else if(i==levels-1)
					{
						query.expression()=buf;
					}
					else
					{
						temp<<"oid%"<<interval<<"==0";
						query.expression()=buf+" and "+temp.str();
						ntemp<<"oid%"<<interval<<"!=0";
						buf=buf+" and "+ntemp.str();
					}
					selector.query()=query;

					if (i==0)
					{
						layout.addLevel(FeatureLevel(0.0,20000000,selector.name()));//make sure the lod0
					}
					else
					{
						if (i==levels-1)
						{
							layout.addLevel(FeatureLevel(0.0,levelRange/pow(2.0,i+1)-100,selector.name()));
						}
						else
						{
							layout.addLevel(FeatureLevel(0.0,levelRange/pow(2.0,i)-100,selector.name()));
						}

					}

					sselectorlist.push_back(selector);
				}//for
				//layout.addLevel(FeatureLevel(0.0,0.10));
				//Style style;
				//style.setName("style");
				//LineSymbol* ls = style.getOrCreateSymbol<LineSymbol>();
				//ls->stroke()->color() = Color::Yellow;
				//ls->stroke()->width() = 2.0f;
				//if (geoType==GeoGlobe::GeoVector::GEO_GEOMETRY_POLYGON)
				//{
				//	style.getOrCreate<PolygonSymbol>()->fill()->color()=Color(Color::Cyan, 0.5);
				//}

				//style.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
				//style.getOrCreate<AltitudeSymbol>()->technique() = AltitudeSymbol::TECHNIQUE_DRAPE;
				////style.getOrCreate<RenderSymbol>()->depthTest()=false;

				//stys->addStyle(style);
				//stys->selectors()=sselectorlist;
				
				osgStyle.setName("style");

				PointSymbol*     point     = osgStyle.get<PointSymbol>();
				LineSymbol*      line      = osgStyle.get<LineSymbol>();
				PolygonSymbol*   polygon   = osgStyle.get<PolygonSymbol>();

				if (polygon)
				{
					if (!line)
					{
						LineSymbol* ls = osgStyle.getOrCreateSymbol<LineSymbol>();
						ls->stroke()->color() = Color(Color::Black,polygon->fill()->color().a());
						ls->stroke()->width() = 1.0f;
					}
				}
				stys->addStyle(osgStyle);
				stys->selectors()=sselectorlist;
				return true;
			}
			//////////////////////////////////////////////////////////////////////////////////////////////////


		};

	}//GeoVector
}//GeoGlobe

#endif//OSGEARTH_DRIVER_GEO_FEATURE_SOURCE_OPTIONS