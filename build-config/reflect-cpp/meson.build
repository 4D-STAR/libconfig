tomlpp_dep = dependency('tomlplusplus', required: true).as_system()

reflect_cpp_sources = files(
    'src/yyjson.c',
    'src/reflectcpp.cpp',
    'src/reflectcpp_json.cpp',
    'src/reflectcpp_toml.cpp',
    'src/rfl/Generic.cpp',
    'src/rfl/generic/Writer.cpp',
    'src/rfl/internal/strings/strings.cpp',
    'src/rfl/json/to_schema.cpp',
    'src/rfl/json/Writer.cpp',
    'src/rfl/parsing/schema/Type.cpp',
    'src/rfl/parsing/schemaful/tuple_to_object.cpp',
    'src/rfl/toml/Writer.cpp'
)

reflect_cpp_include_dirs = include_directories('include')

reflect_cpp_library = static_library(
    'reflect_cpp',
    reflect_cpp_sources,
    include_directories: reflect_cpp_include_dirs,
    install: true,
    dependencies: [tomlpp_dep],
)

reflect_cpp_dep = declare_dependency(
    link_with: reflect_cpp_library,
    include_directories: reflect_cpp_include_dirs,
    dependencies: [tomlpp_dep],
)