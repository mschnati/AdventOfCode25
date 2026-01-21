# Advent of Code 2025 day 2
# https://adventofcode.com/2025/day/2

with open("inputs/input_02.txt") as f:
    ids = f.read().split(',')

def isValid(id: str, part2 = False) -> int:
    if len(id) % 2 == 1:
        return 0
    
    half = len(id) // 2
    return int(id) if id[:half] == id[half:] else 0

count = 0
for id in ids:
    start, end = id.split('-')
    for i in range(int(start), int(end) + 1):
        count += isValid(str(i))

print("Part 1:", count)
