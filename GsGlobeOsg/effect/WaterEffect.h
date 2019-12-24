
#ifndef __WATER_EFFECT__H__
#define __WATER_EFFECT__H__

#include <osg/LOD>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/TextureCubeMap>
#include <osg/Texture2D>
#include <osg/MatrixTransform>
#include <osg/Matrix>
#include <osg/Plane>

namespace GeoGlobe
{
	namespace Effect
	{
		class WaterEffect;

		class WaterTransform : public osg::MatrixTransform
		{
		public:
			WaterTransform(WaterEffect* pWater);
			~WaterTransform();
			virtual void traverse(osg::NodeVisitor& nv);

		private:
			WaterEffect* m_pWater;
		};

		class WaterEffect:public osg::Referenced
		{
		public:
			WaterEffect();
			~WaterEffect();

		public:
			    void SetPosition(const osg::Matrix& matrix);
				void SetWaterSurfaceObject(osg::ref_ptr<osg::Geometry> surface);
				bool PreRender(osg::Group* renderObjects);
				void Update();
				void AttachCamera(osg::Camera* pCamera);
				void SetLigthPosition(const osg::Vec3f& lightPos);
				void SetVisible(bool visible);

		public:
				osg::MatrixTransform* GetMatrixTransform(){return m_transform.get();}
				osg::Camera* GetRTTCamera(){return m_reflectionCamera.get();}
				osg::Camera* GetRefractionCamera(){return m_reflectionCamera.get();}
				static osg::ref_ptr<osg::Geometry> CreateSimpleSurfacePlane( float length, float width);
				osg::Texture2D* CreateTexture( const char* filename);

		protected:
				void LoadShader();
				void SetRTTCamera(int width, int height);
				void SetRefractionCamera(int width, int height);
				void Initialize();

		private:
			osg::Camera* m_pCamera;
			osg::ref_ptr<osg::Geode> m_surfaceObject;
			osg::ref_ptr<osg::Shader> m_vertexShader;
			osg::ref_ptr<osg::Shader> m_fragmentShader;
			osg::ref_ptr<osg::Program>m_program;

			osg::ref_ptr<osg::Texture2D> m_refractionTexture;
			osg::ref_ptr<osg::Texture2D> m_reflectionTexture;
			osg::ref_ptr<osg::Camera> m_reflectionCamera;
			osg::ref_ptr<osg::Camera> m_refractionCamera;
			osg::Vec4d								m_plane;
			osg::Vec3d								m_orgiVertices[3];
			osg::Vec3f								m_lightPosition;
			bool											m_visible;
			 osg::ref_ptr<WaterTransform> m_transform;
			 osg::Matrix								m_worldMatrix;
		};
	}
}
#endif