# About
The project is in an early stage, and for now, there is nothing interesting

# Build
```console
mkdir build && cd build
cmake ..
cmake --build . -j 4
```
To set your preferred compiler for building, create a ```.env.cmake``` file and add this:

```cmake
set(CMAKE_C_COMPILER /path/to/your/c/compiler)
set(CMAKE_CXX_COMPILER /path/to/your/cxx/compiler)
```
To apply a new C compiler, you need to remove the old build and CMake cache, and then run the above commands again
