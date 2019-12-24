#pragma once
#include <list>
#include "../utility/preconfig.h"
#include <object.h> 
#include <tinyxml2.h> 
#include <mathhelp.h> 
#include <image.h> 

KERNEL_NS   

/// \brief 常用金字塔名称
enum GsWellknownPyramid
{
	/// \brief 天地图360金字塔模型
	e360DegreePyramid,
	/// \brief WebMercator通用金字塔模型
	eWebMercatorPyramid,

};

/// \brief 金字塔起算原点位置
enum GsPyramidOriginLocation
{
	/// \brief 起算远点在西北角
	///   ■□
	///   □□
	eNorthWest,
	/// \brief 起算远点在东北角
	///   □■
	///   □□
	eNorthEast,
	/// \brief 起算远点在西南角
	///   □□
	///   ■□
	eSouthWest,
	/// \brief 起算远点在东南角
	/// \details 
	/// □□
	/// □■
	eSouthEast,
};
/// \brief 单一金字塔模型
/// \details
///↑←→↓
///金字塔模型的坐标体系是建立在地图坐标系和设备坐标系以及瓦片坐标系三种坐标系之上的
///下面分别说明两种坐标系的方向：
///                  y轴
///地图坐标系的方向  ↑/
///					 →x轴
///---------------------------------------------
///
///设备坐标系的方向   →x轴
///                  ↓
///					y轴
///---------------------------------------------
///瓦片坐标系是用户自定义坐标系
///其坐标轴方向由用户参数定义。
///
///单一金字塔模型，金字塔从开始级到结束级别采用相同的数学基础
class GS_API GsPyramid:public Utility::GsRefObject
{  
	
	void ReadLevel(tinyxml2::XMLElement* pEle);
	void ReadTileBasic(tinyxml2::XMLElement* pEle);
	void ReadTopTile(tinyxml2::XMLElement* pEle);
	void ReadRange(tinyxml2::XMLElement* pEle);
	void ReadConst(tinyxml2::XMLElement* pEle); 
	void ReadBasic(tinyxml2::XMLElement* pEle); 

	void WriteBasic(tinyxml2::XMLElement* pEle);
	void WriteLevel(tinyxml2::XMLElement* pEle);
	void WriteTileBasic(tinyxml2::XMLElement* pEle);
	void WriteTopTile(tinyxml2::XMLElement* pEle);
	void WriteRange(tinyxml2::XMLElement* pEle);
	void WriteConst(tinyxml2::XMLElement* pEle); 
public:
	//从XML节点序列化和反序列化。
	virtual bool ToXmlNode(tinyxml2::XMLNode* pNode);
	virtual bool FromXmlNode(tinyxml2::XMLNode* pNode);
protected:
	//受保护的函数，仅仅用于子类扩展属性的序列化和反序列化使用。
	virtual bool WriteExtendNode(tinyxml2::XMLNode* pEle);
	virtual bool ReadExtendNode(tinyxml2::XMLNode* pEle);
public: 
	GsPyramid(const char* str);   

	/// \brief 获取已知的金字塔模型
	static Utility::GsSmarterPtr<GsPyramid> WellknownPyramid(GsWellknownPyramid ePyramid);
	
	/// \brief 根据金字塔序列化的xml字符串创建金字塔
	static Utility::GsSmarterPtr<GsPyramid> CreatePyramid(const char* xml);

	/// \brief 根据分辨率计算金字塔。
	/// \param OriginX 起算原点x坐标
	/// \param OriginY 起算原点y坐标
	/// \param res 金字塔分辨率的数组
	/// \param nResCount 金字塔辨率的数量
	/// \param tileSizeWidth 瓦片的像素宽度
	/// \param tileSizeHeight 瓦片的像素高度
	/// \param originLocation 起算原点的位置
	static Utility::GsSmarterPtr<GsPyramid> CreatePyramid(double OriginX, double OriginY, double* res, int nResCount,int tileSizeWidth=256,
		int tileSizeHeight=256, GsPyramidOriginLocation originLocation = eNorthWest);


	/// \brief 顶级瓦片的级别索引
	int TopLevelIndex;
	/// \brief 最下级瓦片的级别索引
	int BottomLevelIndex;
	/// \brief 两级之间X方向上的缩放比例
	double ScaleX;
	/// \brief 两级之间Y方向上的缩放比例
	double ScaleY;

