# OIT OpenGL
This project demonstrates an approch for order independent transparency. For implementation details, see main.cpp

## Building
The build system that was chosen is the SCons build system. To build, open a terminal in the project directory and run 'scons'. To speed up build times, you can use the -j flag to specify the number of theads to use, just like make

## Installing
Due to restrictions with file size and github, the model files and textures (except for dragon.mtl) are not included. All models can be found at https://casual-effects.com/data. Download and extract the 'Crytek Sponza' model into res/sponza. Download and extract the 'Chinese Dragon' model into res/dragon. Replace
```
g root
```
with
```
mtllib dragon.mtl
g root 
usemtl Dragon
```
in dragon.obj

### Linux
Make sure that 64 bit GLFW3 and it's dev files are installed

### Windows
Download 64 bit GLFW3 from https://www.glfw.org/download. If using MSBuild tools use the files from any of the lib-vc*. If using GCC, use the files from lib-mingw-w64