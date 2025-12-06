config_dep = declare_dependency(
    include_directories: include_directories('include'),
    dependencies: [reflect_cpp_dep],
)

config_headers = files(
  'include/fourdst/config/config.h',
  'include/fourdst/config/exceptions/exceptions.h',
  'include/fourdst/config/base.h',
)
install_headers(config_headers, subdir : 'fourdst/fourdst/config')
