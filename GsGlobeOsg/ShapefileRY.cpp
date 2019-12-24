

#include "StdAfx.h"
#include "ShapefileRY.h"


CShapefileRY::CShapefileRY(void)
{
	_FPointsArr=new osg::Vec3dArray;
	Xmin=0;
	Ymin=0;
	Xmax=0;
	Ymax=0;
	Zmin=0;
	Zmax=0;
	Mmin=0;
	Mmax=0;
}

CShapefileRY::~CShapefileRY(void)
{

}

int CShapefileRY::OnChangeByteOrder(int indata)  
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
         
        value=value+int(k*pow(16.0,7-i));  
    }  
    
    return(value);  
}
//数据处理，获取坐标
void CShapefileRY::Read(FILE * ShpFile_fp)
{
		int i;  
		int FileCode;  
		int Unused;  
		int FileLength;  
		int Version;  
		int ShapeType;  


			fread(&FileCode,sizeof(int),1,ShpFile_fp);
			FileCode = OnChangeByteOrder(FileCode);

			for(i=0;i<5;i++)
			fread(&Unused,sizeof(int),1,ShpFile_fp);

			fread(&FileLength,sizeof(int),1,ShpFile_fp);
			FileLength=OnChangeByteOrder(FileLength);

			fread(&Version,sizeof(int),1,ShpFile_fp);

			fread(&ShapeType,sizeof(int),1,ShpFile_fp);//读取ShapeType的值  

			fread(&Xmin,         sizeof(double),1,ShpFile_fp);

			fread(&Ymin,         sizeof(double),1,ShpFile_fp);

			fread(&Xmax,         sizeof(double),1,ShpFile_fp);

			fread(&Ymax,         sizeof(double),1,ShpFile_fp);

			fread(&Zmin,         sizeof(double),1,ShpFile_fp);

			fread(&Zmax,        sizeof(double),1,ShpFile_fp);

			fread(&Mmin,         sizeof(double),1,ShpFile_fp);

			fread(&Mmax,         sizeof(double),1,ShpFile_fp);


		//判断shp文件类型（点线面）
		if (ShapeType==1)//点
		{
#if 1
			FILE * fp;  
			if((fp=fopen("C:/Users/zhangli/Desktop/point.txt","w"))==NULL)  //新建一个txt文件    
			{  
				exit(0);  
			}  
			_FPointsArr=new osg::Vec3dArray;

			int RecordNumber;//记录号，big
			int ContentLength;//记录长度，big
			int num=0;
			while ((fread(&RecordNumber,sizeof(int),1,ShpFile_fp)!=0))//保证每个记录都检测到
			{
				fread(&ContentLength,sizeof(int),1,ShpFile_fp);
				RecordNumber=OnChangeByteOrder(RecordNumber);
				ContentLength=OnChangeByteOrder(ContentLength);
				int shapeT;
				double x;
				double y;
				fread(&shapeT,sizeof(int),1,ShpFile_fp);
				fread(&x,sizeof(double),1,ShpFile_fp);
				fread(&y,sizeof(double),1,ShpFile_fp);

				fprintf(fp,"X=%.9lf",x);//写入点X坐标  
				fprintf(fp,"   ,Y=%.9lf",y);//写入点Y坐标  
				fprintf(fp,"  ,Z=%.9lf\n",0);//写入点Z坐标 

				_FPointsArr->push_back(osg::Vec3d(x,y,0));
			}		
			fclose(fp);

#endif	
		} 
		else if (ShapeType==3)//线
		{
#if 1
			// 读取线状目标的实体信息  
			FILE * fp;  
			if((fp=fopen("C:/Users/zhangli/Desktop/polyline.txt","w"))==NULL)  //新建一个dxf文件    
			{  
				exit(0);  
			}  
			
			_FPointsArr=new osg::Vec3dArray;

			int i,j;  
			int RecordNumber;//记录号，big
			int ContentLength;//记录长度，big 
			int num=0;  
			//int ii=0;
			while((fread(&RecordNumber,sizeof(int),1,ShpFile_fp)!=0)) 
			{     
				
				RecordNumber=OnChangeByteOrder(RecordNumber); 

				fread(&ContentLength,sizeof(int),1,ShpFile_fp);
	  
				ContentLength=OnChangeByteOrder(ContentLength);//对其进行转化 并输出  
				
			//ii++;

				int shapeType;  
				double Box[4];  
				int NumParts;  
				int NumPoints;  
				int *Parts;  

				fread(&shapeType,sizeof(int),1,ShpFile_fp); 

				for(i=0;i<4;i++)  
				{  
					fread(Box+i,sizeof(double),1,ShpFile_fp); // 读 Box  
		
				}  
				fread(&NumParts,sizeof(int),1,ShpFile_fp);// 读 NumParts   
			

				fread(&NumPoints,sizeof(int),1,ShpFile_fp);// 读NumPoints  
		 
				
				Parts=new int[NumParts];  
				for(i=0;i<NumParts;i++)  
				{  
					fread(Parts+i,sizeof(int),1,ShpFile_fp);  
		
				}  
				//aa[ii]=*Parts;

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

						fprintf(fp,"X=%.9lf",*(PointsX+j));//写入点X坐标  
						fprintf(fp,"  ,Y=%.9lf",*(PointsY+j));//写入点Y坐标  
						fprintf(fp,"  ,Z=%.9lf\n",0);//写入点Z坐标  
						//------------------------------------------------------  
					}  

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
			if((fp=fopen("C:/Users/zhangli/Desktop/Polygon.txt","w"))==NULL)  //新建一个dxf文件    
			{  
				exit(0);  
			}  

			_FPointsArr=new osg::Vec3dArray;

			int i,j;  
			int RecordNumber;//记录号，big
			int ContentLength;//记录长度，big
			int num=0;  
			while((fread(&RecordNumber,sizeof(int),1,ShpFile_fp)!=0)) 
			{     
				;  
				RecordNumber=OnChangeByteOrder(RecordNumber);

				fread(&ContentLength,sizeof(int),1,ShpFile_fp);
				ContentLength=OnChangeByteOrder(ContentLength); 

				int shapeType;  
				double Box[4];  
				int NumParts;  
				int NumPoints;  
				int *Parts; 

				fread(&shapeType,sizeof(int),1,ShpFile_fp);

				for(i=0;i<4;i++)  
				{  
					fread(Box+i,sizeof(double),1,ShpFile_fp); // 读 Box   
				}  
				fread(&NumParts,sizeof(int),1,ShpFile_fp);// 读 NumParts   面数量


				fread(&NumPoints,sizeof(int),1,ShpFile_fp);// 读NumPoints 点数量
  

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
					PointsX =new double[pointNum];  
					PointsY =new double[pointNum];  
					//读取坐标点  

					for(j=0;j<pointNum;j++)  
					{  
						fread(PointsX+j, sizeof(double),1,ShpFile_fp);//读取点的X坐标  
						fread(PointsY+j, sizeof(double),1,ShpFile_fp);//读取点的Y坐标  

						fprintf(fp,"X=%.9lf",*(PointsX+j));//写入点X坐标  
						fprintf(fp,"   ,Y=%.9lf",*(PointsY+j));//写入点Y坐标  
						fprintf(fp,"  ,Z=%.9lf\n",0);//写入点Z坐标  
					}  

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
			if((fp=fopen("C:/Users/zhangli/Desktop/pointZ.txt","w"))==NULL)    
			{  
				exit(0);  
			}  
			_FPointsArr=new osg::Vec3dArray;

			int RecordNumber;//记录号，big
			int ContentLength;//记录长度，big
			int num=0;
			while ((fread(&RecordNumber,sizeof(int),1,ShpFile_fp)!=0))//保证每个记录都检测到
			{
				fread(&ContentLength,sizeof(int),1,ShpFile_fp);
				RecordNumber=OnChangeByteOrder(RecordNumber);
				ContentLength=OnChangeByteOrder(ContentLength);
				int shapeT;
				double x;
				double y;
				double z;
				double m;
				fread(&shapeT,sizeof(int),1,ShpFile_fp);
				fread(&x,sizeof(double),1,ShpFile_fp);
				fread(&y,sizeof(double),1,ShpFile_fp);
				fread(&z,sizeof(double),1,ShpFile_fp);
				fread(&m,sizeof(double),1,ShpFile_fp);
				fprintf(fp,"X=%.9lf",x);//写入点X坐标  
				fprintf(fp,"   ,Y=%.9lf",y);//写入点Y坐标  
				fprintf(fp,"  ,Z=%.9lf\n",z);//写入点Z坐标  

				_FPointsArr->push_back(osg::Vec3d(x,y,z));

			}		
			fclose(fp);

#endif	
		}
		else if (ShapeType==13)//含有Z坐标的线段
		{
#if 1
			// 读取线状目标的实体信息  
			FILE * fp;  
			if((fp=fopen("C:/Users/zhangli/Desktop/PolylineZ.txt","w"))==NULL)  //新建一个dxf文件    
			{  
				exit(0);  
			}  
			_FPointsArr=new osg::Vec3dArray;

			int i,j;  
			int RecordNumber;//记录号，big
			int ContentLength;//记录长度，big
			double Zmin;
			double Zmax;
			double Mmin;
			double Mmax;
			int num=0;  
			while((fread(&RecordNumber,sizeof(int),1,ShpFile_fp)!=0))
			{     
			
				RecordNumber=OnChangeByteOrder(RecordNumber); 

				fread(&ContentLength,sizeof(int),1,ShpFile_fp);  

				 
				ContentLength=OnChangeByteOrder(ContentLength);//对其进行转化 并输出  

				int shapeType;  
				double Box[4];  
				int NumParts;  
				int NumPoints;  
				int *Parts;  

				fread(&shapeType,sizeof(int),1,ShpFile_fp); 
			
				 
				for(i=0;i<4;i++)  
				{  
					fread(Box+i,sizeof(double),1,ShpFile_fp); // 读 Box  
				
				}  
				fread(&NumParts,sizeof(int),1,ShpFile_fp);// 读 NumParts   
				 

				if (NumParts==0)
				{
					break;
				}

				fread(&NumPoints,sizeof(int),1,ShpFile_fp);// 读NumPoints  
				  

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
					for(j=0;j<pointNum;j++)  
					{ 	
						fread(PointsZ+j, sizeof(double),1,ShpFile_fp);//读取点的Z坐标 
					}

					fread(&Mmin, sizeof(double),1,ShpFile_fp);//读取点的M最小坐标  
					fread(&Mmax, sizeof(double),1,ShpFile_fp);//读取点的M最大坐标  
					for(j=0;j<pointNum;j++)  
					{  
						fread(PointsM+j, sizeof(double),1,ShpFile_fp);//读取点的M坐标 
					}
					for (j=0;j<pointNum;j++)
					{
						fprintf(fp,"X=%.9lf",*(PointsX+j));//写入点X坐标  
						fprintf(fp,"  ,Y=%.9lf",*(PointsY+j));//写入点Y坐标  
						fprintf(fp,"  ,Z=%.9lf\n",*(PointsZ+j));//写入点Z坐标  
					//	fprintf(fp,"  ,M=%.9lf\n",*(PointsM+j));//写入点M坐标 
						_FPointsArr->push_back(osg::Vec3(*(PointsX+j),*(PointsY+j),*(PointsZ+j)));

					}
					
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
			// 读取面状目标的实体信息  
			FILE * fp;  
			if((fp=fopen("C:/Users/Administrator/Desktop/PolygonZ.txt","w"))==NULL)  //新建一个dxf文件    
			{  
				exit(0);  
			}  
			_FPointsArr=new osg::Vec3dArray;

			int i,j;  
			int RecordNumber;//记录号，big
			int ContentLength;//记录长度，big
			double Zmin;
			double Zmax;
			double Mmin;
			double Mmax;
			int sum=0; 
			_PointNum.push_back(0);

			while((fread(&RecordNumber,sizeof(int),1,ShpFile_fp)!=0))//从ShpFile_fp中以int型字节的长度，读取一次，并将其读取到RecordNumber中  
			{     

				//printf("转换前RecordNumber=%d\n",RecordNumber);  
				RecordNumber=OnChangeByteOrder(RecordNumber);//对RecordNumber进行转化后打印输出   

				fread(&ContentLength,sizeof(int),1,ShpFile_fp);//从ShpFile_fp中以int型字节的长度，读取一次，并将其读取到ContentLength中  
				//printf("转换前ContentLength=%d\n",ContentLength);  
				ContentLength=OnChangeByteOrder(ContentLength);//对其进行转化 并输出  

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

				fread(&NumParts,sizeof(int),1,ShpFile_fp);// 读 NumParts   面数量

				fread(&NumPoints,sizeof(int),1,ShpFile_fp);// 读NumPoints 点数量

				Parts=new int[NumParts];  

				for(i=0;i<NumParts;i++)  
				{  
					fread(Parts+i,sizeof(int),1,ShpFile_fp);  
				}  

				sum=sum+NumPoints;
				_PointNum.push_back(sum);

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

					for(j=0;j<pointNum;j++)  
					{ 	
						fread(PointsZ+j, sizeof(double),1,ShpFile_fp);//读取点的Z坐标 
					}

					fread(&Mmin, sizeof(double),1,ShpFile_fp);//读取点的M最小坐标  
					fread(&Mmax, sizeof(double),1,ShpFile_fp);//读取点的M最大坐标  

					for(j=0;j<pointNum;j++)  
					{  
						fread(PointsM+j, sizeof(double),1,ShpFile_fp);//读取点的M坐标 
					}
					for (j=0;j<pointNum;j++)
					{
						fprintf(fp,"X=%.9lf",*(PointsX+j));//写入点X坐标  
						fprintf(fp,"  ,Y=%.9lf",*(PointsY+j));//写入点Y坐标  
						if (*(PointsZ+j)<-100||*(PointsZ+j)>10000)
						{
							fprintf(fp,"  ,Z=%.9lf\n",0);//写入点Z坐标  

							_FPointsArr->push_back(osg::Vec3(*(PointsX+j),*(PointsY+j),0));
						}
						else
						{
							fprintf(fp,"  ,Z=%.9lf\n",*(PointsZ+j));//写入点Z坐标  
							//	fprintf(fp,"  ,M=%.9lf\n",*(PointsM+j));//写入点M坐标 

							_FPointsArr->push_back(osg::Vec3(*(PointsX+j),*(PointsY+j),*(PointsZ+j)));	
						}

					}

					delete[] PointsX;  
					delete[] PointsY;  
					delete[] PointsZ;  
					delete[] PointsM;  
				}  
				delete[] Parts;  
			}  

			osg::Vec3d bbb=_FPointsArr->at(_PointNum.at(1));
			fclose(fp);  
						 
#endif
		}
		
}





