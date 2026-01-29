// Advent of Code Day 10
// https://adventofcode.com/2025/day/10

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
#include <deque>

struct Manual {
    int16_t lights;
    int16_t len;
    std::vector<int16_t> buttons;
    std::string joltages;
};

std::string getBinaryRep(int n, int len) {
    std::string ans = "";
    for (int i = len - 1; i >= 0; i--) {
        // If i'th bit is set 
        if (n&(1 << i)) ans += '1';
        else ans += '0';
    }
    return ans;
}

template<>
struct std::formatter<Manual> : std::formatter<std::string> {
    auto format(const Manual& m, std::format_context& ctx) const {
        auto out = std::format_to(ctx.out(), "len: {} [", m.len);
        out = std::format_to(out, "{}", getBinaryRep(m.lights, m.len));
        out = std::format_to(out, "] ");
        for (size_t i = 0; i < m.buttons.size(); i++) {
            out = std::format_to(out, "(");
            out = std::format_to(out, "{}", getBinaryRep(m.buttons[i], m.len));
            out = std::format_to(out, ") ");
        }
        out = std::format_to(out, "{{");
        
        out = std::format_to(out, "{}", m.joltages);
        return std::format_to(out, "}}");
    }
};

int main(int argc, char** argv) {
    const std::filesystem::path input_path =
        (argc > 1)  ? std::filesystem::path{argv[1]}
                    : std::filesystem::path{"../inputs/input_10.txt"};

    std::ifstream input_file{input_path};
    if (!input_file.is_open()) {
        std::println(stderr, "Failed to open input file: {}", input_path.string());
        return EXIT_FAILURE;
    }
    constexpr char off{'.'};
    constexpr char on{'#'};

    std::vector<Manual> manuals{};
    manuals.reserve(200);
    std::string line;
    while (getline(input_file, line)) {
        if (line.empty()) continue;
        Manual man{};
        auto pos = line.find(' ');

        // parse lights into bitmask
        man.lights = 0;
        man.len = pos - 2;
        for (size_t i = 1; i < pos - 1; i++) {
            if (line[i] == on) {
                man.lights = man.lights | 1 << man.len - i;
            }
        }
        auto prev_pos = pos + 1;
        pos = line.find(' ', prev_pos);

        // parse list of ints into bit masked int
        auto parse_list = [](std::string_view sv, int16_t length) -> int16_t {
            int16_t result{0};
            const char* ptr = sv.data() + 1;
            const char* end = sv.data() + sv.size() - 1;

            while (ptr < end) {
                int16_t val;
                auto [next_ptr, ec] = std::from_chars(ptr, end, val);

                if (ec == std::errc()) {
                    result = result | 1 << length - val - 1;
                    ptr = next_ptr;
                    if (ptr < end && *ptr == ',') ptr++;
                } else {
                    break;
                }
            }
            return result;
        };

        // parse buttons
        while (pos != std::string::npos) {
            std::string_view current(line.data() + prev_pos, pos - prev_pos);
            man.buttons.push_back(parse_list(current, man.len));
            prev_pos = pos + 1;
            pos = line.find(' ', prev_pos);
        }

        std::string_view last(line.data() + prev_pos, line.size() - prev_pos);
        man.joltages = last;

        manuals.push_back(man);
    }

    // BFS search, press button by XORing
    int32_t part1{0};
    for (auto man : manuals) {
        if (man.lights == 0) continue;
        // like a hash map to see if state was visited
        std::vector<bool> visited(65535, false);

        int16_t presses{0};
        bool found{false};

        std::deque<int16_t> q;
        q.push_back(0);
        while (!found && !q.empty()){
            presses++;
            // only process the current layer
            size_t in_q = q.size();
            while (in_q--) {
                int16_t curr = q.front();
                q.pop_front();

                for (auto but : man.buttons) {
                    int16_t next_state = curr ^ but;

                    if (next_state == man.lights) {
                        found = true;
                        part1 += presses;
                        break;
                    }
                    // add to queue if not visited yet
                    if (!visited[next_state]) {
                        visited[next_state] = true;
                        q.push_back(next_state);
                    }
                }
                if (found) break;
            }
        }
    }

    std::println("Part 1: {}", part1);

    return EXIT_SUCCESS;
}
