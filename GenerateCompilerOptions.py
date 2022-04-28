import os
from os.path import join

from regex import P

Import("env")

AddOption("--platform", dest="platform", type=str)

def getPlatformList() -> dict:
    ret = {}
    for file in env.recursiveGlob("engine/src/platform/", None, "SConscript"):
        file = str(file)
        if file == "engine/src/platform/SConscript":
            continue
        ret[os.path.dirname(file).split("/")[-1]] = os.path.join(env.DIRECTORY, file)
    return ret

env.PLATFORM_LIST = getPlatformList()
if not GetOption("platform") in env.PLATFORM_LIST:

    if GetOption("platform"):
        print(f"'{GetOption('platform')}' is not a valid platform. Available platforms:")
    else:
        print("A valid platform must be specified. Available platforms:")

    for platform in env.PLATFORM_LIST:
        print(" ", platform)
    exit()
env.PLATFORM_NAME = GetOption("platform")

env.BUILD_DIR = join(env.BUILD_DIR, env.PLATFORM_NAME)
env.OBJ_DIR = join(env.BUILD_DIR, env.OBJ_SUBDIR)
env.LIB_DIR = join(env.BUILD_DIR, env.LIB_SUBDIR)

file = open("engine/compiler_settings.h", "r")

generate_settings = True

for line in file.readlines():
    line = line.strip()
    if line.strip().startswith("#define DISABLE_GENERATION true"):
        generate_settings = False
        break

if generate_settings:
    
    data = \
"""#ifndef INCLUDED_COMPILER_OPTIONS
#define INCLUDED_COMPILER_OPTIONS

/*
    Automatically generated before compilation
    Define 'DISABLE_GENERATION' as true to disable generation
*/

#define DISABLE_GENERATION false

"""

    file.close()
    file = open("engine/compiler_settings.h", "w")

    data += "/* Available platforms */\n\n"

    i = 2
    for platform in env.PLATFORM_LIST:
        data += f"#define PLATFORM_{platform.upper()} {i}\n"
        i += 1
    
    data += "\n/* Platform in use */\n\n"
    data += f"#define PLATFORM PLATFORM_{env.PLATFORM_NAME.upper()}"

    data += f"\n\n/* Enable debug mode */\n\n#define DEBUG_ENABLED {str(GetOption('target') == 'debug').lower()}"

    data += "\n\n#define PHYSICS_2D_ENABLED false\n#define PHYSICS_3D_ENABLED false"

    data += "\n\n#endif\n"

    file.write(data)
    file.close()