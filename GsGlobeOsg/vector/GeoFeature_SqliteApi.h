#ifndef OSGEARTHFEATURES_FEATURE_GEOFEATURE_SQLITE
#define OSGEARTHFEATURES_FEATURE_GEOFEATURE_SQLITE 1
#include "Geo_Geometry_Common.h"
#include <sqlite3.h>
#include <iostream>
#include <osgEarth/XmlUtils>
#include <osgEarth/Config>
//#include "stdafx.h"
//#include <WinSock2.h>
//#pragma comment(lib,"Ws2_32.lib")
#include <utility.h>
namespace GeoGlobe {
	namespace GeoVector 
	{
		//数据库文件操作类，包括事务的提交、数据库语句执行
		class CSqliteDB
		{
			long m_CommitCount;
			long m_nMaxCount;
		public:
			sqlite3 * m_pDB;
			CSqliteDB():m_pDB(NULL)
			{
				m_CommitCount = -1;
				m_nMaxCount = 1000; 

			}
			virtual ~CSqliteDB()
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
		class CStmt
		{
		protected:
			sqlite3_stmt * m_pStmt;
			bool m_bFreeOnEnd ;
		public:
			CStmt(sqlite3* db,const char* sql,bool bFreeOnEnd = true):m_pStmt(NULL)
			{
				Prepare(db,sql,bFreeOnEnd);;
			}

			CStmt()
			{
				m_bFreeOnEnd  = true;
				m_pStmt = NULL;
			}
			CStmt(sqlite3_stmt * pStmt,bool bFreeOnEnd = true)
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

			~CStmt()
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

		//metatable
		//要素图层的元信息存储类
		struct GeoFeatureMetatable
		{
			GeoFeatureMetatable()
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

			//std::string FiledsInfo;
			osgEarth::Config FiledsInfo;

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
		class GEOGeometryH
		{
		public:
			GEOGeometryH()
			{
				_infoData=NULL;//三元组数据
				_coordData=NULL;//坐标数据
			}
			~GEOGeometryH()
			{
				
			}

			void setGeomType(geoGEOMETRYTYPE gtype)//设置几何体对象的类型
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

			st_blob_head _BlockHead;
			long* _infoData;
			double* _coordData;
			bool _bValid;
		};
		class GEOFeatureH//吉奥要素类
		{
		public:
			GEOFeatureH()
			{
				_oid=0;
				_bValid=false;
				_geomH=new GEOGeometryH;//新建吉奥几何体类
			}
			~GEOFeatureH()
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
			GEOGeometryH* getGeomH()
			{
				return _geomH;
			}
			GEOGeometryH* _geomH;
			bool valid()
			{
				return _bValid;
			}
			bool _bValid;
		};
		
		/*class GEOFeatureFieldH
		{

		};*/

