/**
 * @file cli.h
 * @brief Integration layer between libconfig and CLI applications.
 *
 * This file contains utilities for automatically mapping C++ configuration structures
 * to command-line arguments, primarily supporting the CLI11 library.
 */
#pragma once

#include <type_traits>
#include "fourdst/config/base.h"
#include "rfl.hpp"

namespace fourdst::config {
    template <typename T>
    struct InspectType;

    /**
     * @brief Concept that defines the requirements for a CLI application class.
     *
     * This concept ensures that the CLI application class `T` has an `add_option` member function
     * compatible with the signature expected by `register_as_cli`. It is satisfied by `CLI::App` from CLI11.
     *
     * @tparam T The type to check against the concept.
     */
    template <typename T>
    concept IsCLIApp = requires(T app, std::string name, std::string description)
    {
        {app.add_option(name, std::declval<int&>(), description)};
    };

    /**
     * @brief Type trait to determine if a type is a Config wrapper.
     *
     * This is the base case which inherits from `std::false_type`.
     *
     * @tparam T The type to inspect.
     */
    template <typename T>
    struct is_config_wrapper : std::false_type {};

    /**
     * @brief Specialization of `is_config_wrapper` for `Config<T>`.
     *
     * This specialization inherits from `std::true_type`, identifying instances of the `Config` class.
     *
     * @tparam T The underlying configuration struct type.
     */
    template <typename T>
    struct is_config_wrapper<Config<T>> : std::true_type {};

    /**
     * @brief Registers configuration structure fields as CLI options.
     *
     * This function iterates over the members of the provided configuration object using reflection
     * and registers each member as a command-line option in the provided CLI application.
     *
     * If the configuration object contains nested structures, field names are flattened using dot notation
     * (e.g., `parent.child.field`).
     *
     * If `T` is a `Config<U>` wrapper, it automatically unwraps the inner value and adds a footer note
     * to the CLI application's help message indicating that options were auto-generated.
     *
     * @tparam T The type of the configuration object. Can be a raw struct or a `Config<Struct>` wrapper.
     * @tparam CliApp The type of the CLI application object. Must satisfy the `IsCLIApp` concept (e.g., `CLI::App`).
     * @param config The configuration object to register.
     * @param app The CLI application instance to add options to.
     * @param prefix Optional prefix for option names. Used internally for recursion; usually omitted by the caller.
     *
     * @par Examples
     * Basic usage with CLI11:
     * @code
     * #include "CLI/CLI.hpp"
     * #include "fourdst/config/config.h"
     *
     * struct MyOptions {
     *     int verbosity = 0;
     *     std::string input_file = "data.txt";
     * };
     *
     * int main(int argc, char** argv) {
     *     fourdst::config::Config<MyOptions> cfg;
     *     CLI::App app{"My Application"};
     *
     *     // Automatically adds flags: --verbosity, --input_file
     *     fourdst::config::register_as_cli(cfg, app);
     *
     *     CLI11_PARSE(app, argc, argv);
     *
     *     // cfg is now populated with values from CLI
     *     return 0;
     * }
     * @endcode
     *
     * Nested structures:
     * @code
     * struct Server {
     *     int port = 8080;
     *     std::string host = "localhost";
     * };
     *
     * struct AppConfig {
     *     Server server;
     *     bool dry_run = false;
     * };
     *
     * // In main...
     * fourdst::config::Config<AppConfig> cfg;
     * // Registers: --server.port, --server.host, --dry_run
     * fourdst::config::register_as_cli(cfg, app);
     * @endcode
     */
    template <typename T, typename CliApp>
    void register_as_cli(T& config, CliApp& app, const std::string& prefix="") {
        if constexpr (is_config_wrapper<T>::value) {
            app.footer("\nNOTE:\n"
                "Configuration options were automatically generated from the config schema.\n"
                "Use the --help flag to see all available options."
            );
            register_as_cli(*config, app, prefix);
        } else {
            auto view = rfl::to_view(config);
            view.apply([&](auto f) {
                auto& value = f.value();
                using ValueType = std::remove_pointer_t<std::decay_t<decltype(value)>>;

                const auto name = std::string(f.name());
                std::string field_name = prefix.empty() ? name : prefix + "." + name;

                if constexpr (IsConfigSchema<ValueType>) {
                    register_as_cli(*value, app, field_name);
                }
                else {
                    app.add_option(
                        "--" + field_name,
                        *value,
                        "Configuration option for " + field_name
                    );
                }

            });
        }
    }
}