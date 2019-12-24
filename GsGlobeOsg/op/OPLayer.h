#ifndef GEOGLOBE_OP_OPLayer
#define GEOGLOBE_OP_OPLayer 1


#include "Model/CT3D_3DOblique.h"
#include "TilePager.h"
#include "Tool/SQLiteOP.h"
#include <osg/Texture2D>

//typedef long long __int64;

namespace GeoGlobe
{
	namespace OP
	{
		/*倾斜影像的属性,参照GeoGlobe*/
		typedef struct OPModelInfo
		{
			__int64 nModelID,nOsgbTileID;
			int nLevel,nRow,nCol;

		}OPModelInfo;

		/*倾斜影像数据集*/
		class OPLayer : public TilePager
		{
		public:
			/*是否初始化*/
			bool _bInitialized;
			/*吉奥瓦片数据库*/
			osg::ref_ptr<CSQLiteOP> m_tileDB;
			/*吉奥几何数据库*/
			osg::ref_ptr<CSQLiteOP> m_geoDB;

			/*读取吉奥格式的数据集*/
			OPLayer(std::string gomdxFileName,const osgEarth::Profile* profile);

			/*读取osgb格式的数据集*/
				OPLayer(std::string osgbFileName,double lon,double lat,double elevation,const osgEarth::Profile* profile);

			/*数据库初始化*/
				bool init(std::string gomdxFileName);

			/*读取数据集范围*/
				bool readTileMeta(std::string tileDBName);

			/*继承TilePager 根据key值生成瓦片*/
			virtual osg::Node* createNode( const osgEarth::TileKey& key );

			/*构建几何数据*/
			osg::ref_ptr<osg::Geometry>  createOPGeometry(C3DOblique* c3dOblique);

			/*构建纹理数据*/
			osg::ref_ptr<osg::Texture2D> createOPTexture(C3DOblique* c3dOblique) ;

			/*构建瓦片数据*/
			osg::ref_ptr<osg::Group> createOPTile(int level,int col,int row);
		};

	}
}

#endif
