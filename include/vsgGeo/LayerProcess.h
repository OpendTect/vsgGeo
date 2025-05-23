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

#include <string>


namespace vsgGeo
{

enum TransparencyType { TransparencyUnknown, Opaque, HasTransparencies, OnlyFullTransparencies, FullyTransparent };

//! A few auxiliary functions for enum TransparencyType derivations:

extern VSGGEO_EXPORT TransparencyType getTransparencyTypeBytewise(const unsigned char* start,const unsigned char* stop,int step);

extern VSGGEO_EXPORT TransparencyType addOpacity(TransparencyType,float opacity);

extern VSGGEO_EXPORT TransparencyType multiplyOpacity(TransparencyType,float opacity);


//============================================================================


class VSGGEO_EXPORT ColorSequence : public vsgGeo::CallbackObject
{
public:
				ColorSequence(unsigned char* array=0);
    virtual			~ColorSequence();

    void			setRGBAValues(unsigned char* array);
    unsigned char*		getRGBAValues()		 { return _arr; }
    const unsigned char*	getRGBAValues() const	 { return _arr; }
    void			touch();
    unsigned int		getModifiedCount() const { return _dirtyCount; }
    TransparencyType		getTransparencyType() const;

protected:
    unsigned char*		_arr;		//! 256*4 bytes
    int				_dirtyCount;
    mutable TransparencyType	_transparencyType;
};


//============================================================================


class LayeredTexture;


class VSGGEO_EXPORT LayerProcess : public osg::Referenced
{
public:
				LayerProcess(LayeredTexture&);
    virtual void		getShaderCode(std::string& code,
					      int stage) const		= 0;
    virtual int			getDataLayerID(int idx=0) const		= 0;
    virtual TransparencyType	getTransparencyType(
					bool imageOnly=false) const	= 0;
    virtual void		doProcess(osg::Vec4f& fragColor,float stackUdf,
					  const osg::Vec2f& globalCoord)= 0;

    virtual bool		isOn(int=0) const	      { return true; }

    virtual bool		needsColorSequence() const    { return false; }

    virtual void		checkForModifiedColorSequence()		{};
    const unsigned char*	getColorSequencePtr() const;
    virtual int			getColorSequenceUndefIdx() const { return -1; }
    void			setColorSequenceTextureSampling(float start,
								float step);
    void			setOpacity(float opacity);
    float			getOpacity() const;

    void			setNewUndefColor(const osg::Vec4f&);
				/*! Can only overrule undef color from
				    source layer(s) if at least one layer
				    has a reference to an undef channel. */

    const osg::Vec4f&		getNewUndefColor() const;
    virtual bool		isUndefPerChannel() const     { return false; }

protected:
    LayeredTexture&		_layTex;
    const unsigned char*	_colSeqPtr;
    float			_colSeqTexSamplingStart;
    float			_colSeqTexSamplingStep;
    osg::Vec4f			_newUndefColor;
    float			_opacity;

    void			getHeaderCode(std::string& code,
					      int& nrUdf,int id,
					      int toIdx=-1,int fromIdx=0) const;
    void			getFooterCode(std::string& code,
					      int& nrUdf,int stage) const;

    void			processHeader(osg::Vec4f& col,float& udf,
					float stackUdf,const osg::Vec2f& coord,
					int id,int toIdx=-1,int fromIdx=0,
					float* orgCol3=0) const;
    void			processFooter(osg::Vec4f& fragColor,
					      osg::Vec4f col,float udf) const;

    void			assignOrgCol3IfNeeded(std::string& code,
						      int toIdx=-1) const;
};


class VSGGEO_EXPORT ColTabLayerProcess : public LayerProcess
{
    class ColSeqCallbackHandler;

public:
				ColTabLayerProcess(LayeredTexture&);
				~ColTabLayerProcess();

    void			setDataLayerID(int idx, int id, int channel=0);
				/*! idx=0: color index
				    idx=1: low-precision squared color index,
				    idx=2: 255-times more precise addition
					   to preceding squared color index. */

				/*! Optional squared color index channels will
				    improve mipmapped coltab mapping quality
				    of the shader (c[0]^2 = c[1]+c[2]/255). */

    int				getDataLayerID(int idx=0) const override;
    int				getDataLayerTextureChannel(int idx) const;

    void			checkForModifiedColorSequence() override;
    void			setColorSequence(ColorSequence*);
    const ColorSequence*	getColorSequence() const;
    int				getColorSequenceUndefIdx() const override;

    bool			needsColorSequence() const override
				{ return true; }

    void			getShaderCode(std::string& code,
					      int stage) const override;

    TransparencyType		getTransparencyType(bool imageOnly=false)
								const override;
    void			doProcess(osg::Vec4f& fragColor,float stackUdf,
					  const osg::Vec2f& globalCoord) override;
protected:
    int				_id[3];
    int				_textureChannel[3];
    ColorSequence*		_colorSequence;
    int				_colSeqModifiedCount;

    ColSeqCallbackHandler*	_colSeqCallbackHandler;
};


class VSGGEO_EXPORT RGBALayerProcess : public LayerProcess
{
public:
				RGBALayerProcess(LayeredTexture&);

    void			setDataLayerID(int idx,int id,int channel=0);
				/*! idx=0: red
				    idx=1: green
				    idx=2: blue
				    idx=3: alpha */
    int				getDataLayerID(int idx) const override;
    int				getDataLayerTextureChannel(int idx) const;

    void			turnOn(int idx,bool yn=true);
    bool			isOn(int idx=0) const override;

    void			applyUndefPerChannel(bool yn=true);
    bool			isUndefPerChannel() const override;


    void			getShaderCode(std::string& code,
					      int stage) const override;

    TransparencyType		getTransparencyType(bool imageOnly=false)
								const override;
    void			doProcess(osg::Vec4f& fragColor,float stackUdf,
				      const osg::Vec2f& globalCoord) override;
protected:
    int			_id[4];
    int				_textureChannel[4];
    bool			_isOn[4];
    bool			_udfPerChannel;
};


class VSGGEO_EXPORT IdentityLayerProcess : public LayerProcess
{
public:
				IdentityLayerProcess(LayeredTexture&,int id);

    int				getDataLayerID(int idx=0) const override;

    void			getShaderCode(std::string& code,
					      int stage) const override;

    TransparencyType		getTransparencyType(bool imageOnly=false)
								const override;
    void			doProcess(osg::Vec4f& fragColor,float stackUdf,
				      const osg::Vec2f& globalCoord) override;
protected:
    int				_id;
};


} //namespace

