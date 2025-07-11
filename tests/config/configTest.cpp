#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <sstream>
#include <algorithm>

#include "fourdst/config/config.h"

std::string EXAMPLE_FILENAME = std::string(getenv("MESON_SOURCE_ROOT")) + "/tests/config/example.yaml";
/**
 * @file configTest.cpp
 * @brief Unit tests for the Config class.
 */

class configTestPrivateAccessor {
public:
    static bool callIsKeyInCache(fourdst::config::Config& config, const std::string& key) {
        return config.isKeyInCache(key);
    }

    static int callCacheSize(fourdst::config::Config& config) {
        return config.configMap.size();
    }
    
    static void callAddToCache(fourdst::config::Config& config, const std::string& key, const YAML::Node& node) {
        config.addToCache(key, node);
    }

    static void callRegisterKeyNotFound(fourdst::config::Config& config, const std::string& key) {
        config.registerUnknownKey(key);
    }

    static bool CheckIfKeyUnknown(fourdst::config::Config& config, const std::string& key) {
        if (std::find(config.unknownKeys.begin(), config.unknownKeys.end(), key) == config.unknownKeys.end()) {
            return false;
        }
        return true;
    }
};

/**
 * @brief Test suite for the Config class.
 */
class configTest : public ::testing::Test {};

/**
 * @brief Test the constructor of the Config class.
 */
TEST_F(configTest, constructor) {
    EXPECT_NO_THROW(fourdst::config::Config::getInstance());
}

TEST_F(configTest, loadConfig) {
    fourdst::config::Config& config = fourdst::config::Config::getInstance();
    EXPECT_TRUE(config.loadConfig(EXAMPLE_FILENAME));
}

TEST_F(configTest, singletonTest) {
    fourdst::config::Config& config1 = fourdst::config::Config::getInstance();
    fourdst::config::Config& config2 = fourdst::config::Config::getInstance();
    EXPECT_EQ(&config1, &config2);
}

TEST_F(configTest, getTest) {
    fourdst::config::Config& config = fourdst::config::Config::getInstance();
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

TEST_F(configTest, secondSingletonTest) {
    fourdst::config::Config& config = fourdst::config::Config::getInstance();
    EXPECT_EQ(config.get<int>("opac:lowTemp:numeric:maxIter", 10), 100);
}

TEST_F(configTest, isKeyInCacheTest) {
    fourdst::config::Config& config = fourdst::config::Config::getInstance();
    config.loadConfig(EXAMPLE_FILENAME);
    EXPECT_TRUE(configTestPrivateAccessor::callIsKeyInCache(config, "opac:lowTemp:numeric:maxIter"));
    EXPECT_FALSE(configTestPrivateAccessor::callIsKeyInCache(config, "opac:lowTemp:numeric:maxIter2"));
}

TEST_F(configTest, cacheSize) {
    fourdst::config::Config& config = fourdst::config::Config::getInstance();
    config.loadConfig(EXAMPLE_FILENAME);
    EXPECT_EQ(configTestPrivateAccessor::callCacheSize(config), 3);
    EXPECT_NE(configTestPrivateAccessor::callCacheSize(config), 4);
    config.get<std::string>("outputDir", "DEBUG");
    EXPECT_EQ(configTestPrivateAccessor::callCacheSize(config), 4);
}

TEST_F(configTest, unknownKeyTest) {
    fourdst::config::Config& config = fourdst::config::Config::getInstance();
    config.loadConfig(EXAMPLE_FILENAME);
    config.get<int>("opac:lowTemp:numeric:random", 10);
    EXPECT_FALSE(configTestPrivateAccessor::CheckIfKeyUnknown(config, "opac:lowTemp:numeric:maxIter"));
    EXPECT_TRUE(configTestPrivateAccessor::CheckIfKeyUnknown(config, "opac:lowTemp:numeric:random"));
}
