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

class VSGGEO_EXPORT TexturePlaneNode : public osg::Node
{
    class BoundingGeometry;
    class TextureCallbackHandler;

public:

				TexturePlaneNode();
				TexturePlaneNode(const TexturePlaneNode&,
				    const osg::CopyOp& op =
					osg::CopyOp::DEEP_COPY_ALL);
				META_Node(vsgGeo,TexturePlaneNode);

    void			setCenter(const osg::Vec3&);
    const osg::Vec3&		getCenter() const;

    void			setTextureBrickSize(int sz,bool strict=false);
				/*!<If not strict, the actual brick size might
				   be chosen smaller for performance reasons. */

    int				getTextureBrickSize() const;

    void			setWidth(const osg::Vec3&);
				/*!<One dim must be zero.
				    Negative width yields mirrored dim. */
    const osg::Vec3&		getWidth() const;

    void			setRotation(const osg::Quat& quaternion);
				//!<Only needed if plane is not axes-aligned
    const osg::Quat&		getRotation() const;

    void			swapTextureAxes(bool yn=true);
    bool			areTextureAxesSwapped() const;

    void			traverse(osg::NodeVisitor&) override;

    void			setLayeredTexture(LayeredTexture*);
    LayeredTexture*		getLayeredTexture();
    const LayeredTexture*	getLayeredTexture() const;

    void			freezeDisplay(bool yn=true);
				/*!<As long as texture plane is frozen, the
				    update traversal is ignored, which avoids
				    showing half-finished updates of lengthy
				    geometry and/or texture changes. */
    bool			isDisplayFrozen() const;

    void			setTextureShift(const osg::Vec2&);
				/*!<Shift of the texture envelope center
				    (in pixel units) with regard to the
				    center of the plane geometry. */
    const osg::Vec2&		getTextureShift() const;

    void			setTextureGrowth(const osg::Vec2&);
				/*!<Size increase/decrease of the texture
				    envelope (in pixel units). Its default
				    size is one pixel shorter than the image
				    envelope (half-a-pixel at every border). */
    const osg::Vec2&		getTextureGrowth() const;

    char			getThinDim() const;

    osg::BoundingSphere		computeBound() const override;

    void			forceRedraw(bool=true);

    osg::Vec3f			getTexelSpanVector(int texdim);

    std::vector<osg::Geometry*>& getGeometries()	{ return _geometries; }
    const osg::Image*		getCompositeTextureImage(bool addBorder=true);
    const osg::Vec2Array*	getCompositeTextureCoords(int geomIdx) const;

protected:
    virtual			~TexturePlaneNode();

    void			cleanUp();
    float			getTexelSizeRatio() const;
    void			finalizeTiling(std::vector<float>& origins,
					       int dim) const;
    bool			needsUpdate() const;
    bool			updateGeometry();
    float			getSense() const;

    void			setUpdateVar(bool& var,bool yn);
				//! Will trigger redraw request if necessary

    bool			_needsUpdate;	// Only set via setUpdateVar(.)
    bool			_frozen;	// Only set via setUpdateVar(.)

    osg::ref_ptr<TextureCallbackHandler>	_textureCallbackHandler;

    OpenThreads::ReadWriteMutex		_redrawLock;
    bool				_isRedrawing;

    osg::Vec3				_center;
    osg::Vec3				_width;
    osg::Quat				_rotation;
    int					_textureBrickSize;
    bool				_isBrickSizeStrict;

    bool				_swapTextureAxes;

    osg::ref_ptr<LayeredTexture>	_texture;
    osg::Vec2				_textureShift;
    osg::Vec2				_textureGrowth;

    std::vector<osg::Geometry*>		_geometries;
    std::vector<Vec2i>			_compositeCutoutOrigins;
    std::vector<Vec2i>			_compositeCutoutSizes;
    int					_compositeCutoutTexUnit;
    osg::ref_ptr<osg::Image>		_compositeImageWithBorder;
    osg::Vec2f				_borderEnvelopeOffset;
    std::vector<osg::StateSet*>		_statesets;

    osg::ref_ptr<BoundingGeometry>	_boundingGeometry;

public:
			// Testing purposes only

    int			_disperseFactor;
    void		setDisperseFactor( int val )
			{
			    _disperseFactor = val;
			    setUpdateVar( _needsUpdate, true );
			}
    int			getDisperseFactor() const
			{ return _disperseFactor; }

    int			_nrQuadsPerBrickSide;
    void		setQuadsPerBrickSide( int nr )
			{
			    if ( nr>0 )
			    {
				_nrQuadsPerBrickSide = nr;
				setUpdateVar( _needsUpdate, true );
			    }
			}
			// TODO: Needs more efficient GL primitives if used
			// for real high-resolution vertex offset shading

    int			getQuadsPerBrickSide() const
			{ return _nrQuadsPerBrickSide; }
};

} // namespace vsgGeo
