cli11_proj = subproject('cli11')
cli11_dep = cli11_proj.get_variable('CLI11_dep')

install_subdir(
    meson.project_source_root() / 'subprojects' / 'CLI11-2.6.1' / 'include',
    install_dir: get_option('includedir') / 'fourdst' / 'vendor',
    strip_directory: true
)