cmake_minimum_required(VERSION 3.10)
project(tape_sort)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(tape_sort 
    src/main.cpp
    src/file_tape.cpp
    src/sorter.cpp
)

find_package(Threads REQUIRED)
target_link_libraries(tape_sort PRIVATE Threads::Threads)

if(CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries(tape_sort PRIVATE stdc++fs)
endif()