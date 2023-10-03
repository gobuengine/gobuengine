c = meson.get_compiler('c')

libgobu_deps = [dependency('glib-2.0'), dependency('json-glib-1.0'), c.find_library('wsock32'), c.find_library('ws2_32')]
libgobu_src = [
    'gobu-gobu.c', 
    '../thirdparty/flecs/flecs.c',
    '../thirdparty/binn/binn.c',
    '../thirdparty/binn/binn_json.c',
    ]

libgobu = static_library('gobu',
    libgobu_src,
    dependencies: libgobu_deps,
    include_directories:['../'],
    cpp_args : '-W -g -w'
)