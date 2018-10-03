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
#include "swgl/pipeline_counters.hpp"

#include <algorithm>
#include <numeric>
#include <array>

namespace swgl {

class image;
class model;

class pipeline_base {
 public:
  pipeline_counters draw() const {
    return draw_impl();
  }

 private:
  virtual pipeline_counters draw_impl() const = 0;
};

template <typename Derived, typename Base = pipeline_base>
class pipeline : public Base {
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
  class face {
   public:
    using value_type = T;

    T interpolate(vector3f weights) const {
      return std::inner_product(
          vertices_.begin() + 1, vertices_.end(), &weights.raw[1],
          (*vertices_.begin()) * weights[0]);
    }

    T& operator[](std::size_t idx) {
      return vertices_[idx];
    }

    T const& operator[](std::size_t idx) const {
      return vertices_[idx];
    }

   private:
    std::array<T, 3> vertices_;
  };

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
    for(int iface = 0; iface < model.nfaces(); ++iface) {
      face<decltype(derived().shade_vertex(iface, 0))> vertex_out;
      for(int j = 0; j < 3; j++) {
        vertex_out[j] = derived().shade_vertex(iface, j);
      }

      // Cull backfaces
      auto normal = cross(
          (vertex_out[2].position - vertex_out[0].position),
          (vertex_out[1].position - vertex_out[0].position));
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
    swgl::bbox<float, 3> box(tri[0].position);
    box.expand(tri[1].position);
    box.expand(tri[2].position);
    box.clamp({0.f, 0.f, 0.f}, vector3f(ri.width - 1.f, ri.height - 1.f, 0.f));
    auto bboxmin = box.min();
    auto bboxmax = box.max();
    vector2i P;
    barycentric_basis barycentric(
        tri[0].position, tri[1].position, tri[2].position);
    for(P.x = static_cast<int>(bboxmin.x); P.x <= bboxmax.x; P.x++) {
      for(P.y = static_cast<int>(bboxmin.y); P.y <= bboxmax.y; P.y++) {
        vector3f bc_screen = barycentric.compute(P);
        if(bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)
          continue;
        float Z = 0;
        for(int k = 0; k < 3; ++k) {
          Z += tri[k].position.z * bc_screen[k];
        }
        int depth_idx = static_cast<int>(P.y * ri.width + P.x);
        if(Z > depth[depth_idx]) {
          depth[depth_idx] = Z;
          stats.increment_pixel_count();
          colour<float> lighted =
              derived().shade_fragment(tri.interpolate(bc_screen));

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
