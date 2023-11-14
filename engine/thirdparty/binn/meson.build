c = meson.get_compiler('c')

binn_dep = [dependency('glib-2.0'), dependency('json-glib-1.0'), c.find_library('m')]

binn_src = ['binn.c', 'binn_json.c']

argst = ['-O2', '-Wall', '-Wextra']

libbinn = static_library('binn',
    binn_src,
    dependencies: binn_dep,
    include_directories:['.'],
    c_args : argst,
)
