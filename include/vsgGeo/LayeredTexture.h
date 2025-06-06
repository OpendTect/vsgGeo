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

#include <vsgGeo/Callback.h>
#include <vsgGeo/LayerProcess.h>
#include <vsgGeo/ThreadGroup.h>
#include <vsgGeo/Vec2i.h>

#include <vector>


namespace vsgGeo
{

enum FilterType		{ Nearest, Linear };
enum ImageDataOrder	{ STR, SRT, TRS, TSR, RST, RTS };


struct LayeredTextureData;
struct TilingInfo;
struct TextureInfo;

class CompositeTextureThread;
class PowerEncodingThread;


class VSGGEO_EXPORT LayeredTexture : public vsgGeo::CallbackObject
{
friend class CompositeTextureThread;

public:
			LayeredTexture();
			LayeredTexture(const LayeredTexture&,
			    const osg::CopyOp& op =osg::CopyOp::DEEP_COPY_ALL);

			META_Object( vsgGeo, LayeredTexture );

    void		turnOn(bool yn=true);
    bool		isOn() const			{ return _isOn; }

    int			addDataLayer(); //returns an id
			//Does its own locking
    void		removeDataLayer(int id);
			//Does its own locking

    void		readLock() { _lock.readLock(); }
			//Protects the layers, not the contents of them
    void		readUnLock() { _lock.readUnlock(); }

    int			nrDataLayers() const { return _dataLayers.size(); }
    int			getDataLayerID(int idx) const;
    int			getDataLayerIndex(int id) const;
    bool		isDataLayerOK(int id) const;

    bool		isEnvelopeDefined() const;
    osg::Vec2f		envelopeCenter() const;
    osg::Vec2f		textureEnvelopeSize() const;
    osg::Vec2f		imageEnvelopeSize() const;

    void		setDataLayerImage(int id,osg::Image*,
					  bool freezewhile0=true,
					  int nrPowerChannels=0);
			/*!Preferred image formats: GL_LUMINANCE, GL_ALPHA,
			   GL_LUMINANCE_ALPHA, GL_RGB, GL_RGBA, GL_BGR, GL_BGRA
			   Preferred data types: GL_UNSIGNED_BYTE,
						 GL_FLOAT (vertex offset only).
			   Option to freeze display updates as long as image=0
			   yields a smooth transition on screen. */

    const osg::Image*	getDataLayerImage(int id) const;
			/*!Returned with permuted dimensional sizes if
			   image data order is set other than default STR. */

    void		setDataLayerOrigin(int id,const osg::Vec2f&);
    const osg::Vec2f&	getDataLayerOrigin(int id) const;

    void		setDataLayerScale(int id,const osg::Vec2f&);
			/*!Beware that non-proportional scaling may result
			   in blurry textures when these are being mipmapped,
			   because OpenGL works with a maximum scale factor
			   over all dimensions instead of one for each.
			   Calling setAnisotropicPower(0) on modern hardware
			   solve this at the cost of speed and memory usage. */
    const osg::Vec2f&	getDataLayerScale(int id) const;

    void		setDataLayerFilterType(int id,FilterType);
    FilterType		getDataLayerFilterType(int id) const;

    void		setDataLayerBorderColor(int id,const osg::Vec4f&);
			/*!Any negative vector value will extend the image
			   edge pixel colors into the border */

    const osg::Vec4f&	getDataLayerBorderColor(int id) const;

    int			getDataLayerTextureUnit(int id) const;
    int			getTextureUnitLayerId(int unit) const;
    int			getTextureUnitNrDims(int unit) const;
    void		addAssignTexCrdLine(std::string& code,int unit) const;

    TransparencyType	getDataLayerTransparencyType(int id,
						     int channel=3) const;
    osg::Vec4f		getDataLayerTextureVec(int id,
					const osg::Vec2f& globalCoord) const;

