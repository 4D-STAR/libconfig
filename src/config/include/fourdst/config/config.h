/* ***********************************************************************
//
//   Copyright (C) 2025 -- The 4D-STAR Collaboration
//   File Author: Emily Boudreaux
//   Last Modified: March 26, 2025
//
//   4DSSE is free software; you can use it and/or modify
//   it under the terms and restrictions the GNU General Library Public
//   License version 3 (GPLv3) as published by the Free Software Foundation.
//
//   4DSSE is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU Library General Public License for more details.
//
//   You should have received a copy of the GNU Library General Public License
//   along with this software; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// *********************************************************************** */
/**
 * @file config.h
 * @brief Main entry point for the fourdst::config library.
 *
 * This header includes all necessary components of the configuration library,
 * providing a unified interface for defining, loading, saving, and integrating
 * configuration structures.
 *
 * @section features Features
 * - **Type-safe Configuration**: Define configs using standard C++ structs.
 * - **Serialization**: Built-in support for TOML loading and saving via `reflect-cpp`.
 * - **Schema Generation**: Generate JSON schemas for editor autocompletion (VS Code, etc.).
 * - **CLI Integration**: Seamlessly expose config fields as command-line arguments (supports CLI11).
 * - **Error Handling**: Comprehensive exception hierarchy for parsing and I/O errors.
 *
 * @par Examples
 *
 * **1. Basic Definition and I/O**
 * @code
 * #include "fourdst/config/config.h"
 *
 * struct Physics {
 *     double gravity = 9.81;
 *     bool enable_drag = true;
 * };
 *
 * struct AppConfig {
 *     std::string name = "My Simulation";
 *     int max_steps = 1000;
 *     Physics physics;
 * };
 *
 * int main() {
 *     fourdst::config::Config<AppConfig> cfg;
 *
 *     // Access defaults
 *     if (cfg->physics.enable_drag) { ... }
 *
 *     // Save to file
 *     cfg.save("config.toml");
 *
 *     // Load from file
 *     cfg.load("config.toml");
 * }
 * @endcode
 *
 * **2. CLI Integration (CLI11)**
 * @code
 * #include "CLI/CLI.hpp"
 * #include "fourdst/config/config.h"
 *
 * int main(int argc, char** argv) {
 *     CLI::App app("Simulation App");
 *     fourdst::config::Config<AppConfig> cfg;
 *
 *     // Automatically registers flags like --name, --max_steps, --physics.gravity
 *     fourdst::config::register_as_cli(cfg, app);
 *
 *     CLI11_PARSE(app, argc, argv);
 *
 *     std::cout << "Starting simulation: " << cfg->name << "\n";
 * }
 * @endcode
 *
 * **3. Error Handling**
 * @code
 * try {
 *     cfg.load("missing_file.toml");
 * } catch (const fourdst::config::exceptions::ConfigLoadError& e) {
 *     std::cerr << "Could not load config: " << e.what() << "\n";
 *     // Falls back to default values
 * } catch (const fourdst::config::exceptions::ConfigParseError& e) {
 *     std::cerr << "Invalid config file format: " << e.what() << "\n";
 *     return 1;
 * }
 * @endcode
 */
#pragma once

#include "fourdst/config/base.h"
#include "fourdst/config/exceptions/exceptions.h"
#include "fourdst/config/cli.h"

