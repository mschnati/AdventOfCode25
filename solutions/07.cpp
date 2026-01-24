// Advent of Code Day 7
// https://adventofcode.com/2025/day/7

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
#include <chrono>

enum Tile {
    Manifold = 1,
    Empty = 0,
    Splitter = -1
};

void printGrid(size_t width, size_t height, auto get_tile) {
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            std::print("{:3}", get_tile(y, x));
        }
        std::println("");
    }
}

int main(int argc, char** argv) {
    const std::filesystem::path input_path =
        (argc > 1)  ? std::filesystem::path{argv[1]}
                    : std::filesystem::path{"../inputs/input_07.txt"};

    std::ifstream input_file{input_path};
    if (!input_file.is_open()) {
        std::println(stderr, "Failed to open input file: {}", input_path.string());
        return EXIT_FAILURE;
    }
    constexpr char man{'S'};
    constexpr char split{'^'};

    // flat contiguous vector with reserved memory for faster access and fewer allocations,
    // but dynamic to work with test and real input
    constexpr size_t h{142};
    constexpr size_t w{141};
    std::vector<int64_t> grid{};
    grid.reserve(h * w);
    std::string line;
    size_t width{0};
    size_t height{0};

    // convert symbols to ints that can be added up to calculate part 2 later
    while (getline(input_file, line)) {
        if (line.empty()) continue;
        if (width == 0) width = line.size();

        for (size_t c = 0; c < width; c++) {
            int32_t t{Tile::Empty};
            
            if (line[c] == man) {
                t = Tile::Manifold;
            } else if (line[c] == split) {
                t = Tile::Splitter;
            }
            grid.push_back(t);
        }
        height++;
    }

    std::println("Grid loaded: {} cols x {} rows", width, height);

    // Helper lambda for 2D access 
    auto at = [&](size_t y, size_t x) -> int64_t& {
        return grid[y * width + x];
    };

    // printGrid(width, height, at);
    int64_t part1{0};
    for (auto [y, x] : std::views::cartesian_product(std::views::iota(0uz, height - 1), std::views::iota(0uz, width))) {
        int64_t t = at(y, x);
        if (t > Tile::Empty) {
            switch (at(y + 1, x))
            {
                case Tile::Splitter:
                    at(y + 1, x + 1) += t;
                    at(y + 1, x - 1) += t;
                    part1++;
                    break;
                default:
                    at(y + 1, x) += t;
                    break;
            }
        }
    }

    // std::println("Result\n");
    // printGrid(width, height, at);
    std::println("Part 1: {}", part1);

    // Part 2: count how many different paths a beam can take
    int64_t part2{0};
    for (size_t x = 0; x < width; x++) {
        part2 += at(height - 1, x);
    }
    std::println("Part 2: {}", part2);

    return EXIT_SUCCESS;
}
