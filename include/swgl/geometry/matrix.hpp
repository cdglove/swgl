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

  using row_type = std::array<T, column_count>;

  matrix() = default;

  friend matrix operator*(matrix const& a, matrix const& b) {
    matrix d;
    for(std::size_t i = 0; i < row_count; ++i) {
      for(std::size_t j = 0; j < column_count; ++j) {
        d[i][j] = 0;
        for(std::size_t k = 0; k < Dimension; ++k) {
          d[i][j] += a[i][k] * b[k][j];
        }
      }
    }

    return d;
  }

  friend vector<T, row_count> operator*(
      matrix const& a, vector<T, row_count> const& v) {
    auto d = vector<T, row_count>::zero();
    for(std::size_t i = 0; i < row_count; ++i) {
      for(std::size_t k = 0; k < Dimension; ++k) {
        d[i] += a[i][k] * v[k];
      }
    }

    return d;
  }

  static matrix identity() {
    matrix ret;
    std::fill(ret.raw.begin(), ret.raw.end(), T(0));
    for(int i = 0; i < Dimension; ++i) {
      ret.m[i][i] = 1;
    }
    return ret;
  }

  row_type& operator[](std::size_t idx) {
    return m[idx];
  }

  row_type const& operator[](std::size_t idx) const {
    return m[idx];
  }

  void set_column(std::size_t col, vector<T, row_count> c) {
    for(int i = 0; i < row_count; ++i) {
      m[i][col] = c[i];
    }
  }

  void set_row(std::size_t row, vector<T, column_count> r) {
    std::copy(r.raw.begin(), r.raw.end(), m[row].begin());
  }

  vector<T, row_count> get_column(std::size_t col) const {
    vector<T, row_count> c;
    for(int i = 0; i < row_count; ++i) {
      c[i] = m[i][col];
    }
    return c;
  }

  void get_row(std::size_t row) {
    vector<T, column_count> r;
    std::copy(m[row].begin(), m[row].end(), r.raw.begin());
    return r;
  }

  union {
    std::array<row_type, row_count> m;
    std::array<T, element_count> raw;
  };
};

template <typename T>
using matrix4  = matrix<T, 4>;
using matrix4f = matrix<float, 4>;

} // namespace swgl

#endif // SWGL_GEOMETRY_BBOX_HPP