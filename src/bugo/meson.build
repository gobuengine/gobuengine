c = meson.get_compiler('c')

libgobu_deps = [dependency('epoxy'), dependency('glib-2.0'), dependency('json-glib-1.0'), c.find_library('wsock32'), c.find_library('ws2_32')]
libgobu_src = [
    'external/flecs.c',
    'external/binn.c',
    'external/binn_json.c',

    'external/tmp_render/rcore.c',
    'external/tmp_render/raudio.c',
    'external/tmp_render/rshapes.c',
    'external/tmp_render/rtext.c',
    'external/tmp_render/rtextures.c',
    'external/tmp_render/rmodels.c',
    'external/tmp_render/utils.c',

    'bugo_text.c',
    'bugo_shape.c',
    'bugo_core.c',
    'bugo_ecs.c',
    ]

argst = ['-W' ,'-g' ,'-w','-DGRAPHICS_API_OPENGL_33']

libgobubugo = static_library('gobu-bugo',
    libgobu_src,
    dependencies: libgobu_deps,
    include_directories:['../'],
    c_args : argst,
)

# library('gobu',
#     libgobu_src,
#     dependencies: libgobu_deps,
#     include_directories:['../'],
#     c_args : argst,
# )