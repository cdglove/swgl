//
// swgl/colour.hpp
//
// Copyright (c) Chris Glover, 2018
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SWGL_COLOUR_HPP
#define SWGL_COLOUR_HPP
#pragma once

#include <array>

namespace swgl {

template <typename T>
class colour {
 public:
  typedef T component_type;

  colour() {
  }

  colour(T r, T g, T b, T a)
      : c_{r, g, b, a} {
  }

  colour(T const* source, std::size_t bytespp) {
    std::copy(source, source + bytespp, c_.begin());
    std::fill(c_.begin() + bytespp, c_.end(), 0);
  }

  T r() const {
    return c_[0];
  }

  T g() const {
    return c_[1];
  }

  T b() const {
    return c_[2];
  }

  T a() const {
    return c_[3];
  }

  T& r() {
    return c_[0];
  }

  T& g() {
    return c_[1];
  }

  T& b() {
    return c_[2];
  }

  T& a() {
    return c_[3];
  }

  T& operator[](std::size_t idx) {
    return c_[idx];
  }

  T operator[](std::size_t idx) const {
    return c_[idx];
  }

  T* data() {
    return c_.data();
  }

  T const* data() const {
    return c_.data();
  }

  friend colour operator*(colour a, colour const& b) {
    for(int i = 0; i < 4; ++i)
      a[i] *= b[i];
    return a;
  }

  void operator*=(colour const& b) {
    for(int i = 0; i < 4; ++i)
      c_[i] *= b[i];
  }

 private:
  std::array<T, 4> c_;
};

template <typename Target, typename Source>
colour<Target> colour_cast(colour<Source> const&);

template <>
inline colour<std::uint8_t> colour_cast(colour<std::uint8_t> const& c) {
    return c;
}

template <>
inline colour<float> colour_cast(colour<float> const& c) {
    return c;
}

template <>
inline colour<std::uint8_t> colour_cast(colour<float> const& source) {
  return colour<std::uint8_t>(
      static_cast<std::uint8_t>(source.r() * 255),
      static_cast<std::uint8_t>(source.g() * 255),
      static_cast<std::uint8_t>(source.b() * 255),
      static_cast<std::uint8_t>(source.a() * 255));
}

template <>
inline colour<float> colour_cast(colour<std::uint8_t> const& source) {
  return colour<float>(
      // clang-format off
      source.r() / 255.f,
      source.g() / 255.f, 
      source.b() / 255.f,
      source.a() / 255.f);
      // clang-format on
}

} // namespace swgl

#endif // SWGL_COLOUR_HPP