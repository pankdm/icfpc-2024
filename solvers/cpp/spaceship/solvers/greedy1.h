#pragma once

#include "spaceship/map.h"
#include "spaceship/solvers/base.h"

#include "common/solvers/solver.h"

#include <algorithm>
#include <string>
#include <vector>

namespace spaceship {
class Greedy1 : public BaseSolver {
 public:
  using TBase = BaseSolver;
  using PSolver = TBase::PSolver;

 public:
  Greedy1() : BaseSolver() {}
  explicit Greedy1(unsigned _max_time) : BaseSolver(_max_time) {}

  PSolver Clone() const override { return std::make_shared<Greedy1>(*this); }

  std::string Name() const override { return "greedy1"; }

  // bool SkipSolutionRead() const override { return true; }
  // bool SkipBest() const override { return true; }

  static std::vector<int64_t> RunAndStopV(int64_t d) {
    auto f = [](int64_t ss) { return (ss * (ss - 1)) / 2; };

    if (d < 0) {
      auto v = RunAndStopV(-d);
      for (auto& vv : v) vv = -vv;
      return v;
    }
    std::vector<int64_t> v;
    int64_t s = 0;
    for (; d > 0;) {
      if (d >= f(s + 1) + s + 1) {
        v.push_back(1);
        s += 1;
      } else if (d >= f(s) + s) {
        v.push_back(0);
      } else {
        v.push_back(-1);
        s -= 1;
      }
      d -= s;
    }
    if (s == 1) v.push_back(-1);
    return v;
  }

  static std::vector<I2Vector> RunAndStopV(const I2Vector& vd) {
    auto v1 = RunAndStopV(vd.dx);
    auto v2 = RunAndStopV(vd.dy);
    auto msize = std::max(v1.size(), v2.size());
    v1.resize(msize, 0);
    v2.resize(msize, 0);
    std::vector<I2Vector> vr(msize);
    for (unsigned i = 0; i < msize; ++i) vr.push_back({v1[i], v2[i]});
    return vr;
  }

  static std::string RunAndStopS(const I2Vector& vd) {
    std::string s;
    for (auto& v : RunAndStopV(vd)) s += V2C(v);
    return s;
  }

  static std::string SolveI(const std::vector<I2Point>& vp) {
    std::string s;
    I2Point last;
    for (auto pp : vp) {
      s += RunAndStopS(pp - last);
      last = pp;
    }
    return s;
  }

  Solution Solve(const TProblem& p) override {
    Solution s;
    s.SetId(p.Id());
    s.commands = SolveI(p.GetPoints());
    std::cout << p.Id() << "\t" << p.GetPoints().size() << "\t"
              << s.commands.size() << std::endl;
    return s;
  }
};
}  // namespace spaceship
