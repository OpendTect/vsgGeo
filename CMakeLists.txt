# vsgGeo - A collection of geoscientific extensions to VulkanSceneGraph.
# Copyright 2025 dGB Beheer B.V.
# 
# vsgGeo is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>
#
# No configuration settings should be modified below.

cmake_minimum_required ( VERSION 3.27.0 )

cmake_policy( SET CMP0048 NEW )
cmake_policy( SET CMP0091 NEW )
#Dummy version, will be retrieved from find_package(vsg)
project( vsgGeo VERSION 0.0.0 )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED True )

cmake_policy( SET CMP0011 NEW )
cmake_policy( SET CMP0057 NEW )
cmake_policy( SET CMP0074 NEW )
cmake_policy( SET CMP0144 NEW )

option( BUILD_SHARED_LIBS "Build shared libraries" ON )
find_package( vsg REQUIRED GLOBAL )
include( "${vsg_DIR}/vsgConfigVersion.cmake" )
project( vsgGeo
	VERSION ${PACKAGE_VERSION}
	DESCRIPTION "vsgGeo" )

# This does include(GNUInstallDirs), which sets up all the correct install directories.
vsg_setup_dir_vars()

add_subdirectory(src)
