
#include <TOOL/CommonFunction.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
//#include <sys/time.h>
struct MachineAndPid {
    unsigned char _machineNumber[3];
    unsigned short _pid; 
};

CCommonFunction::CCommonFunction(void)
{
}
CCommonFunction::~CCommonFunction(void)
{
}

//�ͷ���dll������ڴ�ռ�
void CCommonFunction::FreeDllNewedMem(char* pMem)
{
	if(pMem!=NULL) delete []pMem;
}

bool CCommonFunction::GenUniqueKey(unsigned char* strKey)
{
	if(NULL==strKey) return false;

	static const char hexchars[] = "0123456789abcdef";
	static bool bInited=false;
	static unsigned inc=0;

	unsigned char* T;
	int i, j=0;
	
	if(!bInited){
		srand( (unsigned)time(NULL));
		inc=rand();
		bInited=true;
	}

	unsigned char _time[4];
	unsigned t=(unsigned)time(0);
	T=(unsigned char *)&t;
    _time[0] = T[3];
    _time[1] = T[2];
    _time[2] = T[1];
    _time[3] = T[0];

	for(i=0; i<4; i++)
	{
		strKey[j++]=hexchars[(_time[i]&0xF0)>>4];
		strKey[j++]=hexchars[(_time[i]&0x0F)];
	}

	srand((unsigned)time(NULL));
	unsigned a=rand();
	unsigned b=rand();
	unsigned long long ull=(((unsigned long long)a)<<32)|b;

	MachineAndPid map=(MachineAndPid&)ull;
	unsigned pid=/*GetCurrentProcessId()*/0;
	map._pid ^= (unsigned short)pid;
	unsigned short& rest = (unsigned short &)map._machineNumber[1];
    rest ^= pid>>16;

	T=(unsigned char *)&map;
	for(i=0; i<5; i++)
	{
		strKey[j++]=hexchars[(T[i]&0xF0)>>4];
		strKey[j++]=hexchars[(T[i]&0x0F)];
	}

	unsigned char _inc[3];
	unsigned new_inc = inc++;
    T=(unsigned char *)&new_inc;
    _inc[0] = T[2];
    _inc[1] = T[1];
    _inc[2] = T[0];

	for(i=0; i<4; i++)
	{
		strKey[j++]=hexchars[(_inc[i]&0xF0)>>4];
		strKey[j++]=hexchars[(_inc[i]&0x0F)];
	}

	strKey[24]='\0';
	return true;
}

long long CCommonFunction::TileCoding(int level, int row, int col)
{
	long long tileKey=-1;

	//����������������32��
	if(level>0x1f || level<0) return tileKey;
	//26λ�����ɱ���1���׷ֱ��ʵ�64*64��Ƭ
	if(row>0x3ffffff || row<0) return tileKey;
	if(col>0x3ffffff || col<0) return tileKey;
	
	tileKey=(((long long)level)<<52);
	
//�����кź��кţ�morton��
	long long n64Bits, nMap=0x1;

	for(int i=0; i<26; i++)
	{
		n64Bits=(long long)(row&nMap);
		tileKey|=(n64Bits<<(i+1));

		n64Bits=(long long)(col&nMap);
		tileKey|=(n64Bits<<i);

		nMap<<=1;
	}
	return tileKey;
}

bool CCommonFunction::TileUnCoding(long long tKey, int& level, int& row, int& col)
{
	if(tKey<0) return false;

	long long nMap;

	nMap=tKey;
	level=(int)(nMap>>52);
	level&=0x1f;

	long long n64Bits;

//�����кź��к�	
	nMap=0x1; row=0x0; col=0x0;
	for(int i=0; i<26; i++)
	{
		n64Bits=(tKey&nMap);
		col|=(int)(n64Bits>>i);

		nMap<<=1;
		n64Bits=(tKey&nMap);
		row|=(int)(n64Bits>>(i+1));

		nMap<<=1;
	}
	
	return true;
}

//����������Ƭ�ֱ���
double CCommonFunction::GetZeroLevelTileSpan(double cellRes, int tileSize, int levelNums)
{
	double f=cellRes*tileSize;
	for(int i=1; i<levelNums; i++)
		f*=2.0f;
	return f;
}

