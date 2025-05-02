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