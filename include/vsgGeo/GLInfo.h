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

#include <string>
#include <vector>

namespace vsgGeo
{

class VSGGEO_EXPORT GLInfo : public osg::Referenced
{
public:

    static const osg::ref_ptr<GLInfo>	get();

    const char*			glVendor() const;
    const char*			glRenderer() const;
    const char*			glVersion() const;
    const char*			glExtensions() const;
    bool			isPlatformSupported() const;
    bool			isVertexProgramSupported() const;
    bool			isShaderProgramSupported() const;
    bool			isGeometryShader4Supported() const;

    int				getLimit(int glenum) const;
    void			getLineWidthRange(float& min,float& max) const;

    bool			isOK() const;

protected:
    static void			initGL();
				GLInfo();
    void			updateLimits();
    std::string			_glvendor;
    std::string			_glrenderer;
    std::string			_glversion;
    std::string			_glextensions;

    struct Limit
    {
				Limit( int tok, const char* nm, int val )
				    : _token( tok ), _name( nm ), _value( val )
				{}

	int			_token;
	const char*		_name;
	int			_value;
    };

    bool			getExtension(const char* extntnnm) const;

    std::vector<Limit>		_limits;

    std::string			_errmsg;
};

} // namespace vsgGeo


