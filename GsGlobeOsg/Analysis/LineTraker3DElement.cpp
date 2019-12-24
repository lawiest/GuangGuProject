#include <osgEarth/MapNode>
#include <osg/AutoTransform>
#include <osgViewer/View>
#include <osg/io_utils>
#include <osgGA/EventVisitor>
#include <Analysis/LineTraker3DElement.h>
#include <tool/CodeTransformHelp.h>

#include <osgSim/SphereSegment>
#include <osgUtil/SmoothingVisitor>
#include <osg/io_utils>

using namespace osgEarth::Annotation;
using namespace osgEarth::Symbology;

GeoGlobe::LineTraker3DElement::LineTraker3DElement()
{

}

void GeoGlobe::LineTraker3DElement::StartAnalysis(MapNode* mapNode, const DragMode& defaultMode)
{
	//LineTraker3DElement(mapNode, defaultMode);
}

GeoGlobe::LineTraker3DElement::LineTraker3DElement( MapNode* mapNode, const DragMode& defaultMode ):
	_position( mapNode->getMapSRS(), 0,0,0, ALTMODE_RELATIVE),
	_defaultMode(defaultMode)
{    
	//光照
	getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	//Traker的状态
	_Finished = false;
	//记录交互操作的步骤，鼠标按下获取第一个点，抬起后开始Traker，在单击结束Traker
	_nStep = 0;

	//点的初始化
	_SphereElement = new SphereElement(mapNode);
	addChild(_SphereElement);

	setNumChildrenRequiringEventTraversal( 1 );

	//用于计算垂直投影
	_projector = new osgManipulator::LineProjector;
	
	//设置MapNode
	setMapNode( mapNode );

	//创建显示距离的Label
	CreateLabel();
}

GeoGlobe::LineTraker3DElement::~LineTraker3DElement()
{
	setMapNode( 0L );
}

void GeoGlobe::LineTraker3DElement::setMapNode( MapNode* mapNode )
{
	MapNode* oldMapNode = getMapNode();

	if ( oldMapNode != mapNode )
	{
		_mapNode = mapNode;
	}
}

const GeoPoint& GeoGlobe::LineTraker3DElement::getPosition() const
{
	return _position;
}

void GeoGlobe::LineTraker3DElement::setPosition( const GeoPoint& position, bool fireEvents)
{
	if (_position != position)
	{
		_position = position;
		_SphereElement->setPosition(position);

		if ( fireEvents )
			firePositionChanged();
	}
}

void GeoGlobe::LineTraker3DElement::firePositionChanged()
{
	for( PositionChangedCallbackList::iterator i = _callbacks.begin(); i != _callbacks.end(); i++ )
	{
		i->get()->onPositionChanged(this, _position);
	}
}    

void GeoGlobe::LineTraker3DElement::addPositionChangedCallback( PositionChangedCallback* callback )
{
	_callbacks.push_back( callback );
}

void GeoGlobe::LineTraker3DElement::removePositionChangedCallback( PositionChangedCallback* callback )
{
	PositionChangedCallbackList::iterator i = std::find( _callbacks.begin(), _callbacks.end(), callback);
	if (i != _callbacks.end())
	{
		_callbacks.erase( i );
	}    
}

void GeoGlobe::LineTraker3DElement::traverse(osg::NodeVisitor& nv)
{
	if (nv.getVisitorType() == osg::NodeVisitor::EVENT_VISITOR)
	{
		osgGA::EventVisitor* ev = static_cast<osgGA::EventVisitor*>(&nv);
		for (osgGA::EventQueue::Events::iterator itr = ev->getEvents().begin();
			itr != ev->getEvents().end();
			++itr)
		{
			osgGA::GUIEventAdapter* ea = dynamic_cast<osgGA::GUIEventAdapter*>(itr->get());
			if (ea && handle(*ea, *(ev->getActionAdapter())))
				ea->setHandled(true);
		}
	}
	osg::Group::traverse(nv);
}

