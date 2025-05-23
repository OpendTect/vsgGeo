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


#include <vector>
#include <osg/Vec3>

namespace vsgGeo
{

struct ColorPoint
{
  ColorPoint(float pos_, const osg::Vec3 &color_) :
      pos(pos_), color(color_) {}

  ColorPoint() {}

  // for sorting
  bool operator<(const ColorPoint &other) const
  {
    return this->pos < other.pos;
  }

  float pos;
  osg::Vec3 color;
};

typedef std::vector<ColorPoint> ColorPointList;

class Palette
{
public:
  Palette(const ColorPointList &colorPoints);
  Palette();

  osg::Vec3 get(float value, float min, float max) const;

  const ColorPointList &colorPoints() const { return _colorPoints; }
  void setColorPoints(const ColorPointList &cps);

private:
  ColorPointList _colorPoints;
};

} // namespace vsgGeo
