file(GLOB_RECURSE EXAMPLE_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.c")

foreach(src IN LISTS EXAMPLE_SOURCES)
    get_filename_component(name ${src} NAME_WE)
    get_filename_component(dir  ${src} DIRECTORY)

    if(dir)
        string(REPLACE "/" "_" target "${dir}_${name}")
    else()
        set(target "${name}")
    endif()

    add_executable(${target} ${src})
    target_link_libraries(${target} libGumball)

    set_target_properties(${target} PROPERTIES
        OUTPUT_NAME "${name}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${dir}"
    )
endforeach()
