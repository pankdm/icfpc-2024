#include "spaceship/constants.h"
#include "spaceship/evaluate_solution.h"
#include "spaceship/evaluator.h"
#include "spaceship/problem.h"
#include "spaceship/solution.h"
#include "spaceship/solvers/base.h"
#include "spaceship/solvers/greedy1.h"
#include "spaceship/solvers/greedy1d.h"
#include "spaceship/solvers/greedy1s.h"
#include "spaceship/solvers/greedy2.h"
#include "spaceship/solvers/greedy2a.h"

#include "common/files/command_line.h"
#include "common/solvers/ext/run_n.h"

#include <memory>

void InitCommaneLine(files::CommandLine& cmd) {
  cmd.AddArg("mode", "eval");
  cmd.AddArg("solution", "best");
  cmd.AddArg("solver", "greedy1");
  cmd.AddArg("timelimit", 125);
  cmd.AddArg("nthreads", 4);
  cmd.AddArg("first_problem", 1);
  cmd.AddArg("last_problem", spaceship::last_problem);
}

spaceship::BaseSolver::PSolver CreateSolver(const files::CommandLine& cmd,
                                            const std::string& solver_name) {
  auto timelimit = cmd.GetInt("timelimit");
  if (solver_name == "greedy1") {
    return std::make_shared<spaceship::Greedy1>(timelimit);
  } else if (solver_name == "greedy1d") {
    return std::make_shared<spaceship::Greedy1D>(timelimit);
  } else if (solver_name == "greedy1s") {
    return std::make_shared<spaceship::Greedy1S>(timelimit);
  } else if (solver_name == "greedy2") {
    return std::make_shared<spaceship::Greedy2>(timelimit);
  } else if (solver_name == "greedy2a") {
    return std::make_shared<spaceship::Greedy2A>(timelimit);
  } else {
    std::cerr << "Unknown solver type: " << solver_name << std::endl;
    exit(-1);
  }
}

int main(int argc, char** argv) {
  files::CommandLine cmd;
  InitCommaneLine(cmd);
  cmd.Parse(argc, argv);

  const auto mode = cmd.GetString("mode");
  if (mode == "eval") {
    spaceship::EvaluateSolution(cmd.GetString("solution"));
  } else if (mode == "update") {
    spaceship::UpdateBest(cmd.GetString("solution"));
  } else if (mode == "run") {
    auto solver_name = cmd.GetString("solver");
    auto s = CreateSolver(cmd, solver_name);
    int nthreads = cmd.GetInt("nthreads");
    if (nthreads <= 0)
      solvers::ext::RunN<spaceship::BaseSolver>(*s, cmd.GetInt("first_problem"),
                                                cmd.GetInt("last_problem"));
    else
      solvers::ext::RunNMT<spaceship::BaseSolver>(
          *s, cmd.GetInt("first_problem"), cmd.GetInt("last_problem"),
          nthreads);
  } else {
    std::cerr << "Unknown mode " << mode << std::endl;
  }

  return 0;
}
