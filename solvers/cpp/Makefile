SOLVER := spaceship

define USAGE
usage:
  - make -- see this message
  - make SOLVER=solver_name -- compile given solver
endef
export USAGE

compile:
	g++ -std=c++17 -I "./" -O3 -Wall -DNDEBUG ./${SOLVER}/main.cpp -o ./${SOLVER}.solver

compile-fast:
	g++ -std=c++17 -I "./" -O0 -Wall -DNDEBUG ./${SOLVER}/main.cpp -o ./${SOLVER}.solver


usage:
	@echo "$$USAGE"

install-macos:
	which g++ || brew install gcc g++ cmake
