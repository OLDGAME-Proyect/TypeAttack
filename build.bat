cd build
del /q build\*
rmdir /s /q build
mkdir build
cmake -A Win32 ..
cmake --build .
cd ..