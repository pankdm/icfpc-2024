#pragma once

#include "spaceship/map.h"

#include "common/base.h"
#include "common/geometry/d2/axis/rectangle.h"
#include "common/geometry/d2/base.h"
#include "common/geometry/d2/stl_hash/point.h"
#include "common/geometry/d2/stl_hash/vector.h"
#include "common/hash.h"
#include "common/numeric/utils/abs.h"
#include "common/stl/hash/vector.h"

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

  I2ARectangle PossibleLocations(int64_t s) const {
    auto d = (s * (s + 1)) / 2;
    return I2ARectangle({p.x + s * v.dx - d, p.y + s * v.dy - d},
                        {p.x + s * v.dx + d, p.y + s * v.dy + d});
  };

  static int64_t MaxDistance(int64_t speed, int64_t s, int64_t msas) {
    assert(speed <= s + msas);
    if (speed + s <= msas) {
      return speed * s + (s * (s + 1)) / 2;
    } else {
      auto extra = s + msas - speed;
      auto h1 = extra / 2, h2 = s - h1 - (extra % 2);
      auto ms = speed + h1;
      auto r = ((speed + ms + 1) * h1) / 2 + ms * (extra % 2) +
               ((msas + ms - 1) * h2) / 2;
      return r;
    }
  }

  I2ARectangle PossibleLocations(int64_t s, int64_t msas) const {
    return I2ARectangle(
        {p.x - MaxDistance(-v.dx, s, msas), p.y - MaxDistance(-v.dy, s, msas)},
        {p.x + MaxDistance(v.dx, s, msas), p.y + MaxDistance(v.dy, s, msas)});
  };

  size_t Hash() const {
    return std::hash<std::vector<int64_t>>{}(
        std::vector<int64_t>{p.x, p.y, v.dx, v.dy, 17, p.x, p.y, v.dx, v.dy});
  }

  bool operator==(const SpaceShip& r) const { return (p == r.p) && (v == r.v); }
  bool operator!=(const SpaceShip& r) const { return (p != r.p) || (v != r.v); }
};
}  // namespace spaceship
