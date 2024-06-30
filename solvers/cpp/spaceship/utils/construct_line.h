#pragma once

#include "common/geometry/d2/distance/distance_l1.h"
#include "common/geometry/d2/point.h"
#include "common/timer.h"
#include "common/vector/enumerate.h"

#include <algorithm>
#include <iostream>
#include <vector>
#include <utility>

namespace spaceship {
// TODO:
//   Add faster algorithm for P23 and P25.
//   Current algorithm O(N^2).
static std::vector<I2Point> ConstructLine(const std::vector<I2Point>& vp) {
  Timer t;
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

  std::cout << "\tDone.\tTime = " << t.GetMilliseconds() << std::endl;
  std::vector<I2Point> output;
  for (auto u : vlines[0]) output.push_back(vp[u]);
  return output;
}
}  // namespace spaceship
