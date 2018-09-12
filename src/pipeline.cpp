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
#include "swgl/geometry.hpp"
#include "swgl/geometry/bbox.hpp"
#include "swgl/image.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

static void line(
    Vec2i p0, Vec2i p1, swgl::image& rt, swgl::image::colour_type colour) {
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
    float t = (x - p0.x) / (float)(p1.x - p0.x);
    int y   = static_cast<int>(p0.y * (1.f - t) + p1.y * t);
    if(steep) {
      rt.set(y, x, colour);
    }
    else {
      rt.set(x, y, colour);
    }
  }
}

class barycentric_basis {
 public:
  barycentric_basis(Vec3f t0, Vec3f t1, Vec3f t2)
      : root_(t0)
      , basis_{Vec3f(t2[0] - t0[0], t1[0] - t0[0], 0.f),
               Vec3f(t2[1] - t0[1], t1[1] - t0[1], 0.f)} {
  }

  Vec3f compute(Vec2i P) const {
    basis_[0].z = root_.x - P.x;
    basis_[1].z = root_.y - P.y;

    Vec3f u = cross(basis_[0], basis_[1]);

    if(std::abs(u[2]) < 1) {
      // triangle is degenerate.
      return Vec3f(-1, 1, 1);
    }

    float zrecip = 1.f / u.z;
    return Vec3f(1.f - (u.x + u.y) * zrecip, u.y * zrecip, u.x * zrecip);
  }

 private:
  Vec3f root_;
  mutable std::array<Vec3f, 2> basis_;
};

static Vec3f world_to_screen(Vec3f w, Vec2i half_screen) {
  return Vec3f(
      std::round((w.x + 1.f) * half_screen.x),
      std::round((w.y + 1.f) * half_screen.y), w.z);
}

namespace swgl {
pipeline_counters pipeline::draw_impl() const {
  pipeline_counters stats;
  stats.increment_draw_count();
  Vec3f light_dir(0, 0, -1);
  raster_info ri;
  ri.width           = rt_->width();
  ri.height          = rt_->height();
  ri.half_width      = ri.width / 2;
  ri.half_height     = ri.height / 2;
  model const& model = *model_;
  typedef swgl::image::colour_type::component_type colour_type;
  for(int i = 0; i < model.nfaces(); i++) {
    triangle face = model.face(i);
    face_t<Vec3f> screen_coords;
    face_t<Vec3f> world_coords;
    face_t<Vec2f> uv_coords;
    for(int j = 0; j < 3; j++) {
      Vec3f v = model.position(face[j]);
      screen_coords[j] =
          Vec3f((v.x + 1.f) * ri.half_width, (v.y + 1.f) * ri.half_height, v.z);
      world_coords[j] = v;
      uv_coords[j] = model.uv(face[j]);
    }
    Vec3f n = cross(
        (world_coords[2] - world_coords[0]),
        (world_coords[1] - world_coords[0]));
    n.normalize();
    float intensity = dot(n, light_dir);
    if(intensity > 0) {
      swgl::colour<float> light(intensity, intensity, intensity, 0.f);
      draw_triangle(ri, screen_coords, uv_coords, light, stats);
    }
  }
  return stats;
}

void pipeline::draw_triangle(
    raster_info const& ri,
    face_t<Vec3f> const& tri,
    face_t<Vec2f> uvs,
    colour<float> light,
    pipeline_counters& stats) const {
  stats.increment_triangle_count();
  auto& depth = *depth_;
  swgl::bbox<float, 3> box(tri[0]);
  box.expand(tri[1]);
  box.expand(tri[2]);
  box.clamp(Vec3f(0.f, 0.f, 0.f), Vec3f(Vec3i(ri.width - 1, ri.width - 1, 0)));
  auto bboxmin = box.min();
  auto bboxmax = box.max();
  Vec2i P;
  barycentric_basis barycentric(tri[0], tri[1], tri[2]);
  for(P.x = static_cast<int>(bboxmin.x); P.x <= bboxmax.x; P.x++) {
    for(P.y = static_cast<int>(bboxmin.y); P.y <= bboxmax.y; P.y++) {
      Vec3f bc_screen = barycentric.compute(P);
      if(bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)
        continue;
      float Z = 0;
      Vec2f UV{0, 0};
      for(int k = 0; k < 3; ++k) {
        Z += tri[k].z * bc_screen[k];
        UV = UV + (uvs[k] * bc_screen[k]);
      }
      int depth_idx = static_cast<int>(P.y * ri.width + P.x);
      if(Z > depth[depth_idx]) {
        depth[depth_idx] = Z;
        stats.increment_pixel_count();
        // Invoke pixel shader.
        swgl::image::colour_type albedo =
            textures_[0]->sample(UV.u, UV.v);
        colour<float> lighted = light * colour_cast<float>(albedo);
        rt_->set(
            static_cast<int>(P.x), static_cast<int>(P.y),
            colour_cast<std::uint8_t>(lighted));
      }
    }
  }
} // namespace swgl

} // namespace swgl
