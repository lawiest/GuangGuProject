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

#ifndef OSGEARTHUTIL_GEOWFSFEATURE_QUERY_TOOL_H
#define OSGEARTHUTIL_GEOWFSFEATURE_QUERY_TOOL_H 1

#include <osgEarthUtil/Common>
#include <osgEarth/MapNode>
#include <osgEarth/MapNodeObserver>
#include <osgEarth/GeoData>
#include <osgEarthFeatures/Feature>
#include <osgEarthFeatures/FeatureSource>
#include <osgEarthFeatures/FeatureSourceIndexNode>
#include <osgEarthUtil/Controls>
#include <osgGA/GUIEventHandler>
#include <osg/View>

#include <osgEarth/Config>
#include "GeoFeature_XMLApi.h"
#include "GEOWFSFeatureOptions.h"

namespace GeoGlobe { namespace GeoVector
{
    
    using namespace osgEarth::Features;

    /**
     * Tool that let you query the map for Features generated from a FeatureSource.
     *
     * By default, an unmodified left-click will activate a query. You can replace
     * this test by calling setInputPredicate().
     */
    class GEOWFSFeatureQueryTool : public osgGA::GUIEventHandler
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
        GEOWFSFeatureQueryTool(
			const GeoGlobe::GeoVector::GEOWFSFeatureOptions& options,
            MapNode*  mapNode,
            Callback* callbackToAdd =0L );

        /** dtor */
        virtual ~GEOWFSFeatureQueryTool() 
		{
			
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
		bool getBBoxXML(double xmin,double ymin,double xmax,double ymax);
		std::string createBBoxURL(double xmin,double ymin,double xmax,double ymax);
		void saveResponse(const std::string buffer, const std::string& filename);
		//std::string getAllCachePath(std::string rootPath);
		//void QueryInXML(double xmin,double ymin,double xmax,double ymax);
		void createAttributeTable(osgEarth::Features::AttributeTable& attr);
    protected:
        osg::observer_ptr<MapNode> _mapNode;
        bool                       _mouseDown;
        float                      _mouseDownX, _mouseDownY;

		//std::string _xmlAllCachePath;
		std::string _xmlBBoxPath;
		std::string _xmlBBoxPathRoot;
		std::string _wfsUrl;
		std::string _typeName;
		std::vector<AttributeTable> _attributeTables;
        typedef std::vector< osg::observer_ptr<Callback> > Callbacks;
        Callbacks _callbacks;
    };

    //--------------------------------------------------------------------
	
    /**
     * Sample callback that will display feature attributes upon a query hit.
     */
    class GEOWFSFeatureReadoutCallback : public GEOWFSFeatureQueryTool::Callback
    {
    public:
        GEOWFSFeatureReadoutCallback( osgEarth::Util::Controls::Container* container );

    public:
        virtual void onHit(std::vector<AttributeTable> tables,const EventArgs& args);

        virtual void onMiss( const EventArgs& args );

    protected:
        void clear();
        osgEarth::Util::Controls::Grid* _grid;

        /** dtor */
        virtual ~GEOWFSFeatureReadoutCallback() { }
    };


} } // namespace geoglobe vector

#endif // OSGEARTHUTIL_GEOWFSFEATURE_QUERY_TOOL_H
