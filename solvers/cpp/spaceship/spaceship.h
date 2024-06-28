#pragma once

#include "spaceship/map.h"

#include "common/geometry/d2/base.h"
#include "common/geometry/d2/stl_hash/point.h"
#include "common/geometry/d2/stl_hash/vector.h"
#include "common/hash.h"

namespace spaceship {
class SpaceShip {
 public:
  I2Point p;
  I2Vector v;

 public:
  void ApplyCommand(char c) { ApplyCommand(C2V(c)); }

  void ApplyCommand(const I2Vector& _v) {
    v += _v;
    p += v;
  }

  size_t Hash() const {
    return HashCombine(std::hash<I2Point>{}(p), std::hash<I2Vector>{}(v));
  }
};
}  // namespace spaceship
