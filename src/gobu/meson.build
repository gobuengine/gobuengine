c = meson.get_compiler('c')

libgobu_deps = [dependency('epoxy'), dependency('glib-2.0'), dependency('json-glib-1.0'), c.find_library('wsock32'), c.find_library('ws2_32')]
libgobu_src = [
    'gobu-gobu.c', 
    '../thirdparty/flecs/flecs.c',
    '../thirdparty/binn/binn.c',
    '../thirdparty/binn/binn_json.c',

    '../thirdparty/raybu/rcore.c',
    '../thirdparty/raybu/rshapes.c',
    '../thirdparty/raybu/rtextures.c',
    '../thirdparty/raybu/rtext.c',
    '../thirdparty/raybu/utils.c',
    '../thirdparty/raybu/rmodels.c',
    '../thirdparty/raybu/raudio.c',
    ]

argst = ['-W' ,'-g' ,'-w','-DGRAPHICS_API_OPENGL_33', '-DPLATFORM_CUSTOM', '-DBUILD_LIBTYPE_SHARED']

libgobu = static_library('gobu',
    libgobu_src,
    dependencies: libgobu_deps,
    include_directories:['../'],
    c_args : argst,
)

library('gobu',
    libgobu_src,
    dependencies: libgobu_deps,
    include_directories:['../'],
    c_args : argst,
)