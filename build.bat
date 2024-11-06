@echo off
mkdir build
cd build
cmake ..
cmake --build . --config Release
cd ..
echo Build completed! Executable is in the build folder.
pause
