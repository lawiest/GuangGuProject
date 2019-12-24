#ifndef _CSQL_H_
#define _CSQL_H_
#include "CoreOSG.h"
#include <sqlite3.h>
#include <iostream>

// memcpy
#include<stdio.h>
#include <string.h>

//#include <WinSock2.h>

//#pragma comment(lib,"Ws2_32.lib")

#include "base/SERVICE.h"

//数据库文件操作类，包括事务的提交、数据库语句执行
class CSqliteDBCM
{
	long m_CommitCount;
	long m_nMaxCount;
public:
	sqlite3 * m_pDB;
	CSqliteDBCM():m_pDB(NULL)
	{
		m_CommitCount = -1;
		m_nMaxCount = 1000; 

	}
	virtual ~CSqliteDBCM()
	{
		Commit();
		Close();
	}
	void BeginCommit()
	{
		m_CommitCount = 0;
		Begin();
	}
	long LastInsertRowID()
	{
		return sqlite3_last_insert_rowid(m_pDB);

	}
	long CommitCount()
	{
		return m_CommitCount;
	}
	void MaxCommit(long nMax)
	{
		m_nMaxCount = nMax;
	}
	long MaxCommit()
	{
		return m_nMaxCount;
	}
	void CommitOne()
	{
		if(m_CommitCount== 0)
			Begin();
		m_CommitCount++;


		if(m_CommitCount >m_nMaxCount)
		{
			Commit();
			m_CommitCount = 0;

		}

	}

	void Close()
	{
		if(NULL != m_pDB)
			sqlite3_close(m_pDB);

		m_pDB = NULL;
	}
	sqlite3* DB()
	{
		return m_pDB;
	}
	void Attach(sqlite3* db)
	{	
		Close();
		m_pDB = db;
		BeginCommit();
	}
	//开始会话
	bool Begin()
	{
		return Excute("begin");
	}
	bool Commit()
	{
		m_CommitCount=0;
		return Excute("commit");
	}
	bool Abort()
	{
		return Excute("rollback");
	}

	int Changes()
	{
		return sqlite3_changes(m_pDB);
	}


	void ReportError()
	{
		std::string str = ErrorMessage();
		if(str.empty() )
			return ;

		std::cout<<str.c_str()<<std::endl;
	}
	//最后的消息
	std::string ErrorMessage()
	{
		const char* strError = sqlite3_errmsg(m_pDB);
		if(NULL == strError)
			return std::string();

		return std::string(strError);
	}
	static bool Error(int nErrorCode)
	{
		return !(SQLITE_OK == nErrorCode ||
			SQLITE_DONE == nErrorCode);

	}
	template<class T>
	T Error(int nErrorCode,T bOkValue,T bFailValue)
	{
		if(!Error(nErrorCode))
			return bOkValue;

		const char* strError = sqlite3_errmsg(m_pDB);
		std::cout<<strError <<std::endl;
		return bFailValue;
	}
	//执行SQL语句
	bool Excute(const char* sql)
	{
		int nRet = sqlite3_exec(m_pDB,sql,NULL,NULL,NULL);
		if(nRet == SQLITE_OK )
			return true;
		return Error(nRet,true,false);
	}
	operator sqlite3*() 
	{
		return m_pDB;
	}

};


//数据库查询语句类的封装
class CStmtCM
{
protected:
	sqlite3_stmt * m_pStmt;
	bool m_bFreeOnEnd ;
public:
	CStmtCM(sqlite3* db,const char* sql,bool bFreeOnEnd = true):m_pStmt(NULL)
	{
		Prepare(db,sql,bFreeOnEnd);;
	}

	CStmtCM()
	{
		m_bFreeOnEnd  = true;
		m_pStmt = NULL;
	}
	CStmtCM(sqlite3_stmt * pStmt,bool bFreeOnEnd = true)
	{
		m_pStmt = pStmt;
		m_bFreeOnEnd  = bFreeOnEnd ;
	}

	bool Prepare(sqlite3* db,const char* sql,bool bFreeOnEnd = true)
	{
		Close();
		if(SQLITE_OK != sqlite3_prepare(db,sql,strlen(sql),&m_pStmt,NULL))
			m_pStmt = NULL;

		m_bFreeOnEnd  = bFreeOnEnd ;
		return NULL != m_pStmt;
	}
	operator sqlite3_stmt*() 
	{
		return m_pStmt;
	}

	~CStmtCM()
	{
		if(!m_bFreeOnEnd)
			return;

		Close();
	}

