yaml_cpp_cmake_options = cmake.subproject_options()
yaml_cpp_cmake_options.add_cmake_defines({
 'CMAKE_POLICY_VERSION_MINIMUM': '3.5'
})
yaml_cpp_sp = cmake.subproject(
    'yaml-cpp',
    options: yaml_cpp_cmake_options,
)
yaml_cpp_dep = yaml_cpp_sp.dependency('yaml-cpp')
add_project_arguments('-I' + meson.current_build_dir() + '/subprojects/yaml-cpp/__CMake_build', language: 'cpp')