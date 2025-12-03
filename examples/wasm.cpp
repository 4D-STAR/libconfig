#include "fourdst/config/config.h"
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>


int main(int argc, char* argv[]) {
  fourdst::config::Config& config = fourdst::config::Config::getInstance();

  config.loadConfig("/input.yaml");

  auto men = config.get<std::vector<std::string>>("men", {});

  for (const auto& name : men) {
    std::cout << "men are " << name << std::endl;
  }

}
