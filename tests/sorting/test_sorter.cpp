#include "sorting/sorter.h"
#include "tape/file_tape.h"
#include "tape/file_tape_factory.h"
#include "tape/config.h"

#include <gtest/gtest.h>
#include <algorithm>
#include <filesystem>
#include <memory>
#include <random>
#include <string>
#include <vector>

namespace {

class MockTapeFactory : public tape::TapeFactory {
public:
    std::unique_ptr<tape::ITape> createTape(const std::string& filename, const tape::Config& config) override {
        return std::make_unique<tape::FileTape>(filename, config);
    }
    
    std::unique_ptr<tape::ITape> createEmptyTape(const std::string& filename, const tape::Config& config) override {
        return tape::FileTape::createEmpty(filename, config);
    }
};

class TapeSorterTest : public ::testing::Test {
protected:
    void SetUp() override {
        tempDir = "test_tmp_" + std::to_string(std::random_device{}());
        std::filesystem::create_directory(tempDir);
        
        inputFilename = tempDir + "/input.bin";
        outputFilename = tempDir + "/output.bin";
        
        config = tape::Config(0, 0, 0, 0); 
        tapeFactory = std::make_shared<MockTapeFactory>();
    }
    
    void TearDown() override {
        if (std::filesystem::exists(tempDir)) {
            std::filesystem::remove_all(tempDir);
        }
    }
    
    void createInputTapeWithData(const std::vector<int32_t>& data) {
        auto inputTape = tape::FileTape::createEmpty(inputFilename, config);
        
        for (const auto& value : data) {
            inputTape->write(value);
            inputTape->moveNext();
        }
    }
    
    std::vector<int32_t> readAllFromTape(const std::string& filename) {
        auto tape = tape::FileTape(filename, config);
        tape.rewind();
        
        std::vector<int32_t> result;
        
        while (!tape.isEnd()) {
            result.push_back(tape.read());
            tape.moveNext();
        }
        
        return result;
    }
    
    bool isSorted(const std::string& filename) {
        auto values = readAllFromTape(filename);
        return std::is_sorted(values.begin(), values.end());
    }
    
