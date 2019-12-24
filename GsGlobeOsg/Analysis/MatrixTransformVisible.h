#pragma once
#include "osg/MatrixTransform"

namespace GeoGlobe
{
	class MatrixTransformVisible :public osg::MatrixTransform
	{
	public:
		MatrixTransformVisible();
		/** Copy constructor using CopyOp to manage deep vs shallow copy.*/
		MatrixTransformVisible(const MatrixTransform&, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

		MatrixTransformVisible(const osg::Matrix& matix);
		~MatrixTransformVisible();
		// ÷ÿ–¥±È¿˙
		virtual void traverse(osg::NodeVisitor& nv);
		void setVisible(bool visible);
		bool Visible();

	protected:
		bool m_isVisible;

	};

}