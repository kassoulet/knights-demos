cmake_minimum_required(VERSION 3.10)
project(cochlea_gl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Find SDL2
find_package(SDL2 REQUIRED)

# Find libmodplug using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(MODPLUG REQUIRED libmodplug)

# Add executable
add_executable(cochlea_gl
    Cochlea.cpp
    OpenGLWrapper.cpp
    main.cpp
    xm_player.cpp
)

# Include directories
target_include_directories(cochlea_gl PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${MODPLUG_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(cochlea_gl
    OpenGL::GL
    ${MODPLUG_LIBRARIES}
    SDL2::SDL2
    -lGLEW -lGLU
)

# Platform-specific settings
if(WIN32)
    target_link_libraries(cochlea_gl
        opengl32
        gdi32
        user32
        winmm
    )
elseif(UNIX AND NOT APPLE)
    target_link_libraries(cochlea_gl
        X11
        Xrandr
        Xinerama
        Xi
        Xcursor
        m
        dl
        pthread
    )
elseif(APPLE)
    target_link_libraries(cochlea_gl
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
endif()

# Define preprocessor macros
target_compile_definitions(cochlea_gl PRIVATE
    # Add any necessary preprocessor definitions
)
