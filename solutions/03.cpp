// Advent of Code Day 3
// https://adventofcode.com/2025/day/3

#include <cstdint>
#include <expected>
#include <filesystem>
#include <fstream>
#include <print>
#include <string>
#include <string_view>
#include <array>

namespace {
template <std::size_t N>
std::expected<std::int64_t, std::string> best_digits_value(std::string_view line) {
    if (line.size() < N) {
        return std::unexpected("line too short");
    }
    auto digit = [](char c) -> std::expected<std::int64_t, std::string> {
        if (c < '0' || c > '9') {
            return std::unexpected("non-digit");
        }
        return static_cast<std::int64_t>(c - '0');
    };

    std::array<std::int64_t, N> joltages{};
    auto first = digit(line[0]);
    if (!first) {
        return std::unexpected("invalid digits");
    }
    joltages[0] = *first;

    // search biggest possible digit for current index, search next digit starting from there
    std::int32_t current = 1;
    for (std::size_t result_idx = 0; result_idx < N; result_idx++) {
        auto last_possible_idx = static_cast<std::int32_t>(line.size() - (N - result_idx));
        for (std::int32_t line_idx = current; line_idx <= last_possible_idx; line_idx++) {
            auto value = digit(line[line_idx]);
            if (!value) {
                return std::unexpected("invalid digits");
            }
            if (*value > joltages[result_idx]) {
                joltages[result_idx] = *value;
                current = line_idx + 1;
                if (joltages[result_idx] == 9) break;
            }
        }
    }

    std::int64_t line_value = 0;
    for (std::int64_t d : joltages) {
        line_value = line_value * 10 + d;
    }
    return line_value;
}
} // namespace

int main(int argc, char** argv) {
    const std::filesystem::path input_path =
        (argc > 1)  ? std::filesystem::path{argv[1]}
                    : std::filesystem::path{"../inputs/input_03.txt"};

    std::ifstream input{input_path};
    if (!input.is_open()) {
        std::println(stderr, "Failed to open input file: {}", input_path.string());
        return EXIT_FAILURE;
    }

    std::string line;
    std::int64_t total_part_1 = 0;
    std::int64_t total_part_2 = 0;
    std::size_t line_number = 0;

    auto report_error = [&](const std::string& error) -> std::expected<std::int64_t, std::string> {
        std::println(stderr, "Skipping invalid line {}: {}", line_number, error);
        return std::unexpected(error);
    };

    while (std::getline(input, line)) {
        line_number++;

        auto part1 = best_digits_value<2>(line).or_else(report_error);
        if (part1) {
            total_part_1 += *part1;
        }

        // part 2: 12 digits instead of 2
        auto part2 = best_digits_value<12>(line).or_else(report_error);
        if (part2) {
            total_part_2 += *part2;
        }
    }

    std::println("Part 1: {}", total_part_1);
    std::println("Part 2: {}", total_part_2);

    return EXIT_SUCCESS;
}
