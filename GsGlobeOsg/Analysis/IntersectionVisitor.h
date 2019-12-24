#ifndef GEOGLOBE_INTERSECTION_H
#define GEOGLOBE_INTERSECTION_H 1

#include <osg/NodeVisitor>
#include <osg/Drawable>
#include <osgUtil/Export>
#include <list>

namespace GeoGlobe
{
	class IntersectionVisitor;

	class Intersector : public osg::Referenced
	{
	public:
		Intersector()
		{
		}

		virtual Intersector* clone(GeoGlobe::IntersectionVisitor& iv) = 0;

		virtual bool enter(const osg::Node& node) = 0;

		virtual void leave() = 0;

		virtual void intersect(GeoGlobe::IntersectionVisitor& iv, osg::Node* node) = 0;

		virtual void reset() { }

		virtual bool containsIntersections() = 0;
	};

	class IntersectionVisitor : public osg::NodeVisitor
	{
		public:
			struct ReadCallback : public osg::Referenced
			{
				virtual osg::Node* readNodeFile(const std::string& filename) = 0;
			};


			IntersectionVisitor(Intersector* intersector=0, ReadCallback* readCallback=0);

			META_NodeVisitor(osgUtil, IntersectionVisitor)

			virtual void reset();


			/** Set the intersector that will be used to intersect with the scene, and to store any hits that occur.*/
			void setIntersector(Intersector* intersector);

			/** Get the intersector that will be used to intersect with the scene, and to store any hits that occur.*/
			Intersector* getIntersector() { return _intersectorStack.empty() ? 0 : _intersectorStack.front().get(); }

			/** Get the const intersector that will be used to intersect with the scene, and to store any hits that occur.*/
			const Intersector* getIntersector() const { return _intersectorStack.empty() ? 0 : _intersectorStack.front().get(); }

			/** Set the read callback.*/
			void setReadCallback(ReadCallback* rc) { _readCallback = rc; }

			/** Get the read callback.*/
			ReadCallback* getReadCallback() { return _readCallback.get(); }

			/** Get the const read callback.*/
			const ReadCallback* getReadCallback() const { return _readCallback.get(); }

		public:

			virtual void apply(osg::Node& node);
			virtual void apply(osg::Geode& geode);
			virtual void apply(osg::Group& group);
			virtual void apply(osg::LOD& lod);
			virtual void apply(osg::PagedLOD& lod);
			virtual void apply(osg::MatrixTransform& node);

		protected:

			inline bool enter(const osg::Node& node) { 
				return _intersectorStack.empty() ? false : _intersectorStack.back()->enter(node); 
			}
			inline void leave() { _intersectorStack.back()->leave(); }
			inline void intersect(osg::Node* node) { 
				_intersectorStack.back()->intersect(*this, node);
			}

			typedef std::list< osg::ref_ptr<Intersector> > IntersectorStack;
			IntersectorStack _intersectorStack;

			osg::ref_ptr<ReadCallback> _readCallback;
	};
} // namespace GeoGlobe

#endif