	//TileBasic节点
	/// \brief 瓦片X方向上的瓦片像素大小
	unsigned int TileSizeX;
	/// \brief 瓦片Y方向上的瓦片像素大小
	unsigned int TileSizeY;
	/// \brief 瓦片行列的起始行坐标
	int	OriginRowIndex;
	/// \brief 瓦片行列的起始列坐标
	int	OriginColIndex;
	
	/// \brief 金字塔详细描述信息
	Utility::GsString Description;
	/// \brief 金字塔ID，可用于唯一区别金字塔
	Utility::GsString PyramidID;
	/// \brief 金字塔名称，用于可读性
	Utility::GsString Name;

	//TopTile节点
	/// \brief 顶级定位瓦片范围开始X坐标
	double FromX;
	/// \brief 顶级定位瓦片范围开始Y坐标
	double FromY;
	/// \brief 顶级定位瓦片范围结束X坐标
	double ToX;
	/// \brief 顶级定位瓦片范围结束Y坐标
	double ToY;
	
	//Range节点
	/// \brief 金字塔的有效范围最小X
	double XMin;
	/// \brief 金字塔的有效范围最小Y
	double YMin;
	/// \brief 金字塔的有效范围最大X
	double XMax;
	/// \brief 金字塔的有效范围最大Y
	double YMax;

	//Const节点
	/// \brief 金字塔计算使用的常数PI
	double PAI;
	/// \brief 金字塔计算使用的常数容差
	double Tolerance;

	GsPyramid(void);
	virtual ~GsPyramid(void);

	/// \brief 获取金字塔的起算原点位置
	GsPyramidOriginLocation  OriginLocation();


	/// \brief 根据输入分辨率等相关信息判断是否匹配当前金字塔
	/// \param OriginX 起算原点x坐标
	/// \param OriginY 起算原点y坐标
	/// \param res 金字塔分辨率的数组
	/// \param nResCount 金字塔辨率的数量
	/// \param tileSizeWidth 瓦片的像素宽度
	/// \param tileSizeHeight 瓦片的像素高度
	/// \param originLocation 起算原点的位置
	/// \return 返回输入的分辨率从金字塔第几级开始匹配，如果返回INT_MIN则标识无法匹配
	int MathchLevel(double OriginX, double OriginY, double* res, int nResCount, int tileSizeWidth = 256,
		int tileSizeHeight = 256, GsPyramidOriginLocation originLocation = eNorthWest);

	 
	/// \brief 设置参数为缺省参数
	virtual void CreateDefault();

	/// \brief 根据层号和地理坐标计算落入的行列号
	/// \param level 级别
	/// \param x 地理x坐标
	/// \param y 地理y坐标
	/// \param nRow 返回瓦片的行坐标
	/// \param nCol 返回瓦片的列坐标
	/// \param pixelX 相对瓦片左上角的像素x坐标
	/// \param pixelY 相对瓦片左上角的像素y坐标
	/// \return 返回计算是否成功
	virtual bool TileIndex(int level,double x, double y, int* nRow, int* nCol,int *pixelX = NULL,int *pixelY = NULL);
	 
	/// \brief 根据瓦片行列号计算瓦片经纬度范围
	/// \param nLevel 级别
	/// \param nRow 瓦片的行坐标
	/// \param nCol 瓦片的列坐标
	/// \param dblXMin 地理范围最小x
	/// \param dblYMin 地理范围最小y
	/// \param dblXMax 地理范围最大x
	/// \param dblYMax 地理范围最大y
	/// \return 返回计算是否成功
	virtual bool TileExtent(int nLevel, int nRow, int nCol, double* dblXMin, double* dblYMin, double* dblXMax, double* dblYMax);
	
	/// \brief 计算一个地理坐标范围落入的瓦片行列号
	/// \param xmin 地理范围最小x
	/// \param ymin 地理范围最小y
	/// \param xmax 地理范围最大x
	/// \param ymax 地理范围最大y
	/// \param level 级别
	/// \param nMinRow 返回瓦片的最小行坐标
	/// \param nMinCol 返回瓦片的最小列坐标
	/// \param nMaxRow 返回瓦片的最大行坐标
	/// \param nMaxCol 返回瓦片的最大列坐标
	/// \return 返回计算是否成功
	virtual bool TileIndexRange(double xmin, double ymin,double xmax, double ymax, int level, int* nMinRow, int* nMinCol, int* nMaxRow, int* nMaxCol);
	
