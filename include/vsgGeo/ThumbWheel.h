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


/**
 * Wheel for performing rotation
 */

class VSGGEO_EXPORT ThumbWheel : public osg::Node
{
public:
    ThumbWheel();

    void accept(osg::NodeVisitor&) override;
    osg::BoundingSphere computeBound() const override;

    void setShape(short dim,const osg::Vec2& min,const osg::Vec2& max,
		  float zval = 0);
    const osg::Vec2& getMin() const { return _min; }
    const osg::Vec2& getMax() const { return _max; }

    void enableFadeInOut(bool);
    bool isFadeInOutEnabled() const { return _hasFadeInOut; }
    void setAngle(float angle,float rotationTime=0.0);
    //!<angle in rad, rotation time in seconds
    float getAngle() const { return _currentAngle; }

    void setWheelColor(const osg::Vec4&);
    void setBorderColor(const osg::Vec4&);

    void addRotateCallback(osg::NodeCallback *nc);
    void removeRotateCallback(osg::NodeCallback *nc);

protected:
    friend class VSGGEO_EXPORT ThumbWheelEventHandler;
    void updateCoords();

    enum MouseProximity { Above, Nearby, None };

    MouseProximity getMouseProximity(const osg::Vec2& mousepos ) const;

    void restartAnimation();
    bool updateAnimation(float progress);
    //!<Returns if we should continue to animate
    bool isAnimating() const { return _animationStart>0; }
    bool updateRotation(float progress);

    void updateWheel( ThumbWheel::MouseProximity nmp );
    void updateWheelTexture(float diffAngle);
    bool handleEvent( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& );

    ~ThumbWheel();

    static osg::Texture2D* getSharedTexture();
    //Not threadsafe, don't call simultaniously

    osg::Vec2 _min;
    osg::Vec2 _max;

    osg::Geode* _geode;
    osg::Geometry* _wheelGeometry;
    osg::Material* _wheelMaterial;
    osg::Geometry* _outlineGeometry;
    osg::Material* _outlineMaterial;
    bool _isTracking;
    float _startPos;
    float _startAngle;
    float _currentAngle;
    short _dim;
#if OSG_MIN_VERSION_REQUIRED(3,3,2)
    osg::ref_ptr<osg::Callback> _cb;
#else
    osg::ref_ptr<osg::NodeCallback> _cb;
#endif

    MouseProximity _mouseProximity;

    bool _hasFadeInOut;
    double _animationTime;
    double _animationStart;

    float _rotationToDo;
    float _rotationProgressToDo;
    float _maxRotationProgress;
};


class VSGGEO_EXPORT ThumbWheelEventHandler : public osgGA::GUIEventHandler
{
public:
    ~ThumbWheelEventHandler();

    void addThumbWheel(ThumbWheel*);
    void removeThumbWheel(ThumbWheel*);

protected:
    bool handle(const osgGA::GUIEventAdapter &ea,osgGA::GUIActionAdapter &aa,
		osg::Object*,osg::NodeVisitor*) override;
    bool handle(const osgGA::GUIEventAdapter &ea,
		osgGA::GUIActionAdapter &aa) override
	{ return osgGA::GUIEventHandler::handle(ea,aa); }

#if OSG_MIN_VERSION_REQUIRED(3,3,1)
    bool handle( osgGA::Event* ev, osg::Object* obj,
		 osg::NodeVisitor* nv ) override
	{ return osgGA::GUIEventHandler::handle(ev,obj,nv); }
#endif

    std::vector<osg::ref_ptr<ThumbWheel> > _thumbwheels;
};

class VSGGEO_EXPORT ThumbWheelEventNodeVisitor : public osg::NodeVisitor
{
public:
    ThumbWheelEventNodeVisitor(float deltaangle);
    ~ThumbWheelEventNodeVisitor();
    float getDeltaAngle() const { return _deltaangle; }
protected:
    float _deltaangle;
};

} // namespace vsgGeo
