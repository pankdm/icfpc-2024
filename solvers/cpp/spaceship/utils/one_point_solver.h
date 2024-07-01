#pragma once

#include "spaceship/map.h"

#include "common/base.h"
#include "common/geometry/d2/base.h"
#include "common/geometry/d2/point.h"
#include "common/geometry/d2/vector.h"
#include "common/numeric/utils/abs.h"

#include <iostream>
#include <utility>

namespace spaceship {
class OnePointSolver {
 public:
  static int64_t MaxDistance(unsigned s) { return (s * (s + 1)) / 2; }

  static unsigned MinSteps(const I2Vector& v, const I2Point& p) {
    if ((p.x == 0) && (p.y == 0)) return 0;
    for (unsigned s = 0;; ++s) {
      auto pp = p - v * s;
      auto d = MaxDistance(s);
      if ((pp.x >= -d) && (pp.x <= d) && (pp.y >= -d) && (pp.y <= d)) return s;
    }
    assert(false);
    return 10000000;
  }

  static std::pair<char, unsigned> Solve(const I2Vector& v, const I2Point& p) {
    auto s = MinSteps(v, p);
    assert(s > 0);
    for (int idx = -1; idx <= 1; ++idx) {
      for (int idy = -1; idy <= 1; ++idy) {
        I2Vector idv(idx, idy);
        auto v2 = v + idv;
        if (MinSteps(v2, p - v2) == s - 1) {
          return {V2C(idv), s};
        }
      }
    }
    std::cout << "OPS: Best move was not found." << std::endl;
    return {'5', 0};
  }

  static char BestMove(const I2Vector& v, const I2Point& p) {
    return Solve(v, p).first;
  }

  static std::string GetPath(const I2Vector& v, const I2Point& p) {
    if (p == I2Point()) return "";
    auto cv = v;
    auto cp = p;
    std::string output;
    for (auto s = MinSteps(v, p); s-- > 0;) {
      auto d = MaxDistance(s);
      cp -= cv;
      bool found = false;
      for (int idx = -1; (idx <= 1) && !found; ++idx) {
        for (int idy = -1; (idy <= 1) && !found; ++idy) {
          I2Vector idv(idx, idy);
          cp -= idv;
          cv += idv;
          if ((Abs(cp.x - cv.dx * s) <= d) && (Abs(cp.y - cv.dy * s) <= d)) {
            found = true;
            output += V2C(idv);
          } else {
            cp += idv;
            cv -= idv;
          }
        }
      }
      if (!found) {
        std::cout << "Ops" << std::endl;
      }
    }
    return output;
  }
};
}  // namespace spaceship
