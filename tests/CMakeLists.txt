set(GUMBALL_TEST_SOURCES
    include/core/gumball_navigation_test_suite.h
    source/core/gumball_navigation_test_suite.c
    include/elements/gumball_container_test_suite.h
    source/elements/gumball_container_test_suite.c
    include/types/gumball_animator_test_suite.h
    source/types/gumball_animator_test_suite.c
    include/types/gumball_rectangle_test_suite.h
    source/types/gumball_rectangle_test_suite.c
    source/gumball_test_main.c
)

add_executable(GumballTests ${GUMBALL_TEST_SOURCES})
target_link_libraries(GumballTests PRIVATE libGumball)
target_include_directories(GumballTests PRIVATE include)
target_compile_definitions(GumballTests PRIVATE GUM_TEST_BACKEND_${GUM_BACKEND}=1)

add_test(NAME GumballTests COMMAND GumballTests)

add_executable(GumballDev ${GUM_BACKEND_DIR}/dev.c)
target_link_libraries(GumballDev PRIVATE libGumball)
set_target_properties(GumballDev PROPERTIES
    OUTPUT_NAME dev
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${GUM_BACKEND_DIR}"
)

add_executable(GumballBackendParity
    parity/gumball_backend_parity_scene.c
    ${GUM_BACKEND_DIR}/gumball_backend_parity.c
)
target_link_libraries(GumballBackendParity PRIVATE libGumball)
target_include_directories(GumballBackendParity PRIVATE parity)
