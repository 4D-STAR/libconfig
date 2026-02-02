executable('simple_config_test', 'simple.cpp', dependencies: [config_dep])
executable('cli_example', 'cli_example.cpp', dependencies: [config_dep, cli11_dep])

if meson.is_cross_build() and host_machine.system() == 'wasm'
    executable('simple_config_wasm_test', 'wasm.cpp', dependencies: [config_dep])
endif
