# Define the library
config_sources = files(
  'private/config.cpp',
)

config_headers = files(
    'public/config.h'
)

# Define the libconfig library so it can be linked against by other parts of the build system
libconfig = library('config',
    config_sources, 
    include_directories: include_directories('public'),
    cpp_args: ['-fvisibility=default'],
    dependencies: [yaml_cpp_dep],
    install : true)

config_dep = declare_dependency(
    include_directories: include_directories('public'),
    link_with: libconfig,
    sources: config_sources,
    dependencies: [yaml_cpp_dep],
)

# Make headers accessible
install_headers(config_headers, subdir : 'libconfig')
