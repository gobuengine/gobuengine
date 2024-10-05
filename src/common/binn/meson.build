
message('Build: module::binn')

json_glib = dependency('json-glib-1.0')

libgobu_binn_src = files([
    'binn.c','binn.h',
    'binn_json.c','binn_json.h'
    ])

libgobu_binn = static_library('binn', libgobu_binn_src, dependencies: json_glib, install : true)

