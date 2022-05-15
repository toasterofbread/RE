#!/usr/bin/python3

import os
import sys

args = sys.argv[1:]

if len(args) != 2:
    print("Invalid argument amount (2 required)")

path = args[0]
if not os.path.isfile(path):
    print(f"'{path}' is not a file, or does not exist")
    exit()

if args[1].lower() == "on":
    on = True
elif args[1].lower() == "off":
    on = False
else:
    print("Argument 2 must be 'on' or 'off'")
    exit()

file = open(path, "r")

def isLineValid(line: str):
    
    line = line.strip().lower()
    if line == "":
        return False
    if line.startswith("#include"):
        return False
    if line.startswith("template"):
        return False
    if line.endswith("\\"):
        return False
    if line.endswith("}"):
        return False
    if line.endswith(","):
        return False
    if line.endswith("("):
        return False
    if line.startswith("//"):
        return False

    for sign in ("+", "-", "*", "/"):
        if line.endswith(sign):
            return False

    if line.startswith("return "):
        return False
    
    line = line.replace(" ", "")
    
    if line in ("return;", "continue;", "break;"):
        return False
    
    return True

output = []
i = 0
for line in file.readlines():
    output.append(line)

    if isLineValid(line):
        output.append(f"OS::print({i});\n")
        i += 1

file.close()

file = open(path, "w")
file.writelines(output)
file.close()