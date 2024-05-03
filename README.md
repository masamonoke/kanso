# Build

I had problems compiling assimp library with gcc 13.2.0 and so both builds on macOS and Linux tested on clang 18.1.4

```console
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j 4
```
 
