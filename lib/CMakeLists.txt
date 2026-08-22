add_subdirectory(libgimbal)

file(GLOB_RECURSE SRCS    CONFIGURE_DEPENDS src/*.c)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS api/gumball/*.h)
list(FILTER SRCS EXCLUDE REGEX ".*/backends/.*")

set(BACKEND_LIBS)

if(GUM_BACKEND STREQUAL "SDL3")
    file(GLOB BACKEND_SRCS CONFIGURE_DEPENDS src/backends/sdl3/*.c)
    list(APPEND SRCS ${BACKEND_SRCS})

    if(NOT TARGET SDL3::SDL3)
        find_package(SDL3 CONFIG REQUIRED)
    endif()
    if(NOT TARGET SDL3_ttf::SDL3_ttf)
        find_package(SDL3_ttf CONFIG REQUIRED)
    endif()
    if(NOT TARGET SDL3_image::SDL3_image)
        find_package(SDL3_image CONFIG REQUIRED)
    endif()

    list(APPEND BACKEND_LIBS SDL3::SDL3 SDL3_ttf::SDL3_ttf SDL3_image::SDL3_image)
elseif(GUM_BACKEND STREQUAL "RAYLIB")
    file(GLOB BACKEND_SRCS CONFIGURE_DEPENDS src/backends/raylib/*.c)
    list(APPEND SRCS ${BACKEND_SRCS})

    if(PLATFORM_DREAMCAST)
        include_directories(${KOS_PORTS}/include/raylib)
    elseif(NOT TARGET raylib)
        find_package(raylib CONFIG REQUIRED)
    endif()

    list(APPEND BACKEND_LIBS raylib)

    if(NOT PLATFORM_DREAMCAST)
        find_package(glfw3 CONFIG QUIET)
        if(TARGET glfw)
            list(APPEND BACKEND_LIBS glfw)
        endif()
    endif()
endif()

add_library(libGumball STATIC ${SRCS} ${HEADERS})
target_link_libraries(libGumball PUBLIC libGimbal ${BACKEND_LIBS})
target_include_directories(libGumball
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/api
)
gum_enable_sanitizers(libGumball)