bool GeoGlobe::LineTraker3DElement::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	if (ea.getHandled()) return false;

	osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
	if (!view) return false;
	if (!_mapNode.valid()) return false;

	if (_Finished)
		return false;

	if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON && _nStep == 0)
	{
		double lon, lat, h;
		if (getLocationAt(view, ea.getX(), ea.getY(), lon, lat,h))
		{
			_screenStart = GeoPoint(getMapNode()->getMapSRS(),ea.getX(),ea.getY(),h);
			_nStep++;//0->1
			_start = GeoPoint(getMapNode()->getMapSRS(),lon,lat,h,ALTMODE_ABSOLUTE);
			setPosition( _start, false );
			_start.toWorld(_startWorld);
			draw();

			_pointer.reset();
			// set movement range
			// TODO: values 0.0 and 300000.0 are rather experimental
			GeoPoint posStart(_position.getSRS(), _position.x(), _position.y(), 0.0, ALTMODE_ABSOLUTE);
			osg::Vec3d posStartXYZ;
			posStart.toWorld(posStartXYZ);

			GeoPoint posEnd(_position.getSRS(), _position.x(), _position.y(), 300000.0, ALTMODE_ABSOLUTE);
			osg::Vec3d posEndXYZ;
			posEnd.toWorld(posEndXYZ);

			_projector->setLine(posStartXYZ, posEndXYZ);

			osg::Camera *rootCamera = view->getCamera();
			_pointer.setCamera(rootCamera);
			_pointer.setMousePosition(ea.getX(), ea.getY());

			std::ostringstream s1;
			s1 <<0.0<<"米"<<"\n";
			std::string na = CodeHelp::String_To_UTF8(s1.str());
			m_ptrLabelNode->setPosition(_start);
			m_ptrLabelNode->setText(na);
			m_ptrLabelNode->setStyle(m_SymStyle);
		}
	}

	if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
	{
		if (_nStep == 1)
		{
			_nStep++;
			return false;
		}

		double dbReasult = _end.z() - _start.z();

		OnFinish(dbReasult);

// 		if (m_ptrAnalysisCallBack)
// 			m_ptrAnalysisCallBack->OnFinish(dbReasult);
		
		firePositionChanged();
		_Finished = true;
	}

	if (ea.getEventType() == osgGA::GUIEventAdapter::MOVE && _nStep == 2)
	{
		_pointer._hitIter = _pointer._hitList.begin();
		_pointer.setMousePosition(ea.getX(), ea.getY());

		_screenEnd = GeoPoint(getMapNode()->getMapSRS(),ea.getX(),ea.getY(),0);

		if (_projector->project(_pointer, _startProjectedPoint) && _defaultMode == LineTraker3DElement::DRAGMODE_VERTICAL) 
		{
			//Get the absolute mapPoint that they've drug it to.
			GeoPoint projectedPos;
			projectedPos.fromWorld(_position.getSRS(), _startProjectedPoint);

			// make sure point is not dragged down below
			// TODO: think of a better solution / HeightAboveTerrain performance issues?
			if (projectedPos.z() >= 0.0)
			{
				//If the current position is relative, we need to convert the absolute world point to relative.
				//If the point is absolute then just emit the absolute point.
				if (_position.altitudeMode() == ALTMODE_RELATIVE)
				{
					projectedPos.transformZ(ALTMODE_RELATIVE, getMapNode()->getTerrain());
				}
				//终点要比起点高
				if (projectedPos.z() < _start.z())
					return true;			

				setPosition( projectedPos );
				aa.requestRedraw();

				_end = projectedPos;
				_end.toWorld(_endWorld);
				//重新绘制
				draw();

				std::ostringstream s1;
				
				ostringstream osm;  
				// 浮点数转换限制  
				double tmp = _end.z() - _start.z();  
				osm.precision(2);  
				osm.setf(std::ios::fixed); // 将浮点数的位数限定为小数点之后的位数  
				osm << tmp;   

				s1 <<osm.str()<<"米"<<"\n";
				std::string na = CodeHelp::String_To_UTF8(s1.str());
				m_ptrLabelNode->setPosition(_end);
				m_ptrLabelNode->setText(na);
				m_ptrLabelNode->setStyle(m_SymStyle);
			}
		}
		else if (_defaultMode == LineTraker3DElement::DRAGMODE_HORIZONTAL)
		{
			osg::Vec3d world;
			if ( getMapNode() && getMapNode()->getTerrain()->getWorldCoordsUnderMouse(view, ea.getX(), ea.getY(), world) )
			{
				//Get the absolute mapPoint that they've drug it to.
				GeoPoint mapPoint;
				mapPoint.fromWorld( getMapNode()->getMapSRS(), world );
				//_mapNode->getMap()->worldPointToMapPoint(world, mapPoint);

				//If the current position is relative, we need to convert the absolute world point to relative.
				//If the point is absolute then just emit the absolute point.
				if (_position.altitudeMode() == ALTMODE_RELATIVE)
				{
					mapPoint.alt() = _position.alt();
					mapPoint.altitudeMode() = ALTMODE_RELATIVE;
				}

				setPosition( mapPoint );
				aa.requestRedraw();

				_end = mapPoint;
				_end.toWorld(_endWorld);
				//重新绘制
				draw();
			}
		}
		return true;
	}
	return false;
}

const GeoPoint& GeoGlobe::LineTraker3DElement::getScreenEnd() const
{
	return _screenEnd;
}

// void GeoGlobe::LineTraker3DElement::AddAnalysisCallBack(GLOBE_NAME::AnalysisCallBack* ptrCallBack)
// {
// 	m_ptrAnalysisCallBack = ptrCallBack;
// }