	/// \brief 计算一个范围的瓦片行列号覆盖的地理范围
	/// \param nLevel 级别
	/// \param nMinRow 瓦片的最小行坐标
	/// \param nMinCol 瓦片的最小列坐标
	/// \param nMaxRow 瓦片的最大行坐标
	/// \param nMaxCol 瓦片的最大列坐标
	/// \param dblXMin 返回地理范围最小x
	/// \param dblYMin 返回地理范围最小y
	/// \param dblXMax 返回地理范围最大x
	/// \param dblYMax 返回地理范围最大y
	/// \return 返回计算是否成功
	virtual bool TileExtentRange( int nLevel,  int nMinRow,  int nMinCol,  int nMaxRow,  int nMaxCol,
			double* dblXMin,  double* dblYMin,  double* dblXMax,  double* dblYMax );
	
	/// \brief 用于确定设备坐标轴方向分别为X方向从左到右，Y轴方向从上到下的情况下，相对这个区域的一个瓦片的裁切区域。
	/// \param device 设置的范围
	/// \param nRow 瓦片的行坐标
	/// \param nCol 瓦片的列坐标
	/// \param position 返回列的范围
	/// \param nLevel 瓦片级别
	/// \return 返回计算是否成功
	virtual bool DevicePosition(const Utility::GsRect&  device,  int nRow,  int nCol,  Utility::GsRect* position,int nLevel=0);

	/// \brief 计算某层的瓦片X方向上的分辨率
	virtual double ResolutionX(int nLevel);
	/// \brief 计算某层的瓦片Y方向上的分辨率
	virtual double ResolutionY(int nLevel);

	/// \brief 计算某层的瓦片X方向上的地理跨度
	/// \param nLevel 瓦片的级别
	/// \param x 测试的x坐标
	/// \param y 测试的y坐标
	/// \return 返回地理跨度
	virtual double TileSpanX(int nLevel,double x,double y);

	/// \brief 计算某层的瓦片Y方向上的地理跨度
	/// \param nLevel 瓦片的级别
	/// \param x 测试的x坐标
	/// \param y 测试的y坐标
	/// \return 返回地理跨度
	virtual double TileSpanY(int nLevel,double x,double y);
	
	/// \brief 计算某层X方向上瓦片划分方向
	/// \param nLevel 瓦片的级别
	virtual int DirectionX(int nLevel);

	/// \brief 计算某层Y方向上瓦片划分方向
	/// \param nLevel 瓦片的级别
	virtual int DirectionY(int nLevel);

	/// \brief 从字符串反序列化
	/// \param str 字符串
	/// \return 返回是否成功
	virtual bool FromString(const char*  str);
	 
	/// \brief 序列化成字符串
	/// \param str 输出的序列化字符串
	/// \return 返回是否成功
	virtual bool ToString(Utility::GsString& str);

	/// \brief 序列化成字符串
	/// \return 返回序列化字符串
	virtual Utility::GsString ToString();

	/// \brief 从输入的分辨率计算最合适的级别
	/// \param dblRes 分辨率
	/// \return 返回StartLevel到EndLevel之间最为合适的级别
	virtual int BestLevel(double dblRes);

	/// \brief 从输入的分辨率计算最合适的级别
	/// \param dblRes 分辨率
	/// \param nStart 起算的级别
	/// \param nEnd 结束计算的级别
	/// \return 返回StartLevel到EndLevel之间最为合适的级别
	virtual int BestLevel(double dblRes,int nStart,int nEnd);

	/// \brief 从输入的分辨率计算最合适的级别
	/// \param dblRes 分辨率
	/// \return 返回准确的层级
	virtual double PreciseLevel(double dblRes);
};
/// \brief GsPyramidPtr
GS_SMARTER_PTR(GsPyramid);

/// \brief 复合金字塔对象
/// \details 允许其中的部分或者全部级别都拥有不同的数学基础
class GS_API GsMultiPyramid:public GsPyramid
{
private: 
	//根据等级寻找一个用于计算的子类
	GsPyramid* FindPyramid(int nLevel);
	virtual bool WriteExtendNode(tinyxml2::XMLNode* pEle);
	virtual bool ReadExtendNode(tinyxml2::XMLNode* pEle);
public: 
	/// \brief 保存子金字塔的列表
	Utility::GsList< GsPyramidPtr> ListSubPyramid;
	GsMultiPyramid();
	GsMultiPyramid(const char* strXML);

