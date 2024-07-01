#pragma once

#include "common/files/file_to_string.h"
#include "common/geometry/d2/point.h"
#include "common/geometry/d2/point_io.h"
#include "common/solvers/problem.h"
#include "common/vector/read_from_string.h"

#include <vector>

namespace spaceship {
class Problem : public solvers::Problem {
 protected:
  std::vector<I2Point> points;

 public:
  const std::vector<I2Point>& GetPoints() const { return points; }

  bool Load(const std::string& _id) {
    return Load(_id, "../../problems/spaceship/spaceship" + _id + ".txt");
    // return Load(_id, "../../problems/spaceship_christofides_serdyukov/spaceship" + _id + ".txt");
  }

  bool Load(const std::string& _id, const std::string& filename) {
    id = _id;
    points =
        nvector::ReadFromString<I2Point>(files::FileToString(filename), "\n");
    return !points.empty();
  }
};
}  // namespace spaceship
