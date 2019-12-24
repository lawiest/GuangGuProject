#include <osgEarthUtil/Ephemeris>
#include <osgUtil/IntersectVisitor>
#include "ShapefileRY.h"
#include "SunLigntAnalysis.h"

CSunLightAnalysis::CSunLightAnalysis()
{
	aveMinute = 0;//平均日照时间
}
CSunLightAnalysis::CSunLightAnalysis(osgViewer::Viewer* viewer, const osgEarth::DateTime& qDate)
{
	aveMinute = 0;//平均日照时间
	m_viewer = viewer;
	m_Date = qDate;
}
CSunLightAnalysis::~CSunLightAnalysis(void)
{

}

void CSunLightAnalysis::initSunPositionAndModelGroup(int nTime, osg::Group* modelGroup)
{
	osg::ref_ptr<osgEarth::Util::Ephemeris> ep = new osgEarth::Util::Ephemeris();
	for(int min=480;min<960;min=min+(nTime))//nTime间隔
	{
		int h=min/60;
		osg::Vec3d sunPosition= ep->getSunPosition(m_Date).geocentric;
		m_vSunPosition.push_back(sunPosition);
	}
	m_ls = new osgUtil::LineSegmentIntersector(m_vSunPosition[0],m_vSunPosition[1]);
	m_iv = new osgUtil::IntersectionVisitor(m_ls); 

	m_regionNodeRoot = modelGroup;
}
int CSunLightAnalysis::LineAnalysis(/*osgViewer::Viewer* viewer,*/GeoGlobe::Model::LodModel* lodmodel,int* nSampletime,char* filepath)
{
	FILE * shp=fopen(filepath,"rb");
	CShapefileRY ShapefileR;
	ShapefileR.Read(shp);
	fclose(shp);
	osg::ref_ptr<osg::Vec3dArray> PointArr = ShapefileR._FPointsArr;//坐标序列
	std::vector<int> PointNum = ShapefileR._PointNum;//面坐标点数序列
	std::vector<int> WorldpointArr;
	osg::Vec3dArray* WorldPoint=new osg::Vec3dArray;//存所有采样点的世界坐标
	std::vector<int> SunshineMinute;//各采样点日照量

	osgEarth::SpatialReference *MapWGS84SRS = osgEarth::SpatialReference::get("wgs84");//create("EPSG:4326");
	osgEarth::GeoPoint Centerlatlon;
	Centerlatlon.fromWorld(MapWGS84SRS,lodmodel->_options->_centerPos);
	//选中的模型与面shapefile匹配,判断选中模型是第几个面
	int num=0;//shp中第几个面
	for(int i=0;i<PointNum.size()-1;i++)
	{
		int c=0;
		for(int j=PointNum.at(i);j<PointNum.at(i+1)-1;j++)
		{
			osg::Vec3d p1=PointArr->at(j);
			osg::Vec3d p2=PointArr->at(j+1);
			if(((p1.y()>Centerlatlon.y()) != (p2.y()>Centerlatlon.y()))&&   
				(Centerlatlon.x()<(p2.x()-p1.x())*(Centerlatlon.y()-p1.y())/(p2.y()-p1.y())+p1.x()))
				c=c+1;
		}
		if(c%2!=0)//水平向右引射线，若交点为奇数个，在该多边形内
		{
			num=i;
			break;
		}	
	}
	//选出参与计算的线段
	for(int i=PointNum.at(num);i<PointNum.at(num+1)-1;i++)
	{
		osg::Vec3d p1=PointArr->at(i);//经纬度
		osg::Vec3d p2=PointArr->at(i+1);
		if(p1.x()<=p2.x())//首先删除朝北的线段
			continue;
		osg::Vec3d p0;
		osg::Vec3d p3;
		if(i>PointNum.at(num) && i<PointNum.at(num+1)-2)//朝南的面中选出凸点参与采样计算
		{
			p0=PointArr->at(i-1);
			p3=PointArr->at(i+2);
		}
		if(i==PointNum.at(num))
		{
			int ii=PointNum.at(num+1)-2;
			p0=PointArr->at(ii);
			p3=PointArr->at(i+2);
		}
		if(i==PointNum.at(num+1)-2)
		{
			int ii=PointNum.at(num)+1;
			p0=PointArr->at(i-1);
			p3=PointArr->at(ii);
		}
		//判断两端点的凹凸性，选出两端都是凸顶点的
		double s1=(p1.x()-p0.x())*(p1.y()-p2.y())-(p1.y()-p0.y())*(p1.x()-p2.x());
		double s2=(p2.x()-p1.x())*(p2.y()-p3.y())-(p2.y()-p1.y())*(p2.x()-p3.x());
		if(s1<=0||s2<=0)//顺时针多边形，S>0为凸点
			continue;
		//线段角度容差：与正北方向夹角30°~150°
		osg::Vec2d p12=(osg::Vec2(p2.x()-p1.x(),p2.y()-p1.y()));
		osg::Vec2d pnorth=(osg::Vec2(0.0,1.0));
		double cosr=p12.y()/sqrt(p12.x()*p12.x()+p12.y()*p12.y());
		double angl=acos(cosr)*180/3.1415926;
		if(angl>=30 && angl<=150)
		{
			//经纬度转世界坐标
			p1.z()+=0.9;//从高于地脚线0.9m处计算
			p2.z()+=0.9;
			GeoPoint latlon1(MapWGS84SRS, p1, ALTMODE_ABSOLUTE);
			osg::Vec3d w1;
			latlon1.toWorld(w1);
			GeoPoint latlon2(MapWGS84SRS, p2, ALTMODE_ABSOLUTE);
			osg::Vec3d w2;
			latlon2.toWorld(w2);
			//对线段p1p2采样
			osg::Vec3d sample_point = w1;
			double Length2 = (w1.x()-w2.x())*(w1.x()-w2.x())+(w1.y()-w2.y())*(w1.y()-w2.y())+(w1.z()-w2.z())*(w1.z()-w2.z());
			double Length = sqrt(Length2);
			int j=1;
			while(( (sample_point.x()-w1.x())*(sample_point.x()-w1.x())+(sample_point.y()-w1.y())*(sample_point.y()-w1.y())+(sample_point.z()-w1.z())*(sample_point.z()-w1.z())) <= Length2  )
			{
				WorldPoint->push_back(sample_point);
				sample_point.x()=w1.x()+(w2.x()-w1.x())*j/Length;//间隔1m采样
				sample_point.y()=w1.y()+(w2.y()-w1.y())*j/Length;
				sample_point.z()=w1.z()+(w2.z()-w1.z())*j/Length;
				j++;
			}
				WorldPoint->push_back(w2);
		}
	}	
	unsigned long long uKey = lodmodel->_options->_feature->m_nFID;
	osg::ref_ptr<osg::Node> _node;
	for(int i=0;i<m_regionNodeRoot->asGroup()->getNumChildren();i++)
	{
		_node = m_regionNodeRoot->asGroup()->getChild(i);
		osg::ref_ptr<GeoGlobe::Model::LodModel> _modelnode =  dynamic_cast<GeoGlobe::Model::LodModel*>(_node.get());
		if(uKey == _modelnode->_options->_feature->m_nFID)
		{
			m_regionNodeRoot->asGroup()->removeChild(_node);
			break;
		}
	}
	for(int m=0;m<WorldPoint->size();m++)
	{
		osg::Vec3d sampPoint=WorldPoint->at(m);

		osg::Vec3d sunPosition;
		osgUtil::IntersectVisitor iv;
		int sum_minute=0;
		int count = 0;
		for(int min=480;min<960;min=min+(*nSampletime),count++)//5分钟间隔
		{
			//int h=min/60;
			sunPosition = m_vSunPosition[count];
			m_ls->reset();
			m_ls->setStart(sampPoint);
			m_ls->setEnd(sunPosition);
			m_regionNodeRoot->accept(*m_iv);
			if(!m_ls->containsIntersections())//无碰撞 
				sum_minute=sum_minute+(*nSampletime);
		}
		//	sunPosition= ep->getSunPositionECEF(osgEarth::DateTime(m_date->year(),m_date->month(),m_date->day(), h-8+(min-h*60)/60.0));
		//	osg::ref_ptr<osgUtil::LineSegmentIntersector> ls = new osgUtil::LineSegmentIntersector(sampPoint,sunPosition);
		//	osg::ref_ptr<osgUtil::IntersectionVisitor> _iv = new osgUtil::IntersectionVisitor(ls); 
		//	viewer->getCamera()->accept(*_iv);
		//	if(ls->containsIntersections())//有碰撞 
		//	{
		//		osgUtil::LineSegmentIntersector::Intersections inte=ls->getIntersections();
		//		if(inte.size()==1)//交点为1，碰到自身，无遮挡
		//			sum_minute=sum_minute+5;
				//else交点大于1，有碰撞，有遮挡，不计入日照时间
		//	}
		//	else//无碰撞无遮挡
		//		sum_minute=sum_minute+5;
		//}
		//if(sum_minute<120)//日照时间小于2小时
			//ColorChange = true;
		SunshineMinute.push_back(sum_minute);
	}
	//一栋模型的一天平均日照量
	for(int i=0;i<SunshineMinute.size();i++)
	{
		aveMinute+=SunshineMinute.at(i);
	}
	if(SunshineMinute.size()==0)//假如没有选中模型
		aveMinute=0;
	else
		aveMinute=aveMinute/SunshineMinute.size();
	//QString s = QString::number(aveMinute);
	m_regionNodeRoot->asGroup()->addChild(_node);
	//QString ave_time=dt.toUTC().toString("yyyy-MM-dd hh:mm:ss").mid(11,5);
	//QMessageBox::information(NULL, "一天平均日照量", ave_time+"  共"+s+"分钟");
	//**
	return(aveMinute);
}