	virtual ~GsMultiPyramid();
	/// \brief 根据层号和地理坐标计算落入的行列号
	/// \param level 级别
	/// \param x 地理x坐标
	/// \param y 地理y坐标
	/// \param nRow 返回瓦片的行坐标
	/// \param nCol 返回瓦片的列坐标
	/// \param pixelX 相对瓦片左上角的像素x坐标
	/// \param pixelY 相对瓦片左上角的像素y坐标
	/// \return 返回计算是否成功
	virtual bool TileIndex(int level,double x, double y, int* nRow, int* nCol,int *pixelX = NULL,int *pixelY = NULL);

	/// \brief 根据瓦片行列号计算瓦片经纬度范围
	/// \param nLevel 级别
	/// \param nRow 瓦片的行坐标
	/// \param nCol 瓦片的列坐标
	/// \param dblXMin 地理范围最小x
	/// \param dblYMin 地理范围最小y
	/// \param dblXMax 地理范围最大x
	/// \param dblYMax 地理范围最大y
	/// \return 返回计算是否成功
	virtual bool TileExtent(int nLevel, int nRow, int nCol, double* dblXMin, double* dblYMin, double* dblXMax, double* dblYMax);
	
	/// \brief 计算一个地理坐标范围落入的瓦片行列号
	/// \param xmin 地理范围最小x
	/// \param ymin 地理范围最小y
	/// \param xmax 地理范围最大x
	/// \param ymax 地理范围最大y
	/// \param level 级别
	/// \param nMinRow 返回瓦片的最小行坐标
	/// \param nMinCol 返回瓦片的最小列坐标
	/// \param nMaxRow 返回瓦片的最大行坐标
	/// \param nMaxCol 返回瓦片的最大列坐标
	/// \return 返回计算是否成功
	virtual bool TileIndexRange(double xmin, double ymin,double xmax, double ymax, int level, int* nMinRow, int* nMinCol, int* nMaxRow, int* nMaxCol);
	
	/// \brief 计算一个范围的瓦片行列号覆盖的地理范围
	/// \param nLevel 级别
	/// \param nMinRow 瓦片的最小行坐标
	/// \param nMinCol 瓦片的最小列坐标
	/// \param nMaxRow 瓦片的最大行坐标
	/// \param nMaxCol 瓦片的最大列坐标
	/// \param dblXMin 返回地理范围最小x
	/// \param dblYMin 返回地理范围最小y
	/// \param dblXMax 返回地理范围最大x
	/// \param dblYMax 返回地理范围最大y
	/// \return 返回计算是否成功
	virtual bool TileExtentRange( int nLevel,  int nMinRow,  int nMinCol,  int nMaxRow,  int nMaxCol,
			double* dblXMin,  double* dblYMin,  double* dblXMax,  double* dblYMax );
	/// \brief 计算某层的瓦片X方向上的地理跨度
	/// \param nLevel 瓦片的级别
	/// \param x 测试的x坐标
	/// \param y 测试的y坐标
	/// \return 返回地理跨度
	virtual double TileSpanX(int nLevel,double x,double y);

	/// \brief 计算某层的瓦片Y方向上的地理跨度
	/// \param nLevel 瓦片的级别
	/// \param x 测试的x坐标
	/// \param y 测试的y坐标
	/// \return 返回地理跨度
	virtual double TileSpanY(int nLevel,double x,double y);

	/// \brief 从字符串反序列化
	/// \param str 字符串
	/// \return 返回是否成功
	virtual bool FromString(const char* str);

	/// \brief 用于确定设备坐标轴方向分别为X方向从左到右，Y轴方向从上到下的情况下，相对这个区域的一个瓦片的裁切区域。
	/// \param device 设置的范围
	/// \param nRow 瓦片的行坐标
	/// \param nCol 瓦片的列坐标
	/// \param position 返回列的范围
	/// \param nLevel 瓦片级别
	/// \return 返回计算是否成功
	virtual bool DevicePosition(const Utility::GsRect&  device,  int nRow,  int nCol,  Utility::GsRect* position,int nLevel=0);
	 
};

/// \brief GsMultiPyramidPtr
GS_SMARTER_PTR(GsMultiPyramid);

KERNEL_ENDNS