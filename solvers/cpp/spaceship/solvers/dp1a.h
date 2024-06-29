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
#include "common/solvers/solver.h"
#include "common/stl/hash/vector.h"
#include "common/timer.h"
#include "common/vector/unique.h"

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

namespace spaceship {
class DP1A : public BaseSolver {
 public:
  using TBase = BaseSolver;
  using PSolver = TBase::PSolver;

 public:
  DP1A() : BaseSolver() {}
  explicit DP1A(unsigned _max_time) : BaseSolver(_max_time) {}

  PSolver Clone() const override { return std::make_shared<DP1A>(*this); }

  std::string Name() const override { return "dp1a"; }

  bool SkipSolutionRead() const override { return true; }
  // bool SkipBest() const override { return true; }

 protected:
  class Task {
   public:
    SpaceShip ss;
    std::vector<uint8_t> visited;
    unsigned cost;
    size_t source_hash;

    size_t Hash() const {
      return HashCombine(ss.Hash(), std::hash<std::vector<uint8_t>>{}(visited));
    }
  };

  class TaskInfo {
   public:
    size_t hash;
    unsigned cost;

    bool operator<(const TaskInfo& r) const { return cost < r.cost; }
  };

 public:
  Solution Solve(const TProblem& p) override {
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
    // task_init.vp = tvp;
    task_init.visited = std::vector<uint8_t>(tvp.size(), false);
    task_init.cost = 0;
    task_init.source_hash = 0;
    auto task_init_hash = task_init.Hash();
    tasks[task_init_hash] = task_init;
    vheap[0].Add({task_init_hash, task_init.cost});

    unsigned best_solution = 10000000;
    unsigned code = 0;
    std::vector<unsigned> best_candidate(vheap.size() + 1, best_solution);
    for (;;) {
      if (t.GetSeconds() > max_time_in_seconds) {
        // Time to stop
        code = 1;
        break;
      }
      if (tasks.size() * (tvp.size() + 40) > (1ull << 32)) {
        // Avoid over memory usage
        code = 2;
        break;
      }
      for (unsigned i = vheap.size(); i-- > 0;) {
        best_candidate[i] = std::min<unsigned>(
            vheap[i].Empty() ? best_solution
                             : vheap[i].Top().cost + tvp.size() - i,
            best_candidate[i + 1]);
      }
      bool done = true;
      for (unsigned i = 0; i < vheap.size(); ++i) {
        // if (t.GetSeconds() > max_time_in_seconds) {
        //   // Time to stop
        //   code = 1;
        //   break;
        // }
        // if (tasks.size() * (16 * tvp.size()) > (1ull << 32)) {
        //   // Avoid over memory usage
        //   code = 2;
        //   break;
        // }
        if (vheap[i].Empty()) continue;
        auto best_i = vheap[i].Top().cost;
        if (best_i + tvp.size() - i >= best_solution) {
          vheap[i].Clear();
          continue;
        }
        if (best_i + tvp.size() - i > best_candidate[i]) {
          continue;
        }
        done = false;
        if (i == tvp.size()) {
          // New best solution
          best_solution = best_i;
          std::cout << "New best solution with cost " << best_solution
                    << std::endl;
          std::string ss;
          auto t_hash = vheap[i].Top().hash;
          auto t = &(tasks[t_hash]);
          for (; t->cost > 0;) {
            auto t2 = &(tasks[t->source_hash]);
            ss += V2C(t->ss.v - t2->ss.v);
            t = t2;
          }
          std::reverse(ss.begin(), ss.end());
          s.commands = ss;
          break;
        }
        // for (; !vheap[i].Empty() && (vheap[i].Top().cost == best_i);)
        {
          auto t_hash = vheap[i].Top().hash;
          vheap[i].Pop();
          auto t = tasks[t_hash];
          if (t.cost < best_i) {
            // Already processed
            continue;
          }
          // One steps search
          for (int idx = -1; idx <= 1; ++idx) {
            for (int idy = -1; idy <= 1; ++idy) {
              I2Vector idv(idx, idy);
              Task task_new;
              task_new.ss.p = t.ss.p + t.ss.v + idv;
              task_new.ss.v = task_new.ss.p - t.ss.p;
              task_new.visited = t.visited;
              unsigned shift = 0;
              for (unsigned j = 0; j < tvp.size(); ++j) {
                if (!task_new.visited[j] && (tvp[j] == task_new.ss.p)) {
                  task_new.visited[j] = true;
                  shift += 1;
                }
              }
              task_new.cost = best_i + 1;
              task_new.source_hash = t_hash;
              auto task_new_hash = task_new.Hash();
              auto it = tasks.find(task_new_hash);
              if (it == tasks.end()) {
                tasks[task_new_hash] = task_new;
              } else if (it->second.ss != task_new.ss) {
                // Hash conflict, skipping
                continue;
              } else if (it->second.cost > task_new.cost) {
                it->second.cost = task_new.cost;
                it->second.source_hash = task_new.source_hash;
              } else {
                // Already processed
                continue;
              }
              vheap[i + shift].Add({task_new_hash, task_new.cost});
            }
          }
        }
      }
      if (done) break;
    }
    std::cout << p.Id() << "\t" << p.GetPoints().size() << "\t"
              << s.commands.size() << "\t" << code << std::endl;
    return s;
  }
};
}  // namespace spaceship
