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
#include <vsgGeo/Line3.h>

namespace vsgGeo
{

/**
Node that determines if its drawables should be rendered dependent
on the camera position versus a line in space. */

class VSGGEO_EXPORT OneSideRender : public osg::Node
{
public:

			OneSideRender();
			OneSideRender(const OneSideRender&,
                                      const osg::CopyOp& op = osg::CopyOp::DEEP_COPY_ALL);
			META_Node(vsgGeo,OneSideRender)

    int			getNumDrawables() const { return _drawables.size(); }
    osg::Drawable*	getDrawable(int idx) { return _drawables[idx]; }
    const osg::Drawable* getDrawable(int idx) const { return _drawables[idx]; }
    bool		addDrawable (osg::Drawable* gset);
			/*!<Add a osg::Drawable with a default position of
			    osg::Vec3(0,0,0) and direction osg::Vec3(0,0,1). */
    bool		addDrawable (osg::Drawable *gset, const Line3&);
			/*!<Add a osg::Drawable with a specified position and
			    direction */
    bool		removeDrawable (osg::Drawable* gset);
			//!<Remove a osg::Drawable and its associated position.

    void		setLine(unsigned int idx,const Line3&);
    Line3		getLine(unsigned int idx) const { return _lines[idx]; }

    void		traverse(osg::NodeVisitor&) override;

    bool		isDrawn(int dim,bool first) const;

protected:
    typedef std::vector<osg::ref_ptr<osg::Drawable> > DrawableList;
    osg::BoundingSphere			computeBound() const override;

    std::vector<Line3>			_lines;
    DrawableList			_drawables;
    std::vector<bool>			_isdrawn;
    osg::BoundingBox			_bbox;
};

}