		class GEOFeatureClass//对应存储吉奥要素的文件类
		{
		public:
			//构造函数与数据库相连
			GEOFeatureClass() 
			{

			}
			~GEOFeatureClass()
			{
				
			}
			/////////////////////////
			//初始化要素图层元信息
			bool initMetaTable(GeoFeatureMetatable& geofmt)
			{
				if (!geofmt.set)
				{
					if (readMetaTable(geofmt))
					{
						geofmt.set=true;//读取元表成功
					}
				}
				return geofmt.set;
			}
			//从本地数据库文件读取要素的元信息，包括其经纬度范围等信息
			bool readMetaTable(GeoFeatureMetatable& geofmt)
			{
				_mStmt.Reset();
				//std::string sql="select * from sqlite_master where type='table' and name = 'metatable'";
				//if (_mStmt.Prepare(_mDB.DB(),sql.c_str()))
				//{
				//	//判断表是否存在
				//	if (_mStmt.RecordCount()!=0)
				//	{
				//		//表格存在

						std::string sql="select * from metatable where name='Name'";
						if (_mStmt.Prepare(_mDB.DB(),sql.c_str()))
						{
							//_mStmt.Bind(1,"Name");
							if (_mStmt.Step()==SQLITE_ROW)
							{
								geofmt.name=_mStmt.StringValue(1);
							}
						}

						_mStmt.Reset();
						sql="select * from metatable where name='AliasName'";
						if (_mStmt.Prepare(_mDB.DB(),sql.c_str()))
						{
							//_mStmt.Bind(1,"AliasName");
							if (_mStmt.Step()==SQLITE_ROW)
							{
								geofmt.AliasName=_mStmt.StringValue(1);
							}
						}

						_mStmt.Reset();
						sql="select * from metatable where name='EPSG'";
						if (_mStmt.Prepare(_mDB.DB(),sql.c_str()))
						{
							//_mStmt.Bind(1,"AliasName");
							if (_mStmt.Step()==SQLITE_ROW)
							{
								geofmt.EPSG=_mStmt.IntValue(1);
							}
						}

						_mStmt.Reset();
						sql="select * from metatable where name='Endian'";
						if (_mStmt.Prepare(_mDB.DB(),sql.c_str()))
						{
							//_mStmt.Bind(1,"AliasName");
							if (_mStmt.Step()==SQLITE_ROW)
							{
								geofmt.Endian=_mStmt.IntValue(1);
							}
						}

						_mStmt.Reset();
						sql="select * from metatable where name='SpatialReference'";
						if (_mStmt.Prepare(_mDB.DB(),sql.c_str()))
						{
							//_mStmt.Bind(1,"AliasName");
							if (_mStmt.Step()==SQLITE_ROW)
							{
								geofmt.SpatialReference=_mStmt.StringValue(1);
							}
						}


						_mStmt.Reset();
						sql="select * from metatable where name='FeatureType'";
						if (_mStmt.Prepare(_mDB.DB(),sql.c_str()))
						{
							//_mStmt.Bind(1,"AliasName");
							if (_mStmt.Step()==SQLITE_ROW)
							{
								geofmt.FeatureType=_mStmt.IntValue(1);
								
							}
						}
						
						
						_mStmt.Reset();
						sql="select * from metatable where name='GeometryType'";
						if (_mStmt.Prepare(_mDB.DB(),sql.c_str()))
						{
							//_mStmt.Bind(1,"AliasName");
							if (_mStmt.Step()==SQLITE_ROW)
							{
								geofmt.GeometryType=_mStmt.IntValue(1);
							}
						}
						_mStmt.Reset();
						sql="select * from metatable where name='Scale'";
						if (_mStmt.Prepare(_mDB.DB(),sql.c_str()))
						{
							//_mStmt.Bind(1,"AliasName");
							if (_mStmt.Step()==SQLITE_ROW)
							{
								geofmt.Scale=_mStmt.IntValue(1);
							}
						}
						_mStmt.Reset();
						sql="select * from metatable where name='XTolerance'";
						if (_mStmt.Prepare(_mDB.DB(),sql.c_str()))
						{
							//_mStmt.Bind(1,"AliasName");
							if (_mStmt.Step()==SQLITE_ROW)
							{
								geofmt.XTolerance=_mStmt.DoubleValue(1);
							}
						}
							
						_mStmt.Reset();
						sql="select * from metatable where name='YTolerance'";
						if (_mStmt.Prepare(_mDB.DB(),sql.c_str()))
						{
							//_mStmt.Bind(1,"AliasName");
							if (_mStmt.Step()==SQLITE_ROW)
							{
								geofmt.YTolerance=_mStmt.DoubleValue(1);
							}
						}
						_mStmt.Reset();
						sql="select * from metatable where name='FieldsInfo'";
						if (_mStmt.Prepare(_mDB.DB(),sql.c_str()))
						{
							//_mStmt.Bind(1,"AliasName");
							if (_mStmt.Step()==SQLITE_ROW)
							{
								std::stringstream buf;
								buf<<_mStmt.StringValue(1);
								geofmt.FiledsInfo.fromXML(buf);
								/*geofmt.FiledsInfo=_mStmt.StringValue(1);
								if (geofmt.FiledsInfo=="")
								{
									geofmt.FiledsInfo=_mStmt.StringValue16(1);
								}*/
							}
						}
						_mStmt.Reset();
						sql="select * from metatable where name='XMin'";
						if (_mStmt.Prepare(_mDB.DB(),sql.c_str()))
						{
							//_mStmt.Bind(1,"AliasName");
							if (_mStmt.Step()==SQLITE_ROW)
							{
								geofmt.XMin=_mStmt.DoubleValue(1);
							}
						}	
						_mStmt.Reset();
						sql="select * from metatable where name='YMin'";
						if (_mStmt.Prepare(_mDB.DB(),sql.c_str()))
						{
							//_mStmt.Bind(1,"AliasName");
							if (_mStmt.Step()==SQLITE_ROW)
							{
								geofmt.YMin=_mStmt.DoubleValue(1);
							}
						}	
						_mStmt.Reset();
						sql="select * from metatable where name='XMax'";
						if (_mStmt.Prepare(_mDB.DB(),sql.c_str()))
						{
							//_mStmt.Bind(1,"AliasName");
							if (_mStmt.Step()==SQLITE_ROW)
							{
								geofmt.XMax=_mStmt.DoubleValue(1);
							}
						}	
						_mStmt.Reset();
						sql="select * from metatable where name='YMax'";
						if (_mStmt.Prepare(_mDB.DB(),sql.c_str()))
						{
							if (_mStmt.Step()==SQLITE_ROW)
							{
								geofmt.YMax=_mStmt.DoubleValue(1);
							}
						}		
							return true;
						//if (_mStmt.Prepare(_mDB.DB(),sql.c_str()))
				//	}//if (_mStmt.RecordCount()!=0)

				//}//if (_mStmt.Prepare(_mDB.DB(),sql.c_str()))
				
				
				//return false;
			}
			//////////////////////////////////////////

