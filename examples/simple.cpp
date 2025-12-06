#include "rfl.hpp"
#include "rfl/toml.hpp"

#include <string>
#include <vector>
#include <print>

#include "fourdst/config/config.h"

struct Other {
    std::array<int, 3> a{};
    std::vector<std::string> b{};
    double c;
};

struct Location {
    double x;
    double y;
    Other other{};
};

struct Person {
    std::string name{};
    std::string address{};
    double height{};
    Location location{};
};

int main() {
    const fourdst::config::Config<Person> personConfig;
    personConfig.save("Example.toml");

    fourdst::config::Config<Person> personConfigToLoad;
    personConfigToLoad.load("Example.toml");

    personConfigToLoad.save_schema("ExampleSchema.schema.json");
}