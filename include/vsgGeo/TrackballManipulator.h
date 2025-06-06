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

/* TrackballManipulator with customized event handling */
class VSGGEO_EXPORT TrackballManipulator : public osgGA::MultiTouchTrackballManipulator
{
    public :
	TrackballManipulator(int flags = DEFAULT_SETTINGS);
	TrackballManipulator(const TrackballManipulator& tm,
			     const osg::CopyOp& copyOp = osg::CopyOp::SHALLOW_COPY);

	~TrackballManipulator();

	void enableDragging(bool yn);
	void enableDragging(int mouseButtonMask);
	void enableKeyHandling(bool yn) { _keyHandlingEnabled=yn; }

        void setBoundTraversalMask(unsigned int nm) { _boundTraversalMask = nm; }
        void setViewAllMargin(float nv) { _viewallMargin=nv; }

        void animateTo(const osg::Vec3d& center,const osg::Quat& rotation,
                       double distance, bool animate = true);

        void viewAll(osg::View*,bool animate);
	void viewAll(osg::View* view,const osg::Vec3d& dir,
		     const osg::Vec3d& up, bool animate);
        osg::Matrix getInverseMatrix() const override;

        void changeDistance(float factor) { zoomModel(factor, true); }
        //!<factor is fraction of distance

        void addMovementCallback(osg::NodeCallback *nc);
	void removeMovementCallback(osg::NodeCallback *nc);

	bool isThrown() const { return _thrown; }
	//!<Is the scene auto-rotating?
        void stopThrow() { _thrown = false; }
        //!<Stop eventual auto-rotation

	void setProjectionAsPerspective(bool isPerspective);

	void updateCamera(osg::Camera&) override;
	bool isCameraPerspective() const{ return _perspectiveProjection; }
	bool isDiscreteZooming() const { return _isDiscreteZooming; }

	enum MouseButton { NoButton, LeftButton, MiddleButton, RightButton };

	void setRotateMouseButton(MouseButton mb) { _rotateMouseButton = mb; }
	void setPanMouseButton(MouseButton mb)	  { _panMouseButton = mb; }
	void setZoomMouseButton(MouseButton mb)	  { _zoomMouseButton = mb; }
	void setRotateModKeyMask(int mask)	  { _rotateModKeyMask = mask; }
	void setPanModKeyMask(int mask)		  { _panModKeyMask = mask; }
	void setZoomModKeyMask(int mask)	  { _zoomModKeyMask = mask; }

	void useLeftMouseButtonForAllMovement(bool yn,bool additional=false);

#if OSG_VERSION_LESS_THAN(3,3,2)
    protected:
	void handleMultiTouchDrag(const osgGA::GUIEventAdapter::TouchData* now,
				  const osgGA::GUIEventAdapter::TouchData* last,
                                  const double eventTimeDelta);
    private:

	bool handleTouch( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
#else
    private:
#endif
        static osg::Matrix getInverseMatrix(const osg::Vec3d& center,
				    const osg::Quat& rotation,
                                            double distance);
	bool computeViewAllParams(osg::View*,const osg::Quat& rotation,osg::Vec3d& center,double& distance) const;

        bool handle(const osgGA::GUIEventAdapter&,
		    osgGA::GUIActionAdapter&) override;
        bool handleKeyDown(const osgGA::GUIEventAdapter&,
			   osgGA::GUIActionAdapter& us) override;
        bool handleMouseWheel(const osgGA::GUIEventAdapter&,
			      osgGA::GUIActionAdapter& us) override;
	bool handleMouseWheelZoomOut(const osgGA::GUIEventAdapter&,
				     osgGA::GUIActionAdapter& us);
        bool handleMouseWheelZoomIn(const osgGA::GUIEventAdapter&,
				    osgGA::GUIActionAdapter& us);

        osg::Matrixd computePerspectiveProjectionFromOrtho(const osg::Camera& camera) const;

	bool getZoomCenterIntersectionPoint(osg::View* view, const osg::Vec2d& zoomcenter,osg::Vec3d& intersection) const;
	bool zoomIn(const osg::Vec3d& intersectionPos, float zoomFactor);
	bool zoomOut(osg::View*,float zoomFactor);

	bool isModKeyMatch( int mask ) const;
	int getMappedMouseButton(MouseButton) const;
	void notifyMappedMouseButtonEvents(const osgGA::GUIEventAdapter& ea);
	bool mapMouseButtonMovement(const double eventTimeDelta,const double dx,const double dy,MouseButton);

	bool performMovementLeftMouseButton(const double eventTimeDelta,
				const double dx,const double dy) override;
	bool performMovementMiddleMouseButton(const double eventTimeDelta,
				const double dx,const double dy) override;
	bool performMovementRightMouseButton(const double eventTimeDelta,
				const double dx,const double dy) override;
	bool getPolyIntersectionPoint(osg::View* view, const osg::Vec2d& zoomcenter,osg::Vec3d& intersection,float radius) const;

        unsigned int			_boundTraversalMask;
	bool				_dragEnabled;
	int				_dragMouseButMask;
	bool				_keyHandlingEnabled;
        float				_viewallMargin;
        float				_viewAllInitalFactor;
#if OSG_MIN_VERSION_REQUIRED(3,3,2)
	osg::ref_ptr<osg::Callback>	_cb;
#else
        osg::ref_ptr<osg::NodeCallback> _cb;
#endif
	bool				_perspectiveProjection;
	bool				_projectionSwitched;
	double				_fovy;
	bool				_isDiscreteZooming;

        osg::Vec2f                      _lastKnownMousePos;

	osg::Vec2d		        _touchZoomCenter;
	osg::View*			_touchEventView;

	MouseButton			_rotateMouseButton;
	MouseButton			_panMouseButton;
	MouseButton			_zoomMouseButton;
	int				_rotateModKeyMask;
	int				_panModKeyMask;
	int				_zoomModKeyMask;
	int				_currentModKeyMask;

	bool				_onlyUseLeftButtonForAllMovement;
	bool				_alsoUseLeftButtonForAllMovement;

        class TrackballAnimationData : public osgGA::StandardManipulator::AnimationData
        {
        public:
            osg::Vec3d		_centerMovement;
            osg::Quat		_rotationMovement;
            double		_distanceMovement;

            void start( const osg::Vec3d& centerMovement,
		const osg::Quat& rotationMovement,
                        double distanceMovement,
                        const double startTime );
        };

        void allocAnimationData() override
	{ _animationData = new TrackballAnimationData; }
        void applyAnimationStep(const double currentProgress,
				const double prevProgress) override;
};



class VSGGEO_EXPORT TrackballEventNodeVisitor : public osg::NodeVisitor
{
public:
    enum EventType { RotateStart, PanStart, ZoomStart, MoveStart, Moving, MoveStop };

    TrackballEventNodeVisitor(float deltahorangle, float deltavertangle, float distfactor);
    TrackballEventNodeVisitor(EventType);
    ~TrackballEventNodeVisitor();

    EventType _eventType;

    float _deltahorangle;
    float _deltavertangle;
    float _distfactor;
};


} // namespace vsgGeo
