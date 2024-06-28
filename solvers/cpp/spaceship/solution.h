#pragma once

#include "common/files/file_to_string.h"
#include "common/solvers/solution.h"

#include <fstream>
#include <iostream>
#include <string>

namespace spaceship {
class Solution : public solvers::Solution {
 public:
  std::string commands;

 public:
  bool Empty() const { return commands.empty(); }
  void Clear() { commands.clear(); }

  static std::string FileName(const std::string& id,
                              const std::string& solver_name) {
    return "../../solutions/spaceship/" + solver_name + "/" + id + ".txt";
  }

  bool Load(const std::string& id, const std::string& solver_name) {
    SetId(id);
    auto filename = FileName(GetId(), solver_name);
    commands = files::FileToString(filename);
    return !commands.empty();
  }

  void Save(const std::string& solver_name) const {
    auto filename = FileName(GetId(), solver_name);
    std::ofstream f(filename);
    f << commands;
  }
};
}  // namespace spaceship
