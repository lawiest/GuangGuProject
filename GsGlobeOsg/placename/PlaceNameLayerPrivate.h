#ifndef GEOGLOBE_PLACENAME_PLACENAMELAYER
#define GEOGLOBE_PLACENAME_PLACENAMELAYER 1

#include <osg/LOD>
#include <PlaceName/PlaceNameTileOptions.h>

GLOBE_NS

class PlaceNameLayerPrivate : public osg::LOD
{
public:
	PlaceNameLayerPrivate(PlaceNameLayerOptions *pnLayerOptions);
	void traverse(osg::NodeVisitor& nv);

	void Visible(bool bVisible);
	bool Visible() const;

protected:
	osg::ref_ptr<PlaceNameLayerOptions> m_ptrOptions;
	bool                                m_bVisible;
};

GLOBE_ENDNS

#endif
