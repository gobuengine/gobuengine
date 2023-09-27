libgobu_deps = [dependency('glib-2.0'), dependency('json-glib-1.0')]
libgobu_src = [
    'gobu.c', 
    'gobu-project.c',
    'gobu-json.c',
    './thirdparty/binn/binn.c',
    './thirdparty/binn/binn_json.c',
    ]

libgobu = static_library('gobu',
    libgobu_src,
    dependencies: libgobu_deps,
    include_directories:['./thirdparty']
)