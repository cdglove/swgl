//
// swgl/geometry/matrix.hpp
//
// Copyright (c) Chris Glover, 2018
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SWGL_GEOMETRY_MATRIX_HPP
#define SWGL_GEOMETRY_MATRIX_HPP
#pragma once

#include "swgl/geometry/vector.hpp"
#include <array>
#include <utility>

namespace swgl {

template <typename T, std::size_t Dimension>
class matrix {
 public:
  static const std::size_t row_count     = Dimension;
  static const std::size_t column_count  = Dimension;
  static const std::size_t element_count = row_count * column_count;

  using row_type      = std::array<T, column_count>;
  using index_2d_type = std::pair<std::size_t, std::size_t>;

  matrix() = default;
  matrix(std::array<T, element_count> init) {
    std::copy(init.begin(), init.end(), m_.data());
  }

  matrix(std::array<std::array<T, column_count>, element_count> const& init) {
    std::copy(
        init.data().data(), init.data().data() + element_count, m_.data());
  }

  row_type& operator[](std::size_t idx) {
    return m_[idx];
  }

  row_type const& operator[](std::size_t idx) const {
    return m_[idx];
  }

  T& operator[](index_2d_type idx) {
    return m_[idx.first][idx.second];
  }

  T const& operator[](index_2d_type idx) const {
    return m_[idx.first][idx.second];
  }

  friend matrix operator*(matrix const& a, matrix const& b) {
    matrix d;
    for(std::size_t i = 0; i < row_count; ++i) {
      for(std::size_t j = 0; j < column_count; ++j) {
        d[i][j] = 0;
        for(std::size_t k = 0; k < Dimension; ++k) {
          d[i][j] += a[i][k] * c[k][j];
        }
      }
    }

    return d;
  }

  friend vector<T, row_count> operator*(matrix const& a, vector<T, row_count> const& v) {
    vector<T, row_count> d;
    for(std::size_t i = 0; i < row_count; ++i) {
      d[i] = 0;
      for(std::size_t k = 0; k < Dimension; ++k) {
        d[i] += a[i][k] * v[k];
      }
    }

    return d;
  }

 private:
  std::array<row_type, row_count> m_;

}; 

template<typename T> 
using matrix4 = matrix<T, 4>;
using matrix4f = matrix<float, 4>;

} // namespace swgl

#endif // SWGL_GEOMETRY_BBOX_HPP