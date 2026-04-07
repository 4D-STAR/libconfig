#pragma once

#include "fourdst/config/ansi.h"

#include <rfl.hpp>
#include <toml++/toml.h>
#include <string>
#include <string_view>
#include <vector>
#include <iostream>
#include <format>
#include <type_traits>
#include <optional>
#include <map>
#include <unordered_map>
#include <sstream>

namespace fourdst::config::validate {

    template <typename T> struct is_optional_impl : std::false_type {};
    template <typename T> struct is_optional_impl<std::optional<T>> : std::true_type {};
    template <typename Type> constexpr bool is_optional_v = is_optional_impl<std::remove_cvref_t<Type>>::value;

    template <typename T> struct is_vector_impl : std::false_type {};
    template <typename T, typename A> struct is_vector_impl<std::vector<T, A>> : std::true_type {};
    template <typename Type> constexpr bool is_vector_v = is_vector_impl<std::remove_cvref_t<Type>>::value;

    template <typename T> struct is_map_impl : std::false_type {};
    template <typename K, typename V, typename C, typename A> struct is_map_impl<std::map<K, V, C, A>> : std::true_type {};
    template <typename K, typename V, typename H, typename E, typename A> struct is_map_impl<std::unordered_map<K, V, H, E, A>> : std::true_type {};
    template <typename Type> constexpr bool is_map_v = is_map_impl<std::remove_cvref_t<Type>>::value;

    template <typename Type>
    constexpr bool is_string_like_v = std::is_same_v<std::remove_cvref_t<Type>, std::string> ||
                                      std::is_same_v<std::remove_cvref_t<Type>, std::string_view>;

    template <typename Type>
    constexpr bool is_reflectable_struct_v = std::is_class_v<std::remove_cvref_t<Type>> &&
                                           !is_string_like_v<Type> &&
                                           !is_vector_v<Type> &&
                                           !is_optional_v<Type> &&
                                           !is_map_v<Type>;

    template <typename StructType>
    struct ConfigValidator {
        using NT = rfl::named_tuple_t<StructType>;

        static void check(const toml::table* tbl, const std::string& current_path, std::vector<std::string>& missing) {
            if (!tbl) return;
            check_tuple<NT>(tbl, current_path, missing);
        }
    private:
        template <typename Tuple>
        struct TupleChecker;

        template <typename... Fields>
        struct TupleChecker<rfl::NamedTuple<Fields...>> {
            static void check(const toml::table* tbl, const std::string& path, std::vector<std::string>& missing) {
                (check_field<Fields>(tbl, path, missing), ...);
            }
        };

        template <typename TupleType>
        static void check_tuple(const toml::table* tbl, const std::string& path, std::vector<std::string>& missing) {
            TupleChecker<TupleType>::check(tbl, path, missing);
        }

        template <typename Field>
        static void check_field(const toml::table* tbl, const std::string& path, std::vector<std::string>& missing) {
            std::string_view name_sv = Field::name();
            std::string name(name_sv);

            using RawType = typename Field::Type;
            using Type = std::remove_cvref_t<RawType>;

            std::string full_path = path.empty() ? name : path + "." + name;
            const toml::node* node = tbl->get(name_sv);

            if (!node) {
                if constexpr (!is_optional_v<Type>) {
                    missing.push_back(full_path);
                }
            } else {
                if constexpr (is_reflectable_struct_v<Type>) {
                    if (node->is_table()) {
                        ConfigValidator<Type>::check(node->as_table(), full_path, missing);
                    }
                } else if constexpr (is_vector_v<Type>) {
                    using ElementType = std::remove_cvref_t<typename Type::value_type>;
                    if constexpr (is_reflectable_struct_v<ElementType>) {
                        if (node->is_array()) {
                            const auto& arr = *node->as_array();
                            for (size_t i = 0; i < arr.size(); ++i) {
                                if (arr.get(i)->is_table()) {
                                    ConfigValidator<ElementType>::check(
                                        arr.get(i)->as_table(),
                                        std::format("{}[{}]", full_path, i),
                                        missing
                                    );
                                }
                            }
                        }
                    }
                } else if constexpr (is_optional_v<Type>) {
                    using InnerType = std::remove_cvref_t<typename Type::value_type>;
                    if constexpr (is_reflectable_struct_v<InnerType>) {
                        if (node->is_table()) {
                            ConfigValidator<InnerType>::check(node->as_table(), full_path, missing);
                        }
                    }
                }
            }
        }
    };

    struct MissingFieldTree {
        std::map<std::string, MissingFieldTree> children;
        bool is_missing = false;
    };

    inline void print_missing_field_tree(const MissingFieldTree& tree, std::string indent, bool is_last, const std::string& name, std::string& output) {
        if (!name.empty()) {
            std::string display_name = tree.is_missing ? std::format("{}{}{}", utils::RED.get(), name, utils::RESET.get()) : name;
            output += std::format("{}{} {}\n", indent, is_last ? "└──" : "├──", display_name);
            indent += is_last ? "    " : "│   ";
        }

        size_t count = 0;
        for (auto const& [child_name, child_tree] : tree.children) {
            print_missing_field_tree(child_tree, indent, count == tree.children.size() - 1, child_name, output);
            ++count;
        }
    }

    inline std::string report_all_missing_fields(const std::vector<std::string>& missing) {
        if (missing.empty()) return "";

        MissingFieldTree root;
        for (const auto& path : missing) {
            std::stringstream ss(path);
            std::string part;
            MissingFieldTree* current = &root;
            while (std::getline(ss, part, '.')) {
                current = &current->children[part];
            }
            current->is_missing = true;
        }

        std::string output = "\nConfiguration Missing Field Path(s):\n";
        size_t count = 0;
        for (auto const& [name, child_tree] : root.children) {
            print_missing_field_tree(child_tree, "", count == root.children.size() - 1, name, output);
            ++count;
        }
        return output;
    }
}