#include "FlagSymbol.h"
#include <osgUtil/SmoothingVisitor>
#include <osg/Depth>
#include <osgText/Font>
#include <osgText/Text3D>
#include "tool/CodeTransformHelp.h"
#include <osgEarthAnnotation/AnnotationUtils>
#include "innerimp.h"
#include <osgEarth/GeoTransform>
#include <osg/Array>
#include <osgEarth/MapNode>
#include <OsgCoreSingleton.h>

GLOBE_NS

FlagSymbol::FlagSymbol(double dLongitude, double dLatitude, double dHeight)
{
	m_ptrFlagGeometry = NULL;
	m_ptrLocatorNode = NULL;
	m_ptrVectices = NULL;
	m_ptrTrans = NULL;
	m_ptrText = NULL;

	m_dbLongitude = dLongitude;
	m_dbLatitude = dLatitude;
	m_dbHeight = dHeight;
	m_nIndex = 0;
}

FlagSymbol::~FlagSymbol()
{

}

void FlagSymbol::Build(osgEarth::MapNode* ptrMapNode)
{
	m_pMapNode = ptrMapNode;
	CreateFlag(m_dbLongitude, m_dbLatitude, m_dbHeight);
}

void FlagSymbol::Position(KERNEL_NAME::GsRawPoint3D gsPosition)
{
	if (NULL == m_ptrLocatorNode)
		return;

	osgEarth::GeoPoint point = osgEarth::GeoPoint(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS(), osg::Vec3d(gsPosition.X, gsPosition.Y, gsPosition.Z));
	m_ptrLocatorNode->CastTo<osg::ref_ptr<osgEarth::GeoTransform> >()->setPosition(point);
}

KERNEL_NAME::GsRawPoint3D FlagSymbol::Position()
{
	if (NULL == m_ptrLocatorNode)
		return GsRawPoint3D(0.0,0.0,0.0);

	osgEarth::GeoPoint dPosition =m_ptrLocatorNode->CastTo<osg::ref_ptr<osgEarth::GeoTransform> >()->getPosition();
	return KERNEL_NAME::GsRawPoint3D(dPosition.x(), dPosition.y(), dPosition.z());
}

osg::Node* FlagSymbol::SymbolNode()
{
	if (NULL == m_ptrLocatorNode)
		return NULL;

	return m_ptrLocatorNode->CastTo<osg::ref_ptr<osgEarth::GeoTransform> >();
}

const std::string& UNIFORM_IS_TEXT()
{
	static std::string s = "oeAnno_isText";
	return s;
}

osg::Drawable* create2DQuad(const osg::BoundingBox& box, float padding, const osg::Vec4& color)
{
	osg::Geometry* geom = new osg::Geometry();
	geom->setUseVertexBufferObjects(true);

	osg::Vec3Array* v = new osg::Vec3Array();
	v->reserve(4);
	v->push_back(osg::Vec3(box.xMin() - padding, box.yMin() - padding, 0));
	v->push_back(osg::Vec3(box.xMax() + padding, box.yMin() - padding, 0));
	v->push_back(osg::Vec3(box.xMax() + padding, box.yMax() + padding, 0));
	v->push_back(osg::Vec3(box.xMin() - padding, box.yMax() + padding, 0));
	geom->setVertexArray(v);
	if (v->getVertexBufferObject())
		v->getVertexBufferObject()->setUsage(GL_STATIC_DRAW_ARB);

	osg::DrawElementsUByte* b = new osg::DrawElementsUByte(GL_TRIANGLES);
	b->reserve(6);
	b->push_back(0); b->push_back(1); b->push_back(2);
	b->push_back(2); b->push_back(3); b->push_back(0);
	geom->addPrimitiveSet(b);

	osg::Vec4Array* c = new osg::Vec4Array(1);
	(*c)[0] = color;
	geom->setColorArray(c);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);

	// add the static "isText=true" uniform; this is a hint for the annotation shaders
	// if they get installed.
	static osg::ref_ptr<osg::Uniform> s_isTextUniform = new osg::Uniform(osg::Uniform::BOOL, UNIFORM_IS_TEXT());
	s_isTextUniform->set(false);
	geom->getOrCreateStateSet()->addUniform(s_isTextUniform.get());

	return geom;
}

