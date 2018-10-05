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

inline matrix4f lookat(
    vector3f const& eye, vector3f const& at, vector3f const& up) {
  matrix4f inv(init::identity);
  vector3f z   = eye - at;
  if(z.length_sq() < 0.001f) {
    return inv;
  }

  z.normalize();
  vector3f x = cross(up, z);
  x.normalize();
  vector3f y = cross(z, x);
  y.normalize();

  inv.set_row(0, vector_widen<4>(x, 0.f));
  inv.set_row(1, vector_widen<4>(y, 0.f));
  inv.set_row(2, vector_widen<4>(z, 0.f));
  matrix4f t = matrix4f::identity();
  t.set_column(3, swgl::vector_widen<4>(-at, 1.f));
  auto look = inv * t;
  return look;
}

inline swgl::matrix4f viewport_matrix(int x, int y, int w, int h) {
  static const int depth = 255;
  swgl::matrix4f m       = swgl::matrix4f::identity();

  m[0][3] = x + w / 2.f;
  m[1][3] = y + h / 2.f;
  m[2][3] = depth / 2.f;
  m[0][0] = w / 2.f;
  m[1][1] = h / 2.f;
  m[2][2] = depth / 2.f;
  return m;
}

} // namespace swgl
#endif // SWGL_CAMERA_HPP
