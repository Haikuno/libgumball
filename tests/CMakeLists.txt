set(GUMBALL_TEST_SOURCES
    include/core/gumball_inputsystem_test_suite.h
    source/core/gumball_inputsystem_test_suite.c
    include/core/gumball_manager_test_suite.h
    source/core/gumball_manager_test_suite.c
    include/core/gumball_navigation_test_suite.h
    source/core/gumball_navigation_test_suite.c
    include/elements/gumball_common_test_suite.h
    source/elements/gumball_common_test_suite.c
    include/elements/gumball_container_test_suite.h
    source/elements/gumball_container_test_suite.c
    include/elements/gumball_hierarchy_test_suite.h
    source/elements/gumball_hierarchy_test_suite.c
    include/elements/gumball_modelview_test_suite.h
    source/elements/gumball_modelview_test_suite.c
    include/elements/gumball_root_test_suite.h
    source/elements/gumball_root_test_suite.c
    include/elements/gumball_widget_test_suite.h
    source/elements/gumball_widget_test_suite.c
    include/types/gumball_animator_test_suite.h
    source/types/gumball_animator_test_suite.c
    include/types/gumball_rectangle_test_suite.h
    source/types/gumball_rectangle_test_suite.c
    include/types/gumball_vector2_test_suite.h
    source/types/gumball_vector2_test_suite.c
    source/gumball_test_main.c
)

set(GUMBALL_TEST_RESOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/resources")
file(MAKE_DIRECTORY "${GUMBALL_TEST_RESOURCE_DIR}")
configure_file(koslogo.png "${GUMBALL_TEST_RESOURCE_DIR}/koslogo.png" COPYONLY)
configure_file(gumball_test.ttf "${GUMBALL_TEST_RESOURCE_DIR}/gumball_test.ttf" COPYONLY)

add_library(GumballHeaderCompileC OBJECT
    compile/gumball_inputsystem_compile.c
)
target_link_libraries(GumballHeaderCompileC PRIVATE libGumball)

add_executable(GumballTests ${GUMBALL_TEST_SOURCES})
target_link_libraries(GumballTests PRIVATE libGumball)
target_include_directories(GumballTests PRIVATE
    include
    ${PROJECT_SOURCE_DIR}/lib/src
)
target_compile_definitions(GumballTests PRIVATE
    GUM_TEST_BACKEND_${GUM_BACKEND}=1
    "GUM_TEST_RESOURCE_DIR=\"${GUMBALL_TEST_RESOURCE_DIR}\""
)
gum_enable_sanitizers(GumballTests)

add_test(NAME GumballTests COMMAND GumballTests)

add_executable(GumballDefaultFont
    integration/gumball_default_font_test.c
)
target_link_libraries(GumballDefaultFont PRIVATE libGumball)
target_compile_definitions(GumballDefaultFont PRIVATE
    GUM_TEST_BACKEND_${GUM_BACKEND}=1
    "GUM_TEST_DEFAULT_FONT_PATH=\"${GUM_DEFAULT_FONT_RUNTIME_PATH}\""
    "GUM_TEST_CUSTOM_FONT_PATH=\"${GUMBALL_TEST_RESOURCE_DIR}/gumball_test.ttf\""
)
gum_enable_sanitizers(GumballDefaultFont)

add_test(NAME GumballDefaultFont COMMAND GumballDefaultFont)

if(GUM_BACKEND STREQUAL "SDL3")
    add_executable(GumballSDLRendererRecreation
        sdl3/gumball_renderer_recreation_test.c
    )
    target_link_libraries(GumballSDLRendererRecreation PRIVATE libGumball)
    gum_enable_sanitizers(GumballSDLRendererRecreation)

    add_test(NAME GumballSDLRendererRecreation COMMAND GumballSDLRendererRecreation)
    set_tests_properties(GumballSDLRendererRecreation PROPERTIES
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
elseif(GUM_BACKEND STREQUAL "RAYLIB")
    add_executable(GumballRaylibResourceRestart
        raylib/gumball_resource_restart_test.c
    )
    target_link_libraries(GumballRaylibResourceRestart PRIVATE libGumball)
    gum_enable_sanitizers(GumballRaylibResourceRestart)

    add_test(NAME GumballRaylibResourceRestart COMMAND GumballRaylibResourceRestart)
    set_tests_properties(GumballRaylibResourceRestart PROPERTIES
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
endif()

add_executable(GumballDev ${GUM_BACKEND_DIR}/dev.c)
target_link_libraries(GumballDev PRIVATE libGumball)
set_target_properties(GumballDev PROPERTIES
    OUTPUT_NAME dev
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${GUM_BACKEND_DIR}"
)
gum_enable_sanitizers(GumballDev)

add_executable(GumballBackendParity
    parity/gumball_backend_parity_scene.c
    ${GUM_BACKEND_DIR}/gumball_backend_parity.c
)
target_link_libraries(GumballBackendParity PRIVATE libGumball)
target_include_directories(GumballBackendParity PRIVATE parity)
gum_enable_sanitizers(GumballBackendParity)

if(GUM_PARITY_SDL3_EXECUTABLE AND GUM_PARITY_RAYLIB_EXECUTABLE)
    find_program(PYTHON_EXECUTABLE NAMES python3 python)
    if(NOT PYTHON_EXECUTABLE)
        message(FATAL_ERROR "Cross-backend parity testing requires Python 3")
    endif()
    add_test(NAME GumballBackendParityCompare
        COMMAND ${CMAKE_COMMAND}
            -DSDL3_EXECUTABLE=${GUM_PARITY_SDL3_EXECUTABLE}
            -DRAYLIB_EXECUTABLE=${GUM_PARITY_RAYLIB_EXECUTABLE}
            -DPYTHON_EXECUTABLE=${PYTHON_EXECUTABLE}
            -DCOMPARE_SCRIPT=${CMAKE_CURRENT_SOURCE_DIR}/parity/compare_images.py
            -DOUTPUT_DIRECTORY=${CMAKE_CURRENT_BINARY_DIR}/parity-output
            -P ${CMAKE_CURRENT_SOURCE_DIR}/parity/run_backend_parity.cmake
    )
endif()
