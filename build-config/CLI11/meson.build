cli11_proj = subproject('cli11')
cli11_dep = cli11_proj.get_variable('CLI11_dep')

cli11_headers = cli11_proj.get_variable('cli11_headers')
cli11_impl_headers = cli11_proj.get_variable('cli11_impl_headers')

install_headers(cli11_headers,      install_dir: config_header_root / 'fourdst' / 'vendor' / 'CLI')
install_headers(cli11_impl_headers, install_dir: config_header_root / 'fourdst' / 'vendor' / 'CLI' / 'impl')