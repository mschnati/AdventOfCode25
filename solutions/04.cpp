// Advent of Code Day 4
// https://adventofcode.com/2025/day/4

#include <cstdint>
#include <expected>
#include <filesystem>
#include <fstream>
#include <print>
#include <string>
#include <string_view>
#include <array>
#include <vector>
#include <cstdlib>

struct Grid {
    std::vector<std::string> grid;
    std::int32_t width;
    std::int32_t height;
};

static bool inBounds(const Grid& g, int x, int y) {
    return x >= 0 && y >= 0 && x < g.width && y < g.height;
}

int countAdjacent(const Grid& g, int x, int y, char value) {
    static constexpr std::array<std::pair<int,int>, 8> dirs{{
        {-1, -1}, {0, -1}, {1, -1},
        {-1,  0},          {1,  0},
        {-1,  1}, {0,  1}, {1,  1},
    }};
    int count{0};

    for (auto [dx, dy] : dirs) {
        const int nx = x + dx;
        const int ny = y + dy;
        if (inBounds(g, nx, ny) && g.grid[ny][nx] == value)
            count++;
    }

    return count;
}

int main(int argc, char** argv) {
    using namespace std;
    const filesystem::path input_path =
        (argc > 1)  ? filesystem::path{argv[1]}
                    : filesystem::path{"../inputs/input_04.txt"};

    ifstream input_file{input_path};
    if (!input_file.is_open()) {
        println(stderr, "Failed to open input file: {}", input_path.string());
        return EXIT_FAILURE;
    }

    const char paper{'@'};
    const char empty{'.'};

    Grid input{};
    string line;
    while (getline(input_file, line)) {
        input.grid.push_back(line);
    }
    input.width = static_cast<int>(input.grid[0].size());
    input.height = static_cast<int>(input.grid.size());

    println("width: {}", input.width);
    println("height: {}", input.height);

    int paper_count{0};
    bool removed = false;
    bool part2 = false;
    do {
        removed = false;
        for (int i = 0; i < input.height; i++) {
            for (int j = 0; j < input.width; j++) {
                if (input.grid[i][j] != paper) continue;
                if (countAdjacent(input, j, i, paper) < 4) {
                    paper_count++;
                    if (part2) {
                        input.grid[i][j] = empty;
                        removed = true;
                    }
                }
            }
        }
        if (!part2) {
            println("Part 1: {}", paper_count);
            paper_count = 0;
            removed = true;
            part2 = true;
        }

    } while (removed);

    println("Part 2: {}", paper_count);

    return EXIT_SUCCESS;
}
