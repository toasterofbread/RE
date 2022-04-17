import os

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

    for i, platform in enumerate(env.PLATFORM_LIST):
        if platform == env.PLATFORM_NAME:
            using_platform = i
        
        data += f"#define PLATFORM_{platform.upper()} {i}\n"
    
    data += "\n/* Platform in use */\n\n"
    data += f"#define PLATFORM PLATFORM_{env.PLATFORM_NAME.upper()}"

    data += "\n\n#endif\n"

    file.write(data)
    file.close()