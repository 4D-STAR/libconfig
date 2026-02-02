/**
 * @file base.h
 * @brief Core configuration management classes and concepts.
 *
 * This file defines the `Config` template class which serves as the primary interface
 * for managing typed configuration structures. It handles serialization (save), deserialization (load),
 * and schema generation using the `reflect-cpp` library.
 */
#pragma once
#include <filesystem>
#include <string>
#include <map>
#include <format>
#include <vector>
#include <string_view>
#include <type_traits>

#include "fourdst/config/exceptions/exceptions.h"

#include "rfl.hpp"
#include "rfl/toml.hpp"
#include "rfl/json.hpp"


namespace fourdst::config {

    /**
     * @brief Concept ensuring a type is suitable for configuration schema.
     *
     * A valid configuration schema must be:
     * - A class or struct (`std::is_class_v`)
     * - An aggregate type (`std::is_aggregate_v`), i.e., strict POD-like structure without user-declared constructors.
     * - Not a `std::string`.
     *
     * @tparam T The type to check.
     */
    template <typename T>
    concept IsConfigSchema =
        std::is_class_v<std::decay_t<T>> &&          // Must be a class/struct
        std::is_aggregate_v<std::decay_t<T>> &&      // Must be an aggregate (POD-like)
        !std::same_as<std::decay_t<T>, std::string>; // Explicitly exclude strings

    /**
     * @brief Policies for handling the root name during configuration loading.
     */
    enum class RootNameLoadPolicy {
        /**
         * @brief Updates the internal root name to match what is found in the file.
         */
        FROM_FILE,
        /**
         * @brief Enforces the current internal root name; loading fails if the file's root name differs.
         */
        KEEP_CURRENT
    };

    /**
     * @brief Represents the current state of a Config object.
     */
    enum class ConfigState {
        /**
         * @brief Configuration contains default values and has not been loaded from a file.
         */
        DEFAULT,
        /**
         * @brief Configuration has been successfully populated from a file.
         */
        LOADED_FROM_FILE
    };



    /**
     * @brief Wrapper class for managing strongly-typed configuration structures.
     *
     * The `Config` class wraps a user-defined aggregate struct `T` and provides methods
     * to save/load it to/from TOML files, as well as generate JSON schemas.
     *
     * It uses `reflect-cpp` to automatically inspect the fields of `T`.
     *
     * @tparam T The configuration structure type. Must satisfy `IsConfigSchema`.
     *
     * @par Examples
     * Defining a config struct and using `Config`:
     * @code
     * #include "fourdst/config/config.h"
     *
     * struct MySettings {
     *     int threads = 4;
     *     double timeout = 30.5;
     * };
     *
     * int main() {
     *     fourdst::config::Config<MySettings> cfg;
     *
     *     // Access values (default)
     *     std::cout << "Threads: " << cfg->threads << "\n";
     *
     *     // Save default config
     *     cfg.save("settings.toml");
     *
     *     // Load from file
     *     cfg.load("settings.toml");
     *
     *     // Save JSON Schema for editors
     *     cfg.save_schema("schema.json");
     *
     *     return 0;
     * }
     * @endcode
     */
    template <IsConfigSchema T>
    class Config {
    public:
        /**
         * @brief Default constructor. Initializes the inner content with default values.
         */
        Config() = default;

        /**
         * @brief Access member of the underlying configuration struct.
         * @return Pointer to the constant configuration content.
         */
        const T* operator->() const { return &m_content; }

        /**
         * @brief Get a mutable pointer to the configuration content.
         * @return Pointer to the mutable configuration content.
         */
        T* write() const { return &m_content; }

        /**
         * @brief Dereference operator to access the underlying configuration struct.
         * @return Reference to the mutable configuration content.
         */
        T& operator*() { return m_content; }

        /**
         * @brief Dereference operator to access the underlying configuration struct.
         * @return Reference to the constant configuration content.
         */
        const T& operator*() const { return m_content; }

        /**
         * @brief Explicit accessor for the main configuration content.
         * @return Reference to the constant configuration content.
         */
        const T& main() const { return m_content; }

