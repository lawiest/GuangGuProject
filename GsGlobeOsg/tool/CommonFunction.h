#pragma once
#include <Tool/CommonStruct.h>
#include <vector>

//#ifdef MGDBACCESS_EXPORTS
//#define MGDBACCESS_INTERFACE __declspec(dllexport)
//#else
//#define MGDBACCESS_INTERFACE __declspec(dllimport)
//#endif
using namespace GG;
class  CCommonFunction
{
public:
	CCommonFunction(void);
	~CCommonFunction(void);

	//释放由dll申请的内存空间
	void FreeDllNewedMem(char* pMem);

	//产生24字节的唯一Key
	static bool GenUniqueKey(unsigned char* strKey);

	//对层行列进行编码
	static long long TileCoding(int level, int row, int col);
	//对编码进行解码,得到层行列值
	static bool TileUnCoding(long long tKey, int& level, int& row, int& col);

	//计算第零层瓦片分辨率
	static double GetZeroLevelTileSpan(double cellRes, int tileSize=256, int levelNums=1);

	//计算瓦片地理范围
	static GeoBox GetTileRange(long long tileKey, double zeroTileSpanX=180.0f, double zeroTileSpaY=180.0f, double xOrigin=-180.0f, double yOrigin=90.0f);

	//计算行或列编码值
	static unsigned long GetRowOrCol(const GG::GeoPoint& pt, int levelNo=0, double zeroTileSpan=180.0f, double xOrigin=-180.0f, double yOrigin=90.0f);

	//给定坐标和金字塔参数，返回编码值
	static long long GetTileKey(const GG::GeoPoint& pt, int levelNo=0, double zeroTileSpanX=180.0f, double zeroTileSpanY=180.0f, double xOrigin=-180.0f, double yOrigin=90.0f);
	//给定范围和金字塔参数，返回编码值集合
	static std::vector<long long> GetTileKeySet(const GeoBox& box, int levelNo=0, double zeroTileSpanX=180.0f, double zeroTileSpanY=180.0f, double xOrigin=-180.0f, double yOrigin=90.0f);

//360度标准金字塔操作
	static int StdGetLevelNo(double cellRes, int cellSize=256);
	static double StdGetCellRes(double cellRes, int cellSize=256);
	static long long StdGetTileKey(int level, const GG::GeoPoint& pt);
};

