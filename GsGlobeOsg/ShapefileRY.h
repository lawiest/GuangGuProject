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
class CShapefileRY
{
public:
	CShapefileRY(void);
	~CShapefileRY(void);


	void Read(FILE * ShpFile_fp);
    

	int OnChangeByteOrder(int indata);



public:

	osg::ref_ptr<osg::Vec3dArray> _FPointsArr;

	std::vector<int> _PointNum;

	double Xmin;
	double Ymin;
	double Xmax;
	double Ymax;
	double Zmin;
	double Zmax;
	double Mmin;
	double Mmax;
};

