cli11_proj = subproject('cli11')
cli11_dep = cli11_proj.get_variable('CLI11_dep')

cli11_headers = cli11_proj.get_variable('cli11_headers')
cli11_impl_headers = cli11_proj.get_variable('cli11_impl_headers')

if get_option('build_python')
    install_data(cli11_headers,      install_dir: vendor_header_dir / 'CLI')
    install_data(cli11_impl_headers, install_dir: vendor_header_dir / 'CLI' / 'impl')
else
    install_headers(cli11_headers,      install_dir: vendor_header_dir / 'CLI')
    install_headers(cli11_impl_headers, install_dir: vendor_header_dir / 'CLI' / 'impl')
endif