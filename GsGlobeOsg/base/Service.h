#pragma  once

#include <osg/Referenced>
#include <osg/ref_ptr>
#include <osg/Version>

#if OSG_MIN_VERSION_REQUIRED(2,9,5)
	#include <osgDB/Options>
#endif

struct BoundBox
{
	double west;
	double south;
	double east;
	double north;
};

struct TileMatrix
{
	std::string ID;
	double ScaleDenominator;
	double north;
	double west;
	int TileWidth;
	int TileHeight;
	int MatrixWidth;
	int MatrixHeight;
};

struct TileMatrixSet
{
	std::string ID;
	std::string SupportedCRS;
	std::vector<TileMatrix> TileMatrixs;

	TileMatrix getTileMatrixAt(int index)
	{
		if (index > TileMatrixs.size() - 1)
			return TileMatrixs[TileMatrixs.size() - 1];

		return TileMatrixs[index];
	}
};

struct wmtsLayer
{
	std::string Title;
	std::string Abstract;
	std::string ID;
	BoundBox bound;
	BoundBox wgs84bound;
	std::string style;
	std::string format;
	std::string TileMatrixSetLink;
};

struct TileMetaData
{
	std::string tile;
	std::string crs;

	BoundBox bbox;
	double LevelZeroTileSizeX;
	double LevelZeroTileSizeY;

	int toplevel;
	int bottomlevel;

	int tilepixelsX;
	int tilepixelsY;

	std::string DEMDataType;

	int nDataSetType;
	std::string strDataSetName;
	std::string strLayerURL;
};

struct Pyramid {};

namespace osgEarth { namespace Util
{
	
    class  WMTSCapabilities : public osg::Referenced
    {
    public:
		WMTSCapabilities();
		~WMTSCapabilities();  

		const std::vector<wmtsLayer>& getLayer() {return _layerset;}
        void setLayer(const std::vector<wmtsLayer>& layerset){_layerset = layerset;}

		const std::vector<TileMatrixSet>& getMatrixSet(){return _matrixset;}
        void setMatrixSet(const std::vector<TileMatrixSet>& matrixset){_matrixset = matrixset;}

		wmtsLayer getLayerAt(int index){return _layerset[index];}
		void replaceLayerAt(int index, wmtsLayer layer){_layerset[index] = layer;}

		TileMatrixSet getMatrixSetAt(int index){return _matrixset[index];}
		void replaceMatrixSetAt(int index, TileMatrixSet matrixset){ _matrixset[index] = matrixset;}

    protected:
	
		std::vector<wmtsLayer> _layerset;
		std::vector<TileMatrixSet> _matrixset;
    };

    /*
    * Reads Capabilities from a URL or file
    */
    class  WMTSCapabilitiesReader
    {
    public:
		static WMTSCapabilities* read(const std::string &location, const osgDB::ReaderWriter::Options *options = NULL);
		
		static WMTSCapabilities* read(const std::string &url, const std::string &location, const osgDB::ReaderWriter::Options* options = NULL);
		
		static WMTSCapabilities* read(std::istream &in);
    private:
		WMTSCapabilitiesReader();
		WMTSCapabilitiesReader(const WMTSCapabilitiesReader &cr);

        /** dtor */
		virtual ~WMTSCapabilitiesReader();
    };

    class  GeoGlobeTileCapabilities : public osg::Referenced
    {
		public:
			GeoGlobeTileCapabilities(){};

			~GeoGlobeTileCapabilities(){};  

			const std::string& getName() {return _name;}
			void setName(const std::string& version) {_name = version;}

			const std::string& getVersion() {return _version;}
			void setVersion(const std::string& version) {_version = version;}

			const std::vector<TileMetaData>& getTileMetaData() {return _tilemetadatas;}
			void setTileMetaData(const std::vector<TileMetaData>& tilemetadatas){_tilemetadatas = tilemetadatas;}

			TileMetaData getTileMetaDataAt(int index){return _tilemetadatas[index];}
			void replaceTileMetaDataAt(int index, TileMetaData layer){_tilemetadatas[index] = layer;}

		protected:
			std::string _name;
			std::string _version;
			std::vector<TileMetaData> _tilemetadatas;	
		};

		/*
		* Reads Capabilities from a URL or file
		*/
		class  GeoGlobeTileCapabilitiesReader
		{
		public:
			static GeoGlobeTileCapabilities* read(const std::string &location, const osgDB::ReaderWriter::Options *options = NULL);

			static GeoGlobeTileCapabilities* read(const std::string &url, const std::string &location, const osgDB::ReaderWriter::Options* options = NULL);

			static GeoGlobeTileCapabilities* read(std::istream &in);
		
		private:
			GeoGlobeTileCapabilitiesReader(){}
			GeoGlobeTileCapabilitiesReader(const GeoGlobeTileCapabilitiesReader &cr){}

			virtual ~GeoGlobeTileCapabilitiesReader() { }
		};

	} 
}

