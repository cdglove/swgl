//
// swgl/geometry/vector.hpp
//
// Copyright (c) Chris Glover, 2018
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SWGL_GEOMETRY_VECTOR_HPP
#define SWGL_GEOMETRY_VECTOR_HPP
#pragma once

#include <array>

namespace swgl {

template <typename T, std::size_t Dimension>
class vector;

namespace detail {

} // namespace detail

// template <typename T, std::size_t Dimension>
// class vector {
//  public:
//   using type                         = T;
//   static const std::size_t dimension = Dimension;
//   vector()                           = default;
//   vector(vector const&)              = default;
// };

template <typename T>
class vector<T, 2> {
 public:
  using type                         = T;
  static const std::size_t dimension = 2;
  vector()                           = default;
  vector(vector const&)              = default;

  T& operator[](std::size_t idx) {
    return v[idx];
  }

  T const& operator[](std::size_t idx) const {
    return v[idx];
  }

  union {
    struct {
      T x, y;
    };
    std::array<T, 2> v;
  };
};

template <typename T>
using vector2  = vector<T, 2>;
using vector2f = vector2<float>;
using vector2i = vector2<int>;

template <typename T>
class vector<T, 3> {
 public:
  using type                         = T;
  static const std::size_t dimension = 3;
  vector()                           = default;
  vector(vector const&)              = default;

  T& operator[](std::size_t idx) {
    return v[idx];
  }

  T const& operator[](std::size_t idx) const {
    return v[idx];
  }

  union {
    struct {
      T x, y, z;
    };
    std::array<T, 3> v;
  };
};

template <typename T>
vector<T, 3> cross(vector<T, 3> v1, vector<T, 3> v2) {
  return vector<T, 3>(
      // clang-format off
      v1.y * v2.z - v1.z * v2.y, 
      v1.z * v2.x - v1.x * v2.z,
      v1.x * v2.y - v1.y * v2.x
      // clang-format on
  );
}

template <typename T>
T dot(vector<T, 3> v1, vector<T, 3> v2) {
  return (v1.x() * v2.x() + v1.y() * v2.y() + v1.z() * v2.z());
}

template <typename T>
using vector3  = vector<T, 3>;
using vector3f = vector3<float>;
using vector3i = vector3<int>;

template <typename T>
class vector<T, 4> {
 public:
  using type                         = T;
  static const std::size_t dimension = 4;
  vector()                           = default;
  vector(vector const&)              = default;

  T& operator[](std::size_t idx) {
    return v[idx];
  }

  T const& operator[](std::size_t idx) const {
    return v[idx];
  }

  union {
    struct {
      T x, y, z, w;
    };
    std::array<T, 4> v;
  };
};

template <typename T>
vector<T, 4> cross(vector<T, 4> v1, vector<T, 4> v2) {
  return {
      // clang-format off
      v1.y * v2.z - v1.z * v2.y, 
      v1.z * v2.x - v1.x * v2.z,
      v1.x * v2.y - v1.y * v2.x,
      0
      // clang-format on
  };
}

template <typename T>
T dot(vector<T, 4> v1, vector<T, 4> v2) {
  return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

template <typename T>
using vector4  = vector<T, 4>;
using vector4f = vector4<float>;
using vector4i = vector4<int>;

} // namespace swgl

#endif // SWGL_GEOMETRY_BBOX_HPP