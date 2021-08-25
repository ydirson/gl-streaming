#!/usr/bin/env python3

from collections import OrderedDict
import html
import re
import sys

DATA = OrderedDict()

if len(sys.argv) < 2:
    exit(1)

for log in sys.argv[1:]:
    LOGDATA = OrderedDict()
    DATA[log] = LOGDATA
    with open(log, "r") as f:
        for line in f:
            m = re.match(r"(\[.*): Unsupported", line)
            if m:
                LOGDATA[m.group(1)] = "-"
                continue
            m = re.match(r"(\[.*): FPS: ([0-9]*)", line)
            if not m: m = re.search(r"(glmark2 Score): ([0-9]*)", line)
            if not m: continue
            LOGDATA[m.group(1)] = m.group(2)

MASTERKEYS = DATA[sys.argv[1]].keys()

print("| test | " + " | ".join(DATA.keys()) + " |")
print("| :--- " + "| :-: " * len(DATA) + "|")

for key in MASTERKEYS:
    print("|", html.escape(key), "| ", end="")
    for log in sys.argv[1:]:
        print(DATA[log][key] if key in DATA[log] else "N/A", "| ", end="")
    print()
