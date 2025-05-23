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

#include <vsgGeo/WellLog.h>


namespace vsgGeo
{

class VSGGEO_EXPORT TubeWellLog : public vsgGeo::WellLog
{
public:
			TubeWellLog();
                                TubeWellLog(const TubeWellLog&,
                                        const osg::CopyOp& op = osg::CopyOp::DEEP_COPY_ALL);
                                META_Node(vsgGeo,TubeWellLog);

    enum RenderMode	{ RenderBackSide=-1, RenderBothSides, RenderFrontSide };

    void		setRenderMode(RenderMode mode);
    RenderMode		getRenderMode() const;

    void		setResolution(int) override;
    int			getResolution() const override	{ return _resolution; }

    void		setDisplaySide(DisplaySide) override		{}
    DisplaySide		getDisplaySide() const override { return Left; }
    void		setRepeatNumber(unsigned int) override		{}
    unsigned int	getRepeatNumber() const override	{ return 0; }
    void		setRepeatGap (float) override			{}
    float		getRepeatGap() const override	{ return .0f; }
    void		setFullFilled(bool) override			{}
    bool		getFullFilled() const override	{ return false; }
    void		setSeisLogStyle(bool) override			{}
    bool		getSeisLogStyle() const override { return false; }
    void		setLogFill(bool) override			{}
    bool		getLogFill() const override	{ return false; }
    void		clearLog() override;
    float		getRepeatStep() const override { return 0; }
    osg::ref_ptr<osg::Geometry>	getTubeGeometry() const override
			{ return _tubeGeometry; }
    osg::ref_ptr<osg::Geometry> getLogPathGeometry() const override
			{ return _logPathGeometry; }
    osg::ref_ptr<osg::Geometry> getLogGeometry() const override
			{ return nullptr; }

protected:
					~TubeWellLog();

    osg::BoundingSphere			computeBound() const override;
    void				traverse(osg::NodeVisitor&) override;
    float				getShapeFactor(float val,float minval,
						float maxval) const override;
private:
    void			calcTubeShape();
    void			updateTubeLogColor();
    void			clearTubeShape();
    void			clearVerts();
    void			buildTubeGeometry();
    void			buildCenterLineGeometry();
    void			buildTube(bool) ;

    std::vector<int>			_outTubeIndex;
    osg::ref_ptr<osg::Vec3Array>	_logTubeVerts;
    osg::ref_ptr<osg::Geometry>		_tubeGeometry;
    osg::ref_ptr<osg::Vec4Array>	_tubeLogColors;
    osg::ref_ptr<osg::Vec4Array>	_logColors;
    osg::ref_ptr<osg::FloatArray>	_coordLinedFactors;
    osg::ref_ptr<osg::Geometry>		_logPathGeometry;
    osg::ref_ptr<osg::DrawArrays>	_logPathPrimitiveSet;
    osg::ref_ptr<osg::Vec3Array>	_logPathVerts;
    osg::ref_ptr<osg::Vec4Array>	_logPathColor;
    osg::ref_ptr<osg::LineWidth>	_logPathWidth;
    osg::ref_ptr<osg::Vec3Array>	_logTubeShapePoints;
    osg::ref_ptr<osg::Vec3Array>	_logTubeCircleNormals;
    int					_resolution;

};


} // namespace


