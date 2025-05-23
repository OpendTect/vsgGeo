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

#include <osg/Node>
#include <vsgGeo/Common>
#include <osg/Geode>
#include <osg/Vec3>


namespace vsgGeo
{


class OSGGEO_EXPORT HeightField : public osg::Geode
{
public:
				HeightField();
                                HeightField(const HeightField&,
	const osg::CopyOp& op = osg::CopyOp::DEEP_COPY_ALL);

    void			setHeightData(osg::Vec2Array*);
    const osg::Vec2Array*	setHeightData();

    void			setUndefValue(float);
    float			getUndefValue() const;


protected:
				~HeightField();
    virtual osg::BoundingSphere	computeBound() const;


    osg::ref_ptr<osg::Vec2Array>	_heightfield;

    osg::ref_ptr<osg::Geometry>		_geometry;
    osg::ref_ptr<osg::Vec3Array>	_coordinates;
    osg::ref_ptr<osg::Shader>		_vertexshader;
    osg::ref_ptr<osg::Image>		_heighttexture;
};

} // namespace
