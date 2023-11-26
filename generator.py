#!/usr/bin/env python3
import sys
import random
import argparse

parser = argparse.ArgumentParser(description='random weighted intervals generator')
parser.add_argument('n', type= int)
parser.add_argument('-length', type=int)
parser.add_argument('-breadth', type=int)
args = parser.parse_args()

n = args.n
print(n)

if args.length is not None:
    for i in range(n):
        l = random.randint(-args.breadth, args.breadth - args.length)
        r = l + random.randint(1, args.length)
        v = random.randint(0, min(10 + n, 10**5))
        print(l, r, v)
else: 
    for i in range(n):
        l = random.randint(-args.breadth, args.breadth - 1)
        r = random.randint(l + 1, args.breadth)
        v = random.randint(0, min(10 + n, 10**5))
        print(l, r, v)

#first argument -> n


