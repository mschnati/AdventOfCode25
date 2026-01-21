# Advent of Code 2025 day 2
# https://adventofcode.com/2025/day/2

with open("inputs/input_02.txt") as f:
    ids = f.read().split(',')

def isValid(id: str) -> int:
    if len(id) % 2:
        return 0
    
    half = len(id) // 2
    return int(id) if id[:half] == id[half:] else 0

# Now, an ID is invalid if it is made only of some sequence of digits repeated at least twice.
# 123123, 123123123, 111111111111111, 121212121212 etc.
def part2(id: str) -> int:
    n = len(id)
    for k in range(1, n // 2 + 1):
        if n % k == 0:
            if id == id[:k] * (n // k):
                # print(id)      
                return int(id)
    return 0

count = 0
count2 = 0
for id in ids:
    start, end = id.split('-')
    for i in range(int(start), int(end) + 1):
        count += isValid(str(i))
        count2 += part2(str(i))

print("Part 1:", count)
print("Part 2:", count2)
