// Advent of Code Day 6
// https://adventofcode.com/2025/day/6

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

enum Operator {
    TIMES,
    PLUS
};

struct Problem {
    std::vector<int64_t> nums;
    Operator op;
    int32_t start;
};

template <>
struct std::formatter<Problem> : std::formatter<std::string> {
    auto format(const Problem& p, std::format_context& ctx) const {
        char op_char = (p.op == TIMES) ? '*' : '+';
        auto out = std::format_to(ctx.out(), "Op: '{}', Nums: [", op_char);

        for (size_t i = 0; i < p.nums.size(); ++i) {
            if (i > 0) out = std::format_to(out, ", ");
            out = std::format_to(out, "{}", p.nums[i]);
        }
        return std::format_to(out, "]");
    }
};

int main(int argc, char** argv) {
    const std::filesystem::path input_path =
        (argc > 1)  ? std::filesystem::path{argv[1]}
                    : std::filesystem::path{"../inputs/input_06.txt"};

    std::ifstream input_file{input_path};
    if (!input_file.is_open()) {
        std::println(stderr, "Failed to open input file: {}", input_path.string());
        return EXIT_FAILURE;
    }

    constexpr auto add{'+'};
    constexpr auto mult{'*'};

    std::vector<std::string> homework;
    std::string l;
    while(getline(input_file, l)) homework.push_back(l);
    std::string op_str = homework.back();

    std::vector<Problem> problems{};
    for (auto line : homework) {
        const char* ptr = line.data();
        const char* end = line.data() + line.size();
        size_t col_idx{0};

        while (ptr < end) {
            while (ptr < end && *ptr == ' ') {
                ptr++;
            }
            if (ptr == end) break;

            int64_t num;
            auto [next_ptr, ec] = std::from_chars(ptr, end, num);

            if (ec == std::errc()) {
                if (col_idx >= problems.size()) {
                    problems.push_back({});
                }
                problems[col_idx].nums.push_back(num);
                ptr = next_ptr;
            } else {
                *ptr == add ? problems[col_idx].op = PLUS : problems[col_idx].op = TIMES;
                ptr++;
            }
            col_idx++;
        }
    }

    int64_t part1{0};
    for (const auto& problem : problems) {
        std::println("{}", problem);
        int64_t problem_res{0};
        if (problem.op == TIMES) {
            problem_res = 1;
            for (auto num : problem.nums)
                problem_res *= num;
        } else {
            for (auto num : problem.nums)
                problem_res += num;
        }
        part1 += problem_res;
    }
    std::println("Part 1: {}", part1);

    // Part 2: Read numbers from top to bottom in a straight line. So whitespace matters

    // parse operations to find index bounds
    std::vector<Problem> prob_2{};
    for (int i = 0; i < op_str.length(); i++) {
        if (op_str[i] == add) {
            prob_2.push_back({});
            prob_2.back().op = PLUS;
            prob_2.back().start = i;
        } else if (op_str[i] == mult) {
            prob_2.push_back({});
            prob_2.back().op = TIMES;
            prob_2.back().start = i;
        }
    }
    prob_2.push_back({});
    prob_2.back().start = op_str.length() + 1;

    // iterate column by column and add the result based on the operation
    int64_t part2{0};
    for (int i = 0; i < prob_2.size() - 1; i++) {
        int64_t problem_res{ prob_2[i].op == TIMES ? 1 : 0 };
        for (int j = prob_2[i].start; j < prob_2[i+1].start - 1; j++) {
            int64_t column{0};
            for (int row = 0; row < homework.size() - 1; row++) {
                if (homework[row][j] != ' ') {
                    column = column * 10 + (homework[row][j] - '0');
                }
            }

            if (prob_2[i].op == TIMES) {
                problem_res *= column;
            } else {
                problem_res += column;
            }
        }

        part2 += problem_res;
    }

    std::println("Part 2: {}", part2);

    return EXIT_SUCCESS;
}
