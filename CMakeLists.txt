cmake_minimum_required(VERSION 3.10)
project(LibGumball)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(GUM_BUILD_DOCS             "Build docs"            OFF)
option(GUM_BUILD_EXAMPLES         "Build examples"        OFF)
option(GUM_BUILD_TESTS            "Build unit tests"      OFF) # Not yet implemented
option(GUM_BUILD_BACKEND_RAYLIB   "Enable raylib backend" ON )
option(GUM_BUILD_BACKEND_SDL3     "Enable SDL3 backend"   OFF) # Not yet implemented

add_subdirectory(lib)

if(GUM_BUILD_DOCS)
    find_package(Doxygen REQUIRED)
    add_custom_target(docs ALL
        COMMAND ${CMAKE_COMMAND} -E rm -rf html latex
        COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/docs
        VERBATIM
    )
endif()

if(GUM_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(GUM_BUILD_TESTS)
    add_subdirectory(tests)
endif()
