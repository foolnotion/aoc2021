# Parent project does not export its library target, so this CML implicitly
# depends on being added from it, i.e. the testing is done only from the build
# tree and is not feasible from an install location

project(aoc2021Tests LANGUAGES CXX)

add_executable(aoc2021_test source/aoc2021_test.cpp)
target_link_libraries(aoc2021_test PRIVATE aoc2021_lib)
target_compile_features(aoc2021_test PRIVATE cxx_std_17)

add_test(NAME aoc2021_test COMMAND aoc2021_test)
