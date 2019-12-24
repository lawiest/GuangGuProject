#ifndef GEOGLOBE_PIPE_PIPELINEBUILDER
#define GEOGLOBE_PIPE_PIPELINEBUILDER 1

#include <osg/Referenced>
#include <Pipe/PipeOptions.h>
#include <osg/Geode>

namespace GeoGlobe
{
	namespace Pipe
	{

		const osg::Vec3d Axis_X = osg::Vec3d(1.0,0.0,0.0); 
		const osg::Vec3d Axis_Y = osg::Vec3d(0.0,1.0,0.0);
		const osg::Vec3d Axis_Z = osg::Vec3d(0.0,0.0,1.0);


		struct RectSize
		{
			double m_Height;
			double m_Width;
			void SetSize(double dbLen,double dbWidth)
			{
				m_Height = dbLen;
				m_Width = dbWidth;
			}
		};

		union PipeSize
		{
			RectSize rectSize;//方管
			double dbRadius;//圆管
		};

		class PipeLineBuilder:public osg::Referenced
		{
		public:
			PipeLineModelOptions* _options;
			osg::Vec3d            _centerCart;
			PipeSize              _size;
			double                _cutLen;
			osg::Vec4d            _color;
			PipeLineBuilder(PipeLineModelOptions* options)
			{
				_options=options;

				_centerCart=_options->_modelCart;

				switch(options->_sectionType)
				{
				case eCircle:
					{
						PipeSize size;
						//拿到的数据单位为mm
						size.dbRadius = (options->_diameter/2)*0.001;

		
						_size = size;
						_cutLen = size.dbRadius/2;
					}
					break;
				case eRect:
					{
						PipeSize size;
						size.rectSize.m_Height = options->_height*0.001;
						size.rectSize.m_Width = options->_width*0.001;
						_size = size;
						_cutLen =( (size.rectSize.m_Height>size.rectSize.m_Width)?size.rectSize.m_Height:size.rectSize.m_Width)/2;
					}
					break;
				}

			}

		    void calcuSectionVertices(std::vector<osg::Vec3d>& vSectionVertices,int nSlice,enumCrossSectionType eShape,PipeSize pipeSize)
			{

				switch(eShape)
				{
				case eRect:
					{
						double dbLen = pipeSize.rectSize.m_Height;
						double dbWidth = pipeSize.rectSize.m_Width;
						osg::Vec4d;
						//vSectionVertices.resize(4);
						vSectionVertices.push_back(osg::Vec3d(dbLen/2,0.0,dbWidth/2));
						vSectionVertices.push_back(osg::Vec3d(-dbLen/2,0.0,dbWidth/2));
						vSectionVertices.push_back(osg::Vec3d(-dbLen/2,0.0,-dbWidth/2));
						vSectionVertices.push_back(osg::Vec3d(dbLen/2,0.0,-dbWidth/2));
						return;
					}
				case eCircle:
					{
						double dRadius = pipeSize.dbRadius;
			/*			vSectionVertices.push_back(osg::Vec3d(500,0,0));
						vSectionVertices.push_back(osg::Vec3d(0,0,500));
						vSectionVertices.push_back(osg::Vec3d(-500,0,0));
						vSectionVertices.push_back(osg::Vec3d(0,0,-500));*/
						osg::Vec3d vertex;
						float angle = 0.0;
						for (int i=0;i<nSlice;i++ )
						{
							vertex.x() = cosf(angle) * dRadius;
							vertex.y() = sinf(angle) * dRadius;
							vertex.z() =0 ;
							vSectionVertices.push_back(vertex);
							angle += 2*osg::PI/nSlice;
						}
						return;
					}
				default:
					break;
				}
				//return NULL;
			}

