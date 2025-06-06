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

#include <vsgGeo/TrackballManipulator.h>
#include <vsgGeo/ComputeBoundsVisitor.h>


#define mAllTraversals (0xFFFFFFFF)

namespace vsgGeo
{


void ComputeBoundsVisitor::applyBoundingBox(const osg::BoundingBox& bbox)
{
    if ( !bbox.valid() )
	return;

    if ( _matrixStack.empty()) _bb.expandBy( bbox );
    else
    {
	const osg::Matrix& matrix = _matrixStack.back();
	if (_bb.valid())
	{
	    _bb.expandBy(bbox.corner(0) * matrix);
	    _bb.expandBy(bbox.corner(1) * matrix);
	    _bb.expandBy(bbox.corner(2) * matrix);
	    _bb.expandBy(bbox.corner(3) * matrix);
	    _bb.expandBy(bbox.corner(4) * matrix);
	    _bb.expandBy(bbox.corner(5) * matrix);
	    _bb.expandBy(bbox.corner(6) * matrix);
	    _bb.expandBy(bbox.corner(7) * matrix);
	}
    }
}


TrackballManipulator::TrackballManipulator(int flags)
    : osgGA::MultiTouchTrackballManipulator(flags)
    , _dragEnabled(true)
    , _keyHandlingEnabled(true)
    , _boundTraversalMask(mAllTraversals)
    , _viewallMargin(0.2f)
    , _viewAllInitalFactor(3.5f)
    , _projectionSwitched(false)
    , _perspectiveProjection(true)
    , _fovy(45.0f)
    , _touchZoomCenter(0,0)
    , _touchEventView(0)
    , _rotateMouseButton(LeftButton)
    , _panMouseButton(MiddleButton)
    , _zoomMouseButton(RightButton)
    , _rotateModKeyMask(-1)
    , _panModKeyMask(-1)
    , _zoomModKeyMask(-1)
    , _currentModKeyMask(0)
    , _onlyUseLeftButtonForAllMovement(false)
    , _alsoUseLeftButtonForAllMovement(false)
    , _isDiscreteZooming(false)
    , _lastKnownMousePos(-1,-1)
{
    enableDragging( true );
}


TrackballManipulator::TrackballManipulator(const TrackballManipulator& tm, const osg::CopyOp& copyOp)
    : osgGA::MultiTouchTrackballManipulator(tm, copyOp)
    , osg::Object(tm, copyOp)	// needs explicit init in copy constructor because of [-Wextra] warning
    , _dragEnabled(tm._dragEnabled)
    , _dragMouseButMask(tm._dragMouseButMask)
    , _keyHandlingEnabled(true)
    , _boundTraversalMask(tm._boundTraversalMask)
    , _viewallMargin(tm._viewallMargin)
    , _viewAllInitalFactor(tm._viewAllInitalFactor)
    , _touchZoomCenter(0,0)
    , _touchEventView(0)
    , _rotateMouseButton(LeftButton)
    , _panMouseButton(MiddleButton)
    , _zoomMouseButton(RightButton)
    , _rotateModKeyMask(-1)
    , _panModKeyMask(-1)
    , _zoomModKeyMask(-1)
    , _currentModKeyMask(0)
    , _onlyUseLeftButtonForAllMovement(false)
    , _alsoUseLeftButtonForAllMovement(false)
    , _isDiscreteZooming(false)
    , _lastKnownMousePos(-1,-1)
{}


TrackballManipulator::~TrackballManipulator()
{}


void TrackballManipulator::enableDragging( bool yn )
{
    _dragMouseButMask = osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON |
			osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON |
			osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON;
    if ( !yn )
	_dragMouseButMask = osgGA::GUIEventAdapter::NONE;
}


void TrackballManipulator::enableDragging( int mouseButtonMask )
{ _dragMouseButMask = mouseButtonMask; }


osg::Matrixd TrackballManipulator::computePerspectiveProjectionFromOrtho(const osg::Camera& camera) const
{
    double left, right, bottom, top, zNear, zFar;
    camera.getProjectionMatrixAsOrtho(left, right, bottom, top, zNear, zFar);
    if ( zNear< 0 )
    {
	zNear /=(-1.0f);
	zFar  += fabs(zNear);
    }

    const osg::Viewport* vwp = camera.getViewport();
    const double aspectRatio = vwp->aspectRatio();
    return osg::Matrix::perspective(_fovy,aspectRatio, zNear, zFar);
}


void TrackballManipulator::setProjectionAsPerspective(bool isPerspective)
{
    if ( isPerspective==_perspectiveProjection )
	return;

    _perspectiveProjection = isPerspective;

    _projectionSwitched = !_projectionSwitched;
}


void TrackballManipulator::updateCamera(osg::Camera& camera)
{
    if ( _perspectiveProjection )
    {
	if ( _projectionSwitched )
	{
	    osg::Matrixd mtd = computePerspectiveProjectionFromOrtho(camera);
	    camera.setProjectionMatrix(mtd);
	}
	else
	{
	    double fovy, oldAspectRatio, zNear, zFar;
	    const osg::Viewport* vwp = camera.getViewport();
	    const double aspectRatio = vwp->aspectRatio();
	    if ( camera.getProjectionMatrixAsPerspective(fovy, oldAspectRatio, zNear, zFar))
		camera.setProjectionMatrixAsPerspective(_fovy, aspectRatio, zNear, zFar);
	    else // Set to dummy (working) defaults
		camera.setProjectionMatrixAsPerspective(_fovy, 1, 1, 1000);
	}
    }
    else
    {
	double zNear = 0, zFar = 0;
	if ( _projectionSwitched )
	{
	    double fovy, oldAspectRatio;
	    camera.getProjectionMatrixAsPerspective(fovy, oldAspectRatio, zNear, zFar);
	}
	else
	{
	    double left, right, bottom, up;
	    camera.getProjectionMatrixAsOrtho(left, right, bottom, up, zNear, zFar);
	}

	const osg::Viewport* vwp = camera.getViewport();
	const double aspectRatio = vwp->aspectRatio();
	const double y = _distance * _fovy/90.;
	const double x = y*aspectRatio;

	camera.setProjectionMatrixAsOrtho(-x, x, -y, y, 1, 1000);
    }

    _projectionSwitched = false;

    camera.setViewMatrix(getInverseMatrix());

}


#define MAX(x,y) ((x)>(y)?(x) : (y))

bool TrackballManipulator::computeViewAllParams(osg::View* view, const osg::Quat& rotation,
						osg::Vec3d& center, double& distance) const
{
    if ( !_node )
	return false;

    vsgGeo::ComputeBoundsVisitor visitor(osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN);
    visitor.setTraversalMask(_boundTraversalMask);

    _node->accept(visitor);

    osg::BoundingBox &bb = visitor.getBoundingBox();
    if ( bb.valid() )
    {
	osg::BoundingSphere boundingSphere;
	boundingSphere.expandBy(bb);
	if ( !boundingSphere.valid() )
	    return false;

	distance = _viewAllInitalFactor * boundingSphere._radius;
	center = boundingSphere.center();

	if ( !view || !view->getCamera() || !view->getCamera()->getViewport() )
	    return true;

	const osg::Viewport* vp = view->getCamera()->getViewport();
	const osg::Matrix projMatrix = _perspectiveProjection
	    ? view->getCamera()->getProjectionMatrix()
	    : computePerspectiveProjectionFromOrtho(*view->getCamera());

	const osg::Matrix windowMatrix = vp->computeWindowMatrix();

	//Results are better if this is iterated, and two times is a nice compromize between
	//speed and result.
	float changeFactor = 0.05f;
	char prevsign = 0;
	for ( int idx=0; idx<100; idx++ )
	{
	    const osg::Matrix viewMatrix = getInverseMatrix( center, rotation, distance );
	    const osg::Matrix transform = viewMatrix * projMatrix * windowMatrix;

	    osg::BoundingBox screenBBox;
	    screenBBox.expandBy(osg::Vec3(bb.xMin(), bb.yMin(), bb.zMin()) * transform);
	    screenBBox.expandBy(osg::Vec3(bb.xMin(), bb.yMin(), bb.zMax()) * transform);
	    screenBBox.expandBy(osg::Vec3(bb.xMin(), bb.yMax(), bb.zMin()) * transform);
	    screenBBox.expandBy(osg::Vec3(bb.xMin(), bb.yMax(), bb.zMax()) * transform);
	    screenBBox.expandBy(osg::Vec3(bb.xMax(), bb.yMin(), bb.zMin()) * transform);
	    screenBBox.expandBy(osg::Vec3(bb.xMax(), bb.yMin(), bb.zMax()) * transform);
	    screenBBox.expandBy(osg::Vec3(bb.xMax(), bb.yMax(), bb.zMin()) * transform);
	    screenBBox.expandBy(osg::Vec3(bb.xMax(), bb.yMax(), bb.zMax()) * transform);

	    const float relXMaxFactor = fabs(screenBBox.xMax()/vp->width()-0.5f)*2;
	    const float relXMinFactor = fabs(screenBBox.xMin()/vp->width()-0.5f)*2;
	    const float relYMaxFactor = fabs(screenBBox.yMax()/vp->height()-0.5f)*2;
	    const float relYMinFactor = fabs(screenBBox.yMin()/vp->height()-0.5f)*2;

	    const float xFactor = MAX(relXMaxFactor,relXMinFactor);
	    const float yFactor = MAX(relYMaxFactor,relYMinFactor);

	    const float factor = MAX(xFactor,yFactor) * (1.0f+_viewallMargin);

	    if ( factor>0.99 && factor<1.01 )
		break;

	    const char cursign = factor>1 ? 1 : -1;

	    if ( !prevsign )
		prevsign = factor>1 ? 1 : -1;
	    else if ( prevsign!=cursign )
		changeFactor /=2;

	    if ( factor>1 )
		distance *= (1+changeFactor);
	    else
		distance /= (1+changeFactor);

	    prevsign = cursign;
	}
    }
    else
    {
	osg::BoundingSphere boundingSphere = _node->getBound();

	if ( !boundingSphere.valid() )
	    return false;

	distance = _viewAllInitalFactor * boundingSphere._radius;
	center = boundingSphere.center();
    }

    return true;
}


osg::Matrix TrackballManipulator::getInverseMatrix() const
{
    return getInverseMatrix(_center, _rotation, _distance);
}


osg::Matrix TrackballManipulator::getInverseMatrix(const osg::Vec3d& center,
						   const osg::Quat& rotation,
						   double distance)
{
    return osg::Matrixd::translate(-center) *
	osg::Matrixd::rotate(rotation.inverse()) *
	osg::Matrixd::translate(0.0, 0.0, -distance);
}


void TrackballManipulator::notifyMappedMouseButtonEvents(const osgGA::GUIEventAdapter& ea)
{
    if ( ea.getHandled() || !_dragEnabled )
	return;

    if ( ea.getEventType()!=osgGA::GUIEventAdapter::PUSH && ea.getEventType()!=osgGA::GUIEventAdapter::RELEASE )
	return;

    TrackballEventNodeVisitor nv( TrackballEventNodeVisitor::MoveStart );
    if ( ea.getEventType()==osgGA::GUIEventAdapter::PUSH )
    {
	MouseButton mouseButton = NoButton;

	if ( ea.getButtonMask()==osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON )
	    mouseButton = LeftButton;
	if ( ea.getButtonMask()==osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON )
	    mouseButton = MiddleButton;
	if ( ea.getButtonMask()==osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON )
	    mouseButton = RightButton;

	const MouseButton mappedMouseButton = (MouseButton) getMappedMouseButton(mouseButton);

	if ( mappedMouseButton == LeftButton )
	    nv._eventType = TrackballEventNodeVisitor::RotateStart;
	if ( mappedMouseButton == MiddleButton )
	    nv._eventType = TrackballEventNodeVisitor::PanStart;
	if ( mappedMouseButton == RightButton )
	    nv._eventType = TrackballEventNodeVisitor::ZoomStart;
    }
    else
	nv._eventType = TrackballEventNodeVisitor::MoveStop;

    osg::NodeCallback* nodecb = dynamic_cast<osg::NodeCallback*>( _cb.get() );
    if ( nodecb )
	(*nodecb)( 0, &nv );
}


bool TrackballManipulator::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
    if ( ea.getEventType()==osgGA::GUIEventAdapter::MOVE && !ea.isMultiTouchEvent() )
    {
	_lastKnownMousePos = osg::Vec2( ea.getX(), ea.getY() );
    }

