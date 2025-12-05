#pragma once
#include <filesystem>
#include "glaze/toml.hpp"
#include <string>
#include <map>
#include <optional>

#include "fourdst/config/registry.h"
#include "fourdst/config/exceptions/exceptions.h"

namespace fourdst::config::utils {
    inline std::string extract_error_key(const std::string& buffer, const glz::error_ctx& pe) {
        if (pe.location >= buffer.size()) return "unknown";

        size_t line_start = pe.location;
        while (line_start > 0 && buffer[line_start - 1] != '\n') {
            line_start--;
        }

        size_t line_end = pe.location;
        while (line_end < buffer.size() && buffer[line_end] != '\n' && buffer[line_end] != '\r') {
            line_end++;
        }

        std::string line = buffer.substr(line_start, line_end - line_start);

        const size_t separator_pos = line.find_first_of("=:");

        if (separator_pos != std::string::npos) {
            std::string key_part = line.substr(0, separator_pos);

            while (!key_part.empty() && std::isspace(key_part.back())) {
                key_part.pop_back();
            }

            size_t first_char = 0;
            while (first_char < key_part.size() && std::isspace(key_part[first_char])) {
                first_char++;
            }

            if (first_char < key_part.size()) {
                return key_part.substr(first_char);
            }
        }

        return line;
    }
}

namespace fourdst::config {
    enum class ConfigState {
        DEFAULT,
        LOADED_FROM_FILE
    };

    template <typename T>
    class Config {
    public:
        Config() {
            (void)m_registrar;
        }

        const T* operator->() const { return &m_content.main; }
        const T& main() const { return m_content.main; }

        void save(std::optional<std::string_view> path = std::nullopt) const {
            if (!path) {
                path = std::string(glz::name_v<T>) + ".toml";
            }
            auto err = glz::write_file_toml(m_content, path.value(), std::string{});
            if (err) {
                throw exceptions::ConfigSaveError(
                    std::format(
                        "Config::save: Failed to save config to {} with glaze error {} ",
                        std::string(path.value()),
                        glz::format_error(err.ec)
                        )
                );
            }
        }

        void load(const std::string_view path) {
            std::string buffer;
            const auto ec = glz::file_to_buffer(buffer, path);
            if (ec != glz::error_code::none) {
                throw exceptions::ConfigLoadError(
                    std::format(
                        "Config::load: Failed to load config from {} with glaze error {} ",
                        std::string(path),
                        glz::format_error(ec)
                    )
                );
            }
            auto err = glz::read<
                glz::opts{
                    .format = glz::TOML,
                    .error_on_unknown_keys = true
                }>(m_content, buffer);
            if (err) {
                throw exceptions::ConfigParseError(
                    std::format(
                        "Config::load: Failed to parse config from {} with glaze error {} (Key: {}) ",
                        std::string(path),
                        glz::format_error(err.ec),
                        utils::extract_error_key(buffer, err)
                    )
                );
            }
            m_state = ConfigState::LOADED_FROM_FILE;
        }

        void save_schema(const std::string_view dir) const {
            Registry::generate_named(dir, std::string(glz::name_v<T>));
        }

        ConfigState get_state() const { return m_state; }

        std::string describe_state() const {
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
        struct Content {
            T main;
        };
        Content m_content;
        ConfigState m_state = ConfigState::DEFAULT;


        struct Registrar {
            Registrar() {
                const auto name = std::string(glz::name_v<T>);
                Registry::register_schema<Content>(name);
            }
        };
        static inline Registrar m_registrar;
    };
}

template <typename T, typename CharT>
struct std::formatter<fourdst::config::Config<T>, CharT> {

    static constexpr auto parse(auto& ctx) { return ctx.begin(); }

    auto format(const fourdst::config::Config<T>& config, auto& ctx) const {
        const T& inner_value = config.main();
        struct Content {
            T main;
        };
        Content content{inner_value};
        std::string buffer;
        const glz::error_ctx ec = glz::write<glz::opts{.format=glz::TOML, .prettify = true}>(content, buffer);
        if (ec) {
            return std::format_to(ctx.out(), "Error serializing config");
        }

        return std::format_to(ctx.out(), "{}", buffer);
    }
};
