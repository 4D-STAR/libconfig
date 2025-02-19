#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

#include "yaml-cpp/yaml.h"

#include "config.h"

Config::Config() {}

Config::~Config() {}

Config& Config::getInstance() {
    static Config instance;
    return instance;
}

bool Config::loadConfig(const std::string& configFile) {
    configFilePath = configFile;
    try {
        yamlRoot = YAML::LoadFile(configFile);
    } catch (YAML::BadFile& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
    return true;
}

