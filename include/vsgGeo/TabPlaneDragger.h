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

// TabPlaneDragger with customized TranslatePlaneDragger event handling

class VSGGEO_EXPORT TabPlaneDragger : public osgManipulator::TabPlaneDragger
{
    public :

	TabPlaneDragger(float handleScaleFactor=20.0f);

	bool handle(const osgManipulator::PointerInfo& pi,
		    const osgGA::GUIEventAdapter& ea,
		    osgGA::GUIActionAdapter& aa) override;

        bool isCurrentEventHandler() const;

	int getCurMouseButtonModKeyIdx() const;
	const osg::Vec2& getPositionOnScreen() const;
	osg::Vec2 getNormalizedPosOnPlane() const;
	const osg::Vec2& getNormalizedPosOnScreen() const;

	float getPlaneNormalAngleToCamera() const;
	const osg::Vec2& getPlaneNormalProjOnScreen() const;
	const osg::Vec2& getUpwardPlaneAxisProjOnScreen() const;

	void set1DTranslateMouseButtonMask(int mask,int idx=0);
	int get1DTranslateMouseButtonMask(int idx=0) const;
	void set2DTranslateMouseButtonMask(int mask,int idx=0);
	int get2DTranslateMouseButtonMask(int idx=0) const;

	void set1DTranslateModKeyMask(int mask,int idx=0);
	int get1DTranslateModKeyMask(int idx=0) const;
	void set2DTranslateModKeyMask(int mask,int idx=0);
	int get2DTranslateModKeyMask(int idx=0) const;

    protected:

	bool handleInternal(const osgManipulator::PointerInfo& pi, const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	bool convToTranslatePlaneDraggerEvent(osgGA::GUIEventAdapter&);

	void traverse(osg::NodeVisitor&) override;

	int			_curMouseButModKeyIdx;
	osg::Vec2		_positionOnScreen;
	osg::Vec2		_normalizedPosOnScreen;
	float			_normalAngleToCamera;
	osg::Vec2		_upwardPlaneAxisProj;
	osg::Vec2		_planeNormalProj;

	std::vector<int>	_mouseButMasks1D;
	std::vector<int>	_mouseButMasks2D;
	std::vector<int>	_modKeyMasks1D;
	std::vector<int>	_modKeyMasks2D;
};

} // namespace vsgGeo
