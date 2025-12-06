executable('simple_config_test', 'simple.cpp', dependencies: [config_dep])

if meson.is_cross_build() and host_machine.system() == 'wasm'
    executable('simple_config_wasm_test', 'wasm.cpp', dependencies: [config_dep])
endif