	void Close()
	{
		if(m_pStmt == NULL)
			return;

		sqlite3_finalize(m_pStmt);
		m_pStmt  = NULL;
	}
	bool IsNull()
	{
		return m_pStmt == NULL;
	}
	int RecordCount()
	{
		return sqlite3_data_count(m_pStmt);
	}
	int Step()
	{
		if(NULL == m_pStmt)
			return -1;
		int nRet = sqlite3_step(m_pStmt); 
		return nRet;
	}
	int IntValue(int nCol)
	{
		return sqlite3_column_int(m_pStmt,nCol);
	}
	__int64 Int64Value(int nCol)
	{
		return sqlite3_column_int64(m_pStmt,nCol);
	}
	std::string StringValue(int nCol)
	{
		return std::string((const char*)sqlite3_column_text(m_pStmt,nCol));
	}
	std::string StringValue16(int nCol)
	{
		return std::string((const char*)sqlite3_column_text16(m_pStmt,nCol));
	}
	const unsigned char* BlobValue(int nCol)
	{
		return (unsigned char*)sqlite3_column_blob(m_pStmt,nCol);
	}
	long ValueLength(int nCol)
	{
		return sqlite3_column_bytes(m_pStmt,nCol);
	}
	double DoubleValue(int nCol)
	{
		return sqlite3_column_double(m_pStmt,nCol);
	}
	int Reset()
	{
		return sqlite3_reset(m_pStmt);
	}
	//绑定blob
	int Bind(int nIndex,const unsigned char* pBlob ,int nLen)
	{
		return sqlite3_bind_blob(m_pStmt,nIndex,pBlob,nLen,NULL);
	}

	//绑定integer
	int Bind(int nIndex,int nValue)
	{
		return sqlite3_bind_int(m_pStmt,nIndex,nValue);
	}

	//绑定double
	int Bind(int nIndex,double nValue)
	{
		return sqlite3_bind_double(m_pStmt,nIndex,nValue);
	}
	//绑定integer
	int Bind(int nIndex,unsigned __int64 nValue)
	{
		return Bind(nIndex,(__int64)nValue);
	}
	//绑定integer
	int Bind(int nIndex,__int64 nValue)
	{
		return sqlite3_bind_int64(m_pStmt,nIndex,nValue);
	}

	//绑定integer
	int Bind(int nIndex)
	{
		return sqlite3_bind_null(m_pStmt,nIndex);
	}

	//绑定integer
	int Bind(int nIndex,const char* pStr)
	{
		return sqlite3_bind_text(m_pStmt,nIndex,pStr,strlen(pStr),NULL);
	}
	/*int Bind(int nIndex,const char* pStr)
	{
	return sqlite3_bind_text(m_pStmt,nIndex,pStr,-1,NULL);
	}*/
	std::string ColumnCountName(int n)
	{
		const char* pName = sqlite3_column_name(m_pStmt,n);
		if(NULL == pName)
			return std::string();
		return pName;
	}

	int ColumnCount()
	{
		return sqlite3_column_count(m_pStmt);
	}
};

//要素图层的元信息存储类
struct GeoFeatureMetatableCM
{
	GeoFeatureMetatableCM()
	{
		init();
	}
	std::string name;
	std::string AliasName;
	unsigned int Endian;
	int EPSG;
	std::string SpatialReference;
	int FeatureType;
	int GeometryType;
	int Scale;
	double XTolerance;
	double YTolerance;

	double XMin;
	double XMax;
	double YMin;
	double YMax;

	bool set;

	void init()
	{
		name="EmptyFea";
		AliasName="EmptyFea";
		Endian=0;
		SpatialReference="";
		FeatureType=0;
		GeometryType=0;
		Scale=4000000;
		XTolerance=5e-7;
		YTolerance=5e-7;
		//FiledsInfo="";
		XMax=180.0;
		XMin=-180.0;
		YMax=90.0;
		YMin=-90.0;
		set=false;
	}
};

//吉奥几何体类
class GEOGeometryHCM
{
public:
	GEOGeometryHCM()
	{
		_infoData=NULL;//三元组数据
		_coordData=NULL;//坐标数据
	}
	~GEOGeometryHCM()
	{

	}

	void setGeomType(geoGEOMETRYTYPE1 gtype)//设置几何体对象的类型
	{
		_BlockHead.type=gtype;
	}
	int getGeomType()//获取几何体对象的类型
	{
		return _BlockHead.type;
	}

	int getNumPoints()//获取点的个数
	{
		return _BlockHead.coordnum/_BlockHead.dim;//点的个数不是坐标个数
	}
	int getPartPoints(int p)//从0开始计算part，三元组的个数,每个部分点个数，不是坐标个数
	{

		//获取每一部分的点的个数
		if (p==getNumInfo()-1)//最后一个三元组
		{
			return (_BlockHead.coordnum-_infoData[3*p]+1)/_BlockHead.dim;
		}
		else
		{
			return (_infoData[3*p+3]-_infoData[3*p])/_BlockHead.dim;
		}

	}
	int getPartType(int p)//获取几何对象某个部分的类型
	{
		return _infoData[3*p+1];
	}
	int getPartOffset(int p)//获取几何对象某个部分的坐标偏移值
	{
		return (_infoData[p*3]-1)/_BlockHead.dim;
	}
	int getNumInfo()//获得三元组的个数，即几何对象的组成部分个数
	{
		return _BlockHead.infonum/3;//infodata为一个三元组，三元组的个数
	}