			osg::Matrix calcuRotateMatrix(osg::Vec3d refenceVec /*参考向量*/,osg::Vec3d transVec /*变换向量*/)
			{
				osg::Vec3d vRotateAxis;//旋转轴

				refenceVec.normalize();
				//D3DXVec3Normalize(&refenceVec,&refenceVec);
				//D3DXVec3Cross(&vRotateAxis,&transVec,&refenceVec);
				vRotateAxis=refenceVec^transVec;


				float fDot = transVec*refenceVec;
				float fvCylinderLen = transVec.length();
				float fLineLen =  refenceVec.length();
				double dAngel =  acos(fDot/(fvCylinderLen*fLineLen));

				osg::Matrix rotMatrix;
				rotMatrix.setRotate(osg::Quat(-dAngel,vRotateAxis));
			//	osg::Quat()
				//D3DXMatrixRotationAxis(&rotMatrix,&vRotateAxis,dAngel);//旋转

				return rotMatrix;
			}

			osg::Matrix calcuRotateMatrix(const std::vector<osg::Vec3d> &vReferenceDir,const std::vector<osg::Vec3d> &vTransformDir)
			{
				osg::Matrix  resMatrix;

				//先计算一个待变换向量到参考向量的矩阵
				osg::Matrix rotateMatrix,rotateMatrix1;
				rotateMatrix = calcuRotateMatrix(vReferenceDir[0],vTransformDir[0]);

				//在上次变换基础上，重新计算另一个待变换向量
				osg::Vec3d vecTrans ;
				vecTrans=vTransformDir[1]*rotateMatrix;
				//D3DXVec3TransformNormal(&vecTrans,&vTransformDir[1],&rotateMatrix);

				//再计算另一个待变换向量到另一个参考向量的矩阵
				rotateMatrix1 = calcuRotateMatrix(vReferenceDir[1],vecTrans);

				//最终矩阵
				resMatrix=rotateMatrix*rotateMatrix1;
				//D3DXMatrixMultiply(&resMatrix,&rotateMatrix,&rotateMatrix1);
				return resMatrix;
			}

			osg::Matrix calcuTranformMatrix(enumCrossSectionType eShape,const osg::Vec3d &pipeLineDir, osg::Vec3d &centerPosDKE)
			{
				switch(eShape)
				{
				case eRect:
					{
						//方管要参考两个方向，进行变换
						osg::Vec3d outDir=centerPosDKE;
						outDir.normalize();
						//垂直地表的向量
						//D3DXVec3Normalize(&outDir,&D3DXVECTOR3(centerPosDKE.x,centerPosDKE.y,centerPosDKE.z));
						
						outDir=pipeLineDir^outDir;
						//D3DXVec3Cross(&outDir,&pipeLineDir,&outDir);
						vector<osg::Vec3d> vReferenceDir,vTransDir;
						vReferenceDir.resize(2);
						vReferenceDir[0] = pipeLineDir;
						vReferenceDir[1] = outDir;
						vTransDir.resize(2);
						vTransDir[0] = Axis_Y;
						vTransDir[1] = Axis_X;
						return calcuRotateMatrix(vReferenceDir,vTransDir);
					}
				case eCircle:
					{
						return calcuRotateMatrix(pipeLineDir,Axis_Z);
					}
				default:
					break;
				}

				osg::Matrix matix;
				//D3DXMatrixIdentity(&matix);
				return matix;
			}

