#include "SchemeComparisonAnalysis/CDraggerContainers.h"


COEDraggerContainers::COEDraggerContainers(osgEarth::MapNode* m_MapNode,osg::ref_ptr<osg::Group> m_root)
{
	_Dragger=new CMFManipulator::Translate1DDragger;
	osg::Matrix tempMat;
	ResetDraggerContainers(tempMat);

	_mMapNode=m_MapNode;
	_mMapSRS=m_MapNode->getMapSRS();
	_mGeoSRS=m_MapNode->getMapSRS()->getGeographicSRS();

	_root=m_root;
}

void COEDraggerContainers::addDraggerToScene(osg::ref_ptr<osg::MatrixTransform> m_sceneMatTransf,DragType m_dragname)
{
	//计算拖拽器方向
	osg::ref_ptr<osg::Vec3dArray> xyzcModelWorldVertices=new osg::Vec3dArray;
	computeDragdir(m_sceneMatTransf,xyzcModelWorldVertices);

	//添加操作器
	if (m_dragname==TranslateAxis)
	{
		_Dragger=new TranslateAxisCMDragger(xyzcModelWorldVertices);
	}
	else if (m_dragname==Rotateball)
	{
		_Dragger=new TrackballCMDragger(false,xyzcModelWorldVertices);
	}
	else if (m_dragname==ScaleAxis)
	{
		_Dragger=new ScaleAxisCMDDragger(xyzcModelWorldVertices);
	}
	else
	{
		cout<<"ERROR：操作器不存在!"<<endl;
	}

	//不同模型的时候，重置
	if (_preTransfModel!=m_sceneMatTransf)
	{
		_preTransfModel=m_sceneMatTransf;
	//	ResetDraggerContainers(_preTransfModel->getMatrix());
		*_Dragger->_DTPreLastWorldMatrix=_preTransfModel->getMatrix();
		_predragTmpMatrix=new osg::Matrix;
		_predragTmpMatrix=_Dragger->_DTPreLastWorldMatrix;
	}
	else
		this->_predragTmpMatrix=_Dragger->_DTPreLastWorldMatrix;


	_root->addChild(_Dragger); //直接将拖拽器添加在场景

	float scale = m_sceneMatTransf->getBound().radius();//大小
	_Dragger->setMatrix(osg::Matrix::scale(scale, scale, scale) * osg::Matrix::translate(m_sceneMatTransf->getBound().center()));

	_Dragger->setupDefaultGeometry();
	_Dragger->addTransformUpdating(m_sceneMatTransf); //参数为 matrixtransform类型
	_Dragger->setHandleEvents(true); 
	_Dragger->setActivationModKeyMask(osgGA::GUIEventAdapter::MODKEY_LEFT_CTRL);

}

void COEDraggerContainers::DisableDragger(osg::ref_ptr<osg::MatrixTransform> m_draggerToMatTransf) 
{ 
	if (_Dragger!=NULL)
	{
		_root->removeChild(_Dragger);
		_Dragger->removeTransformUpdating(m_draggerToMatTransf); 
		_Dragger->setHandleEvents(false); 	
	}
	else
	{
		cout<<"ERROR：未添加操作器!"<<endl;
	}
}

void COEDraggerContainers::ResetDraggerContainers(osg::Matrix mTmpMatrix)
{
	_predragTmpMatrix=new osg::Matrix;
	*_predragTmpMatrix=mTmpMatrix;

	_dragCMVectMatrix=new vector<osg::Matrix>;
	_dragCMTimes=new int;

	_predragCMVectMatrix=new vector<osg::Matrix>;
	_predragCMVectMatrix->push_back(mTmpMatrix);
	_predragCMVectMatrix->push_back(mTmpMatrix);
	_predragCMVectMatrix->push_back(mTmpMatrix);
	_predragCMTimes=new int;
	*_predragCMTimes=0;
}

void COEDraggerContainers::computeDragdir(osg::ref_ptr<osg::MatrixTransform> m_sceneMatTransf,osg::ref_ptr<osg::Vec3dArray> xyzcModelWorldVertices)
{
	osg::ref_ptr<osg::Vec3dArray> xyzcModelLlVertices=new osg::Vec3dArray;

	//1.中心点世界转经纬
	GeoPoint convertPoint;
	convertPoint.fromWorld(_mGeoSRS,m_sceneMatTransf->getBound().center());

	xyzcModelLlVertices->push_back(osg::Vec3d(convertPoint.x(),convertPoint.y(),convertPoint.z()));
	//2.获得X,Y,Z方向三个点
	osg::Vec3d xCenterM(convertPoint.x()-0.001f,convertPoint.y(),convertPoint.z());
	osg::Vec3d yCenterM(convertPoint.x(),convertPoint.y()-0.001f,convertPoint.z());
	osg::Vec3d zCenterM(convertPoint.x(),convertPoint.y(),convertPoint.z()-10.001f);

	xyzcModelLlVertices->push_back(xCenterM);
	xyzcModelLlVertices->push_back(yCenterM);
	xyzcModelLlVertices->push_back(zCenterM);
	//3.X、Y、Z、C全部转为世界坐标

	for(osg::Vec3dArray::iterator iter=xyzcModelLlVertices->begin();iter!=xyzcModelLlVertices->end();iter++)
	{
		osgEarth::GeoPoint mapPos=osgEarth::GeoPoint(_mGeoSRS,(*iter).x(),(*iter).y(),(*iter).z(),osgEarth::AltitudeMode::ALTMODE_ABSOLUTE).transform(_mMapSRS);

		osg::Vec3d tempWVarr;
		mapPos.toWorld(tempWVarr);//经纬度转换为世界坐标

		xyzcModelWorldVertices->push_back(tempWVarr);//新坐标矩阵计算
	}
}



