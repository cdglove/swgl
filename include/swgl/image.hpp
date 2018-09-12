//
// swgl/image.hpp
//
// Copyright (c) Chris Glover, 2018
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SWGL_IMAGE_HPP
#define SWGL_IMAGE_HPP

#include <iosfwd>
#include <cassert>
#include <cstdint>
#include "swgl/colour.hpp"

namespace swgl {

template<typename T>
class colour;

class image {
 public:
  enum Format { GRAYSCALE = 1, RGB = 3, RGBA = 4 };

  using colour_type = colour<std::uint8_t>;

  image();
  image(int w, int h, int bpp);
  image(const image& img);
  bool read_tga_file(const char* filename);
  bool write_tga_file(const char* filename, bool rle = true);
  bool flip_horizontally();
  bool flip_vertically();
  bool scale(int w, int h);
  colour_type get(int x, int y) const;
  colour_type sample(float u, float v) const;
  void set(int x, int y, colour_type const& c);
  ~image();
  image& operator=(const image& img);
  int width() const;
  int height() const;
  int bytespp() const;
  unsigned char const* data() const;
  void clear();
  void clear(colour_type const& c);
 private:
  unsigned char* data_;
  int width_;
  int height_;
  int bytespp_;

  bool load_rle_data(std::istream& in);
  bool unload_rle_data(std::ostream& out);
};

} // namespace swgl

#endif // SWGL_IMAGE_HPP
