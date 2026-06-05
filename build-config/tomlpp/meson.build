tomlpp_dep = dependency('tomlplusplus', required: true, static: true).as_system()

install_subdir(
    meson.project_source_root() / 'subprojects' / 'tomlplusplus-3.4.0' / 'include',
    install_dir: get_option('includedir'),
    strip_directory: true
)