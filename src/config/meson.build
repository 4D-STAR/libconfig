config_dep = declare_dependency(
    include_directories: include_directories('include'),
    dependencies: [reflect_cpp_dep],
)

config_headers = files(
    'include/fourdst/config/config.h',
    'include/fourdst/config/base.h',
    'include/fourdst/config/cli.h',
    'include/fourdst/config/ansi.h',
    'include/fourdst/config/validate.h',
)
install_headers(config_headers, subdir : 'fourdst/config')

condig_exception_headers = files(
    'include/fourdst/config/exceptions/exceptions.h',
)
install_headers(config_headers, subdir : 'fourdst/config/exceptions')

