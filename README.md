# TapeSort

Application that emulates external sorting of data on tape-like storage devices. This implementation uses the external merge sort algorithm with configurable memory constraints to sort integer values.

# Overview

The project simulates sorting on tape-like storage, where:

- Random access is not available (only sequential read/write)
- Operations like reading, writing, rewinding, and moving to the next element have configurable delays
- Memory constraints can be specified to limit how much data can be processed at once

# Build
1. Clone the repository:
```bash
git clone https://github.com/niklvdanya/TapeSort.git
cd TapeSort
```
2. Create a build directory and configure with CMake:
```bash
mkdir build
cd build
cmake ..
```
3. Build the project:
```bash
cmake --build .
```

# Running Tests
After building, you can run the unit tests:
```bash
ctest
```
Or directly:
```bash
./tape_sort_tests
```

# Usage

The application supports the following commands:

## Basic Commands
The application supports the following commands:
```bash
tape_sort sort <input_file> <output_file> [memory_limit_kb] [config_file]
tape_sort generate <output_file> <count>
tape_sort print <file> [max_elements]
tape_sort validate <file>
tape_sort help
```
## Command Examples

Generate random data:
```bash
./tape_sort generate data.bin 1000
```
Generates a binary file containing 1000 random integers.

Sort data:
```bash
./tape_sort sort data.bin sorted.bin 64
```
Sorts the data from data.bin to sorted.bin with a memory limit of 64KB.

Print file contents:
```bash
./tape_sort print sorted.bin 20
```
Displays the first and last 20 elements of sorted.bin.

Validate sorted data:
```bash
./tape_sort validate sorted.bin
```
Checks if the data in sorted.bin is correctly sorted.

Get help:
```bash
./tape_sort help
```
Displays available commands and their descriptions.

## Configuration
You can customize the tape operation delays by creating a configuration file with the following format:
```bash
readDelay <value>
writeDelay <value>
rewindDelay <value>
shiftDelay <value>
```
Where each value is specified in milliseconds. A sample configuration file is included at `config/tape_config.txt.`