//
// swgl/model.hpp
//
// Copyright (c) Chris Glover, 2018
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SWGL_MODEL_HPP
#define SWGL_MODEL_HPP

#include "swgl/geometry.hpp"
#include <array>
#include <iosfwd>
#include <vector>

namespace swgl {

class model {
 public:
  model(std::istream& in);
  ~model();
  int nverts() const;
  int nfaces() const;
  Vec3f position(int face, int i) const;
  Vec2f uv(int face, int i) const;

 private:
  std::vector<Vec3f> positions_;
  std::vector<Vec2f> uvs_;
  std::vector<std::array<int, 3>> idx_position_;
  std::vector<std::array<int, 3>> idx_uv_;
};

} // namespace swgl

#endif // SWGL_MODEL_HPP
