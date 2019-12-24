#pragma once

//获取SHP文件的矢量坐标

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include<iomanip>//setiosflags 头文件

#include<stdio.h>  
#include<math.h>  
#include<stdlib.h>  

#include <osg/Group>

using namespace std;
class CShapefileR
{
public:
	CShapefileR(void);
	~CShapefileR(void);

	//边界
	class ShpBox
	{
	public:
		ShpBox()
		{
			Xmin=0;
			Xmax=0;
			Ymin=0;
			Ymax=0;
			Zmin=0;
			Zmax=0;
			Mmin=0;
			Mmax=0;
		}
		~ShpBox()
		{

		}
		double Xmin;
		double Xmax;
		double Ymin;
		double Ymax;
		double Zmin;
		double Zmax;
		double Mmin;
		double Mmax;
	};

	//每个矢量串数据
	class ShpData
	{
	public:
		ShpData()
		{
			_ShpBox=new ShpBox;
			_ShpPointsArr=new osg::Vec3dArray;
            _ShpNum = new unsigned long long ;
		}
		~ShpData()
		{

		}
		ShpBox* _ShpBox;
		osg::ref_ptr<osg::Vec3dArray> _ShpPointsArr;
        unsigned long long *_ShpNum;
	};

	void Read(FILE * ShpFile_fp);
 
//	void WriteShp(FILE * ShpFile_fp);

	int OnChangeByteOrder(int indata);//数据变换,交换位序


	ShpBox *_AllBox;
	int *_VectorNums;
	std::vector<ShpData*> _FShpDataVec;

};

