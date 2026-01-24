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

enum class Tile : char {
    Manifold = 'S',
    Beam = '|',
    Empty = '.',
    Splitter = '^'
};
template <>
struct std::formatter<Tile> : std::formatter<char> {
    auto format(Tile t, std::format_context& ctx) const {
        return std::formatter<char>::format(static_cast<char>(t), ctx);
    }
};

void printGrid(size_t width, size_t height, auto get_tile) {
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            std::print("{}", get_tile(y, x));
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

    // flat contiguous vector with reserved memory for faster access and fewer allocations,
    // but dynamic to work with test and real input
    constexpr size_t h{142};
    constexpr size_t w{141};
    std::vector<Tile> grid{};
    grid.reserve(h * w);
    std::string line;
    size_t width{0};
    size_t height{0};

    while (getline(input_file, line)) {
        if (line.empty()) continue;
        
        if (width == 0) width = line.size();

        for (char c : line) {
            grid.push_back(static_cast<Tile>(c));
        }

        height++;
    }

    std::println("Grid loaded: {} cols x {} rows", width, height);

    // Helper lambda for 2D access 
    auto at = [&](size_t y, size_t x) -> Tile& {
        return grid[y * width + x];
    };

    printGrid(width, height, at);
    int64_t part1{0};
    for (auto [y, x] : std::views::cartesian_product(std::views::iota(0uz, height - 1), std::views::iota(0uz, width))) {
        Tile t = at(y, x);
        if (t == Tile::Manifold || t == Tile::Beam) {
            switch (at(y + 1, x))
            {
                case Tile::Empty:
                    at(y + 1, x) = Tile::Beam;
                    break;
                case Tile::Splitter:
                    at(y + 1, x + 1) = Tile::Beam;
                    at(y + 1, x - 1) = Tile::Beam;
                    part1++;
                    break;
                default:
                    break;
            }
        }
    }

    std::println("Result\n");

    printGrid(width, height, at);

    std::println("Part 1: {}", part1);

    return EXIT_SUCCESS;
}
