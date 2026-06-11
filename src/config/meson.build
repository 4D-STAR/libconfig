config_dep = declare_dependency(
    include_directories: [
        include_directories('include'),
        include_directories('include/fourdst/config'),
    ],
    dependencies: [reflect_cpp_dep],
)

config_headers = files(
    'include/fourdst/config/config.h',
    'include/fourdst/config/base.h',
    'include/fourdst/config/cli.h',
    'include/fourdst/config/ansi.h',
    'include/fourdst/config/validate.h',
)

config_exception_headers = files(
    'include/fourdst/config/exceptions/exceptions.h',
)

if get_option('build_python')
    install_data(config_headers,           install_dir: config_header_install_dir)
    install_data(config_exception_headers, install_dir: config_header_install_dir / 'exceptions')
else
    install_headers(config_headers,           install_dir: config_header_install_dir)
    install_headers(config_exception_headers, install_dir: config_header_install_dir / 'exceptions')
endif