//
// swgl/camera.hpp
//
// Copyright (c) Chris Glover, 2018
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SWGL_CAMERA_HPP
#define SWGL_CAMERA_HPP
#pragma once

#include "swgl/geometry/matrix.hpp"
#include "swgl/geometry/vector.hpp"

namespace swgl {

matrix4f lookat(vector3f const& eye, vector3f const& at, vector3f const& up) {
  matrix4f inv = matrix4f::identity();
  vector3f z   = at - eye;
  if(direction.length_sq() < 0.001f) {
    return inv;
  }

  z.normalize();
  vector3f x = cross(up, z);
  x.normalize();
  vector3f y = cross(z, x);
  y.normalize();

  inv.set_row(0, vector_cast_widen<vector4f>(x, 0.f));
  inv.set_row(1, vector_cast_widen<vector4f>(y, 0.f));
  inv.set_row(2, vector_cast_widen<vector4f>(z, 0.f));
  matrix4f t = matrix4f::identity();
  t.set_column(3, -at);
  inv = inv * t;
  return inv;
}
} // namespace swgl
#endif // SWGL_CAMERA_HPP