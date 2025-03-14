@echo off
if exist build rmdir /s /q build
if exist bin rmdir /s /q bin

@echo on
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
cd ..
pause

