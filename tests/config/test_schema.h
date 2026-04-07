#pragma once
#include <array>
#include <string>

struct PhysicsConfigOptions {
    bool diffusion;
    std::optional<bool> convection;
    std::optional<bool> radiation;
    std::array<int, 3> flags;
};

struct SimulationConfigOptions {
    double time_step = 1;
    double total_time = 10;
    int output_frequency = 1;
};

struct OutputConfigOptions {
    std::string directory = "./output";
    std::string format = "hdf5";
    std::optional<bool> save_plots = false;
};

struct TestConfigSchema {
    std::string description;
    std::string author;

    PhysicsConfigOptions physics;
    SimulationConfigOptions simulation;
    OutputConfigOptions output;
};
