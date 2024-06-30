#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <string>
#include <vector>
#include <omp.h>
#include <limits>
#include <bitset>
#include <map>

class Rng {
private:
    long long seed;

public:
    Rng(long long seed) : seed(seed) {}

    int next() {
        int ret = seed;
        seed = (seed * 48271) % ((1LL << 31) - 1);
        // seed = (seed * 3) % ((1LL << 17) - 1);
        return ret;
    }
};

const int max_size = 256 * 256;
const int dx[] = {0, 1, 0, -1};
const int dy[] = {1, 0, -1, 0};

struct MazeState {
    std::vector<std::string> const& maze;
    int x, y;
    std::bitset<max_size> visited;
    std::pair<int, int> last_unvisited;
    int n_unvisited;

    std::vector<std::pair<int, int>> seeds_steps;

    MazeState(std::vector<std::string> const& maze)
            : maze(maze), x(-1), y(-1), last_unvisited(-1, -1), n_unvisited(0) {
        for (int i = 0; i < maze.size(); ++i) {
            for (int j = 0; j < maze[0].size(); ++j) {
                if (maze[i][j] == '.') {
                    ++n_unvisited;
                } else if (maze[i][j] == 'L') {
                    visited[i * maze[0].size() + j] = true;
                    x = i;
                    y = j;
                }
            }
        }
    }

    MazeState(MazeState const& other)
            : maze(other.maze),
              x(other.x),
              y(other.y),
              visited(other.visited),
              last_unvisited(other.last_unvisited),
              n_unvisited(other.n_unvisited),
              seeds_steps(other.seeds_steps) {}
    
    void simulate(int seed, int max_steps) {
        Rng rng(seed);
        int d = 0;
        int steps = 0;

        while (n_unvisited > 0 && (max_steps == -1 || steps < max_steps)) {
            ++steps;

            // d = rng.next() % 4;
            int x1 = x + dx[d], y1 = y + dy[d];

            if (x1 >= 0 && x1 < maze.size() && y1 >= 0 && y1 < maze[0].size() &&
                maze[x1][y1] != '#') {
                x = x1;
                y = y1;
                if (!visited[x * maze[0].size() + y]) {
                    visited[x * maze[0].size() + y] = true;
                    --n_unvisited;
                    last_unvisited = {x, y};
                }
            }

            d = (d + rng.next() % 3 + 3) % 4;
        }

        seeds_steps.emplace_back(seed, steps);
    }

};

std::map<std::pair<int, int>, int> compute_stats(const MazeState& state, int iters) {
    std::map<std::pair<int, int>, int> stats;
    #pragma omp parallel for
    for (int i = 1; i <= iters; ++i) {
        MazeState state_copy(state);
        state_copy.simulate(i, -1);

        #pragma omp critical
        {
            stats[state_copy.last_unvisited]++;
        }
    }

    return stats;
}

int find_best_seed(const MazeState& root_state, int sim_iters, int search_iters, const std::map<std::pair<int, int>, int>& stats) {
    int best_seed = -1;
    double best_score = -1;

    double total_score1 = 0, total_score2 = 0;
    for (auto& [pos, count] : stats) {
        total_score1 += count;
    }
    for (int i = 0; i < root_state.maze.size(); ++i) {
        for (int j = 0; j < root_state.maze[0].size(); ++j) {
            if (root_state.maze[i][j] == '.' && !root_state.visited[i * root_state.maze[0].size() + j]) {
                total_score2 += 1;
            }
        }
    }

#pragma omp parallel for
    for (int i = 1; i <= search_iters; ++i) {
        MazeState state(root_state);
        state.simulate(i, sim_iters);

        double score1 = 0, score2 = 0;
        for (auto& [pos, count] : stats) {
            if (state.visited[pos.first * state.maze[0].size() + pos.second]) {
                score1 += count;
            }
        }
        for (int i = 0; i < state.maze.size(); ++i) {
            for (int j = 0; j < state.maze[0].size(); ++j) {
                if (root_state.maze[i][j] == '.' && !root_state.visited[i * state.maze[0].size() + j] && state.visited[i * state.maze[0].size() + j]) {
                    score2 += 1;
                }
            }
        }
        score1 /= total_score1;
        score2 /= total_score2;
        double score = score1 + score2;

        #pragma omp critical
        {
            if (score > best_score) {
                std::cerr << "New best score: " << score << "(" << score1 << ", " << score2 << ") with seed " << i << std::endl;
                best_score = score;
                best_seed = i;
            }
        }
    }
    
    return best_seed;
}

int main() {
    std::ifstream file("problems/lambdaman/lambdaman19.txt");
    if (!file.is_open()) {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }

    std::vector<std::string> maze;
    std::string line;

    while (std::getline(file, line)) {
        maze.push_back(line);
    }
    MazeState root_state(maze);

    int total_iters = 1000000;
    int splits = 10;
    int stats_iters = 1000;
    int search_iters = 1000000 / 2;
    // int stats_iters = 100;
    // int search_iters = 1000;

    for (int split = 0; split < splits; ++split) {
        std::cerr << "here " << split << " " << root_state.n_unvisited << "\n";
        auto stats = compute_stats(root_state, stats_iters);
        int best_seed = find_best_seed(root_state, total_iters / splits, search_iters, stats);
        
        root_state.simulate(best_seed, total_iters / splits);
    }
    std::cout << root_state.n_unvisited << std::endl;
    for (auto& [seed, steps] : root_state.seeds_steps) {
        std::cout << seed << " " << steps << std::endl;
    }
    return 0;
}