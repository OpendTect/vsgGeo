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

#include <vsgGeo/TabPlaneDragger.h>


namespace vsgGeo
{


TabPlaneDragger::TabPlaneDragger( float handleScaleFactor )
    : osgManipulator::TabPlaneDragger( handleScaleFactor )
    , _curMouseButModKeyIdx( -1 )
    , _normalizedPosOnScreen( 0.0, 0.0 )
    , _positionOnScreen( 0.0, 0.0 )
    , _normalAngleToCamera( M_PI/2 )
    , _upwardPlaneAxisProj( 0.0, 0.0 )
    , _planeNormalProj( 0.0, 0.0 )
{
    set1DTranslateMouseButtonMask( osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON );
    set1DTranslateModKeyMask( osgGA::GUIEventAdapter::NONE );

    set2DTranslateMouseButtonMask( osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON );
    set2DTranslateModKeyMask( osgGA::GUIEventAdapter::MODKEY_SHIFT );
}


void TabPlaneDragger::traverse( osg::NodeVisitor& nv )
{
    if ( nv.getVisitorType()==osg::NodeVisitor::CULL_VISITOR )
    {
	osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>( &nv );
	const osg::RefMatrix& MVPW = *cv->getMVPW();

	const osg::Vec3 xVec = osg::Vec3(0.5,0,0)*MVPW-osg::Vec3(-0.5,0,0)*MVPW;
	const osg::Vec3 yVec = osg::Vec3(0,0.5,0)*MVPW-osg::Vec3(0,-0.5,0)*MVPW;
	const osg::Vec3 zVec = osg::Vec3(0,0,0.5)*MVPW-osg::Vec3(0,0,-0.5)*MVPW;

	osg::Vec2 xProj( xVec[0], xVec[1] );
	osg::Vec2 yProj( zVec[0], zVec[1] );
	osg::Vec2 zProj( yVec[0], yVec[1] );

	const float xLen = xProj.length();
	const float yLen = yProj.length();

	xProj.normalize(); yProj.normalize(); zProj.normalize();

	const float xWeight = fabs( xProj * zProj );
	const float yWeight = fabs( yProj * zProj );

	osg::Vec2 normalProjDir = zProj;
	normalProjDir *= (xWeight*xLen+yWeight*yLen) / (xWeight+yWeight);

	osg::Vec2 upAxisDir = xProj * xLen;
	if ( fabs(xProj[1]) < fabs(yProj[1]) ) 
	    upAxisDir = yProj * yLen;
	if ( upAxisDir[1] < 0.0 )
	    upAxisDir = -upAxisDir;

	const osg::RefMatrix& MV = *cv->getModelViewMatrix();
	osg::Vec3 normalDir = osg::Vec3(0,0,0)*MV - osg::Vec3(0,1,0)*MV;
	float cosAngle = normalDir.normalize() ? normalDir[2] : 1.0;

	if ( cosAngle < 0.0 )
	{
	    cosAngle = -cosAngle;
	    upAxisDir = -upAxisDir;
	}

	_normalAngleToCamera = acos( cosAngle );
	_planeNormalProj = normalProjDir;
	_upwardPlaneAxisProj = upAxisDir;
    }

    osgManipulator::TabPlaneDragger::traverse( nv );
}


bool TabPlaneDragger::handleInternal(const osgManipulator::PointerInfo& pointer, const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
    if (ea.getButtonMask() & osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON) return false;

    // Check if the dragger node is in the nodepath.
    if (!pointer.contains(this)) return false;

    // Since the translate plane and the handleNode lie on the same plane the hit could've been on either one. But we
    // need to handle the scaling draggers before the translation. Check if the node path has the scaling nodes else
    // check for the scaling nodes in next hit.
    if (_cornerScaleDragger->handle(pointer, ea, aa))
        return true;
    if (_horzEdgeScaleDragger->handle(pointer, ea, aa))
        return true;
    if (_vertEdgeScaleDragger->handle(pointer, ea, aa))
        return true;

    osgManipulator::PointerInfo nextPointer(pointer);
    nextPointer.next();

    while (!nextPointer.completed())
    {
        if (_cornerScaleDragger->handle(nextPointer, ea, aa))
            return true;
        if (_horzEdgeScaleDragger->handle(nextPointer, ea, aa))
            return true;
        if (_vertEdgeScaleDragger->handle(nextPointer, ea, aa))
            return true;

        nextPointer.next();
    }

    // Start of customization. The rest of this function is supposed to
    // remain an exact copy of the handle(.) function in the base class.

    osg::ref_ptr<osgGA::GUIEventAdapter> ea1 = new osgGA::GUIEventAdapter(ea);
    if ( !convToTranslatePlaneDraggerEvent(*ea1) )
	return false;

    if (_translateDragger->handle(pointer, *ea1, aa))
        return true;

    // End of customization

    return false;
}


static TabPlaneDragger* _currentEventHandler = 0;

bool TabPlaneDragger::handle(const osgManipulator::PointerInfo& pointer, const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
    _currentEventHandler = this;
    bool res = handleInternal( pointer, ea, aa );
    _currentEventHandler = 0;
    return res;
}


bool TabPlaneDragger::isCurrentEventHandler() const
{
    return _currentEventHandler==this;
}


int TabPlaneDragger::getCurMouseButtonModKeyIdx() const
{ return _curMouseButModKeyIdx; }


osg::Vec2 TabPlaneDragger::getNormalizedPosOnPlane() const
{
    const osg::Vec3 localIntersecPoint = _pointer.getLocalIntersectPoint(); 
    return osg::Vec2( localIntersecPoint[0]+0.5, localIntersecPoint[2]+0.5 );
}


const osg::Vec2& TabPlaneDragger::getPositionOnScreen() const
{ return _positionOnScreen; }


const osg::Vec2& TabPlaneDragger::getNormalizedPosOnScreen() const
{ return _normalizedPosOnScreen; }


float TabPlaneDragger::getPlaneNormalAngleToCamera() const
{ return _normalAngleToCamera; }


const osg::Vec2& TabPlaneDragger::getPlaneNormalProjOnScreen() const
{ return _planeNormalProj; }


const osg::Vec2& TabPlaneDragger::getUpwardPlaneAxisProjOnScreen() const
{ return _upwardPlaneAxisProj; }


static bool isModKeyMaskMatching( osgGA::GUIEventAdapter& ea, int mask )
{
    const int ctrl  = osgGA::GUIEventAdapter::MODKEY_CTRL;
    const int shift = osgGA::GUIEventAdapter::MODKEY_SHIFT;
    const int alt   = osgGA::GUIEventAdapter::MODKEY_ALT;
    const int meta  = osgGA::GUIEventAdapter::MODKEY_META;
    const int super = osgGA::GUIEventAdapter::MODKEY_SUPER;
    const int hyper = osgGA::GUIEventAdapter::MODKEY_HYPER;

    // Let's be insensitive to left and/or right mod key issues as long as
    // distinguishing between them not fully supported by OSG (actually Qt).

    if ( (ea.getModKeyMask()&ctrl)  != (mask&ctrl) )  return false;
    if ( (ea.getModKeyMask()&shift) != (mask&shift) ) return false;
    if ( (ea.getModKeyMask()&alt)   != (mask&alt) )   return false;
    if ( (ea.getModKeyMask()&meta)  != (mask&meta) )  return false;
    if ( (ea.getModKeyMask()&super) != (mask&super) ) return false;
    if ( (ea.getModKeyMask()&hyper) != (mask&hyper) ) return false;

    return true;
}


bool TabPlaneDragger::convToTranslatePlaneDraggerEvent( osgGA::GUIEventAdapter& ea )
{
    _positionOnScreen[0] = ea.getX();
    _positionOnScreen[1] = ea.getY();
    _normalizedPosOnScreen[0] = ea.getXnormalized();
    _normalizedPosOnScreen[1] = ea.getYnormalized();

    if ( ea.getEventType()==osgGA::GUIEventAdapter::RELEASE )
	_curMouseButModKeyIdx = -1;

    for ( int idx=0; idx<_mouseButMasks1D.size() || idx<_modKeyMasks1D.size(); idx++ )
    {
	if ( ea.getButtonMask()==get1DTranslateMouseButtonMask(idx) &&
	     isModKeyMaskMatching(ea,get1DTranslateModKeyMask(idx)) )
	{
	    // Perpendicular-to-plane translation
	    ea.setButtonMask( osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON );
	    ea.setModKeyMask( osgGA::GUIEventAdapter::NONE );

	    if ( ea.getEventType()==osgGA::GUIEventAdapter::PUSH )
		_curMouseButModKeyIdx = idx;

	    return true;
	}
    }

    for ( int idx=0; idx<_mouseButMasks2D.size() || idx<_modKeyMasks2D.size(); idx++ )
    {
	if ( ea.getButtonMask()==get2DTranslateMouseButtonMask(idx) &&
	     isModKeyMaskMatching(ea,get2DTranslateModKeyMask(idx)) )
	{
	    // In-plane translation
	    ea.setButtonMask( osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON );
	    ea.setModKeyMask( osgGA::GUIEventAdapter::NONE );
	    _curMouseButModKeyIdx = idx;

	    if ( ea.getEventType()==osgGA::GUIEventAdapter::PUSH )
		_curMouseButModKeyIdx = idx;

	    return true;
	}
    }

    // Don't steal RELEASE. Otherwise Translate1DDragger is not deactivated!
    return ea.getEventType()==osgGA::GUIEventAdapter::RELEASE;
}


void TabPlaneDragger::set1DTranslateMouseButtonMask( int mask, int idx )
{
    if ( idx>=0 && idx<_mouseButMasks1D.size() )
	_mouseButMasks1D[idx] = mask;
    else if ( idx==_mouseButMasks1D.size() )
	_mouseButMasks1D.push_back( mask );
}


int TabPlaneDragger::get1DTranslateMouseButtonMask( int idx ) const
{
    if ( idx>=0 && idx<_mouseButMasks1D.size() )
	return _mouseButMasks1D[idx];

    return osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON;
}


void TabPlaneDragger::set2DTranslateMouseButtonMask( int mask, int idx )
{
    if ( idx>=0 && idx<_mouseButMasks2D.size() )
	_mouseButMasks2D[idx] = mask;
    else if ( idx==_mouseButMasks2D.size() )
	_mouseButMasks2D.push_back( mask );
}


int TabPlaneDragger::get2DTranslateMouseButtonMask( int idx ) const
{
    if ( idx>=0 && idx<_mouseButMasks2D.size() )
	return _mouseButMasks2D[idx];

    return osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON;
}


void TabPlaneDragger::set1DTranslateModKeyMask( int mask, int idx )
{
    if ( idx>=0 && idx<_modKeyMasks1D.size() )
	_modKeyMasks1D[idx] = mask;
    else if ( idx==_modKeyMasks1D.size() )
	_modKeyMasks1D.push_back( mask );
}


int TabPlaneDragger::get1DTranslateModKeyMask( int idx ) const
{
    if ( idx>=0 && idx<_modKeyMasks1D.size() )
	return _modKeyMasks1D[idx];

    return osgGA::GUIEventAdapter::NONE;
}


void TabPlaneDragger::set2DTranslateModKeyMask( int mask, int idx )
{
    if ( idx>=0 && idx<_modKeyMasks2D.size() )
	_modKeyMasks2D[idx] = mask;
    else if ( idx==_modKeyMasks2D.size() )
	_modKeyMasks2D.push_back( mask );
}


int TabPlaneDragger::get2DTranslateModKeyMask( int idx ) const
{
    if ( idx>=0 && idx<_modKeyMasks2D.size() )
	return _modKeyMasks2D[idx];

    return osgGA::GUIEventAdapter::NONE;
}


} // end namespace

