![img](assets/logo/logo.png)
# libconfig

libconfig is the unified configuration module for SERiF and related projects

This has been broken out of the main serif project to allow for more modularity

## Building
In order to build libconstants you need `meson>=1.5.0`. This can be installed with `pip`

```bash
pip install "meson>=1.5.0"
```

Then from the root libconstants directory it is as simple as

```bash
meson setup build --buildtype=release
meson compile -C build
meson test -C build
```

this will auto generate a pkg-config file for you so that linking other libraries to libconfig is easy.

## Usage
libconfig makes use of [glaze](https://github.com/stephenberry/glaze?tab=readme-ov-file) to provide compile time reflection
and serialization/deserialization of configuration structs. This allows for config options to be defined in code
and strongly typed.

```c++
#include "fourdst/config/config.h"
#include <string>

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
    
    // If you do not provide a name, glaze will use the struct's name
    cfg.save(); // saves to MySimulationConfig.toml
    
    // You can save the json schema for the config
    cfg.save_schema("."); // Here you must provide the directory to save to rather than a full file name
    
    // You can load a config from a file
    cfg.load("my_config.toml");
    
    // You can accesss the config values
    std::println("My Simulation Name: {}, My Simulation Gravity: {}", cfg->name, cfg->physics.gravity);
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

