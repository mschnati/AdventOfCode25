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
#include <cmath>
#include <random>
#include <functional>

struct Manual {
    int16_t lights; // target bitmask
    int16_t len;    // num lights
    std::vector<std::vector<int16_t>> buttons;
    std::vector<int64_t> joltages;
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
        for (const auto& btn : m.buttons) {
            out = std::format_to(out, "(");
            for (size_t i = 0; i < btn.size(); ++i) {
                if (i > 0) out = std::format_to(out, ",");
                out = std::format_to(out, "{}", btn[i]);
            }
            out = std::format_to(out, ") ");
        }
        out = std::format_to(out, "{{");
        for (size_t i = 0; i < m.joltages.size(); ++i) {
            if (i > 0) out = std::format_to(out, ",");
            out = std::format_to(out, "{}", m.joltages[i]);
        }
        return std::format_to(out, "}}");
    }
};

// Solve Part 2 via simplex + branch-and-bound ILP
// Minimize sum of button presses  
int64_t solve_manual(const Manual& m) {
    constexpr double INF = 1e100;
    constexpr double EPS = 1e-9;

    const int n = static_cast<int>(m.joltages.size());
    const int vars = static_cast<int>(m.buttons.size());
    const int rows = 2 * n + vars;

    if (n == 0 || vars == 0) return 0;

    std::vector<std::vector<double>> A(rows, std::vector<double>(vars + 1, 0.0));

    // Button non-negativity: -x_i <= 0
    for (int i = 0; i < vars; ++i) {
        const int row = rows - i - 1;
        A[row][i] = -1.0;
    }

    // Equality constraints for each counter
    for (int i = 0; i < vars; ++i) {
        for (int idx : m.buttons[i]) {
            if (idx < 0 || idx >= n) continue;
            A[idx][i] += 1.0;       // sum x <= c
            A[idx + n][i] -= 1.0;   // -sum x <= -c
        }
    }
    for (int i = 0; i < n; ++i) {
        A[i][vars] = static_cast<double>(m.joltages[i]);
        A[i + n][vars] = -static_cast<double>(m.joltages[i]);
    }

    auto simplex = [&](const std::vector<std::vector<double>>& A,
                       const std::vector<double>& C,
                       std::vector<double>& x) -> double {
        const int mrows = static_cast<int>(A.size());
        const int nvars = static_cast<int>(A[0].size()) - 1;

        std::vector<int> N(nvars + 1), B(mrows);
        for (int i = 0; i < nvars; ++i) N[i] = i;
        N[nvars] = -1;
        for (int i = 0; i < mrows; ++i) B[i] = nvars + i;

        std::vector<std::vector<double>> D(mrows + 2, std::vector<double>(nvars + 2, 0.0));
        for (int i = 0; i < mrows; ++i) {
            for (int j = 0; j < nvars; ++j) D[i][j] = A[i][j];
            D[i][nvars] = A[i][nvars];
            D[i][nvars + 1] = -1.0;
        }
        for (int j = 0; j < nvars; ++j) D[mrows][j] = C[j];

        for (int i = 0; i < mrows; ++i) {
            std::swap(D[i][nvars], D[i][nvars + 1]);
        }

        D[mrows + 1][nvars] = 1.0;

        auto pivot = [&](int r, int s) {
            const double k = 1.0 / D[r][s];
            for (int i = 0; i < mrows + 2; ++i) {
                if (i == r) continue;
                for (int j = 0; j < nvars + 2; ++j) {
                    if (j == s) continue;
                    D[i][j] -= D[r][j] * D[i][s] * k;
                }
            }
            for (int j = 0; j < nvars + 2; ++j) D[r][j] *= k;
            for (int i = 0; i < mrows + 2; ++i) D[i][s] *= -k;
            D[r][s] = k;
            std::swap(B[r], N[s]);
        };

        auto find = [&](int p) -> bool {
            while (true) {
                int s = -1;
                for (int i = 0; i < nvars + 1; ++i) {
                    if (!p && N[i] == -1) continue;
                    if (s == -1 || D[mrows + p][i] < D[mrows + p][s] - EPS ||
                        (std::abs(D[mrows + p][i] - D[mrows + p][s]) <= EPS && N[i] < N[s])) {
                        s = i;
                    }
                }
                if (D[mrows + p][s] > -EPS) return true;

                int r = -1;
                for (int i = 0; i < mrows; ++i) {
                    if (D[i][s] <= EPS) continue;
                    double ratio = D[i][nvars + 1] / D[i][s];
                    if (r == -1 || ratio < D[r][nvars + 1] / D[r][s] - EPS ||
                        (std::abs(ratio - D[r][nvars + 1] / D[r][s]) <= EPS && B[i] < B[r])) {
                        r = i;
                    }
                }
                if (r == -1) return false;
                pivot(r, s);
            }
        };

        int r = 0;
        for (int i = 1; i < mrows; ++i) {
            if (D[i][nvars + 1] < D[r][nvars + 1]) r = i;
        }

        if (D[r][nvars + 1] < -EPS) {
            pivot(r, nvars);
            if (!find(1) || D[mrows + 1][nvars + 1] < -EPS) return -INF;
        }

        for (int i = 0; i < mrows; ++i) {
            if (B[i] == -1) {
                int s = 0;
                for (int j = 1; j < nvars; ++j) {
                    if (D[i][j] < D[i][s] - EPS ||
                        (std::abs(D[i][j] - D[i][s]) <= EPS && N[j] < N[s])) {
                        s = j;
                    }
                }
                pivot(i, s);
            }
        }

        if (!find(0)) return -INF;

        x.assign(nvars, 0.0);
        for (int i = 0; i < mrows; ++i) {
            if (0 <= B[i] && B[i] < nvars) x[B[i]] = D[i][nvars + 1];
        }

        double val = 0.0;
        for (int i = 0; i < nvars; ++i) val += C[i] * x[i];
        return val;
    };

    const std::vector<double> C(vars, 1.0);
    double best = INF;

    std::function<void(const std::vector<std::vector<double>>&)> branch;
    branch = [&](const std::vector<std::vector<double>>& curA) {
        std::vector<double> x;
        double val = simplex(curA, C, x);
        if (val == -INF || val + EPS >= best) return;

        int k = -1;
        double v = 0.0;
        for (int i = 0; i < vars; ++i) {
            double r = std::round(x[i]);
            if (std::abs(x[i] - r) > EPS) {
                k = i;
                v = std::floor(x[i]);
                break;
            }
        }

        if (k == -1) {
            best = val;
            return;
        }

        std::vector<double> s(vars + 1, 0.0);
        s[k] = 1.0;
        s[vars] = v;
        auto leftA = curA;
        leftA.push_back(s);
        branch(leftA);

        s[k] = -1.0;
        s[vars] = -v - 1.0;
        auto rightA = curA;
        rightA.push_back(s);
        branch(rightA);
    };

    branch(A);
    return static_cast<int64_t>(std::llround(best));
}

