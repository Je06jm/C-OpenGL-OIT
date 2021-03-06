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
        "/DIS_RELEASE"
    ],
    "Other" : [
        "-Isrc",
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

env = Environment()
platform = "Other"
truePlatform = "Other"

if env["CC"] == "cl":
    platform = "Windows"
    truePlatform = "Windows"

if env["CC"] != "cl":
    platform = "Other"

for flag in flags[platform]:
    env.Append(CCFLAGS = " " + flag)
    
for flag in linker[platform]:
    env.Append(LINKFLAGS = " " + flag)

env.Append(LIBS = libs[truePlatform])

env.Program("OIT", Glob("src/*.cpp") + Glob("src/*.c"))
