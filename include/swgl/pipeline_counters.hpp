//
// swgl/pipeline.hpp
//
// Copyright (c) Chris Glover, 2018
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SWGL_PIPELINECOUNTERS_HPP
#define SWGL_PIPELINECOUNTERS_HPP
#pragma once

#include "swgl/geometry/barycentric.hpp"
#include "swgl/geometry/bbox.hpp"
#include "swgl/geometry/matrix.hpp"
#include "swgl/image.hpp"
#include "swgl/model.hpp"
#include "swgl/pipeline_counters.hpp"

#include <algorithm>
#include <array>

#ifndef SWGL_ENABLE_PIPELINE_COUNTERS
#  define SWGL_ENABLE_PIPELINE_COUNTERS 1
#endif

#if SWGL_ENABLE_PIPELINE_COUNTERS
#  define SWGL_PIPELINE_COUNTER(x)                                             \
    do {                                                                       \
      x;                                                                       \
    } while(false)
#else
#  define SWGL_PIPELINE_COUNTER(x)                                             \
    do {                                                                       \
    } while(false)
#endif

namespace swgl {

class pipeline_counters {
 public:
  pipeline_counters() = default;

  void increment_pixel_count() {
    SWGL_PIPELINE_COUNTER(++num_pixels_);
  }

  void increment_triangle_count() {
    SWGL_PIPELINE_COUNTER(++num_triangles_);
  }

  void increment_draw_count() {
    SWGL_PIPELINE_COUNTER(++num_draws_);
  }

  int pixel_count() const {
    return SWGL_ENABLE_PIPELINE_COUNTERS ? num_pixels_ : 0;
  }

  int triangle_count() const {
    return SWGL_ENABLE_PIPELINE_COUNTERS ? num_triangles_ : 0;
  }

  int draw_count() const {
    return SWGL_ENABLE_PIPELINE_COUNTERS ? num_draws_ : 0;
  }

  pipeline_counters& operator+=(pipeline_counters const& other) {
#if SWGL_ENABLE_PIPELINE_COUNTERS
    num_pixels_ += other.num_pixels_;
    num_triangles_ += other.num_triangles_;
    num_draws_ += other.num_draws_;
#endif
    return *this;
  }

 private:
  int num_pixels_    = 0;
  int num_triangles_ = 0;
  int num_draws_     = 0;
};

} // namespace swgl

#endif // SWGL_PIPELINE_HPP