    if ( ea.getEventType()==osgGA::GUIEventAdapter::PUSH )
	_currentModKeyMask = ea.getModKeyMask();

    if ( ea.getEventType()==osgGA::GUIEventAdapter::FRAME )
    {
	return osgGA::MultiTouchTrackballManipulator::handle(ea, aa);
    }

    osg::Vec3d oldEyePos, oldCenterPos, oldUpDir;
    getTransformation(oldEyePos, oldCenterPos, oldUpDir);
    const double oldDist = _distance;

    bool res(false);

    if ( ea.isMultiTouchEvent() )
    {
	_touchEventView = aa.asView();
#if OSG_VERSION_LESS_THAN(3,3,2)
	res = handleTouch( ea, aa );
#else
	res = osgGA::MultiTouchTrackballManipulator::handle(ea, aa);
#endif
    }
    else
    {
	if ( ea.getButton() )
	    _dragEnabled = ea.getButton() & _dragMouseButMask;

	_touchEventView = 0;
	if ( !_dragEnabled && ea.getEventType()==osgGA::GUIEventAdapter::DRAG )
	    return false;
	res = osgGA::MultiTouchTrackballManipulator::handle(ea, aa);
    }

    if ( _cb )
    {
	float horAngle = 0;
	float vertAngle = 0;
	if ( ea.getEventType()==osgGA::GUIEventAdapter::DRAG || ea.isMultiTouchEvent() )
	{
	    osg::Vec3 oldViewDir = oldEyePos-oldCenterPos;
	    oldViewDir.normalize();

	    osg::Vec3d newEyePos, newCenterPos, newUpDir;
	    getTransformation(newEyePos, newCenterPos, newUpDir);

	    osg::Vec3 newViewDir = newEyePos-newCenterPos;
	    newViewDir.normalize();

	    const float rotationAngle = newViewDir * oldViewDir;
	    if ( rotationAngle )
	    {
		const osg::Vec3 rotationAxis = newViewDir^oldViewDir;

		osg::Vec3 vertAxis = (oldViewDir)^oldUpDir;
		vertAxis.normalize();
		osg::Vec3 horAxis = oldUpDir;
		horAxis.normalize();

		horAngle = rotationAngle * (horAxis*rotationAxis);
		vertAngle = rotationAngle * (vertAxis*rotationAxis);
	    }
	}

	if ( horAngle || vertAngle || _distance!=oldDist )
	{
	    TrackballEventNodeVisitor nv( horAngle, vertAngle, (_distance-oldDist)/oldDist );

	    osg::NodeCallback* nodecb = dynamic_cast<osg::NodeCallback*>( _cb.get() );
	    if ( nodecb )
		(*nodecb)( 0, &nv );
	}

	notifyMappedMouseButtonEvents( ea );
    }

