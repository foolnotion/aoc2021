cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    aoc2021
    VERSION 0.1.0
    DESCRIPTION "Advent of Code 2021"
    HOMEPAGE_URL "https://adventofcode.com"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

docs_early_return()

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

# ---- Find deps ----
find_package(Eigen3 REQUIRED)
find_package(FastFloat REQUIRED)
find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(cpp-sort REQUIRED)
find_package(fmt REQUIRED)
find_package(robin_hood REQUIRED)
find_package(scn CONFIG REQUIRED)

# ---- Declare library ----

add_library(
    aoc2021_lib OBJECT
    source/day01.cpp
    source/day02.cpp
    source/day03.cpp
    source/day04.cpp
    source/day05.cpp
    source/day06.cpp
    source/day07.cpp
    source/day08.cpp
    source/day09.cpp
    source/day10.cpp
    source/day11.cpp
)

target_include_directories(
    aoc2021_lib ${aoc2021_warning_guard}
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source>"
    ${EIGEN3_INCLUDE_DIR}
)

target_compile_features(aoc2021_lib PUBLIC cxx_std_20)

target_compile_options(aoc2021_lib PUBLIC "$<$<CONFIG:Release>:-O3;-march=x86-64;-mavx2;-mfma>")

# ---- Declare executable ----

add_executable(aoc2021_aoc2021 source/main.cpp)
add_executable(aoc2021::aoc2021 ALIAS aoc2021_aoc2021)

set_target_properties(
    aoc2021_aoc2021 PROPERTIES
    OUTPUT_NAME aoc2021
    EXPORT_NAME aoc2021
)

target_compile_features(aoc2021_aoc2021 PRIVATE cxx_std_20)

target_link_libraries(aoc2021_aoc2021 PRIVATE aoc2021_lib fmt::fmt Microsoft.GSL::GSL scn::scn)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT aoc2021_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of aoc2021"
  )
endif()

include(cmake/dev-mode.cmake)
