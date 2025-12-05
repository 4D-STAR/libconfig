#pragma once
#include <vector>
#include <functional>
#include <filesystem>
#include <string>
#include <glaze/glaze.hpp>
#include <exception>
#include "fourdst/config/exceptions/exceptions.h"

namespace fourdst::config {

    struct Registry {
        using SchemaWriter = std::function<void(std::filesystem::path)>;

        static std::unordered_map<std::string, SchemaWriter>& get_writers() {
            static std::unordered_map<std::string, SchemaWriter> writers;
            return writers;
        }

        template <typename T>
        static void register_schema(const std::string& name) {
            auto& writers = get_writers();

            writers.insert({name, [name](const std::filesystem::path &dir) {

                auto schema_r = glz::write_json_schema<T>();
                if (!schema_r.has_value()) {
                    throw std::runtime_error("Failed to generate schema for " + name);
                }
                 std::string schema = schema_r.value();

                const auto path = dir / (name + ".schema.json");
                const auto err = glz::buffer_to_file(schema, path.string());

                if (err != glz::error_code::none) {
                    throw exceptions::SchemaGenerationError("Failed to write schema for " + name + " to " + path.string());
                }
            }});
        }

        static void generate_all(const std::filesystem::path& dir) {
            std::filesystem::create_directories(dir);
            for (const auto &writer: get_writers() | std::views::values) {
                writer(dir);
            }
        }

        static void generate_named(const std::filesystem::path& dir, const std::string& name) {
            const auto& writers = get_writers();
            const auto it = writers.find(name);
            if (it == writers.end()) {
                throw exceptions::SchemaNameError("No schema registered with name: " + name);
            }
            it->second(dir);
        }
    };

}