    return res;
}


bool TrackballManipulator::handleKeyDown( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
    if ( !_keyHandlingEnabled )
	return false;

    return osgGA::MultiTouchTrackballManipulator::handleKeyDown( ea, us );
}


void TrackballManipulator::useLeftMouseButtonForAllMovement( bool yn, bool additional )
{
    _onlyUseLeftButtonForAllMovement = yn && !additional;
    _alsoUseLeftButtonForAllMovement = yn &&  additional;
}


bool TrackballManipulator::isModKeyMatch( int mask ) const
{
    return mask==-1 || mask==_currentModKeyMask || mask&_currentModKeyMask;
}


int TrackballManipulator::getMappedMouseButton( MouseButton mouseButton ) const
{
    if ( !_onlyUseLeftButtonForAllMovement )
    {
	if ( mouseButton==_rotateMouseButton && isModKeyMatch(_rotateModKeyMask) )
	    return LeftButton;
	if ( mouseButton==_panMouseButton && isModKeyMatch(_panModKeyMask) )
	    return MiddleButton;
	if ( mouseButton==_zoomMouseButton && isModKeyMatch(_zoomModKeyMask) )
	    return RightButton;
    }

    if ( _onlyUseLeftButtonForAllMovement || _alsoUseLeftButtonForAllMovement )
    {
	if ( mouseButton == LeftButton )
	{
	    const int shift = osgGA::GUIEventAdapter::MODKEY_SHIFT;
	    const int ctrl = osgGA::GUIEventAdapter::MODKEY_CTRL;

	    if ( !isModKeyMatch(shift) && !isModKeyMatch(ctrl) )
		return LeftButton;
	    if ( isModKeyMatch(shift) && !isModKeyMatch(ctrl) )
		return MiddleButton;
	    if ( !isModKeyMatch(shift) && isModKeyMatch(ctrl) )
		return MiddleButton;
	    if ( isModKeyMatch(shift) && isModKeyMatch(ctrl) )
		return RightButton;
	}
    }

    return NoButton;
}


