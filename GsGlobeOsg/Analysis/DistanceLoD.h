#pragma once
#include <osg/LOD>
namespace osg

{
	enum DistanceLODMODE
	{
		// д╛ходёй╫
		DISTANCE,
		DISTANCECHILD
	};

	class DistanceLOD:public osg::LOD
	{
	public:
		DistanceLOD(DistanceLODMODE lodModel=DISTANCE);
		~DistanceLOD();

		virtual void traverse(NodeVisitor& nv)override;

		bool Visible();
		void Visible(bool visible);

	private:

		bool m_isVisible;
		DistanceLODMODE m_LodMode;

	};
}