int main(int argc, char** argv) {
    const std::filesystem::path input_path =
        (argc > 1)  ? std::filesystem::path{argv[1]}
                    : std::filesystem::path{"../inputs/input_10.txt"};

    std::ifstream input_file{input_path};
    if (!input_file.is_open()) {
        std::println(stderr, "Failed to open input file: {}", input_path.string());
        return EXIT_FAILURE;
    }
        std::vector<Manual> manuals{};
    manuals.reserve(200);
    std::string line;

    // Parsing
    while (getline(input_file, line)) {
        if (line.empty()) continue;
        Manual man{};
        auto pos = line.find(' ');

        constexpr char on{'#'};

        // Parse lights into bitmask for Part 1
        man.lights = 0;
        man.len = pos - 2;
        for (size_t i = 1; i < pos - 1; i++) {
            if (line[i] == on) {
                man.lights = man.lights | (1 << (man.len - i));
            }
        }

        auto prev_pos = pos + 1;
        pos = line.find(' ', prev_pos);

        // Helper to parse comma-separated lists "(1,2,3)" or "{1,2,3}"
        auto parse_indices = []<typename T>(std::string_view sv) -> std::vector<T> {
            std::vector<T> result;
            const char* ptr = sv.data() + 1; // skip ( or {
            const char* end = sv.data() + sv.size() - 1; // skip ) or }

            while (ptr < end) {
                T val; 
                auto [next_ptr, ec] = std::from_chars(ptr, end, val);
                if (ec == std::errc()) {
                    result.push_back(val);
                    ptr = next_ptr;
                    if (ptr < end && *ptr == ',') ptr++;
                } else break;
            }
            return result;
        };

        // Parse buttons: (1,3) (2) ...
        while (pos != std::string::npos) {
            std::string_view chunk(line.data() + prev_pos, pos - prev_pos);
            man.buttons.push_back(parse_indices.operator()<int16_t>(chunk));
            
            prev_pos = pos + 1;
            pos = line.find(' ', prev_pos);
        }

        // Parse joltages: {3,5,4,7}
        std::string_view last(line.data() + prev_pos, line.size() - prev_pos);
        man.joltages = parse_indices.operator()<int64_t>(last); 

        manuals.push_back(man);
    }

    // Part 1: BFS with XOR
    int32_t part1{0};
    for (const auto& man : manuals) {
        if (man.lights == 0) continue; // Part 1 only cares about getting lights to 0

        std::vector<bool> visited(65536, false);
        std::deque<int16_t> q;
        
        q.push_back(0);
        visited[0] = true;

        int16_t presses{0};
        bool found{false};

        while (!found && !q.empty()) {
            presses++;
            size_t in_q = q.size();
            while (in_q--) {
                int16_t curr = q.front();
                q.pop_front();

                for (const auto& btn_indices : man.buttons) {
                    // Convert Indices back to Mask
                    int16_t btn_mask = 0;
                    for (int idx : btn_indices) {
                        if (idx < man.len) // Safety check
                            btn_mask |= (1 << (man.len - idx - 1));
                    }

                    int16_t next_state = curr ^ btn_mask;
                    if (next_state == man.lights) {
                        found = true;
                        part1 += presses;
                        break;
                    }

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

    // Part 2: Linear Equations. Joltages are counters that need to be reached by button presses
    int64_t part2{0};
    for (const auto& man : manuals) {
        int64_t t = solve_manual(man);
        // std::println("{}\nPresses for Joltages {}", man, t);
        part2 += t;
    }

    std::println("Part 2: {}", part2);

    return EXIT_SUCCESS;
}