    void		setDataLayerUndefLayerID(int id,int undef_id);
    int			getDataLayerUndefLayerID(int id) const;
    void		setDataLayerUndefChannel(int id,int channel);
    int			getDataLayerUndefChannel(int id) const;

    void		setDataLayerImageUndefColor(int id,const osg::Vec4f&);
			/*!Use negative vector value(s) if undef pixels
			   from the undef channel do not show a special
			   color value in the image channel(s) (default). */

    const osg::Vec4f&	getDataLayerImageUndefColor(int id) const;

    void		setDataLayerImageOrder(int id,ImageDataOrder);
    ImageDataOrder	getDataLayerImageOrder(int id) const;

    void		setDataLayerSliceNr(int id,int nr);
    int			getDataLayerSliceNr(int id) const;

    void		transformDataLayerCoord(osg::Vec2f& local,
						int fromId, int toId);

    void		setDataLayerVertex2TextureTransform(int id,
							const osg::Matrixf*);
    const osg::Matrixf*	getDataLayerVertex2TextureTransform(int id) const;

    struct VertexOffsetCutoutInfo
    {
			VertexOffsetCutoutInfo()
			    : _lod( 0.0 )
			    , _offsetIfUndef( 0.0 )
			{}

			float _lod;
			float _offsetIfUndef;
    };

    bool		canDoVertexOffsetNow(bool usingFloat) const;
    void		useNormalizedTexCoords(bool);
			/* Enables repeated use of one single tile geometry at
			   the cost of a few extra computations in the shader */

    void		setVertexOffsetLayerID(int id);
    int			getVertexOffsetLayerID() const;
    void		setVertexOffsetChannel(int channel);
    int			getVertexOffsetChannel() const;
    void		setVertexOffsetFactor(float);
    float		getVertexOffsetFactor() const;
    void		setVertexOffsetBias(float);
    float		getVertexOffsetBias() const;
    void		setVertexOffsetTexelSpanVectors(const osg::Vec3f& v0,
							const osg::Vec3f& v1);
    const osg::Vec3f&	getVertexOffsetTexelSpanVector(int dim) const;

			/* Vertex offset convenience functions. Doing it
			   yourself with no checks will maximize speed. */
    void		setVertexOffsetValue(float val,float undefVal,
					     int s,int t,int r=0);
    float		getVertexOffsetValue(float undefVal,
					     int s,int t,int r=0) const;
    template<class T>
    void		setVertexOffsetValues(T* start,T* stop,float undefVal,
					      int s,int t,int r=0);

    void		setStackUndefLayerID(int id);
    int			getStackUndefLayerID() const;
    void		setStackUndefChannel(int channel);
    int			getStackUndefChannel() const;
    void		setStackUndefColor(const osg::Vec4f&);
    const osg::Vec4f&	getStackUndefColor() const;

    void		invertUndefLayers(bool yn=true);
    bool		areUndefLayersInverted() const;

    int			nrProcesses() const	   { return _processes.size(); }
    LayerProcess*	getProcess(int idx);
    const LayerProcess*	getProcess(int idx) const;

    void		addProcess(LayerProcess*);
			//Does its own locking
    void		removeProcess(const LayerProcess*);
			//Does its own locking
    void		moveProcessEarlier(const LayerProcess*);
			//Does its own locking
    void		moveProcessLater(const LayerProcess*);
			//Does its own locking

    struct		TextureCoordData
    {
			TextureCoordData(int tu,
				const osg::Vec2f& tc00, const osg::Vec2f& tc01,
				const osg::Vec2f& tc10, const osg::Vec2f& tc11,
				const Vec2i& origin, const Vec2i& size)
			    : _textureUnit( tu )
			    , _tc00( tc00 ), _tc01( tc01 )
			    , _tc10( tc10 ), _tc11( tc11 )
			    , _cutoutOrigin( origin ), _cutoutSize( size )
			{}

