#pragma once

#include "common/geometry/d2/point.h"
#include "common/geometry/d2/compare/point_xy.h"
#include "common/vector/unique.h"

#include <vector>

namespace spaceship {
static std::vector<I2Point> DropDups(const std::vector<I2Point>& vp, bool keep_order = false) {
  std::vector<I2Point> vpnew(vp);
  for (unsigned i = 0; i < vpnew.size(); ++i) {
    if (vpnew[i] == I2Point()) {
      if (keep_order) {
        vpnew.erase(vpnew.begin() + i--);
      } else {
        vpnew[i--] = vpnew.back();
        vpnew.pop_back();
      }
    }
  }

  if (!keep_order) std::sort(vpnew.begin(), vpnew.end(), CompareXY<int64_t>);
  nvector::Unique(vpnew);

  return vpnew;
}
}  // namespace spaceship
