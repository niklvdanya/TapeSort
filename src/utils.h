#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>

class TapeUtils {
public:
    static void generateRandomTape(const std::string& filename, size_t count, int32_t minValue = 0, int32_t maxValue = 1000000) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int32_t> dist(minValue, maxValue);
        
        std::ofstream file(filename, std::ios::binary | std::ios::trunc);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file for writing: " + filename);
        }
        
        std::cout << "Generating " << count << " random elements..." << std::endl;
        
        for (size_t i = 0; i < count; ++i) {
            int32_t value = dist(gen);
            file.write(reinterpret_cast<const char*>(&value), sizeof(value));
        }
        
        file.close();
        std::cout << "Done generating random data" << std::endl;
    }
    
    static void printTapeContent(const std::string& filename, size_t maxElements = 20) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file for reading: " + filename);
        }
        
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        
        size_t numElements = fileSize / sizeof(int32_t);
        
        std::cout << "File contains " << numElements << " elements" << std::endl;
        
        size_t elementsToShow = std::min(numElements, maxElements);
        std::vector<int32_t> data(elementsToShow);
        
        file.read(reinterpret_cast<char*>(data.data()), elementsToShow * sizeof(int32_t));
        
        for (size_t i = 0; i < elementsToShow; ++i) {
            std::cout << i << ": " << data[i] << std::endl;
        }
        
        if (numElements > maxElements) {
            std::cout << "..." << std::endl;
            
            file.seekg((numElements - maxElements) * sizeof(int32_t));
            file.read(reinterpret_cast<char*>(data.data()), maxElements * sizeof(int32_t));
            
            for (size_t i = 0; i < maxElements; ++i) {
                std::cout << (numElements - maxElements + i) << ": " << data[i] << std::endl;
            }
        }
        
        file.close();
    }
    
    static bool validateSorted(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file for validation: " + filename);
        }
        
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        
        if (fileSize == 0) {
            std::cout << "File is empty" << std::endl;
            return true;
        }
        
        size_t numElements = fileSize / sizeof(int32_t);
        
        std::cout << "Validating " << numElements << " elements..." << std::endl;
        
        int32_t prev, curr;
        file.read(reinterpret_cast<char*>(&prev), sizeof(prev));
        
        bool isSorted = true;
        size_t errorsFound = 0;
        
        for (size_t i = 1; i < numElements; ++i) {
            file.read(reinterpret_cast<char*>(&curr), sizeof(curr));
            
            if (curr < prev) {
                if (errorsFound < 5) {
                    std::cout << "Error at position " << i << ": " << prev << " > " << curr << std::endl;
                }
                isSorted = false;
                errorsFound++;
            }
            
            prev = curr;
            
            if (i % (numElements / 10) == 0) {
                std::cout << "Validated " << i << " of " << numElements << " elements" << std::endl;
            }
        }
        
        if (isSorted) {
            std::cout << "Validation successful: File is properly sorted" << std::endl;
        } else {
            std::cout << "Validation failed: Found " << errorsFound << " errors" << std::endl;
        }
        
        return isSorted;
    }
};