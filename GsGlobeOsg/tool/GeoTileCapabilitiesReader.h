#ifndef GEOGLOBE_TOOL_GEOTILECAPABILITIESREADER
#define GEOGLOBE_TOOL_GEOTILECAPABILITIESREADER 1

#include <osg/Referenced>
#include <osg/ref_ptr>
#include <osg/Version>
#include <osgDB/ReaderWriter>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#if OSG_MIN_VERSION_REQUIRED(2,9,5)
#include <osgDB/Options>
#endif

#include <osgEarth/XmlUtils>
#include <osgEarth/HTTPClient>

#include <osgEarth/Common>
#include <osgEarthUtil/Common>

#include <string>
#include <vector>



namespace GeoGlobe{
	namespace Tool
	{
		struct BoundBox
		{
			//double minx;//west
			//double miny;//south
			//double maxx;//east
			//double maxy;//north
			double west;
			double south;
			double east;
			double north;
		};

		struct TileData
		{
			std::string layerName;
			std::string tile;
			std::string crs;
			//--------------------//
			BoundBox bbox;//��γ�ȷ�Χ
			double LevelZeroTileSizeX;//������������Ƭ���
			double LevelZeroTileSizeY;//������������Ƭ�߶�
			//-------------------//�Զ�Ϊ��λ
			int toplevel;
			int bottomlevel;

			int tilepixelsX;
			int tilepixelsY;//��Ƭ����

			std::string DEMDataType; //������������

			/*Pyramid pyramid;
			double invalidvalue;
			double invalidcolor;
			double transparency;*/
		};


		class  GeoTileCapabilities : public osg::Referenced
		{
		public:
			GeoTileCapabilities(){};

			~GeoTileCapabilities(){};  

			const std::string& getName() {return _name;}
			void setName(const std::string& version) {_name = version;}

			const std::string& getVersion() {return _version;}
			void setVersion(const std::string& version) {_version = version;}

			const std::vector<TileData>& getTileData() {return _tiledatas;}
			void setTileData(const std::vector<TileData>& tiledatas){_tiledatas = tiledatas;}

			TileData getTileDataAt(int index){return _tiledatas[index];}
			void replaceTileDataAt(int index, TileData layer){_tiledatas[index] = layer;}

		protected:
			std::string _name;
			std::string _version;

			/*std::string _service;
			std::string _serveraddress;*/

			std::vector<TileData> _tiledatas;	
		};

		class  GeoTileCapabilitiesReader
		{
		public:
			static GeoTileCapabilities* read( const std::string &location, const osgDB::ReaderWriter::Options *options )
			{
				GeoTileCapabilities *caps = NULL;
				if ( osgDB::containsServerAddress( location ) )
				{
					osgEarth::HTTPResponse response = osgEarth::HTTPClient::get( location, options );
					if ( response.isOK() && response.getNumParts() > 0 )
					{
						osgEarth::ReadResult r = osgEarth::URI(location).readString();
						std::stringstream buf( r.getString() );
						caps = read(buf);
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

			static GeoTileCapabilities* read( const std::string &url, const std::string &location,const osgDB::ReaderWriter::Options* options=NULL)
			{
				GeoTileCapabilities *caps = NULL;
				if (osgDB::fileExists(location))
				{
					//__android_log_print(ANDROID_LOG_ERROR, "GeoTileCapabilities", "read cache cap IN");
					std::ifstream in(location.c_str(),ios::binary);
					caps = read( in );

				}
				else
				{
					//__android_log_print(ANDROID_LOG_ERROR, "GeoTileCapabilities", "read cache cap OUT");
					osgEarth::ReadResult rr = osgEarth::URI(url).readString( options );
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

			static GeoTileCapabilities* read( std::istream &in )
			{

				osg::ref_ptr<GeoTileCapabilities> capabilities = new GeoTileCapabilities;
				/*GeoGlobeTileCapabilities capabilities;*/
				osgEarth::Config doc;
				doc.fromXML( in );
				if (doc.empty() || doc.children().empty())
				{
					OE_NOTICE << "Failed to load Capabilities " << std::endl;
					return 0;
				}

				capabilities->setName(doc.child("getcapabilitiesresponse").child("servicecapabilities").child("name").value());
				capabilities->setVersion(doc.child("getcapabilitiesresponse").child("servicecapabilities").child("version").value());
				/*capabilities.setName(doc.child("GetCapabilitiesResponse").child("name").value());
				capabilities.setVersion(doc.child("GetCapabilitiesResponse").child("version").value())*/

				const osgEarth::Config capdata=doc.child("getcapabilitiesresponse").child("servicecapabilities").child("data");
				std::vector<TileData> tdatas;
				for (osgEarth::ConfigSet::const_iterator i=capdata.children().begin(); i != capdata.children().end(); ++i)
				{
					const osgEarth::Config& cftiledata=*i;
					if(cftiledata.key()=="tiledata")
					{
						TileData tdata;
						tdata.layerName=cftiledata.child("layername").value();
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
				capabilities->setTileData(tdatas);
				//__android_log_print(ANDROID_LOG_ERROR, "GeoTileCapabilities", "read cache cap Succeed");
				return capabilities.release();

			}


		private:
			GeoTileCapabilitiesReader(){}
			GeoTileCapabilitiesReader(const GeoTileCapabilitiesReader &cr){}

			virtual ~GeoTileCapabilitiesReader() { }
		};

	}}


#endif
