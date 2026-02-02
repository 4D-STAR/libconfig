#include "CLI/CLI.hpp"
#include "fourdst/config/config.h"

#include <string>
#include <print>
#include <optional>

struct SubConfig{
    int a = 0;
    double b = 1.0;
    std::string c = "default";
};

struct MainConfig {
    std::string name = "example";
    SubConfig subconfig;
    double value = 3.14;
    std::optional<std::string> help = std::nullopt;
};


int main(const int argc, char** argv) {
    fourdst::config::Config<MainConfig> config;
    CLI::App app("Example CLI Application with Config");

    fourdst::config::register_as_cli(config, app, "cfg");

    app.parse(argc, argv);

    std::println("Configuration: \n{}", config);

    return 0;
}