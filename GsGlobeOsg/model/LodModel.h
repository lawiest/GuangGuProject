#ifndef GEOGLOBE_MODEL_LODMODEL
#define GEOGLOBE_MODEL_LODMODEL 1
#include <osg/PagedLOD>
#include <osg/CullStack>
#include <algorithm>
//#include <Model/ModelDriver.h>
#include <osg/ShapeDrawable>
#include <Model/Model.h>
#include <Tool/GlobalSetting.h>

//#include <QuadTreeNode/Model.h>

using namespace osg;
namespace GeoGlobe
{

	namespace Model
	{
		/*Lod模型，不包含模型的几何与纹理，只用于实现模型的LOD动态调度的功能类
		 研究该类之前建议先了解pagedlod，我只是在原来基础上做了稍微改写，其中最关键部分在traverse函数内
		 这部分比较复杂，因为源代码大部分都是用的pagelod的所以也没补充什么注释*/

		class LodModel : public osg::PagedLOD
		{
		public:
			//LodModel();
			//LodModel(const LodModel&,const CopyOp& copyop=CopyOp::SHALLOW_COPY);
			/*Lod模型是否被拾取*/
			bool _selectd;
			
			bool _visble;
			/*Lod模型包围盒半径*/
			double _modelRadius;
			//virtual void accept(osg::NodeVisitor& nv) 
			//{ 
			//	if (nv.validNodeMask(*this)) 
			//	{ 
			//		nv.pushOntoNodePath(this); nv.apply(*this); nv.popFromNodePath(); 
			//	} 
			//}
			/*Lod模型的属性信息*/
			//osg::ref_ptr<ModelOptions> _options;
			~LodModel();
			/*构造函数，设置各个模型的LOD可视范围*/
			LodModel(LodModelOptions* options);

			osg::ref_ptr<LodModelOptions> _options;

			std::string createModelName(std::string dbName,ULONGLONG featureKey,int lod);


            /*设置模型被拾取后的状态*/
			void select(bool select);

			void SetVisible(bool bVisble);

			void traverse(NodeVisitor& nv);


#if 0
			void travseModel(int i,bool updateTimeStamp,double timeStamp,unsigned int frameNumber,int &lastChildTraversed,NodeVisitor& nv);
#endif
		};
	}
}
#endif
