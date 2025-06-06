#pragma once

/* -*-c++-*- VulkanSceneGraph - Copyright (C) 1998-2006 Robert Osfield
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the VulkanSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * VulkanSceneGraph Public License for more details.
 *
*/


namespace vsgGeo {

class Vec2i
{
    public:

        /** Data type of vector components.*/
        typedef int value_type;

        /** Number of vector components. */
        enum { num_components = 2 };

        value_type _v[2];

        /** Constructor that sets all components of the vector to zero */
        Vec2i() { _v[0]=0; _v[1]=0; }

        Vec2i(value_type xval, value_type yval) { _v[0] = xval; _v[1] = yval; }

        inline bool operator == (const Vec2i& v) const { return _v[0]==v._v[0] && _v[1]==v._v[1]; }
        inline bool operator != (const Vec2i& v) const { return _v[0]!=v._v[0] || _v[1]!=v._v[1]; }
        inline bool operator <  (const Vec2i& v) const
        {
            if (_v[0]<v._v[0]) return true;
            else if (_v[0]>v._v[0]) return false;
            else return (_v[1]<v._v[1]);
        }

        inline value_type* ptr() { return _v; }
        inline const value_type* ptr() const { return _v; }

        inline void set( value_type xval, value_type yval )
        {
            _v[0]=xval; _v[1]=yval;
        }

        inline void set( const Vec2i& rhs)
        {
            _v[0]=rhs._v[0]; _v[1]=rhs._v[1];
        }

        inline value_type& operator [] (int i) { return _v[i]; }
        inline value_type operator [] (int i) const { return _v[i]; }

        inline value_type& x() { return _v[0]; }
        inline value_type& y() { return _v[1]; }

        inline value_type x() const { return _v[0]; }
        inline value_type y() const { return _v[1]; }

        inline value_type& r() { return _v[0]; }
        inline value_type& g() { return _v[1]; }

        inline value_type r() const { return _v[0]; }
        inline value_type g() const { return _v[1]; }

        inline Vec2i operator * (value_type rhs) const
        {
            return Vec2i(_v[0]*rhs, _v[1]*rhs);
        }

        inline Vec2i& operator *= (value_type rhs)
        {
            _v[0]*=rhs;
            _v[1]*=rhs;
            return *this;
        }

        inline Vec2i operator / (value_type rhs) const
        {
            return Vec2i(_v[0]/rhs, _v[1]/rhs);
        }

        inline Vec2i& operator /= (value_type rhs)
        {
            _v[0]/=rhs;
            _v[1]/=rhs;
            return *this;
        }

        inline Vec2i operator * (const Vec2i& rhs) const
        {
            return Vec2i(_v[0]*rhs._v[0], _v[1]*rhs._v[1]);
        }

        inline Vec2i operator + (const Vec2i& rhs) const
        {
            return Vec2i(_v[0]+rhs._v[0], _v[1]+rhs._v[1]);
        }

        inline Vec2i& operator += (const Vec2i& rhs)
        {
            _v[0] += rhs._v[0];
            _v[1] += rhs._v[1];
            return *this;
        }

        inline Vec2i operator - (const Vec2i& rhs) const
        {
            return Vec2i(_v[0]-rhs._v[0], _v[1]-rhs._v[1]);
        }

        inline Vec2i& operator -= (const Vec2i& rhs)
        {
            _v[0]-=rhs._v[0];
            _v[1]-=rhs._v[1];
            return *this;
        }


        inline Vec2i operator - () const
        {
            return Vec2i (-_v[0], -_v[1]);
        }


};    // end of class Vec2i


/** multiply by vector components. */
inline Vec2i componentMultiply(const Vec2i& lhs, const Vec2i& rhs)
{
    return Vec2i(lhs[0]*rhs[0], lhs[1]*rhs[1]);
}

/** divide rhs components by rhs vector components. */
inline Vec2i componentDivide(const Vec2i& lhs, const Vec2i& rhs)
{
    return Vec2i(lhs[0]/rhs[0], lhs[1]/rhs[1]);
}

}    // end of namespace vsgGeo

