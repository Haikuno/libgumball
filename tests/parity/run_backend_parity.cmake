if(NOT SDL3_EXECUTABLE OR NOT RAYLIB_EXECUTABLE OR
   NOT PYTHON_EXECUTABLE OR NOT COMPARE_SCRIPT OR NOT OUTPUT_DIRECTORY)
    message(FATAL_ERROR "Parity test requires SDL3_EXECUTABLE, RAYLIB_EXECUTABLE, PYTHON_EXECUTABLE, COMPARE_SCRIPT, and OUTPUT_DIRECTORY")
endif()

file(MAKE_DIRECTORY "${OUTPUT_DIRECTORY}")

set(SDL3_OUTPUT "${OUTPUT_DIRECTORY}/backend-parity-sdl3.png")
set(RAYLIB_OUTPUT "${OUTPUT_DIRECTORY}/backend-parity-raylib.png")
set(DIFF_OUTPUT "${OUTPUT_DIRECTORY}/backend-parity-diff.png")

execute_process(
    COMMAND "${SDL3_EXECUTABLE}" "${SDL3_OUTPUT}"
    RESULT_VARIABLE SDL3_RESULT
)
if(NOT SDL3_RESULT EQUAL 0)
    message(FATAL_ERROR "SDL3 parity executable failed with exit code ${SDL3_RESULT}")
endif()

execute_process(
    COMMAND "${RAYLIB_EXECUTABLE}" "${RAYLIB_OUTPUT}"
    RESULT_VARIABLE RAYLIB_RESULT
)
if(NOT RAYLIB_RESULT EQUAL 0)
    message(FATAL_ERROR "raylib parity executable failed with exit code ${RAYLIB_RESULT}")
endif()

execute_process(
    COMMAND "${PYTHON_EXECUTABLE}" "${COMPARE_SCRIPT}"
            "${SDL3_OUTPUT}" "${RAYLIB_OUTPUT}" "${DIFF_OUTPUT}"
    RESULT_VARIABLE COMPARE_RESULT
)
if(NOT COMPARE_RESULT EQUAL 0)
    message(FATAL_ERROR "Backend parity comparison failed with exit code ${COMPARE_RESULT}")
endif()