	void getGeoPoint(int v,double& x,double& y,double& z)//根据索引值获取点的坐标
	{
		if (_BlockHead.dim==2)
		{
			z=0.0;
			x=_coordData[2*v];
			y=_coordData[2*v+1];
		}
		if (_BlockHead.dim==3)
		{
			z=_coordData[3*v+2];
			x=_coordData[3*v];
			y=_coordData[3*v+1];
		}
	}
	osg::Vec2d getGeoPoint(int v)//根据索引值获取点的坐标
	{
		osg::Vec2d p;
		p.x()=_coordData[2*v];
		p.y()=_coordData[2*v+1];
		return p;
	}
	bool valid()
	{
		return _bValid;
	}

	fcscmf_blob_head _BlockHead;
	long* _infoData;
	double* _coordData;
	bool _bValid;
};

class GEOFeatureHCM//吉奥要素类
{
public:
	GEOFeatureHCM()
	{
		_oid=0;
		_bValid=false;
		_geomH=new GEOGeometryHCM;//新建吉奥几何体类
	}
	~GEOFeatureHCM()
	{
		clear();
	}

	void clear()
	{
		_bValid=false;
		if (_geomH)
		{
			if (_geomH->_infoData!=NULL)
			{
				delete[] _geomH->_infoData;
				_geomH->_infoData=NULL;
			}
			if (_geomH->_coordData!=NULL)
			{
				delete[] _geomH->_coordData;
				_geomH->_coordData=NULL;
			}
			delete _geomH;
			_geomH=NULL;
		}
	}
	void create()
	{

	}
	//oid
	long getOid()//得到要素类的oid号
	{
		return _oid;
	}
	void setOid(long oid)
	{
		_oid=oid;
	}
	long _oid;
	//geometry
	GEOGeometryHCM* getGeomH()
	{
		return _geomH;
	}
	GEOGeometryHCM* _geomH;
	bool valid()
	{
		return _bValid;
	}
	bool _bValid;
};

class FcsData
{
public:
	FcsData()
	{

	}

	//链接指定数据库
	void linkSqlDB(std::string dbPath)
	{
		_dbPath=dbPath;
		sqlite3* db;
		sqlite3_open(dbPath.c_str(),&db);
		_mDB.Attach(db);

/*
		//处理元表
		initMetaTable(_geoFeaMetatable);//初始化要素图层元信息*/

		_featureCount=0;//特征个数
	}

	bool getoids(std::string sql,std::vector<int>& oids)
	{
		_mStmt.Reset();
		if (!_mStmt.Prepare(_mDB.DB(),sql.c_str()))
		{
			return false;
		}
		while(_mStmt.Step()==SQLITE_ROW)
		{
			int oid=_mStmt.IntValue(0);
			oids.push_back(oid);
		}

		if (oids.size()>0)
		{
			return true;
		}
		return false;
	}

	bool getCMFids(std::string sql,std::vector<float>& ids)
	{
		_mStmt.Reset();
		if (!_mStmt.Prepare(_mDB.DB(),sql.c_str()))
		{
			return false;
		}
		while(_mStmt.Step()==SQLITE_ROW)
		{
			float id=_mStmt.IntValue(0);
			ids.push_back(id);
		}

		if (ids.size()>0)
		{
			return true;
		}
		return false;
	}
	
	//根据要素的oid号创建相应要素
	bool getChunkData(int oid,GEOFeatureHCM* geoFeaH)
	{
		_mStmt.Reset();
		std::string strsql="select * from datatable where oid=?";
		if (_mStmt.Prepare(_mDB.DB(),strsql.c_str()))
		{
			_mStmt.Bind(1,oid);
			if (_mStmt.Step()==SQLITE_ROW)
			{
					geoFeaH->setOid(oid);

					unsigned char* geometrydata=(unsigned char*)_mStmt.BlobValue(1);//获取oid矢量的坐标块的内存地址

					if (createGEOGeometryH(geometrydata,geoFeaH->getGeomH()))
					{
						geoFeaH->_bValid=true;
						return true;
					}

				return false;
			}

		}
	return false;

	}

