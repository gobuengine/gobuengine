c = meson.get_compiler('c')

message('Build: libgobu')

subdir('pixi')
subdir('ecs')
subdir('utility')
subdir('binn')

lib_deps = [
    c.find_library('m'),
    dependency('glib-2.0'), 
    dependency('json-glib-1.0'),
    dependency('gio-2.0'),
    dependency('epoxy'),
    ]

lib_src = [libgobu_binn, libgobu_utility, libgobu_ecs, lib_pixi, files(['gb_project.c','gb_setting_project.c'])]

libgobu = static_library('gobu', 
    lib_src, 
    dependencies: lib_deps, 
    include_directories:[],
    c_args : ['-ggdb','-Wall','-w','-Wunused-parameter'])

