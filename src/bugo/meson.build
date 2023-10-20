c = meson.get_compiler('c')

subdir('external/lua')

bugo_deps = [
        dependency('epoxy'),
        dependency('glib-2.0'),
        dependency('json-glib-1.0'),
        c.find_library('wsock32'),
        c.find_library('ws2_32'),
    ]

bugo_src = [
    'external/flecs.c',
    'external/binn.c',
    'external/binn_json.c',

    'components/bugo_bcamera2d.c',
    'components/bugo_bcolor.c',
    'components/bugo_brectangle.c',
    'components/bugo_bvector2.c',
    'components/bugo_blua.c',
    'components/bugo_draw_shape_rect.c',
    'components/bugo_draw_sprite.c',
    'components/bugo_renderer.c',
    'components/bugo_phaser.c',

    'external/tmp_render/rcore.c',
    'external/tmp_render/raudio.c',
    'external/tmp_render/rshapes.c',
    'external/tmp_render/rtext.c',
    'external/tmp_render/rtextures.c',
    'external/tmp_render/rmodels.c',
    'external/tmp_render/utils.c',

    'bugo.c',
    ]

argst = ['-W' ,'-g' ,'-w','-DGRAPHICS_API_OPENGL_33']

libgobubugo = static_library('gobu-bugo',
    bugo_src,
    link_with:[liblua],
    dependencies: bugo_deps,
    include_directories:['../','external/lua'],
    c_args : argst,
)

# library('gobu',
#     bugo_src,
#     dependencies: bugo_deps,
#     include_directories:['../'],
#     c_args : argst,
# )