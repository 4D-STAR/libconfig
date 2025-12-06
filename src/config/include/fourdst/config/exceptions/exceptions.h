#pragma once

#include <stdexcept>
#include <string>

namespace fourdst::config::exceptions {
    class ConfigError : public std::exception {
    public:
        explicit ConfigError(const std::string & what): m_msg(what) {}

        [[nodiscard]] const char* what() const noexcept override {
            return m_msg.c_str();
        }
    private:
        std::string m_msg;
    };

    class ConfigSaveError final : public ConfigError {
        using ConfigError::ConfigError;
    };

    class ConfigLoadError final : public ConfigError {
        using ConfigError::ConfigError;
    };

    class ConfigParseError final : public ConfigError {
        using ConfigError::ConfigError;
    };

    class SchemaSaveError final : public ConfigError {
        using ConfigError::ConfigError;
    };


}