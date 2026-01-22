// Advent of Code Day 3
// https://adventofcode.com/2025/day/3

#include <cstdint>
#include <expected>
#include <filesystem>
#include <fstream>
#include <print>
#include <string>
#include <string_view>

namespace {
struct LineResult {
    std::int32_t first{};
    std::int32_t second{};
};

std::expected<LineResult, std::string> parse_line(std::string_view line) {
    if (line.size() < 2) {
        return std::unexpected("line too short");
    }
    auto digit = [](char c) -> std::expected<std::int32_t, std::string> {
        if (c < '0' || c > '9') {
            return std::unexpected("non-digit");
        }
        return static_cast<std::int32_t>(c - '0');
    };

    auto first = digit(line[0]);
    auto second = digit(line[1]);
    if (!first || !second) {
        return std::unexpected("invalid digits");
    }

    LineResult result{*first, *second};
    for (std::size_t i = 2; i < line.size(); ++i) {
        auto value = digit(line[i]);
        if (!value) {
            return std::unexpected("invalid digits");
        }
        if (*value > result.first && i + 1 < line.size()) {
            result.first = *value;
            result.second = 0;
        } else if (*value > result.second) {
            result.second = *value;
        }
    }

    return result;
}
} // namespace

int main(int argc, char** argv) {
    const std::filesystem::path input_path =
        (argc > 1) ? std::filesystem::path{argv[1]}
                   : std::filesystem::path{"../inputs/input_03.txt"};

    std::ifstream input{input_path};
    if (!input.is_open()) {
        std::println(stderr, "Failed to open input file: {}", input_path.string());
        return EXIT_FAILURE;
    }

    std::string line;
    std::int32_t total = 0;
    std::size_t line_number = 0;

    while (std::getline(input, line)) {
        line_number++;
        auto value = parse_line(line)
            .transform([](const LineResult& result) {
                return result.first * 10 + result.second;
            })
            .or_else([&](const std::string& error) -> std::expected<int, std::string> {
                std::println(stderr, "Skipping invalid line {}: {}", line_number, error);
                return std::unexpected(error);
            });
        if (value) {
            total += *value;
        }
    }

    std::println("Part 1: {}", total);
    
    return EXIT_SUCCESS;
}