void FlagSymbol::CreateFlag(double x, double y, double z)
{
	if (NULL == m_pMapNode)
		return;

	osg::ref_ptr<osg::Geode> geode = new osg::Geode();

	osg::ref_ptr<osg::Geometry> ptrGeometry = new osg::Geometry();
	m_ptrFlagGeometry = new ReferenceImp<osg::Geometry>(ptrGeometry);

	osg::ref_ptr<osg::Vec3Array> ptrVectices = new osg::Vec3Array();
	m_ptrVectices = new ReferenceImp<osg::Vec3Array>(ptrVectices);

	osg::ref_ptr<osg::DrawElementsUShort> ptrIndex = new osg::DrawElementsUShort(osg::PrimitiveSet::TRIANGLES);

	CreateRectangle(0.0, 0.0, 0.0, 5.0, 5.0, 150.0, ptrIndex.get(), true);//Root
	CreateRectangle(5.0, 0.0, 150.0, 150.0, 5.0, 5.0, ptrIndex.get());//Bottom
	CreateRectangle(150.0, 0.0, 150.0, 5.0, 5.0, 105.0,  ptrIndex.get());//Right
	CreateRectangle(0.0, 0.0, 150.0, 5.0, 5.0, 100.0,  ptrIndex.get());//Left
	CreateRectangle(0.0, 0.0, 250.0, 155.0, 5.0, 5.0, ptrIndex.get());//Top


	osg::ref_ptr<osg::Drawable> boxfill = create2DQuad(osg::BoundingBox(0.0, 150.0, 3.0, 150.0, 250.0, 5), 0, osg::Vec4(1.0f, 1.0f, 1.0f, 0.4f));
	//开启透明
	boxfill->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	boxfill->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	//设置法线跟着缩放
	boxfill->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);

	std::string fontFile("./simhei.ttf");
	osg::ref_ptr<osgText::Font> font = osgText::readRefFontFile(fontFile);
	osg::ref_ptr<osgText::Text3D> text3D = new osgText::Text3D;

	m_ptrText = new ReferenceImp<osgText::Text3D>(text3D);

	text3D->setFont(font.get());
	//text3D->setStyle(style.get());
	text3D->setColor(osg::Vec4(0.0, 0.0, 0.0, 1));
	text3D->setCharacterSize(60.0);
	text3D->setDrawMode(osgText::Text3D::TEXT/* | osgText::Text3D::BOUNDINGBOX*/);
	text3D->setAxisAlignment(osgText::Text3D::XY_PLANE);
	text3D->setText(CodeHelp::String_To_UTF8("哈哈"), osgText::String::Encoding::ENCODING_UTF8);
	text3D->setPosition(osg::Vec3(20.0, 180.0, 5.0));

	//顶点
	ptrGeometry->setVertexArray(ptrVectices);
	//颜色
	osg::ref_ptr<osg::Vec4Array> color = new osg::Vec4Array;
	color->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	ptrGeometry->setColorArray(color.get(), osg::Array::BIND_OVERALL);
	//状态设置可alpha融合
	osg::ref_ptr<osg::StateSet> stateset = ptrGeometry->getOrCreateStateSet();
	stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
	//设置法线跟着缩放
	stateset->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);
	//stateset->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS, 0, 1, false), osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);
	stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	stateset->setRenderBinDetails(11, "RenderBin");
	//开启透明
	stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	//三角图元索引
	ptrGeometry->addPrimitiveSet(ptrIndex);
	//自动生成法线
	osgUtil::SmoothingVisitor::smooth(*ptrGeometry, osg::DegreesToRadians(30.0));

	geode->addDrawable(ptrGeometry);
	geode->addDrawable(text3D);
	geode->addDrawable(boxfill);

	//自动变换节点
	osg::ref_ptr<osg::AutoTransform> autoTrans = new osg::AutoTransform();
	m_ptrTrans = new ReferenceImp<osg::AutoTransform>(autoTrans);

	//osg::Matrixd matrixX;
	//matrixX.identity();
	//matrixX.makeRotate(-osg::PI / 2, osg::Vec3d(1.0, 0.0, 0.0));
	//autoTrans->setRotation(matrixX.getRotate());
	autoTrans->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
	//autoTrans->setAutoScaleToScreen(true);
	autoTrans->addChild(geode.release());
	autoTrans->setScale(4000);
	
	osg::ref_ptr<osgEarth::GeoTransform> ptrLocator = new osgEarth::GeoTransform();
	m_ptrLocatorNode = new ReferenceImp<osgEarth::GeoTransform>(ptrLocator);

	osgEarth::GeoPoint point = osgEarth::GeoPoint(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS(), osg::Vec3d(x, y, z));
	ptrLocator->setPosition(point);
	ptrLocator->addChild(autoTrans);
}

void FlagSymbol::Text(std::string strText)
{
	if (NULL == m_ptrText)
		return;

	m_ptrText->CastTo<osg::ref_ptr<osgText::Text3D>>()->setText(CodeHelp::String_To_UTF8(strText), osgText::String::Encoding::ENCODING_UTF8);
}

void FlagSymbol::Scale(double dbScale)
{
	if (NULL == m_ptrTrans)
		return;

	m_ptrTrans->CastTo<osg::ref_ptr<osg::AutoTransform> >()->setScale(dbScale);
}

void FlagSymbol::RotateToScreen(bool bRotate)
{
	if (NULL == m_ptrTrans)
		return;
	
	if (bRotate == true)
		m_ptrTrans->CastTo<osg::ref_ptr<osg::AutoTransform> >()->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
	else
		m_ptrTrans->CastTo<osg::ref_ptr<osg::AutoTransform> >()->setAutoRotateMode(osg::AutoTransform::NO_ROTATION);
}

