// Advent of Code Day 5
// https://adventofcode.com/2025/day/5

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

struct Range {
    int64_t start;
    int64_t end;
    auto operator<=>(const Range&) const = default;
};

int main(int argc, char** argv) {
    const std::filesystem::path input_path =
        (argc > 1)  ? std::filesystem::path{argv[1]}
                    : std::filesystem::path{"../inputs/input_05.txt"};

    std::ifstream input_file{input_path};
    if (!input_file.is_open()) {
        std::println(stderr, "Failed to open input file: {}", input_path.string());
        return EXIT_FAILURE;
    }

    std::vector<Range> ranges{};
    ranges.reserve(200);
    std::string line;
    while (getline(input_file, line)) {
        if (line.empty()) break;
        Range r;
        auto [ptr, ec] = std::from_chars(line.data(), line.data() + line.size(), r.start);
        std::from_chars(ptr + 1, line.data() + line.size(), r.end);
        ranges.push_back(r);
    }

    // preload ingredients
    std::vector<int64_t> ingredients;
    std::string ingredient_line;
    while (getline(input_file, ingredient_line)) {
        int64_t id;
        std::from_chars(ingredient_line.data(), ingredient_line.data() + ingredient_line.size(), id);
        ingredients.push_back(id);
    }

    /* The numbers are very large so saving every ingredient doesnt seem reasonable. 
     * Sort ranges and combine them in new vector to improve speed.
     * While bigger than start -> fresh if smaller than end.
     * If smaller than start -> spoiled.
    */
    std::sort(ranges.begin(), ranges.end());

    std::vector<Range> merged;
    merged.reserve(ranges.size());
    merged.push_back(ranges[0]);
    
    for (size_t i = 1; i < ranges.size(); i++) {
        auto& last = merged.back();

        // if overlap or adjacent, merge
        if (ranges[i].start <= last.end) {
            last.end = std::max(last.end, ranges[i].end);
        } else {
            merged.push_back(ranges[i]);
        }
    }
    
    // measuring difference in speed between linear and binary out of curiousity
    // binary ~2x the speed even with such a small sample size
    auto start_time = std::chrono::high_resolution_clock::now();

    int64_t count{0};
    for (int64_t id : ingredients) {
        /* ~146 micro seconds */
        // for (auto range : merged) {
        //     if (id < range.start) break;
        //     if (id <= range.end) {
        //         count++;
        //         break;
        //     }
        // }

        /* ~78 micro seconds */
        auto it = std::upper_bound(merged.begin(), merged.end(), id, 
            [](int64_t val, const Range& r) { return val < r.start; });

        if (it != merged.begin()) {
            if (id <= std::prev(it)->end) {
                count++;
            }
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    std::println("Part 1: {}", count);
    std::println("Search duration: {} microseconds", duration.count());

    // part 2: count how many IDs are considered fresh
    int64_t part2{0};
    for (auto const range : merged) {
        part2 += range.end - range.start + 1;
    }
    std::println("Part 2: {}", part2);


    return EXIT_SUCCESS;
}
