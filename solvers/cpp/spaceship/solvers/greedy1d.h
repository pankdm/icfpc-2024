#pragma once

#include "spaceship/map.h"
#include "spaceship/solvers/base.h"
#include "spaceship/solvers/greedy1.h"

#include "common/geometry/d2/distance/distance_linf.h"
#include "common/solvers/solver.h"

#include <algorithm>
#include <string>
#include <vector>

namespace spaceship {
class Greedy1D : public BaseSolver {
 public:
  using TBase = BaseSolver;
  using PSolver = TBase::PSolver;

 public:
  Greedy1D() : BaseSolver() {}
  explicit Greedy1D(unsigned _max_time) : BaseSolver(_max_time) {}

  PSolver Clone() const override { return std::make_shared<Greedy1D>(*this); }

  std::string Name() const override { return "greedy1d"; }

  // bool SkipSolutionRead() const override { return true; }
  // bool SkipBest() const override { return true; }

  Solution Solve(const TProblem& p) override {
    Solution s;
    s.SetId(p.Id());
    I2Point last;
    auto vp = p.GetPoints();
    for (; !vp.empty();) {
      for (unsigned i = 0; i < vp.size() - 1; ++i) {
        if (DistanceLInf(last, vp[i]) < DistanceLInf(last, vp.back())) {
          std::swap(vp[i], vp.back());
        }
      }
      s.commands += Greedy1::RunAndStopS(vp.back() - last);
      last = vp.back();
      vp.pop_back();
    }
    std::cout << p.Id() << "\t" << p.GetPoints().size() << "\t"
              << s.commands.size() << std::endl;
    return s;
  }
};
}  // namespace spaceship