bool TrackballManipulator::mapMouseButtonMovement( const double eventTimeDelta, const double dx, const double dy, MouseButton mouseButton )
{
    const MouseButton mappedMouseButton = (MouseButton) getMappedMouseButton( mouseButton );

    if ( mappedMouseButton==LeftButton )
	return osgGA::MultiTouchTrackballManipulator::performMovementLeftMouseButton( eventTimeDelta, dx, dy );

    if ( mappedMouseButton==MiddleButton )
	return osgGA::MultiTouchTrackballManipulator::performMovementMiddleMouseButton( eventTimeDelta, dx, dy );

    if ( mappedMouseButton==RightButton )
    {
	_isDiscreteZooming = false;
	return osgGA::MultiTouchTrackballManipulator::performMovementRightMouseButton( eventTimeDelta, dx, dy );
    }

    return false;
}


bool TrackballManipulator::performMovementLeftMouseButton( const double eventTimeDelta, const double dx, const double dy )
{
    return mapMouseButtonMovement( eventTimeDelta, dx, dy, LeftButton );
}


bool TrackballManipulator::performMovementMiddleMouseButton( const double eventTimeDelta, const double dx, const double dy )
{
    return mapMouseButtonMovement( eventTimeDelta, dx, dy, MiddleButton );
}

