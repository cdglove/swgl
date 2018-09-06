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

#include "swgl/model.hpp"

namespace swgl {

class image;
class model;

class pipeline {
 public:
  void draw(model const& m, image& rt) const {
    draw_impl(m, rt);
  }

 private:
  virtual void draw_impl(model const& m, image& rt) const;
};

} // namespace swgl

#endif // SWGL_PIPELINE_HPP