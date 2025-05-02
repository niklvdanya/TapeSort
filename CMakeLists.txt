cmake_minimum_required(VERSION 3.10)
project(tape_sort)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(SOURCES
    src/tape/config.cpp
    src/tape/file_tape.cpp
    src/tape/file_tape_factory.cpp
    src/sorting/sorter.cpp
    src/utils/tape_utils.cpp
    src/app/command_factory.cpp
    src/app/commands.cpp
    src/main.cpp
)

add_executable(tape_sort ${SOURCES})

find_package(Threads REQUIRED)
target_link_libraries(tape_sort PRIVATE Threads::Threads)

if(CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries(tape_sort PRIVATE stdc++fs)
endif()

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/tmp)
file(COPY ${CMAKE_SOURCE_DIR}/config/tape_config.txt DESTINATION ${CMAKE_BINARY_DIR}/config)

enable_testing()
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/v1.13.0.zip
)
FetchContent_MakeAvailable(googletest)

set(LIB_SOURCES
    src/tape/config.cpp
    src/tape/file_tape.cpp
    src/tape/file_tape_factory.cpp
    src/sorting/sorter.cpp
    src/utils/tape_utils.cpp
    src/app/command_factory.cpp
    src/app/commands.cpp
)

add_library(tape_sort_lib STATIC ${LIB_SOURCES})
target_link_libraries(tape_sort_lib PRIVATE Threads::Threads)
if(CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries(tape_sort_lib PRIVATE stdc++fs)
endif()

set(TEST_SOURCES
    tests/tape/test_file_tape.cpp
    tests/sorting/test_sorter.cpp
    tests/main.cpp
)

add_executable(tape_sort_tests ${TEST_SOURCES})
target_link_libraries(tape_sort_tests PRIVATE tape_sort_lib gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(tape_sort_tests)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/tests/tmp)
file(COPY ${CMAKE_SOURCE_DIR}/config/tape_config.txt DESTINATION ${CMAKE_BINARY_DIR}/tests/config)