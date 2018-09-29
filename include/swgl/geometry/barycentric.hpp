//
// swgl/geometry/barycentric.hpp
//
// Copyright (c) Chris Glover, 2018
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SWGL_GEOMETRY_BARYCENTRIC_HPP
#define SWGL_GEOMETRY_BARYCENTRIC_HPP
#pragma once

#include "swgl/geometry/vector.hpp"

namespace swgl {

class barycentric_basis {
 public:
  barycentric_basis(swgl::vector3f t0, swgl::vector3f t1, swgl::vector3f t2)
      : root_(t0)
      , basis_{swgl::vector3f(t2[0] - t0[0], t1[0] - t0[0], 0.f),
               swgl::vector3f(t2[1] - t0[1], t1[1] - t0[1], 0.f)} {
  }

  swgl::vector3f compute(swgl::vector2i P) const {
    basis_[0].z = root_.x - P.x;
    basis_[1].z = root_.y - P.y;

    swgl::vector3f u = cross(basis_[0], basis_[1]);

    if(std::abs(u[2]) < 1) {
      // triangle is degenerate.
      return {-1, 1, 1};
    }

    float zrecip = 1.f / u.z;
    return {1.f - (u.x + u.y) * zrecip, u.y * zrecip, u.x * zrecip};
  }

 private:
  swgl::vector3f root_;
  mutable std::array<swgl::vector3f, 2> basis_;
};


} // namespace swgl

#endif // SWGL_GEOMETRY_BBOX_HPP