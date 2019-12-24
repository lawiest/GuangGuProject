#ifndef _OBLIQUE_IMAGE_
#define _OBLIQUE_IMAGE_

#include<osgViewer/Viewer>

#define MAXLEN 10
struct Stacktab
{
	//Data data[MAXLEN];
	//std::string tab[MAXLEN];
	size_t tabpos[MAXLEN];
	int top;
};

//封装了从文件名加载倾斜OSGB数据的接口
namespace GeoGlobe
{
	class CObliqueImageLoader{
	public:

		static double dbHeight;

		static osg::Group* m_ptrGroup;

		static double m_dbStepX;
		static double m_dbStepY;

		static bool m_isRotate;

		CObliqueImageLoader();
		~CObliqueImageLoader();
	public:
		//从文件名加载OSGB数据
		static void SetLod(double dbMinRange, double dbLod);
		static osg::Group* LoadFromServer(std::string strUrl, double dbMinLod = 0.0, double dbMaxLod = 300.0);
		static osg::Group* LoadFromServerEx(std::string strUrl);
		static osg::Group* LoadFromFile(std::string path, double dbMinLod = 0.0, double dbMaxLod = 300.0);
		static osg::Group* LoadFromServerNormal(std::string strUrl);
		static void SetStep(double dbStep);
		static osg::Vec3d Position();
		static std::string ProjString();

	protected:
		void getFolders(std::string path, std::vector<std::string>& files);
		void getFiles( std::string path, std::vector<std::string>& files ) ;
		static osg::Vec3d m_vecPos;
		static std::string m_strSrs;
	};
}

#endif