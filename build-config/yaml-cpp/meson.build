yaml_cpp_sp = cmake.subproject(
    'yaml-cpp'
)
yaml_cpp_dep = yaml_cpp_sp.dependency('yaml-cpp')
add_project_arguments('-I' + meson.current_build_dir() + '/subprojects/yaml-cpp/__CMake_build', language: 'cpp')