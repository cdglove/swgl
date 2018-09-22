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

#include <algorithm>

namespace swgl {

namespace detail {

template <typename T, std::size_t Dim>
struct bbox_dimension;

template <typename T>
struct bbox_dimension<T, 2> {
  using type = vector2<T>;
};

template <typename T>
struct bbox_dimension<T, 3> {
  using type = vector3<T>;
};

} // namespace detail

template <typename T, std::size_t Dimension>
class bbox {
 public:
  using dimension_type = typename detail::bbox_dimension<T, Dimension>::type;

  bbox(dimension_type const& p)
      : min_(p)
      , max_(p) {
  }

  bbox(dimension_type const* points, std::size_t num_points) {
	  for(std::size_t i = 0; i < Dimension; ++i) {
		auto min_max = std::minmax_element(points, points + num_points, 
			[&i](dimension_type const& a, dimension_type const& b) {
				return a[i] < b[i];
		});
		min_[i] = (*min_max.first)[i];
		max_[i] = (*min_max.second)[i];
	  }
  }

  bbox(dimension_type min, dimension_type max) {
    for(std::size_t i = 0; i < Dimension; ++i) {
      assert(first[i] <= second[i]);
    }

	min_ = min;
	max_ = max;
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