bool TrackballManipulator::performMovementRightMouseButton( const double eventTimeDelta, const double dx, const double dy )
{
    return mapMouseButtonMovement( eventTimeDelta, dx, dy, RightButton );
}


TrackballEventNodeVisitor::TrackballEventNodeVisitor(float deltahorangle, float deltavertangle, float distfactor)
    : _eventType( Moving )
    , _deltahorangle(deltahorangle)
    , _deltavertangle(deltavertangle)
    , _distfactor(distfactor)
{}


TrackballEventNodeVisitor::TrackballEventNodeVisitor(EventType et)
    : _eventType( et )
    , _deltahorangle(0.0)
    , _deltavertangle(0.0)
    , _distfactor(1.0)
{}


TrackballEventNodeVisitor::~TrackballEventNodeVisitor()
{}



#define mDefaultHandling osgGA::TrackballManipulator::handleMouseWheel(ea, us)


bool TrackballManipulator::handleMouseWheel(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
    bool res = false;
    const osgGA::GUIEventAdapter::ScrollingMotion sm = ea.getScrollingMotion();

    stopThrow();

    if( ((sm == osgGA::GUIEventAdapter::SCROLL_DOWN && _wheelZoomFactor > 0.)) ||
       ((sm == osgGA::GUIEventAdapter::SCROLL_UP   && _wheelZoomFactor < 0.)) )
    {
	res = handleMouseWheelZoomIn(ea, us);
	_isDiscreteZooming = true;
    }

    if( ((sm == osgGA::GUIEventAdapter::SCROLL_UP && _wheelZoomFactor > 0.)) ||
       ((sm == osgGA::GUIEventAdapter::SCROLL_DOWN   && _wheelZoomFactor < 0.)))
    {
	 res = handleMouseWheelZoomOut(ea, us);
	 _isDiscreteZooming = true;
    }

    return res ? true : mDefaultHandling;
}


bool TrackballManipulator::handleMouseWheelZoomOut(const osgGA::GUIEventAdapter&,
						   osgGA::GUIActionAdapter& us)
{
    return zoomOut(us.asView(),fabs(_wheelZoomFactor));
}


bool TrackballManipulator::handleMouseWheelZoomIn(const osgGA::GUIEventAdapter& ea,
						   osgGA::GUIActionAdapter& us)
{
    osg::Vec3d intersectionPos;
    if ( _lastKnownMousePos.x()<0 || _lastKnownMousePos.y()<0 )
	return false;

    if ( !getZoomCenterIntersectionPoint(us.asView(),_lastKnownMousePos,intersectionPos) )
	return false;

    return zoomIn(intersectionPos,fabs(_wheelZoomFactor));
}


bool TrackballManipulator::zoomIn(const osg::Vec3d& intersectionPos, float zoomFactor)
{
    //General idea is that we move center point towards the eye, and the eye-point towards
    //the intersection point.
    //If we are close we will move the center of rotation further away though to avoid getting stuck
    osg::Vec3d oldEyePos, oldCenterPos, oldUpDir;
    getTransformation(oldEyePos, oldCenterPos, oldUpDir);

    osg::Vec3d movementDir = intersectionPos-oldEyePos;
    const double oldDistance = movementDir.length();
    if ( oldDistance<1.0 )
	return false;
    zoomFactor = 0.1;
    movementDir /= oldDistance;

    double minDist = _minimumDistance;
    if( getRelativeFlag(_minimumDistanceFlagIndex ))
	minDist *= _modelSize;

    double movementDist = zoomFactor * oldDistance;
    if (oldDistance-movementDist<minDist)
    {
	movementDist = zoomFactor*minDist;
    }

    const osg::Vec3d movement = movementDir*movementDist;
    const osg::Vec3d newEyePos = oldEyePos + movement;
    osg::Vec3d newCenterDir = oldCenterPos - oldEyePos;
    const double centerDist = newCenterDir.length();

    newCenterDir /= centerDist;

    double centerMovementDist = centerDist * zoomFactor;
    if ( centerDist-centerMovementDist<minDist )
    {
	centerMovementDist = zoomFactor*minDist;
    }

    const osg::Vec3d newCenterPos = newEyePos + newCenterDir * (centerDist-centerMovementDist);
    setTransformation(newEyePos, newCenterPos, oldUpDir);

    return true;
}


