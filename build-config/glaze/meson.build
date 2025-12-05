glaze_cmake_options = cmake.subproject_options()

glaze_cmake_options.add_cmake_defines({
     'BUILD_SHARED_LIBS': 'OFF',
     'BUILD_STATIC_LIBS': 'ON',
     'CMAKE_INSTALL_LIBDIR': get_option('libdir'),
     'CMAKE_INSTALL_INCLUDEDIR': get_option('includedir'),
     'CMAKE_POSITION_INDEPENDENT_CODE': 'ON',
     'galze_BUILD_EXAMPLES': 'OFF',
 })

glaze_sp = cmake.subproject(
    'glaze',
    options: glaze_cmake_options,
)
glaze_dep = glaze_sp.dependency('glaze_glaze')
