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


class VSGGEO_EXPORT WellLog : public osg::Node
{
public:
				WellLog();
                                WellLog(const WellLog&,
	const osg::CopyOp& op = osg::CopyOp::DEEP_COPY_ALL);

    void			setPath(osg::Vec3Array*);
    osg::Vec3Array*		getPath(){return _logPath;}
    const osg::Vec3Array*	getPath() const { return _logPath; }

    void			setShapeLog(osg::FloatArray*);
				/*!<Linked with path. Start is assumed to be
				    start of path.*/
    osg::FloatArray*		getShapeLog() {return _shapeLog;}
    const osg::FloatArray*	getShapeLog() const {return _shapeLog;}
    void			setMaxShapeValue(float);
    float			getMaxShapeValue() const {return _maxShapeValue;}

    void			setMinShapeValue(float);
    float			getMinShapeValue()const {return _minShapeValue;}
    void			setFillLogValues(osg::FloatArray*);
				/*!<Linked with path. Start is assumed to be
				    start of path. */
    osg::FloatArray*		getFillLogValues() {return _fillLog;}
    const osg::FloatArray*	getFillLogValues() const {return _fillLog;}
    void			setFillLogDepths(osg::FloatArray*);
    osg::FloatArray*		getFillLogDepths() {return _fillLogDepths;}

    void			setMaxFillValue(float);
    float			getMaxFillValue() const {return _maxFillValue;}

    void			setMinFillValue(float);
    float			getMinFillValue() const {return _minFillValue;}

    void			setFillLogColorTab(osg::Vec4Array*);
    osg::Vec4Array*		getFillLogColorTab() {return _colorTable;}
    const osg::Vec4Array*	getFillLogColorTab() const {return _colorTable;}

    void			setShowLog(bool);
    bool			getShowLog() const {return _showLog;}

    void			setLogWidth(float);
    float			getLogWidth() const {return _logWidth;}

    void			setRevScale(bool yn);
    bool			getRevScale() const { return _revScale; }

    void			setFillRevScale(bool yn);
    bool			getFillRevScale() const { return _fillRevScale; }

    void			setLineColor(const osg::Vec4&);
    const osg::Vec4&		getLineColor() const;
    void			setLineWidth(float);
    float			getLineWidth() const;

    bool			getDisplayStatus();

    enum			DisplaySide {Left, Right, Center};
    virtual void		setDisplaySide(DisplaySide)		= 0;
    virtual DisplaySide		getDisplaySide() const			= 0;
    virtual void		setRepeatNumber(unsigned int)		= 0;
    virtual unsigned int	getRepeatNumber() const			= 0;
    virtual void		setRepeatGap(float)			= 0;
    virtual float		getRepeatGap() const			= 0;
    virtual void		setFullFilled(bool)			= 0;
    virtual bool		getFullFilled() const			= 0;
    virtual void		setSeisLogStyle(bool)			= 0;
    virtual bool		getSeisLogStyle() const			= 0;
    virtual void		setLogFill(bool)			= 0;
    virtual bool		getLogFill() const			= 0;
    virtual void		setResolution(int)			= 0;
    virtual int			getResolution() const			= 0;

    virtual float		getRepeatStep() const			= 0;
    virtual osg::ref_ptr<osg::Geometry> getLogPathGeometry() const	= 0;
    virtual osg::ref_ptr<osg::Geometry> getLogGeometry() const		= 0;
    virtual osg::ref_ptr<osg::Geometry>	getTubeGeometry() const		= 0;

    virtual void		clearLog();

protected:
				~WellLog();

    osg::BoundingSphere		computeBound() const override		= 0;
    void			traverse(osg::NodeVisitor&) override;
    virtual float		getShapeFactor(float logval,float minval,
					       float maxval) const;

    void			clearDraw();
    bool			eyeChanged(const osg::Vec3);

    osg::Vec3			getPrjDirection(const osgUtil::CullVisitor* cv) const;
    int				getClosestIndex(const osg::FloatArray& arr, float value);
    osg::ref_ptr<osg::Geode>		_geode;
    osg::ref_ptr<osg::Group>		_nonShadingGroup;
    osg::ref_ptr<osg::Vec4Array>	_colorTable;
    osg::ref_ptr<osg::Vec3Array>	_logPath;
    osg::ref_ptr<osg::FloatArray>	_shapeLog;
    osg::ref_ptr<osg::FloatArray>	_fillLog;
    osg::ref_ptr<osg::FloatArray>	_fillLogDepths;
    std::vector<int>			_outFillIndex;
    osg::ref_ptr<osg::Vec4Array>	_lineColor;
    osg::ref_ptr<osg::LineWidth>	_lineWidth;

    bool		_showLog;
    bool		_colorTableChanged;
    bool		_logWidthChanged;
    bool		_forceCoordReCalculation;
    bool		_forceReBuild;
    bool		_revScale;
    bool		_fillRevScale;
    float		_maxShapeValue;
    float		_minShapeValue;
    float		_logWidth;
    float		_maxFillValue;
    float		_minFillValue;
    osg::Vec3		_preProjDir;
    osg::BoundingBox		_bbox;

};

} // namespace


