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


class BoundingGeometry : public osg::Geometry
{
/* BoundingGeometry is an auxiliary class tuning the intersection visitor
   process in two ways: it passes the VolumeTile to the IntersectionVisitor
   as a simple implementation of volume intersection (only sufficient in case
   of non-transparent volumes), and it provides a slightly bigger bounding
   box to circumvent unjustified clipping by osgUtil::LineSegmentIntersector
   which is caused by numerical instability. See osg-forum branch:
   "LineSegmentIntersector gives incorrect results (intersections missing)". */

public:
			BoundingGeometry(osgVolume::VolumeTechnique&);
#if OSG_MIN_VERSION_REQUIRED(3,3,2)
    osg::BoundingBox	computeBoundingBox() const override
			{ return _boundingBox; }
#else
    osg::BoundingBox	computeBound() const	{ return _boundingBox; }
#endif

protected:
    osg::BoundingBox		_boundingBox;
};


/* FixedFunctionTechnique with overruled filter settings */
class VSGGEO_EXPORT FixedFunctionTechnique : public osgVolume::FixedFunctionTechnique
{
    public :
	FixedFunctionTechnique();
	FixedFunctionTechnique(const FixedFunctionTechnique&,
			       const osg::CopyOp& copyop=
					osg::CopyOp::SHALLOW_COPY);

	void init() override;
	void traverse(osg::NodeVisitor&) override;

	void setBorderColor(const osg::Vec4f&);
	const osg::Vec4f& getBorderColor() const;

    protected:
	osg::ref_ptr<BoundingGeometry> _boundingGeometry;

	osg::Vec4f			_borderColor;
};


/* RayTracedTechnique with overruled filter settings and customized shaders */
class VSGGEO_EXPORT RayTracedTechnique : public osgVolume::RayTracedTechnique
{
    public :
	RayTracedTechnique(bool dynamicFragShading=false);
	RayTracedTechnique(const RayTracedTechnique&,
			   const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);

	void init() override;
	void traverse(osg::NodeVisitor&) override;

	void setBorderColor(const osg::Vec4f&);
	const osg::Vec4f& getBorderColor() const;

	void setCustomShader(osg::Shader::Type,const char* code);
	static const char* volumeTfFragDepthCode();

	enum FragShaderType { ColTab, RGBA, Identity };
	void setFragShaderType(FragShaderType);
	int getFragShaderType() const;

	void enableDestChannel(int destChannel,bool yn);
	bool isDestChannelEnabled(int destChannel) const;

	void setSourceChannel(int destChannel,int sourceChannel);
	int getSourceChannel(int destChannel) const;

	static const osg::Vec4f& getChannelDefaults();

	void setColTabValueChannel(int channel);
	int getColTabValueChannel() const;
	void setColTabUndefChannel(int undefChannel);
	int getColTabUndefChannel() const;
	void setColTabUndefValue(float undefValue);
	float getColTabUndefValue() const;
        void setColTabUndefColor(const osg::Vec4f& undefColor);
	const osg::Vec4f& getColTabUndefColor() const;
	void invertColTabUndefChannel(bool yn);
	bool isColTabUndefChannelInverted() const;

	static bool isShadingSupported();

    protected:
	void	updateFragShaderCode();

	std::vector< osg::ref_ptr<osg::Shader> >	_customShaders;
	osg::ref_ptr<BoundingGeometry>			_boundingGeometry;

	osg::ref_ptr<osg::Shader>	_dynamicFragShader;
	FragShaderType			_fragShaderType;
	bool				_dstEnabled[4];
	int				_srcChannel[4];

	int				_colTabValueChannel;
	int				_colTabUndefChannel;
	float				_colTabUndefValue;
	osg::Vec4f			_colTabUndefColor;
	bool				_invertColTabUndefChannel;

	osg::Vec4f			_borderColor;
};


} // namespace vsgGeo