int CSunLightAnalysis::WindowAnalysis(GeoGlobe::Model::LodModel* lodmodel,int* windowditance,int* nSampletime,char* filepath)
{
		
	osgEarth::SpatialReference *MapWGS84SRS = osgEarth::SpatialReference::get("wgs84");
	osgEarth::GeoPoint Centerlatlon;
	Centerlatlon.fromWorld(MapWGS84SRS,lodmodel->_options->_centerPos);

	unsigned long long uKey = lodmodel->_options->_feature->m_nFID;
	osg::ref_ptr<osg::Node> _node;
	for(int i=0;i<m_regionNodeRoot->asGroup()->getNumChildren();i++)
		{
		_node = m_regionNodeRoot->asGroup()->getChild(i);
		osg::ref_ptr<GeoGlobe::Model::LodModel> _modelnode =  dynamic_cast<GeoGlobe::Model::LodModel*>(_node.get());
		if(uKey == _modelnode->_options->_feature->m_nFID)
			{
			m_regionNodeRoot->asGroup()->removeChild(_node);
			break;
		}
	}

	int sunTime = calculate(Centerlatlon,windowditance,nSampletime,filepath);
	m_regionNodeRoot->asGroup()->addChild(_node);
				//else交点大于1，有碰撞，有遮挡，不计入日照时间
	return(sunTime);
			}

