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

class LayeredTexture;
class Vec2i;

/*!Class to display an arbitrary section through a 3D volume with a layered
   texture. The section follows a path on the x-y plane, and starts/stops at
   the z-coordinates given by setZRange().
*/

class VSGGEO_EXPORT TexturePanelStripNode : public osg::Node
{
    class BoundingGeometry;
    class TextureCallbackHandler;

public:
				TexturePanelStripNode();
				TexturePanelStripNode(
					const TexturePanelStripNode&,
					const osg::CopyOp& op =
					osg::CopyOp::DEEP_COPY_ALL);
				META_Node(vsgGeo,TexturePanelStripNode);

    void			setTexture(vsgGeo::LayeredTexture*);
    vsgGeo::LayeredTexture*	getTexture()		{ return _texture; }

    void			freezeDisplay(bool yn=true);
				/*!<As long as TexturePanelStrip is frozen,
				    the update traversal is ignored, which
				    avoids showing half-finished updates of
				    lengthy geometry and/or texture changes. */
    bool			isDisplayFrozen() const	{ return _frozen; }

    void			setTextureBrickSize(int sz,bool strict=false);
				/*!<If not strict, the actual brick size is
				    allowed to be chosen smaller to optimize
				    performance. */

    void			setPath(const osg::Vec2Array& coords);
    const osg::Vec2Array&	getPath() const	{ return *_pathCoords; }

    void			setPath2TextureMapping(
					    const osg::FloatArray& offsets);
				/*!<Monotonously non-decreasing list of column
				    offsets into the texture. There should be
				    one offset per path coordinate. */
    const osg::FloatArray&	getPath2TextureMapping() const
						{ return *_pathTexOffsets; }

    void			setPathTextureShift(float shift,int startidx=0);
				/*!<Extra horizontal shift of (part of) the
				    specified texture mapping (in pixel units).
				    Shifting starts at path coord "startidx" */
    float			getPathTextureShift() const
						{ return _pathTextureShift; }
    int				getPathTextureShiftStartIdx() const
						{ return _pathTexShiftStartIdx;}

    void			setZRange(float top,float bottom);
    float			getBottom() const	{ return _bottom; }
    float			getTop() const		{ return _top; }

    void			setZRange2TextureMapping(bool yn,
						    float topOffset=0.0f,
						    float bottomOffset=1.0f);
				//!<Mapped on full texture row range if not set.

    float			getTopTextureMapping() const;
    float			getBottomTextureMapping() const;
    bool			isZRange2TextureMappingSet() const
						{ return _validZRangeOffsets; }

    void			setZTextureShift(float);
				/*!<Extra vertical shift of the specified
				    texture mapping (in pixel units). */
    float			getZTextureShift() const
						{ return _zTextureShift; }

    void			swapTextureAxes(bool yn=true);
    bool			areTextureAxesSwapped() const
						{ return _swapTextureAxes; }

    void			smoothNormals(bool yn=true);
    bool			areNormalsSmoothed() const
						{ return _smoothNormals; }

    osg::BoundingSphere		computeBound() const override;

    void			forceRedraw(bool=true);

    std::vector<osg::Geometry*>& getGeometries()	{ return _geometries; }

    const osg::Image*		getCompositeTextureImage(bool addBorder=true);
    const osg::Vec2Array*	getCompositeTextureCoords(int geomIdx) const;

protected:
    virtual			~TexturePanelStripNode();

    void			cleanUp();
    float			getTexelSizeRatio() const;
    void			traverse(osg::NodeVisitor&) override;
    bool			updateGeometry();

    int				getValidNormalIdx(int panelIdx,
						  bool forward) const;
    osg::Vec3			getAverageNormal(int knotIdx) const;
    void			computeNormals();

    float			calcZTexOffset(int idx) const;
    float			calcPathTexOffset(int idx) const;
    bool			getLocalGeomAtTexOffset(
				    osg::Vec2& pathCoord,osg::Vec3& normal,
				    float texOffset,int guessPanelIdx=-1) const;

    void			finalizeZTiling(
				    const std::vector<float>& tOrigins,
				    std::vector<float>& zCoords,
				    std::vector<float>& zOffsets);

    void			setUpdateVar(bool& var,bool yn);
				//! Will trigger redraw request if necessary

    bool			_needsUpdate;	// Only set via setUpdateVar(.)
    bool			_frozen;	// Only set via setUpdateVar(.)

    osg::ref_ptr<TextureCallbackHandler>	_textureCallbackHandler;

    OpenThreads::ReadWriteMutex			_redrawLock;
    bool					_isRedrawing;

    osg::ref_ptr<vsgGeo::LayeredTexture>	_texture;
    int						_textureBrickSize;
    bool					_isBrickSizeStrict;
    osg::ref_ptr<osg::Vec2Array>		_pathCoords;
    osg::ref_ptr<osg::FloatArray>		_pathTexOffsets;
    float					_pathTextureShift;
    int						_pathTexShiftStartIdx;
    float					_top;
    float					_bottom;
    bool					_validZRangeOffsets;
    float					_topTexOffset;
    float					_bottomTexOffset;
    float					_zTextureShift;
    bool					_swapTextureAxes;
    bool					_smoothNormals;
    std::vector<osg::Geometry*>			_geometries;
    std::vector<Vec2i>				_compositeCutoutOrigins;
    std::vector<Vec2i>				_compositeCutoutSizes;
    int						_compositeCutoutTexUnit;
    osg::ref_ptr<osg::Image>			_compositeImageWithBorder;
    osg::Vec2f					_borderEnvelopeOffset;
    std::vector<osg::StateSet*>			_statesets;
    osg::ref_ptr<osg::FloatArray>		_panelWidths;
    osg::ref_ptr<osg::Vec3Array>		_panelNormals;
    osg::ref_ptr<osg::Vec3Array>		_knotNormals;
    osg::ref_ptr<BoundingGeometry>		_boundingGeometry;

public:
			// Testing purposes only
    int			_altTileMode;
    void		toggleTilingMode()
			{
			    _altTileMode=(_altTileMode+1)%3;
			    setUpdateVar( _needsUpdate, true );
			}
};



}

