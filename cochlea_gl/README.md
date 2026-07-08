# Cochlea GL

OpenGL demo application ported from DirectX to OpenGL/SDL2.

## Building

### Prerequisites

**Ubuntu/Debian:**
```bash
sudo apt-get install libsdl2-dev libmodplug-dev libglew-dev libglu1-mesa-dev
```

**Fedora:**
```bash
sudo dnf install SDL2-devel libmodplug-devel glew-devel glu-devel
```

**Arch Linux:**
```bash
sudo pacman -S sdl2 libmodplug glew glu
```

**macOS:**
```bash
brew install sdl2 libmodplug glew
```

### Build Commands

Using make:

```bash
make          # Build
make clean    # Clean
make rebuild  # Rebuild
make install  # Install
```

Or directly with meson+ninja:

```bash
meson setup build
ninja -C build
```

Or in a container (installs all dependencies and builds via CMake):

```bash
docker build -t cochlea_gl .
```

### Running

```bash
./build/cochlea_gl [options]
```

**Options:**
```
  -w, --width <pixels>   Window width (default: 640)
  -h, --height <pixels>  Window height (default: 480)
  -r, --rate <hz>        Audio sample rate (default: 44100)
                         Common values: 22050, 44100, 48000, 88200, 96000
  --help                 Show help message
```

**Examples:**
```bash
# Default 640x480 at 44100 Hz
./build/cochlea_gl

# HD resolution at 96kHz
./build/cochlea_gl -w 1280 -h 720 -r 96000

# Fullscreen-like experience
./build/cochlea_gl -w 1920 -h 1080
```

## Controls

- **ESC** - Quit
- **Arrow Keys** - Navigation (if supported by demo)

## Dependencies

- SDL2 - Cross-platform multimedia library
- OpenGL - Graphics rendering
- GLEW - OpenGL Extension Wrangler
- GLU - OpenGL Utility Library
- libmodplug - Module music player (XM files), configured for HQ playback (8-tap FIR resampling, oversampling, noise reduction) - used directly, no MIDAS wrapper

## Notes

- The original MIDAS audio wrapper has been removed
- Audio is now handled directly by libmodplug through xm_player

## License

MIT License

Copyright (c) 2025 the Cochlea GL port contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

This license covers the OpenGL port's code. The original 1999 Cochlea demo
(code, graphics, music) is credited to KNIGHTS — Gautier "Impulse" Portet,
Nicolas "Stuff" Challeil, and Neo Nakai — and was released as freeware.
