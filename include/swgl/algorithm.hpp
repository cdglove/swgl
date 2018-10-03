//
// swgl/algorithm.hpp
//
// Copyright (c) Chris Glover, 2018
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SWGL_ALGORITHM_HPP
#define SWGL_ALGORITHM_HPP
#pragma once

#include <algorithm>
#include <cassert>

namespace swgl {

template <typename T, typename Compare>
constexpr T const& clamp(T const& v, T const& lo, T const& hi, Compare comp) {
  assert(!comp(hi, lo));
  return comp(v, lo) ? lo : comp(hi, v) ? hi : v;
}

template <typename T>
constexpr T const& clamp(T const& v, T const& lo, T const& hi) {
  return clamp(v, lo, hi, std::less<>());
}

} // namespace swgl

#endif // SWGL_ALGORITHM_HPP