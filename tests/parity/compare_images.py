#!/usr/bin/env python3

from pathlib import Path
import sys
from PIL import Image, ImageChops

if len(sys.argv) != 4:
    raise SystemExit("usage: compare_images.py <first> <second> <diff>")

first = Image.open(sys.argv[1]).convert("RGBA")
second = Image.open(sys.argv[2]).convert("RGBA")

if first.size != second.size:
    print(f"size mismatch: {first.size} != {second.size}")
    raise SystemExit(1)

first_pixels = list(first.getdata())
second_pixels = list(second.getdata())
differing = sum(a != b for a, b in zip(first_pixels, second_pixels))

if differing:
    diff = ImageChops.difference(first, second).convert("RGB")
    diff.save(Path(sys.argv[3]))
    print(f"backend parity failed: {differing} pixels differ")
    raise SystemExit(1)

print("backend parity: pixel-perfect")