			void calcuVertex(osg::Vec3dArray* vPipeVertices, int nStack,osg::Vec3d centerPosDKE,enumCrossSectionType eShape,
				const std::vector<osg::Vec3d>& vSectionVt,osg::Vec3d startCart,osg::Vec3d endCart)
			{
				
				double dbTransX,dbTransY,dbTransZ;
				osg::Matrix TranslateMatrix;
				osg::Matrix rotateMatrix;
				osg::Matrix resultMatrix;

				//计算管段的旋转矩阵
				osg::Vec3d pipeLineDir  = endCart - startCart;
				rotateMatrix = calcuTranformMatrix(eShape,pipeLineDir,centerPosDKE);
				pipeLineDir;
				osg::Vec3d vNormalizeLineDir=pipeLineDir;
				vNormalizeLineDir.normalize();
				//D3DXVec3Normalize(&vNormalizeLineDir,&D3DXVECTOR3(pipeLineDir.x,pipeLineDir.y,pipeLineDir.z));

				int nSection = 0;
				int j = 0;
				for (int k = 0;k<=nStack;k++)
				{
					float fModify = 0.0;
					//管段两头
					if (k == 0 )
						fModify = _cutLen;
					if (k == nStack)
						fModify = -_cutLen;

					//截面中心点位置
					osg::Vec3d centerSec;
					centerSec.x() = startCart.x() + pipeLineDir.x()/nStack*k + vNormalizeLineDir.x()*fModify;
					centerSec.y() = startCart.y() + pipeLineDir.y()/nStack*k + vNormalizeLineDir.y()*fModify;
					centerSec.z() = startCart.z() + pipeLineDir.z()/nStack*k + vNormalizeLineDir.z()*fModify;

					dbTransX = centerSec.x() - centerPosDKE.x()  ;
					dbTransY = centerSec.y() - centerPosDKE.y() ;
					dbTransZ = centerSec.z() - centerPosDKE.z();

					//计算截面世界矩阵
					//D3DXMatrixTranslation(&TranslateMatrix,dbTransX,dbTransY,dbTransZ);
					TranslateMatrix.setTrans(osg::Vec3d(dbTransX,dbTransY,dbTransZ));
					resultMatrix = rotateMatrix * TranslateMatrix;

					//截面上个每个顶点
					for (int i=0;i<vSectionVt.size();i++)
					{
						
						osg::Vec3d vec=vSectionVt[i];

						/*vec.x = vSectionVt->asArray()[i].x
						vec.y = vSectionVt[i].y;
						vec.z = vSectionVt[i].z;*/
						vPipeVertices->push_back(vec*resultMatrix);
						//D3DXVec3TransformCoord(&vPipeVertices[j],&vec,&resultMatrix);
						//j++;
					}
				}
			}


			osg::Drawable* buildRectPipeLineModel(int nSliceNum)
			{


				return NULL;
			}

			void calcuCirclePipeLineIndex(osg::DrawElementsUInt* vDrawIndex,int nStack,int nSlice)
			{
				//构建索引
				for (int k=0;k<nStack;k++)
				{
					//一个圆柱的索引
					int nBeginIndex = k*nSlice;
					for (int i=0;i<nSlice-1;i++)
					{
						vDrawIndex->push_back(nBeginIndex+i);
						vDrawIndex->push_back(nBeginIndex+i+nSlice);
						vDrawIndex->push_back(nBeginIndex+i+1);

						vDrawIndex->push_back(nBeginIndex+i+1);
						vDrawIndex->push_back(nBeginIndex+i+nSlice);
						vDrawIndex->push_back(nBeginIndex+i+nSlice+1);
					}
					vDrawIndex->push_back(nBeginIndex+nSlice-1);
					vDrawIndex->push_back(nBeginIndex+2*nSlice-1);
					vDrawIndex->push_back(nBeginIndex);
					vDrawIndex->push_back(nBeginIndex);
					vDrawIndex->push_back(nBeginIndex+2*nSlice-1);
					vDrawIndex->push_back(nBeginIndex+nSlice);
				}
			}

