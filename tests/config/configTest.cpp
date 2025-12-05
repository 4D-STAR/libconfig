#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <sstream>
#include <algorithm>

#include "fourdst/config/config.h"
#include "test_schema.h"


std::string get_good_example_file() {
    const char* source_root = getenv("MESON_SOURCE_ROOT");
    if (source_root == nullptr) {
        throw std::runtime_error("MESON_SOURCE_ROOT environment variable is not set.");
    }
    return std::string(source_root) + "/tests/config/example_config_files/example.good.toml";
}


enum class BAD_FILES {
    UNKNOWN_KEY,
    INVALID_TYPE,
    INCORRECT_ARRAY_SIZE
};

std::string get_bad_example_file(BAD_FILES type) {
    const char* source_root = getenv("MESON_SOURCE_ROOT");
    if (source_root == nullptr) {
        throw std::runtime_error("MESON_SOURCE_ROOT environment variable is not set.");
    }
    switch (type) {
        case BAD_FILES::UNKNOWN_KEY:
            return std::string(source_root) + "/tests/config/example_config_files/example.unknownkey.toml";
        case BAD_FILES::INVALID_TYPE:
            return std::string(source_root) + "/tests/config/example_config_files/example.invalidtype.toml";
        case BAD_FILES::INCORRECT_ARRAY_SIZE:
            return std::string(source_root) + "/tests/config/example_config_files/example.incorrectarraysize.toml";
    }
    throw std::runtime_error("Invalid BAD_FILES type.");
}

// std::string EXAMPLE_FILENAME = std::string(getenv("MESON_SOURCE_ROOT")) + "/tests/config/example.toml";
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
    EXPECT_NO_THROW(fourdst::config::Config<TestConfigSchema>());
}

TEST_F(configTest, load_good_file) {
    using namespace fourdst::config;
    Config<TestConfigSchema> cfg;
    EXPECT_NO_THROW(cfg.load(get_good_example_file()));
}

TEST_F(configTest, load_unknown_key_file) {
    using namespace fourdst::config;
    Config<TestConfigSchema> cfg;
    EXPECT_THROW(cfg.load(get_bad_example_file(BAD_FILES::UNKNOWN_KEY)), exceptions::ConfigParseError);
}

TEST_F(configTest, load_invalid_type_file) {
    using namespace fourdst::config;
    Config<TestConfigSchema> cfg;
    EXPECT_THROW(cfg.load(get_bad_example_file(BAD_FILES::INVALID_TYPE)), exceptions::ConfigParseError);
}

TEST_F(configTest, load_incorrect_array_size_file) {
    using namespace fourdst::config;
    Config<TestConfigSchema> cfg;
    EXPECT_THROW(cfg.load(get_bad_example_file(BAD_FILES::INCORRECT_ARRAY_SIZE)), exceptions::ConfigParseError);
}

TEST_F(configTest, check_value) {
    using namespace fourdst::config;
    Config<TestConfigSchema> cfg;
    EXPECT_NO_THROW(cfg.load(get_good_example_file()));
    EXPECT_EQ(cfg->author, "Example Author");
}

TEST_F(configTest, nested_values) {
    using namespace fourdst::config;
    Config<TestConfigSchema> cfg;
    EXPECT_NO_THROW(cfg.load(get_good_example_file()));
    EXPECT_EQ(cfg->physics.convection, false);
}

TEST_F(configTest, override_default) {
    using namespace fourdst::config;
    Config<TestConfigSchema> cfg;
    EXPECT_NO_THROW(cfg.load(get_good_example_file()));
    EXPECT_EQ(cfg->simulation.time_step, 0.01);
}

TEST_F(configTest, array_values) {
    using namespace fourdst::config;
    Config<TestConfigSchema> cfg;
    EXPECT_NO_THROW(cfg.load(get_good_example_file()));
    constexpr std::array<int, 3> expected = {1, 0, 1};
    EXPECT_EQ(cfg->physics.flags, expected);
}

TEST_F(configTest, string_values) {
    using namespace fourdst::config;
    Config<TestConfigSchema> cfg;
    EXPECT_NO_THROW(cfg.load(get_good_example_file()));
    EXPECT_EQ(cfg->output.format, "csv");
}

TEST_F(configTest, save_default) {
    using namespace fourdst::config;
    const Config<TestConfigSchema> cfg;
    EXPECT_NO_THROW(cfg.save("TestConfigSchema.toml"));
}

TEST_F(configTest, save_schema) {
    using namespace fourdst::config;
    const Config<TestConfigSchema> cfg;
    EXPECT_NO_THROW(cfg.save_schema("./"));
}
