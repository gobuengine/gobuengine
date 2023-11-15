c = meson.get_compiler('c')

flecs_dep = [c.find_library('m'), c.find_library('wsock32'), c.find_library('ws2_32')]

flecs_src = ['flecs.c']

argst = ['-O2', '-Wall', '-Wextra']

libflecs = static_library('flecs',
    flecs_src,
    dependencies: flecs_dep,
    include_directories:['.'],
    c_args : argst,
)
