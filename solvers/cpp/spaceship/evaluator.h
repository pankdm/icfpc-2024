#pragma once

#include "spaceship/constants.h"
#include "spaceship/map.h"
#include "spaceship/problem.h"
#include "spaceship/solution.h"
#include "spaceship/spaceship.h"

#include "common/geometry/d2/point.h"
#include "common/geometry/d2/stl_hash/point.h"
#include "common/solvers/evaluator.h"

#include <unordered_set>

namespace spaceship {
class Evaluator : public solvers::Evaluator {
 public:
  using Result = solvers::Evaluator::Result;

  static bool Compare(const Result& l, const Result& r) {
    return l.correct ? r.correct ? l.score < r.score : true : false;
  }

  // Check if solution valid
  static bool Valid(const Problem& p, const Solution& s) {
    if (s.commands.size() > max_moves) return false;
    std::unordered_set<I2Point> ps;
    for (const auto& pp : p.GetPoints()) {
      ps.insert(pp);
    }
    SpaceShip ss;
    for (auto c : ("5" + s.commands)) {
      ss.ApplyCommand(c);
      auto it = ps.find(ss.p);
      if (it != ps.end()) {
        ps.erase(it);
      }
    }
    return ps.empty();
  }

  static Result Apply(const Problem& p, const Solution& s) {
    return Valid(p, s) ? Result(true, s.commands.size()) : Result(false, 0);
  }
};
}  // namespace spaceship
