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

class VSGGEO_EXPORT MarkerShape
{
    public:
			MarkerShape();
				~MarkerShape();

        enum			ShapeType { None=-1,Box=0, Cone, Cylinder,
					  Sphere, Plane, Cross, Arrow, Point };

	void			setType(ShapeType);
	ShapeType		getType() { return _shapeType; }

	void			setSize(float);
	float			getSize() const { return _size; }

	void			setHeightRatio(float);
	float			getHeightRatio() const { return _heightRatio; }

	void			setDetail(float);
	float			getDetail() const;

	void			setCenter(const osg::Vec3f&);
	const osg::Vec3f&	getCenter() const { return _center; }

	osg::ref_ptr<osg::Drawable> createShape(const osg::Vec4& color,
						    const osg::Quat& rot) const;
	osg::ref_ptr<osg::Geode>    createPoints(osg::Vec3Array*,osg::Vec4Array*);
	bool				isShadingSupported() const;

    protected:

	osg::ref_ptr<osg::Drawable>     createCrossDrawable(const osg::Vec4& col) const;
	osg::ref_ptr<osg::Drawable>     createArrowDrawable(const osg::Vec4& color,
							    const osg::Quat& rot) const;
	void				addShader(osg::StateSet*,ShapeType);

	osg::ref_ptr<osg::TessellationHints>	_hints;
	osg::Vec3f				_center;
	float					_size;
	float					_heightRatio;
	ShapeType				_shapeType;

	static const char*			getVertexShader();
	static const char*			getGeomShaderBox();
	static const char*			getGeomShaderSphere();
};

} // namespace vsgGeo
