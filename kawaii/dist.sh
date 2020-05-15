#!/bin/sh

mkdir dist

./configure --disable-sdl && make clean all && cp src/kawaii.exe dist/kawaii-compo.exe
./configure && make clean all && cp src/kawaii.exe dist/kawaii-sdl.exe
make dist && mv kawaii-2.0.tar.gz dist/
upx --best dist/*.exe