	////////////////////////////////////////////////////////////字节序转换函数
 void ntohl_BlobHead(fcscmf_blob_head& bhead)
	{
		bhead.oid=ntohl(bhead.oid);
		bhead.oid2=ntohl(bhead.oid2);
		bhead.type=(geoGEOMETRYTYPE1)ntohl(bhead.type);
		bhead.dim=ntohl(bhead.dim);
		bhead.infonum=ntohl(bhead.infonum);
		bhead.coordnum=ntohl(bhead.coordnum);
	}
 LONGLONG ntoh_int64(LONGLONG val)
	{
		LONGLONG ret=val;
#if __BYTE_ORDER == __LITTLE_ENDIAN
		ret=(((LONGLONG)ntohl((unsigned long)val))<<32| ntohl((unsigned long)(val>>32)));
#endif;
		return ret;
	}
 double ntoh_double(double val)
	{
		double ret=val;
#if __BYTE_ORDER == __LITTLE_ENDIAN
		LONGLONG tmp=ntoh_int64(*((LONGLONG*)&val));
		ret=*((double*)&tmp);
#endif;
		return ret;
	}
	///////////////////////////////////////////////////////////////////////////

	//根据二进制块创建该要素对应的几何体
	bool createGEOGeometryH(unsigned char* geoData,GEOGeometryHCM* geoGeometryH)
	{
		if (geoData==NULL)
		{
			return false;
		}
		memcpy(&(geoGeometryH->_BlockHead),geoData,24);//24字节
		ntohl_BlobHead(geoGeometryH->_BlockHead);//网络顺序和主机顺序

		fcscmf_blob_head bh=geoGeometryH->_BlockHead;

		//获取几何体的三元组信息
		geoGeometryH->_infoData=new long[bh.infonum];
		memcpy(geoGeometryH->_infoData,geoData+24,bh.infonum*4);
		for (long i=0;i<bh.infonum;i++)
		{
			geoGeometryH->_infoData[i]=ntohl(geoGeometryH->_infoData[i]);
		}

		//获取几何体的坐标信息
		geoGeometryH->_coordData=new double[bh.coordnum];
		memcpy(geoGeometryH->_coordData,geoData+24+bh.infonum*4,bh.coordnum*8);
		for(long j=0;j<bh.coordnum;j++)
		{
			geoGeometryH->_coordData[j]=ntoh_double(geoGeometryH->_coordData[j]);
		}
		geoGeometryH->_bValid=true;
		return true;
	}

	void populate( GEOGeometryHCM* geomHandle, osg::ref_ptr<osg::Vec3dArray>  target, int numIndex ,int numPoints,float height)
	{
		for( int v=numIndex;v<numIndex+numPoints; v=v+1 ) 
		{
			double x=0, y=0, z=0;
			geomHandle->getGeoPoint(v,x,y,z);
			osg::Vec3d p( x, y, height);
			if ( target->size() == 0 || p != target->back() ) 
				target->push_back( p );
		}
	}

	bool ReadTileRangeFromFCSQL(TileMetaData& metadata)
	{
		if (!_mDB) return false;

		sqlite3_stmt* stmt;

		std::string sql;

		sql="select value from metatable where name='XMin'";
		if (SQLITE_OK==sqlite3_prepare(_mDB,sql.c_str(),-1,&stmt,0))
		{
			while(sqlite3_step(stmt)==SQLITE_ROW)
			{
				metadata.bbox.west=sqlite3_column_double(stmt,0);
			}
		}

		sql="select value from metatable where name='XMax'";
		if (SQLITE_OK==sqlite3_prepare(_mDB,sql.c_str(),-1,&stmt,0))
		{
			while(sqlite3_step(stmt)==SQLITE_ROW)
			{
				metadata.bbox.east=sqlite3_column_double(stmt,0);
			}
		}

		sql="select value from metatable where name='YMin'";
		if (SQLITE_OK==sqlite3_prepare(_mDB,sql.c_str(),-1,&stmt,0))
		{
			while(sqlite3_step(stmt)==SQLITE_ROW)
			{
				metadata.bbox.north=sqlite3_column_double(stmt,0);
			}
		}

		sql="select value from metatable where name='YMax'";
		if (SQLITE_OK==sqlite3_prepare(_mDB,sql.c_str(),-1,&stmt,0))
		{
			while(sqlite3_step(stmt)==SQLITE_ROW)
			{
				metadata.bbox.south=sqlite3_column_double(stmt,0);
			}
		}

		sql="select value from metatable where name='Name'";
		if (SQLITE_OK==sqlite3_prepare(_mDB,sql.c_str(),-1,&stmt,0))
		{
			while(sqlite3_step(stmt)==SQLITE_ROW)
			{
				metadata.strDataSetName=(char*)sqlite3_column_text(stmt,0);
			}
		}

		sqlite3_finalize(stmt);
		return true;
	}


	std::string _dbPath;
	CSqliteDBCM _mDB;
	CStmtCM _mStmt;
	GeoFeatureMetatableCM _geoFeaMetatable;
	int _featureCount;

};

#endif
