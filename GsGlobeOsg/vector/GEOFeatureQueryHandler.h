/* -*-c++-*- */
/* osgEarth - Dynamic map generation toolkit for OpenSceneGraph
 * Copyright 2008-2014 Pelican Mapping
 * http://osgearth.org
 *
 * osgEarth is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef OSGEARTHUTIL_GEOFEATURE_QUERY_TOOL_H
#define OSGEARTHUTIL_GEOFEATURE_QUERY_TOOL_H 1

#include <osgEarthUtil/Common>
#include <osgEarth/MapNode>
#include <osgEarth/MapNodeObserver>
#include <osgEarth/GeoData>
#include <osgEarth/SpatialReference>
#include <osgEarthFeatures/Feature>
#include <osgEarthFeatures/FeatureSource>
#include <osgEarthFeatures/FeatureSourceIndexNode>
#include <osgEarthUtil/Controls>
#include <osgGA/GUIEventHandler>
#include <osg/View>

#include "GeoFeature_SqliteApi.h"
#include "LineIntersectTest.h"
#include "GEOVectorAttributeQuery.h"
namespace GeoGlobe { namespace GeoVector
{
    
    using namespace osgEarth::Features;

    /**
     * Tool that let you query the map for Features generated from a FeatureSource.
     *
     * By default, an unmodified left-click will activate a query. You can replace
     * this test by calling setInputPredicate().
     */
    class GEOFeatureQueryTool : public osgGA::GUIEventHandler
    {
    public:

        struct Callback : public osg::Referenced
        {
            struct EventArgs 
            {
                const osgGA::GUIEventAdapter*  _ea;
                osgGA::GUIActionAdapter*       _aa;
                osg::Vec3d                     _worldPoint;
            };

            // called when a valid feature is found under the mouse coords
            virtual void onHit( FeatureSourceIndexNode* index, FeatureID fid, const EventArgs& args ) { }
			virtual void onHit(std::vector<AttributeTable> tables,const EventArgs& args ) {}
            // called when no feature is found under the mouse coords
            virtual void onMiss( const EventArgs& args ) { }
        };

    public:
        /**
         * Constructs a new feature query tool.
         *
         * @param mapNode
         *      Map node containing feature data to query
         * @param callbackToAdd
         *      (optional) convenience; calls addCallback with this parameter
         */
        GEOFeatureQueryTool( 
			std::string xmlPath,
			osg::Group* root,
            MapNode*  mapNode,
            Callback* callbackToAdd =0L ) ;

        /** dtor */
        virtual ~GEOFeatureQueryTool() 
		{
			if (_geoFeaClasses)
			{
				delete[] _geoFeaClasses;
			}
		}

        /**
         * Adds a feature query callback.
         */
        void addCallback( Callback* callback );


    public: // GUIEventHandler

        virtual bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );

    public: // MapNodeObserver

        virtual void setMapNode( MapNode* mapNode );

        virtual MapNode* getMapNode() { return _mapNode.get(); }

	public: //xmlPath
		bool init(std::string xmlPath);

		void ptWintoWorld(osgViewer::View* view,osg::Vec2f& mousePoints,osg::Vec3d& worldPoints);
		void ptWorldtoLLH(osgEarth::SpatialReference*srs,osg::Vec3d& worldPoints,osg::Vec3d& llhPoints);
		void getQueryPtToPoly(osgViewer::View* view,osg::Vec2f& mousePoints,osg::Vec3dArray* worldPoints,osg::Vec3dArray*llhPoints);
		void getBoundBox(osg::Vec3dArray* llhPoints,osgEarth::Bounds& bds);
	
		void QueryInSqlite(osg::Vec3dArray* llhPoints,std::vector<AttributeTable>& attrTables);
		void createAttributeTable(Config fields,GEOFeatureClass* gfc,int oid,osgEarth::Features::AttributeTable& attr);
		void highlightVector(std::vector<int>& oids,GEOFeatureClass* geoFeaClass,osg::Group* root,MapNode* mapNode);
		void getIntersectVectors(osg::Vec3dArray* llhPoints,std::vector<int>& oids,GEOFeatureClass* geoFeaClass);
		
    protected:
        osg::observer_ptr<MapNode> _mapNode;
        bool                       _mouseDown;
        float                      _mouseDownX, _mouseDownY;

		osg::ref_ptr<osg::Group> _root;
		osg::ref_ptr<osg::Vec3dArray> _worldPoints;
		osg::ref_ptr<osg::Vec3dArray> _llhPoints;
		float _dis;

		GEOFeatureClass* _geoFeaClasses;
		unsigned _dataCount;
		std::vector<AttributeTable> _attributeTables;
        typedef std::vector< osg::observer_ptr<Callback> > Callbacks;
        Callbacks _callbacks;
    };

    //--------------------------------------------------------------------
	
    /**
     * Sample callback that will display feature attributes upon a query hit.
     */
    class GEOFeatureReadoutCallback : public GEOFeatureQueryTool::Callback
    {
    public:
        GEOFeatureReadoutCallback( osgEarth::Util::Controls::Container* container );

    public:
        virtual void onHit(std::vector<AttributeTable> tables,const EventArgs& args);

        virtual void onMiss( const EventArgs& args );

    protected:
        void clear();
        osgEarth::Util::Controls::Grid* _grid;

        /** dtor */
        virtual ~GEOFeatureReadoutCallback() { }
    };


} } // namespace geoglobe vector

#endif // OSGEARTHUTIL_GEOFEATURE_QUERY_TOOL_H
