cmake_minimum_required(VERSION 3.16)
project(terminalShogi LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output executable at repo root
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

# Collect all source files
file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/src/core/**/*.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/*.cpp
    ${CMAKE_SOURCE_DIR}/src/game/**/*.cpp
    ${CMAKE_SOURCE_DIR}/src/main.cpp
)

# Find ncurses
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

# Build executable
add_executable(terminalShogi ${SOURCES})

# Link ncurses
target_link_libraries(terminalShogi ${CURSES_LIBRARIES})
