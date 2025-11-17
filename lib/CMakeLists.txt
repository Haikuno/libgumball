cmake_minimum_required(VERSION 3.10)
project(LibGumball)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_subdirectory(libgimbal)

file(GLOB_RECURSE SRCS    CONFIGURE_DEPENDS	  src/*.c)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS   api/gumball/*.h)
list(FILTER SRCS EXCLUDE REGEX ".*/backends/.*")

if (GUM_BUILD_BACKEND_SDL3)
	list(APPEND SRCS src/backends/gumball_backend_SDL3.c)
endif()

if (GUM_BUILD_BACKEND_RAYLIB)
	list(APPEND SRCS src/backends/gumball_backend_raylib.c)
endif()

add_library(libGumball STATIC ${SRCS} ${HEADERS})
target_link_libraries(libGumball libGimbal raylib SDL3)
target_include_directories(libGumball
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/api
)