GeoBox CCommonFunction::GetTileRange(long long tileKey, double zeroTileSpanX, double zeroTileSpanY, double xOrigin, double yOrigin)
{
	GeoBox range;
	range.SetInvalid();

	int i, level, row, col;
	if(!TileUnCoding(tileKey, level, row, col)) return range;

	double fSpanX=zeroTileSpanX, fSpanY=zeroTileSpanY;
	for(i=0; i<level; i++)
	{
		fSpanX/=2.0f;
		fSpanY/=2.0f;
	}

	range.ll.x=col*fSpanX-xOrigin;
	range.tr.x=range.ll.x+fSpanX;
	range.tr.y=yOrigin-row*fSpanY;
	range.ll.y=range.tr.y-fSpanY;
	
	return range;
}

unsigned long CCommonFunction::GetRowOrCol(const GeoPoint& pt, int levelNo, double zeroTileSpan, double xOrigin, double yOrigin)
{
	double fSpan=zeroTileSpan;
	for(int i=0; i<levelNo; i++)
		fSpan/=2.0f;

	unsigned long code=(unsigned long)((pt.x-xOrigin+1e-8)/fSpan);
	return code;
}

long long CCommonFunction::GetTileKey(const GeoPoint& pt, int levelNo, double zeroTileSpanX, double zeroTileSpanY, double xOrigin, double yOrigin)
{
	int i, row, col;
	double fSpanX, fSpanY;

	fSpanX=zeroTileSpanX;
	fSpanY=zeroTileSpanY;
	for(i=0; i<levelNo; i++)
	{
		fSpanX/=2.0f;
		fSpanY/=2.0f;
	}

	col=(int)((pt.x-xOrigin+1e-8)/fSpanX);
	/*----Y�᷽����Ƭԭ�����Ϸ�----*/
	row=(int)((yOrigin-pt.y-1e-8)/fSpanY);
	/*----Y�᷽����Ƭԭ�����·�----*/
	//row=(int)((pt.y-yOrigin+1e-8)/fSpan);

	return TileCoding(levelNo, row, col);
}

std::vector<long long> CCommonFunction::GetTileKeySet(const GeoBox& box, int levelNo, double zeroTileSpanX, double zeroTileSpanY, double xOrigin, double yOrigin)
{
	int i, j, row1, row2, col1, col2;
	double fSpanX, fSpanY;
	long long tKey;
	std::vector<long long> keyList;

	fSpanX=zeroTileSpanX;
	fSpanY=zeroTileSpanY;
	for(i=0; i<levelNo; i++)
	{
		fSpanX/=2.0f;
		fSpanY/=2.0f;
	}

	col1=(int)((box.ll.x-xOrigin+1e-8)/fSpanX);
	col2=(int)((box.tr.x-xOrigin+1e-8)/fSpanX);
	/*----Y�᷽����Ƭԭ�����Ϸ�----*/
	row1=(int)((yOrigin-box.tr.y-1e-8)/fSpanY);
	row2=(int)((yOrigin-box.ll.y-1e-8)/fSpanY);
	/*----Y�᷽����Ƭԭ�����·�----*/
	/*row1=(int)((box.ll.y-yOrigin+1e-8)/fSpanY);
	row2=(int)((box.tr.y-yOrigin+1e-8)/fSpanY);*/

	for(i=row1; i<=row2; i++)
		for(j=col1;j<=col2; j++)
		{
			tKey=TileCoding(levelNo, i, j);
			keyList.push_back(tKey);
		}

	return keyList;
}

int CCommonFunction::StdGetLevelNo(double cellRes, int cellSize)
{
	int i;
	double f1, f2, f3;
	
	i=0;
	f1=360.0f/(double)cellSize;
	f2=f1/2.0f;

	while(true){
		if(fabs(f1-cellRes)<1e-8) return i;
		if(fabs(f2-cellRes)<1e-8) return i+1;
		if(f1>cellRes && cellRes>f2) break;

		i++;
		f1/=2.0f; f2/=2.0f;
	}

	f3=(f1+f2)/2.0f;
	if((cellRes-f3)<1e-8) i++;
	return i;
}

double CCommonFunction::StdGetCellRes(double cellRes, int cellSize)
{
	int level=StdGetLevelNo(cellRes, cellSize);

	double f1=360.0f/(double)cellSize;
	for(int i=0; i<level; i++)
		f1/=2.0f;

	return f1;
}

long long CCommonFunction::StdGetTileKey(int level, const GeoPoint& pt)
{
	int i, row, col;
	double f1;

	f1=360.0f;
	for(i=0; i<level; i++)
		f1/=2.0f;

	row=(int)((90.0f-pt.y-1e-8)/f1);
	col=(int)((pt.x+180.0f+1e-8)/f1);
	return TileCoding(level, row, col);
}
