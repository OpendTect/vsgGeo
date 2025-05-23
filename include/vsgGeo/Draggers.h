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

// Translate1DDragger with extra activation constraint functionality

class VSGGEO_EXPORT Translate1DDragger : public osgManipulator::Translate1DDragger
{
    public :

	Translate1DDragger();
	Translate1DDragger(const osg::Vec3d& s, const osg::Vec3d& e);

	void traverse(osg::NodeVisitor&) override;
	bool handle(const osgManipulator::PointerInfo&,
		    const osgGA::GUIEventAdapter&,
		    osgGA::GUIActionAdapter&) override;

	void setInactivationModKeyMask(unsigned int mask);
	unsigned int getInactivationModKeyMask() const;

    protected:
	unsigned int	_inactivationModKeyMask;
};


//=============================================================================


// Translate2DDragger with extra activation constraint functionality

class VSGGEO_EXPORT Translate2DDragger : public osgManipulator::Translate2DDragger
{
    public :

	Translate2DDragger();
	Translate2DDragger(const osg::Plane& plane);

	void traverse(osg::NodeVisitor&) override;
	bool handle(const osgManipulator::PointerInfo&,
		    const osgGA::GUIEventAdapter&,
		    osgGA::GUIActionAdapter&) override;

	void setInactivationModKeyMask(unsigned int mask);
	unsigned int getInactivationModKeyMask() const;

    protected:
	unsigned int	_inactivationModKeyMask;
};


} // namespace vsgGeo
