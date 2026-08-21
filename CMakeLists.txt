cmake_minimum_required(VERSION 3.10)
project(LibGumball LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(GUM_BUILD_DOCS     "Build docs"       OFF)
option(GUM_BUILD_EXAMPLES "Build examples"   OFF)
option(GUM_BUILD_TESTS    "Build unit tests" OFF)

set(GUM_BACKEND "SDL3" CACHE STRING "libGumball backend")
set_property(CACHE GUM_BACKEND PROPERTY STRINGS SDL3 RAYLIB)
string(TOUPPER "${GUM_BACKEND}" GUM_BACKEND)

if(GUM_BACKEND STREQUAL "SDL3")
    set(GUM_BACKEND_DIR "sdl3")
elseif(GUM_BACKEND STREQUAL "RAYLIB")
    set(GUM_BACKEND_DIR "raylib")
else()
    message(FATAL_ERROR "Unsupported libGumball backend: ${GUM_BACKEND}")
endif()

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
    enable_testing()
    add_subdirectory(tests)
endif()
