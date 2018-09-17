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

namespace detail {
template <typename Derived>
class vector_impl {
 public:
  typename Derived::type& operator[](std::size_t idx) {
    return v_[idx];
  }

  typename Derived::type const& operator[](std::size_t idx) const {
    return v_[idx];
  }

  friend Derived operator+(Derived a, Derived const& b) {
    for(std::size_t i = 0; i < Derived::dimension; ++i) {
      a[i] += b[i];
    }

    return a;
  }

  friend Derived operator-(Derived a, Derived const& b) {
    for(std::size_t i = 0; i < Derived::dimension; ++i) {
      a[i] -= b[i];
    }

    return a;
  }

  friend Derived operator*(Derived a, Derived const& b) {
    for(std::size_t i = 0; i < Derived::dimension; ++i) {
      a[i] *= b[i];
    }

    return a;
  }

  friend Derived operator/(Derived a, Derived const& b) {
    for(std::size_t i = 0; i < Derived::dimension; ++i) {
      a[i] /= b[i];
    }
  }

  Derived& operator+=(Derived const& b) {
    for(std::size_t i = 0; i < Derived::dimension; ++i) {
      (*this)[i] += b[i];
    }

    return *this;
  }

  Derived& operator-=(Derived const& b) {
    for(std::size_t i = 0; i < Derived::dimension; ++i) {
      (*this)[i] -= b[i];
    }

    return *this;
  }

  Derived& operator*=(Derived const& b) {
    for(std::size_t i = 0; i < Derived::dimension; ++i) {
      (*this)[i] *= b[i];
    }

    return *this;
  }

  Derived& operator/=(Derived const& b) {
    for(std::size_t i = 0; i < Derived::dimension; ++i) {
      (*this)[i] /= b[i];
    }

    return *this;
  }

 private:
  std::array<typename Derived::type, Derived::dimension> v_;
};

} // namespace detail

template <typename T, std::size_t Dimension>
class vector : public detail::vector_impl<vector<T, Dimension>> {
 public:
  using type                         = T;
  static const std::size_t dimension = Dimension;
};

template <typename T>
class vector<T, 2> : public detail::vector_impl<vector<T, 2>> {
 public:
  T& x() {
    return (*this)[0];
  }

  T const& x() const {
    return (*this)[0];
  }

  T& y() {
    return (*this)[1];
  }

  T const& y() const {
    return (*this)[1];
  }
};

template <typename T>
class vector<T, 3> : public detail::vector_impl<vector<T, 3>> {
 public:
  T& x() {
    return (*this)[0];
  }

  T const& x() const {
    return (*this)[0];
  }

  T& y() {
    return (*this)[1];
  }

  T const& y() const {
    return (*this)[0];
  }

  T& z() {
    return (*this)[1];
  }

  T const& z() const {
    return (*this)[0];
  }
};

template <typename T>
vector<T, 3> cross(vector<T, 3> v1, vector<T, 3> v2) {
  return vector<T, 3>(
      // clang-format off
      v1.y() * v2.z() - v1.z() * v2.y(), 
      v1.z() * v2.x() - v1.x() * v2.z(),
      v1.x() * v2.y() - v1.y() * v2.x()
      // clang-format on
  );
}

template <typename T>
T dot(vector<T, 3> v1, vector<T, 3> v2) {
  return (v1.x() * v2.x() + v1.y() * v2.y() + v1.z() * v2.z());
}

template <typename T>
class vector<T, 4> : public detail::vector_impl<vector<T, 4>> {
 public:
  T& x() {
    return (*this)[0];
  }

  T const& x() const {
    return (*this)[0];
  }

  T& y() {
    return (*this)[1];
  }

  T const& y() const {
    return (*this)[0];
  }

  T& z() {
    return (*this)[1];
  }

  T const& z() const {
    return (*this)[0];
  }

  T& w() {
    return (*this)[1];
  }

  T const& w() const {
    return (*this)[0];
  }
};

template<typename T> 
using vector4 = vector<T, 4>;
using vector4f = vector<float, 4>;
using vector4i = vector<int, 4>;

} // namespace swgl

#endif // SWGL_GEOMETRY_BBOX_HPP