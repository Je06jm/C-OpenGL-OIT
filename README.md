# OIT OpenGL
This project demonstrates an approch for order independent transparency. For implementation details, see main.cpp

## Build System
The build system that was chosen is the SCons build system (https://scons.org/).

### Installing SCons
Scons required python 3 to be installed, along with pip. Once installed run
```
python3 -m pip install scons
```
or if you are on Windows
```
python -m pip install scons
```

## Installing
Due to restrictions with file size and github, the model files and textures (except for dragon.mtl) are not included. However, the build system will handle the downloading and extracting of the files. In the project directory run
```
scons get-dependecies
```
### Linux
Make sure that 64 bit GLFW3 and it's dev files are installed

#### Ubuntu
```
sudo apt install libglfw3
sudo apt install libglfw3-dev
```

### Windows
Download 64 bit GLFW3 from https://www.glfw.org/download. If using MSBuild tools use the files from any of the lib-vc*. If using GCC, use the files from lib-mingw-w64

## Building
The build system that was chosen is the SCons build system. To build, open a terminal in the project directory and run
```
scons
```
To speed up build times, you can use the -j flag to specify the number of theads to use, just like make. For example:
```
scons -j4
```
