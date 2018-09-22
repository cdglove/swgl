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
    std::istringstream iss(line);
    char ctrash;
    if(!line.compare(0, 2, "v ")) {
      iss >> ctrash;
      vector3f v;
      for(int i = 0; i < 3; i++) {
        iss >> v.raw[i];
	  }
      positions_.push_back(v);
    }
    else if(!line.compare(0, 3, "vt ")) {
      iss >> ctrash >> ctrash;
      vector2f uv;
      if(iss >> uv.u >> uv.v) {
        uvs_.push_back(uv);
	  }
    }
    else if(!line.compare(0, 2, "f ")) {
      iss >> ctrash;
      auto decrement_indices = [](auto& c) {
        std::transform(
            c.begin(), c.end(), c.begin(), [](int i) { return i - 1; });
      };
      std::size_t slashes = std::count(line.begin(), line.end(), '/');
      std::array<int, 3> ipos, iuv, inorm;
      if(slashes == 0) {
        if(iss >> ipos[0] >> ipos[1] >> ipos[2]) {
          decrement_indices(ipos);
          idx_position_.push_back(ipos);
          continue;
        }
      }
      else if(slashes == 3) {
        if(iss >> ipos[0] >> ctrash >> iuv[0] >> ipos[1] >> ctrash >> iuv[1] >>
           ipos[2] >> ctrash >> iuv[2]) {
          decrement_indices(ipos);
          decrement_indices(iuv);
          idx_position_.push_back(ipos);
          idx_uv_.push_back(iuv);
          continue;
        }
      }
      else if(slashes == 6) {
        if(!(iss >> ipos[0] >> ctrash >> iuv[0] >> ctrash >> inorm[0] >>
             ipos[1] >> ctrash >> iuv[1] >> ctrash >> inorm[1] >> ipos[2] >>
             ctrash >> iuv[2] >> ctrash >> inorm[2])) {
          if(iss >> ipos[0] >> ctrash >> ctrash >> inorm[0] >> ipos[1] >>
             ctrash >> ctrash >> inorm[1] >> ipos[2] >> ctrash >> ctrash >>
             inorm[2]) {
            decrement_indices(ipos);
            decrement_indices(inorm);
            idx_position_.push_back(ipos);
            idx_uv_.push_back(inorm);
            continue;
          }
        }

        decrement_indices(ipos);
        decrement_indices(iuv);
        decrement_indices(inorm);
        idx_position_.push_back(ipos);
        idx_uv_.push_back(iuv);
        //idx_norm_.push_back(inorm);
      }
    }
  }

  std::cerr << "# v# " << positions_.size() << " f# " << idx_position_.size()
            << std::endl;
}

model::~model() = default;

int model::nverts() const {
  return static_cast<int>(positions_.size());
}

int model::nfaces() const {
  return static_cast<int>(idx_position_.size());
}

vector3f model::position(int face, int idx) const {
  return positions_[idx_position_[face][idx]];
}

vector2f model::uv(int face, int idx) const {
  return uvs_[idx_uv_[face][idx]];
}

} // namespace swgl
