#pragma once

#include "spaceship/map.h"
#include "spaceship/solvers/base.h"

#include "common/geometry/d2/compare/point_xy.h"
#include "common/geometry/d2/distance/distance_linf.h"
#include "common/geometry/d2/point_io.h"
#include "common/geometry/d2/stl_hash/point.h"
#include "common/geometry/d2/vector_io.h"
#include "common/hash.h"
#include "common/heap.h"
#include "common/solvers/ext/evaluate.h"
#include "common/solvers/solver.h"
#include "common/stl/hash/vector.h"
#include "common/timer.h"
#include "common/vector/enumerate.h"
#include "common/vector/unique.h"

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

namespace spaceship {
class DP2A : public BaseSolver {
 public:
  using TBase = BaseSolver;
  using PSolver = TBase::PSolver;

 public:
  DP2A() : BaseSolver() {}
  explicit DP2A(unsigned _max_time) : BaseSolver(_max_time) {}

  PSolver Clone() const override { return std::make_shared<DP2A>(*this); }

  std::string Name() const override { return "dp2a"; }

  bool SkipSolutionRead() const override { return true; }
  // bool SkipBest() const override { return true; }

 protected:
  class Task {
   public:
    SpaceShip ss;
    std::vector<uint16_t> vp;

    unsigned cost;
    unsigned min_final_cost;

    size_t source_hash;

    size_t Hash() const {
      return HashCombine(ss.Hash(), std::hash<std::vector<uint16_t>>{}(vp));
    }

    void ComputeMinFinalCost(const std::vector<I2Point>& tvp) {
      auto vt = vp;
      unsigned min_extra_cost = vp.size();
      for (unsigned s = 1; !vt.empty(); ++s) {
        auto b = ss.PossibleLocations(s);
        for (unsigned j = 0; j < vt.size(); ++j) {
          if (b.Inside(tvp[vt[j]])) {
            vt[j--] = vt.back();
            vt.pop_back();
            min_extra_cost = std::max<unsigned>(min_extra_cost, s + vt.size());
          }
        }
      }
      // min_final_cost = cost + vp.size();
      min_final_cost = cost + min_extra_cost;
    }
  };

  class TaskInfo {
   public:
    size_t hash;
    unsigned min_final_cost;

    bool operator<(const TaskInfo& r) const {
      return min_final_cost < r.min_final_cost;
    }
  };

 public:
  Solution Solve(const TProblem& p) override {
    auto rb =
        solvers::ext::Evaluate<Evaluator, Problem, Solution>(p.Id(), "best");

    Timer t;
    Solution s;
    s.SetId(p.Id());
    auto tvp = p.GetPoints();

    // Clean zero
    for (unsigned i = 0; i < tvp.size(); ++i) {
      if (tvp[i] == I2Point()) {
        tvp[i--] = tvp.back();
        tvp.pop_back();
      }
    }

    // Drop dups
    std::sort(tvp.begin(), tvp.end(), CompareXY<int64_t>);
    nvector::Unique(tvp);

    // Init heap
    std::unordered_map<size_t, Task> tasks;
    std::vector<HeapMinOnTop<TaskInfo>> vheap;
    vheap.resize(tvp.size() + 1);
    Task task_init;
    task_init.vp = nvector::Enumerate<uint16_t>(0u, tvp.size());
    task_init.cost = 0;
    task_init.source_hash = 0;
    auto task_init_hash = task_init.Hash();
    task_init.ComputeMinFinalCost(tvp);
    tasks[task_init_hash] = task_init;
    vheap[0].Add({task_init_hash, task_init.min_final_cost});

    unsigned best_solution = (rb.correct ? rb.score : 10000000u);
    unsigned status = 0;
    std::vector<unsigned> best_candidate(vheap.size() + 1, best_solution);
    for (;;) {
      if (t.GetSeconds() > max_time_in_seconds) {
        // Time to stop
        status = 1;
        break;
      }
      if (tasks.size() * (2 * tvp.size() + 40) > (1ull << 32)) {
        // Avoid over memory usage
        status = 2;
        break;
      }
      for (unsigned i = vheap.size(); i-- > 0;) {
        best_candidate[i] = std::min<unsigned>(
            vheap[i].Empty() ? best_solution : vheap[i].Top().min_final_cost,
            best_candidate[i + 1]);
      }
      bool done = true;
      for (unsigned i = 0; i < vheap.size(); ++i) {
        if (vheap[i].Empty()) continue;
        if (vheap[i].Top().min_final_cost >= best_solution) {
          vheap[i].Clear();
          continue;
        }
        if (vheap[i].Top().min_final_cost > best_candidate[i]) {
          // Let's not spend budget here when exist more promising candidates
          // with higher coverage
          continue;
        }
        done = false;
        if (i == tvp.size()) {
          // New best solution
          best_solution = vheap[i].Top().min_final_cost;
          std::cout << "New best solution with cost " << best_solution
                    << std::endl;
          std::string ss;
          auto t_hash = vheap[i].Top().hash;
          auto t = &(tasks[t_hash]);
          if (t->cost != t->min_final_cost) {
            std::cout << "\tUnexpected cost diff:\t" << t->cost << "\t"
                      << t->min_final_cost << std::endl;
          }
          for (; t->cost > 0;) {
            auto t2 = &(tasks[t->source_hash]);
            ss += V2C(t->ss.v - t2->ss.v);
            t = t2;
          }
          std::reverse(ss.begin(), ss.end());
          s.commands = ss;
          break;
        }

        auto t_hash = vheap[i].Top().hash;
        auto t_min_final_cost = vheap[i].Top().min_final_cost;
        vheap[i].Pop();
        auto t = tasks[t_hash];
        if (t.min_final_cost < t_min_final_cost) {
          // Already processed
          continue;
        }

        for (int idx = -1; idx <= 1; ++idx) {
          for (int idy = -1; idy <= 1; ++idy) {
            I2Vector idv(idx, idy);
            Task task_new;
            task_new.ss.p = t.ss.p + t.ss.v + idv;
            task_new.ss.v = t.ss.v + idv;
            task_new.vp = t.vp;
            unsigned shift = 0;
            for (unsigned j = 0; j < task_new.vp.size(); ++j) {
              if (tvp[task_new.vp[j]] == task_new.ss.p) {
                task_new.vp.erase(task_new.vp.begin() + j);
                shift += 1;
                break;
              }
            }
            task_new.cost = t.cost + 1;
            task_new.source_hash = t_hash;
            auto task_new_hash = task_new.Hash();
            auto it = tasks.find(task_new_hash);
            if (it == tasks.end()) {
              task_new.ComputeMinFinalCost(tvp);
              if (task_new.min_final_cost < t.min_final_cost) {
                std::cout << "Min final cost should not decrease." << std::endl;
              }
              tasks[task_new_hash] = task_new;
            } else if (it->second.ss != task_new.ss) {
              // Hash conflict, skipping
              continue;
            } else if (it->second.cost > task_new.cost) {
              // Better path to the same point
              auto d = it->second.cost - task_new.cost;
              it->second.cost -= d;
              it->second.min_final_cost -= d;
              task_new.min_final_cost = it->second.min_final_cost;
              it->second.source_hash = task_new.source_hash;
            } else {
              // Already processed
              continue;
            }
            vheap[i + shift].Add({task_new_hash, task_new.min_final_cost});
          }
        }
      }
      if (done) break;
    }
    std::cout << p.Id() << "\t" << p.GetPoints().size() << "\t" << tvp.size()
              << "\t" << s.commands.size() << "\t" << status << std::endl;
    return s;
  }
};
}  // namespace spaceship
