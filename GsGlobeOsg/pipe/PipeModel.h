#ifndef GEOGLOBE_PIPE_PIPEMODEL
#define GEOGLOBE_PIPE_PIPEMODEL 1

#include<osg/PagedLOD>

using namespace osg;
namespace GeoGlobe
{
	namespace Pipe
	{

/*管线瓦片与管线几何数据的中间层，通过它实现管线几何数据的实时动态加载与卸载*/
		class PipeModel:public osg::PagedLOD
		{
		public:
			PipeModel(){}

			void traverse(NodeVisitor& nv)
			{



				if (nv.getFrameStamp() &&
					nv.getVisitorType()==osg::NodeVisitor::CULL_VISITOR)
				{
					setFrameNumberOfLastTraversal(nv.getFrameStamp()->getFrameNumber());
				}

//直接抄的osg::PagedLOD中traverse代码
				double timeStamp = nv.getFrameStamp()?nv.getFrameStamp()->getReferenceTime():0.0;
				unsigned int frameNumber = nv.getFrameStamp()?nv.getFrameStamp()->getFrameNumber():0;
				bool updateTimeStamp = nv.getVisitorType()==osg::NodeVisitor::CULL_VISITOR;

				switch(nv.getTraversalMode())
				{
				case(NodeVisitor::TRAVERSE_ALL_CHILDREN):
					std::for_each(_children.begin(),_children.end(),NodeAcceptOp(nv));
					break;
				case(NodeVisitor::TRAVERSE_ACTIVE_CHILDREN):
					{
						float required_range = 0;
						if (_rangeMode==DISTANCE_FROM_EYE_POINT)
						{
							required_range = nv.getDistanceToViewPoint(getCenter(),true);
						}
						else
						{
							osg::CullStack* cullStack = dynamic_cast<osg::CullStack*>(&nv);
							if (cullStack && cullStack->getLODScale()>0.0f)
							{
								required_range = cullStack->clampedPixelSize(getBound()) / cullStack->getLODScale();
							}
							else
							{
								// fallback to selecting the highest res tile by
								// finding out the max range
								for(unsigned int i=0;i<_rangeList.size();++i)
								{
									required_range = osg::maximum(required_range,_rangeList[i].first);
								}
							}
						}

						int lastChildTraversed = -1;
						bool needToLoadChild = false;
						for(unsigned int i=0;i<_rangeList.size();++i)
						{
							if (_rangeList[i].first<=required_range && required_range<_rangeList[i].second)
							{
								if (i<_children.size())
								{
									if (updateTimeStamp)
									{
										_perRangeDataList[i]._timeStamp=timeStamp;
										_perRangeDataList[i]._frameNumber=frameNumber;
									}

									_children[i]->accept(nv);
									lastChildTraversed = (int)i;
								}
								else
								{
									needToLoadChild = true;
								}
							}
						}

						if (needToLoadChild)
						{
							unsigned int numChildren = _children.size();

							// select the last valid child.
							if (numChildren>0 && ((int)numChildren-1)!=lastChildTraversed)
							{
								if (updateTimeStamp)
								{
									_perRangeDataList[numChildren-1]._timeStamp=timeStamp;
									_perRangeDataList[numChildren-1]._frameNumber=frameNumber;
								}
								_children[numChildren-1]->accept(nv);
							}

							// now request the loading of the next unloaded child.
							if (!_disableExternalChildrenPaging &&
								nv.getDatabaseRequestHandler() &&
								numChildren<_perRangeDataList.size())
							{
								// compute priority from where abouts in the required range the distance falls.
								float priority = (_rangeList[numChildren].second-required_range)/(_rangeList[numChildren].second-_rangeList[numChildren].first);

								// invert priority for PIXEL_SIZE_ON_SCREEN mode
								if(_rangeMode==PIXEL_SIZE_ON_SCREEN)
								{
									priority = -priority;
								}

								// modify the priority according to the child's priority offset and scale.
								priority = _perRangeDataList[numChildren]._priorityOffset + priority * _perRangeDataList[numChildren]._priorityScale;

								if (_databasePath.empty())
								{
									nv.getDatabaseRequestHandler()->requestNodeFile(_perRangeDataList[numChildren]._filename,nv.getNodePath(),priority,nv.getFrameStamp(), _perRangeDataList[numChildren]._databaseRequest, _databaseOptions.get());
								}
								else
								{
									// prepend the databasePath to the child's filename.
									nv.getDatabaseRequestHandler()->requestNodeFile(_databasePath+_perRangeDataList[numChildren]._filename,nv.getNodePath(),priority,nv.getFrameStamp(), _perRangeDataList[numChildren]._databaseRequest, _databaseOptions.get());
								}
							}

						}


						break;
					}
				default:
					break;
				}
			}

		};
	}
}





#endif