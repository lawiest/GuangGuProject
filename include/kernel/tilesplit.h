#pragma once
#include "../utility/preconfig.h" 
#include "geodatabase.h"
#include <map.h>
#include "image.h"

KERNEL_NS

/// \brief 基于文件缓存的矢量瓦片切片
class GS_API GsFileVectorTileSpliter:public Utility::GsRefObject
{  
	struct LayerStruct:public UTILITY_NAME::GsVector<Utility::GsString>
	{
		int Index;
		Utility::GsString Name;
		LayerStruct()
		{
			Index = 0;
		}
		LayerStruct(int index,const char* name)
		{
			Index = index;
			Name = name;
		}
	};
	/// \brief 金字塔对象
	GsPyramidPtr		m_ptrPyramid;
	/// \brief 切片级别范围
	int m_MinLevel,m_MaxLevel;
	/// \brief 总碎片数
	long long m_nTotalPieces;
	/// \brief 几何简化容差
	double m_SimplfyTol;
	/// \brief 瓦片数据集对象
	GsTileClassPtr				m_ptrTileClass;
	/// \brief 坐标转换对象
	GsCoordinateTransformationPtr m_ptrTans;
	/// \brief 切片过滤范围
	geostar::gobjptr			m_FilterExtent;
	/// \brief 切片过滤范围所在瓦片外接矩形
	geostar::gobjptr		    m_FilterGeoTilesExtent;
	//因为会外扩所以写入缓存的瓦片会过多，使用层行列过滤掉部分瓦片
	Utility::GsStlMap<int, GeoStar::Utility::GsRect> m_mapLevelRect;
	/// \brief 是否裁切
	bool m_bKnifeClip;
	/// \brief 裁切的trans
	geostar::gobjptr	m_TranKnife;
	/// \brief 瓦片属性缓存
	Utility::GsGrowByteBuffer m_AttributeBuffer;
	/// \brief 瓦片图层索引缓存
	Utility::GsStlMap<Utility::GsString,LayerStruct> m_LayerIndex;
	/// \brief 切片对象缓存
	Utility::GsStlMap<int,geostar::gobjptr> m_ClipCache;
	/// \brief 临时切片文件io对象
	geostar::geo_reader* m_TempRead;
	geostar::geo_writer* m_TempWriter;
	// 切片临时文件存储路径
	Utility::GsString m_strCacheFile;

	/// \brief 创建临时切片文件io对象
	void CreateReader();
	/// \brief 获取图层的索引
	int LayerIndex(const char* layerName,GsFeature* pFea,int* pAttIndex,int nAttCount);
	/// brief 根据level创建一个clip
	geostar::geo_trans* CreateClip(int nLevel);
	/// brief 写一个碎片到临时文件
	void Write(geostar::geo_object* o);
	/// brief 预先切割,返回切割后gobj对象数据，防止一次性切割内存占用过大
	geostar::gobjptr PreTrans(geostar::gobjptr ptrGeo,int level);
	/// brief 计算预切割层级 返回当前层级表示不需要预切割
	int NeedPareClipLevel(geostar::gobjptr ptrGeo,GsPyramid *ptrPyramid,int level);
	bool m_FilterInvalidPixel;
	Utility::GsStlMap<unsigned long long, int> m_IndexMap;
public:
	GsFileVectorTileSpliter(GsTileClass* pTileClass,GsPyramid* pPyramid,int nMin,int nMax,double dblSimplfyTol = 0);
	virtual ~GsFileVectorTileSpliter();
	
	/// \brief 设置过滤范围,过滤范围外的数据不会进入瓦片
	void FilterExtent(const GsBox& box);
	/// \brief 设置过滤范围，过滤范围外的数据不会进入瓦片
	void FilterExtent(GsGeometry* pGeo);

	/// \brief 坐标转换对象
	void CoordinateTransformation(GsCoordinateTransformation* ptrTransObj);
	/// \brief 坐标转换对象 
	GsCoordinateTransformation* CoordinateTransformation();
	/// \brief 设置获取最小的切片级别
	int& MinLevel();
	/// \brief 设置获取最大的切片级别
	int& MaxLevel();
	/// \brief 添加一个Feature进行瓦片切割。
	bool Add(GsFeature* pFea,int* pAttIndex,int nAttCount,const char* lyrName =  NULL);
	/// \brief 提交数据。
	virtual void Commit(); 

	/// \brief 设置是否对生产的瓦片使用GZ压缩
	void GZipCompress(bool bEnable);
	/// \brief 获取是否对生产的瓦片使用GZ压缩
	bool GZipCompress();

	/// \brief 设置切片临时文件存储路径
	void CachePath(const char* strPath);

	/// \brief 设置是否只生产过滤范围内的数据
	void Clip(bool bClip);
	/// \brief 获取是否只生产过滤范围内的数据
	bool Clip();
	void FilterInvalidPixel(bool b);
	bool FilterInvalidPixel();
	void ClearInvalidPixel();
public:
	/// \brief 当提交数据时显示进度。
	Utility::GsDelegate<void(long long,long long)> OnCommit;
};