        /**
         * @brief Saves the current configuration to a TOML file.
         *
         * Wraps the configuration content under the current root name (default "main")
         * and writes it to the specified path.
         *
         * @param path The file path to write to.
         * @throws exceptions::ConfigSaveError If the file cannot be opened.
         *
         * @par Examples
         * @code
         * cfg.save("config.toml");
         * @endcode
         */
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

        /**
         * @brief Sets the root name/key used in the TOML file.
         *
         * The default root name is "main". This name appears as the top-level table in the TOML file (e.g., `[main]`).
         *
         * @param name The new root name.
         */
        void set_root_name(const std::string_view name) {
            m_root_name = name;
        }

        /**
         * @brief Gets the current root name.
         * @return The root name string view.
         */
        [[nodiscard]] std::string_view get_root_name() const {
            return m_root_name;
        }

        /**
         * @brief Sets the policy for handling root name mismatches during load.
         * @param policy The policy (FROM_FILE or KEEP_CURRENT).
         */
        void set_root_name_load_policy(const RootNameLoadPolicy policy) {
            m_root_name_load_policy = policy;
        }

        /**
         * @brief Gets the current root name load policy.
         * @return The current policy.
         */
        [[nodiscard]] RootNameLoadPolicy get_root_name_load_policy() const {
            return m_root_name_load_policy;
        }

        /**
         * @brief Returns a string description of the current root name load policy.
         * @return "FROM_FILE", "KEEP_CURRENT", or "UNKNOWN".
         */
        [[nodiscard]] std::string describe_root_name_load_policy() const {
            switch (m_root_name_load_policy) {
                case RootNameLoadPolicy::FROM_FILE:
                    return "FROM_FILE";
                case RootNameLoadPolicy::KEEP_CURRENT:
                    return "KEEP_CURRENT";
                default:
                    return "UNKNOWN";
            }
        }

        /**
         * @brief Loads configuration from a TOML file.
         *
         * Reads the file, parses it, and updates the internal configuration state.
         *
         * @param path The file path to read from.
         * @throws exceptions::ConfigLoadError If the config is already loaded, file doesn't exist, or root name mismatch (under KEEP_CURRENT policy).
         * @throws exceptions::ConfigParseError If the file content is invalid TOML or doesn't match the schema.
         *
         * @par Examples
         * @code
         * try {
         *     cfg.load("config.toml");
         * } catch (const fourdst::config::exceptions::ConfigError& e) {
         *     std::cerr << "Error loading config: " << e.what() << std::endl;
         * }
         * @endcode
         */
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

        /**
         * @brief Generates and saves a JSON schema for the configuration structure.
         *
         * Useful for enabling autocompletion and validation in editors (e.g., VS Code).
         *
         * @param path The path to save the schema file to.
         * @throws exceptions::SchemaSaveError If the file cannot be opened.
         *
         * @par Examples
         * @code
         * Config<MyConfig>::save_schema("MyConfig.schema.json");
         * @endcode
         */
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

        /**
         * @brief Gets the current state of the configuration object.
         * @return The current state (DEFAULT or LOADED_FROM_FILE).
         */
        [[nodiscard]] ConfigState get_state() const { return m_state; }

        /**
         * @brief Returns a string description of the current configuration state.
         * @return "DEFAULT", "LOADED_FROM_FILE", or "UNKNOWN".
         */
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

/**
 * @brief Formatter specialization for Config<T> to allow easy printing.
 *
 * This allows a Config object to be directly formatted/printed (e.g., via std::print or std::format).
 * It outputs the configuration in its TOML representation.
 *
 * @par Example
 * @code
 * std::println("Current Setup:\n{}", config);
 * @endcode
 */
template <typename T, typename CharT>
struct std::formatter<fourdst::config::Config<T>, CharT> {

    static constexpr auto parse(auto& ctx) { return ctx.begin(); }

    auto format(const fourdst::config::Config<T>& config, auto& ctx) const {
        // Create a wrapper map to preserve the root name in the output
        std::map<std::string, T> wrapper;
        wrapper[std::string(config.get_root_name())] = config.main();

        // Serialize to TOML using reflect-cpp
        const std::string toml_string = rfl::toml::write(wrapper);

        // Write to the formatter output
        return std::format_to(ctx.out(), "{}", toml_string);
    }
};
