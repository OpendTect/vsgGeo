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

#include <vsgGeo/MarkerShape.h>


namespace vsgGeo
{


class VSGGEO_EXPORT MarkerSet : public osg::Node
{
    public:
					MarkerSet();
					~MarkerSet();

	void				setVertexArray(osg::Vec3Array*);
	osg::Vec3Array*			getVertexArray() const { return _vertexArr; }

	void				setNormalArray(osg::Vec3Array*);
	osg::Vec3Array*			getNormalArray() const { return _normalArr; }

	void				setColorArray(osg::Vec4Array*);
	osg::Vec4Array*			getColorArray() const { return _colorArr; }

	void				setOnOffArray(osg::ByteArray*);
	osg::ByteArray*			getOnOffArray() const { return _onoffArr; }

	void				setSingleColor(osg::Vec4&);
	osg::Vec4			getSingleColor() const { return _singleColor; }

	void				useSingleColor(bool);
	bool				usesSingleColor() { return _applySingleColor; }

	void				setShape(vsgGeo::MarkerShape::ShapeType);
	vsgGeo::MarkerShape::ShapeType	getShape();

	void				setDetail(float ratio);
						//!<Between 0 and 1.
	float				getDetail();

	void				setMarkerSize(float, bool useScreensize = true);
	float				getMarkerSize();
	bool				isScreenSize() const { return _useScreenSize; }

	void				setMarkerHeightRatio(float);
	float				getMarkerHeightRatio();

	void				setRotationForAllMarkers(const osg::Quat&);
	const osg::Quat&		getRotationForAllMarkers() const;

	void				applyRotationToAllMarkers(bool);

	void				setSingleMarkerRotation(const osg::Quat&,int);

	void				setMinScale(float minscale);
	float				getMinScale() const {return _minScale;}

	void				setMaxScale(float maxscale);
	float				getMaxScale() const {return _maxScale;}

	void				setRotateMode(osg::AutoTransform::AutoRotateMode);
	osg::AutoTransform::AutoRotateMode
					getRotateMode() const { return _rotateMode; }

	void				turnMarkerOn(unsigned int idx,bool);
	bool				markerOn(unsigned int idx) const;
	void				turnAllMarkersOn(bool);
	void				forceRedraw(bool);
	void				removeAllMarkers();
	void				setPickable(bool);
	bool				isPickable() const;

    protected:
	osg::BoundingSphere		computeBound() const override;
	void				traverse(osg::NodeVisitor&) override;
	bool				updateShapes();
	osg::Vec4			getColor(int) const;
	bool				useShader(MarkerShape::ShapeType)const;

    private:

	osg::ref_ptr<osg::Vec3Array>		_vertexArr;
	osg::ref_ptr<osg::Vec4Array>		_colorArr;
	osg::ref_ptr<osg::ByteArray>		_onoffArr;
	osg::ref_ptr<osg::Vec3Array>		_normalArr;
	osg::ref_ptr<osg::Switch>		_switchNode;
	std::vector<osg::Quat>			_rotationSet;
	osg::AutoTransform::AutoRotateMode	_rotateMode;
	osg::Quat				_rotationForAllMarkers;
	bool					_applyRotationForAll;
	osg::Vec4f				_singleColor;
	bool					_ispickable;

	float					_minScale;
	float					_maxScale;
	bool					_useScreenSize;
	bool					_applySingleColor;
	vsgGeo::MarkerShape			_markerShape;
	bool					_forceRedraw;
	mutable osg::BoundingBox		_bbox;
	bool					_dirtyBoundAtNextCullStage;
	bool					_waitForAutoTransformUpdate;
	osg::Vec3				_prevEyePoint;
	OpenThreads::Mutex			_osgMutex;
};

}
