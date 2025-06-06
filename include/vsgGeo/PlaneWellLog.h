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

class VSGGEO_EXPORT PlaneWellLog : public vsgGeo::WellLog
{
public:
			PlaneWellLog();
                        PlaneWellLog(const PlaneWellLog&,
                                     const osg::CopyOp& op =
					osg::CopyOp::DEEP_COPY_ALL);
                        META_Node(vsgGeo,PlaneWellLog);

    void		setDisplaySide(DisplaySide) override;
    DisplaySide		getDisplaySide() const override { return _dispSide; }
    void		setRepeatNumber( unsigned int ) override;
    unsigned int	getRepeatNumber() const	override
			{ return _repeatNumber; }
    void		setRepeatGap (float) override;
    float		getRepeatGap() const override { return _repeatGap; }
    void		setFullFilled(bool) override;
    bool		getFullFilled() const override { return _isFullFilled; }

    void		setSeisLogStyle(bool) override;
    bool		getSeisLogStyle() const override { return _seisStyle; }

    void		setLogFill(bool) override;
    bool		getLogFill() const override { return _isFilled; }

    void		setResolution(int) override			{}
    int			getResolution() const override { return 0; }

    float		getRepeatStep() const override;
    osg::ref_ptr<osg::Geometry> getLogPathGeometry() const override
			{ return _lineGeometry; }
    osg::ref_ptr<osg::Geometry> getLogGeometry() const override
			{ return _triangleGeometry; }
    osg::ref_ptr<osg::Geometry>	getTubeGeometry() const override
			{ return nullptr; }

    void		clearLog() override;

protected:
				 ~PlaneWellLog();

    osg::BoundingSphere		computeBound() const override;
    void			traverse(osg::NodeVisitor&) override;
    float			getShapeFactor(float val, float minval,
                                           float maxval) const override;

    enum RenderMode		{LOGLINE_ONLY,SEISMIC_ONLY,
				 LOGFILL_ONLY,LOGLNFL_BOTH};

private:
    void			calcCoordinates(const osg::Vec3& normal);
    void			calcFactors();
    osg::Vec3			calcNormal(const osg::Vec3& projdir) const;
    void			updateFilledLogColor();
    void			clearFactors();
    void			clearCoords();
    void			buildLineGeometry();
    void			buildTriangleGeometry();
    RenderMode			getLogItem() const;

    osg::ref_ptr<osg::DrawArrays>	_linePrimitiveSet;
    osg::ref_ptr<osg::Geometry>		_lineGeometry;

    osg::ref_ptr<osg::Geometry>		_triangleGeometry;
    osg::ref_ptr<osg::DrawArrays>	_trianglePrimitiveSet;

    osg::ref_ptr<osg::Vec3Array>	_logLinedPoints;
    osg::ref_ptr<osg::Vec3Array>	_logLinedTriPoints;
    osg::ref_ptr<osg::FloatArray>	_coordLinedFactors;
    osg::ref_ptr<osg::FloatArray>	_coordLinedTriFactors;
    osg::ref_ptr<osg::Vec4Array>	_logColors;

    unsigned int    _repeatNumber;
    float	    _repeatGap;
    DisplaySide     _dispSide;
    bool	    _seisStyle;
    bool	    _isFilled;
    float	    _triGeometryWidth;
    bool	    _isFullFilled;
};


} // namespace