bool GeoGlobe::LineTraker3DElement::getLocationAt(osgViewer::View* view, double x, double y, double &lon, double &lat, double &h)
{
	osgUtil::LineSegmentIntersector::Intersections results;            
	if ( getMapNode() &&  view->computeIntersections( x, y, results, 0x1 ) )
	{
		// find the first hit under the mouse:
		osgUtil::LineSegmentIntersector::Intersection first = *(results.begin());
		osg::Vec3d point = first.getWorldIntersectPoint();

		double lat_rad, lon_rad, height;       
		getMapNode()->getMap()->getProfile()->getSRS()->getEllipsoid()->convertXYZToLatLongHeight( 
			point.x(), point.y(), point.z(), lat_rad, lon_rad, height );

		lat = osg::RadiansToDegrees( lat_rad );
		lon = osg::RadiansToDegrees( lon_rad );
		h = height;
		return true;
	}
	return false;
}

void GeoGlobe::LineTraker3DElement::setColor( const osg::Vec4f& color )
{

}

const GeoPoint& GeoGlobe::LineTraker3DElement::getStart() const
{
	return _start;
}

const osg::Vec3d& GeoGlobe::LineTraker3DElement::getStartWorld() const
{
	return _startWorld;
}

void GeoGlobe::LineTraker3DElement::setStart(const GeoPoint& start)
{
	_start = start;
	_start.toWorld(_startWorld);
	draw();
}

const GeoPoint& GeoGlobe::LineTraker3DElement::getEnd() const
{
	return _end;
}

const GeoPoint& GeoGlobe::LineTraker3DElement::getScreenStart() const
{
	return _screenStart;
}

const osg::Vec3d& GeoGlobe::LineTraker3DElement::getEndWorld() const
{
	return _endWorld;
}

void GeoGlobe::LineTraker3DElement::setEnd(const GeoPoint& end)
{
	_end = end;
	_end.toWorld(_endWorld);
	draw();
}


void GeoGlobe::LineTraker3DElement::draw()
{
	osg::ref_ptr<osg::MatrixTransform> ptrMt = 0L;

	osg::ref_ptr<osg::Geometry> LineGeometry;
	osg::ref_ptr<osg::Geode> LineGeode;

	if (_start != _end)
	{
		//初始化几何
		LineGeometry = new osg::Geometry;
		LineGeode = new osg::Geode;
		LineGeode->addDrawable(LineGeometry.get());

		LineGeometry->setUseDisplayList(false);
		LineGeometry->setUseVertexBufferObjects(true);


		osg::Vec3Array* verts = new osg::Vec3Array();
		verts->reserve(2);
		LineGeometry->setVertexArray(verts);

		//颜色初始化
		osg::Vec4Array* colors = new osg::Vec4Array();
		colors->reserve( 2 );
		LineGeometry->setColorArray( colors );
		colors->push_back( osg::Vec4(0.0f,1.0f,0.0f,1.0f) );
		colors->push_back( osg::Vec4(0.0f,1.0f,0.0f,1.0f) );
		LineGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

		//顶点初始化
		_start.toWorld(_startWorld);
		_end.toWorld(_endWorld);

		verts->push_back(_startWorld - _startWorld);
		verts->push_back(_endWorld - _startWorld);

		LineGeometry->addPrimitiveSet( new osg::DrawArrays( GL_LINES, 0, verts->size()) ); 

		ptrMt = new osg::MatrixTransform;
		ptrMt->setMatrix(osg::Matrixd::translate(_startWorld));
		ptrMt->addChild(LineGeode);

		getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	}

	//Remove all children from this group
	removeChildren(2, getNumChildren());

	if (ptrMt)
		addChild(ptrMt);
}


bool GeoGlobe::LineTraker3DElement::IsFinished()
{
	return _Finished;
}

void GeoGlobe::LineTraker3DElement::CreateLabel()
{
	m_SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->size()=18.0f;
	//m_SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->halo()->color()=Color::White;
	m_SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->fill()->color()=Color::White;
	m_SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->font() = "msyh.ttc";
	m_SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->encoding() = osgEarth::Symbology::TextSymbol::ENCODING_UTF8;
	m_SymStyle.getOrCreate<osgEarth::IconSymbol>()->declutter() = false;
	m_SymStyle.getOrCreate<osgEarth::TextSymbol>()->declutter() = false;

	std::ostringstream s1;
	s1 <<0.0<<"米"<<"\n";
	std::string na = CodeHelp::String_To_UTF8(s1.str());
	m_ptrLabelNode = new LabelNode(_start, na, m_SymStyle);
	m_ptrLabelNode->setDynamic( true );
	m_ptrLabelNode->setCullingActive(true);
	addChild(m_ptrLabelNode);
}

