#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class t>
struct Vec2 {
  union {
    struct {
      t u, v;
    };
    struct {
      t x, y;
    };
    t raw[2];
  };
  Vec2()
      : u(0)
      , v(0) {
  }
  Vec2(t _u, t _v)
      : u(_u)
      , v(_v) {
  }

  template <typename U>
  explicit Vec2(Vec2<U> const& other)
      : x(static_cast<t>(other.x))
      , y(static_cast<t>(other.y)) {
  }

  Vec2<t> operator+(const Vec2<t>& V) const {
    return Vec2<t>(u + V.u, v + V.v);
  }
  Vec2<t> operator-(const Vec2<t>& V) const {
    return Vec2<t>(u - V.u, v - V.v);
  }
  Vec2<t> operator*(float f) const {
    return Vec2<t>(static_cast<t>(u * f), static_cast<t>(v * f));
  }
  t operator[](std::size_t i) const {
    return raw[i];
  }
  t& operator[](std::size_t i) {
    return raw[i];
  }
  template <class>
  friend std::ostream& operator<<(std::ostream& s, Vec2<t>& v);
};

template <class t>
struct Vec3 {
  union {
    struct {
      t x, y, z;
    };
    struct {
      t ivert, iuv, inorm;
    };
    t raw[3];
  };
  Vec3()
      : x(0)
      , y(0)
      , z(0) {
  }
  Vec3(t _x, t _y, t _z)
      : x(_x)
      , y(_y)
      , z(_z) {
  }

  template <typename U>
  explicit Vec3(Vec3<U> const& other)
      : x(static_cast<t>(other.x))
      , y(static_cast<t>(other.y))
      , z(static_cast<t>(other.z)) {
  }

  Vec3<t> operator+(const Vec3<t>& v) const {
    return Vec3<t>(x + v.x, y + v.y, z + v.z);
  }
  Vec3<t> operator-(const Vec3<t>& v) const {
    return Vec3<t>(x - v.x, y - v.y, z - v.z);
  }
  Vec3<t> operator*(float f) const {
    return Vec3<t>(
        static_cast<t>(x * f), static_cast<t>(y * f), static_cast<t>(z * f));
  }
  t operator[](std::size_t i) const {
    return raw[i];
  }
  t& operator[](std::size_t i) {
    return raw[i];
  }
  float norm() const {
    return std::sqrt(x * x + y * y + z * z);
  }
  Vec3<t>& normalize(t l = 1) {
    *this = (*this) * (l / norm());
    return *this;
  }
  template <class>
  friend std::ostream& operator<<(std::ostream& s, Vec3<t>& v);
};

template <typename T>
Vec3<T> cross(Vec3<T> v1, Vec3<T> v2) {
  return Vec3<T>(
      // clang-format off
      v1.y * v2.z - v1.z * v2.y, 
      v1.z * v2.x - v1.x * v2.z,
      v1.x * v2.y - v1.y * v2.x
      // clang-format on
  );
}

template <typename T>
T dot(Vec3<T> v1, Vec3<T> v2) {
  return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;
using Vec3f = Vec3<float>;
using Vec3i = Vec3<int>;

template <class t>
std::ostream& operator<<(std::ostream& s, Vec2<t>& v) {
  s << "(" << v.x << ", " << v.y << ")\n";
  return s;
}

template <class t>
std::ostream& operator<<(std::ostream& s, Vec3<t>& v) {
  s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
  return s;
}

#endif //__GEOMETRY_H__
