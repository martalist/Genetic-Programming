# Genetic Programming

This is my own personal experiementation with genetic programming in C++.

For now, there is not much to see. I will add to this as things progress. 


## Notes for extern_c_lib branch

The program has the following dependencies: 

- [Boost](https://www.boost.org/) 1.66
- [Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page) 3.3
- Google Test

GTest will be automatically downloaded by CMake, and isn't really needed for the library implementation. Boost and Eigen will need to be installed manually (e.g. `brew install boost` on macOS should do it).

### Compiling and installing

Run the following commands

```
git clone git@github.com:martalist/Genetic-Programming.git
cd Genetic-Programming
git co -b extern_c_lib origin/extern_c_lib
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make install
```

The C API is very simple, consisting of a single function and two structs. See `include/gpforecast.h`.
