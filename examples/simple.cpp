#include "fourdst/config/config.h"
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>

std::string get_validated_filename(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <input_file>" << std::endl;
        exit(1);
    }

    std::filesystem::path const file_path{ argv[1] };

    if (not std::filesystem::exists(file_path) || not std::filesystem::is_regular_file(file_path)) {
        std::cout << "Error: File does not exist or is not a regular file." << std::endl;
        exit(1);
    }

    return file_path.string();
}

int main(int argc, char* argv[]) {
  fourdst::config::Config& config = fourdst::config::Config::getInstance();

  std::string filename = get_validated_filename(argc, argv);
  config.loadConfig(filename);

  auto men = config.get<std::vector<std::string>>("men", {});

  for (const auto& name : men) {
    std::cout << "men are " << name << std::endl;
  }

}