bool TrackballManipulator::zoomOut(osg::View* view,float zoomFactor)
{
  if ( !_node.get() || !view )
	return false;

    osg::Vec3d oldEyePos, oldCenterPos, oldUpDir;
    getTransformation(oldEyePos, oldCenterPos, oldUpDir);

    /*General idea is that if we are close, we move the eyepoint towards the
      viewAll eye-point, and move the center towards the viewAll-center.

      If we are not close, we simply move eyepoint backwards (standard interaction */

    osg::Vec3d viewAllCenter;
    double viewAllDistance;

    if ( !computeViewAllParams( view, _rotation, viewAllCenter, viewAllDistance) )
	return false;

    const double currentDistFromIdealCenter = (oldEyePos-viewAllCenter).length();
    if ( currentDistFromIdealCenter>viewAllDistance)
	return false;

    osg::Vec3d viewDir = oldEyePos - oldCenterPos;
    const double viewDirLength = viewDir.length();
    if ( viewDirLength<1.0 )
	return false;

    viewDir /= viewDirLength;

    osg::Vec3d viewAllPos = viewAllCenter + viewDir * viewAllDistance;

    const osg::Vec3d eyePath = viewAllPos - oldEyePos;
    const double eyePathLength = eyePath.length();
    if ( eyePathLength<1 )
	return false;

    const osg::Vec3d eyePathDir = eyePath / eyePathLength;

    double eyeMovementLength = _distance * zoomFactor;
    if ( eyeMovementLength> eyePathLength )
	eyeMovementLength = eyePathLength;

    const double factor = eyeMovementLength / eyePathLength;

    const osg::Vec3d newEyePos = oldEyePos + eyePathDir * eyeMovementLength;

    const osg::Vec3d centerPath =  viewAllCenter - oldCenterPos;
    const double centerPathLength = centerPath.length();
    osg::Vec3d newCenterPos = oldCenterPos;
    if ( centerPathLength>1 )
    {
	const osg::Vec3d centerPathDir = centerPath / centerPathLength;
	double centerMovementLength = centerPathLength * factor;
	newCenterPos = oldCenterPos + centerPathDir * centerMovementLength;
    }

    setTransformation(newEyePos, newCenterPos, oldUpDir);

    return true;
}


bool TrackballManipulator::getZoomCenterIntersectionPoint(osg::View* view, const osg::Vec2d& zoomcenter,osg::Vec3d& intersection) const
{
    if ( !view )
	return false;

    osg::Camera *camera = view->getCamera();
    if( !camera )
	return false;

    // perform intersection computation
    osg::ref_ptr< osgUtil::LineSegmentIntersector > picker =
	new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, zoomcenter.x(), zoomcenter.y());
    osgUtil::IntersectionVisitor iv( picker.get());
    iv.setTraversalMask( _intersectTraversalMask );
    camera->accept( iv );

    // return on no intersections
    if( !picker->containsIntersections() )
    {
	if ( _perspectiveProjection )
	return false;

	bool findIntersection = false;
	float radius = 1.0f;
	while ( radius<100.0f )
	{
	    findIntersection = getPolyIntersectionPoint(view,zoomcenter,intersection,radius);
	    if ( findIntersection )
		break;
	    else
		radius += 2.0f;
	}

	return findIntersection;
     }

    // get all intersections
    osgUtil::LineSegmentIntersector::Intersections& intersections = picker->getIntersections();

    // new center
    intersection = (*intersections.begin()).getWorldIntersectPoint();


    return true;
}


