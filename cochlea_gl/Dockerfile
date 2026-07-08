FROM ubuntu:24.04

# Set non-interactive mode for package installations
ENV DEBIAN_FRONTEND=noninteractive

# Install prerequisites
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    libsdl2-dev \
    libmodplug-dev \
    libglew-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    libxext-dev \
    libx11-dev \
    xvfb \
    mesa-utils \
    libgl1-mesa-glx \
    libgl1-mesa-dri \
    x11-apps \
    wget \
    curl

# Create app directory
WORKDIR /app

# Copy the source files
COPY . /app/

# Build the OpenGL version
RUN mkdir -p build && cd build && \
    cmake .. && \
    make -j$(nproc)

# Create a startup script
RUN echo '#!/bin/bash

# Check if running under Wayland or X11

if [ -z "$DISPLAY" ] && [ -z "$WAYLAND_DISPLAY" ]; then

  # Start virtual framebuffer if no display is available

  if [ -x "$(command -v Xvfb)" ]; then

    Xvfb :0 -screen 0 640x480x24 &

    export DISPLAY=:0

    sleep 1

  fi

elif [ -n "$WAYLAND_DISPLAY" ]; then

  # For Wayland, we might need to use XWayland

  export GDK_BACKEND=x11

  export QT_QPA_PLATFORM=xcb

fi



# Run the OpenGL application

cd /app/build && ./cochlea_gl

' > /app/start.sh && chmod +x /app/start.sh

# Expose display
EXPOSE 6000

CMD ["/app/start.sh"]
