cmake_minimum_required(VERSION 3.10)
project(LibGumball)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")

option(GUM_BUILD_DOCS             "Build docs"            OFF)
option(GUM_BUILD_EXAMPLES         "Build examples"        OFF)
option(GUM_BUILD_TESTS            "Build unit tests"      OFF)
option(GUM_BUILD_BACKEND_SDL3     "Enable SDL3 backend"   OFF)
option(GUM_BUILD_BACKEND_RAYLIB   "Enable raylib backend"  ON)

if(GUM_BUILD_DOCS)
    find_package(Doxygen REQUIRED)
    add_custom_target(docs ALL
        COMMAND ${CMAKE_COMMAND} -E rm -rf html latex
        COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
        # COMMAND xdg-open html/index.html || true
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

add_subdirectory(lib)

# add_executable(testlibGumball main.c)
# target_link_libraries(testlibGumball libGumball)