/// \brief GsFileVectorTileSpliterPtr
GS_SMARTER_PTR(GsFileVectorTileSpliter);

/// \brief 栅格瓦片切片
class GS_API GsRasterTileSpliter
{
	long long m_nNow;
	long long m_nTotal;
	int m_nTileSize;
	int m_nPixelByteSize;
	int m_nCommit, m_nMaxCommit;
	Utility::GsSharedPtr<GsColorToRGBA> m_ptrTrans;
private:
	bool Spliter(int level, Utility::GsGrowByteBuffer* buff, Utility::GsImage* img, const GsBox& box);
	bool Spliter(int level,int row,int col,int width,int height, Utility::GsGrowByteBuffer* buff);
private:

	GsGeometryPtr m_ptrFilterExtent;
	GsRasterClassPtr m_ptrRasterClass;
	GsTileClassPtr	 m_ptrTileClass;
	GsPyramidPtr	m_ptrPyramid; 
	/// \brief 内存缓存瓦片行数量
	int				m_MemCacheTileRows;
	/// \brief 内存缓存瓦片列数量
	int				m_MemCacheTileCols;
public:
	GsRasterTileSpliter(GsRasterClass* pRasterClass, GsPyramid* pPyramid, 
		GsTileClass* pTileClass);
	/// \brief 最大提交数量，保存瓦片数量达到此数据量之后提交一次事务
	int MaxCommit();
	/// \brief 最大提交数量，保存瓦片数量达到此数据量之后提交一次事务
	void MaxCommit(int commit);


	/// \brief 内存缓存瓦片行数量
	int MemoryCacheTileRows();
	/// \brief 内存缓存瓦片行数量
	void MemoryCacheTileRows(int r);

	/// \brief 内存缓存瓦片列数量
	int MemoryCacheTileCols();
	/// \brief 内存缓存瓦片列数量
	void MemoryCacheTileCols(int c);


	/// \brief 设置过滤范围,过滤范围外的数据不会进入瓦片
	void FilterExtent(const GsBox& box);
	/// \brief 设置过滤范围，过滤范围外的数据不会进入瓦片
	void FilterExtent(GsGeometry* pGeo);

	/// \brief 获取过滤范围 
	GsGeometry* FilterExtent();
	 
	/// \brief 执行切割过程
	bool Execute(int nStartLevel, int nEndLevel);

public:
	/// \brief 当切割瓦片的时候发送的进度，返回false则标识中断
	Utility::GsDelegate<bool(int,int ,int,long long, long long)> OnProgress;
};

class GS_API GsMapTileSpliter
{
public:
	GsMapTileSpliter(const char* pMapDefinePath, GsPyramid* pPyramid,GsTileClass* pTileClass);

	void FilterExtent(const GsBox& box);
	/// \brief 设置过滤范围，过滤范围外的数据不会进入瓦片
	void FilterExtent(GsGeometry* pGeo);
	/// \brief 获取过滤范围 
	GsGeometry* FilterExtent();
	/// \brief 执行切割过程
	bool Execute(int nStartLevel, int nEndLevel);

	void SuperWidth(int w);

	int SuperWidth();
	Utility::GsDelegate<bool(int, int, int, long long, long long)> OnProgress;

protected:
	//大位图
	GsMemoryImageCanvasPtr m_Canvas;
	//临时小位图，
	GsMemoryImageCanvasPtr m_smallCanvas;
	//临时空瓦片，用于判断切出来的瓦片是否为空
	GsMemoryImageCanvasPtr m_EmptyCanvas;
	//临时空瓦片，用于判断切出来的瓦片是否为空
	GsMemoryImageCanvasPtr m_EmptyBigCanvas;
	GsDisplayPtr m_ptrDisplay;
	GsTrackCancelPtr m_ptrTrack;
	GsDisplayTransformationPtr m_pDT;
	int m_minLevel;
	int m_maxLevel;
	GsMapPtr m_pMap;
	GsPyramidPtr m_pPyramid;
	GsTileClassPtr m_pTileClass;
	GsBox m_tmpBox;
	GsGeometryPtr m_pGeo;
	GsColor m_Col;
	int m_width;//默认256*10
	//瓦片输出类型
	GsTileEncodingType m_TileEncodingType;
	//颜色自适应
	bool m_bUseColorDepth;
	//水印
	bool m_MarkWater;
	//裁切瓦片
	bool m_Clip;
	GsGraphicsPathPtr m_CanvasClip;
	bool DrawBigCanvas(GsBox box, GeoStar::Utility::GsRect rect);
	long ClipHugeCanvas(int nLevel, int nMinRow, int nMinCol, int nMaxRow, int nMaxCol);
	
	bool InitLevelExent(int nLevel, int &nMinRow, int& mMinCol, int &nMaxRow, int &nMaxCol);
	bool InitClip();

	void LRC2Box(int l, int r1, int c1, int r2, int c2,GsBox &box);
};
KERNEL_ENDNS