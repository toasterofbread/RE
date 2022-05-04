import fnmatch
import io
import os
from os.path import join
import subprocess
import sys
from datetime import datetime
from spectre7 import utils

AddOption("--target", dest="target", type=str, default="debug")
AddOption("--print-binary-path", dest="print_binary_path", action="store_true", default=False)
AddOption("--run", dest="run_after_build", action="store_true", default=False)
AddOption("--clear-console", dest="clear_before_build", action="store_true", default=False)
AddOption("--test", dest="run_tests", action="store_true", default=False)
AddOption("--verbose", dest="verbose", action="store_true", default=False)
AddOption("--no-engine", dest="build_engine", action="store_false", default=True)
AddOption("--no-project", dest="build_project", action="store_false", default=True)

if GetOption("clear_before_build"):
    os.system("clear")

env = Environment()
Export("env")
env.DIRECTORY = os.getcwd() + "/"

env.PROJECT_NAME = "RE"

env.BUILD_DIR = "build/"
env.OBJ_SUBDIR = "obj/"
env.LIB_SUBDIR = "lib/"

env.ENGINE_SRC = "engine/src/"
env.ENGINE_TESTS = "engine/tests/"
env.ENGINE_EXTERNAL = "engine/external/"
env.PROJECT_SRC = "project/src/"

env.ENGINE_BINARY_NAME = "engine"
env.TEST_BINARY_NAME = "Test"

COMPILER_SETTINGS = "engine/compiler_settings.h"

env.COLOURS = {
    "progress": {"colour": "magenta", "attrs": ["bold"]},
    "skip": {"colour": "white", "attrs": ["dark"]},
    "compile": {"colour": "cyan", "attrs": []},
    "link": {"colour": "red", "attrs": []},
    "index": {"colour": "red", "attrs": []}
}

def recursiveGlob(self, path: str, extension: str, name: str = None, exclude_dirs: list = []):
    return utils.recursiveGlob(join(env.DIRECTORY, path), extension, name, exclude_dirs)

AddMethod(Environment, recursiveGlob)

def getGitHash(self):
    return subprocess.check_output(["git", "describe", "--always"]).strip().decode()
AddMethod(Environment, getGitHash)

f = io.StringIO()
def msg(self, message):

    if GetOption("verbose"):
        print(message)
    else:
        f.write(str(message) + "\n")
        f.flush()
        env.progress_function(True)

AddMethod(Environment, msg)

def progress_function(node = None, force: bool = False):
    if GetOption("verbose"):
        return

    global node_count, previous_line

    lines = str(f.getvalue()).split("\n")

    if lines[-2] != previous_line or force:
        previous_line = lines[-2]

        progress = int((node_count / (node_total)) * 100.0)
        out.write(f"{env.convertSconsLine(previous_line, progress, out)}\n")
        out.flush()

        node_count += 1
    
AddMethod(Environment, progress_function)

ENV = env

def onBuildFinished(target, source, env):

    duration = datetime.now() - begin_time

    out.write(utils.format_colour("magenta", f"\n - - Build completed in {duration.seconds} seconds - - \n\n", ["reverse"]))
    out.flush()

    ENV.runProjectBinary(source[0])

env.RUN_AFTER_BUILD = GetOption("run_after_build")
SConscript("GenerateCompilerOptions.py")

if not GetOption("build_engine") and not GetOption("build_project"):
    print("Netither engine nor project are to be built, exiting...")
    exit()

if GetOption("print_binary_path"):
    print(BUILD_DIR + PROJECT_BINARY_NAME)
    exit()

engine_objs = []

if GetOption("build_engine"):
    for file in env.recursiveGlob(env.ENGINE_SRC, "cpp", None, ["engine/src/platform/"]) + env.recursiveGlob(env.ENGINE_SRC + "platform/" + env.PLATFORM_NAME, "cpp"):
        object_path = str(file).replace(env.ENGINE_SRC, env.OBJ_DIR + "engine/").replace(".cpp", env["OBJSUFFIX"])
        env.Object(target = object_path, source=file)
        BUILD_TARGETS.append(object_path)
        engine_objs.append(object_path)

    for file in env.recursiveGlob(env.ENGINE_EXTERNAL, "cpp", None):
        object_path = str(file).replace(env.ENGINE_EXTERNAL, env.OBJ_DIR + "engine/external/").replace(".cpp", env["OBJSUFFIX"])
        env.Object(target = object_path, source=file, CCFLAGS="-g")
        BUILD_TARGETS.append(object_path)
        engine_objs.append(object_path)

    env.engine_binary = env.Library(join(env.LIB_DIR, env.ENGINE_BINARY_NAME + env["LIBSUFFIX"]), engine_objs)
    Depends(env.engine_binary, engine_objs)

project_objs = []
project_env = env.Clone()

if GetOption("build_project"):
    for file in env.recursiveGlob(env.PROJECT_SRC, "cpp"):
        object_path = str(file).replace(env.PROJECT_SRC, env.OBJ_DIR + "project/").replace(".cpp", env["OBJSUFFIX"])
        project_env.Object(target = object_path, source=file)
        BUILD_TARGETS.append(object_path)
        project_objs.append(object_path)

    env.project_binary = project_env.Program(
        target=join(env.BUILD_DIR, env.PROJECT_NAME + ".elf"),# + env["PROGSUFFIX"],
        source=project_objs
    )

    if GetOption("build_engine"):
        Depends(env.project_binary, env.engine_binary)

    env.FINISH_COMMAND = Command("build_finished", env.project_binary[0], onBuildFinished)

    Depends(env.project_binary, project_objs)
    Depends(env.FINISH_COMMAND, env.project_binary)

else:
    env.FINISH_COMMAND = Command("build_finished", [None], onBuildFinished)
    Depends(env.FINISH_COMMAND, env.engine_binary)

node_total = len(engine_objs) + len(project_objs) + 2

if GetOption("run_tests"):
    test_objs = []
    for file in env.recursiveGlob(env.ENGINE_TESTS, "cpp"):
        object_path = str(file).replace(env.ENGINE_TESTS, env.OBJ_DIR + "test/").replace(".cpp", env["OBJSUFFIX"])
        project_env.Object(target = object_path, source=file)
        BUILD_TARGETS.append(object_path)
        test_objs.append(object_path)

    test_binary = project_env.Program(
        target=env.BUILD_DIR + env.TEST_BINARY_NAME + env["PROGSUFFIX"],
        source=test_objs
    )

    Depends(test_binary, env.engine_binary)
    Depends(test_binary, test_objs)

    node_total += len(test_objs)

SConscript("engine/src/platform/SConscript")

for key in env.Dictionary():
    project_env[key] = env[key]
project_env.Prepend(LIBS = env.ENGINE_BINARY_NAME)

# ---------------------------------------------------

begin_time = datetime.now()
out = sys.stdout

if not GetOption("verbose"):
    node_count = -1
    previous_line = ""
    sys.stdout = f

Progress(progress_function)

Depends(env.FINISH_COMMAND, BUILD_TARGETS)

if GetOption("run_tests"):
    Depends(env.FINISH_COMMAND, test_binary)

BUILD_TARGETS.append('build_finished')