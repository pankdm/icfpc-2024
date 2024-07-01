#pragma once

#include "spaceship/map.h"
#include "spaceship/solvers/base.h"
#include "spaceship/solvers/line_sweep2.h"
#include "spaceship/spaceship.h"
#include "spaceship/utils/construct_line.h"
#include "spaceship/utils/one_point_solver.h"
#include "spaceship/utils/drop_dups.h"

#include "common/assert_exception.h"
#include "common/geometry/d2/point_io.h"
#include "common/geometry/d2/vector_io.h"
#include "common/geometry/d2/stl_hash/vector.h"
#include "common/hash.h"
#include "common/heap.h"
#include "common/numeric/utils/abs.h"
#include "common/solvers/solver.h"
#include "common/timer.h"
#include "common/vector/union.h"

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

namespace spaceship {
class LineSweep2B : public LineSweep2 {
 public:
  using TBase = LineSweep2;
  using PSolver = TBase::PSolver;

 protected:
  unsigned max_steps_between_points;

 public:
  LineSweep2B() : LineSweep2() {}
  LineSweep2B(unsigned _max_time, unsigned _max_steps_between_points, unsigned _max_extra) :
    LineSweep2(_max_time, _max_steps_between_points, _max_extra) {}

  PSolver Clone() const override {
    return std::make_shared<LineSweep2B>(*this);
  }

  std::string Name() const override { return "ls2b"; }

  bool SkipSolutionRead() const override { return true; }
  // bool SkipBest() const override { return true; }

  Solution Solve(const TProblem& p) override {
    Solution s;
    s.SetId(p.Id());

    TProblem pl;
    for (int i = -1; i < 30; ++i) {
      if (pl.Load(p.Id(), "../../problems/spaceship_lkh_euc/spaceship" + p.Id() + ".txt" + ((i >= 0) ? (".v" + std::to_string(i)) : ""))) {
      // if (pl.Load(p.Id(), "../../problems/spaceship_lkh_max/spaceship" + p.Id() + ".txt" + ((i >= 0) ? (".v" + std::to_string(i)) : ""))) {
        auto line = DropDups(pl.GetPoints(), true);
        auto st = SolveI(line, max_time_in_seconds);
        if (!st.empty() && (s.commands.empty() || (s.commands.size() > st.size()))) {
          s.commands = st;
        }
      }
    }

    std::cout << p.Id() << "\t" << p.GetPoints().size() << "\t" << s.commands.size() << std::endl;
    return s;
  }
};
}  // namespace spaceship