int CSunLightAnalysis::WindowAnalysisCMF(CModelF* CMFmodel,int* windowditance,int* nSampletime,char* filepath)
{
	osgEarth::SpatialReference *MapWGS84SRS = osgEarth::SpatialReference::get("wgs84");
	osgEarth::GeoPoint Centerlatlon;
	Centerlatlon.fromWorld(MapWGS84SRS,CMFmodel->getBound().center());//获取CMF模型的中心点坐标

	unsigned long long* uKey = CMFmodel->_mukey;
	osg::ref_ptr<osg::Node> _node;
	for(int i=0;i<m_regionNodeRoot->asGroup()->getNumChildren();i++)
	{
		_node = m_regionNodeRoot->asGroup()->getChild(i);
		osg::ref_ptr<CModelF> _modelnode =  dynamic_cast<CModelF*>(_node.get());
		if(*uKey == *(_modelnode->_mukey))
		{
			m_regionNodeRoot->asGroup()->removeChild(_node);
			break;
		}
	}

	int sunTime = calculate(Centerlatlon,windowditance,nSampletime,filepath);
	m_regionNodeRoot->asGroup()->addChild(_node);
	return(sunTime);
}

//初始化（太阳位置/选中的模型组/相交线）
int CSunLightAnalysis::calculate(osgEarth::GeoPoint& CenterLatlon,int* windowditance,int* Sampletime,char* filepath)
{
	FILE * shp=fopen(filepath,"rb");
	CShapefileRY ShapefileR;
	ShapefileR.Read(shp);
	fclose(shp);
	osg::ref_ptr<osg::Vec3dArray> PointArr = ShapefileR._FPointsArr;//坐标序列
	std::vector<int> PointNum = ShapefileR._PointNum;//面坐标点数序列
	int min=0;//窗户四角日照量的最小值
	osgEarth::SpatialReference *MapWGS84SRS = osgEarth::SpatialReference::get("wgs84");
	int num=0;//shp中第几个面
	for(int i=0;i<PointNum.size()-1;i++)
	{
		int c=0;
		for(int j=PointNum.at(i);j<PointNum.at(i+1)-1;j++)
		{
			osg::Vec3d p1=PointArr->at(j);
			osg::Vec3d p2=PointArr->at(j+1);
			if(((p1.y()>CenterLatlon.y()) != (p2.y()>CenterLatlon.y()))&&   
				(CenterLatlon.x()<(p2.x()-p1.x())*(CenterLatlon.y()-p1.y())/(p2.y()-p1.y())+p1.x()))
				c=c+1;
		}
		if(c%2!=0)//水平向右引射线，若交点为奇数个，在该多边形内
		{
			num=i;
			break;
		}	
	}
	for(int i=PointNum.at(num);i<PointNum.at(num+1)-1;i++)
	{
		osg::Vec3d p1=PointArr->at(i);//经纬度
		osg::Vec3d p2=PointArr->at(i+1);
		if(p1.x()<=p2.x())//首先删除朝北的线段
			continue;
		osg::Vec3d p0;
		osg::Vec3d p3;
		if(i>PointNum.at(num) && i<PointNum.at(num+1)-2)//朝南的面中选出凸点参与采样计算
		{
			p0=PointArr->at(i-1);
			p3=PointArr->at(i+2);
		}
		if(i==PointNum.at(num))
		{
			int ii=PointNum.at(num+1)-2;
			p0=PointArr->at(ii);
			p3=PointArr->at(i+2);
		}
		if(i==PointNum.at(num+1)-2)
		{
			int ii=PointNum.at(num)+1;
			p0=PointArr->at(i-1);
			p3=PointArr->at(ii);
	}
		double s1=(p1.x()-p0.x())*(p1.y()-p2.y())-(p1.y()-p0.y())*(p1.x()-p2.x());
		double s2=(p2.x()-p1.x())*(p2.y()-p3.y())-(p2.y()-p1.y())*(p2.x()-p3.x());
		if(s1<=0||s2<=0)//顺时针多边形，S>0为凸点
			continue;
		osg::Vec2d p12=(osg::Vec2(p2.x()-p1.x(),p2.y()-p1.y()));
		osg::Vec2d pnorth=(osg::Vec2(0.0,1.0));
		double cosr=p12.y()/sqrt(p12.x()*p12.x()+p12.y()*p12.y());
		double angl=acos(cosr)*180/3.1415926;
		if(angl>=30 && angl<=150)
		{
			p1.z()+=0.9;//从高于地脚线0.9m处计算
			p2.z()+=0.9;
			GeoPoint latlon1(MapWGS84SRS, p1, ALTMODE_ABSOLUTE);
			osg::Vec3d w1;
			latlon1.toWorld(w1);
			GeoPoint latlon2(MapWGS84SRS, p2, ALTMODE_ABSOLUTE);
			osg::Vec3d w2;
			latlon2.toWorld(w2);
			double Length2 = (w1.x()-w2.x())*(w1.x()-w2.x())+(w1.y()-w2.y())*(w1.y()-w2.y())+(w1.z()-w2.z())*(w1.z()-w2.z());
			double Length = sqrt(Length2);
			osg::Vec3d sample_point1;
			osg::Vec3d sample_point2;
			osg::Vec3d sample_point3;
			osg::Vec3d sample_point4;
			if(Length>(*windowditance)+2.0)//墙面长度>窗户间距+窗户长度，才有窗户存在
			{
				sample_point1 = w1;
				int j=1;
				while(( (sample_point1.x()-w1.x())*(sample_point1.x()-w1.x())+(sample_point1.y()-w1.y())*(sample_point1.y()-w1.y())+(sample_point1.z()-w1.z())*(sample_point1.z()-w1.z())) <= Length2  )
				{
					osg::Vec3dArray* WorldPoint=new osg::Vec3dArray;//存所有采样点的世界坐标
					sample_point1.x()=w1.x()+(w2.x()-w1.x())*(*windowditance+2.0)*j/Length;
					sample_point1.y()=w1.y()+(w2.y()-w1.y())*(*windowditance+2.0)*j/Length;
					sample_point1.z()=w1.z()+(w2.z()-w1.z())*(*windowditance+2.0)*j/Length;					
					sample_point2.x()=sample_point1.x()-(w2.x()-w1.x())*2.0/Length;
					sample_point2.y()=sample_point1.y()-(w2.y()-w1.y())*2.0/Length;
					sample_point2.z()=sample_point1.z()-(w2.z()-w1.z())*2.0/Length;

					osgEarth::GeoPoint lat1;
					lat1.fromWorld(MapWGS84SRS,sample_point1);
					lat1.z()+=1.5;
					lat1.toWorld(sample_point4);

					osgEarth::GeoPoint lat2;
					lat2.fromWorld(MapWGS84SRS,sample_point2);
					lat2.z()+=1.5;
					lat2.toWorld(sample_point3);
					WorldPoint->push_back(sample_point1);
					WorldPoint->push_back(sample_point2);
					WorldPoint->push_back(sample_point3);
					WorldPoint->push_back(sample_point4);
					min = complete(sample_point1,*Sampletime);
					for(int m=1;m<WorldPoint->size();m++)
					{
						osg::Vec3d sampPoint = WorldPoint->at(m);
						int summinute = complete(sampPoint,*Sampletime);
						if(summinute < min)
							min = summinute;
					}
					if(min>=120)
						return(min);
					j++;
				}
}
		}
	}
	return(min);
}

