with open("../inputs/input_01.txt") as f:
    lines = f.readlines()

dial = 50
count = 0
count2 = 0

for line in lines:
    # print(line, end="")
    line.strip()
    if line.startswith("R"):
        num = int(line[1:])
        # Count how many multiples of 100 are in (dial, dial + num]
        crossings = (dial + num) // 100 - dial // 100
        count2 += crossings
        
        dial = (dial + num) % 100
    elif line.startswith("L"):
        num = int(line[1:])
        crossings = (dial - 1) // 100 - (dial - num - 1) // 100
        count2 += crossings
        
        dial = (dial - num) % 100
    
    
    # print(dial, end="\n\n")
    if dial == 0:
        count += 1

print(f"Part 1: {count}")
print(f"Part 2: {count2}")
