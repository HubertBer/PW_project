#!/bin/python3
import sys
import re

def parse_value(value_str):
    # Extract the float value from the string
    match = re.match(r'(\d+\.\d+)ms', value_str)
    if match:
        return float(match.group(1))
    match = re.match(r'(\d+)ms', value_str)
    return float(match.group(1))

# Read the data from stdin
lines = sys.stdin.readlines()

# Iterate over pairs (assuming even number of lines)
data = [(lines[i].strip(), parse_value(lines[i + 1].strip())) for i in range(0, len(lines), 2)]

# Sort the data based on the float values (second element of each pair)
sorted_data = sorted(data, key=lambda x: x[1])

# Print the top 5 pairs
print("Top 5:")
for pair in sorted_data[:5]:
    print(f"{pair[0]}: {pair[1]} ms")
