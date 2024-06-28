#pragma once

#include "spaceship/map.h"

#include "common/geometry/d2/base.h"

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
};
}  // namespace spaceship
