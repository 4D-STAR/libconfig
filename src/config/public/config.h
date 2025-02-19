#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

#include "yaml-cpp/yaml.h"

/**
 * @class Config
 * @brief Singleton class to manage configuration settings loaded from a YAML file.
 */
class Config {
 private:
    /**
     * @brief Private constructor to prevent instantiation.
     */
    Config();

    /**
     * @brief Destructor.
     */
    ~Config();

    YAML::Node yamlRoot; ///< Root node of the YAML configuration.
    std::string configFilePath; ///< Path to the configuration file.

 public:
    /**
     * @brief Get the singleton instance of the Config class.
     * @return Reference to the Config instance.
     */
    static Config& getInstance();

    Config (const Config&) = delete;
    Config& operator= (const Config&) = delete;
    Config (Config&&) = delete;
    Config& operator= (Config&&) = delete;

    /**
     * @brief Load configuration from a YAML file.
     * @param configFilePath Path to the YAML configuration file.
     * @return True if the configuration was loaded successfully, false otherwise.
     */
    bool loadConfig(const std::string& configFilePath);

    /**
     * @brief Get the input table from the configuration.
     * @return Input table as a string.
     */
    std::string getInputTable() const;

    /**
     * @brief Get a configuration value by key.
     * @tparam T Type of the value to retrieve.
     * @param key Key of the configuration value.
     * @param defaultValue Default value to return if the key does not exist.
     * @return Configuration value of type T.
     * 
     * @example
     * @code
     * Config& config = Config::getInstance();
     * config.loadConfig("example.yaml");
     * int maxIter = config.get<int>("opac:lowTemp:numeric:maxIter", 10);
     */
   template <typename T>
   T get(const std::string &key, T defaultValue) {
      YAML::Node node = YAML::Clone(yamlRoot);
      std::istringstream keyStream(key);
      std::string subKey;
      while (std::getline(keyStream, subKey, ':')) {
         if (!node[subKey]) {
               return defaultValue;
         }
         node = node[subKey]; // go deeper
      }

      try {
         return node.as<T>();
      } catch (const YAML::Exception& e) {
         return defaultValue; // return default value if the key does not exist
      }
   }

};

#endif