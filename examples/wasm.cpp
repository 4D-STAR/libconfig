#include "fourdst/config/config.h"
#include <string>
#include <vector>
#include <iostream>

struct AppConfig {
    double x;
    double y;
    std::vector<std::string> men;
};

int main(int argc, char* argv[]) {
    fourdst::config::Config<AppConfig> config;
    config.load("/input.toml");

    auto x = config->x;
    const auto& men = config->men;

    for (const auto& name : men) {
        std::cout << "men are " << name << std::endl;
    }

}
