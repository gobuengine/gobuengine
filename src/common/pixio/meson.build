message('Build: module::pixio')

cc = meson.get_compiler('c')

deps = []
deps += dependency('glib-2.0')
deps += declare_dependency(link_with: libgobu_pixi)
deps += dependency('threads')

if host_machine.system() == 'windows'
    deps += cc.find_library('ws2_32') # Required for http addon
endif

libgobu_pixio_src = files(
    'flecs.c',
    'pixio.c',
    'pixio_render.c',
    'pixio_type.c',
    'pixio_text.c',
)

libgobu_pixio = static_library('pixio', libgobu_pixio_src, dependencies: deps, install: true)