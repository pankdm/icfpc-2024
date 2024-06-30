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
class Greedy2B : public BaseSolver {
 public:
  using TBase = BaseSolver;
  using PSolver = TBase::PSolver;

 public:
  Greedy2B() : BaseSolver() {}
  explicit Greedy2B(unsigned _max_time) : BaseSolver(_max_time) {}

  PSolver Clone() const override { return std::make_shared<Greedy2B>(*this); }

  std::string Name() const override { return "greedy2b"; }

  // bool SkipSolutionRead() const override { return true; }
  // bool SkipBest() const override { return true; }

  static std::string GetPath(SpaceShip& ss, const I2Point& dest, unsigned s) {
    std::string sr;
    for (; s-- > 0;) {
      ss.p += ss.v;
      bool found = false;
      I2Vector best_idv;
      int64_t best_idv_score = 0;
      for (int idx = -1; (idx <= 1) && !found; ++idx) {
        for (int idy = -1; (idy <= 1) && !found; ++idy) {
          I2Vector idv(idx, idy);
          ss.p += idv;
          ss.v += idv;
          if (ss.PossibleLocations(s).Inside(dest)) {
            if (!found) {
              found = true;
              best_idv = idv;
              best_idv_score = ss.v.LengthSquared();
            } else if (ss.v.LengthSquared() > best_idv_score) {
              best_idv = idv;
              best_idv_score = ss.v.LengthSquared();
            }
          }
          ss.p -= idv;
          ss.v -= idv;
        }
      }
      if (found) {
        ss.p += best_idv;
        ss.v += best_idv;
        sr += V2C(best_idv);
      } else {
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
