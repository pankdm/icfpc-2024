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
class LineMSTSaver : public BaseSolver {
 public:
  using TBase = BaseSolver;
  using PSolver = TBase::PSolver;

 public:
  LineMSTSaver() : BaseSolver() {}
  explicit LineMSTSaver(unsigned _max_time) : BaseSolver(_max_time) {}

  PSolver Clone() const override {
    return std::make_shared<LineMSTSaver>(*this);
  }

  std::string Name() const override { return "line_mst_saver"; }

  bool SkipSolutionRead() const override { return true; }
  // bool SkipBest() const override { return true; }
  
  static std::vector<std::vector<int>> MST(const std::vector<I2Point>& vp) {
    std::cout << "vp.size() " << vp.size() << std::endl;
    //for (auto &v: vp)
    //  std::cout << "mst v:" << v << "," << std::endl;

    std::vector<std::vector<int>> G(vp.size());
    std::vector<int> have(vp.size(), 0);
    std::vector<int64_t> distance(vp.size(), ((long long)1)<<62);
    std::vector<int> distance_to(vp.size(), -1);

    distance[0] = 0;
    for (int i = vp.size(); i > 0; --i) {
      int best_v = -1;
      int64_t best_dist = ((long long)1)<<62;
      for (int i = 0; i < (int)vp.size(); ++i) {
        if (have[i])
          continue;
        int64_t dist = distance[i];
        if (dist < best_dist) {
          best_dist = dist;
          best_v = i;
        }
      }
      //std::cout << "best_v=" << best_v << " best_dist=" << best_dist << std::endl;
      have[best_v] = 1;
      if (distance_to[best_v] >= 0) {
        G[best_v].push_back(distance_to[best_v]);
        G[distance_to[best_v]].push_back(best_v);
      }
      for (int i = 0; i < (int)vp.size(); ++i) {
        int32_t dist = DistanceL1(vp[best_v], vp[i]);
        if (distance[i] > dist) {
          distance[i] = dist;
          distance_to[i] = best_v;
        }
      }
    }
    
    return G;
  }

  static std::pair<int, int> GetDeepest(std::vector<std::vector<int>>& G, int here, int parent) {
     //std::cout << "here=" << here << " parent=" << parent << std::endl;
     std::pair<int, int> result(-1, here);
     for (auto v: G[here]) {
       if (v != parent)
         result = max(result, GetDeepest(G, v, here));
     }
     result.first += 1;
     return result;
  }

  static bool GetPathToDeepest(std::vector<std::vector<int>>& G, int here, int parent, int dest, std::vector<int>& path) {
     if (here == dest) {
        path.push_back(here);
        return true;
     }
     std::pair<int, int> result(0, here);
     for (auto v: G[here]) {
       if (v != parent && GetPathToDeepest(G, v, here, dest, path)) {
         path.push_back(here);
         return true;
       }
     }
     return false;
  }

  static std::vector<I2Point> FindLine(const std::vector<I2Point>& vp, const std::string id="") {
    std::cout << "Constructing line...";
    std::vector<I2Point> result;

    std::vector<I2Point> points_left = vp;
    I2Point last(0, 0);
    bool first = true;
    while (points_left.size())
    {
      std::vector<I2Point> points;
      points.reserve(points_left.size() + 1);
      points.emplace_back(last);
      for (auto &v : points_left)
        points.emplace_back(v);
      //points.insert(points_left.end(), points_left.begin(), points_left.end());

      std::vector<int> path_to_deepest;
      std::vector<std::vector<int>> G = MST(points);
      std::pair<int, int> deepest = GetDeepest(G, 0, -1);
      //std::cout << "deepest.first=" << deepest.first << " deepest.second=" << deepest.second << std::endl;
      GetPathToDeepest(G, 0, -1, deepest.second, path_to_deepest);
      //for (auto &v: path_to_deepest)
      //  std::cout << "path:" << v << "," << std::endl;
      if (first && id.length()) {
        first = false;
        std::string filename = "../../problems/spaceship_mst/spaceship" + id + ".txt";
        std::fstream file(filename, std::fstream::out);
        file << points.size() << std::endl;
        for (auto p : points) {
          file << p << std::endl;
        }
        for (int i = 0; i < (int)points.size(); ++i)
          for (auto v: G[i])
            if (i < v)
              file << i << ' ' << v << std::endl;
        file.close();
      }

      std::vector<int> marks(points.size(), 0);
      marks[path_to_deepest.back()] = 1;
      for (auto i = path_to_deepest.rbegin() + 1; i != path_to_deepest.rend(); ++i) {
        result.push_back(points[*i]);
        //std::cout << result.back() << ',' << std::endl;
        marks[*i] = 1;
      }
      
      points_left.clear();
      for (int i = 0; i < (int)points.size(); ++i) {
        if (!marks[i]) {
          points_left.emplace_back(points[i]);
        }
      }
      last = result.back();
    }
    
    std::cout << " Done" << std::endl;
    return result;
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
    auto line = FindLine(tvp, p.Id());
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
