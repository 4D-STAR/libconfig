#include "fourdst/config/config.h"
#include "glaze/glaze.hpp"
#include <string>
#include <vector>

using namespace fourdst::config;

struct sub {
    double x;
    double y;
};

struct BoundaryConditions {
    double pressure = 1e6;
    sub sub;
};

struct ExampleConfig  {
    double parameterA = 1.0;
    int parameterB = 1.0;
    std::string parameterC = "default_value";
    std::vector<double> parameterD = {0.1, 0.2, 0.3};
    BoundaryConditions boundaryConditions;
};

struct Person {
    int age;
    std::string name;
};

struct AppConfig {
    double x;
    double y;
    Person person;
};

int main() {
    const Config<ExampleConfig> cfg;
    cfg.save();
    cfg.save_schema(".");

    Config<AppConfig> loaded;
    loaded.save_schema(".");
    loaded.load("config_example.toml");
    std::println("{}", loaded);
}