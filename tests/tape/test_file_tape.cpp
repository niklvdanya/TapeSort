#include "tape/file_tape.h"
#include "tape/config.h"

#include <gtest/gtest.h>
#include <filesystem>
#include <random>
#include <string>
#include <vector>

namespace {

constexpr int kDefaultReadDelay = 0;
constexpr int kDefaultWriteDelay = 0;
constexpr int kDefaultRewindDelay = 0;
constexpr int kDefaultShiftDelay = 0;

class FileTapeTest : public ::testing::Test {
protected:
    void SetUp() override {
        tempFilename = "test_tape_" + std::to_string(std::random_device{}()) + ".bin";
        config = tape::Config(kDefaultReadDelay, kDefaultWriteDelay, kDefaultRewindDelay, kDefaultShiftDelay);
    }

    void TearDown() override {
        if (std::filesystem::exists(tempFilename)) {
            std::filesystem::remove(tempFilename);
        }
    }

    std::string tempFilename;
    tape::Config config;
};

TEST_F(FileTapeTest, CreatesEmptyTape) {
    auto tape = tape::FileTape::createEmpty(tempFilename, config);
    ASSERT_TRUE(tape);
    EXPECT_TRUE(tape->isEnd());
    EXPECT_EQ(0, tape->getPosition());
}

TEST_F(FileTapeTest, WritesToTape) {
    auto tape = tape::FileTape::createEmpty(tempFilename, config);
    
    const int testValue = 42;
    tape->write(testValue);
    
    tape->rewind();
    EXPECT_FALSE(tape->isEnd());
    EXPECT_EQ(testValue, tape->read());
}

TEST_F(FileTapeTest, MovesNextPosition) {
    auto tape = tape::FileTape::createEmpty(tempFilename, config);
    
    const std::vector<int32_t> values = {10, 20, 30, 40, 50};
    
    for (const auto& value : values) {
        tape->write(value);
        tape->moveNext();
    }
    
    tape->rewind();
    
    for (size_t i = 0; i < values.size(); ++i) {
        EXPECT_EQ(i, tape->getPosition());
        EXPECT_EQ(values[i], tape->read());
        tape->moveNext();
    }
    
    EXPECT_TRUE(tape->isEnd());
}

TEST_F(FileTapeTest, RewindsTape) {
    auto tape = tape::FileTape::createEmpty(tempFilename, config);
    
    const std::vector<int32_t> values = {1, 2, 3};
    
    for (const auto& value : values) {
        tape->write(value);
        tape->moveNext();
    }
    
    EXPECT_TRUE(tape->isEnd());
    
    tape->rewind();
    
    EXPECT_FALSE(tape->isEnd());
    EXPECT_EQ(0, tape->getPosition());
    EXPECT_EQ(values[0], tape->read());
}

TEST_F(FileTapeTest, ThrowsWhenReadingPastEnd) {
    auto tape = tape::FileTape::createEmpty(tempFilename, config);
    
    EXPECT_TRUE(tape->isEnd());
    EXPECT_THROW(tape->read(), std::runtime_error);
}

TEST_F(FileTapeTest, WritesAndReadsLargeData) {
    auto tape = tape::FileTape::createEmpty(tempFilename, config);
    
    const size_t dataSize = 10000;
    std::vector<int32_t> testData(dataSize);
    
    std::mt19937 gen(42); 
    std::uniform_int_distribution<int32_t> dist(1, 1000000);
    
    for (auto& value : testData) {
        value = dist(gen);
    }
    
    for (const auto& value : testData) {
        tape->write(value);
        tape->moveNext();
    }
    
    tape->rewind();
    
    for (size_t i = 0; i < dataSize; ++i) {
        EXPECT_EQ(testData[i], tape->read());
        tape->moveNext();
    }
    
    EXPECT_TRUE(tape->isEnd());
}

TEST_F(FileTapeTest, OverwritesExistingData) {
    auto tape = tape::FileTape::createEmpty(tempFilename, config);
    
    tape->write(100);
    tape->moveNext();
    tape->write(200);
    tape->moveNext();
    tape->write(300);
    
    tape->rewind();
    tape->moveNext();
    tape->write(999);
    
    tape->rewind();
    
    EXPECT_EQ(100, tape->read());
    tape->moveNext();
    EXPECT_EQ(999, tape->read());
    tape->moveNext();
    EXPECT_EQ(300, tape->read());
}

TEST_F(FileTapeTest, MoveConstructor) {
    auto originalTape = tape::FileTape::createEmpty(tempFilename, config);
    
    const std::vector<int32_t> values = {1, 2, 3, 4, 5};
    
    for (const auto& value : values) {
        originalTape->write(value);
        originalTape->moveNext();
    }
    
    originalTape->rewind();
    originalTape->moveNext();
    
    tape::FileTape movedTape(std::move(*originalTape));
    
    EXPECT_EQ(1, movedTape.getPosition());
    EXPECT_EQ(values[1], movedTape.read());
}

TEST_F(FileTapeTest, MoveAssignment) {
    auto firstTape = tape::FileTape::createEmpty(tempFilename, config);
    
    const std::vector<int32_t> values = {10, 20, 30, 40, 50};
    
    for (const auto& value : values) {
        firstTape->write(value);
        firstTape->moveNext();
    }
    
    firstTape->rewind();
    firstTape->moveNext();
    
    std::string secondFilename = tempFilename + "_second";
    auto secondTape = tape::FileTape::createEmpty(secondFilename, config);
    
    *secondTape = std::move(*firstTape);
    
    EXPECT_EQ(1, secondTape->getPosition());
    EXPECT_EQ(values[1], secondTape->read());
    
    if (std::filesystem::exists(secondFilename)) {
        std::filesystem::remove(secondFilename);
    }
}

}  // namespace