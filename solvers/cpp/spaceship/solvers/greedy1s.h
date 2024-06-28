#pragma once

#include "spaceship/map.h"
#include "spaceship/solvers/base.h"
#include "spaceship/solvers/greedy1.h"

#include "common/geometry/d2/compare/point_xy.h"
#include "common/geometry/d2/compare/point_yx.h"
#include "common/solvers/solver.h"

#include <algorithm>
#include <string>
#include <vector>

namespace spaceship {
class Greedy1S : public BaseSolver {
 public:
  using TBase = BaseSolver;
  using PSolver = TBase::PSolver;

 public:
  Greedy1S() : BaseSolver() {}
  explicit Greedy1S(unsigned _max_time) : BaseSolver(_max_time) {}

  PSolver Clone() const override { return std::make_shared<Greedy1S>(*this); }

  std::string Name() const override { return "greedy1s"; }

  // bool SkipSolutionRead() const override { return true; }
  // bool SkipBest() const override { return true; }

  Solution Solve(const TProblem& p) override {
    Solution s;
    s.SetId(p.Id());
    auto vp = p.GetPoints();
    std::sort(vp.begin(), vp.end(), CompareXY<int64_t>);
    auto s1 = Greedy1::SolveI(vp);
    std::sort(vp.begin(), vp.end(), CompareYX<int64_t>);
    auto s2 = Greedy1::SolveI(vp);
    s.commands = (s1.size() <= s2.size()) ? s1 : s2;
    std::cout << p.Id() << "\t" << p.GetPoints().size() << "\t"
              << s.commands.size() << std::endl;
    return s;
  }
};
}  // namespace spaceship
