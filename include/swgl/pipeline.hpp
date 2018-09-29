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

#include "swgl/geometry/barycentric.hpp"
#include "swgl/geometry/bbox.hpp"
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

class pipeline_base {
 public:
  pipeline_counters draw() const {
    return draw_impl();
  }

 private:
  virtual pipeline_counters draw_impl() const = 0;
};

template <typename Derived>
class pipeline : public pipeline_base {
 public:
  void set_render_target(image& rt) {
    rt_ = &rt;
  }

  void set_model(model const& m) {
    model_ = &m;
  }

  void set_depth(std::vector<float>& depth) {
    depth_ = &depth;
  }

  model const& get_model() const {
    return *model_;
  }

 private:
  template <typename T>
  using face_t = std::array<T, 3>;

  struct raster_info {
    int width;
    int height;
  };

  pipeline_counters draw_impl() const override {
    pipeline_counters stats;
    stats.increment_draw_count();
    raster_info ri;
    ri.width           = rt_->width();
    ri.height          = rt_->height();
    model const& model = *model_;
    for(int face = 0; face < model.nfaces(); ++face) {
      face_t<decltype(derived().shade_vertex(face, 0))> vertex_out;
      for(int j = 0; j < 3; j++) {
        vertex_out[j] = derived().shade_vertex(face, j);
      }

      // Cull backfaces
      auto normal = cross(
          (vertex_out[2].screen_coords - vertex_out[0].screen_coords),
          (vertex_out[1].screen_coords - vertex_out[0].screen_coords));
      normal.normalize();
      if(dot(normal, vector3f(0.f, 0.f, -1.f)) < 0.f) {
        continue;
      }

      draw_triangle(ri, vertex_out, stats);
    }
    return stats;
  }

  template <typename VertexOutput>
  void draw_triangle(
      raster_info const& ri,
      VertexOutput const& tri,
      pipeline_counters& stats) const {
    stats.increment_triangle_count();
    auto& depth = *depth_;
    swgl::bbox<float, 3> box(tri[0].screen_coords);
    box.expand(tri[1].screen_coords);
    box.expand(tri[2].screen_coords);
    box.clamp({0.f, 0.f, 0.f}, vector3f(ri.width - 1.f, ri.height - 1.f, 0.f));
    auto bboxmin = box.min();
    auto bboxmax = box.max();
    vector2i P;
    barycentric_basis barycentric(
        tri[0].screen_coords, tri[1].screen_coords, tri[2].screen_coords);
    for(P.x = static_cast<int>(bboxmin.x); P.x <= bboxmax.x; P.x++) {
      for(P.y = static_cast<int>(bboxmin.y); P.y <= bboxmax.y; P.y++) {
        vector3f bc_screen = barycentric.compute(P);
        if(bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)
          continue;
        float Z = 0;
        vector2f UV{0, 0};
        for(int k = 0; k < 3; ++k) {
          Z += tri[k].screen_coords.z * bc_screen[k];
        }
        int depth_idx = static_cast<int>(P.y * ri.width + P.x);
        if(Z > depth[depth_idx]) {
          depth[depth_idx] = Z;
          stats.increment_pixel_count();
          // Interpolate pixel input.
          typename VertexOutput::value_type interpolated;
          for(int k = 0; k < 3; ++k) {
            interpolated.uv_coords =
                interpolated.uv_coords + (tri[k].uv_coords * bc_screen[k]);
            interpolated.normal =
                interpolated.normal + (tri[k].normal * bc_screen[k]);
          }
          // Invoke pixel shader.
          colour<float> lighted = derived().shade_fragment(interpolated);

          if(lighted.a() > 0.f) {
            rt_->set(
                static_cast<int>(P.x), static_cast<int>(P.y),
                colour_cast<std::uint8_t>(lighted));
          }
        }
      }
    }
  }

  Derived& derived() {
    return static_cast<Derived&>(*this);
  }

  Derived const& derived() const {
    return static_cast<Derived const&>(*this);
  }

  model const* model_        = nullptr;
  std::vector<float>* depth_ = nullptr;
  image* rt_                 = nullptr;
};

} // namespace swgl

#endif // SWGL_PIPELINE_HPP