			//该要素图层类初始化，包括获取元信息，设定要素的个数为0
			void init(std::string dbPath)
			{
				_dbPath=dbPath;
				sqlite3* db;
				sqlite3_open(dbPath.c_str(),&db);
				_mDB.Attach(db);
				//处理元表
				
				initMetaTable(_geoFeaMetatable);
				
				_featureCount=0;
			}

			//根据要素的oid号创建相应要素
			bool createGEOFeatureH(int oid,GEOFeatureH* geoFeaH,bool bcreateGEOGeometry=true)
			{
				_mStmt.Reset();
				std::string strsql="select * from datatable where oid=?";
				if (_mStmt.Prepare(_mDB.DB(),strsql.c_str()))
				{
					_mStmt.Bind(1,oid);
					if (_mStmt.Step()==SQLITE_ROW)
					{
						geoFeaH->setOid(oid);
					    if (bcreateGEOGeometry)
					    {
							unsigned char* geometrydata=(unsigned char*)_mStmt.BlobValue(1);

							if (createGEOGeometryH(geometrydata,geoFeaH->getGeomH()))
							{
								geoFeaH->_bValid=true;
								return true;
							}
							
					    }
						return false;
					}//if (_mStmt.Step()==SQLITE_ROW)
					
				}//if (_mStmt.Prepare(_mDB.DB(),strsql.c_str()))
				return false;

			}
            int ntohl(int n)
            {
                return GeoStar::Utility::GsEndianConverter::Swap((unsigned int)n);
            }

			////////////////////////////////////////////////////////////字节序转换函数
			inline void ntohl_BlobHead(st_blob_head& bhead)
			{
				bhead.oid=ntohl(bhead.oid);
				bhead.oid2=ntohl(bhead.oid2);
				bhead.type=(geoGEOMETRYTYPE)ntohl(bhead.type);
				bhead.dim=ntohl(bhead.dim);
				bhead.infonum=ntohl(bhead.infonum);
				bhead.coordnum=ntohl(bhead.coordnum);
			}
            inline long long ntoh_int64(long long val)
			{
                return GeoStar::Utility::GsEndianConverter::Swap((unsigned long long)val);
			}
			inline double ntoh_double(double val)
			{
                return GeoStar::Utility::GsEndianConverter::Swap(val);
			}
			///////////////////////////////////////////////////////////////////////////

