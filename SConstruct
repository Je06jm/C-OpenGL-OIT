import urllib.request
import os
import zipfile

AddOption(
    "--debug-opengl",
    action="store_true",
    dest="debug-opengl",
    default=False,
    help="Enables OpenGL debug logging"
)

AddOption(
    "--debug-build",
    action="store_true",
    dest="debug-build",
    default=False,
    help="Enables debug logging"
)

AddOption(
    "--glfw-linker-fix",
    action="store_true",
    dest="glfw-linker-fix",
    default=False,
    help="Links to glfw instead of glfw3"
)

env = Environment()

libs = {
    "Windows" : [
        "glfw3dll"
    ],
    "Other" : [
        "dl"
    ]
}

if GetOption("glfw-linker-fix"):
    libs["Other"] += ["glfw"]
else:
    libs["Other"] += ["glfw3"]

libs["Other"] += ["pthread"]

flags = {
    "Windows" : [
        "/EHsc",
        "/O2",
        "/I libs",
        "/I src"
    ],
    "Other" : [
        "-Isrc",
        "-Ilibs",
        "-O2"
    ]
}

linker = {
    "Windows" : [
    ],
    "Other" : [
        "-L./"
    ]
}

dirs = [
    "src",
    "src/helper",
    "src/platform",
    "src/rendering",
    "libs"
]

if not GetOption("debug-build"):
    env.Append(CPPDEFINES=["IS_RELEASE"])

if GetOption("debug-opengl"):
    env.Append(CPPDEFINES=["DEBUG_OPENGL"])

def download(url, path):
    print("Downloading " + url)
    urllib.request.urlretrieve(url, "temp.zip")

    print("Extracting zip into " + path)
    with zipfile.ZipFile("temp.zip", "r") as zipFile:
        zipFile.extractall(path)

    os.remove("temp.zip")

if "get-dependecies" in COMMAND_LINE_TARGETS:
    download("http://casual-effects.com/g3d/data10/research/model/dragon/dragon.zip", "res/dragon")
    download("http://casual-effects.com/g3d/data10/common/model/crytek_sponza/sponza.zip", "res/sponza")

    print("Processing res/dragon/dragon.obj")

    fin = open("res/dragon/dragon.obj", "r")
    lines = fin.readlines()
    fin.close()

    fout = open("res/dragon/dragon.obj", "w")

    for line in lines:
        if line.strip() == "g root":
            fout.write("mtllib dragon.mtl\ng root\nusemtl Dragon\n")
        else:
            fout.write(line)
    
    fout.close()

    print("Done")
    quit()

platform = "Other"
truePlatform = "Other"

files = []
for d in dirs:
    files += Glob(d + "/*.cpp")
    files += Glob(d + "/*.c")

if env["CC"] == "cl":
    platform = "Windows"

if env["HOST_OS"] == "win32":
    truePlatform = "Windows"

for flag in flags[platform]:
    env.Append(CCFLAGS = " " + flag)
    
for flag in linker[platform]:
    env.Append(LINKFLAGS = " " + flag)

env.Append(LIBS = libs[truePlatform])

env.Program("OIT", files)
