cmake_minimum_required(VERSION 3.15)
project(OpenGLLearning VERSION 1.0.0 LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Find required packages
find_package(OpenGL REQUIRED)

# GLFW - Using local copy in glfw subdirectory
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)

message(STATUS "Using GLFW from: ${CMAKE_CURRENT_SOURCE_DIR}/glfw")

# GLAD - Assuming GLAD sources are in your project or system
# If you have GLAD as a separate library, adjust accordingly
# For now, assuming glad.c is compiled as part of the project

# Collect all source files
set(SOURCE_FILES
    glad/src/glad.c

    main.cpp
    src/color.cpp
    src/model_view_projection.cpp
    src/stb_image.cpp
    src/transformation.cpp
    src/drawable/drawable.cpp
    src/drawable/obj_loader.cpp
    src/drawable/primitive.cpp
    src/drawable/shape/2d/complex.cpp
    src/drawable/shape/2d/primitive.cpp
    src/logging/logging.cpp
    src/mygl/texture.cpp
    src/mygl/debug/debug.cpp
    src/mygl/shader/shader.cpp
)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/headers
    ${CMAKE_CURRENT_SOURCE_DIR}/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/glew/include
    ${CMAKE_CURRENT_SOURCE_DIR}/glm/include
    ${OPENGL_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    glfw
)

# Platform-specific configuration
if(WIN32)
    # Windows-specific libraries
    target_link_libraries(${PROJECT_NAME} PRIVATE gdi32)
    
    if(MSVC)
        # MSVC-specific settings
        message(STATUS "Configuring for MSVC")
        # Set Windows subsystem to avoid console window (optional)
        # set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
    elseif(MINGW)
        # MinGW-specific settings: static linking to avoid DLL dependencies
        message(STATUS "Configuring for MinGW")
        target_link_options(${PROJECT_NAME} PRIVATE
            -static-libgcc
            -static-libstdc++
            -Wl,--allow-multiple-definition
        )
    endif()
elseif(UNIX AND NOT APPLE)
    # Linux-specific settings
    message(STATUS "Configuring for Linux")
endif()

# Copy resources to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/res
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res
    COMMENT "Copying resources to build directory"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/src/mygl/shader
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders
    COMMENT "Copying shaders to build directory"
)

# Print build information
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
