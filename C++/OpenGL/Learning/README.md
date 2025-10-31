# OpenGL Learning Project

A C++ OpenGL project for learning 3D graphics programming with GLFW and GLAD.

<video controls src="res/demo.mp4" title="Title"></video>

## Features

- 3D shapes and primitives
- Model loading from OBJ files
- Texture support
- Lighting system (ambient, diffuse, specular)
- Transformations and MVP matrix
- Custom shader support

## Prerequisites

Before building this project, you need:

1. **CMake** (version 3.15 or higher)
    - Download from [cmake.org](https://cmake.org/download/)
    - Or install via: `winget install Kitware.CMake`

2. **C++ Compiler** (one of):
    - Visual Studio 2019/2022 with C++ desktop development workload
    - MinGW-w64
    - Clang

3. **GLFW** (OpenGL window and input library)
    - Download from [glfw.org](https://www.glfw.org/download.html)
    - Unzip into glfw directory

4. **GLAD** (OpenGL loader)
    - If not included, generate from [glad.dav1d.de](https://glad.dav1d.de/)
        - gl: 4.6
        - profile: core
    - Unzip into glad directory

5. **GLEW** (OpenGL Extension Wrangler Library)
    - Download from [glew.sourceforge.net](http://glew.sourceforge.net/)
    - Unzip into glew directory

6. **GLM** (OpenGL Mathematics Library)
    - Download from [glm.g-truc.net](https://github.com/g-truc/glm/releases/tag/1.0.2)
    - Extract glm folder

## Building the Project

### Using CMake (Recommended)

```powershell
# Configure (add -DGLFW_DIR if needed)
cmake -B build -S .

# Build
cmake --build build --config Release

# Run
.\build\bin\Release\OpenGLLearning.exe
```

## Project Structure

```
.
├── main.cpp                    # Main application entry point
├── CMakeLists.txt             # CMake build configuration
├── gladEnumParser.py          # GLAD enum parsing utility
├── res/                       # Resources directory
│   ├── obj_files/            # 3D model files
│   └── textures/             # Texture files
└── src/                       # Source code
    ├── headers/              # Header files
    │   ├── drawable/        # Drawable objects
    │   ├── logging/         # Logging utilities
    │   └── mygl/            # OpenGL wrapper classes
    └── *.cpp                # Implementation files
```

## Controls

Example:
- **W/A/S/D** - Move camera
- **Mouse** - Look around
- **ESC** - Exit application

## Dependencies

- **GLFW** - Window and input handling
- **GLAD** - OpenGL function loader
- **GLM** (if used) - Mathematics library for graphics
- **STB Image** - Image loading (included via stb_image.cpp)
