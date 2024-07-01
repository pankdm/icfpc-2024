#pragma once

#include "spaceship/map.h"
#include "spaceship/solvers/base.h"
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
class LineSweep2 : public BaseSolver {
 public:
  using TBase = BaseSolver;
  using PSolver = TBase::PSolver;

 protected:
  unsigned max_steps_between_points;

 public:
  LineSweep2() : BaseSolver() {}
  LineSweep2(unsigned _max_time, unsigned _max_steps_between_points) : BaseSolver(_max_time), max_steps_between_points(_max_steps_between_points) {}

  PSolver Clone() const override {
    return std::make_shared<LineSweep2>(*this);
  }

  std::string Name() const override { return "ls2"; }

  bool SkipSolutionRead() const override { return true; }
  // bool SkipBest() const override { return true; }

 protected:
  class Task {
   public:
    I2Vector v;
    I2Vector vfrom;

    unsigned cost;
    unsigned extra;
    unsigned final_cost;
  };

  class TaskInfo {
   public:
    I2Vector v;
    unsigned cost;
    unsigned final_cost;

    bool operator<(const TaskInfo& r) const {
      return final_cost < r.final_cost;
    }
  };

 protected:
  std::vector<std::unordered_map<int64_t, std::vector<int64_t>>> cache_psat;
  uint64_t cache_psat_memory = 0;

 public:
  std::vector<int64_t> PossibleSpeedAtLocation(int64_t x, int64_t v0, unsigned time) {
    // Manual
    if (time == 0) {
      return (x == 0) ? std::vector<int64_t>{v0} : std::vector<int64_t>{};
    } else if (time == 1) {
      return (x == v0 - 1) ? std::vector<int64_t>{v0 - 1} :
             (x == v0) ? std::vector<int64_t>{v0} :
             (x == v0 + 1) ? std::vector<int64_t>{v0 + 1} : std::vector<int64_t>{};
    }
    auto max_t = (time * (time + 1)) / 2;
    if (Abs(x - v0 * time) > max_t) return {};

    // Cache
    if (cache_psat.size() <= time) {
      cache_psat.resize(time + 1);
    }
    x -= v0 * time;
    auto it = cache_psat[time].find(x);
    if (it == cache_psat[time].end()) {
      auto v1 = PossibleSpeedAtLocation(x + 1, -1, time - 1);
      auto v2 = PossibleSpeedAtLocation(x, 0, time - 1);
      auto v3 = PossibleSpeedAtLocation(x - 1, 1, time - 1);
      auto vu = nvector::UnionV(nvector::UnionV(v1, v2), v3);
      cache_psat[time][x] = vu;
      cache_psat_memory += 32 + 8 * vu.size();
      it = cache_psat[time].find(x);
    }
    auto vv = it->second;
    for (auto& v : vv) v += v0;
    return vv;
  }