bool TrackballManipulator::getPolyIntersectionPoint(osg::View* view,const osg::Vec2d& zoomcenter,
 osg::Vec3d& intersection,float radius) const
{
     osg::ref_ptr<osgUtil::PolytopeIntersector> polyIntersector =
	    new osgUtil::PolytopeIntersector(osgUtil::Intersector::WINDOW,
	    zoomcenter.x()-radius,zoomcenter.y()-radius,zoomcenter.x()+radius,
	    zoomcenter.y()+radius);

     polyIntersector->setPrimitiveMask(osgUtil::PolytopeIntersector::ALL_PRIMITIVES);

     osgUtil::IntersectionVisitor iv(polyIntersector.get());
     view->getCamera()->accept( iv );

     if ( polyIntersector->containsIntersections() )
     {
	    const osgUtil::PolytopeIntersector::Intersection polypick =
	    polyIntersector->getFirstIntersection();

	    osg::Vec3 pickCenter = polypick.localIntersectionPoint;
	    if ( polypick.matrix->valid() )
	    {
	    intersection = pickCenter*( *polypick.matrix );
	    return true;
	    }
	    return false;
     }

 return false;

}


#if OSG_VERSION_LESS_THAN(3,3,2)
void TrackballManipulator::handleMultiTouchDrag(const osgGA::GUIEventAdapter::TouchData* now,
    const osgGA::GUIEventAdapter::TouchData* last, const double )
{
    const osg::Vec2 pt_1_now(now->get(0).x_,now->get(0).y_);
    const osg::Vec2 pt_2_now(now->get(1).x_,now->get(1).y_);
    const osg::Vec2 pt_1_last(last->get(0).x_,last->get(0).y_);
    const osg::Vec2 pt_2_last(last->get(1).x_,last->get(1).y_);

    if ( last->get(0).phase == osgGA::GUIEventAdapter::TOUCH_BEGAN )
    {
	_touchZoomCenter = (pt_1_last + pt_2_last)/2;
    }

    if ( last->get(0).phase == osgGA::GUIEventAdapter::TOUCH_ENDED )
    {
	_touchZoomCenter.x() = 0;
	_touchZoomCenter.y() = 0;
    }


    const float gap_now((pt_1_now - pt_2_now).length());
    const float gap_last((pt_1_last - pt_2_last).length());

    const float zoomFactor = (gap_last - gap_now)/gap_last;

    bool success = false;
    if ( fabs(zoomFactor) > 0.02f && _touchZoomCenter.length() !=0 )
    {
	if ( zoomFactor< 0 )
	{
	    osg::Vec3d intersectionPos;
	    if ( getZoomCenterIntersectionPoint(_touchEventView,
					_touchZoomCenter,intersectionPos) )
		success = zoomIn(intersectionPos, fabs(zoomFactor) );
	}
	else
	{
	    success = zoomOut(_touchEventView, fabs(zoomFactor) );
	}
    }

    _isDiscreteZooming = false;

    if ( success )
	return;

    if (fabs(zoomFactor) > 0.02f)
	zoomModel(zoomFactor , true);
}
#endif

void TrackballManipulator::animateTo(const osg::Vec3d& newCenter,
				     const osg::Quat& newRotation,
				     double newDistance,
				     bool animate)
{
    if ( animate && getAnimationTime() )
    {
	TrackballAnimationData* ad = dynamic_cast<TrackballAnimationData*>(_animationData.get());
	if ( ad )
	{
	    ad->start(newCenter-_center, newRotation-_rotation,
		      newDistance-_distance, osg::Timer::instance()->time_s());
	    return;
	}
    }

    _center = newCenter;
    _distance = newDistance;
    _rotation = newRotation;
}


static bool isProjectionMatrixSet(osg::Camera* camera,bool perspectiveProjection)
{
    if ( !camera ) return false;

    double zNear, zFar;
    if ( perspectiveProjection )
    {
	double fovy, oldAspectRatio;
	return camera->getProjectionMatrixAsPerspective(fovy, oldAspectRatio, zNear, zFar);
    }
    else
    {
	double left, right, bottom, up;
	return  camera->getProjectionMatrixAsOrtho(left, right, bottom, up, zNear, zFar);
    }

}


void TrackballManipulator::viewAll(osg::View* view, const osg::Vec3d& dir, const osg::Vec3d& up, bool animate)
{
    osg::Vec3d f( -dir );  f.normalize();
    osg::Vec3d s( f^up );  s.normalize();
    osg::Vec3d u( s^f );   u.normalize();

    const osg::Matrixd rotation_matrix(s[0], u[0], -f[0], 0.0f,
				  s[1], u[1], -f[1], 0.0f,
				  s[2], u[2], -f[2], 0.0f,
				  0.0f, 0.0f,  0.0f, 1.0f);

    const osg::Quat newRotation = rotation_matrix.getRotate().inverse();

    osg::Vec3d newCenter;
    double newDistance;

    if ( !isProjectionMatrixSet(view->getCamera(),_perspectiveProjection) )
	updateCamera( *view->getCamera() );

    computeViewAllParams(view, newRotation, newCenter, newDistance);

    animateTo(newCenter, newRotation, newDistance, animate );
}