			//根据二进制块创建该要素对应的几何体
			bool createGEOGeometryH(unsigned char* geoData,GEOGeometryH* geoGeometryH)
			{
				if (geoData==NULL)
				{
					return false;
				}
				memcpy(&(geoGeometryH->_BlockHead),geoData,24);
				ntohl_BlobHead(geoGeometryH->_BlockHead);

				st_blob_head bh=geoGeometryH->_BlockHead;

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

			//获取要素的总个数
			int getFeatureCount()
			{
				
				_mStmt.Reset();
				std::string strsql="select count(*) from datatable";
				if (_mStmt.Prepare(_mDB.DB(),strsql.c_str()))
				{
					while (_mStmt.Step()==SQLITE_ROW)
					{
						_featureCount=_mStmt.IntValue(0);
						return _featureCount;
					}
				}
				return 0;
			}

			//获取与该范围相交的要素oid编号
			bool excuteSeletSqlandQuerytoGetOID(osgEarth::Bounds bounds,std::string sql,std::vector<int>& oids)
			{
				_mStmt.Reset();
				if (!_mStmt.Prepare(_mDB.DB(),sql.c_str()))
				{
					return false;
				}
				while(_mStmt.Step()==SQLITE_ROW)
				{
					int oid=_mStmt.IntValue(0);
					//oids.push_back(oid);
					osgEarth::Bounds bds=osgEarth::Bounds(_mStmt.DoubleValue(1),_mStmt.DoubleValue(2),_mStmt.DoubleValue(3),_mStmt.DoubleValue(4));
					geoGEOMETRYTYPE geoType = (geoGEOMETRYTYPE)_geoFeaMetatable.GeometryType;
					if (geoType == GEO_GEOMETRY_POLYGON)
					{
						if (bds.intersects(bounds) /*&& !bds.contains(bounds)*/)
						{
							/*if (bds.area2d()<=bounds.area2d())
							{*/
								oids.push_back(oid);
							/*}*/
						}
					}
					else if (geoType == GEO_GEOMETRY_PATH || geoType==GEO_GEOMETRY_LINE)
					{
						if (bds.intersects(bounds) /*&& !bds.contains(bounds)*/)
						{
						    oids.push_back(oid);
						}
					}
					else if (geoType == GEO_GEOMETRY_RING )
					{
						if (bds.intersects(bounds) /*&& !bds.contains(bounds)*/)
						{
							oids.push_back(oid);
						}
					}
					else if ( geoType == GEO_GEOMETRY_POINT)
					{
						if (bds.intersects(bounds) /*&& !bds.contains(bounds)*/)
						{
							oids.push_back(oid);
						}
					}
					else if (
						geoType == GEO_GEOMETRY_COLLECTION ||
						geoType == GEO_GEOMETRY_MULTIPOINT ||
						geoType == GEO_GEOMETRY_POLYLINE )
					{
						if (bds.intersects(bounds) /*&& !bds.contains(bounds)*/)
						{
							oids.push_back(oid);
						}
					}
					
				}
				if (oids.size()>0)
				{
					return true;
				}
				return false;
			}

			//获取包含该范围内的要素的oid
			bool containPickpoint(osgEarth::Bounds bounds,std::string sql,std::vector<int>& oids)
			{
				//_mStmt.Reset();
				if (!_mStmt.Prepare(_mDB.DB(),sql.c_str()))
				{
					return false;
				}
				while(_mStmt.Step()==SQLITE_ROW)
				{
					int oid=_mStmt.IntValue(0);
					//oids.push_back(oid);
					osgEarth::Bounds bds=osgEarth::Bounds(_mStmt.DoubleValue(1),_mStmt.DoubleValue(2),_mStmt.DoubleValue(3),_mStmt.DoubleValue(4));
					if (bds.contains(bounds))
					{
						oids.push_back(oid);
					}

				}
				if (oids.size()>0)
				{
					return true;
				}
				return false;
			}
			//判断该要素的包围盒是否与某范围相交
			bool bOidIntersectObjectBound(int oid,osgEarth::Bounds bounds)
			{
				_mStmt.Reset();
				std::string sql="select oid,xmin,ymin,xmax,ymax from dataindex where oid=?";
				if (_mStmt.Prepare(_mDB.DB(),sql.c_str()))
				{
					_mStmt.Bind(1,oid);
					if (_mStmt.Step()==SQLITE_ROW)
					{
						osgEarth::Bounds bds=osgEarth::Bounds(_mStmt.DoubleValue(1),_mStmt.DoubleValue(2),_mStmt.DoubleValue(3),_mStmt.DoubleValue(4));
						if (bds.intersects(bounds))
						{
							return true;
						}
					}
					return false;
				}
				return false;
			}

			//判断该要素的包围盒是否包含某范围
			bool bOidContainObjectBound(int oid,osgEarth::Bounds bounds)
			{
				_mStmt.Reset();
				std::string sql="select oid,xmin,ymin,xmax,ymax from dataindex where oid=?";
				if (_mStmt.Prepare(_mDB.DB(),sql.c_str()))
				{
					_mStmt.Bind(1,oid);
					if (_mStmt.Step()==SQLITE_ROW)
					{
						osgEarth::Bounds bds=osgEarth::Bounds(_mStmt.DoubleValue(1),_mStmt.DoubleValue(2),_mStmt.DoubleValue(3),_mStmt.DoubleValue(4));
						if (bds.contains(bounds))
						{
							return true;
						}
					}
					return false;
				}
				return false;
			}

			//判断某范围是否包含该要素的包围盒
			bool bOidInObjectBound(int oid,osgEarth::Bounds bounds)
			{
				_mStmt.Reset();
				std::string sql="select oid,xmin,ymin,xmax,ymax from dataindex where oid=?";
				if (_mStmt.Prepare(_mDB.DB(),sql.c_str()))
				{
					_mStmt.Bind(1,oid);
					if (_mStmt.Step()==SQLITE_ROW)
					{
						osgEarth::Bounds bds=osgEarth::Bounds(_mStmt.DoubleValue(1),_mStmt.DoubleValue(2),_mStmt.DoubleValue(3),_mStmt.DoubleValue(4));
						if (bounds.contains(bds))
						{
							return true;
						}
					}
					return false;
				}
				return false;
			}

			//根据要素的oid号获取该要素的包围盒
			bool getOidBoundBox(int oid,osgEarth::Bounds& bds)
			{
				_mStmt.Reset();
				std::string sql="select oid,xmin,ymin,xmax,ymax from dataindex where oid=?";
				if (_mStmt.Prepare(_mDB.DB(),sql.c_str()))
				{
					_mStmt.Bind(1,oid);
					if (_mStmt.Step()==SQLITE_ROW)
					{
						bds=osgEarth::Bounds(_mStmt.DoubleValue(1),_mStmt.DoubleValue(2),_mStmt.DoubleValue(3),_mStmt.DoubleValue(4));
						return true;
					}
					return false;
				}
				return false;
			}

			//根据oid号获取featureid
            int getOidfromFieldFeatureID(long long feature_id)
			{
				std::string sql="select oid from datatable where featureid=?";
				_mStmt.Reset();
				if (!_mStmt.Prepare(_mDB.DB(),sql.c_str()))
				{
					sql="select oid from datatable where feature_id=?";
					_mStmt.Reset();
					if (!_mStmt.Prepare(_mDB.DB(),sql.c_str()))
					{
						return -1;
					}
				}
				_mStmt.Bind(1,feature_id);
				if(_mStmt.Step()==SQLITE_ROW)
				{
					return _mStmt.IntValue(0);

				}
				return -1;
			}

			//根据要素oid获取其他字段的属性，包括int，string，double类型
			int excuteFieldSQLint(std::string fieldname,int oid)
			{

				std::string sql="select "+fieldname+" from datatable where oid=?";
				_mStmt.Reset();
				if (!_mStmt.Prepare(_mDB.DB(),sql.c_str()))
				{
					return -1;
				}
				_mStmt.Bind(1,oid);
				if(_mStmt.Step()==SQLITE_ROW)
				{
					return _mStmt.IntValue(0);

				}
				return -1;
			}
			std::string excuteFieldSQLstring(std::string fieldname,int oid)
			{

				std::string sql="select "+fieldname+" from datatable where oid=?";
				_mStmt.Reset();
				if (!_mStmt.Prepare(_mDB.DB(),sql.c_str()))
				{
					return "";
				}
				_mStmt.Bind(1,oid);
				if(_mStmt.Step()==SQLITE_ROW)
				{
					return _mStmt.StringValue(0);

				}
				return "";
			}
			double excuteFieldSQLdouble(std::string fieldname,int oid)
			{
				std::string sql="select "+fieldname+" from datatable where oid=?";
				_mStmt.Reset();
				if (!_mStmt.Prepare(_mDB.DB(),sql.c_str()))
				{
					return -1;
				}
				_mStmt.Bind(1,oid);
				if(_mStmt.Step()==SQLITE_ROW)
				{
					return _mStmt.DoubleValue(0);

				}
				return -1;
			}


			////////////////////////////////////////////////////////
			std::string getdbPath()
			{
				return _dbPath;
			}
			//////////////////////////////////////////////////////////////
			std::string _dbPath;
			CSqliteDB _mDB;
			CStmt _mStmt;
			GeoFeatureMetatable _geoFeaMetatable;
			int _featureCount;
			
		};
	}//geovector
}//geoglobe



#endif//OSGEARTHFEATURES_FEATURE_SQLITE