	int		_textureUnit;
	osg::Vec2f	_tc00, _tc01, _tc10, _tc11;
	Vec2i		_cutoutOrigin ,_cutoutSize;
    };

    bool		needsRetiling() const;
    void		reInitTiling(float externalTexelSizeRatio=1.0f);
			/*!Ratio dx/dy denotes external texel distortion.
			   Used by seam management to minimize tile artifacts
			   caused by mipmapping with non-square texels. */

    bool /* as_asked */	planTiling(int brickSize,
				   std::vector<float>& xTickMarks,
				   std::vector<float>& yTickMarks,
				   bool strict=true) const;
			/*!If not strict, the brick size of the returned tiling
			   might be chosen different to optimize performance. */

    osg::Vec2f		tilingPlanResolution() const;
			//!Reciprocal of the scale of highest-resolution layer

    osg::StateSet*	createCutoutStateSet(const osg::Vec2f& origin,
			    const osg::Vec2f& opposite,
			    std::vector<TextureCoordData>&,
			    const VertexOffsetCutoutInfo* info=0) const;
			/*!If needsRetiling() is true, call reInitTiling(.)
			   first, followed by the optional planTiling(.),
			   and next (re)create your CutoutStateSets. */

    osg::StateSet*	getSetupStateSet();
    void		updateSetupStateSet();

    void		setAnisotropicPower(int power);
			/*!Default power=-1 will disable anisotropic filtering.
			   If supported by hardware, power=0 already corrects
			   mipmap filtering at perpendicular viewing angles of
			   inherently non-square texels. Each extra power step
			   also corrects for square texels that just display
			   non-square at small viewing angles, up to a ratio
			   2^(power) between the displayed texel dimensions. */
    float		getAnisotropicPower() const;

    void		setSeamPower(int power,int dim=-1);
			/*!Default power 0 adds a one-texel seam to both tile
			   side. Each extra power step doubles its width. At
			   the cost of speed and memory usage, a larger seam
			   minimizes tile artifacts that may occur when zooming
			   out, e.g. at near-parallel viewing angles. Both
			   the two dimensions will be set if none specified. */
    int			getSeamPower(int dim=0) const;

    void		allowShaders(bool yn=true,bool maySkipEarlyProcs=false);
			/*!Fallback to fixed function pipeline if not enough
			   texture units available, unless option is set to
			   just skip the earliest processes. */

    bool		areShadersAllowed() const   { return _allowShaders; }
    bool		areShadersUsedNow() const   { return _useShaders; }

    void		enableMipmapping(bool yn);
    bool		isMipmappingEnabled() const;

    enum TextureSizePolicy { PowerOf2, AnySize };

    void		setTextureSizePolicy(TextureSizePolicy);
    TextureSizePolicy	getTextureSizePolicy() const;

    void		setMaxTextureCopySize(unsigned int width_x_height);
			/*!The size of a non-power-of-2 texture layer will
			   upper bound the optimal tile size at some texture
			   size policies. Therefore, a power-of-2 scaled copy
			   is created for small non-power-of-2 textures. */

    void		setGraphicsContextID(int id=-1);
			/*!Without a valid ID, the texture hardware info is
			   automatically derived from all known contexts. */
    int			getGraphicsContextID() const;

    static void		overrideGraphicsContextMaxTextureSize(int);
			/*!To be set before creating class instances.
			   Use if graphics card exaggerates its specs. */

    int			maxTextureSize() const;
    int			nrTextureUnits() const;

    const osg::Image*	getCompositeTextureImage();
    int			compositeLayerId()	{ return _compositeLayerId; }

    void		setCompositeSubsampleSteps(int);
    int			getCompositeSubsampleSteps() const;

