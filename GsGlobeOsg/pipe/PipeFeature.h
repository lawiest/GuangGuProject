#pragma once
//#include <d3dx9.h>
//#include "ModelResource.h"
#include <osg/Vec3d>
#include <osg/Referenced>
#include <osg/Matrix>

#include "Pipe/GeoPipeModel.pb.h"
typedef unsigned long long  UINT64;
typedef unsigned long long  uint64;


//这部分代码直接复制GeoGlobe的故不补充注释


enum enumPipeFeatureType
{
	ePipeFeaturePoint =0,
	ePipeFeatureLine
};

enum enumCrossSectionType
{
	eCircle = 0,
	eRect
};
 
class CPipeFeatureBase : public osg::Referenced
{
public:
	CPipeFeatureBase(void)
	{
		m_nFeatureID = -1;
		m_nSymbolID = -1;
	}
	virtual ~CPipeFeatureBase(){}
	virtual bool Serialize(GEOPIPEMODEL::PIPE* pPBPipe) = 0;	

	enumPipeFeatureType PipeFeatureType(){return m_eFeatureType;}
	UINT64 FeatureID(){return m_nFeatureID;}
	UINT64 SymbolID(){return m_nSymbolID;}

protected:
	enumPipeFeatureType m_eFeatureType;
	UINT64 m_nFeatureID;//²»ÊÇÒªËØOID£¬ÒÔÃâµãÒªËØÓëÏßÒªËØÖØ¸´
	UINT64 m_nSymbolID;//ÈýÎ¬»¯·ûºÅID
};


class CPipeLineFeature : public CPipeFeatureBase
{
public:
	CPipeLineFeature()
	{
		m_eFeatureType = ePipeFeatureLine;
	}
	virtual ~CPipeLineFeature(){}
	virtual bool Serialize(GEOPIPEMODEL::PIPE* pPBPipe);

	osg::Vec3d StartPoint(){return m_ptStartGeo;}
	osg::Vec3d EndPoint(){return m_ptEndGeo;}
	enumCrossSectionType CrossSectionType(){return m_eCrossSecType;}
	double Diameter(){return m_dbDiameter;}
	double Height(){return m_dbHeight;}
	double Width(){return m_dbWidth;}
	uint64 StartPointID(){return m_nStartID;}
	uint64 EndPointID(){return m_nEndID;}

private:
	enumCrossSectionType m_eCrossSecType;
	osg::Vec3d m_ptStartGeo;
	osg::Vec3d m_ptEndGeo;
	double   m_dbDiameter;
	double   m_dbHeight;
	double   m_dbWidth;
	uint64   m_nStartID;
	uint64   m_nEndID;
};


class CPipePointFeature : public CPipeFeatureBase
{
public:
	CPipePointFeature()
	{
		m_eFeatureType = ePipeFeaturePoint;
	}
	virtual ~CPipePointFeature(){}
	virtual bool Serialize(GEOPIPEMODEL::PIPE* pPBPipe);

	osg::Matrix GetMatrix()
	{
		osg::Matrix rotMatrix,scaleMatrix;

		rotMatrix.setRotate(osg::Quat(m_dbRotateX,osg::Vec3d(1,0,0),m_dbRotateY,osg::Vec3d(0,1,0),m_dbRotateZ,osg::Vec3d(0,0,1)));
		scaleMatrix.scale(osg::Vec3d(m_dbScaleX,m_dbScaleY,m_dbScaleZ));
		//D3DXMatrixRotationX(&rotMatrixX,m_dbRotateX);
		//D3DXMatrixRotationY(&rotMatrixY,m_dbRotateY);
		//D3DXMatrixRotationZ(&rotMatrixZ,m_dbRotateZ);
		//rotMatrix = rotMatrixX*rotMatrixY*rotMatrixZ;

		//D3DXMATRIX scaleMatrix;
		//D3DXMatrixScaling(&scaleMatrix,m_dbScaleX,m_dbScaleY,m_dbScaleZ);

		return scaleMatrix*rotMatrix;
	}

	osg::Vec3d GeoPoint() const{return m_ptGeo;}

private:
	osg::Vec3d m_ptGeo;
	double   m_dbCoverDepth;
	double   m_dbRotateX;
	double   m_dbRotateY;
	double   m_dbRotateZ;
	double   m_dbScaleX;
	double   m_dbScaleY;
	double   m_dbScaleZ;
};