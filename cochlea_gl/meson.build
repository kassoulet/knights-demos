# Build with: make
# Or: meson setup build && ninja -C build
#
# Install dependencies (Ubuntu/Debian):
#   sudo apt-get install libsdl2-dev libmodplug-dev libglew-dev libglu1-mesa-dev
#

project('cochlea_gl', 'cpp',
  version: '1.0',
  default_options: ['cpp_std=c++17']
)

cc = meson.get_compiler('cpp')

# Find dependencies
opengl_dep = dependency('gl')
sdl2_dep = dependency('sdl2')
modplug_dep = dependency('libmodplug')

# GLEW and GLU - try pkg-config first, fall back to library search
glew_dep = dependency('glew', required: false)
if not glew_dep.found()
  glew_lib = cc.find_library('GLEW', required: false)
  if glew_lib.found()
    glew_dep = declare_dependency(link_with: glew_lib)
  else
    # GLEW might be header-only or built into OpenGL
    glew_dep = declare_dependency()
  endif
endif

glu_dep = dependency('glu', required: false)
if not glu_dep.found()
  glu_lib = cc.find_library('GLU', required: false)
  if glu_lib.found()
    glu_dep = declare_dependency(link_with: glu_lib)
  else
    # GLU might be part of OpenGL
    glu_dep = declare_dependency()
  endif
endif

# Platform-specific dependencies
if host_machine.system() == 'windows'
  platform_deps = [
    dependency('gdi32'),
    dependency('user32'),
    dependency('winmm'),
  ]
elif host_machine.system() == 'darwin'
  platform_deps = [
    dependency('appleframeworks', frameworks: ['Cocoa', 'IOKit', 'CoreVideo']),
  ]
else
  # Linux/Unix
  platform_deps = [
    dependency('x11'),
    dependency('xrandr'),
    dependency('xinerama'),
    dependency('xi'),
    dependency('xcursor'),
    dependency('threads'),
  ]
  math_lib = cc.find_library('m')
endif

# Source files
sources = files(
  'Cochlea.cpp',
  'OpenGLWrapper.cpp',
  'main.cpp',
  'xm_player.cpp',
)

# Include current source directory
inc = include_directories('.')

# Textures are loaded relative to the working directory (with a "gfx/"
# fallback) at runtime, so copy the assets next to the built executable -
# without this, running from build/ fails to find any texture and falls
# back to a placeholder checkerboard for everything.
run_command('cp', '-r', meson.current_source_dir() / 'gfx', meson.current_build_dir(), check: false)

# Build executable
executable('cochlea_gl',
  sources,
  include_directories: inc,
  dependencies: [
    opengl_dep,
    sdl2_dep,
    modplug_dep,
    glew_dep,
    glu_dep,
    platform_deps,
  ],
  install: true,
)
