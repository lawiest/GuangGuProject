#ifndef SHADOWING_TEST_H
#define SHADOWING_TEST_H

#include <osgEarthUtil/Common>
#include <osg/Camera>
#include <osg/Texture2DArray>
#include <osg/Matrix>
#include <osg/Uniform>
#include <osg/Light>

    /**
     * Group that casts shadows on its subgraph.
     *
     * NOTE!! This object is not multi-camera aware yet.
     */
    class ShadowCaster1 : public osg::Group
    {
    public:
        ShadowCaster1();

        /** Whether shadows are supported (requires GLSL) */
        bool supported() const { return _supported; }

        /**
         * The light that will cast shadows.
         * NOTE: Only works for point lights, like the sun.
         */
        void setLight(osg::Light* light) { _light = light; }
        osg::Light* getLight() { return _light.get(); }

        /**
         * Group of geometry that should cast shadows on this node's children.
         * You must add geometry here in order to cast shadows. The geometry
         * you add here will only be used to generate shadows - it must still
         * exist elsewhere in the scene graph for rendering.
         */
        osg::Group* getShadowCastingGroup() { return _castingGroup.get(); }

        /**
         * Slice ranges. Each slice (the space beteen each value in the list)
         * represents a single shadow map in the Cascading Shadow Maps 
         * implementation.
         */
        const std::vector<float>& getRanges() { return _ranges; }
        void setRanges(const std::vector<float>& ranges);

        /**
         * The GPU texture image unit that will store the shadow map while
         * rendering the subgraph.
         */
        int getTextureImageUnit() const { return _texImageUnit; }
        void setTextureImageUnit(int unit);

        /**
         * The size (in both dimensions) of the shadow depth texture. Bigger
         * is sharper. Default is 1024. The total texture size used will be
         * size * # of range slices * 3 bytes.
         */
        unsigned getTextureSize() const { return _size; }
        void setTextureSize(unsigned size);

        /**
         * The ambient color of the shadow. This is blended with the fragment
         * color to achieve shadowing. Default is 0x7f7f7fff
         */
        void setShadowColor(const osg::Vec4f& value);
        const osg::Vec4f& getShadowColor() const { return _color; }

        /**
         * The blurring factor to apply to shadow PCF sampling. Using a blurring
         * factor will incur a GPU performance penalty. Default is 0.0.
         * Decent values are [0.0 -> 0.001].
         */
        void setBlurFactor(float value);
        float getBlurFactor() const { return _blurFactor; }


    public: // osg::Node

        virtual void traverse(osg::NodeVisitor& nv);

    protected:
        virtual ~ShadowCaster1() { }

        void reinitialize();

        bool                                    _supported;
        osg::ref_ptr<osg::Group>                _castingGroup;
        unsigned                                _size;
        float                                   _blurFactor;
        osg::Vec4f                              _color;
        osg::ref_ptr<osg::Light>                _light;
        osg::ref_ptr<osg::Texture2DArray>       _shadowmap;
        osg::ref_ptr<osg::StateSet>             _rttStateSet;
        std::vector<float>                      _ranges;
        std::vector<osg::ref_ptr<osg::Camera> > _rttCameras;
        osg::Matrix                             _prevProjMatrix;

        int                         _texImageUnit;
        osg::ref_ptr<osg::StateSet> _renderStateSet;
        osg::ref_ptr<osg::Uniform>  _shadowMapTexGenUniform;
        osg::ref_ptr<osg::Uniform>  _shadowBlurUniform;
        osg::ref_ptr<osg::Uniform>  _shadowColorUniform;
    };

 // namespace osgEarth::Util

#endif // OSGEARTH_UTIL_SHADOWING_H