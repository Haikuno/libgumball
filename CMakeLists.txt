cmake_minimum_required(VERSION 3.10)
project(LibGumball LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(GUM_BUILD_DOCS        "Build docs"       OFF)
option(GUM_BUILD_EXAMPLES    "Build examples"   OFF)
option(GUM_BUILD_TESTS       "Build unit tests" OFF)
option(GUM_ENABLE_SANITIZERS "Instrument libGumball targets with ASan/UBSan" OFF)

set(GUM_PARITY_SDL3_EXECUTABLE "" CACHE FILEPATH
    "Built SDL3 parity executable used by the optional cross-backend CTest")
set(GUM_PARITY_RAYLIB_EXECUTABLE "" CACHE FILEPATH
    "Built raylib parity executable used by the optional cross-backend CTest")

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

set(GUM_DEFAULT_FONT_BUILD_PATH "${CMAKE_CURRENT_BINARY_DIR}/lib/resources/fonts/default.ttf")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib/resources/fonts")
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/resources/fonts/default.ttf"
    "${GUM_DEFAULT_FONT_BUILD_PATH}"
    COPYONLY
)

set(GUM_DEFAULT_FONT_RUNTIME_PATH "" CACHE FILEPATH
    "Target-visible absolute path to libGumball's bundled default.ttf")
if(GUM_DEFAULT_FONT_RUNTIME_PATH)
    if(NOT IS_ABSOLUTE "${GUM_DEFAULT_FONT_RUNTIME_PATH}")
        message(FATAL_ERROR "GUM_DEFAULT_FONT_RUNTIME_PATH must be absolute")
    endif()
else()
    set(GUM_DEFAULT_FONT_RUNTIME_PATH "${GUM_DEFAULT_FONT_BUILD_PATH}")
endif()

function(gum_enable_sanitizers target)
    if(NOT GUM_ENABLE_SANITIZERS)
        return()
    endif()

    if(NOT CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        message(FATAL_ERROR "GUM_ENABLE_SANITIZERS requires GCC or Clang")
    endif()

    target_compile_options(${target} PRIVATE
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
    )

    get_target_property(target_type ${target} TYPE)
    if(NOT target_type STREQUAL "STATIC_LIBRARY")
        set_property(TARGET ${target} APPEND_STRING PROPERTY LINK_FLAGS " -fsanitize=address,undefined")
    endif()
endfunction()

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
