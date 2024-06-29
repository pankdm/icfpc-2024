#pragma once

#include "spaceship/map.h"
#include "spaceship/solvers/base.h"

#include "common/geometry/d2/compare/point_xy.h"
#include "common/solvers/solver.h"
#include "common/vector/unique.h"

#include <algorithm>
#include <random>
#include <string>
#include <vector>
#include <map>

namespace spaceship {
class Greedy3 : public BaseSolver {
 public:
  using TBase = BaseSolver;
  using PSolver = TBase::PSolver;

 public:
  Greedy3() : BaseSolver() {}
  explicit Greedy3(unsigned _max_time) : BaseSolver(_max_time) {}

  PSolver Clone() const override { return std::make_shared<Greedy3>(*this); }

  std::string Name() const override { return "greedy3"; }

  class I2VectorCmp {
   public:
    bool operator ()(const I2Vector& a, const I2Vector &b) const {
      if (a.dx != b.dx)
        return a.dx < b.dx;
      return a.dy < b.dy;
    }
  };

  // bool SkipSolutionRead() const override { return true; }
  // bool SkipBest() const override { return true; }
  class PathsPool {
   public:
    std::map<I2Vector, std::string, I2VectorCmp> all;
    int attempts;
    PathsPool() : all(), attempts(0) {}
    void add(const I2Vector &v, const std::string& s) {
      //std::cout << "add (" << v << ')' << std::endl;
      auto f = all.find(v);
      attempts += 1;
      if (f == all.end()) {
        all[v] = s;
      }
    }
    size_t size() const { return all.size(); }
  };

  static void GetPaths(SpaceShip& ss, const I2Point& dest, unsigned s, std::string& sr, PathsPool& all) {
    if (all.size() > 20 || all.attempts >= 100000) return;
    if (s)
    {
      --s;
      ss.p += ss.v;
      bool found = false;
      for (int idx = -1; idx <= 1; ++idx) {
        for (int idy = -1; idy <= 1; ++idy) {
          I2Vector idv(idx, idy);
          ss.p += idv;
          ss.v += idv;
          if (ss.PossibleLocations(s).Inside(dest)) {
            found = true;
            sr += V2C(idv);
            GetPaths(ss, dest, s, sr, all);
            sr.pop_back();
          }
          ss.p -= idv;
          ss.v -= idv;
        }
      }
      if (!found)
        std::cout << "wut" << std::endl;
      ss.p -= ss.v;
    }
    else
      all.add(ss.v, sr);
  }
  
  static std::vector<std::pair<I2Vector, std::string>> GetUniquePaths(SpaceShip& ss, const I2Point& dest, unsigned s) {
    //SpaceShip ss_bak = ss;
    PathsPool pool;
    std::string sr;
    if (s)
      GetPaths(ss, dest, s, sr, pool);
    /*for (auto [v, str] : result) {
      ss = ss_bak;
      for (auto i : str) {
        ss.ApplyCommand(i);
      }
      if (ss.v != v || ss.p != dest)
        std::cout << "wut " << s << " (" << ss_bak.p << ") (" << ss_bak.v << ") (" << dest << ") " << str << std::endl;
    }*/
    //std::cout << "s = " << s << " result.size() = " << result.size() << std::endl;
    std::vector<std::pair<I2Vector, std::string>> result;
    for (auto &i : pool.all)
      result.emplace_back(i);
    return result; 
  }
  
  class SolutionString {
  public:
    std::shared_ptr<SolutionString> prev;
    std::string here;
    int length;
    SolutionString() : prev(), here(), length(0) {}
    SolutionString(const SolutionString& prev, const std::string& here) : prev(std::make_shared<SolutionString>(prev)), here(here), length(prev.length + here.length()) {}
  };
  
  class SolutionsPoolCmp {
   public:
    bool operator () (const std::pair<int, SpaceShip> &aa, const std::pair<int, SpaceShip> &bb) const {
      if (aa.first != bb.first)
        return aa.first < bb.first;
      const auto &a = aa.second;
      const auto &b = bb.second;
      if (a.p.x != b.p.x)
        return a.p.x < b.p.x;
      if (a.p.y != b.p.y)
        return a.p.y < b.p.y;
      if (a.v.dx != b.v.dx)
        return a.v.dx < b.v.dx;
      return a.v.dy < b.v.dy;
    }
  };

