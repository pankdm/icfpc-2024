#pragma once

#include "spaceship/map.h"
#include "spaceship/solvers/base.h"
#include "spaceship/spaceship.h"
#include "spaceship/utils/construct_line.h"
#include "spaceship/utils/drop_dups.h"
#include "spaceship/utils/one_point_solver.h"
#include "spaceship/utils/two_points_solver.h"

#include "common/solvers/solver.h"

#include <algorithm>
#include <string>
#include <vector>

namespace spaceship {
class GreedyLS1 : public BaseSolver {
 public:
  using TBase = BaseSolver;
  using PSolver = TBase::PSolver;

 public:
  GreedyLS1() : BaseSolver() {}
  explicit GreedyLS1(unsigned _max_time) : BaseSolver(_max_time) {}

  PSolver Clone() const override { return std::make_shared<GreedyLS1>(*this); }

  std::string Name() const override { return "greedyls1"; }

  bool SkipSolutionRead() const override { return true; }
  // bool SkipBest() const override { return true; }

  static std::string SolveI(const std::vector<I2Point>& line, unsigned max_time_in_seconds) {
    Timer t;
    OnePointSolver ps1;
    thread_local TwoPointsSolver ps2;
    ps2.ResetHashConflicts();
    std::string sr;
    SpaceShip ss;
    unsigned covered = 0;
    unsigned time_per_step = (max_time_in_seconds * 1000) / line.size();
    for (; covered + 1 < line.size();) {
      auto c = (t.GetSeconds() < max_time_in_seconds)
                   ? ps2.BestMove(ss.v, (line[covered] - ss.p).ToPoint(),
                                  (line[covered + 1] - ss.p).ToPoint(),
                                  time_per_step)
                   : ps1.BestMove(ss.v, (line[covered] - ss.p).ToPoint());
      sr += c;
      auto v = C2V(c);
      ss.v += v;
      ss.p += ss.v;
      if (ss.p == line[covered]) {
        ++covered;
      }
    }
    for (; ss.p != line.back(); ) {
      auto c = ps1.BestMove(ss.v, (line.back() - ss.p).ToPoint());
      sr += c;
      auto v = C2V(c);
      ss.v += v;
      ss.p += ss.v;
    }
    std::cout << "\tTPS cache size: " << ps2.CacheSize()
              << "\tHash Conflicts: " << ps2.HashConflicts1()
              << "\t" << ps2.HashConflicts2() << std::endl;
    return sr;
  }

  Solution Solve(const TProblem& p) override {
    Solution s;
    s.SetId(p.Id());
    auto tvp = DropDups(p.GetPoints());
    auto line = ConstructLine(tvp);

    // Solve
    auto s1 = SolveI(line, max_time_in_seconds / 2);
    std::reverse(line.begin(), line.end());
    auto s2 = SolveI(line, max_time_in_seconds / 2);
    s.commands = (s2.empty()                 ? s1
                  : s1.empty()               ? s2 
                  : (s1.size() <= s2.size()) ? s1
                                             : s2);

    std::cout << p.Id() << "\t" << p.GetPoints().size() << "\t"
              << s.commands.size() << std::endl;
    return s;
  }
};
}  // namespace spaceship
