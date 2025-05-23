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

class VSGGEO_EXPORT	PolyLineNode : public osg::Node
{
public:
			PolyLineNode();
			PolyLineNode(const PolyLineNode&,
			    const osg::CopyOp& op = osg::CopyOp::DEEP_COPY_ALL);
			META_Node(vsgGeo,PolyLineNode);

    void		traverse(osg::NodeVisitor&) override;
    void		setVertexArray(osg::Array*);
    void		setRadius(float);
    float		getRadius() const { return _radius; }
    void		setResolution(int);
    int			getResolution() const { return _resolution; }
    void		useAutoScreenScaling(bool);
    void		addPrimitiveSet(osg::PrimitiveSet*);
    void		removePrimitiveSet(int);
    void		removeAllPrimitiveSets();
    int			getPrimitiveSetIndex(const osg::PrimitiveSet*) const;
    void		touchPrimitiveSet(int)			{}
    void		clearAll();

protected:

    virtual		~PolyLineNode();
    bool		updateGeometry();
    void		reScaleCoordinates(const osgUtil::CullVisitor*);
    bool		isCameraChanged(const osgUtil::CullVisitor*);
    bool		needsUpdate() const;

    void		getOrthoVecs(const osg::Vec3&,osg::Vec3&,osg::Vec3&) const;
    bool		buildA3DLineStrip(osg::Vec3Array&,
			    osg::Vec3Array&,int pidx0,int pidx1, int pidx2,bool first,bool last);
    void		addCap(const osg::Vec3& p,const osg::Vec3Array& cornerpoints,const osg::Vec3& norm);

    osg::BoundingSphere	computeBound() const override;
    void		touch();
    void		setUpdateFlag(bool);

    float			_radius;
    int				_resolution;
    osg::BoundingBox		_bbox;

    bool			_needsUpdate;
    bool			_isGeometryChanged;
    bool			_screenSizeScaling;
    float			_pixsz;


    unsigned int                 _arrayModifiedCount;
    std::vector<unsigned int>	 _primitivesetModCount;

    osg::Vec3Array*		 _geom3DCoords;
    osg::Vec3Array*		 _geom3DNormals;
    osg::ref_ptr<osg::Vec3Array> _polyLineCoords;
    osg::ref_ptr<osg::Vec3Array> _unScaledGeomCoords;
    osg::ref_ptr<osg::Geometry>	 _geometry;

    std::vector< osg::ref_ptr<osg::PrimitiveSet> >    _primitiveSets;
    std::vector<bool>		 _capflags;
};

}
