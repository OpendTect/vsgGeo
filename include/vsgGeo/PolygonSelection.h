#pragma once

/* vsgGeo - A collection of geoscientific extensions to VulkanSceneGraph.
Copyright 2025 dGB Beheer B.V.

vsgGeo is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>

*/

#include <vsgGeo/Common.h>


namespace vsgGeo
{

class PolygonSelectionEventHandler;

class VSGGEO_EXPORT PolygonSelection : public osg::Node
{
public:
			    PolygonSelection();
			    PolygonSelection(const PolygonSelection&,
			    const osg::CopyOp& op = osg::CopyOp::DEEP_COPY_ALL);
    bool		    setEventHandlerCamera(osg::Camera*,bool handleAfterScene=true);

    void		    setHUDCamera(osg::Camera*);

    osg::Vec2		    projectPointToScreen(osg::Vec3) const; // deprecated
    bool		    projectPointToScreen(const osg::Vec3& pointIn3d,
						 osg::Vec2& pointIn2d) const;

    const osg::Vec3Array*   getCoords() const { return _coords; }

    enum ShapeType	    { Off, Polygon, Rectangle };
    ShapeType		    getShapeType() const  { return _shapeType; }
    void		    setShapeType(ShapeType st);

    void		    setColor(const osg::Vec4& col);

    void		    addCallBack(osg::NodeCallback *nc);
    void		    removeCallBack(osg::NodeCallback *nc);

    void		    setZCoord(float);
    float		    getZCoord() const { return _zcoord; }

    void		    clear();
    void		    accept(osg::NodeVisitor&) override;

protected:

    friend class	    PolygonSelectionEventHandler;

    osg::BoundingSphere	    computeBound() const override;

    void		    setLatestMousePoints(const osg::Vec3&);
    bool		    checkInteractionObjectIntersection(const osg::Vec3&) const;
    bool		    isCameraChanged();

    bool		    handleEvent(const osgGA::GUIEventAdapter&);

			    ~PolygonSelection();


    PolygonSelectionEventHandler*	_eventHandler;
#if OSG_MIN_VERSION_REQUIRED(3,3,2)
    osg::ref_ptr<osg::Callback>		_callback;
#else
    osg::ref_ptr<osg::NodeCallback>	_callback;
#endif
    osg::ref_ptr<osg::Geode>		_geode;
    osg::Geometry*			_lineGeometry;
    osg::Camera*			_masterCamera;
    osg::Vec3				_cameraPos;
    osg::Camera*			_hudCamera;
    osg::DrawArrays*                    _coordsList;
    ShapeType				_shapeType;
    osg::Material*			_material;
    osg::Vec3Array*			_coords;
    float                               _zcoord;
    bool                                _ison;
    bool                                _isDrawing;
    osg::BoundingBox	                _bbox;
};

} // namespace vsgGeo
