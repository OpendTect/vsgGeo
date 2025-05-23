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

class VSGGEO_EXPORT Line3
{
public:
                Line3() {}
		Line3(const osg::Vec3& pos,const osg::Vec3& dir)
		    : _pos( pos ), _dir( dir ) {}
		Line3(const Line3& l) : _pos(l._pos), _dir(l._dir) {}

    bool	intersectWith(const osg::Plane& plane,
			      float& intercept) const;

    osg::Vec3	getPositionOnLine(float t) const { return _pos + (_dir * t);}
    osg::Vec3	getInterSectionPoint(const osg::Plane& plane) const;

    osg::Vec3	_pos;
    osg::Vec3	_dir;
};


VSGGEO_EXPORT osgDB::InputStream& operator>>(osgDB::InputStream&,Line3&);
VSGGEO_EXPORT osgDB::OutputStream& operator<<(osgDB::OutputStream&,const Line3&);

} // namespace vsgGeo
