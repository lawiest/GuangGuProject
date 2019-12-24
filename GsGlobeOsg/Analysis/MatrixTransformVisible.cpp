#include "MatrixTransformVisible.h"

GeoGlobe::MatrixTransformVisible::MatrixTransformVisible()
{
	m_isVisible = true;
}

GeoGlobe::MatrixTransformVisible::MatrixTransformVisible(const MatrixTransform &matr, const osg::CopyOp & copyop)
	:MatrixTransform(matr,copyop)
{
}

GeoGlobe::MatrixTransformVisible::MatrixTransformVisible(const osg::Matrix & matix)
	:MatrixTransform(matix)
{
}

GeoGlobe::MatrixTransformVisible::~MatrixTransformVisible()
{
}

void GeoGlobe::MatrixTransformVisible::traverse(osg::NodeVisitor & nv)
{
	if (nv.getTraversalMode() == osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN )
	{
		if (m_isVisible)
			Group::traverse(nv);
	}
	else
	{
		Group::traverse(nv);
	}
}

void GeoGlobe::MatrixTransformVisible::setVisible(bool visible)
{
	m_isVisible = visible;
}

bool GeoGlobe::MatrixTransformVisible::Visible()
{
	return m_isVisible;
}
