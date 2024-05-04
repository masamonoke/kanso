# About
The project is on quite early state and for now there is nothing interesting

# Build

```console
mkdir build && cd build
cmake ..
cmake --build . -j 4
```

<p>I had problems compiling assimp library with gcc 13.2.0 and so both builds on macOS and Linux tested on clang 18.1.4.</p>
<p>To set your favorite compiler for build you can create file .env.cmake and add this line to it:</p>

```cmake
set(CMAKE_C_COMPILER /path/to/your/c/compiler)
```
<p>To apply new C compiler you need to remove old build and cmake cache and do upper commands again</p>
