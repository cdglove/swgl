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
#include "swgl/shaders/basic_lighted_model.hpp"

namespace swgl { namespace shaders {

class flat : public pipeline<flat, basic_lighted_model> {
 public:
  void set_albedo(image const& texture) {
    albedo_ = &texture;
  }

 private:
  struct vertex_out {
    vector3f position = vector3f::zero();
    vector2f uv       = vector2f::zero();
    float light       = 0.f;

    friend vertex_out operator*(vertex_out v, float scaler) {
      v.position *= scaler;
      v.uv *= scaler;
      v.light *= scaler;
      return v;
    }

    friend vertex_out operator+(vertex_out a, vertex_out const& b) {
      a.position += b.position;
      a.uv += b.uv;
      a.light += b.light;
      return a;
    }
  };

  friend class pipeline<flat, basic_lighted_model>;

  vertex_out shade_vertex(std::size_t face, std::size_t idx) const {
    auto& model = get_model();
    vertex_out out;
    vector4f proj =
        cached_mvpv_ * vector_widen<4>(model.position(face, idx), 1.f);
    out.position = vector_narrow<3>(proj) / proj.w;
    out.uv       = model.uv(face, idx);

    // Compute directional light.
    auto normal = cross(
        (model.position(face, 1) - model.position(face, 0)),
        (model.position(face, 2) - model.position(face, 0)));
    normal.normalize();
    normal.normalize();

    float intensity = dot(normal, draw_info_->directional_light);
    intensity       = std::max(0.f, intensity);

    // Add some Ambient
    intensity += 0.2f;
    intensity = std::min(intensity, 1.f);
    out.light = intensity;
    return out;
  }

  colour<float> shade_fragment(vertex_out const& in) const {
    swgl::colour<float> light(in.light, in.light, in.light, 1.f);
    swgl::image::colour_type albedo = albedo_->sample(in.uv.u, in.uv.v);
    colour<float> lighted           = light * colour_cast<float>(albedo);
    lighted.a()                     = 1.f;
    return lighted;
  }

  image const* albedo_ = nullptr;
};

}} // namespace swgl::shaders

#endif // SWGL_SHADERS_FLAT_HPP
