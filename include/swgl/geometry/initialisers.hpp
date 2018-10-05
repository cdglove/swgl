//
// swgl/geometry/initialisers.hpp
//
// Copyright (c) Chris Glover, 2018
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SWGL_GEOMETRY_INITIALISERS_HPP
#define SWGL_GEOMETRY_INITIALISERS_HPP
#pragma once

namespace swgl { namespace init {
struct one_t {};
struct zero_t {};
struct uninitialised_t {};
struct identity_t {};

constexpr one_t one;
constexpr zero_t zero;
constexpr uninitialised_t uninitialised;
constexpr identity_t identity;
}}     // namespace swgl::init
#endif // SWGL_GEOMETRY_INITIALISERS_HPP