void TrackballManipulator::viewAll(osg::View* view, bool animate)
{
    osg::Vec3d newCenter;
    double newDistance;

    if ( !isProjectionMatrixSet(view->getCamera(),_perspectiveProjection) )
	updateCamera( *view->getCamera() );

    computeViewAllParams(view, _rotation, newCenter, newDistance);

    animateTo(newCenter, _rotation, newDistance, animate);
}


void TrackballManipulator::TrackballAnimationData::start( const osg::Vec3d& centerMovement,
	       const osg::Quat& rotationMovement,
	       double distanceMovement,
	       const double startTime )
{
    AnimationData::start(startTime);

    _centerMovement = centerMovement;
    _rotationMovement = rotationMovement;
    _distanceMovement = distanceMovement;
}


void  TrackballManipulator::applyAnimationStep( const double currentProgress, const double prevProgress )
{
    TrackballAnimationData* ad = dynamic_cast< TrackballAnimationData* >( _animationData.get() );
    if ( !ad )
	return;

    const double prevMovementProgress = 0.5 * (1 - cos(prevProgress*M_PI));
    const double currentMovementProgress = 0.5 * (1 - cos(currentProgress*M_PI));
    if ( prevMovementProgress >= 1.0 )
	return;

    const double progress = currentMovementProgress - prevMovementProgress;
    const double frac = progress / (1.0 - prevMovementProgress);

    const osg::Vec3d newCenter = _center + ad->_centerMovement*frac;
    ad->_centerMovement -= newCenter - _center;
    _center = newCenter;

    osg::Quat newRotation;
    newRotation.slerp( frac, _rotation, _rotation+ad->_rotationMovement );
    ad->_rotationMovement -= newRotation - _rotation;
    _rotation = newRotation;

    const double newDistance = _distance + ad->_distanceMovement*frac;
    ad->_distanceMovement -= newDistance - _distance;
    _distance = newDistance;
}


void TrackballManipulator::addMovementCallback(osg::NodeCallback* nc)
{
    if ( !_cb )
	_cb = nc;
    else
	_cb->addNestedCallback(nc);
}


void TrackballManipulator::removeMovementCallback(osg::NodeCallback* nc)
{
    if ( nc==_cb )
	_cb = _cb->getNestedCallback();
    else
	_cb->removeNestedCallback(nc);


}


#if OSG_VERSION_LESS_THAN(3,3,2)

bool TrackballManipulator::handleTouch(const osgGA::GUIEventAdapter& ea,
				       osgGA::GUIActionAdapter& aa)
{
    switch(ea.getEventType())
    {

    case osgGA::GUIEventAdapter::PUSH:
    case osgGA::GUIEventAdapter::DRAG:
    case osgGA::GUIEventAdapter::RELEASE:
	if ( ea.isMultiTouchEvent() )
	{
	    double eventTimeDelta = 1/60.0; //_ga_t0->getTime() - _ga_t1->getTime();
	    if( eventTimeDelta < 0. )
	    {
		OSG_WARN << "Manipulator warning: eventTimeDelta = " << eventTimeDelta << std::endl;
		eventTimeDelta = 0.;
	    }

	    osgGA::GUIEventAdapter::TouchData* data = ea.getTouchData();

	    // three touches or two taps for home position
	    if ( (data->getNumTouchPoints() == 3) || ((data->getNumTouchPoints() == 1) && (data->get(0).tapCount >= 2)) )
	    {
		flushMouseEventStack();
		_thrown = false;
		home(ea,aa);
	    }

	    else if ( data->getNumTouchPoints() >= 2 )
	    {
		if ( (_lastTouchData.valid()) && (_lastTouchData->getNumTouchPoints() >= 2) )
		{
		    handleMultiTouchDrag(data, _lastTouchData.get(), eventTimeDelta);
		}
	    }

	    _lastTouchData = data;

	    // check if all touches ended
	    unsigned int num_touches_ended(0);
	    for( osgGA::GUIEventAdapter::TouchData::iterator i = data->begin(); i != data->end(); ++i )
	    {
		if ( (*i).phase == osgGA::GUIEventAdapter::TOUCH_ENDED )
		    num_touches_ended++;
	    }

	    if( num_touches_ended == data->getNumTouchPoints() )
	    {
		_lastTouchData = NULL;
	    }

	}
	break;
    default:
	break;
    }

    return osgGA::MultiTouchTrackballManipulator::handle(ea, aa);
}


#endif



} // end namespace