  std::string SolveI(const std::vector<I2Point>& line, unsigned max_time_in_seconds, bool silent = false) {
    Timer t;
    std::vector<std::unordered_map<I2Vector, Task>> tasks;
    std::vector<HeapMinOnTop<TaskInfo>> vheap;

    tasks.resize(line.size() + 1);
    vheap.resize(line.size());
    Task task_init;
    task_init.v = I2Vector();
    task_init.cost = 0;
    task_init.extra = OnePointSolver::MinSteps(task_init.v, line[0]);
    task_init.final_cost = task_init.cost + task_init.extra;
    if (task_init.extra <= max_steps_between_points) {
      tasks[0][task_init.v] = task_init;
      vheap[0].Add({task_init.v, task_init.cost, task_init.final_cost});
    }

    bool solution_exist = false;
    unsigned best_solution = 10000000;
    I2Vector best_solution_v;
    unsigned status = 0;
    for (;;) {
      if (t.GetSeconds() > max_time_in_seconds) {
        // Time to stop
        status = 1;
        break;
      }
      uint64_t memory = 0;
      for (auto& ti : tasks) memory += 80 * ti.size();
      for (auto& ti : vheap) memory += 40 * ti.Size();
      if (memory + cache_psat_memory > (1ull << 32)) {
        // Avoid over memory usage
        status = 2;
        break;
      }

      bool done = true;
      for (unsigned i = 0; i < vheap.size(); ++i) {
        if (vheap[i].Empty()) continue;
        // TODO: Improve "line.size() - i - 1" adjustment
        if (vheap[i].Top().final_cost + line.size() - i - 1 >= best_solution) {
          vheap[i].Clear();
          continue;
        }
        done = false;
        if (i == vheap.size() - 1) {
          // New best solution
          solution_exist = true;
          best_solution = vheap[i].Top().final_cost;
          best_solution_v = vheap[i].Top().v;
          if (!silent) {
            std::cout << "New best solution with cost " << best_solution
                      << std::endl;
          }
          break;
        }

        auto top = vheap[i].Top();
        vheap[i].Pop();
        auto t_v = top.v;
        auto t_cost = top.cost;
        auto t = tasks[i][t_v];
        if (t.cost < t_cost) {
          // Already processed
          continue;
        }

        // Add i+1
        SpaceShip ss;
        if (i > 0) ss.p = line[i - 1];
        ss.v = t.v;
        auto b = ss.PossibleLocations(t.extra);
        if (b.Inside(line[i])) {
          auto vx =
              PossibleSpeedAtLocation(line[i].x - ss.p.x, ss.v.dx, t.extra);
          auto vy =
              PossibleSpeedAtLocation(line[i].y - ss.p.y, ss.v.dy, t.extra);
          if (vx.empty() || vy.empty()) {
            std::cout << "Unexpected empty set of possible speed." << std::endl;
            Assert(false);
          }
          if (vx.size() * vy.size() < 1000000) {
            // Avoid too much new candidates
            for (auto dx : vx) {
              for (auto dy : vy) {
                I2Vector new_v(dx, dy);
                auto it = tasks[i + 1].find(new_v);
                if (it == tasks[i + 1].end()) {
                  Task task_new;
                  task_new.v = new_v;
                  task_new.vfrom = t.v;
                  task_new.cost = t.final_cost;
                  task_new.extra = OnePointSolver::MinSteps(task_new.v, (line[i + 1] - line[i]).ToPoint());
                  task_new.final_cost = task_new.cost + task_new.extra;
                  if (task_new.extra <= max_steps_between_points) {
                    tasks[i + 1][task_new.v] = task_new;
                    vheap[i + 1].Add({task_new.v, task_new.cost, task_new.final_cost});
                  }
                } else {
                  if (it->second.cost > t.final_cost) {
                    // Better cost, reset node
                    it->second.vfrom = t.v;
                    it->second.cost = t.final_cost;
                    it->second.extra = OnePointSolver::MinSteps(it->second.v, (line[i + 1] - line[i]).ToPoint());
                    it->second.final_cost = it->second.cost + it->second.extra;
                    vheap[i + 1].Add({it->second.v, it->second.cost, it->second.final_cost});
                  }
                }
              }
            }
          }
        }

        // Increase search window
        if (t.extra < max_steps_between_points) {
          tasks[i][t_v].extra += 1;
          tasks[i][t_v].final_cost += 1;
          vheap[i].Add({t.v, t.cost, t.final_cost + 1});
        }
      }
      if (done) break;
    }
    if (!silent) {
      size_t cache_size = 0, cache_psat_size = 0;
      for (auto& it : tasks) cache_size += it.size();
      for (auto& it : cache_psat) cache_psat_size += it.size();
      std::cout << "\tStatus = " << status << "\tCashe size = " << cache_size << "\tPSAT cashe size = " << cache_psat_size
                << std::endl;
    }
    
    // Reconstruct solution
    if (!solution_exist) return "";
    std::string output;
    {
      std::vector<I2Vector> vv;
      auto vcur = best_solution_v;
      for (unsigned i = vheap.size(); i-- > 0;) {
        vv.push_back(vcur);
        vcur = tasks[i][vcur].vfrom;
      }
      std::reverse(vv.begin(), vv.end());
      assert(vv.size() == line.size());
      for (unsigned i = 0; i + 1 < vv.size(); ++i) {
        auto p0 = (i == 0) ? I2Point() : line[i - 1], p1 = line[i];
        auto v0 = vv[i], v1 = vv[i + 1];
        unsigned steps;
        for (steps = 0;; ++steps) {
          auto vvx = PossibleSpeedAtLocation(p1.x - p0.x, v0.dx, steps),
               vvy = PossibleSpeedAtLocation(p1.y - p0.y, v0.dy, steps);
          if (std::binary_search(vvx.begin(), vvx.end(), v1.dx) && std::binary_search(vvy.begin(), vvy.end(), v1.dy))
            break;
        }
        for (; steps-- > 0;) {
          p0 += v0;
          bool found_x = false, found_y = false;
          int dx, dy;
          for (dx = -1; (dx <= 1) && !found_x; ++dx) {
            p0.x += dx;
            v0.dx += dx;
            auto vvx = PossibleSpeedAtLocation(p1.x - p0.x, v0.dx, steps);
            if (std::binary_search(vvx.begin(), vvx.end(), v1.dx)) {
              found_x = true;
              break;
            } else {
              p0.x -= dx;
              v0.dx -= dx;
            }
          }
          for (dy = -1; (dy <= 1) && !found_y; ++dy) {
            p0.y += dy;
            v0.dy += dy;
            auto vvy = PossibleSpeedAtLocation(p1.y - p0.y, v0.dy, steps);
            if (std::binary_search(vvy.begin(), vvy.end(), v1.dy)) {
              found_y = true;
              break;
            } else {
              p0.y -= dy;
              v0.dy -= dy;
            }
          }
          if (!found_x || !found_y) {
            std::cout << "Can't recover path in LS2" << std::endl;
            return "";
          } else {
            output += V2C(I2Vector(dx, dy));
          }
        }
      }
      output += OnePointSolver::GetPath(
          vv.back(), (line[line.size() - 1] - line[line.size() - 2]).ToPoint());
    }
    return output;
  }

  Solution Solve(const TProblem& p) override {
    Solution s;
    s.SetId(p.Id());

    // Regular solution
    auto line = ConstructLine(DropDups(p.GetPoints()));

    // Solve
    auto s1 = SolveI(line, max_time_in_seconds / 2);
    std::reverse(line.begin(), line.end());
    auto s2 = SolveI(line, max_time_in_seconds / 2);
    s.commands = (s2.empty()                 ? s1
                  : s1.empty()               ? s2
                  : (s1.size() <= s2.size()) ? s1
                                             : s2);

    // // Alternative
    // auto line = DropDups(p.GetPoints(), true);
    // s.commands = SolveI(line, max_time_in_seconds);

    std::cout << p.Id() << "\t" << p.GetPoints().size() << "\t" << s.commands.size() << std::endl;
    return s;
  }
};
}  // namespace spaceship
