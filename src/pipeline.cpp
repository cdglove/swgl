//
// src/pipeline.cpp
//
// Copyright (c) Chris Glover, 2018
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include "swgl/pipeline.hpp"
#include "swgl/colour.hpp"
#include "swgl/geometry/bbox.hpp"
#include "swgl/geometry/matrix.hpp"
#include "swgl/geometry/vector.hpp"
#include "swgl/image.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

static void line(
    swgl::vector2i p0,
    swgl::vector2i p1,
    swgl::image& rt,
    swgl::image::colour_type colour) {
  bool steep = false;
  if(std::abs(p0.x - p1.x) < std::abs(p0.y - p1.y)) {
    std::swap(p0.x, p0.y);
    std::swap(p1.x, p1.y);
    steep = true;
  }
  if(p0.x > p1.x) {
    std::swap(p0, p1);
  }

  for(int x = p0.x; x <= p1.x; x++) {
    float t = (x - p0.x) / static_cast<float>(p1.x - p0.x);
    int y   = static_cast<int>(p0.y * (1.f - t) + p1.y * t);
    if(steep) {
      rt.set(y, x, colour);
    }
    else {
      rt.set(x, y, colour);
    }
  }
}
