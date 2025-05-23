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

#include <vsgGeo/TabPlaneDragger.h>


namespace vsgGeo
{
/**
 * TabBoxDragger consists of 6 TabPlaneDraggers to form a box dragger that
 * performs translation and scaling.
 * It is a customized copy of osgManipulator::TabBoxDragger
 */

class VSGGEO_EXPORT TabBoxDragger : public osgManipulator::CompositeDragger
{
    public:

	TabBoxDragger(float handleScaleFactor=20.0f);

	META_OSGMANIPULATOR_Object(osgManipulator,TabBoxDragger)

	/** Setup default geometry for dragger. */
	void setupDefaultGeometry() override;

	void setPlaneColor(const osg::Vec4& color);

	const TabPlaneDragger* getEventHandlingTabPlane() const;
	int getEventHandlingTabPlaneIdx() const;
	int getCurMouseButtonModKeyIdx() const;

	void set1DTranslateMouseButtonMaskOfPlanes(int mask,int idx=0);
	int get1DTranslateMouseButtonMaskOfPlanes(int idx=0) const;
	void set2DTranslateMouseButtonMaskOfPlanes(int mask,int idx=0);
	int get2DTranslateMouseButtonMaskOfPlanes(int idx=0) const;

	void set1DTranslateModKeyMaskOfPlanes(int mask,int idx=0);
	int get1DTranslateModKeyMaskOfPlanes(int idx=0) const;
	void set2DTranslateModKeyMaskOfPlanes(int mask,int idx=0);
	int get2DTranslateModKeyMaskOfPlanes(int idx=0) const;

    protected:

	virtual ~TabBoxDragger();

	std::vector< osg::ref_ptr< vsgGeo::TabPlaneDragger > > _planeDraggers;
};

} // namespace vsgGeo
