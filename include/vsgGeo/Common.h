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

#include <vsg/all.h>
#include <vsgGeo/Export.h>

#ifndef M_PI
#define M_PI 3.14159265
#endif

#if defined(_MSC_VER)
# define DISABLE_WARNINGS \
__pragma(warning(push)) \
__pragma(warning( disable : 4100 ))
# define ENABLE_WARNINGS __pragma(warning(pop))
#else
# define DISABLE_WARNINGS
# define ENABLE_WARNINGS
#endif

#define REGISTER_EMPTY_OBJECT_WRAPPER(NAME, PROTO, CLASS, ASSOCIATES) \
REGISTER_OBJECT_WRAPPER(NAME, PROTO, CLASS, ASSOCIATES) \
{ if ( wrapper ) return; }