    std::string tempDir;
    std::string inputFilename;
    std::string outputFilename;
    tape::Config config;
    std::shared_ptr<tape::TapeFactory> tapeFactory;
};

TEST_F(TapeSorterTest, SortsEmptyTape) {
    createInputTapeWithData({});
    
    auto inputTape = tapeFactory->createTape(inputFilename, config);
    auto outputTape = tapeFactory->createEmptyTape(outputFilename, config);
    
    sorting::TapeSorter sorter(1024, config, tapeFactory, tempDir);
    sorter.sort(*inputTape, *outputTape);
    
    auto result = readAllFromTape(outputFilename);
    EXPECT_TRUE(result.empty());
}

TEST_F(TapeSorterTest, SortsAlreadySortedData) {
    std::vector<int32_t> testData = {1, 2, 3, 4, 5};
    createInputTapeWithData(testData);
    
    auto inputTape = tapeFactory->createTape(inputFilename, config);
    auto outputTape = tapeFactory->createEmptyTape(outputFilename, config);
    
    sorting::TapeSorter sorter(1024, config, tapeFactory, tempDir);
    sorter.sort(*inputTape, *outputTape);
    
    auto result = readAllFromTape(outputFilename);
    EXPECT_EQ(testData, result);
}

TEST_F(TapeSorterTest, SortsReversedData) {
    std::vector<int32_t> testData = {5, 4, 3, 2, 1};
    std::vector<int32_t> expected = {1, 2, 3, 4, 5};
    
    createInputTapeWithData(testData);
    
    auto inputTape = tapeFactory->createTape(inputFilename, config);
    auto outputTape = tapeFactory->createEmptyTape(outputFilename, config);
    
    sorting::TapeSorter sorter(1024, config, tapeFactory, tempDir);
    sorter.sort(*inputTape, *outputTape);
    
    auto result = readAllFromTape(outputFilename);
    EXPECT_EQ(expected, result);
}

TEST_F(TapeSorterTest, SortsRandomData) {
    const size_t dataSize = 1000;
    std::vector<int32_t> testData(dataSize);
    
    std::mt19937 gen(42); 
    std::uniform_int_distribution<int32_t> dist(1, 1000000);
    
    for (auto& value : testData) {
        value = dist(gen);
    }
    
    createInputTapeWithData(testData);
    
    auto inputTape = tapeFactory->createTape(inputFilename, config);
    auto outputTape = tapeFactory->createEmptyTape(outputFilename, config);
    
    sorting::TapeSorter sorter(64 * 1024, config, tapeFactory, tempDir);
    sorter.sort(*inputTape, *outputTape);
    
    EXPECT_TRUE(isSorted(outputFilename));
    
    auto result = readAllFromTape(outputFilename);
    std::sort(testData.begin(), testData.end());
    EXPECT_EQ(testData, result);
}

TEST_F(TapeSorterTest, HandlesSmallChunks) {
    const size_t dataSize = 10000;
    std::vector<int32_t> testData(dataSize);
    
    std::mt19937 gen(42);
    std::uniform_int_distribution<int32_t> dist(1, 1000000);
    
    for (auto& value : testData) {
        value = dist(gen);
    }
    
    createInputTapeWithData(testData);
    
    auto inputTape = tapeFactory->createTape(inputFilename, config);
    auto outputTape = tapeFactory->createEmptyTape(outputFilename, config);

    const size_t smallMemoryLimit = 40;
    
    sorting::TapeSorter sorter(smallMemoryLimit, config, tapeFactory, tempDir);
    sorter.sort(*inputTape, *outputTape);
    
    EXPECT_TRUE(isSorted(outputFilename));
    
    auto result = readAllFromTape(outputFilename);
    std::sort(testData.begin(), testData.end());
    EXPECT_EQ(testData, result);
}

TEST_F(TapeSorterTest, HandlesLargeDataSmallMemory) {
    const size_t dataSize = 100000;
    std::vector<int32_t> testData(dataSize);
    
    std::mt19937 gen(42);
    std::uniform_int_distribution<int32_t> dist(1, 1000000);
    
    for (auto& value : testData) {
        value = dist(gen);
    }
    
    createInputTapeWithData(testData);
    
    auto inputTape = tapeFactory->createTape(inputFilename, config);
    auto outputTape = tapeFactory->createEmptyTape(outputFilename, config);
    
    const size_t verySmallMemoryLimit = 100;
    
    sorting::TapeSorter sorter(verySmallMemoryLimit, config, tapeFactory, tempDir);
    sorter.sort(*inputTape, *outputTape);
    
    EXPECT_TRUE(isSorted(outputFilename));
    
    auto result = readAllFromTape(outputFilename);
    EXPECT_EQ(dataSize, result.size());

    std::sort(testData.begin(), testData.end());
    
    for (size_t i = 0; i < 10 && i < dataSize; ++i) {
        EXPECT_EQ(testData[i], result[i]);
    }
    
    for (size_t i = 1; i <= 10 && i <= dataSize; ++i) {
        EXPECT_EQ(testData[dataSize - i], result[dataSize - i]);
    }
}

TEST_F(TapeSorterTest, HandlesIdenticalValues) {
    std::vector<int32_t> testData = {5, 5, 5, 5, 5};
    std::vector<int32_t> expected = {5, 5, 5, 5, 5};
    
    createInputTapeWithData(testData);
    
    auto inputTape = tapeFactory->createTape(inputFilename, config);
    auto outputTape = tapeFactory->createEmptyTape(outputFilename, config);
    sorting::TapeSorter sorter(8, config, tapeFactory, tempDir);
    sorter.sort(*inputTape, *outputTape);
    
    auto result = readAllFromTape(outputFilename);
    EXPECT_EQ(expected, result);
}

TEST_F(TapeSorterTest, HandlesNegativeValues) {
    std::vector<int32_t> testData = {-5, 3, -2, 0, 10, -8};
    std::vector<int32_t> expected = {-8, -5, -2, 0, 3, 10};
    
    createInputTapeWithData(testData);
    
    auto inputTape = tapeFactory->createTape(inputFilename, config);
    auto outputTape = tapeFactory->createEmptyTape(outputFilename, config);
    
    sorting::TapeSorter sorter(1024, config, tapeFactory, tempDir);
    sorter.sort(*inputTape, *outputTape);
    
    auto result = readAllFromTape(outputFilename);
    EXPECT_EQ(expected, result);
}

TEST_F(TapeSorterTest, SingleElementTape) {
    std::vector<int32_t> testData = {42};
    
    createInputTapeWithData(testData);
    
    auto inputTape = tapeFactory->createTape(inputFilename, config);
    auto outputTape = tapeFactory->createEmptyTape(outputFilename, config);
    
    sorting::TapeSorter sorter(1024, config, tapeFactory, tempDir);
    sorter.sort(*inputTape, *outputTape);
    
    auto result = readAllFromTape(outputFilename);
    EXPECT_EQ(testData, result);
}

TEST_F(TapeSorterTest, StressTestWithVerySmallChunks) {
    const size_t dataSize = 1000;
    std::vector<int32_t> testData(dataSize);
    
    std::mt19937 gen(42);
    std::uniform_int_distribution<int32_t> dist(-1000000, 1000000);
    
    for (auto& value : testData) {
        value = dist(gen);
    }
    
    createInputTapeWithData(testData);
    
    auto inputTape = tapeFactory->createTape(inputFilename, config);
    auto outputTape = tapeFactory->createEmptyTape(outputFilename, config);
    const size_t minimumMemoryLimit = 4;
    
    sorting::TapeSorter sorter(minimumMemoryLimit, config, tapeFactory, tempDir);
    sorter.sort(*inputTape, *outputTape);
    
    EXPECT_TRUE(isSorted(outputFilename));
    
    auto result = readAllFromTape(outputFilename);
    std::sort(testData.begin(), testData.end());
    EXPECT_EQ(testData, result);
}

}  // namespace