#pragma once

#include "spaceship/map.h"
#include "spaceship/solvers/base.h"

#include "common/geometry/d2/compare/point_xy.h"
#include "common/geometry/d2/distance/distance_l1.h"
#include "common/hash.h"
#include "common/heap.h"
#include "common/solvers/solver.h"
#include "common/timer.h"
#include "common/vector/enumerate.h"
#include "common/vector/unique.h"

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

namespace spaceship {
class LineSweep1A : public BaseSolver {
 public:
  using TBase = BaseSolver;
  using PSolver = TBase::PSolver;

 public:
  LineSweep1A() : BaseSolver() {}
  explicit LineSweep1A(unsigned _max_time) : BaseSolver(_max_time) {}

  PSolver Clone() const override {
    return std::make_shared<LineSweep1A>(*this);
  }

  std::string Name() const override { return "ls1a"; }

  bool SkipSolutionRead() const override { return true; }
  // bool SkipBest() const override { return true; }

 protected:
  class Task {
   public:
    SpaceShip ss;
    unsigned covered;

    unsigned cost;
    unsigned min_final_cost;

    size_t source_hash;

    size_t Hash() const { return HashCombine(ss.Hash(), covered); }

    void ComputeMinFinalCost(const std::vector<I2Point>& line) {
      unsigned s = 1;
      if (covered < line.size()) {
        for (;; ++s) {
          if (ss.PossibleLocations(s).Inside(line[covered])) break;
        }
      }
      min_final_cost = cost + line.size() - covered + s - 1;
      // min_final_cost = cost + line.size() - covered;
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
  static std::string SolveI(const std::vector<I2Point>& line,
                            unsigned max_time_in_seconds) {
    Timer t;
    std::unordered_map<size_t, Task> tasks;
    std::vector<HeapMinOnTop<TaskInfo>> vheap;
    std::string best_s;
    uint64_t hash_conflicts = 0;

    vheap.resize(line.size() + 1);
    Task task_init;
    task_init.covered = 0;
    task_init.cost = 0;
    task_init.source_hash = 0;
    auto task_init_hash = task_init.Hash();
    task_init.ComputeMinFinalCost(line);
    tasks[task_init_hash] = task_init;
    vheap[0].Add({task_init_hash, task_init.min_final_cost});

    unsigned best_solution = 10000000;
    unsigned status = 0;
    std::vector<unsigned> best_candidate(vheap.size() + 1, best_solution);
    for (;;) {
      if (t.GetSeconds() > max_time_in_seconds) {
        // Time to stop
        status = 1;
        break;
      }
      if (tasks.size() * 80 > (1ull << 32)) {
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
        if (i == line.size()) {
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
          best_s = ss;
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
            task_new.covered = t.covered;
            if (line[task_new.covered] == task_new.ss.p) {
              task_new.covered += 1;
            }
            task_new.cost = t.cost + 1;
            task_new.source_hash = t_hash;
            auto task_new_hash = task_new.Hash();
            auto it = tasks.find(task_new_hash);
            if (it == tasks.end()) {
              task_new.ComputeMinFinalCost(line);
              if (task_new.min_final_cost < t.min_final_cost) {
                std::cout << "Min final cost should not decrease." << std::endl;
              }
              tasks[task_new_hash] = task_new;
            } else if (it->second.ss != task_new.ss) {
              // Hash conflict, skipping
              ++hash_conflicts;
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
            vheap[task_new.covered].Add(
                {task_new_hash, task_new.min_final_cost});
          }
        }
      }
      if (done) break;
    }
    std::cout << "Status = " << status << "\tCashe size = " << tasks.size()
              << "\tHash conflicts = " << hash_conflicts << std::endl;
    return best_s;
  }

  // TODO:
  //   Add faster algorithm for P23 and P25.
  //   Current algorithm O(N^2).
  static std::vector<I2Point> FindLine(const std::vector<I2Point>& vp) {
    std::cout << "Constructing line...";
    std::vector<std::vector<unsigned>> vlines;
    std::vector<unsigned> index = nvector::Enumerate<unsigned>(0u, vp.size());
    std::vector<unsigned> status(vp.size(), 3);
    for (auto& i : index) vlines.push_back({i});
    std::vector<std::pair<int64_t, std::pair<unsigned, unsigned>>> vd;
    // TODO: Reduce number of pairs here
    for (unsigned i = 0; i < vp.size(); ++i) {
      for (unsigned j = i + 1; j < vp.size(); ++j) {
        vd.push_back({DistanceL1(vp[i], vp[j]), {i, j}});
      }
    }
    std::sort(vd.begin(), vd.end(),
              [](auto& l, auto& r) { return l.first < r.first; });
    for (auto& pp : vd) {
      auto i = pp.second.first;
      auto j = pp.second.second;
      if ((status[i] == 0) || (status[j] == 0) || (index[i] == index[j]))
        continue;
      if (vlines[index[i]].size() < vlines[index[j]].size()) std::swap(i, j);
      auto ij = index[j];
      if (ij != vlines.size() - 1) {
        index[vlines.back()[0]] = index[vlines.back().back()] = ij;
        std::swap(vlines[ij], vlines.back());
      }
      auto ii = index[i];
      if (status[i] == 1) {
        status[i] = 2;
        status[vlines[ii].back()] = 1;
        std::reverse(vlines[ii].begin(), vlines[ii].end());
      }
      if (status[j] == 2) {
        status[j] = 1;
        status[vlines.back()[0]] = 2;
        std::reverse(vlines.back().begin(), vlines.back().end());
      }
      vlines[ii].insert(vlines[ii].end(), vlines.back().begin(),
                        vlines.back().end());
      status[i] &= ~2;
      status[j] &= ~1;
      index[vlines[ii].back()] = ii;
      vlines.pop_back();
    }

    std::cout << "\tDone." << std::endl;
    std::vector<I2Point> output;
    for (auto u : vlines[0]) output.push_back(vp[u]);
    return output;
  }

  Solution Solve(const TProblem& p) override {
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

    // Construct line
    auto line = FindLine(tvp);

    // Solve
    auto s1 = SolveI(line, max_time_in_seconds / 2);
    std::reverse(line.begin(), line.end());
    auto s2 = SolveI(line, max_time_in_seconds / 2);
    s.commands = (s2.empty()                 ? s1
                  : s1.empty()               ? s2
                  : (s1.size() <= s2.size()) ? s1
                                             : s2);

    // Init heap
    std::cout << p.Id() << "\t" << p.GetPoints().size() << "\t" << tvp.size()
              << "\t" << s.commands.size() << std::endl;
    return s;
  }
};
}  // namespace spaceship
