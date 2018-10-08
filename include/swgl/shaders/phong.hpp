//
// swgl/shaders/phong.hpp
//
// Copyright (c) Chris Glover, 2018
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SWGL_SHADERS_PHONG_HPP
#define SWGL_SHADERS_PHONG_HPP
#pragma once

#include "swgl/algorithm.hpp"
#include "swgl/pipeline.hpp"
#include "swgl/shaders/basic_lighted_model.hpp"

namespace swgl { namespace shaders {

class phong : public pipeline<phong, basic_lighted_model> {
 public:
  void set_albedo(image const& texture) {
    albedo_ = &texture;
  }

 private:
  struct vertex_out {
    vector3f cam_pos  = vector3f::zero();
    vector3f position = vector3f::zero();
    vector3f normal   = vector3f::zero();
    vector2f uv       = vector2f::zero();

    friend vertex_out operator*(vertex_out v, float scaler) {
      v.cam_pos *= scaler;
      v.position *= scaler;
      v.normal *= scaler;
      v.uv *= scaler;
      return v;
    }

    friend vertex_out operator+(vertex_out a, vertex_out const& b) {
      a.cam_pos += b.cam_pos;
      a.position += b.position;
      a.normal += b.normal;
      a.uv += b.uv;
      return a;
    }
  };

  static vector3f reflect(vector3f neg_light_dir, vector3f normal) {
    return neg_light_dir - (2.f * (dot(normal, neg_light_dir) * normal));
  }

  friend class pipeline<phong, basic_lighted_model>;

  vertex_out shade_vertex(std::size_t face, std::size_t idx) const {
    auto& model = get_model();
    vertex_out out;
    vector4f vertex_position = vector_widen<4>(model.position(face, idx), 1.f);

    vector4f proj = mvpv_ * vertex_position;
    out.cam_pos   = vector_narrow<3>(mv_ * vertex_position);
    out.position  = vector_narrow<3>(proj) / proj.w;
    out.uv        = model.uv(face, idx);
    out.normal    = vector_narrow<3>(mv_ * vector_widen<4>(model.normal(face, idx), 1.f));
    return out;
  }

  colour<float> shade_fragment(vertex_out const& in) const {
    vector3f light_dir   = (point_light_cs_ - in.cam_pos);
    float light_distance = light_dir.normalize();
    float n_dot_l        = dot(in.normal, light_dir);
    n_dot_l              = std::max(0.f, n_dot_l);

    float phong = 0.f;
    if(n_dot_l > 0.f) {
      vector3f view = vector_narrow<3>(draw_info_->view.get_column(3));
      vector3f ref  = reflect(-light_dir, in.normal);

      float phong = dot(view, ref);
      phong       = clamp(phong, 0.f, 1.f);
      phong       = pow(phong, 16.f);
    }

    vector4f albedo =
        to_vector(colour_cast<float>(albedo_->sample(in.uv.u, in.uv.v)));
    float attenuation = 1.f / light_distance;
    vector4f spec_colour(0.f, 0.f, 1.f, 1.f);
    float ambient = 0.2f;

    vector4f lighted = (albedo * attenuation * n_dot_l) +
                       (spec_colour * attenuation * phong) + (albedo * ambient);

    return from_vector(lighted);
  }

  image const* albedo_ = nullptr;
};

}} // namespace swgl::shaders

#endif // SWGL_SHADERS_PHONG_HPP
