// Advent of Code Day 8
// https://adventofcode.com/2025/day/8

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
#include <tuple>

struct JBox {
    int64_t x;
    int64_t y;
    int64_t z;
};

struct Edge {
    size_t u;
    size_t v;
    int64_t dist;

    auto operator<=>(const Edge&) const = default;
};

// disjoint set union for union find to track circuits 
// Union-Find maintains partition of elements into disjoint sets (trees)
// find() uses path compression to flatten the tree for ~O(1) lookup
// unite() uses union-by-size to attach the smaller tree to the larger one, keeping trees balanced
struct DSU {
    std::vector<int> parent;
    std::vector<int> size;

    DSU(int n) {
        parent.resize(n);
        std::iota(parent.begin(), parent.end(), 0);
        size.assign(n, 1);
    }

    // returns the root of the set containing i, while applying path compression
    int find(int i) {
        if (parent[i] == i)
            return i;
        return parent[i] = find(parent[i]);
    }

    // unites the sets containing i and j using union by size
    int unite(int i, int j) {
        i = find(i);
        j = find(j);
        if (i != j) {
            if (size[i] < size[j])
                std::swap(i, j);
            parent[j] = i;
            size[i] += size[j];
        }
        return size[i];
    }
};

int64_t dist(const JBox& p, const JBox& q) {
    // we just need to compare the distances, so doing the expensive sqrt is actually not necessary,
    // as its monotonic
    return (p.x - q.x)*(p.x - q.x) + (p.y - q.y)*(p.y - q.y) + (p.z - q.z)*(p.z - q.z);
}

int main(int argc, char** argv) {
    const std::filesystem::path input_path =
        (argc > 1)  ? std::filesystem::path{argv[1]}
                    : std::filesystem::path{"../inputs/input_08.txt"};

    std::ifstream input_file{input_path};
    if (!input_file.is_open()) {
        std::println(stderr, "Failed to open input file: {}", input_path.string());
        return EXIT_FAILURE;
    }
    
    std::string line;
    std::vector<JBox> junctions{};
    junctions.reserve(1000);
    // not the cleanest looking but probably the fastest, as the input format is known
    while (getline(input_file, line)) {
        if (line.empty()) break;
        JBox j;
        const char* ptr = line.data();
        const char* end = ptr + line.size();

        auto [ptr1, ec1] = std::from_chars(ptr, end, j.x);
        auto [ptr2, ec2] = std::from_chars(ptr1 + 1, end, j.y);
        std::from_chars(ptr2 + 1, end, j.z);

        junctions.push_back(j);
    }

    // generate all edges
    std::vector<Edge> edges;
    // pre-calculate size: N * (N-1) / 2
    size_t n = junctions.size();
    edges.reserve(n * (n - 1) / 2);
    for (size_t i = 0; i < n; i++) {
        for (size_t j = i + 1; j < n; j++) {
            edges.push_back({i, j, dist(junctions[i], junctions[j])});
        }
    }

    // sort by distance
    std::sort(edges.begin(), edges.end(), [](const Edge& a, const Edge& b) {
        return a.dist < b.dist;
    });

    DSU dsu(n);
    // connect 1000 closest
    std::println("Total edges: {}", edges.size());
    size_t limit = (junctions.size() == 1000) ? 1000 : 10;

    for (size_t i = 0; i < limit; i++) {
        dsu.unite(edges[i].u, edges[i].v);
    }

    std::vector<int64_t> circuit_sizes;
    circuit_sizes.reserve(n);
    for (int i = 0; i < n; ++i) {
        // Since we tracked size in the root, we only take sizes from root nodes
        if (dsu.parent[i] == i) {
            circuit_sizes.push_back(dsu.size[i]);
        }
    }

    // Sort descending to find largest clusters
    std::sort(circuit_sizes.rbegin(), circuit_sizes.rend());

    int64_t result = circuit_sizes[0] * circuit_sizes[1] * circuit_sizes[2];
    std::println("Top 3 cluster sizes: {}, {}, {}", circuit_sizes[0], circuit_sizes[1], circuit_sizes[2]);
    std::println("Part 1: {}", result);

    // part 2: connect until there is only one set. return multiplication of x-coords of the last two connected ones
    for (size_t i = limit; i < edges.size(); i++) {
        if (dsu.unite(edges[i].u, edges[i].v) == n) {
            int64_t x1{junctions[edges[i].u].x};
            int64_t x2{junctions[edges[i].v].x};
            std::println("Part 2: {} * {} = {}", x1, x2, x1*x2);
            break;
        }
    }

    return EXIT_SUCCESS;
}
