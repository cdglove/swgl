//
// swgl/shaders/flat.hpp
//
// Copyright (c) Chris Glover, 2018
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SWGL_SHADERS_FLAT_HPP
#define SWGL_SHADERS_FLAT_HPP
#pragma once

#include "swgl/pipeline.hpp"

namespace swgl { namespace shaders {

class flat : public pipeline<flat> {
 public:
  void set_albedo(image const& texture) {
    albedo_ = &texture;
  }

  void set_view(matrix4f m) {
    view_ = m;
  }

  void set_projection(matrix4f m) {
    projection_ = m;
  }

  void set_viewport(matrix4f m) {
    viewport_ = m;
  }

  void set_view_matrix(matrix4f m) {
    model_mat_ = m;
  }

 private:
  struct vertex_out {
    vector3f screen_coords = vector3f::zero();
    vector3f normal        = vector3f::zero();
    vector2f uv_coords     = vector2f::zero();
  };

  using base = pipeline<flat>;
  friend class base;

  vertex_out shade_vertex(std::size_t face, std::size_t idx) const {
    auto& model = get_model();
    vertex_out out;
    vector4f proj =
        viewport_ * projection_ * view_ * model_mat_ *
        vector_cast_widen<swgl::vector4f>(model.position(face, idx), 1.f);
    out.screen_coords = vector_cast_narrow<swgl::vector3f>(proj) / proj.w;
    out.uv_coords     = model.uv(face, idx);

    out.normal = cross(
        (model.position(face, 2) - model.position(face, 0)),
        (model.position(face, 1) - model.position(face, 0)));
    out.normal.normalize();
    return out;
  }

  colour<float> shade_fragment(vertex_out const& in) const {
    vector3f light_dir(0, 0, -1);
    float intensity = dot(in.normal, light_dir);
    intensity       = std::max(0.f, intensity);
    // Add some Ambient
    intensity += 0.1f;
    intensity = std::min(intensity, 1.f);
    colour<float> lighted;
    if(intensity >= 0) {
      swgl::colour<float> light(intensity, intensity, intensity, 0.f);
      swgl::image::colour_type albedo =
          albedo_->sample(in.uv_coords.u, in.uv_coords.v);
      lighted     = light * colour_cast<float>(albedo);
      lighted.a() = 1;
    }
    else {
      lighted = colour<float>(0.f, 0.f, 0.f, 1.f);
    }
    // lighted = colour<float>(std::abs(in.normal.x), std::abs(in.normal.y),
    // std::abs(in.normal.z), 1.f);
    return lighted;
  }

  image const* albedo_ = nullptr;
  matrix4f projection_ = matrix4f::identity();
  matrix4f view_       = matrix4f::identity();
  matrix4f viewport_   = matrix4f::identity();
  matrix4f model_mat_  = matrix4f::identity();
};

}} // namespace swgl::shaders

#endif // SWGL_SHADERS_FLAT_HPP
