#!/bin/python3
import re

def parse_value(value_str):
    # Extract the float value from the string
    match = re.match(r'(\d+\.\d+)ms', value_str)
    if match:
        return float(match.group(1))
    match = re.match(r'(\d+)ms', value_str)
    return float(match.group(1))

def calculate_mean(values):
    if not values:
        return None
    return sum(values) / len(values)

def main():
	# Read 10 values from the user
        input_values = [input() for i in range(10)]

        # Parse the values and calculate the mean
        parsed_values = [parse_value(value) for value in input_values]
        mean = calculate_mean(parsed_values)
        mean = "{:.5}".format(mean)
        # Print the result
        print(f"{mean}ms")
if __name__ == "__main__":
    main()
