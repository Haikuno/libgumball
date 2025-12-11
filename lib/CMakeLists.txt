add_subdirectory(libgimbal)

file(GLOB_RECURSE SRCS    CONFIGURE_DEPENDS	  src/*.c)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS   api/gumball/*.h)
list(FILTER SRCS EXCLUDE REGEX ".*/backends/.*")

if (GUM_BUILD_BACKEND_SDL3)
	list(APPEND SRCS src/backends/gumball_backend_SDL3.c)
endif()

if (GUM_BUILD_BACKEND_RAYLIB)
    file(GLOB RAYLIB_SRCS src/backends/raylib/*.c)
    list(APPEND SRCS ${RAYLIB_SRCS})
endif()

if (PLATFORM_DREAMCAST)
	include_directories(${KOS_PORTS}/include/raylib) # To use <raylib.h> like in PC, instead of <raylib/raylib.h>
endif()

add_library(libGumball STATIC ${SRCS} ${HEADERS})
target_link_libraries(libGumball libGimbal raylib)
target_include_directories(libGumball
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/api
)
