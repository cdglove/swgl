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

#include "swgl/geometry/matrix.hpp"
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

class pipeline_counters {
 public:
  pipeline_counters() = default;

  void increment_pixel_count() {
    SWGL_PIPELINE_STAT(++num_pixels_);
  }

  void increment_triangle_count() {
    SWGL_PIPELINE_STAT(++num_triangles_);
  }

  void increment_draw_count() {
    SWGL_PIPELINE_STAT(++num_draws_);
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

  pipeline_counters& operator+=(pipeline_counters const& other) {
#if SWGL_ENABLE_PIPELINE_STATS
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

class pipeline {
 public:
  pipeline_counters draw() const {
    return draw_impl();
  }

  void set_model(model const& model) {
    model_ = &model;
  }

  void set_depth(std::vector<float>& depth) {
    depth_ = &depth;
  }

  void set_texture(std::size_t idx, image const& texture) {
    textures_.resize(std::max(idx + 1, textures_.size()));
    textures_[idx] = &texture;
  }

  void set_render_target(image& rt) {
    rt_ = &rt;
  }

  matrix4f projection_;
  matrix4f view_;
  
 protected:
  template <typename T>
  using face_t = std::array<T, 3>;

  struct raster_info {
    int width;
    int height;
  };

  void draw_triangle(
      raster_info const& ri,
      face_t<vector3f> const& tri,
      face_t<vector2f> uvs,
      colour<float> light,
      pipeline_counters& stats) const;

 private:
  model const* model_;
  std::vector<float>* depth_;
  std::vector<image const*> textures_;
  image* rt_;

  virtual pipeline_counters draw_impl() const;
};

} // namespace swgl

#endif // SWGL_PIPELINE_HPP
