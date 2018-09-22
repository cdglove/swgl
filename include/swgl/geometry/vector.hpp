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

template <typename T, std::size_t Dimension>
class vector_operators {
 private:
  using derived_type = vector<T, Dimension>;

 public:
  T& operator[](std::size_t idx) {
    return derived().raw[idx];
  }

  T const& operator[](std::size_t idx) const {
    return derived().raw[idx];
  }

  friend derived_type operator+(derived_type a, derived_type const& b) {
    for(std::size_t i = 0; i < derived_type::dimension; ++i) {
      a[i] += b[i];
    }

    return a;
  }

  friend derived_type operator-(derived_type a, derived_type const& b) {
    for(std::size_t i = 0; i < derived_type::dimension; ++i) {
      a[i] -= b[i];
    }

    return a;
  }

  friend derived_type operator*(derived_type a, derived_type const& b) {
    for(std::size_t i = 0; i < derived_type::dimension; ++i) {
      a[i] *= b[i];
    }

    return a;
  }

  friend derived_type operator*(derived_type a, T scaler) {
    for(std::size_t i = 0; i < derived_type::dimension; ++i) {
      a[i] *= scaler;
    }

    return a;
  }

  friend derived_type operator*(T scaler, derived_type b) {
    for(std::size_t i = 0; i < derived_type::dimension; ++i) {
      b[i] *= scaler;
    }

    return b;
  }

  friend derived_type operator/(derived_type a, derived_type const& b) {
    for(std::size_t i = 0; i < Dimension; ++i) {
      a[i] /= b[i];
    }
  }

  derived_type& operator+=(derived_type const& b) {
    for(std::size_t i = 0; i < Dimension; ++i) {
      (*this)[i] += b[i];
    }

    return derived();
    ;
  }

  derived_type& operator-=(derived_type const& b) {
    for(std::size_t i = 0; i < Dimension; ++i) {
      (*this)[i] -= b[i];
    }

    return derived();
    ;
  }

  derived_type& operator*=(derived_type const& b) {
    for(std::size_t i = 0; i < Dimension; ++i) {
      (*this)[i] *= b[i];
    }

    return derived();
    ;
  }

  derived_type& operator*=(T scaler) {
    for(std::size_t i = 0; i < Dimension; ++i) {
      (*this)[i] *= scaler;
    }

    return derived();
  }

  derived_type& operator/=(derived_type const& b) {
    for(std::size_t i = 0; i < Dimension; ++i) {
      (*this)[i] /= b[i];
    }

    return derived();
    ;
  }

  float normalize() {
    float len = length();
    derived() *= 1 / len;
    return len;
  }

  float length() const {
    return std::sqrt(dot(derived(), derived()));
  }

 private:
  derived_type& derived() {
    return static_cast<derived_type&>(*this);
  }

  derived_type const& derived() const {
    return static_cast<derived_type const&>(*this);
  }
};

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
class vector<T, 2> : public detail::vector_operators<T, 2> {
 public:
  using type                         = T;
  static const std::size_t dimension = 2;
  vector()                           = default;
  vector(vector const&)              = default;

  vector(T a, T b)
      : x(a)
      , y(b) {
  }

  union {
    struct {
      T x, y;
    };
    struct {
      T u, v;
    };
    std::array<T, 2> raw;
  };
};

template <typename T>
using vector2  = vector<T, 2>;
using vector2f = vector2<float>;
using vector2i = vector2<int>;

template <typename T>
class vector<T, 3> : public detail::vector_operators<T, 3> {
 public:
  using type                         = T;
  static const std::size_t dimension = 3;
  vector()                           = default;
  vector(vector const&)              = default;

  vector(T a, T b, T c)
      : x(a)
      , y(b)
      , z(c) {
  }

  union {
    struct {
      T x, y, z;
    };
    std::array<T, 3> raw;
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
  return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

template <typename T>
using vector3  = vector<T, 3>;
using vector3f = vector3<float>;
using vector3i = vector3<int>;

template <typename T>
class vector<T, 4> : public detail::vector_operators<T, 4> {
 public:
  using type                         = T;
  static const std::size_t dimension = 4;
  vector()                           = default;
  vector(vector const&)              = default;

  vector(T a, T b, T c, T d)
      : x(a)
      , y(b)
      , z(c)
      , w(d) {
  }

  union {
    struct {
      T x, y, z, w;
    };
    std::array<T, 4> raw;
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