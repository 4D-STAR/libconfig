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
#include <optional>

struct MyPhysicsOptions {
    int gravity = 10;
    float friction = 0.5f;
    std::optional<float> dampening = 0.1f;
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
        // You can also pass an optional bool as a second argument to turn on verbose error 
        // reporting. This will display a tree of missing or invalid fields. Note that due to limitations
        // in C++'s ability to detect default iniailized values vs initializer list values 
        // missing fields which you set an initializer list for are still considered missing. 
        // **ONLY fields marked with std::optional are exempt from this rule.**
    } catch (const fourdst::config::exceptions::ConfigError& e) {
        std::println("Error loading config: {}", e.what());
    }
    
    // You can access the config values
    std::println("My Simulation Name: {}, My Simulation Gravity: {}", cfg->name, cfg->physics.gravity);
    
    // libconfig intentioanlly discourages direct modification of config values. However, if you need 
    // to modify values after loading them you can use the mutate function. This takes a lambda
    // which recives a mutable reference to the underlying config struct.
    
    cfg.mutate([](MySimulationConfig& config) {
        config->physics.enable_wind = true;
    });
    
    // Making these mutations will put the config into a "MODIFIED" state and will cache the unmodified values. 
    // You can reset the state and revert to the unmodified values with the reset function.
    
    cfg.reset();
    
    // The current state of the config can be checked with the get_state() function or the describe_state() function.
    // get_state returns an enum value while describe_state returns a human readable string.
    
    std::println("Config State: {}", cfg.describe_state());
    fourdst::config::ConfigState state = cfg.get_state();
    
    // Possible states are DEFAULT, LOADED_FROM_FILE, and MODIFIED
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

