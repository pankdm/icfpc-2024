#include "spaceship/constants.h"
#include "spaceship/evaluate_solution.h"
#include "spaceship/evaluator.h"
#include "spaceship/problem.h"
#include "spaceship/solution.h"
#include "spaceship/solvers/base.h"
#include "spaceship/solvers/dp1.h"
#include "spaceship/solvers/dp1a.h"
#include "spaceship/solvers/dp2.h"
#include "spaceship/solvers/dp2a.h"
#include "spaceship/solvers/greedy1.h"
#include "spaceship/solvers/greedy1d.h"
#include "spaceship/solvers/greedy1s.h"
#include "spaceship/solvers/greedy2.h"
#include "spaceship/solvers/greedy2a.h"
#include "spaceship/solvers/greedy2b.h"
#include "spaceship/solvers/greedy3.h"
#include "spaceship/solvers/greedy3ls.h"
#include "spaceship/solvers/greedyls1.h"
#include "spaceship/solvers/line_sweep1.h"
#include "spaceship/solvers/line_sweep1a.h"
#include "spaceship/solvers/line_sweep2.h"
#include "spaceship/solvers/line_sweep2a.h"
#include "spaceship/solvers/line_sweep2b.h"

#include "common/files/command_line.h"
#include "common/solvers/ext/run_n.h"

#include <memory>

void InitCommaneLine(files::CommandLine& cmd) {
  cmd.AddArg("mode", "eval");
  cmd.AddArg("solution", "best");
  cmd.AddArg("solver", "greedy1");
  cmd.AddArg("timelimit", 125);
  cmd.AddArg("max_extra", 5);
  cmd.AddArg("max_speed_at_stop", 100);
  cmd.AddArg("max_steps_between_points", 100);
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
  } else if (solver_name == "greedy2b") {
    return std::make_shared<spaceship::Greedy2B>(timelimit);
  } else if (solver_name == "greedy3") {
    return std::make_shared<spaceship::Greedy3>(
        cmd.GetInt("max_speed_at_stop"));
  } else if (solver_name == "greedy3ls") {
    return std::make_shared<spaceship::Greedy3LS>(timelimit);
  } else if (solver_name == "greedyls1") {
    return std::make_shared<spaceship::GreedyLS1>(timelimit);
  } else if (solver_name == "dp1") {
    return std::make_shared<spaceship::DP1>(timelimit);
  } else if (solver_name == "dp1a") {
    return std::make_shared<spaceship::DP1A>(timelimit);
  } else if (solver_name == "dp2") {
    return std::make_shared<spaceship::DP2>(timelimit);
  } else if (solver_name == "dp2a") {
    return std::make_shared<spaceship::DP2A>(timelimit);
  } else if (solver_name == "ls1") {
    return std::make_shared<spaceship::LineSweep1>(timelimit);
  } else if (solver_name == "ls1a") {
    return std::make_shared<spaceship::LineSweep1A>(timelimit);
  } else if (solver_name == "ls2") {
    return std::make_shared<spaceship::LineSweep2>(timelimit, cmd.GetInt("max_steps_between_points"), cmd.GetInt("max_extra"));
  } else if (solver_name == "ls2a") {
    return std::make_shared<spaceship::LineSweep2A>(timelimit, cmd.GetInt("max_steps_between_points"), cmd.GetInt("max_extra"));
  } else if (solver_name == "ls2b") {
    return std::make_shared<spaceship::LineSweep2B>(timelimit, cmd.GetInt("max_steps_between_points"), cmd.GetInt("max_extra"));
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
