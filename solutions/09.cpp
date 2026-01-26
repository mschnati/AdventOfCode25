// Advent of Code Day 9
// https://adventofcode.com/2025/day/9

#include <iomanip>
#include <iostream>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <fstream>
#include <print>
#include <string>
#include <string_view>
#include <ranges>
#include <array>
#include <vector>
#include <cstdlib>
#include <charconv>
#include <numeric>

enum Corner {
    NONE = -1,
    TOP_L = 0,
    TOP_R,
    BOT_L,
    BOT_R,
};

struct Tile {
    int64_t x;
    int64_t y;
};

int main(int argc, char** argv) {
    const std::filesystem::path input_path =
        (argc > 1)  ? std::filesystem::path{argv[1]}
                    : std::filesystem::path{"../inputs/input_09.txt"};

    std::ifstream input_file{input_path};
    if (!input_file.is_open()) {
        std::println(stderr, "Failed to open input file: {}", input_path.string());
        return EXIT_FAILURE;
    }

    std::string line;
    std::vector<Tile> tiles{};
    tiles.reserve(500);

    Tile tile;
    // read all tiles
    while (getline(input_file, line)) {
        if (line.empty()) continue;
        auto [ptr, ec] = std::from_chars(line.data(), line.data() + line.size(), tile.x);
        std::from_chars(ptr + 1, line.data() + line.size(), tile.y);
        tiles.push_back(tile);
    }

    if (tiles.empty()) return EXIT_SUCCESS;

    // Brute Force: Check every pair of points
    int64_t max_area = 0;

    for (size_t i = 0; i < tiles.size(); ++i) {
        for (size_t j = i + 1; j < tiles.size(); ++j) {
            const auto& p1 = tiles[i];
            const auto& p2 = tiles[j];

            int64_t width = std::abs(p1.x - p2.x) + 1;
            int64_t height = std::abs(p1.y - p2.y) + 1;
            
            int64_t area = width * height;
            
            if (area > max_area) {
                max_area = area;
            }
        }
    }

    std::println("Part 1: {}", max_area);

    return EXIT_SUCCESS;
}
