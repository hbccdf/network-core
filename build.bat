@echo off
mkdir build
cd build
cmake -G "Visual Studio 14 Win64" ..
rem vcmake -G "Visual Studio 14 Win64" ..
cd ..
pause