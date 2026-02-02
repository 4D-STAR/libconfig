/**
 * @file exceptions.h
 * @brief Exception classes for the configuration library.
 *
 * This file defines the hierarchy of exceptions thrown by the `fourdst::config` library.
 * All exceptions inherit from `ConfigError`, which in turn inherits from `std::exception`.
 */
#pragma once

#include <stdexcept>
#include <string>

namespace fourdst::config::exceptions {
    /**
     * @brief Base exception class for all configuration-related errors.
     *
     * Provides a standard way to catch any error originating from the configuration library.
     * Stores a string message describing the error.
     */
    class ConfigError : public std::exception {
    public:
        /**
         * @brief Constructs a ConfigError with a specific message.
         * @param what The error message.
         */
        explicit ConfigError(const std::string & what): m_msg(what) {}

        /**
         * @brief Returns the error message.
         * @return C-style string containing the error message.
         */
        [[nodiscard]] const char* what() const noexcept override {
            return m_msg.c_str();
        }
    private:
        std::string m_msg;
    };

    /**
     * @brief Thrown when saving the configuration to a file fails.
     *
     * This usually indicates file I/O errors (e.g., permission denied, disk full).
     */
    class ConfigSaveError final : public ConfigError {
        using ConfigError::ConfigError;
    };

    /**
     * @brief Thrown when loading the configuration from a file fails.
     *
     * This can occur if the file does not exist, or if there are policy violations
     * (e.g., root name mismatch when `KEEP_CURRENT` is set).
     */
    class ConfigLoadError final : public ConfigError {
        using ConfigError::ConfigError;
    };

    /**
     * @brief Thrown when parsing the configuration file fails.
     *
     * This indicates that the file exists but contains invalid TOML syntax or
     * data that does not match the expected schema type.
     */
    class ConfigParseError final : public ConfigError {
        using ConfigError::ConfigError;
    };

    /**
     * @brief Thrown when generating or saving the JSON schema fails.
     *
     * This typically indicates file I/O errors when writing the schema file.
     */
    class SchemaSaveError final : public ConfigError {
        using ConfigError::ConfigError;
    };


}