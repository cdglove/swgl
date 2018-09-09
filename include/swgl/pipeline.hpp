//
// swgl/pipeline.hpp
//
// Copyright (c) Chris Glover, 2018
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SWGL_PIPELINE_HPP
#define SWGL_PIPELINE_HPP
#pragma once

#include "swgl/image.hpp"
#include "swgl/model.hpp"

#ifndef SWGL_ENABLE_PIPELINE_STATS
#  define SWGL_ENABLE_PIPELINE_STATS 1
#endif

#if SWGL_ENABLE_PIPELINE_STATS
#  define SWGL_PIPELINE_STAT(x)                                                \
    do {                                                                       \
      x;                                                                       \
    } while(false)
#else
#  define SWGL_PIPELINE_STAT(x)                                                \
    do {                                                                       \
    } while(false)
#endif

namespace swgl {

class image;
class model;

class pipeline_stats {
 public:
  pipeline_stats()
      : num_pixels_(0)
      , num_triangles_(0)
      , num_draws_(0) {
  }

  void increment_pixel_count() {
    SWGL_PIPELINE_STAT(++num_pixels_);
  }

  void increment_triangle_count() {
    SWGL_PIPELINE_STAT(++num_triangles_);
  }

  void increment_draw_count() {
    SWGL_PIPELINE_STAT(++num_draws_);
  }

  void record_depth(float depth) {
    SWGL_PIPELINE_STAT((max_depth_ = std::max(max_depth_, depth)));
    SWGL_PIPELINE_STAT((min_depth_ = std::max(min_depth_, depth)));
  }

  int pixel_count() const {
    return SWGL_ENABLE_PIPELINE_STATS ? num_pixels_ : 0;
  }

  int triangle_count() const {
    return SWGL_ENABLE_PIPELINE_STATS ? num_triangles_ : 0;
  }

  int draw_count() const {
    return SWGL_ENABLE_PIPELINE_STATS ? num_draws_ : 0;
  }

  float max_depth() const {
    return max_depth_;
  }

  float min_depth() const {
    return min_depth_;
  }

  pipeline_stats& operator+=(pipeline_stats const& other) {
#if SWGL_ENABLE_PIPELINE_STATS
    num_pixels_ += other.num_pixels_;
    num_triangles_ += other.num_triangles_;
    num_draws_ += other.num_draws_;
#endif
    return *this;
  }

 private:
  int num_pixels_;
  int num_triangles_;
  int num_draws_;
  float max_depth_;
  float min_depth_;
};

class pipeline {
 public:
  pipeline_stats draw(
      model const& m, image& rt, std::vector<float>& depth) const {
    return draw_impl(m, rt, depth);
  }

 private:
  virtual pipeline_stats draw_impl(
      model const& m, image& rt, std::vector<float>& depth) const;
};

} // namespace swgl

#endif // SWGL_PIPELINE_HPP