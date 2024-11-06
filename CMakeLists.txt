cmake_minimum_required(VERSION 3.10)
project(DoodleJump)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build)

file(GLOB SOURCE_FILES
    "src/*.cpp"
    "src/*.h"
)

add_executable(DoodleJump ${SOURCE_FILES})

target_link_libraries(DoodleJump PRIVATE
    gdiplus
    user32
    gdi32
)

add_custom_command(TARGET DoodleJump POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/material
    ${CMAKE_SOURCE_DIR}/build/material
)