//
// swgl/shaders/basic_lighted_model.hpp
//
// Copyright (c) Chris Glover, 2018
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SWGL_SHADERS_BASICLIGHTEDMODEL_HPP
#define SWGL_SHADERS_BASICLIGHTEDMODEL_HPP
#pragma once

#include "swgl/geometry/matrix.hpp"
#include "swgl/geometry/vector.hpp"
#include "swgl/pipeline_counters.hpp"

namespace swgl { namespace shaders {

class basic_lighted_model {
 public:
  struct draw_info {
    matrix4f model;
    matrix4f view;
    matrix4f projection;
    matrix4f viewport;
    vector3f eye;
    vector3f point_light;
    vector3f directional_light;
    float ambient_light;
  };

  pipeline_counters draw(draw_info const& info) const {
    prepare_for_draw(info);
    return draw_impl();
  }

 protected:
  void prepare_for_draw(draw_info const& info) const {
    draw_info_ = &info;
    mv_        = info.view * info.model;
    mvpv_      = info.viewport * info.projection * mv_;

    directional_light_cs_ = vector_narrow<3>(
        info.view * vector_widen<4>(info.directional_light, 1.f));
    point_light_cs_ =
        vector_narrow<3>(info.view * vector_widen<4>(info.point_light, 1.f));
  }

  mutable draw_info const* draw_info_;
  mutable matrix4f mvpv_;
  mutable matrix4f mv_;
  mutable vector3f directional_light_cs_;
  mutable vector3f point_light_cs_;

 private:
  virtual pipeline_counters draw_impl() const = 0;
};

}} // namespace swgl::shaders

#endif // SWGL_SHADERS_BASICLIGHTEDMODEL_HPP