#!/usr/bin/env python3

from pathlib import Path
import sys
from PIL import Image, ImageChops

if len(sys.argv) not in (3, 4):
    raise SystemExit("usage: compare_images.py <sdl3.png> <raylib.png> [diff.png]")

first_path = Path(sys.argv[1])
second_path = Path(sys.argv[2])
diff_path = Path(sys.argv[3]) if len(sys.argv) == 4 else first_path.with_name("backend-parity-diff.png")

first = Image.open(first_path).convert("RGBA")
second = Image.open(second_path).convert("RGBA")

if first.size != second.size:
    print(f"FAIL: image sizes differ ({first.size} != {second.size})")
    raise SystemExit(1)

first_pixels = list(first.getdata())
second_pixels = list(second.getdata())
differing = sum(a != b for a, b in zip(first_pixels, second_pixels))

if differing:
    diff = ImageChops.difference(first, second).convert("RGB")
    diff.save(diff_path)
    print(f"FAIL: {differing} differing pixels")
    print(f"Diff written to {diff_path}")
    raise SystemExit(1)

if diff_path.exists():
    diff_path.unlink()

print("PASS: 0 differing pixels")
