//
// swgl/image.cpp
//
// Copyright (c) Chris Glover, 2018
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include "swgl/image.hpp"
#include "swgl/colour.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <math.h>
#include <string.h>
#include <time.h>
#include <cstring>

#pragma pack(push, 1)
struct TGA_Header {
  char idlength;
  char colormaptype;
  char datatypecode;
  short colormaporigin;
  short colormaplength;
  char colormapdepth;
  short x_origin;
  short y_origin;
  short width;
  short height;
  char bitsperpixel;
  char imagedescriptor;
};
#pragma pack(pop)

namespace swgl {

image::image()
    : data_(NULL)
    , width_(0)
    , height_(0)
    , bytespp_(0) {
}

image::image(int w, int h, int bpp)
    : data_(NULL)
    , width_(w)
    , height_(h)
    , bytespp_(bpp) {
  unsigned long nbytes = width_ * height_ * bytespp_;
  data_                = new unsigned char[nbytes];
  std::memset(data_, 0, nbytes);
}

image::image(const image& img)
    : data_(NULL)
    , width_(img.width_)
    , height_(img.height_)
    , bytespp_(img.bytespp_) {
  unsigned long nbytes = width_ * height_ * bytespp_;
  data_                = new unsigned char[nbytes];
  std::memcpy(data_, img.data_, nbytes);
}

image::~image() {
  if(data_)
    delete[] data_;
}

image& image::operator=(const image& img) {
  if(this != &img) {
    if(data_)
      delete[] data_;
    width_               = img.width_;
    height_              = img.height_;
    bytespp_             = img.bytespp_;
    unsigned long nbytes = width_ * height_ * bytespp_;
    data_                = new unsigned char[nbytes];
    std::memcpy(data_, img.data_, nbytes);
  }
  return *this;
}

image::colour_type image::get(int x, int y) const {
  assert(!(!data_ || x < 0 || y < 0 || x >= width_ || y >= height_));
  return colour_type(data_ + (x + y * width_) * bytespp_, bytespp_);
}

image::colour_type image::sample(float u, float v) const {
  return get(static_cast<int>(u * width_), static_cast<int>(v * height_));
}

void image::set(int x, int y, colour_type const& c) {
  assert(!(!data_ || x < 0 || y < 0 || x >= width_ || y >= height_));
  std::copy(c.data(), c.data() + bytespp_, data_ + (x + y * width_) * bytespp_);
}

int image::bytespp() const {
  return bytespp_;
}

int image::width() const {
  return width_;
}

int image::height() const {
  return height_;
}

bool image::read_tga_file(const char* filename) {
  if(data_)
    delete[] data_;
  data_ = NULL;
  std::ifstream in;
  in.open(filename, std::ios::binary);
  if(!in.is_open()) {
    std::cerr << "can't open file " << filename << "\n";
    in.close();
    return false;
  }
  TGA_Header header;
  in.read((char*)&header, sizeof(header));
  if(!in.good()) {
    in.close();
    std::cerr << "an error occured while reading the header\n";
    return false;
  }
  width_   = header.width;
  height_  = header.height;
  bytespp_ = header.bitsperpixel >> 3;
  if(width_ <= 0 || height_ <= 0 ||
     (bytespp_ != GRAYSCALE && bytespp_ != RGB && bytespp_ != RGBA)) {
    in.close();
    std::cerr << "bad bpp (or width/height) value\n";
    return false;
  }
  unsigned long nbytes = bytespp_ * width_ * height_;
  data_                = new unsigned char[nbytes];
  if(3 == header.datatypecode || 2 == header.datatypecode) {
    in.read((char*)data_, nbytes);
    if(!in.good()) {
      in.close();
      std::cerr << "an error occured while reading the data\n";
      return false;
    }
  }
  else if(10 == header.datatypecode || 11 == header.datatypecode) {
    if(!load_rle_data(in)) {
      in.close();
      std::cerr << "an error occured while reading the data\n";
      return false;
    }
  }
  else {
    in.close();
    std::cerr << "unknown file format " << (int)header.datatypecode << "\n";
    return false;
  }

  if(bytespp_ >= 3) {
    for(int i = 0; i < width_ * height_ * bytespp_; i += bytespp_) {
      std::swap(data_[i], data_[i + 2]);
    }
  }
  if(!(header.imagedescriptor & 0x20)) {
    flip_vertically();
  }
  if(header.imagedescriptor & 0x10) {
    flip_horizontally();
  }
  std::cerr << width_ << "x" << height_ << "/" << bytespp_ * 8 << "\n";
  in.close();
  return true;
}

bool image::load_rle_data(std::istream& in) {
  unsigned long pixelcount   = width_ * height_;
  unsigned long currentpixel = 0;
  unsigned long currentbyte  = 0;
  colour_type colorbuffer;
  do {
    unsigned char chunkheader = 0;
    chunkheader               = in.get();
    if(!in.good()) {
      std::cerr << "an error occured while reading the data_\n";
      return false;
    }
    if(chunkheader < 128) {
      chunkheader++;
      for(int i = 0; i < chunkheader; i++) {
        in.read(reinterpret_cast<char*>(colorbuffer.data()), bytespp_);
        if(!in.good()) {
          std::cerr << "an error occured while reading the header\n";
          return false;
        }
        for(int t = 0; t < bytespp_; t++)
          data_[currentbyte++] = colorbuffer[t];
        currentpixel++;
        if(currentpixel > pixelcount) {
          std::cerr << "Too many pixels read\n";
          return false;
        }
      }
    }
    else {
      chunkheader -= 127;
      in.read(reinterpret_cast<char*>(colorbuffer.data()), bytespp_);
      if(!in.good()) {
        std::cerr << "an error occured while reading the header\n";
        return false;
      }
      for(int i = 0; i < chunkheader; i++) {
        for(int t = 0; t < bytespp_; t++)
          data_[currentbyte++] = colorbuffer[t];
        currentpixel++;
        if(currentpixel > pixelcount) {
          std::cerr << "Too many pixels read\n";
          return false;
        }
      }
    }
  } while(currentpixel < pixelcount);
  return true;
}

bool image::write_tga_file(const char* filename, bool rle) {
  unsigned char developer_area_ref[4] = {0, 0, 0, 0};
  unsigned char extension_area_ref[4] = {0, 0, 0, 0};
  unsigned char footer[18] = {'T', 'R', 'U', 'E', 'V', 'I', 'S', 'I', 'O',
                              'N', '-', 'X', 'F', 'I', 'L', 'E', '.', '\0'};
  std::ofstream out;
  out.open(filename, std::ios::binary);
  if(!out.is_open()) {
    std::cerr << "can't open file " << filename << "\n";
    out.close();
    return false;
  }
  TGA_Header header;
  memset((void*)&header, 0, sizeof(header));
  header.bitsperpixel = bytespp_ << 3;
  header.width        = width_;
  header.height       = height_;
  header.datatypecode =
      (bytespp_ == GRAYSCALE ? (rle ? 11 : 3) : (rle ? 10 : 2));
  header.imagedescriptor = 0x20; // top-left origin
  out.write((char*)&header, sizeof(header));
  if(!out.good()) {
    out.close();
    std::cerr << "can't dump the tga file\n";
    return false;
  }
  if(!rle) {
    out.write((char*)data_, width_ * height_ * bytespp_);
    if(!out.good()) {
      std::cerr << "can't unload raw data_\n";
      out.close();
      return false;
    }
  }
  else {
    if(!unload_rle_data(out)) {
      out.close();
      std::cerr << "can't unload rle data_\n";
      return false;
    }
  }
  out.write((char*)developer_area_ref, sizeof(developer_area_ref));
  if(!out.good()) {
    std::cerr << "can't dump the tga file\n";
    out.close();
    return false;
  }
  out.write((char*)extension_area_ref, sizeof(extension_area_ref));
  if(!out.good()) {
    std::cerr << "can't dump the tga file\n";
    out.close();
    return false;
  }
  out.write((char*)footer, sizeof(footer));
  if(!out.good()) {
    std::cerr << "can't dump the tga file\n";
    out.close();
    return false;
  }
  out.close();
  return true;
}

// TODO: it is not necessary to break a raw chunk for two equal pixels (for the
// matter of the resulting size)
bool image::unload_rle_data(std::ostream& out) {
  const unsigned char max_chunk_length = 128;
  unsigned long npixels                = width_ * height_;
  unsigned long curpix                 = 0;
  while(curpix < npixels) {
    unsigned long chunkstart = curpix * bytespp_;
    unsigned long curbyte    = curpix * bytespp_;
    unsigned char run_length = 1;
    bool raw                 = true;
    while(curpix + run_length < npixels && run_length < max_chunk_length) {
      bool succ_eq = true;
      for(int t = 0; succ_eq && t < bytespp_; t++) {
        succ_eq = (data_[curbyte + t] == data_[curbyte + t + bytespp_]);
      }
      curbyte += bytespp_;
      if(1 == run_length) {
        raw = !succ_eq;
      }
      if(raw && succ_eq) {
        run_length--;
        break;
      }
      if(!raw && !succ_eq) {
        break;
      }
      run_length++;
    }
    curpix += run_length;
    out.put(raw ? run_length - 1 : run_length + 127);
    if(!out.good()) {
      std::cerr << "can't dump the tga file\n";
      return false;
    }
    out.write(
        (char*)(data_ + chunkstart), (raw ? run_length * bytespp_ : bytespp_));
    if(!out.good()) {
      std::cerr << "can't dump the tga file\n";
      return false;
    }
  }
  return true;
}

bool image::flip_horizontally() {
  if(!data_)
    return false;
  int half = width_ >> 1;
  for(int i = 0; i < half; i++) {
    for(int j = 0; j < height_; j++) {
      colour_type c1 = get(i, j);
      colour_type c2 = get(width_ - 1 - i, j);
      set(i, j, c2);
      set(width_ - 1 - i, j, c1);
    }
  }
  return true;
}

bool image::flip_vertically() {
  if(!data_)
    return false;
  unsigned long bytes_per_line = width_ * bytespp_;
  unsigned char* line          = new unsigned char[bytes_per_line];
  int half                     = height_ >> 1;
  for(int j = 0; j < half; j++) {
    unsigned long l1 = j * bytes_per_line;
    unsigned long l2 = (height_ - 1 - j) * bytes_per_line;
    memmove((void*)line, (void*)(data_ + l1), bytes_per_line);
    memmove((void*)(data_ + l1), (void*)(data_ + l2), bytes_per_line);
    memmove((void*)(data_ + l2), (void*)line, bytes_per_line);
  }
  delete[] line;
  return true;
}

unsigned char const* image::data() const {
  return data_;
}

void image::clear() {
  memset((void*)data_, 0, width_ * height_ * bytespp_);
}

void image::clear(colour_type const& c) {
  std::size_t k = 0;
  for(std::size_t i = 0; i < width_ * height_; ++i) {
    for(std::size_t j = 0; j < bytespp_; ++j) {
      data_[k++] = c[j];
    }
  }
}

bool image::scale(int w, int h) {
  if(w <= 0 || h <= 0 || !data_)
    return false;
  unsigned char* tdata_    = new unsigned char[w * h * bytespp_];
  int nscanline            = 0;
  int oscanline            = 0;
  int erry                 = 0;
  unsigned long nlinebytes = w * bytespp_;
  unsigned long olinebytes = width_ * bytespp_;
  for(int j = 0; j < height_; j++) {
    int errx = width_ - w;
    int nx   = -bytespp_;
    int ox   = -bytespp_;
    for(int i = 0; i < width_; i++) {
      ox += bytespp_;
      errx += w;
      while(errx >= (int)width_) {
        errx -= width_;
        nx += bytespp_;
        memcpy(tdata_ + nscanline + nx, data_ + oscanline + ox, bytespp_);
      }
    }
    erry += h;
    oscanline += olinebytes;
    while(erry >= (int)height_) {
      if(erry >= (int)height_ << 1) // it means we jump over a scanline
        memcpy(tdata_ + nscanline + nlinebytes, tdata_ + nscanline, nlinebytes);
      erry -= height_;
      nscanline += nlinebytes;
    }
  }
  delete[] data_;
  data_   = tdata_;
  width_  = w;
  height_ = h;
  return true;
}

} // namespace swgl
