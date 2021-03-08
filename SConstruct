import urllib.request
import os
import zipfile

libs = {
    "Windows" : [
        "glfw3dll"
    ],
    "Other" : [
        "dl",
        "glfw3",
        "pthread"
    ]
}

flags = {
    "Windows" : [
        "/EHsc",
        "/O2",
        "/DIS_RELEASE",
        "/I libs",
        "/I src"
    ],
    "Other" : [
        "-Isrc",
        "-Ilibs",
        "-O2",
        "-DIS_RELEASE"
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

def download(url, path):
    print("Downloading " + url)
    urllib.request.urlretrieve(url, "temp.zip")

    print("Extracting zip into " + path)
    with zipfile.ZipFile("temp.zip", "r") as zipFile:
        zipFile.extractall(path)

    os.remove("temp.zip")

if "get-dependecies" in COMMAND_LINE_TARGETS:
    download("https://casual-effects.com/g3d/data10/research/model/dragon/dragon.zip", "res/dragon")
    download("https://casual-effects.com/g3d/data10/common/model/crytek_sponza/sponza.zip", "res/sponza")

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

env = Environment()
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
