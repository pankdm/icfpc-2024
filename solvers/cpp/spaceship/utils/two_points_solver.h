#pragma once

#include "spaceship/map.h"
#include "spaceship/spaceship.h"
#include "spaceship/utils/one_point_solver.h"

#include "common/geometry/d2/point.h"
#include "common/geometry/d2/vector.h"
#include "common/heap.h"
#include "common/numeric/bits/rotate.h"
#include "common/stl/hash/array.h"
#include "common/timer.h"

#include <array>
#include <iostream>
#include <tuple>
#include <unordered_map>

namespace spaceship {
class TwoPointsSolver {
 protected:
  class Task {
   public:
    SpaceShip ss;
    unsigned cost;
    unsigned min_final_cost;
    size_t source_hash;

    size_t Hash() const { return ss.Hash(); }

    void ComputeMinFinalCost(const I2Point& p1, const I2Point& p2) {
      unsigned min_extra_cost = 0;
      if (ss.p == p1) {
        min_extra_cost = OnePointSolver::MinSteps(ss.v, (p2 - p1).ToPoint());
      } else {
        for (unsigned s = 1; ; ++s) {
          auto b = ss.PossibleLocations(s);
          if (b.Inside(p1)) {
            min_extra_cost = s + 1;
            break;
          }
        }
      }
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

 protected:
  // using TKey = uint64_t;
  using TKey = std::array<int64_t, 6>;

  std::unordered_map<TKey, std::pair<char, unsigned>> cache;
  uint64_t hash_conflicts = 0;

 public:
  // static TKey HKey(const I2Vector& v, const I2Point& p1, const I2Point& p2) {
  //   return numeric::RotateBitsR(v.dx, 54) + numeric::RotateBitsR(v.dy, 48) +
  //         numeric::RotateBitsR(p1.x, 36) + numeric::RotateBitsR(p1.y, 24) +
  //         numeric::RotateBitsR(p2.x - p1.x, 12) + numeric::RotateBitsR(p2.y - p1.y, 0);
  // }
  static TKey HKey(const I2Vector& v, const I2Point& p1, const I2Point& p2) {
    return {v.dx, v.dy, p1.x, p1.y, p2.x, p2.y};
  }

  size_t CacheSize() const { return cache.size(); }
  void ResetHashConflicts() { hash_conflicts = 0; }

  std::pair<char, unsigned> Solve(const I2Vector& v, const I2Point& p1, const I2Point& p2, unsigned time_in_ms) {
    // Check cache
    if ((p1 == I2Point()) && (p1 == p2)) return OnePointSolver::Solve(v, p2);
    auto hkey = HKey(v, p1, p2);
    auto it = cache.find(hkey);
    if (it != cache.end()) return it->second;

    // Solve
    Timer t;
    std::unordered_map<size_t, Task> tasks;
    HeapMinOnTop<TaskInfo> hheap;
    Task task_init;
    task_init.ss.v = v;
    task_init.cost = 0;
    task_init.source_hash = 0;
    auto task_init_hash = task_init.Hash();
    task_init.ComputeMinFinalCost(p1, p2);
    tasks[task_init_hash] = task_init;
    hheap.Add({task_init_hash, task_init.min_final_cost});

    unsigned best_solution = 10000000;
    char best_solution_move = '5';
    size_t best_solution_task_hash = 0;

    for (;;) {
      if (t.GetMilliseconds() > time_in_ms) {
        // Timeout
        return OnePointSolver::Solve(v, p1);
      }
      
      if (hheap.Empty()) break;
      if (hheap.Top().min_final_cost >= best_solution) break;

      auto t_hash = hheap.Top().hash;
      auto t_min_final_cost = hheap.Top().min_final_cost;
      hheap.Pop();
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
          task_new.cost = t.cost + 1;
          task_new.source_hash = t_hash;
          auto task_new_hash = task_new.Hash();

          auto it = tasks.find(task_new_hash);
          if (it == tasks.end()) {
            // Check if answer is known
            if (task_new.ss.p == p1) {
              auto min_extra_cost = OnePointSolver::MinSteps(task_new.ss.v, (p2 - p1).ToPoint());
              task_new.min_final_cost = task_new.cost + min_extra_cost;
              if (task_new.min_final_cost < best_solution) {
                best_solution = task_new.min_final_cost;
                best_solution_task_hash = task_new_hash;
                tasks[task_new_hash] = task_new;
              }
              continue;
            }
            // ...
            // Check inside cache
            // ...
            task_new.ComputeMinFinalCost(p1, p2);
            if (task_new.min_final_cost < t.min_final_cost) {
              std::cout << "TPS: Min final cost should not decrease." << std::endl;
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
          hheap.Add({task_new_hash, task_new.min_final_cost});
        }
      }
    }

    // Save solution
    if (best_solution_task_hash != 0) {
      auto t_hash = best_solution_task_hash;
      auto t = &(tasks[t_hash]);
      for (; t->cost > 0;) {
        auto t2 = &(tasks[t->source_hash]);
        if (t2->cost + 1 != t->cost) {
          std::cout << "TPS: Incorrect cost during solution construction." << std::endl;
        }
        auto thkey = HKey(t2->ss.v, (p1 - t2->ss.p).ToPoint(),
                          (p2 - t2->ss.p).ToPoint());
        auto it2 = cache.find(thkey);
        if (it2 == cache.end()) {
          cache[thkey] = {V2C(t->ss.v - t2->ss.v), best_solution - t2->cost};
        } else {
          if (it2->second != std::make_pair(V2C(t->ss.v - t2->ss.v), best_solution - t2->cost)) {
            std::cout << "TPS: Different value in cache than expecting." << std::endl;
            std::cout << "\t" << "Old: " << it2->second.second
                      << "\tNew:" << best_solution - t2->cost << std::endl;
            std::cout << "\t" << t2->ss.v << "\t" << p1 - t2->ss.p << "\t"
                      << p2 - t2->ss.p << std::endl;
          }
        }
        if (t2->cost == 0) {
          best_solution_move = V2C(t->ss.v - t2->ss.v);
        }
        t = t2;
      }
    }

    return {best_solution_move, best_solution};
  }

  unsigned MinSteps(const I2Vector& v, const I2Point& p1, const I2Point& p2, unsigned time_in_ms) {
    return Solve(v, p1, p2, time_in_ms).second;
  }

  char BestMove(const I2Vector& v, const I2Point& p1, const I2Point& p2, unsigned time_in_ms) {
    return Solve(v, p1, p2, time_in_ms).first;
  }
};
}  // namespace spaceship
