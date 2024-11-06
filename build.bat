@echo off
mkdir build
cd build
cmake ..
cmake --build . --config Release
cd ..
echo Build completed! Executable is in the bin folder.
pause