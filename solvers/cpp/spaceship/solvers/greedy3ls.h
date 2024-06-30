#pragma once

#include "spaceship/map.h"
#include "spaceship/solvers/base.h"
#include "spaceship/solvers/greedy3.h"
#include "spaceship/spaceship.h"
#include "spaceship/utils/construct_line.h"
#include "spaceship/utils/drop_dups.h"

#include "common/solvers/solver.h"

#include <algorithm>
#include <string>
#include <vector>

namespace spaceship {
class Greedy3LS : public BaseSolver {
 public:
  using TBase = BaseSolver;
  using PSolver = TBase::PSolver;

 public:
  Greedy3LS() : BaseSolver() {}
  explicit Greedy3LS(unsigned _max_time) : BaseSolver(_max_time) {}

  PSolver Clone() const override { return std::make_shared<Greedy3LS>(*this); }

  std::string Name() const override { return "greedy3ls"; }

  // bool SkipSolutionRead() const override { return true; }
  // bool SkipBest() const override { return true; }

  static std::string SolveI(const std::vector<I2Point>& line, unsigned max_speed_at_stop) {
    std::string sr;
    SpaceShip ss;
    for (unsigned il = 0; il < line.size(); ++il) {
      if (il + 1 == line.size()) max_speed_at_stop = 10000u; // No reason to limit for last point
      for (unsigned s = 0;; ++s) {
        auto b = ss.PossibleLocations(s, max_speed_at_stop);
        if (b.Inside(line[il])) {
          sr += Greedy3::GetPath(ss, line[il], s, max_speed_at_stop);
          break;
        }
      }
    }
    return sr;
  }

  Solution Solve(const TProblem& p) override {
    Solution s;
    s.SetId(p.Id());
    auto tvp = DropDups(p.GetPoints());
    auto line = ConstructLine(tvp);
    auto line2 = line;
    std::reverse(line2.begin(), line2.end());

    std::vector<unsigned> vmsas{0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
                                10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                                20, 22, 24, 26, 28, 30, 33, 36, 39, 42,
                                46, 50, 55, 60, 66, 72, 79, 86, 94, 103};
    for (auto msas : vmsas) {
      for (const auto& l : {line, line2}) {
        auto sl = SolveI(l, msas);
        if (!sl.empty() && (s.commands.empty() || (s.commands.size() > sl.size()))) {
          s.commands = sl;
        }
      }
    }                                

    std::cout << p.Id() << "\t" << p.GetPoints().size() << "\t"
              << s.commands.size() << std::endl;
    return s;
  }
};
}  // namespace spaceship
