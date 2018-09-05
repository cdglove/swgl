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
#include <vector>
#include <iosfwd>

namespace swgl {

class model {
 public:
  model(std::istream& in);
  ~model();
  int nverts() const;
  int nfaces() const;
  Vec3f vert(int i) const;
  std::vector<int> face(int idx) const;

 private:
  std::vector<Vec3f> verts_;
  std::vector<std::vector<int>> faces_;
};

} // namespace swgl

#endif // SWGL_MODEL_HPP
