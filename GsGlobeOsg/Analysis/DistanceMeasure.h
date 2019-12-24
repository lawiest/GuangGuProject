/* -*-c++-*- */
/* osgEarth - Dynamic map generation toolkit for OpenSceneGraph
 * Copyright 2016 Pelican Mapping
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

#ifndef GEOGLOBE__DISTANCEMEASURE_H
#define GEOGLOBE__DISTANCEMEASURE_H 1

#include <osgEarthUtil/Common>
#include <osgEarth/MapNode>
#include <osgEarth/MapNodeObserver>
#include <osgEarthAnnotation/FeatureNode>
#include <osgEarthSymbology/Style>
#include <osg/Group>
#include <osgGA/GUIEventHandler>
#include <osgViewer/View>
#include <placename/PlaceNode.h>
#include <AnalysisCallBack.h>

namespace GeoGlobe 
{ 
	
    class DistanceMeasureHandler : public osgEarth::MapNodeObserver, public osgGA::GUIEventHandler
    {
    public:

		struct DistanceMeasureEventHandler : public osg::Referenced
        {
        public:
            virtual void onDistanceChanged(DistanceMeasureHandler* sender, double distance) {}
            virtual ~DistanceMeasureEventHandler() { }
        };

	

        typedef std::list< osg::ref_ptr< DistanceMeasureEventHandler > > MeasureToolEventHandlerList;

		osg::Group* DisplayGroup();
        DistanceMeasureHandler( osgEarth::MapNode* mapNode );
        virtual ~DistanceMeasureHandler();

        bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );        

        bool getLocationAt(osgViewer::View* view, double x, double y, double &lon, double &lat);
        
        void clear();

        void addEventHandler(DistanceMeasureEventHandler* handler );

        void setMouseButton(int mouseButton);
        int getMouseButton() const;

		osgEarth::GeoInterpolation getGeoInterpolation() const;
        void setGeoInterpolation(osgEarth::GeoInterpolation geoInterpolation );


		void addDistanceMeasureEventHandler(DistanceMeasureEventHandler* callback);
		void removeDistanceMeasureEventHandler(DistanceMeasureEventHandler* callback);


        void setIsPath( bool bPath );
        bool getIsPath() const;

        /** Sets the style of the measurment line (to something other than the default) */
        void setLineStyle( const osgEarth::Symbology::Style& style );
        const osgEarth::Symbology::Style& getLineStyle() const { return _feature->style().value(); }

        void setIntersectionMask( osg::Node::NodeMask intersectionMask ) { _intersectionMask = intersectionMask; }
        osg::Node::NodeMask getIntersectionMask() const { return _intersectionMask;}
        
        osgEarth::Features::Feature* getFeature() const { return _feature.get(); }

		void AddAnalysisCallBack(GLOBE_NAME::AnalysisCallBack* ptrCallBack);

		UTILITY_NAME::GsDelegate<void(double)> OnFinish;

    public: // MapNodeObserver

        virtual void setMapNode(osgEarth::MapNode* mapNode );
        virtual osgEarth::MapNode* getMapNode() { return _mapNode.get(); }

    protected:
		osgEarth::GeoInterpolation _geoInterpolation;
        void fireDistanceChanged();
        bool _lastPointTemporary;
        bool _gotFirstLocation;
        bool _Finished;
        bool _mouseDown;
        float _mouseDownX, _mouseDownY;
        int _mouseButton;

		double distance;
        osg::ref_ptr< osg::Group > m_ptrGroup;

        osg::ref_ptr< osgEarth::Annotation::FeatureNode > _featureNode;
        osg::ref_ptr< osgEarth::Features::Feature >  _feature;

        osg::ref_ptr< osgEarth::Annotation::FeatureNode > _extentFeatureNode;
        osg::ref_ptr< osgEarth::Features::Feature >       _extentFeature;

        MeasureToolEventHandlerList _eventHandlers;
        bool _isPath;        
        osg::observer_ptr< osgEarth::MapNode > _mapNode;
        osg::Node::NodeMask _intersectionMask;

		
        void rebuild();
	private:
		osgEarth::GeoPoint _start;
		osgEarth::GeoPoint _end;
		osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> m_ptrLabelNode;
		osgEarth::Symbology::Style m_SymStyle;
		GLOBE_NAME::AnalysisCallBackPtr m_ptrAnalysisCallBack;
    };
}
#endif
