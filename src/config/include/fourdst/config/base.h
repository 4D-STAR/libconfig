#pragma once
#include <filesystem>
#include <string>
#include <map>
#include <optional>
#include <format>

#include "fourdst/config/exceptions/exceptions.h"

#include "rfl.hpp"
#include "rfl/toml.hpp"
#include "rfl/json.hpp"

namespace fourdst::config {
    enum class RootNameLoadPolicy {
        FROM_FILE,
        KEEP_CURRENT
    };

    enum class ConfigState {
        DEFAULT,
        LOADED_FROM_FILE
    };

    template <typename T>
    class Config {
    public:
        Config() = default;
        const T* operator->() const { return &m_content; }
        const T& main() const { return m_content; }

        void save(std::string_view path) const {
            T default_instance{};
            std::unordered_map<std::string, T> wrapper;
            wrapper[m_root_name] = m_content;
            const std::string toml_string = rfl::toml::write(wrapper);

            std::ofstream ofs{std::string(path)};
            if (!ofs.is_open()) {
                throw exceptions::ConfigSaveError(
                    std::format("Failed to open file for writing config: {}", path)
                );
            }

            ofs << toml_string;
            ofs.close();
        }

        void set_root_name(const std::string_view name) {
            m_root_name = name;
        }

        [[nodiscard]] std::string_view get_root_name() const {
            return m_root_name;
        }

        void set_root_name_load_policy(const RootNameLoadPolicy policy) {
            m_root_name_load_policy = policy;
        }

        RootNameLoadPolicy get_root_name_load_policy() const {
            return m_root_name_load_policy;
        }

        std::string describe_root_name_load_policy() const {
            switch (m_root_name_load_policy) {
                case RootNameLoadPolicy::FROM_FILE:
                    return "FROM_FILE";
                case RootNameLoadPolicy::KEEP_CURRENT:
                    return "KEEP_CURRENT";
                default:
                    return "UNKNOWN";
            }
        }

        void load(const std::string_view path) {
            if (m_state == ConfigState::LOADED_FROM_FILE) {
                throw exceptions::ConfigLoadError(
                    "Config has already been loaded from file. Reloading is not supported.");
            }

            if (!std::filesystem::exists(path)) {
                throw exceptions::ConfigLoadError(
                    std::format("Config file does not exist: {}", path));
            }

            using wrapper = std::unordered_map<std::string, T>;
            const rfl::Result<wrapper> result = rfl::toml::load<wrapper>(std::string(path));

            if (!result) {
                throw exceptions::ConfigParseError(
                    std::format("Failed to load config from file: {}", path));
            }


            std::string loaded_root_name = result.value().begin()->first;

            if (m_root_name_load_policy == RootNameLoadPolicy::KEEP_CURRENT && m_root_name != loaded_root_name) {
                throw exceptions::ConfigLoadError(
                    std::format(
                        "Root name mismatch when loading config from file. Current root name is '{}', but file root name is '{}'. If you want to use the root name from the file, set the root name load policy to FROM_FILE using set_root_name_load_policy().",
                        m_root_name,
                        loaded_root_name
                    )
                );
            }
            m_root_name = loaded_root_name;

            m_content = result.value().at(loaded_root_name);

            m_state = ConfigState::LOADED_FROM_FILE;
        }

        static void save_schema(const std::string& path) {
            using wrapper = std::unordered_map<std::string, T>;
            const std::string json_schema = rfl::json::to_schema<wrapper>(rfl::json::pretty);

            std::ofstream ofs{std::string(path)};
            if (!ofs.is_open()) {
                throw exceptions::SchemaSaveError(
                    std::format("Failed to open file for writing schema: {}", path)
                );
            }

            ofs << json_schema;
            ofs.close();
        }

        [[nodiscard]] ConfigState get_state() const { return m_state; }

        [[nodiscard]] std::string describe_state() const {
            switch (m_state) {
                case ConfigState::DEFAULT:
                    return "DEFAULT";
                case ConfigState::LOADED_FROM_FILE:
                    return "LOADED_FROM_FILE";
                default:
                    return "UNKNOWN";
            }
        }

    private:
        T m_content;
        std::string m_root_name = "main";
        ConfigState m_state = ConfigState::DEFAULT;
        RootNameLoadPolicy m_root_name_load_policy = RootNameLoadPolicy::KEEP_CURRENT;
    };
}

template <typename T, typename CharT>
struct std::formatter<fourdst::config::Config<T>, CharT> {

    static constexpr auto parse(auto& ctx) { return ctx.begin(); }

    auto format(const fourdst::config::Config<T>& config, auto& ctx) const {
        const T& inner_value = config.main();
        std::map<std::string, T> wrapper;
        wrapper[std::string(config.get_root_name())] = inner_value;
        std::string buffer;
        return buffer;
    }
};
