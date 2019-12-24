#ifndef OSGEARTHUTIL_GEOWFS_H
#define OSGEARTHUTIL_GEOWFS_H 1

#include <osgEarth/GeoData>
#include <osgEarth/URI>

#define ATTR_VERSION "version"
#define ELEM_CAPABILITY "capability"
#define ELEM_SERVICE "service"
#define ELEM_REQUEST "request"
#define ELEM_ABSTRACT "abstract"
#define ELEM_TILED "tiled"
#define ELEM_MAXLEVEL "maxlevel"
#define ELEM_FIRSTLEVEL "firstlevel"
#define ELEM_FORMAT "format"
#define ELEM_NAME "name"
#define ELEM_TITLE "title"
#define ELEM_SRS "srs"
#define ELEM_FEATURETYPELIST "featuretypelist"
#define ELEM_FEATURETYPE "featuretype"
#define ELEM_LATLONGBOUNDINGBOX "latlongboundingbox"
#define ATTR_MINX              "minx"
#define ATTR_MINY              "miny"
#define ATTR_MAXX              "maxx"
#define ATTR_MAXY              "maxy"

namespace GeoGlobe { namespace GeoVector
{
	//WFS服务中要素类型结构体
    class GEOWFSFeatureType : public osg::Referenced
    {
    public:
		GEOWFSFeatureType();

        /** dtor */
        virtual ~GEOWFSFeatureType() { }

        const std::string& getName() const { return _name;}//要素的名称
        void setName(const std::string& name) { _name = name;}

        const std::string& getTitle() const { return _title;}//要素的标题
        void setTitle(const std::string& title) { _title = title;}

        const std::string& getSRS() const { return _srs;}//要素的空间坐标系
        void  setSRS(const std::string& srs) { _srs = srs; }
		const std::string& getOriginSRS() const { return m_OriginSRS; }//要素的空间坐标系
		void  setOriginSRS(const std::string& srs) { m_OriginSRS = srs; }

        const std::string& getAbstract() const { return _abstract;}
        void setAbstract(const std::string& abstract) { _abstract = abstract; }

        const osgEarth::GeoExtent& getExtent() const { return _extent;}
        void setExtent(const osgEarth::GeoExtent& extent) { _extent = extent;}

        int getMaxLevel() const { return _maxLevel;}
        void setMaxLevel( int maxLevel ) { _maxLevel = maxLevel; }

        int getFirstLevel() const { return _firstLevel;}
        void setFirstLevel( int firstLevel ) { _firstLevel = firstLevel; }

        bool getTiled() const { return _tiled;}
        void setTiled(bool tiled) { _tiled = tiled;}
        
    private:
        std::string _name;
        std::string _title;
        std::string _srs;
		std::string m_OriginSRS;
		
        std::string _abstract;
        osgEarth::GeoExtent   _extent;
        bool        _tiled;
        int         _maxLevel;
        int         _firstLevel;
    };

    typedef std::vector< osg::ref_ptr< GEOWFSFeatureType > > GEOFeatureTypeList;

    /**
    *WFS Capabilities
    */
    class GEOWFSCapabilities : public osg::Referenced
    {
    public:
        GEOWFSCapabilities(){}

        /** dtor */
        virtual ~GEOWFSCapabilities() { }

        /**
        *Gets the WFS capabilities version
        */
        const std::string& getVersion() const {return _version;}

        /**
        *Sets the WFS capabilities version
        */
        void setVersion(const std::string& version) {_version = version;}        

        const std::string& getName() const { return _name; }
        void setName(const std::string& name) { _name = name; }

        const std::string& getTitle() const { return _title;}
        void setTitle(const std::string& title) { _title = title;}

        const std::string& getAbstract() const { return _abstract; }
        void setAbstract( const std::string& abstract) { _abstract = abstract; }

		GEOWFSFeatureType* getFeatureTypeByName(const std::string& name);
        GEOFeatureTypeList& getFeatureTypes() { return _featureTypes; }
                
    protected:
        GEOFeatureTypeList _featureTypes;

        std::string _version;
        std::string _name;
        std::string _title;
        std::string _abstract;
    };

   //WFS服务Capability读取类
    class GEOWFSCapabilitiesReader
    {
    public:
		static GEOWFSCapabilities* read(const osgEarth::URI& location, std::string filename, const osgDB::Options* dbOptions = 0L);
		static GEOWFSCapabilities* read(const osgEarth::URI& location, const osgDB::Options* dbOptions = 0L);
		static GEOWFSCapabilities* read(std::istream &in);
    private:
        GEOWFSCapabilitiesReader(){}
        GEOWFSCapabilitiesReader(const GEOWFSCapabilitiesReader &cr){}

        virtual ~GEOWFSCapabilitiesReader() { }
    };

} }

#endif
