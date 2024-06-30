#pragma once

#include "spaceship/map.h"
#include "spaceship/solvers/base.h"
#include "spaceship/spaceship.h"
#include "spaceship/utils/drop_dups.h"

#include "common/geometry/d2/distance/distance_linf.h"
#include "common/solvers/solver.h"

#include <algorithm>
#include <random>
#include <string>
#include <vector>

namespace spaceship {
class Greedy3 : public BaseSolver {
 public:
  using TBase = BaseSolver;
  using PSolver = TBase::PSolver;

 protected:
  int64_t max_speed_at_stop;

 public:
  Greedy3() : BaseSolver() {}
  explicit Greedy3(unsigned _max_speed_at_stop)
      : BaseSolver(0), max_speed_at_stop(_max_speed_at_stop) {}

  PSolver Clone() const override { return std::make_shared<Greedy3>(*this); }

  std::string Name() const override { return "greedy3"; }

  bool SkipSolutionRead() const override { return true; }
  // bool SkipBest() const override { return true; }

  std::string GetPath(SpaceShip& ss, const I2Point& dest, unsigned s) const {
    std::string sr;
    for (; s-- > 0;) {
      ss.p += ss.v;
      bool found = false;
      for (int idx = -1; (idx <= 1) && !found; ++idx) {
        for (int idy = -1; (idy <= 1) && !found; ++idy) {
          I2Vector idv(idx, idy);
          ss.p += idv;
          ss.v += idv;
          if ((Abs(ss.v.dx) <= max_speed_at_stop + s) &&
              (Abs(ss.v.dy) <= max_speed_at_stop + s) &&
              ss.PossibleLocations(s, max_speed_at_stop).Inside(dest)) {
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

  std::string SolveI(const std::vector<I2Point>& tvp) const {
    std::string sr;
    SpaceShip ss;
    auto vp = tvp;
    for (; !vp.empty();) {
      bool found = false;
      for (unsigned s = 1; !found; ++s) {
        auto b = ss.PossibleLocations(s, max_speed_at_stop);
        auto best_i = tvp.size();
        for (unsigned i = 0; i < vp.size(); ++i) {
          if (b.Inside(vp[i])) {
            found = true;
            if ((best_i >= vp.size()) ||
                (DistanceLInf(ss.p, vp[i]) < DistanceLInf(ss.p, vp[best_i]))) {
              best_i = i;
            }
          }
        }
        if (found) {
          sr += GetPath(ss, vp[best_i], s);
          vp[best_i] = vp.back();
          vp.pop_back();
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

    // Shuffle and use random seeds
    std::shuffle(tvp.begin(), tvp.end(), std::mt19937(17));
    s.commands = SolveI(tvp);

    std::cout << p.Id() << "\t" << p.GetPoints().size() << "\t"
              << s.commands.size() << std::endl;
    return s;
  }
};
}  // namespace spaceship
