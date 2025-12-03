cmake = import('cmake')
yaml_cpp_cmake_options = cmake.subproject_options()

yaml_cpp_cmake_options.add_cmake_defines({
        'BUILD_SHARED_LIBS': 'OFF',
        'BUILD_STATIC_LIBS': 'ON',
        'YAML_CPP_BUILD_TESTS': 'OFF',
        'CMAKE_CXX_FLAGS': '-Wno-shadow',
        'CMAKE_C_FLAGS': '-Wno-shadow',
        'CMAKE_INSTALL_LIBDIR': get_option('libdir'),
        'CMAKE_INSTALL_INCLUDEDIR': get_option('includedir'),
        'CMAKE_POLICY_VERSION_MINIMUM': '3.5',
        'CMAKE_POSITION_INDEPENDENT_CODE': 'ON'
    })

yaml_cpp_sp = cmake.subproject(
    'yaml-cpp',
    options: yaml_cpp_cmake_options,
)


yaml_cpp_tgt = yaml_cpp_sp.target('yaml-cpp')
yaml_cpp_inc = yaml_cpp_sp.include_directories('yaml-cpp')
empty_yaml_cpp_file = configure_file(
  output: 'yaml_cpp_dummy_ar.cpp',
  command: ['echo'],
  capture: true
  )
libyaml_static = static_library(
   'yaml_cpp-static',
   empty_yaml_cpp_file,
   objects: [yaml_cpp_tgt.extract_all_objects(recursive: true)],
   include_directories: yaml_cpp_inc,
   pic: true,
   install: false
)


yaml_cpp_dep = declare_dependency(
  link_with: libyaml_static,
  include_directories: yaml_cpp_inc,
)