void FlagSymbol::CreateRectangle(double x, double y, double z, double dLength, double dWidth, double dHeight, osg::DrawElementsUShort* ptrIndex, bool bIsFlagRoot)
{
	double dFlagRootPadding = 0;

	if (bIsFlagRoot)
		dFlagRootPadding = 5.0;

	m_ptrVectices->CastTo<osg::ref_ptr<osg::Vec3Array> >()->push_back(osg::Vec3(x, z + dFlagRootPadding, y));//0
	m_ptrVectices->CastTo<osg::ref_ptr<osg::Vec3Array> >()->push_back(osg::Vec3(x + dLength, z, y)); //1
	m_ptrVectices->CastTo<osg::ref_ptr<osg::Vec3Array> >()->push_back(osg::Vec3(x + dLength, z, y + dWidth)); //2
	m_ptrVectices->CastTo<osg::ref_ptr<osg::Vec3Array> >()->push_back(osg::Vec3(x, z + dFlagRootPadding, y + dWidth));//3

	m_ptrVectices->CastTo<osg::ref_ptr<osg::Vec3Array> >()->push_back(osg::Vec3(x, z + dHeight, y));//4
	m_ptrVectices->CastTo<osg::ref_ptr<osg::Vec3Array> >()->push_back(osg::Vec3(x + dLength, z + dHeight, y)); //5
	m_ptrVectices->CastTo<osg::ref_ptr<osg::Vec3Array> >()->push_back(osg::Vec3(x + dLength, z + dHeight, y + dWidth)); //6
	m_ptrVectices->CastTo<osg::ref_ptr<osg::Vec3Array> >()->push_back(osg::Vec3(x, z + dHeight, y + dWidth)); //7

	ptrIndex->push_back(0 + 8 * m_nIndex);
	ptrIndex->push_back(2 + 8 * m_nIndex);
	ptrIndex->push_back(1 + 8 * m_nIndex);
	ptrIndex->push_back(2 + 8 * m_nIndex);
	ptrIndex->push_back(0 + 8 * m_nIndex);
	ptrIndex->push_back(3 + 8 * m_nIndex);

	ptrIndex->push_back(0 + 8 * m_nIndex);
	ptrIndex->push_back(5 + 8 * m_nIndex);
	ptrIndex->push_back(1 + 8 * m_nIndex);
	ptrIndex->push_back(5 + 8 * m_nIndex);
	ptrIndex->push_back(0 + 8 * m_nIndex);
	ptrIndex->push_back(4 + 8 * m_nIndex);

	ptrIndex->push_back(0 + 8 * m_nIndex);
	ptrIndex->push_back(3 + 8 * m_nIndex);
	ptrIndex->push_back(4 + 8 * m_nIndex);
	ptrIndex->push_back(4 + 8 * m_nIndex);
	ptrIndex->push_back(3 + 8 * m_nIndex);
	ptrIndex->push_back(7 + 8 * m_nIndex);

	ptrIndex->push_back(4 + 8 * m_nIndex);
	ptrIndex->push_back(6 + 8 * m_nIndex);
	ptrIndex->push_back(5 + 8 * m_nIndex);
	ptrIndex->push_back(6 + 8 * m_nIndex);
	ptrIndex->push_back(4 + 8 * m_nIndex);
	ptrIndex->push_back(7 + 8 * m_nIndex);

	ptrIndex->push_back(1 + 8 * m_nIndex);
	ptrIndex->push_back(6 + 8 * m_nIndex);
	ptrIndex->push_back(2 + 8 * m_nIndex);
	ptrIndex->push_back(6 + 8 * m_nIndex);
	ptrIndex->push_back(1 + 8 * m_nIndex);
	ptrIndex->push_back(5 + 8 * m_nIndex);

	ptrIndex->push_back(2 + 8 * m_nIndex);
	ptrIndex->push_back(7 + 8 * m_nIndex);
	ptrIndex->push_back(3 + 8 * m_nIndex);
	ptrIndex->push_back(7 + 8 * m_nIndex);
	ptrIndex->push_back(2 + 8 * m_nIndex);
	ptrIndex->push_back(6 + 8 * m_nIndex);

	m_nIndex++;
}

//设置是否可见
void FlagSymbol::Visible(bool bVisible)
{
	if (NULL == m_ptrLocatorNode)
		return;

	unsigned int unMask;

	if (bVisible == true)
		unMask = 0xffffffff;
	else
		unMask = 0x00000000;

	m_ptrLocatorNode->CastTo<osg::ref_ptr<osgEarth::GeoTransform> >()->setNodeMask(unMask);
}

//获取是否可见
bool FlagSymbol::Visible() const
{
	if (NULL == m_ptrLocatorNode)
		return NULL;
	
	unsigned int unMask = m_ptrLocatorNode->CastTo<osg::ref_ptr<osgEarth::GeoTransform> >()->getNodeMask();

	if (unMask == 0)
		return false;
	else
		return true;
}

//获取图层类型
int FlagSymbol::UID()
{
	return 0;
}

GLOBE_ENDNS