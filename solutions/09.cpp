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
#include <algorithm>

struct Tile {
    int64_t x;
    int64_t y;
};

struct Box {
    int64_t x, y, u, v; // min_x, min_y, max_x, max_y

    // construct normalized box from two points
    static Box from(Tile p1, Tile p2) {
        return {
            std::min(p1.x, p2.x), std::min(p1.y, p2.y),
            std::max(p1.x, p2.x), std::max(p1.y, p2.y)
        };
    }

    int64_t area() const { 
        return (u - x + 1) * (v - y + 1); 
    }
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

    std::vector<Box> lines;
    std::vector<Box> pairs;
    size_t n = tiles.size();

    // generate lines (pairwise in order + wrap around) as Boxes
    for (size_t i = 0; i < n; ++i) {
        lines.push_back(Box::from(tiles[i], tiles[(i + 1) % n]));
    }

    // generate pairs (combinations) as Boxes and sort by area
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            pairs.push_back(Box::from(tiles[i], tiles[j]));
        }
    }
    std::sort(pairs.begin(), pairs.end(), [](const Box& a, const Box& b) {
        return a.area() > b.area();
    });

    std::println("Part 1: {}", pairs[0].area());

    // Part 2: Find largest rectangle that does not overlap with any line's bounding box
    for (const auto& rect : pairs) {
        bool overlap = false;
        
        for (const auto& line : lines) {
            if (line.x < rect.u && line.y < rect.v && line.u > rect.x && line.v > rect.y) {
                overlap = true;
                break;
            }
        }

        if (!overlap) {
            std::println("Part 2: {}", rect.area());
            break; 
        }
    }

    return EXIT_SUCCESS;
}
