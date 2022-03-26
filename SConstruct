import fnmatch
import io
import os
import subprocess
from sys import stdout
from datetime import datetime

import sys
sys.path.append(os.path.expanduser("~/.local/lib/python3.10/site-packages"))
sys.path.append("/usr/lib/python3.10/site-packages")
from spectre7 import utils

# - Constants -
INCLUDE_LIBRARIES = [
    "raylib",
    "dw", 
    "yaml-cpp",
    "pthread",
    "box2d"
]

OUTPUT_DIR = "build/"
OBJ_DIR = "build/obj/"
LIB_DIR = "build/lib/"

ENGINE_SRC = "engine/src/"
PROJECT_SRC = "project/src/"

ENGINE_BINARY_NAME = "engine"
PROJECT_BINARY_NAME = "Project.bin"

# Colours
PROGRESS_PERCENT_COLOUR = {"colour": "magenta", "attrs": ["bold"]}
SKIPPED_FILEPATH_COLOUR = {"colour": "white", "attrs": ["dark"]}
COMPILING_FILEPATH_COLOUR = {"colour": "cyan", "attrs": []}
LINKING_FILEPATH_COLOUR = {"colour": "red", "attrs": []}
INDEXING_FILEPATH_COLOUR = {"colour": "red", "attrs": []}

def RecursiveGlob(path: str, extension: str):
    matches = []
    for root, dirnames, filenames in os.walk(path):
        for filename in fnmatch.filter(filenames, "*." + extension):
            matches.append(File(os.path.join(root, filename)))

    return matches

def GetGitHash():
    return subprocess.check_output(["git", "describe", "--always"]).strip().decode()

def RunProjectBinary():
    os.system(OUTPUT_DIR + PROJECT_BINARY_NAME)

AddOption("--print-binary-path", dest="print_binary_path", action="store_true", default=False)
AddOption("--run", dest="run_after_build", action="store_true", default=False)
AddOption("--clear-console", dest="clear_before_build", action="store_true", default=False)

if GetOption("print_binary_path"):
    print(OUTPUT_DIR + PROJECT_BINARY_NAME)
    exit()

if GetOption("clear_before_build"):
    os.system("clear")

engine_env = Environment(
    CPPPATH = ["include/raylib", "include/raylib-cpp", "."],
    CCFLAGS = ["-std=c++17", "-g", "-fpermissive", "-fdiagnostics-color=always", f"-DGIT_COMMIT_HASH='\"{GetGitHash()}\"'"],
    LIBPATH = [LIB_DIR, "include/lib/"],
    LIBS = INCLUDE_LIBRARIES,
    CXX = "g++",
)

engine_env["CXXCOMSTR"]    = "Compiling $SOURCE"
engine_env["LINKCOMSTR"]   = "Linking $TARGET"

project_env = engine_env.Clone(LIBS = INCLUDE_LIBRARIES + [ENGINE_BINARY_NAME])

engine_objs = []
for file in RecursiveGlob(ENGINE_SRC, "cpp"):
    object_path = str(file).replace(ENGINE_SRC, OBJ_DIR + "engine/").replace(".cpp", ".o")
    # object_path = OBJ_DIR + "engine/" + str(file).split("/")[-1].replace(".cpp", ".o")
    engine_env.Object(target = object_path, source=file)
    BUILD_TARGETS.append(object_path)
    engine_objs.append(object_path)

engine_binary = engine_env.Library(LIB_DIR + ENGINE_BINARY_NAME, engine_objs)
Depends(engine_binary, engine_objs)

project_objs = []
for file in RecursiveGlob(PROJECT_SRC, "cpp"):
    object_path = str(file).replace(PROJECT_SRC, OBJ_DIR + "project/").replace(".cpp", ".o")
    # object_path = OBJ_DIR + "project/" + str(file).split("/")[-1].replace(".cpp", ".o")
    project_env.Object(target = object_path, source=file)
    BUILD_TARGETS.append(object_path)
    project_objs.append(object_path)

project_binary = project_env.Program(
    target=OUTPUT_DIR + PROJECT_BINARY_NAME,
    source=project_objs
)

Depends(project_binary, engine_binary)
Depends(project_binary, project_objs)

# ---------------------------------------------------

out = stdout
node_count = -1
previous_line = ""
begin_time = datetime.now()

f = io.StringIO()
sys.stdout = f

def progress_function(node):
    global node_count, previous_line

    lines = str(f.getvalue()).split("\n")

    if lines[-2] != previous_line:
        previous_line = lines[-2]

        progress = int((node_count / (len(project_objs) + len(engine_objs) + 1)) * 100.0)
        out.write(f"{convert_scons_line(previous_line, progress)}\n")
        out.flush()

        node_count += 1

def convert_scons_line(line: str, progress: int):

    line = line.strip()

    if line.endswith(" is up to date."):
        path = line.split("`")[1].split("'")[0].replace(".o", ".cpp").replace(OBJ_DIR + "project/", PROJECT_SRC).replace(OBJ_DIR + "engine/", ENGINE_SRC)
        line = "Compiling " + utils.format_colour(SKIPPED_FILEPATH_COLOUR["colour"], path, SKIPPED_FILEPATH_COLOUR["attrs"]) + " (skipping)"
    elif line.startswith("Compiling "):
        line = "Compiling " + utils.format_colour(COMPILING_FILEPATH_COLOUR["colour"], line[len("Compiling "):], COMPILING_FILEPATH_COLOUR["attrs"])
    elif line.startswith("ranlib "):
        line = "Indexing library " + utils.format_colour(INDEXING_FILEPATH_COLOUR["colour"], line[len("ranlib "):], INDEXING_FILEPATH_COLOUR["attrs"])
    elif line.startswith("Linking "):
        line = "Linking program " + utils.format_colour(LINKING_FILEPATH_COLOUR["colour"], line[len("Linking "):], LINKING_FILEPATH_COLOUR["attrs"])
    elif line == "scons: Building targets ...":
        return utils.format_colour("magenta", f"\n - - Starting build - - \n", ["reverse"])

    progress = str(progress)
    for i in range(3 - len(progress)):
        progress = " " + progress
    progress = f"[{progress}%]"

    return f"{utils.format_colour(PROGRESS_PERCENT_COLOUR['colour'], progress, PROGRESS_PERCENT_COLOUR['attrs'])} {line}"

def progress_finish(target, source, env):

    duration = datetime.now() - begin_time

    out.write(utils.format_colour("magenta", f"\n - - Build completed in {duration.seconds} seconds - - \n\n", ["reverse"]))
    out.flush()

    if GetOption("run_after_build"):
        RunProjectBinary()

Progress(progress_function)

progress_finish_command = Command('progress_finish', [], progress_finish)

Depends(progress_finish_command, BUILD_TARGETS)
Depends(progress_finish_command, project_binary)
BUILD_TARGETS.append('progress_finish')