int CSunLightAnalysis::complete(osg::Vec3d &vPoint, int nSampletime)
{
	
	osg::Vec3d sunPosition;
	osgUtil::IntersectVisitor iv;
	int sum_minute=0;
	int count = 0;
	for(int min=480;min<960;min=min+(nSampletime),count++)//5分钟间隔
	{
		//int h=min/60;
		sunPosition = m_vSunPosition[count];

		//相交线重置（不重置交点信息会不停累积）
		m_ls->reset();
		//设置起点终点
		m_ls->setStart(vPoint);
		m_ls->setEnd(sunPosition);
		//区域范围的根节点与之相交
		m_regionNodeRoot->accept(*m_iv);
		//m_viewer->getCamera()->accept(*m_iv);

		//m_regionNodeRoot已做了筛选，则此次不需判断相交次数
		if(!m_ls->containsIntersections())//无碰撞 
			sum_minute=sum_minute+(nSampletime);
	}

	return(sum_minute);
}


void CSunLightAnalysis::OcclusionQuery(GeoGlobe::Model::LodModel* lodmodel,int* nSampletime)
{
	osgEarth::SpatialReference *MapWGS84SRS = osgEarth::SpatialReference::get("wgs84");
	unsigned long long myKey = lodmodel->_options->_feature->m_nFID;
	unsigned long long uKey=0;
	//取模型包围盒边界的7个点
	osg::Vec3d boxmax = lodmodel->_options->_feature->m_MaxVertex;
	osg::Vec3d boxmin = lodmodel->_options->_feature->m_MinVertex;
	osg::Vec3d p1=(osg::Vec3(boxmin.x(),boxmax.y(),boxmin.z()+0.9));
	osg::Vec3d p2=(osg::Vec3(boxmin.x(),(boxmax.y()+boxmin.y())/2.0,boxmin.z()+0.9));
	osg::Vec3d p3=(osg::Vec3(boxmin.x(),boxmin.y(),boxmin.z()+0.9));
	osg::Vec3d p4=(osg::Vec3((boxmin.x()+boxmax.x())/2.0,boxmin.y(),boxmin.z()+0.9));
	osg::Vec3d p5=(osg::Vec3(boxmax.x(),boxmin.y(),boxmin.z()+0.9));
	osg::Vec3d p6=(osg::Vec3(boxmax.x(),(boxmax.y()+boxmin.y())/2.0,boxmin.z()+0.9));
	osg::Vec3d p7=(osg::Vec3(boxmax.x(),boxmax.y(),boxmin.z()+0.9));
	osg::Vec3dArray* latlonArr=new osg::Vec3dArray;
	latlonArr->push_back(p1);
	latlonArr->push_back(p2);
	latlonArr->push_back(p3);
	latlonArr->push_back(p4);
	latlonArr->push_back(p5);
	latlonArr->push_back(p6);
	latlonArr->push_back(p7);
	osg::Vec3dArray* Sample_PointArr=new osg::Vec3dArray;//存7个点的世界坐标
	for(int i=0;i<7;i++)
	{
		osg::Vec3d pi=latlonArr->at(i);
		GeoPoint latlon(MapWGS84SRS, pi, ALTMODE_ABSOLUTE);
		osg::Vec3d p;
		latlon.toWorld(p);
		Sample_PointArr->push_back(p);
	}

	//逐点计算，提取遮挡物ID
	for(int m=0;m<Sample_PointArr->size();m++)
	{
		osg::Vec3d sampPoint=Sample_PointArr->at(m);

	    osg::Vec3d sunPosition;
	    osgUtil::IntersectVisitor iv;
		int sum_minute=0;
		int count = 0;
		for(int min=480;min<960;min=min+(*nSampletime),count++)//5分钟间隔
	    {
			//int h=min/60;
			sunPosition = m_vSunPosition[count];

			//相交线重置（不重置交点信息会不停累积）
			m_ls->reset();
			m_ls->setStart(sampPoint);
			m_ls->setEnd(sunPosition);
			m_regionNodeRoot->accept(*m_iv);
			if(m_ls->containsIntersections())//碰撞 
		    {
				osgUtil::LineSegmentIntersector::Intersections intersections=m_ls->getIntersections();
					for(osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();
					hitr != intersections.end();
					++hitr)
					{
						osg::NodePath nodePath = hitr->nodePath;
						for (int i=0; i<nodePath.size();i++)
						{
							osg::ref_ptr<osg::Node> node = nodePath[i];
							if (node == NULL)  continue;
							osg::ref_ptr<GeoGlobe::Model::LodModel> modelnode =  dynamic_cast<GeoGlobe::Model::LodModel*>(node.get());
						    if (!modelnode)  continue;

						    uKey = modelnode->_options->_feature->m_nFID;
						    break;
						}
						//剔除重复的ID和被遮挡物自身ID，存入OcclusionId
						bool sameID = false;
						if(OcclusionId.size()==0 && uKey != myKey)
						{
							OcclusionId.push_back(uKey);
							sameID=true;
						}
						else
						{
							for(int i=0;i<OcclusionId.size();i++)
							{
								if(uKey==OcclusionId.at(i))
								{
									sameID=true;
									break;
								}
							}
						}
						if(sameID == false && uKey != myKey)
							OcclusionId.push_back(uKey);
					}
				}
			}
		}
	}

//}
