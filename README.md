<div align="center">
  <h3 align="center">libGumball</h3>

  <p align="center">
    C23-Based, libGimbal-powered UI Library
    <br />
    <a href="http://libgumball.psyops.studio"><strong>Explore the docs »</strong></a>
    <br />
  </p>

</div>

# Overview #
libGumball is a modern UI library written in C23, built on top of libGimbal to provide an ergonomic way to create user interfaces in pure C.
Designed primarily for video game development, it leverages libGimbal's powerful property system, dynamic type system, and other capabilities to make UI building intuitive, efficient, and performant.

# Building #
First, ensure submodules are installed with:
```
git submodule update --init --recursive
```

To build the project from the command-line, you can do the following:
```
mkdir build
cd build
cmake ..
cmake --build .
```

You can also pass `-DGUM_ENABLE_EXAMPLES=ON` and/or `-DGUM_ENABLE_TESTS=ON` if you wish to build the examples / unit tests.

# Using #
This library is meant to be included as a submodule of your own project.
From your project root directory, run:
```
git submodule add git@github.com:haikuno/libgumball.git libgumball
git submodule update --init --recursive
```

Then, in your CMakeLists.txt file, add the following:
```
add_subdirectory(libgumball)
taget_link_libraries(yourproject libGumball)
```

# Credits #
## Author ##
Agustín Bellagamba ([Haikuno](https://github.com/Haikuno))
## Dependencies ##
- [libGimbal](https://github.com/gyrovorbis/libgimbal)

## Optional Dependencies ##
- Backend: [raylib](https://github.com/raysan5/raylib)
- Backend: [SDL](https://github.com/libsdl-org/SDL) and [SDL_ttf](https://github.com/libsdl-org/SDL_ttf)
