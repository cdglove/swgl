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

class triangle {
 public:
  triangle(int a, int b, int c)
      : indices_{a, b, c} {
  }

  int operator[](std::size_t idx) {
    return indices_[idx];
  }

 private:
  std::array<int, 3> indices_;
};

class model {
 public:
  model(std::istream& in);
  ~model();
  int nverts() const;
  int nfaces() const;
  Vec3f position(int i) const;
  Vec2f uv(int i) const;
  triangle const& face(int idx) const;

 private:
  std::vector<Vec3f> positions_;
  std::vector<Vec2f> uvs_;
  std::vector<triangle> faces_;
};

} // namespace swgl

#endif // SWGL_MODEL_HPP
