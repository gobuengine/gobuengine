c = meson.get_compiler('c')

message('Build: libgtkray')

lib_deps = [
    c.find_library('m'),
    dependency('gtk4'),
    dependency('epoxy'),
    ]

lib_src = files(['gtkray.c'])

libraygtk = static_library('gtkray', 
    lib_src, 
    dependencies: lib_deps, 
    include_directories:[],
    c_args : ['-ggdb','-Wall','-w','-Wunused-parameter'])

