#pragma once

#include "spaceship/map.h"
#include "spaceship/solvers/base.h"
#include "spaceship/spaceship.h"
#include "spaceship/utils/drop_dups.h"

#include "common/solvers/solver.h"

#include <algorithm>
#include <random>
#include <string>
#include <vector>

namespace spaceship {
class Greedy2 : public BaseSolver {
 public:
  using TBase = BaseSolver;
  using PSolver = TBase::PSolver;

 public:
  Greedy2() : BaseSolver() {}
  explicit Greedy2(unsigned _max_time) : BaseSolver(_max_time) {}

  PSolver Clone() const override { return std::make_shared<Greedy2>(*this); }

  std::string Name() const override { return "greedy2"; }

  // bool SkipSolutionRead() const override { return true; }
  // bool SkipBest() const override { return true; }

  static std::string GetPath(SpaceShip& ss, const I2Point& dest, unsigned s) {
    std::string sr;
    for (; s-- > 0;) {
      ss.p += ss.v;
      bool found = false;
      for (int idx = -1; (idx <= 1) && !found; ++idx) {
        for (int idy = -1; (idy <= 1) && !found; ++idy) {
          I2Vector idv(idx, idy);
          ss.p += idv;
          ss.v += idv;
          if (ss.PossibleLocations(s).Inside(dest)) {
            found = true;
            sr += V2C(idv);
          } else {
            ss.p -= idv;
            ss.v -= idv;
          }
        }
      }
      if (!found) {
        std::cout << "Ops" << std::endl;
      }
    }
    return sr;
  }

  static std::string SolveI(const std::vector<I2Point>& tvp) {
    std::string sr;
    SpaceShip ss;
    auto vp = tvp;
    for (; !vp.empty();) {
      bool found = false;
      for (unsigned s = 1; !found; ++s) {
        auto b = ss.PossibleLocations(s);
        for (unsigned i = 0; i < vp.size(); ++i) {
          if (b.Inside(vp[i])) {
            found = true;
            sr += GetPath(ss, vp[i], s);
            vp[i] = vp.back();
            vp.pop_back();
            break;
          }
        }
      }
    }
    return sr;
  }

  Solution Solve(const TProblem& p) override {
    Solution s;
    s.SetId(p.Id());
    auto tvp = DropDups(p.GetPoints());

    // Shuffle and use random seeds
    std::shuffle(tvp.begin(), tvp.end(), std::mt19937(17));
    s.commands = SolveI(tvp);

    std::cout << p.Id() << "\t" << p.GetPoints().size() << "\t"
              << s.commands.size() << std::endl;
    return s;
  }
};
}  // namespace spaceship
