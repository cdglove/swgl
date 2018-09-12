//
// src/model.hpp
//
// Copyright (c) Chris Glover, 2018
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include "swgl/model.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace swgl {

model::model(std::istream& in) {
  std::string line;
  while(!in.eof()) {
    std::getline(in, line);
    std::istringstream iss(line.c_str());
    char trash;
    float ftrash;
    int itrash;
    if(!line.compare(0, 2, "v ")) {
      iss >> trash;
      Vec3f v;
      for(int i = 0; i < 3; i++)
        iss >> v.raw[i];
      positions_.push_back(v);
    }
    else if(!line.compare(0, 3, "vt ")) {
        iss >> trash >> trash;
        Vec2f uv;
        if(iss >> uv.u >> uv.v >> ftrash)
            uvs_.push_back(uv);
    }
    else if(!line.compare(0, 2, "f ")) {
      std::size_t slashes = std::count(line.begin(), line.end(), '/');
      int first, second, third;
      if(slashes == 0) {
        if(!(iss >> trash >> first >> second >> third)) {
          continue;
        }
      }
      else if(slashes == 3) {
        if(!(iss >> trash >> first >> trash >> itrash >> second >> trash >>
             itrash >> third >> trash >> itrash)) {
          continue;
        }
      }
      else if(slashes == 6) {
        if(!(iss >> trash >> first >> trash >> itrash >> trash >> itrash >>
             second >> trash >> itrash >> trash >> itrash >> third >> trash >>
             itrash >> trash >> itrash)) {
          if(!(iss >> trash >> first >> trash >> trash >> itrash >> second >>
               trash >> trash >> itrash >> third >> trash >> trash >> itrash)) {
            continue;
          }
        }
      }
      faces_.push_back({first - 1, second - 1, third - 1});
    }
  }
  std::cerr << "# v# " << positions_.size() << " f# " << faces_.size() << std::endl;
}

model::~model() {
}

int model::nverts() const {
  return (int)positions_.size();
}

int model::nfaces() const {
  return (int)faces_.size();
}

triangle const& model::face(int idx) const {
  return faces_[idx];
}

Vec3f model::position(int i) const {
  return positions_[i];
}

Vec2f model::uv(int i) const {
  return uvs_[i];
}

} // namespace swgl
