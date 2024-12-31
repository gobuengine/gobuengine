message('Build: module::libgobu')

cc = meson.get_compiler('c')

deps = []
deps += dependency('glib-2.0')
deps += dependency('threads')

if host_machine.system() == 'windows'
    deps += cc.find_library('ws2_32') # Required for http addon
endif

libgobu_src = files(
    'externs/flecs.c',
    'externs/binn.c',
    'externs/binn_json.c',
    'audio.c',
    'ecs.c',
    'gfx_backend.c',
    'gobu.c',
    'shapes.c',
    'text.c',
    'texture.c',
    'util.c',
)

libgobu = static_library('gobu', libgobu_src, dependencies: deps, install: true)