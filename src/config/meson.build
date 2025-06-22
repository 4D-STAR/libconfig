# Define the library
config_sources = files(
  'lib/config.cpp',
)


# Define the libconfig library so it can be linked against by other parts of the build system
libconfig = library('config',
    config_sources, 
    include_directories: include_directories('include'),
    cpp_args: ['-fvisibility=default'],
    dependencies: [yaml_cpp_dep],
    install : true)

config_dep = declare_dependency(
    include_directories: include_directories('include'),
    link_with: libconfig,
    sources: config_sources,
    dependencies: [yaml_cpp_dep],
)

# Make headers accessible
config_headers = files(
  'include/fourdst/config/config.h'
)
install_headers(config_headers, subdir : 'fourdst/fourdst/config')