			void computerCirclePipeLineNormal(osg::Vec3dArray* vPipeLineVertices,osg::Vec3dArray* vVerticeNormals,int nStack,int nSlices)
			{


				vector<osg::Vec3d> vFaceNormal;
				//计算圆柱侧面法线
				osg::Vec3d pNormal,pV1,pV2;
				for (int i=0;i<nSlices-1;i++)
				{
					pV1 = vPipeLineVertices->asVector()[i] - vPipeLineVertices->asVector()[i+1];
					pV2 = vPipeLineVertices->asVector()[i+nSlices] - vPipeLineVertices->asVector()[i];
					pNormal=pV1^pV2;
					
					vFaceNormal.push_back(pNormal);
				}

				//最后一个侧面
				int j = nSlices;
				pV1 = vPipeLineVertices->asVector()[0] - vPipeLineVertices->asVector()[nSlices-1];
				pV2 = vPipeLineVertices->asVector()[nSlices-1]- vPipeLineVertices->asVector()[2*nSlices-1];
				pNormal=pV1^pV2;
				//D3DXVec3Cross(&pNormal,&pV1,&pV2);
				vFaceNormal.push_back(pNormal);

				//计算点法线
				osg::Vec3d pointNormal,pointNormalize;
				for (int i=0;i<=nStack;i++)
				{
					int nStartIndex = i*nSlices;
					pointNormal = (vFaceNormal[0]+vFaceNormal[vFaceNormal.size()-1])/2;
					//D3DXVec3Normalize(&pointNormalize,&pointNormal);
					pointNormal.normalize();
					vVerticeNormals->push_back(pointNormal);

					//计算每个截面上点法线
					int nNoramlIndex = 0;
					for (int j=nStartIndex+1;j<nStartIndex+nSlices;j++)
					{
						pointNormal = (vFaceNormal[nNoramlIndex]+vFaceNormal[nNoramlIndex+1])/2;
						pointNormal.normalize();
						vVerticeNormals->push_back(pointNormal);
						nNoramlIndex++;
					}
				}

			}

			osg::Drawable* buildCirclePipeLineModel(int nSliceNum)
			{
				int nStack=1;
				std::vector<osg::Vec3d> vSectionVertices;
				calcuSectionVertices(vSectionVertices,nSliceNum,eCircle,_size);

				osg::Vec3dArray* vTempPipeVertices=new osg::Vec3dArray;//不带法线的顶点
				//vTempPipeVertices->resize(vSectionVertices.size()*(nStack+1));
				calcuVertex(vTempPipeVertices,nStack,_centerCart,eCircle,vSectionVertices,_options->_startCart,_options->_endCart);


				osg::ref_ptr<osg::Vec3dArray> normals = new osg::Vec3dArray ;
				computerCirclePipeLineNormal(vTempPipeVertices,normals,nStack,nSliceNum);



				osg::DrawElementsUInt* triangle=new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES,0);
				calcuCirclePipeLineIndex(triangle,nStack,nSliceNum);

				osg::ref_ptr<osg::Geometry> pipeLine=new osg::Geometry;

				if(vTempPipeVertices->size())
					pipeLine->setVertexArray(vTempPipeVertices);
	/*			if( normals->size())
					pipeLine->setVertexArray( normals);*/

				if(normals->size())
					pipeLine->setNormalArray(normals, osg::Array::BIND_PER_VERTEX);


				//pipeLine->setColorBinding()
				osg::Vec4dArray* colors=new osg::Vec4dArray;//不带法线的顶点
				colors->push_back(_color);
				pipeLine->setColorArray(colors,osg::Array::BIND_OVERALL);
			



                pipeLine->addPrimitiveSet(triangle);

				return pipeLine.release();
			}

