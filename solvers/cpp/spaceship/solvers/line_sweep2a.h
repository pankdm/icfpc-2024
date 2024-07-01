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
class LineSweep2A : public LineSweep2 {
 public:
  using TBase = LineSweep2;
  using PSolver = TBase::PSolver;

 protected:
  unsigned max_steps_between_points;

 public:
  LineSweep2A() : LineSweep2() {}
  LineSweep2A(unsigned _max_time, unsigned _max_steps_between_points) : LineSweep2(_max_time, _max_steps_between_points) {}

  PSolver Clone() const override {
    return std::make_shared<LineSweep2A>(*this);
  }

  std::string Name() const override { return "ls2a"; }

  bool SkipSolutionRead() const override { return true; }
  // bool SkipBest() const override { return true; }

  Solution Solve(const TProblem& p) override {
    Solution s;
    s.SetId(p.Id());

    // Construct initial line
    auto line = ConstructLine(DropDups(p.GetPoints()));

    if (p.Id() == "17") {
      std::reverse(line.begin() + 40, line.begin() + 93);
      std::reverse(line.begin() + 40, line.begin() + 43);
      std::reverse(line.begin() + 18, line.begin() + 99);
      std::reverse(line.begin() + 18, line.begin() + 97);
      std::reverse(line.begin() + 26, line.begin() + 30);
      std::reverse(line.begin() + 32, line.begin() + 55);
    }

    if (Abs(line[0].x) + Abs(line[0].y) > Abs(line.back().x) + Abs(line.back().y))
      std::reverse(line.begin(), line.end());

    s.commands = SolveI(line, max_time_in_seconds, true);
    std::cout << p.Id() << "\t" << p.GetPoints().size() << "\t"
              << s.commands.size() << std::endl;
    if (s.commands.empty()) return s;

    int64_t max_distance = 100;
    if (p.Id() == "18") max_distance = 1000;

    for (bool stop = false; !stop;) {
      stop = true;
      auto best_new = s.commands.size();
      std::string best_s;
      std::string best_mode;
      unsigned best_i, best_j;

      // Reverse block
      for (unsigned i = 0; i < line.size(); ++i) {
        for (unsigned j = i + 1; j < line.size(); ++j) {
          if (DistanceLInf(line[i], line[j]) > max_distance) continue;
          std::reverse(line.begin() + i, line.begin() + j + 1);
          auto st = SolveI(line, max_time_in_seconds, true);
          std::reverse(line.begin() + i, line.begin() + j + 1);
          if (!st.empty() && (st.size() <= s.commands.size())) {
            std::cout << "\tB\t" << i << "\t" << j << "\t" << st.size() << "\t" << DistanceLInf(line[i], line[j]) <<
            std::endl;
            if (st.size() < best_new) {
              best_new = st.size();
              best_s = st;
              best_mode = "B";
              best_i = i;
              best_j = j;
            }
          }
        }
      }

      if (best_new < s.commands.size()) {
        stop = false;
        s.commands = best_s;
        std::cout << "New best: " << s.commands.size() << std::endl;
      }
      if (best_mode == "B") {
        std::cout << "Apply modification: B\t" << best_i << "\t" << best_j
                  << std::endl;
        std::reverse(line.begin() + best_i, line.begin() + best_j + 1);
      }
    }

    // Modifications search:
    // // Neighbors swap
    // for (unsigned i = 1; i < line.size(); ++i) {
    //   std::swap(line[i - 1], line[i]);
    //   auto st = SolveI(line, max_time_in_seconds, true);
    //   std::swap(line[i - 1], line[i]);
    //   if (!st.empty() && (st.size() <= s.commands.size()))
    //     std::cout << "\tP\t" << i - 1 << "\t" << i << "\t" << st.size() <<
    //     std::endl;
    // }

    // // Pairs swap
    // for (unsigned i = 0; i < line.size(); ++i) {
    //   for (unsigned j = i + 1; j < line.size(); ++j) {
    //     if (DistanceLInf(line[i], line[j]) > 100) continue;
    //     std::swap(line[i], line[j]);
    //     auto st = SolveI(line, max_time_in_seconds, true);
    //     std::swap(line[i], line[j]);
    //     if (!st.empty() && (st.size() <= s.commands.size()))
    //       std::cout << "\tP\t" << i << "\t" << j << "\t" << st.size() <<
    //       std::endl;
    //   }
    // }

    // // Reverse block
    // for (unsigned i = 0; i < line.size(); ++i) {
    //   for (unsigned j = i + 2; j < line.size(); ++j) {
    //     if (DistanceLInf(line[i], line[j]) > 100) continue;
    //     std::reverse(line.begin() + i, line.begin() + j);
    //     auto st = SolveI(line, max_time_in_seconds, true);
    //     std::reverse(line.begin() + i, line.begin() + j);
    //     if (!st.empty() && (st.size() <= s.commands.size()))
    //       std::cout << "\tB\t" << i << "\t" << j << "\t" << st.size() <<
    //       std::endl;
    //   }
    // }

    return s;
  }
};
}  // namespace spaceship
