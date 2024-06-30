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
class LineSaver : public BaseSolver {
 public:
  using TBase = BaseSolver;
  using PSolver = TBase::PSolver;

 public:
  LineSaver() : BaseSolver() {}
  explicit LineSaver(unsigned _max_time) : BaseSolver(_max_time) {}

  PSolver Clone() const override {
    return std::make_shared<LineSaver>(*this);
  }

  std::string Name() const override { return "line_saver"; }

  bool SkipSolutionRead() const override { return true; }
  // bool SkipBest() const override { return true; }

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
    if (std::abs(line[0].x) + std::abs(line[0].y) > std::abs(line.back().x) + std::abs(line.back().y))
        std::reverse(line.begin(), line.end());
    
    std::string id = p.Id();
    std::string filename = "../../problems/spaceship_line/spaceship" + id + ".txt";
    std::fstream file(filename, std::fstream::out);
    for (auto p : line)
        file << p << std::endl;
    file.close();
    s.commands = "";
    return s;
  }
};
}  // namespace spaceship