			osg::Node* buildLodPipeLineModel(int lod)
			{
				//Reset();
				//vector<_3DSurfacePair> ListTriangleMeshPair;
				osg::ref_ptr<osg::Geode> geode=new  osg::Geode;
			
			
				//osg::ref_ptr<osg::MatrixTransform> geode=new  osg::Geode;

				switch(_options->_sectionType)
				{
				case eRect:
					{
						geode->addDrawable(buildRectPipeLineModel(4));
						//C3DTriangleMesh* pTriangleMesh0 = ToTriangleMesh(&m_vecLodVertices[0][0],m_vecLodVertices[0].size(),0/*无效参数*/,
						//	VERTEXTYPE_V3fN3f,&m_vecLodIndiecs[0][0],m_vecLodIndiecs[0].size(),m_nMatID);
						//ListTriangleMeshPair.push_back(_3DSurfacePair(0,pTriangleMesh0));
					}
					break;
				case eCircle:
					{
						//圆管需要构建几何lod  
						//lod =1 
						
						geode->addDrawable(buildCirclePipeLineModel(4*lod));
						//C3DTriangleMesh* pTriangleMesh0 = ToTriangleMesh(&m_vecLodVertices[0][0],m_vecLodVertices[0].size(),0/*无效参数*/,
						//	VERTEXTYPE_V3fN3f,&m_vecLodIndiecs[0][0],m_vecLodIndiecs[0].size(),m_nMatID);
						//ListTriangleMeshPair.push_back(_3DSurfacePair(1,pTriangleMesh0));

						////lod =2 
						//BuildPipeLineModel(8,m_vecLodVertices[1],m_vecLodIndiecs[1]);
						//C3DTriangleMesh* pTriangleMesh1 = ToTriangleMesh(&m_vecLodVertices[1][0],m_vecLodVertices[1].size(),0/*无效参数*/,
						//	VERTEXTYPE_V3fN3f,&m_vecLodIndiecs[1][0],m_vecLodIndiecs[1].size(),m_nMatID);
						//ListTriangleMeshPair.push_back(_3DSurfacePair(2,pTriangleMesh1));

						////lod =3
						//BuildPipeLineModel(20,m_vecLodVertices[2],m_vecLodIndiecs[2]);
						//C3DTriangleMesh* pTriangleMesh2 = ToTriangleMesh(&m_vecLodVertices[2][0],m_vecLodVertices[2].size(),0/*无效参数*/,
						//	VERTEXTYPE_V3fN3f,&m_vecLodIndiecs[2][0],m_vecLodIndiecs[2].size(),m_nMatID);
						//ListTriangleMeshPair.push_back(_3DSurfacePair(3,pTriangleMesh2));
					}
					break;
				}

				return geode.release();
				//m_solidPipeModel->set3DSurfaceList(ListTriangleMeshPair);
				//m_solidPipeModel->writeBuffer(LodModelBuffer);
			}

			osg::Node* buildPipeLineModel()
			{


				int size;
				char* data;
				_options->_dataBase->_symDB->QuerySingleKeyValueTable("ModelSymbol",_options->_modelGeometryID,size,&data);

				CMaterial* pMaterial = new CMaterial();
	

				Buffer tempBuf(data+5, size);
				if (!pMaterial->readBuffer(tempBuf))
				{
					SafeDelete(pMaterial);
					return NULL;
				}
				
				_color=toColor(pMaterial->getSimpleMat().ulColor);
				SafeDelete(pMaterial);

				osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform();
				mt->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
				//mt->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
				mt->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
				mt->setMatrix(/*osg::Matrixd::scale(10000000,10000000,10000000)**/osg::Matrixd::translate(_centerCart));
				osg::LOD* lodPipeLine=new osg::LOD;
				lodPipeLine->addChild(buildLodPipeLineModel(1),200,FLT_MAX);
				lodPipeLine->addChild(buildLodPipeLineModel(2),40,200);
				lodPipeLine->addChild(buildLodPipeLineModel(3),0,40);
				mt->addChild(lodPipeLine);
				//QuerySingleKeyValueTable("ModelGeometry",geoKey,dataLen,geoData)
			

				
				
				return mt.release();
			}

			osg::Vec4d  toColor(long lColor)
			{
				long alpha = (lColor >> 24) & 0xff;
				long Red   = (lColor >> 16) & 0xff;
				long Green = (lColor >> 8)  & 0xff;
				long Blue  =  lColor		& 0xff;

				return  osg::Vec4d(Red/255.0,Green/255.0,Blue/255.0,alpha/255.0);
			}

		};
	}
}

#endif