/* ***********************************************************************
//
//   Copyright (C) 2025 -- The 4D-STAR Collaboration
//   File Author: Emily Boudreaux
//   Last Modified: March 20, 2025
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
    m_loaded = true;
    return true;
}

bool Config::isKeyInCache(const std::string &key) {
    return configMap.find(key) != configMap.end();
}

void Config::addToCache(const std::string &key, const YAML::Node &node) {
    configMap[key] = node;
}

void Config::registerUnknownKey(const std::string &key) {
    unknownKeys.push_back(key);
}

bool Config::has(const std::string &key) {
    if (!m_loaded) {
        throw std::runtime_error("Error! Config file not loaded");
    }
    if (isKeyInCache(key)) { return true; }

    YAML::Node node = YAML::Clone(yamlRoot);
    std::istringstream keyStream(key);
    std::string subKey;
    while (std::getline(keyStream, subKey, ':')) {
    if (!node[subKey]) {
            registerUnknownKey(key);
            return false;
    }
    node = node[subKey]; // go deeper
    }

    // Key exists and is of the requested type
    addToCache(key, node);
    return true;
}

void recurse_keys(const YAML::Node& node, std::vector<std::string>& keyList, const std::string& path = "") {
    if (node.IsMap()) {
        for (const auto& it : node) {
            std::string key = it.first.as<std::string>();
            std::string new_path = path.empty() ? key : path + ":" + key;
            recurse_keys(it.second, keyList, new_path);
        }
    } else {
        keyList.push_back(path);
    }

}

std::vector<std::string> Config::keys() const {
    std::vector<std::string> keyList;
    YAML::Node node = YAML::Clone(yamlRoot);
    recurse_keys(node, keyList);
    return keyList;
}