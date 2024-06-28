#pragma once

#include "common/geometry/d2/vector.h"

namespace spaceship {
static I2Vector C2V(char c) {
  auto i = int(c - '1');
  return I2Vector((i % 3) - 1, (i / 3) - 1);
}

static char V2C(const I2Vector& v) { return '1' + char(v.dx + 3 * v.dy + 4); }
}  // namespace spaceship