    static int		image2TextureChannel(int channel,GLenum format);
    static int		powerOf2Ceil(unsigned int nr);	// nr<=2^30 supported

protected:
			~LayeredTexture();
    void		assignTextureUnits();
    void		checkForModifiedImages();
    void		buildShaders();
    void		createColSeqTexture();
    void		updateSetupStateSetIfNeeded();
    void		updateTextureInfoIfNeeded() const;
    void		updateTilingInfoIfNeeded() const;
    int			getSeamWidth(int layerIdx,int dim) const;
    float		getMaxAnisotropy(int layerIdx) const;
    int			getTileOverlapUpperBound(int dim) const;
    TextureSizePolicy	usedTextureSizePolicy() const;
    bool		isDisplayFrozen() const;

    void		add3DTextureToStateSet(const LayeredTextureData&,
				std::vector<LayeredTexture::TextureCoordData>&,
				osg::StateSet&) const;

    int /* nrProc */	getProcessInfo(std::vector<int>& orderedLayerIDs,
				       int& nrUsedLayers,bool& useShaders,
				       bool* stackIsOpaque=0) const;

    void		setDataLayerTextureUnit(int id,int unit);
    void		raiseUndefChannelRefCount(bool yn, int idx=-1);

    bool		divideAxis(float totalSize,int brickSize,
				   std::vector<float>& tickMarks) const;

    void		getVertexShaderCode(std::string& code,
				const std::vector<int>& activeUnits) const;
    void		getFragmentShaderCode(std::string& code,
				const std::vector<int>& activeUnits,
				int nrProc,bool stackIsOpaque) const;

    void		createCompositeTexture(bool dummyTexture=false,
					       bool triggerProgress=false);
    void		setRenderingHint(bool stackIsOpaque);

    void		setUpdateVar(bool& var,bool yn);
			//! Will trigger redraw request if necessary

    bool		_updateSetupStateSet;	// Only set via setUpdateVar(.)
    bool		_retileCompositeLayer;	// Only set via setUpdateVar(.)

			/* LayeredTexture is also responsible for requesting
			   redraw, either directly or via setUpdateVar(.), if
			   one of these auxiliary class variables is updated:

			   TilingInfo::_needsUpdate
			   TilingInfo::_retilingNeeded
			   TextureInfo::_isValid
			   LayeredTextureData::_freezeDisplay
			   LayeredTextureData::_dirtyTileImages
			*/

    int			encodeBaseChannelPower(osg::Image& image,
					       int nrPowerChannels);

    bool				_isOn;
    OpenThreads::ReadWriteMutex		_lock;
    std::vector<LayeredTextureData*>	_dataLayers;
    std::vector<LayerProcess*>		_processes;

    std::vector<int>			_id2idxTable;
    std::vector<int>			_releasedIds;

    osg::ref_ptr<osg::StateSet>		_setupStateSet;

    TextureSizePolicy			_textureSizePolicy;
    unsigned int			_maxTextureCopySize;
    float				_anisotropicPower;
    Vec2i				_seamPower;
    float				_externalTexelSizeRatio;

    mutable TilingInfo*			_tilingInfo;
    mutable TextureInfo*		_texInfo;

    bool				_useNormalizedTexCoords;
    int					_vertexOffsetLayerId;
    int					_vertexOffsetChannel;
    float				_vertexOffsetFactor;
    float				_vertexOffsetBias;
    osg::Vec3f				_vertexOffsetSpanVec0;
    osg::Vec3f				_vertexOffsetSpanVec1;

    int					_stackUndefLayerId;
    int					_stackUndefChannel;
    osg::Vec4f				_stackUndefColor;

    bool				_invertUndefLayers;

    bool				_allowShaders;
    bool				_useShaders;
    bool				_maySkipEarlyProcesses;
    bool				_enableMipmapping;

    int					_compositeLayerId;
    int					_compositeSubsampleSteps;
    bool				_compositeLayerUpdate;
    bool				_reInitTiling;

    osg::ref_ptr<ThreadGroup<CompositeTextureThread> > _compositeThreads;
    osg::ref_ptr<ThreadGroup<PowerEncodingThread> > _powerEncodingThreads;
};


} //namespace

