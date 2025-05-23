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

class VSGGEO_EXPORT AxesNode : public osg::MatrixTransform
{
public:

				AxesNode();
				AxesNode(const AxesNode&,const osg::CopyOp& op =
					 osg::CopyOp::DEEP_COPY_ALL);
				META_Node(vsgGeo,AxesNode);

    void			traverse(osg::NodeVisitor&) override;
    void			setSize(osg::Vec2 size);
    void			setPosition(osg::Vec2 pos);
    void			setRadius(const float&);
    float			getRadius() const { return _radius; }
    float			getLength() const { return _length; }
    void			setLength(const float&);
    void			setAnnotationColor(osg::Vec4 col);
    void			setAnnotationText(int dim, const osgText::String&);
    const osgText::String&	getAnnotationText(int dim) const;
    void			setAnnotationTextSize(float);
    float			getAnnotationTextSize() const {return _textSize;}
    void			setAnnotationFont(osgText::Font*);
    [[deprecated("Use setPrimaryCamera")]]
    void			setMasterCamera(osg::Camera*);
    void			setPrimaryCamera(osg::Camera*);

protected:

    virtual			~AxesNode();
    bool			updateGeometry();
    bool			needsUpdate() const;
    bool			computeTransform(osg::Matrix&) const;

    float			_radius;
    float			_length;
    float			_textSize;
    osg::Vec2			_pos;
    osg::Vec2			_size;
    osg::Vec4			_annotColor;
    osg::ref_ptr<osgText::Font>	_font;
    bool			_needsUpdate;
    osg::ref_ptr<osg::Camera>	_masterCamera;

    osgText::String		_annotText[3];
};


} // namespace vsgGeo
