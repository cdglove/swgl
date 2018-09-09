//
// swgl/geometry/bbox.hpp
//
// Copyright (c) Chris Glover, 2018
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SWGL_GEOMETRY_BBOX_HPP
#define SWGL_GEOMETRY_BBOX_HPP
#pragma once

namespace swgl {

namespace detail {

template <typename T, std::size_t Dim>
struct bbox_dimension;

template <typename T>
struct bbox_dimension<T, 2> {
  typedef Vec2<T> type;
};

template <typename T>
struct bbox_dimension<T, 3> {
  typedef Vec3<T> type;
};

} // namespace detail

template <typename T, std::size_t Dimension>
class bbox {
 public:
  typedef typename detail::bbox_dimension<T, Dimension>::type dimension_type;

  bbox(dimension_type const& p)
      : min_(p)
      , max_(p) {
  }

  bbox(dimension_type first, dimension_type second) {
    for(std::size_t i = 0; i < Dimension; ++i) {
      min_[i] = std::max(first[i], second[i]);
      max_[i] = std::min(first[i], second[i]);
    }
  }

  void expand(dimension_type p) {
    for(std::size_t i = 0; i < Dimension; ++i) {
      min_[i] = std::min(min_[i], p[i]);
      max_[i] = std::max(max_[i], p[i]);
    }
  }

  void clamp(dimension_type min, dimension_type max) {
    for(std::size_t i = 0; i < Dimension; ++i) {
      min_[i] = std::max(min_[i], min[i]);
      max_[i] = std::min(max_[i], max[i]);
    }
  }

  dimension_type min() const {
    return min_;
  }

  dimension_type max() const {
    return max_;
  };

 private:
  dimension_type min_;
  dimension_type max_;
}; // namespace swgl

} // namespace swgl

#endif // SWGL_GEOMETRY_BBOX_HPP