  class SolutionsPool {
   public:
    std::map<std::pair<int, SpaceShip>, SolutionString, SolutionsPoolCmp> all;
    void add(const SpaceShip &v, const SolutionString& s) {
      std::pair<int, SpaceShip> k(s.length, v);
      auto f = all.find(k);
      if (f == all.end()) {
        if (all.size() == 20) {
          f = all.end();
          --f;
          SolutionsPoolCmp cmp;
          if (!cmp(f->first, k)) {
            all[k] = s;
            f = all.end();
            --f;
            all.erase(f);
          }
        }
        else
          all[k] = s;
      }
    }
    size_t size() const { return all.size(); }
  };
  
  static int estimateSize(const SpaceShip& ss, const I2Point& dest) {
    int s = 1;
    int d = 1;
    int x = ss.p.x + ss.v.dx;
    while (dest.x > x + d) {
      s += 1;
      d += s;
      x += ss.v.dx;
    }
    while (dest.x < x - d) {
      s += 1;
      d += s;
      x += ss.v.dx;
    }
    int y = ss.p.y + ss.v.dy;
    while (dest.y > y + d) {
      s += 1;
      d += s;
      y += ss.v.dy;
    }
    while (dest.y < y - d) {
      s += 1;
      d += s;
      y += ss.v.dy;
    }
    return s;
  }

  static std::string SolveI(const std::vector<I2Point>& tvp) {
    SolutionsPool last;
    SolutionsPool next;
    last.add(SpaceShip(), SolutionString());
    auto vp = tvp;
    for (; !vp.empty();) {
      I2Point p;
      {
        bool found = false;
        const SpaceShip &ss = last.all.begin()->first.second;
        for (unsigned s = 1; !found; ++s) {
          auto b = ss.PossibleLocations(s);
          for (unsigned i = 0; i < vp.size(); ++i) {
            if (b.Inside(vp[i])) {
              found = true;
              p = vp[i];
              vp[i] = vp.back();
              vp.pop_back();
              break;
            }
          }
        }
      }
      std::cout << "point:" << p << " " << vp.size() << ' ' << last.size() << std::endl;
      for (auto [ss_pair, sol] : last.all) {
        SpaceShip ss = ss_pair.second;
        //std::cout << "sol.length = " << sol.length << std::endl;
        SolutionsPool here;
        unsigned tmp = 1;//estimateSize(ss, p);
        /*for (unsigned s = 1; s < tmp; ++s) {
          if (ss.PossibleLocations(s).Inside(p))
            std::cout << "error (" << ss.p << ") (" << ss.v << ") (" << p << ')' << std::endl;  
        }*/
        for (unsigned s = tmp; here.size() < 20 && s < tmp + 5; ++s) {
          auto b = ss.PossibleLocations(s);
          if (!here.size())
            tmp = s;
          //std::cout << "s " << s << " tmp " << tmp << " size " << here.size() << std::endl;
          if (b.Inside(p)) {
            //std::cout << "found!" << std::endl;
            for (auto [v, s_how] : GetUniquePaths(ss, p, s)) {
              SpaceShip nss;
              nss.v = v;
              nss.p = p;
              here.add(nss, SolutionString(sol, s_how));
            }
          }
        }
        for (auto [ss_pair, sol] : here.all)
          next.add(ss_pair.second, sol);
      }
      std::swap(last.all, next.all);
      next.all.clear();
    }
    SolutionString best;
    for (auto [ss, sol] : last.all)
      if (best.length == 0 || best.length > sol.length)
        best = sol;
    std::vector<std::string> result_array;
    while (best.prev) {
      result_array.emplace_back(best.here);
      best = *(best.prev);
    }
    result_array.emplace_back(best.here);
    std::reverse(result_array.begin(), result_array.end());
    std::string sr;
    for (auto &i : result_array) {
      std::cout << i << ',';
      sr += i;
    }
    std::cout << std::endl;
    return sr;
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
    std::cout << "I'm running!" << std::endl;

    // Shuffle and use random seeds
    std::shuffle(tvp.begin(), tvp.end(), std::mt19937(17));
    s.commands = SolveI(tvp);

    std::cout << p.Id() << "\t" << p.GetPoints().size() << "\t"
              << s.commands.size() << std::endl;
    return s;
  }
};
}  // namespace spaceship
