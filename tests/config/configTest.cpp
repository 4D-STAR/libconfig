#include <gtest/gtest.h>
#include "config.h"
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <sstream>

std::string EXAMPLE_FILENAME = std::string(getenv("MESON_SOURCE_ROOT")) + "/tests/config/example.yaml";
/**
 * @file configTest.cpp
 * @brief Unit tests for the Config class.
 */

/**
 * @brief Test suite for the Config class.
 */
class configTest : public ::testing::Test {};

/**
 * @brief Test the constructor of the Config class.
 */
TEST_F(configTest, constructor) {
    EXPECT_NO_THROW(Config::getInstance());
}

TEST_F(configTest, loadConfig) {
    Config& config = Config::getInstance();
    EXPECT_TRUE(config.loadConfig(EXAMPLE_FILENAME));
}

TEST_F(configTest, singletonTest) {
    Config& config1 = Config::getInstance();
    Config& config2 = Config::getInstance();
    EXPECT_EQ(&config1, &config2);
}

TEST_F(configTest, getTest) {
    Config& config = Config::getInstance();
    config.loadConfig(EXAMPLE_FILENAME);
    int maxIter = config.get<int>("opac:lowTemp:numeric:maxIter", 10);
    EXPECT_EQ(maxIter, 100);
    EXPECT_NE(maxIter, 10);

    std::string logLevel = config.get<std::string>("logLevel", "DEBUG");
    EXPECT_EQ(logLevel, "INFO");
    EXPECT_NE(logLevel, "DEBUG");

    float polytropicIndex = config.get<float>("poly:physics:index", 2);
    EXPECT_EQ(polytropicIndex, 1.5);
    EXPECT_NE(polytropicIndex, 2);

    float polytropicIndex2 = config.get<float>("poly:physics:index2", 2.0);
    EXPECT_EQ(polytropicIndex2, 2.0);
}

TEST_F(configTest, secondSingleton) {
    Config& config = Config::getInstance();
    EXPECT_EQ(config.get<int>("opac:lowTemp:numeric:maxIter", 10), 100);
}
