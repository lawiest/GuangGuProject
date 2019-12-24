
#ifndef GEOGLOBE_PLACENAME_PLACENAMETILE
#define GEOGLOBE_PLACENAME_PLACENAMETILE 1

#include <PlaceName/PlaceNode.h> 
#include <PlaceName/geopoi.pb.h>
#include <PlaceName/PlaceNameTileOptions.h>

GLOBE_NS

typedef unsigned long long  ULONGLONG;
	
class CPBPOITile
{ 
	GEOPOI::PBPOITile m_POITile;
public:
	CPBPOITile(void)
	{
	}
	virtual ~CPBPOITile(void)
	{

	}

	GEOPOI::PBPOITile* TilePtr()
	{
		return &m_POITile;
	}

	void ReSet()
	{
		m_POITile = GEOPOI::PBPOITile();
	}

	virtual bool From(const unsigned char* pBuff,int nSize)
	{
		return m_POITile.ParseFromArray(pBuff,nSize);
	}


	long long Version()
	{
		m_POITile.version();
	}
	void Version(long long  ver)
	{
		m_POITile.set_version(ver);
	}

	long long TileKey()
	{
		return m_POITile.tilekey();
	}
	void TileKey(long long nKey)
	{
		m_POITile.set_tilekey(nKey);
	}

	void TileKey(int l,int r,int c)
	{
		long long nKey = Code(l,r,c);
		m_POITile.set_tilekey(nKey);
	}

	int Level()
	{
		long long nKey = m_POITile.tilekey();
		return Level(nKey);
	}

	int Row()
	{
		long long nKey = m_POITile.tilekey();
		return Row(nKey);
	}
	int Col()
	{
		long long nKey = m_POITile.tilekey();
		return Col(nKey);
	}

	int Level(long long nTileKey)
	{
		unsigned int nLevel = (long long)(nTileKey >> 48);
		nLevel &= 0x3f;
		return nLevel;
	}

	int Row(long long nTileKey)
	{
		long long n64Bits;
		long long nMap = 0x1;
		int nRow = 0x0; 

		for (int i=0; i<24; i++)
		{
			n64Bits = (nTileKey & nMap); 

			nMap <<= 1;
			n64Bits = (nTileKey & nMap);
			nRow |= (long long)(n64Bits >> (i + 1));

			nMap <<= 1;
		}

		return nRow;
	}

	int Col(long long nTileKey)
	{
		long long n64Bits;
		long long nMap = 0x1;
		int nCol = 0x0;

		for (int i=0; i<24; i++)
		{
			n64Bits = (nTileKey & nMap);
			nCol |= (long long)(n64Bits >> i);

			nMap <<= 1;
			n64Bits = (nTileKey & nMap); 
			nMap <<= 1;
		}
		return nCol;
	}

	long long Code(int nLevel, int nRow, int nCol)
	{
		long long n64Bits;
		long long nKey = 0;

		n64Bits = (long long)nLevel;
		nKey |= (n64Bits << 48);

		int nMap = 0x1;
		for (int i = 0; i<24; i++)
		{
			n64Bits = (long long)(nRow & nMap);
			nKey |= (n64Bits << (i+1));

			n64Bits =(long long)(nCol & nMap);
			nKey |= (n64Bits << i);

			nMap <<= 1;
		}
		return nKey;
	}

	void Decode(long long nKey,int& nLevel, int& nRow, int& nCol)
	{
		nLevel = Level(nKey);
		nRow = Row(nKey);
		nCol = Col(nKey);
	}
};

class CPBPOI
{
	GEOPOI::PBPOI* m_POI; 
	GEOPOI::PBPOITile* m_pTile;
public:
	CPBPOI(GEOPOI::PBPOI* POI,GEOPOI::PBPOITile* pTile):m_POI(POI)
	{
		m_pTile = pTile;
		m_POI = POI;
	}
	CPBPOI(GEOPOI::PBPOITile* pTile) 
	{
		m_pTile = pTile;
		m_POI = m_pTile->add_pois();

	}
	virtual ~CPBPOI()
	{ 
	}
	GEOPOI::PBPOI* POIPtr()
	{
		return m_POI;
	}
	int FontSize()
	{
		if(m_POI->has_fontsize())
			return m_POI->fontsize();
		return 16;
	}
	std::string FontName()
	{
		if(m_POI->has_fontnameindex())
			return m_pTile->mutable_stringtable()->s(m_POI->fontnameindex());
		return std::string("微软雅黑");
	}
	void FontName(const char* strName)
	{
		GEOPOI::StringTable* pTable = m_pTile->mutable_stringtable();
		for(int i =0;i<pTable->s_size();i++)
		{
			std::string strT = pTable->s(i);
			if(strcmp(strName,strT.c_str())==0)
			{
				m_POI->set_fontnameindex(i);
				return;
			}
		}

		m_POI->set_fontnameindex(pTable->s_size());
		pTable->add_s(strName);
	}
};



class PlaceNameTile : public osg::Group
{
public:
    PlaceNameTile(const osgEarth::TileKey& tileKey, GLOBE_NAME::PlaceNameLayerOptions* options);
public:
	/*是否有数据*/
	bool hasRealData();
public:
	int         _level;
	int         _row;
	int         _col;
	/*创建瓦片内的地名数据*/
	void createPlaceNameTile();
    /*创建地名图标*/
	osg::ref_ptr<osg::Geometry> createIcon(int symbolID);
	osg::ref_ptr<osg::Image> createIconImage(int symbolID);
    /*地名图标写入数据库中*/
	bool writeIconToCache(int dataID,const char* pData,int dataType,int nDataLen);
    /*地名的通用属性*/
	PlaceNameLayerOptions*            _options;
	/*瓦片Key值*/
	osgEarth::TileKey          _tileKey;
	float            _minRange;
	float            _maxRange;
	osg::Vec3d       _center;       
	/*瓦片平均*/
	double           _altitude;
};

GLOBE_ENDNS

#endif // OSGEARTH_DRIVERS_MP_TERRAIN_ENGINE_TILE_PAGED_LOD
