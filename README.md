# Computer Graphics 203.3710, Summer 2018
<!-- assignment 1 submitted on 2/8/18 -->
<!-- Assignment 3 submitted on 9/10/18 -->
## Students Team data

Name: Itamar Shenhar  
Student Number: 318826104

Name: Tomer Keren  
Student Number: 318678471

### Lecturer
*Roi Poranne*

[roiporanne@cs.haifa.ac.il](mailto:roiporanne@cs.haifa.ac.il)

URL: [https://www.inf.ethz.ch/personal/poranner/](https://www.inf.ethz.ch/personal/poranner/)

### Assistant
*Nave Zelzer*

[nzelzer@campus.haifa.ac.il](mailto:nzelzer@campus.haifa.ac.il)

## Course Description

This course provides an introduction to the foundations of Computer Graphics. We will cover the basic mathematical concepts, such as 2D and 3D transformations, shading models, and rendering techniques. The ultimate goal of this course is to develop a basic model viewer.

Students will experiment with modern graphics programming and build a renderer using C++ and OpenGL.

By the end of the course, you will be able to:

* Explain and apply the fundamental mathematical concepts used in rendering.
* Implement a basic graphics pipeline based on rasterization.
* Develop simple graphics programs in C++ using OpenGL and GLSL.

*Textbook*:
Interactive Computer Graphics: A Top-Down Approach with Shader-Based OpenGL ,6th Edition

## Schedule and Course Notes:

[*Assignment 1: Wireframe Viewer*](Assignments/homework1.pdf)

[*Breakdown of the assignment to small tasks, you can use it as a starting point*](Assignments/homework1Breakdown.pdf)

## Assignments

* [Assignment 1: Wireframe Viewer](Assignments/homework1.pdf)

* [Assignment 2: Basic Shading](Assignments/homework2.pdf)

* [Assignment 3: OpenGL Renderer](Assignments/homework3.pdf)

## General Instructions

### Installing CMAKE
We use CMake for cross-platform builds. On Windows you can download it from: [https://cmake.org/download/](https://cmake.org/download/). If you are using Linux or macOS, we recommend installing it with a package manager instead of the CMake download page. E.g. on Debian/Ubuntu:
```
sudo apt-get install cmake
```
or with Homebrew on macOS:
```
brew install cmake.
```

### Building the code
```sh
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
cd bin
./MeshViewer <obj file>
```

Or use the CMAKE gui to create a visual studio solution, or use your compiler/IDE of choice.
Once the solution has been generated, open it with visual studio. Check that it compiles and runs.
