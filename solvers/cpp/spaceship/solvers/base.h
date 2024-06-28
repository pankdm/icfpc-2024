#pragma once

#include "spaceship/evaluator.h"
#include "spaceship/problem.h"
#include "spaceship/solution.h"

#include "common/solvers/solver.h"

namespace spaceship {
using BaseSolver = solvers::Solver<Problem, Solution, Evaluator>;
}  // namespace spaceship
