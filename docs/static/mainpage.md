![img](logo.png)
# libconfig

libconfig is the unified configuration module for SERiF and related projects

This has been broken out of the main serif project to allow for more modularity

## Building
In order to build libconfig you need `meson>=1.5.0`. This can be installed with `pip`

```bash
pip install "meson>=1.5.0"
```

Then from the root libconfig directory it is as simple as

```bash
meson setup build --buildtype=release
meson compile -C build
meson test -C build
```

this will auto generate a pkg-config file for you so that linking other libraries to libconfig is easy.

## Usage
libconfig makes use of [reflect-cpp](https://github.com/getml/reflect-cpp) to provide compile time reflection
and serialization/deserialization of configuration structs. This allows for config options to be defined in code
and strongly typed.

### Basic Usage

```c++
#include "fourdst/config/config.h"
#include <string>
#include <print>

struct MyPhysicsOptions {
    int gravity = 10;
    float friction = 0.5f;
    bool enable_wind = false;
};

struct MyControlOptions {
    double time_step = 0.01;
    double max_time = 100.0;
};

struct MySimulationConfig {
    std::string name = "my_simulation";
    MyPhysicsOptions physics;
    MyControlOptions control;
};

int main() {
    fourdst::config::Config<MySimulationConfig> cfg;
    
    // You can save the default config to a file
    cfg.save("default_config.toml");
    
    // You can save the json schema for the config
    // This allows editors like VS Code to provide autocompletion
    fourdst::config::Config<MySimulationConfig>::save_schema("config.schema.json");
    
    // You can load a config from a file
    try {
        cfg.load("my_config.toml");
    } catch (const fourdst::config::exceptions::ConfigError& e) {
        std::println("Error loading config: {}", e.what());
    }
    
    // You can access the config values
    std::println("My Simulation Name: {}, My Simulation Gravity: {}", cfg->name, cfg->physics.gravity);
}
```

### CLI Integration
libconfig integrates with [CLI11](https://github.com/CLIUtils/CLI11) to automatically expose configuration fields as command-line arguments.

```c++
#include "CLI/CLI.hpp"
#include "fourdst/config/config.h"

int main(int argc, char** argv) {
    fourdst::config::Config<MySimulationConfig> cfg;
    CLI::App app("My Application");

    // Automatically registers:
    // --name
    // --physics.gravity
    // --physics.friction
    // --physics.enable_wind
    // ... and so on
    fourdst::config::register_as_cli(cfg, app);

    CLI11_PARSE(app, argc, argv);
    
    // cfg is now populated with values from CLI arguments
    return 0;
}
```

## Example output TOML
```toml
[main]
name = "my_simulation"
[main.physics]
gravity = 10
friction = 0.5
enable_wind = false
[main.control]
time_step = 0.01
max_time = 100.0
```

