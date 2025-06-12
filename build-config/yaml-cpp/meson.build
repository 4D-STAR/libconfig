yaml_cpp_cmake_options = cmake.subproject_options()
yaml_cpp_cmake_options.add_cmake_defines({
 'CMAKE_POLICY_VERSION_MINIMUM': '3.5',
 'BUILD_SHARED_LIBS': 'ON',
 'CMAKE_SKIP_INSTALL_RULES': 'ON',
 'YAML_CPP_BUILD_TESTS': 'OFF'
})
yaml_cpp_sp = cmake.subproject(
    'yaml-cpp',
    options: yaml_cpp_cmake_options,
)
yaml_cpp_dep = yaml_cpp_sp.dependency('yaml-cpp')