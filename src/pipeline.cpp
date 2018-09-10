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

static Vec3f barycentric(Vec3f t0, Vec3f t1, Vec3f t2, Vec2i P) {
  Vec3f u = cross(
      Vec3f(t2[0] - t0[0], t1[0] - t0[0], t0[0] - P[0]),
      Vec3f(t2[1] - t0[1], t1[1] - t0[1], t0[1] - P[1]));
  if(std::abs(u[2]) < 1) {
    return Vec3f(-1, 1, 1); // triangle is degenerate, in this case return smth
                            // with negative coordinates
  }
  return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

static void draw_triangle_barycentric(
    std::array<Vec3f, 3> const& tri,
    swgl::image& rt,
    std::vector<float>& depth,
    swgl::image::colour_type colour,
    swgl::pipeline_stats& stats) {
  stats.increment_triangle_count();
  swgl::bbox<float, 3> box(tri[0]);
  box.expand(tri[1]);
  box.expand(tri[2]);
  box.clamp(
      Vec3f(0.f, 0.f, 0.f), Vec3f(
                                static_cast<float>(rt.width() - 1),
                                static_cast<float>(rt.height() - 1), 0.f));
  auto bboxmin = box.min();
  auto bboxmax = box.max();
  Vec2i P;
  for(P.x = static_cast<int>(bboxmin.x); P.x <= bboxmax.x; P.x++) {
    for(P.y = static_cast<int>(bboxmin.y); P.y <= bboxmax.y; P.y++) {
      Vec3f bc_screen = barycentric(tri[0], tri[1], tri[2], P);
      if(bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)
        continue;
      float Z = 0;
      for(int k = 0; k < 3; ++k)
        Z += tri[k].z * bc_screen[k];
      int depth_idx = static_cast<int>(P.y * rt.width() + P.x);
      if(Z > depth[depth_idx]) {
        depth[depth_idx] = Z;
        stats.increment_pixel_count();
        rt.set(static_cast<int>(P.x), static_cast<int>(P.y), colour);
      }
    }
  }
}

static void draw_triangle_segments(
    std::array<Vec3f, 3> tri_,
    swgl::image& rt,
    std::vector<float>& depth,
    swgl::image::colour_type colour,
    swgl::pipeline_stats& stats) {
  stats.increment_triangle_count();
  std::array<Vec3i, 3> tri{Vec3i(tri_[0]), Vec3i(tri_[1]), Vec3i(tri_[2])};
  if(tri[0].y == tri[1].y && tri[0].y == tri[2].y)
    return; // i dont care about degenerate triangles
  if(tri[0].y > tri[1].y) {
    std::swap(tri[0], tri[1]);
    std::swap(tri_[0], tri_[1]);
  }
  if(tri[0].y > tri[2].y) {
    std::swap(tri[0], tri[2]);
    std::swap(tri_[0], tri_[2]);
  }
  if(tri[1].y > tri[2].y) {
    std::swap(tri[1], tri[2]);
    std::swap(tri_[1], tri_[2]);
  }
  int total_height = tri[2].y - tri[0].y;
  int width        = rt.width();
  for(int i = 0; i < total_height; i++) {
    bool second_half   = i > tri[1].y - tri[0].y || tri[1].y == tri[0].y;
    int segment_height = second_half ? static_cast<int>(tri[2].y - tri[1].y)
                                     : static_cast<int>(tri[1].y - tri[0].y);
    float alpha = (float)i / total_height;
    float beta  = (float)(i - (second_half ? tri[1].y - tri[0].y : 0)) /
                 segment_height; // be careful: with above conditions no
                                 // division by zero here
    Vec3i A = Vec3i(tri[0] + (tri[2] - tri[0]) * alpha);
    Vec3i B = Vec3i(
        second_half ? tri[1] + (tri[2] - tri[1]) * beta
                    : tri[0] + (tri[1] - tri[0]) * beta);
    if(A.x > B.x)
      std::swap(A, B);
    for(int j = static_cast<int>(A.x); j <= B.x; j++) {
      Vec2i P(j, static_cast<int>(tri[0].y) + i);
      Vec3f bc_screen = barycentric(tri_[0], tri_[1], tri_[2], P);
      float Z         = 0;
      for(int k = 0; k < 3; ++k)
        Z += tri_[k].z * bc_screen[k];
      int depth_idx = static_cast<int>(P.y * rt.width() + P.x);
      if(Z > depth[depth_idx]) {
        depth[depth_idx] = Z;
        stats.increment_pixel_count();
        rt.set(
            j, static_cast<int>(tri[0].y) + i,
            colour); // attention, due to int casts t0.y+i != A.y
      }
    }
  }
}

static Vec3f world_to_screen(Vec3f w, Vec2i half_screen) {
  return Vec3f(
      std::round((w.x + 1.f) * half_screen.x),
      std::round((w.y + 1.f) * half_screen.y), w.z);
}

bool draw_barycentric = false;
namespace swgl {
pipeline_stats pipeline::draw_impl(
    model const& model, image& rt, std::vector<float>& depth) const {
  pipeline_stats stats;
  stats.increment_draw_count();
  Vec3f light_dir(0, 0, -1);
  int width       = rt.width();
  int height      = rt.height();
  int half_width  = width / 2;
  int half_height = height / 2;
  typedef swgl::image::colour_type::component_type colour_type;
  for(int i = 0; i < model.nfaces(); i++) {
    triangle face = model.face(i);
    std::array<Vec3f, 3> screen_coords;
    std::array<Vec3f, 3> world_coords;
    for(int j = 0; j < 3; j++) {
      Vec3f v = model.vert(face[j]);
      screen_coords[j] =
          Vec3f((v.x + 1.f) * half_width, (v.y + 1.f) * half_height, v.z);
      world_coords[j] = v;
    }
    Vec3f n = cross(
        (world_coords[2] - world_coords[0]),
        (world_coords[1] - world_coords[0]));
    n.normalize();
    float intensity = dot(n, light_dir);
    if(intensity > 0) {
      swgl::colour<float> output(intensity, intensity, intensity, 0.f);
      if(draw_barycentric) {
        draw_triangle_barycentric(
            screen_coords, rt, depth, swgl::colour_cast<colour_type>(output),
            stats);
      }
      else {
        draw_triangle_segments(
            screen_coords, rt, depth, swgl::colour_cast<colour_type>(output),
            stats);
      }
    }
  }

  return stats;
}

} // namespace swgl