#include "StdAfx.h"
#include "ShapefileR.h"


CShapefileR::CShapefileR(void)
{
	_AllBox=new ShpBox;
	_VectorNums=new int;
	*_VectorNums=0;
}

CShapefileR::~CShapefileR(void)
{

}

//交换位序
int CShapefileR::OnChangeByteOrder(int indata)  
{  
    char ss[9];  
    char ee[8];  
    unsigned long val=unsigned long(indata);  
    ultoa(val,ss,16);// 将十六进制的数(val)转到一个字符串(ss)中,itoa(val,ss,16);  
    int i;  
    int length=strlen(ss);  
    if(length!=8)  
    {  
        for(i=0;i<8-length;i++)  
            ee[i]='0';  
        for(i=0;i<length;i++)  
            ee[i+8-length]=ss[i];  
        for(i=0;i<8;i++)  
            ss[i]=ee[i];  
    }  
    ////****** 进行倒序  
    int t;  
    t=ss[0];ss[0]=ss[6];ss[6]=t;  
    t=ss[1];ss[1]=ss[7];ss[7]=t;  
    t=ss[2];ss[2]=ss[4];ss[4]=t;  
    t=ss[3];ss[3]=ss[5];ss[5]=t;  
    ////******  
    //****** 将存有十六进制数 (val) 的字符串 (ss) 中的十六进制数转成十进制数  
    int value=0;  
    for(i=0;i<8;i++)  
    {  
        int k;  
        if(ss[i]=='a'||ss[i]=='b'||ss[i]=='c'||ss[i]=='d'||ss[i]=='e'||ss[i]=='f')  
            k=10+ss[i]-'a';  
        else  
            k=ss[i]-'0';  
        //printf("k=%d\n",k);  
        value=value+int(k*pow(16.0,7-i));  
    }  
    //printf("value=%d\n",value);  
    return(value);  
}
//数据处理，获取坐标
void CShapefileR::Read(FILE * ShpFile_fp)
{
		int i;  
		int FileCode;  
		int Unused;  
		int FileLength;  
		int Version;  
		int ShapeType;  
		double AXmin;
		double AYmin;
		double AXmax;
		double AYmax;
		double AZmin;
		double AZmax;
		double AMmin;
		double AMmax;
		// 打开坐标文件
		//FILE * ShpFile_fp;
		//if(ShpFile_fp=fopen((LPSTR)(LPCTSTR)fileName,"rb"))!=NULL)
		//{

			fread(&FileCode,sizeof(int),1,ShpFile_fp);
			FileCode = OnChangeByteOrder(FileCode);
//			printf("FileCode=%d\n",FileCode);

			for(i=0;i<5;i++)
			fread(&Unused,sizeof(int),1,ShpFile_fp);

			fread(&FileLength,sizeof(int),1,ShpFile_fp);//读取FileLength  
			FileLength=OnChangeByteOrder(FileLength);
//			printf("FileLength=%d\n",FileLength);

			fread(&Version,sizeof(int),1,ShpFile_fp);//读取Version的值  
//			printf("Version=%d\n",Version);

			fread(&ShapeType,sizeof(int),1,ShpFile_fp);//读取ShapeType的值  
//			printf("ShapeType=%d\n",ShapeType);

			fread(&AXmin,         sizeof(double),1,ShpFile_fp);
			_AllBox->Xmin=AXmin;
//			printf("Xmin=%lf\n",Xmin);

			fread(&AYmin,         sizeof(double),1,ShpFile_fp);
			_AllBox->Ymin=AYmin;
//			printf("Ymin=%lf\n",Ymin);

			fread(&AXmax,         sizeof(double),1,ShpFile_fp);
			_AllBox->Xmax=AXmax;
//			printf("Xmax=%lf\n",Xmax);

			fread(&AYmax,         sizeof(double),1,ShpFile_fp);
			_AllBox->Ymax=AYmax;
//			printf("Ymax=%lf\n",Ymax);

			fread(&AZmin,         sizeof(double),1,ShpFile_fp);
			_AllBox->Zmin=AZmin;
//			printf("Zmin=%lf\n",Zmin);

			fread(&AZmax,        sizeof(double),1,ShpFile_fp);
			_AllBox->Zmax=AZmax;
//			printf("Zmax=%lf\n",Zmax);

			fread(&AMmin,         sizeof(double),1,ShpFile_fp);
			_AllBox->Mmin=AMmin;
//			printf("Mmin=%lf\n",Mmin);

			fread(&AMmax,         sizeof(double),1,ShpFile_fp);
			_AllBox->Mmax=AMmax;
//			printf("Mmax=%lf\n",Mmax);
	//	}

		//判断shp文件类型（点线面）
		if (ShapeType==1)//点
		{
#if 1
			FILE * fp;  
			if((fp=fopen("../CmfData/shp/point.txt","w"))==NULL)  //新建一个txt文件    
			{  
				exit(0);  
			}  

			int RecordNumber;//记录号，big
			int ContentLength;//记录长度，big
			int num=0;
			while ((fread(&RecordNumber,sizeof(int),1,ShpFile_fp)!=0))//保证每个记录都检测到
			{
				fread(&ContentLength,sizeof(int),1,ShpFile_fp);
				RecordNumber=OnChangeByteOrder(RecordNumber);
				ContentLength=OnChangeByteOrder(ContentLength);

				_FShpDataVec.push_back(new ShpData);
				int shapeT;
				double x;
				double y;

                *_VectorNums+=1;
                *_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpNum=*_VectorNums;

				fread(&shapeT,sizeof(int),1,ShpFile_fp);
				fread(&x,sizeof(double),1,ShpFile_fp);
				fread(&y,sizeof(double),1,ShpFile_fp);

				fprintf(fp,"%.9lf",x);//写入点X坐标  
				fprintf(fp,",%.9lf",y);//写入点Y坐标  
				fprintf(fp,",%.9lf\n",0);//写入点Z坐标 

				_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpPointsArr->push_back( osg::Vec3(x,y,0));
			}		
			fclose(fp);

#endif	
		} 
		else if (ShapeType==3)//线
		{
#if 1
			// 读取线状目标的实体信息  
			FILE * fp;  
			if((fp=fopen("../CmfData/shp/polyline.txt","w"))==NULL)  //新建一个dxf文件    
			{  
				exit(0);  
			}  

			int i,j;  
			int RecordNumber;//记录号，big
			int ContentLength;//记录长度，big 
			int num=0;  
			while((fread(&RecordNumber,sizeof(int),1,ShpFile_fp)!=0))//从ShpFile_fp中以int型字节的长度，读取一次，并将其读取到RecordNumber中  
			{     
				//printf("转换前RecordNumber=%d\n",RecordNumber);  
				RecordNumber=OnChangeByteOrder(RecordNumber);//对RecordNumber进行转化后打印输出  
			//	printf("\n\nRecordNumber=%d\n",RecordNumber);  
				//fprintf(fp,"\n\nRecordNumber=%d\n",RecordNumber);  

				fread(&ContentLength,sizeof(int),1,ShpFile_fp);//从ShpFile_fp中以int型字节的长度，读取一次，并将其读取到ContentLength中  
				//printf("转换前ContentLength=%d\n",ContentLength);  
				ContentLength=OnChangeByteOrder(ContentLength);//对其进行转化 并输出  
			//	printf("ContentLength=%d\n",ContentLength);  
				//fprintf(fp,"ContentLength=%d\n",ContentLength);  

				_FShpDataVec.push_back(new ShpData);
				int shapeType;  
				double Box[4];  
				int NumParts;  
				int NumPoints;  
				int *Parts;  

				fread(&shapeType,sizeof(int),1,ShpFile_fp);//从ShpFile_fp中以int型字节的长度，读取一次，并将其读取到ContentLength中  
			//	printf("shapeType=%d\n",shapeType);//打印输出  
				//fprintf(fp,"shapeType=%d\n",shapeType);  

				for(i=0;i<4;i++)  
				{  
					fread(Box+i,sizeof(double),1,ShpFile_fp); // 读 Box  
			//		printf("Box+%d=%lf\n",i,*(Box+i));  
				}  

				//单个矢量边界
				_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpBox->Xmin=Box[0];
				_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpBox->Ymin=Box[1];
				_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpBox->Xmax=Box[2];
				_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpBox->Ymax=Box[3];


				fread(&NumParts,sizeof(int),1,ShpFile_fp);// 读 NumParts   
			//	printf("NumParts=%d\n",NumParts);  
				//fprintf(fp,"NumParts=%d\n",NumParts);  

				*_VectorNums+=1;
				*_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpNum=*_VectorNums;

				fread(&NumPoints,sizeof(int),1,ShpFile_fp);// 读NumPoints  
		//		printf("NumPoints=%d\n",NumPoints);  
				//fprintf(fp,"NumPoints=%d\n",NumPoints);  

				Parts=new int[NumParts];  
				for(i=0;i<NumParts;i++)  
				{  
					fread(Parts+i,sizeof(int),1,ShpFile_fp);  
		//			printf("Parts+%d=%d\n",i,*(Parts+i));  
				}  

				int pointNum;  
				for(i=0;i<NumParts;i++)  
				{  
					if(i!=NumParts-1)  
						pointNum = Parts[i+1]-Parts[i];  
					else  
						pointNum = NumPoints-Parts[i];  
					double *PointsX;  
					double *PointsY;  
					PointsX =new double[pointNum];  
					PointsY =new double[pointNum];  
					//读取坐标点  
					for(j=0;j<pointNum;j++)  
					{  
						fread(PointsX+j, sizeof(double),1,ShpFile_fp);//读取点的X坐标  
						fread(PointsY+j, sizeof(double),1,ShpFile_fp);//读取点的Y坐标    

						fprintf(fp,"%.9lf",*(PointsX+j));//写入点X坐标  
						fprintf(fp,",%.9lf",*(PointsY+j));//写入点Y坐标  
						fprintf(fp,",%.9lf\n",0);//写入点Z坐标  
						//------------------------------------------------------  
						_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpPointsArr->push_back( osg::Vec3(*(PointsX+j),*(PointsY+j),0));
					}  
					fprintf(fp,"\n",0);//与另一个元素空一行

					delete[] PointsX;  
					delete[] PointsY;  
				}  
				delete[] Parts;  
			}  
			//------------------------------------------------------  
			fclose(fp);  

#endif
		} 
		else if(ShapeType==5)//面
		{
#if 1
			// 读取线状目标的实体信息  
			FILE * fp;  
			if((fp=fopen("../CmfData/shp/Polygon.txt","w"))==NULL)  //新建一个dxf文件    
			{  
				exit(0);  
			}  

			int i,j;  
			int RecordNumber;//记录号，big
			int ContentLength;//记录长度，big
			int num=0;  
			while((fread(&RecordNumber,sizeof(int),1,ShpFile_fp)!=0))//从ShpFile_fp中以int型字节的长度，读取一次，并将其读取到RecordNumber中  
			{     
				//printf("转换前RecordNumber=%d\n",RecordNumber);  
				RecordNumber=OnChangeByteOrder(RecordNumber);//对RecordNumber进行转化后打印输出   

				fread(&ContentLength,sizeof(int),1,ShpFile_fp);//从ShpFile_fp中以int型字节的长度，读取一次，并将其读取到ContentLength中  
				//printf("转换前ContentLength=%d\n",ContentLength);  
				ContentLength=OnChangeByteOrder(ContentLength);//对其进行转化 并输出  

				_FShpDataVec.push_back(new ShpData);
				int shapeType;  
				double Box[4];  
				int NumParts;  
				int NumPoints;  
				int *Parts; 

				fread(&shapeType,sizeof(int),1,ShpFile_fp);//从ShpFile_fp中以int型字节的长度，读取一次，并将其读取到ContentLength中  

				for(i=0;i<4;i++)  
				{  
					fread(Box+i,sizeof(double),1,ShpFile_fp); // 读 Box   
				}  

				//单个矢量边界
				_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpBox->Xmin=Box[0];
				_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpBox->Ymin=Box[1];
				_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpBox->Xmax=Box[2];
				_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpBox->Ymax=Box[3];

				fread(&NumParts,sizeof(int),1,ShpFile_fp);// 读 NumParts   面数量
				*_VectorNums+=1;
				*_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpNum=*_VectorNums;

				fread(&NumPoints,sizeof(int),1,ShpFile_fp);// 读NumPoints 点数量
  

				Parts=new int[NumParts];  

				for(i=0;i<NumParts;i++)  
				{  
					fread(Parts+i,sizeof(int),1,ShpFile_fp);  
				}  

				int pointNum;  
				for(i=0;i<NumParts;i++)  
				{  
					if(i!=NumParts-1)  //如果不是最后一个面
						pointNum = Parts[i+1]-Parts[i];  //获取该面的点数（后面一个面的索引值减去该面的）
					else  
						pointNum = NumPoints-Parts[i];  
					double *PointsX;  
					double *PointsY;  
					PointsX =new double[pointNum];  
					PointsY =new double[pointNum];  
					//读取坐标点  

					for(j=0;j<pointNum;j++)  
					{  
						fread(PointsX+j, sizeof(double),1,ShpFile_fp);//读取点的X坐标  
						fread(PointsY+j, sizeof(double),1,ShpFile_fp);//读取点的Y坐标  

						fprintf(fp,"%.9lf",*(PointsX+j));//写入点X坐标  
						fprintf(fp,",%.9lf",*(PointsY+j));//写入点Y坐标  
						fprintf(fp,",%.9lf\n",0);//写入点Z坐标  

						_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpPointsArr->push_back( osg::Vec3(*(PointsX+j),*(PointsY+j),0));
					}  
					fprintf(fp,"\n",0);//与另一个元素空一行

					delete[] PointsX;  
					delete[] PointsY;  
				}  
				delete[] Parts;  
			}  

			fclose(fp);  
#endif
		}
		else if (ShapeType==11)//含有Z的点
		{
#if 1
			FILE * fp;  
			if((fp=fopen("../CmfData/shp/pointZ.txt","w"))==NULL)  //新建一个txt文件    
			{  
				exit(0);  
			}  

			int RecordNumber;//记录号，big
			int ContentLength;//记录长度，big
			int num=0;
			while ((fread(&RecordNumber,sizeof(int),1,ShpFile_fp)!=0))//保证每个记录都检测到
			{
				fread(&ContentLength,sizeof(int),1,ShpFile_fp);
				RecordNumber=OnChangeByteOrder(RecordNumber);
				ContentLength=OnChangeByteOrder(ContentLength);

				_FShpDataVec.push_back(new ShpData);//开辟新矢量空间
				int shapeT;
				double x;
				double y;
				double z;
				double m;

				*_VectorNums+=1;
				*_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpNum=*_VectorNums;

				fread(&shapeT,sizeof(int),1,ShpFile_fp);
				fread(&x,sizeof(double),1,ShpFile_fp);
				fread(&y,sizeof(double),1,ShpFile_fp);
				fread(&z,sizeof(double),1,ShpFile_fp);
				fread(&m,sizeof(double),1,ShpFile_fp);
				fprintf(fp,"%.9lf",x);//写入点X坐标  
				fprintf(fp,",%.9lf",y);//写入点Y坐标  
				fprintf(fp,",%.9lf\n",z);//写入点Z坐标  

				_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpPointsArr->push_back( osg::Vec3(x,y,z));

			}		
			fclose(fp);

#endif	
		}
		else if (ShapeType==13)//含有Z坐标的线段
		{
#if 1
			// 读取线状目标的实体信息  
			FILE * fp;  
			if((fp=fopen("../CmfData/shp/PolylineZ.txt","w"))==NULL)  //新建一个dxf文件    
			{  
				exit(0);  
			}  


			int i,j;  
			int RecordNumber;//记录号，big
			int ContentLength;//记录长度，big
			double Zmin;
			double Zmax;
			double Mmin;
			double Mmax;
			int num=0;  
			while((fread(&RecordNumber,sizeof(int),1,ShpFile_fp)!=0))//从ShpFile_fp中以int型字节的长度，读取一次，并将其读取到RecordNumber中  
			{     
				//printf("转换前RecordNumber=%d\n",RecordNumber);  
				RecordNumber=OnChangeByteOrder(RecordNumber);//对RecordNumber进行转化后打印输出  

				fread(&ContentLength,sizeof(int),1,ShpFile_fp);//从ShpFile_fp中以int型字节的长度，读取一次，并将其读取到ContentLength中  

				//printf("转换前ContentLength=%d\n",ContentLength);  
				ContentLength=OnChangeByteOrder(ContentLength);//对其进行转化 并输出  

				_FShpDataVec.push_back(new ShpData);//开辟新矢量空间

				int shapeType;  
				double Box[4];  
				int NumParts;  
				int NumPoints;  
				int *Parts;  

				fread(&shapeType,sizeof(int),1,ShpFile_fp);//从ShpFile_fp中以int型字节的长度，读取一次，并将其读取到ContentLength中  
			
				//fprintf(fp,"shapeType=%d\n",shapeType);  

				for(i=0;i<4;i++)  
				{  
					fread(Box+i,sizeof(double),1,ShpFile_fp); // 读 Box  
				
				}  

				//单个矢量边界
				_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpBox->Xmin=Box[0];
				_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpBox->Ymin=Box[1];
				_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpBox->Xmax=Box[2];
				_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpBox->Ymax=Box[3];


				fread(&NumParts,sizeof(int),1,ShpFile_fp);// 读 NumParts  

				*_VectorNums+=1;
				*_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpNum=*_VectorNums;
				//fprintf(fp,"NumParts=%d\n",NumParts);  

				if (NumParts==0)
				{
					break;
				}

				fread(&NumPoints,sizeof(int),1,ShpFile_fp);// 读NumPoints  
				//fprintf(fp,"NumPoints=%d\n",NumPoints);  

				Parts=new int[NumParts];  
				for(i=0;i<NumParts;i++)  
				{  
					fread(Parts+i,sizeof(int),1,ShpFile_fp);  
				}  

				int pointNum;  
				for(i=0;i<NumParts;i++)  
				{  
					if(i!=NumParts-1)  
						pointNum = Parts[i+1]-Parts[i];  
					else  
						pointNum = NumPoints-Parts[i];  
					double *PointsX;  
					double *PointsY;  
					double *PointsZ;  
					double *PointsM;  
					PointsX =new double[pointNum];  
					PointsY =new double[pointNum];  
					PointsZ =new double[pointNum];
					PointsM =new double[pointNum];
					//读取坐标点  注意先后顺序

					for(j=0;j<pointNum;j++)  
					{  
						fread(PointsX+j, sizeof(double),1,ShpFile_fp);//读取点的X坐标  
						fread(PointsY+j, sizeof(double),1,ShpFile_fp);//读取点的Y坐标
					}  

					fread(&Zmin, sizeof(double),1,ShpFile_fp);//读取点的Z最小坐标  
					fread(&Zmax, sizeof(double),1,ShpFile_fp);//读取点的Z最大坐标  

					_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpBox->Zmin=Zmin;
					_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpBox->Zmax=Zmax;

					for(j=0;j<pointNum;j++)  
					{ 	
						fread(PointsZ+j, sizeof(double),1,ShpFile_fp);//读取点的Z坐标 
					}

					fread(&Mmin, sizeof(double),1,ShpFile_fp);//读取点的M最小坐标  
					fread(&Mmax, sizeof(double),1,ShpFile_fp);//读取点的M最大坐标  

					_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpBox->Mmax=Mmax;
					_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpBox->Mmin=Mmin;

					for(j=0;j<pointNum;j++)  
					{  
						fread(PointsM+j, sizeof(double),1,ShpFile_fp);//读取点的M坐标 
					}
					for (j=0;j<pointNum;j++)
					{
						fprintf(fp,"%.9lf",*(PointsX+j));//写入点X坐标  
						fprintf(fp,",%.9lf",*(PointsY+j));//写入点Y坐标  
						fprintf(fp,",%.9lf\n",*(PointsZ+j));//写入点Z坐标  
					//	fprintf(fp,"  ,M=%.9lf\n",*(PointsM+j));//写入点M坐标 

						_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpPointsArr->push_back( osg::Vec3(*(PointsX+j),*(PointsY+j),*(PointsZ+j)));
					}
					fprintf(fp,"\n",0);//与另一个元素空一行
					
					delete[] PointsX;  
					delete[] PointsY;  
					delete[] PointsZ;  
					delete[] PointsM;  
				}  
				delete[] Parts;  
			}  

			fclose(fp);  

#endif
		}
		else if (ShapeType==15)//含有Z坐标的面
		{
#if 1
			// 读取线状目标的实体信息  
			FILE * fp;  
			if((fp=fopen("../CmfData/shp/PolygonZ.txt","w"))==NULL)  //新建一个dxf文件    
			{  
				exit(0);  
			}  
			
			int i,j;  
			int RecordNumber;//记录号，big
			int ContentLength;//记录长度，big
			double Zmin;
			double Zmax;
			double Mmin;
			double Mmax;
			int num=0;  

			while((fread(&RecordNumber,sizeof(int),1,ShpFile_fp)!=0))//从ShpFile_fp中以int型字节的长度，读取一次，并将其读取到RecordNumber中  
			{     
	
				//printf("转换前RecordNumber=%d\n",RecordNumber);  
				RecordNumber=OnChangeByteOrder(RecordNumber);//对RecordNumber进行转化后打印输出   

				fread(&ContentLength,sizeof(int),1,ShpFile_fp);//从ShpFile_fp中以int型字节的长度，读取一次，并将其读取到ContentLength中  
				//printf("转换前ContentLength=%d\n",ContentLength);  
				ContentLength=OnChangeByteOrder(ContentLength);//对其进行转化 并输出  

				_FShpDataVec.push_back(new ShpData);//开辟新矢量空间

				int shapeType;  
				double Box[4];  
				int NumParts;  
				int NumPoints;  
				int *Parts; 

				fread(&shapeType,sizeof(int),1,ShpFile_fp);//从ShpFile_fp中以int型字节的长度，读取一次，并将其读取到ContentLength中  

				for(i=0;i<4;i++)  
				{  
					fread(Box+i,sizeof(double),1,ShpFile_fp); // 读 Box   
				}  

				//单个矢量边界
				_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpBox->Xmin=Box[0];
				_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpBox->Ymin=Box[1];
				_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpBox->Xmax=Box[2];
				_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpBox->Ymax=Box[3];

				fread(&NumParts,sizeof(int),1,ShpFile_fp);// 读 NumParts   面数量

				*_VectorNums+=1;
				*_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpNum=*_VectorNums;

				fread(&NumPoints,sizeof(int),1,ShpFile_fp);// 读NumPoints 点数量


				Parts=new int[NumParts];  

				for(i=0;i<NumParts;i++)  
				{  
					fread(Parts+i,sizeof(int),1,ShpFile_fp);  
				}  

				int pointNum;  
				for(i=0;i<NumParts;i++)  
				{  
					if(i!=NumParts-1)  //如果不是最后一个面
						pointNum = Parts[i+1]-Parts[i];  //获取该面的点数（后面一个面的索引值减去该面的）
					else  
						pointNum = NumPoints-Parts[i];  

					double *PointsX;  
					double *PointsY;  
					double *PointsZ;  
					double *PointsM;  
					PointsX =new double[pointNum];  
					PointsY =new double[pointNum];  
					PointsZ =new double[pointNum];
					PointsM =new double[pointNum];
					//读取坐标点  注意先后顺序

					for(j=0;j<pointNum;j++)  
					{  
						fread(PointsX+j, sizeof(double),1,ShpFile_fp);//读取点的X坐标  
						fread(PointsY+j, sizeof(double),1,ShpFile_fp);//读取点的Y坐标
					}  


					fread(&Zmin, sizeof(double),1,ShpFile_fp);//读取点的Z最小坐标  
					fread(&Zmax, sizeof(double),1,ShpFile_fp);//读取点的Z最大坐标  

					_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpBox->Zmin=Zmin;
					_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpBox->Zmax=Zmax;
					
					for(j=0;j<pointNum;j++)  
					{ 	
						fread(PointsZ+j, sizeof(double),1,ShpFile_fp);//读取点的Z坐标 
					}

					fread(&Mmin, sizeof(double),1,ShpFile_fp);//读取点的M最小坐标  
					fread(&Mmax, sizeof(double),1,ShpFile_fp);//读取点的M最大坐标  

					_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpBox->Mmax=Mmax;
					_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpBox->Mmin=Mmin;

					for(j=0;j<pointNum;j++)  
					{  
						fread(PointsM+j, sizeof(double),1,ShpFile_fp);//读取点的M坐标 
					}
					for (j=0;j<pointNum;j++)
					{
						fprintf(fp,"%.9lf",*(PointsX+j));//写入点X坐标  
						fprintf(fp,",%.9lf",*(PointsY+j));//写入点Y坐标  
						if (*(PointsZ+j)<-100||*(PointsZ+j)>10000)
						{
							fprintf(fp,",%.9lf\n",0);//写入点Z坐标  

							_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpPointsArr->push_back( osg::Vec3(*(PointsX+j),*(PointsY+j),0));
						}
						else
						{
							fprintf(fp,",%.9lf\n",*(PointsZ+j));//写入点Z坐标  
						//	fprintf(fp,"  ,M=%.9lf\n",*(PointsM+j));//写入点M坐标 

							_FShpDataVec.at(_FShpDataVec.size()-1)->_ShpPointsArr->push_back(osg::Vec3(*(PointsX+j),*(PointsY+j),*(PointsZ+j)));	
						}

					}
					fprintf(fp,"\n",0);//与另一个元素空一行

					delete[] PointsX;  
					delete[] PointsY;  
					delete[] PointsZ;  
					delete[] PointsM;  
				}  
				delete[] Parts;  
			}  

			fclose(fp);  
#endif
		}
		
}
