message('Build: module::libgobu')

cc = meson.get_compiler('c')

renderer_selected = get_option('backrender')

# Definiciones de backends
backends_defines = {
  'glcore': '-DSOKOL_GLCORE',
  'gles3': '-DSOKOL_GLES3', 
  'd3d11': '-DSOKOL_D3D11',
  'metal': '-DSOKOL_METAL',
  'wgpu': '-DSOKOL_WGPU'
}

deps = []
deps += dependency('glib-2.0')
deps += dependency('threads')

if host_machine.system() == 'windows'
    deps += cc.find_library('ws2_32') # Required for http addon

    # Windows dependencies backends
    deps += cc.find_library('kernel32')
    deps += cc.find_library('user32')
    deps += cc.find_library('shell32')
    deps += cc.find_library('gdi32')
    deps += cc.find_library('ole32')

    if renderer_selected == 'd3d11'
        deps += cc.find_library('dxgi')
        deps += cc.find_library('d3d11')
    endif
endif

libgobu_src = files(
    'externs/flecs.c',
    'gobu-backend-sokol.c',
    'gobu-util.c',
    'gobu-ecs.c',
    'gobu-gfx.c',
    'gobu-input.c',
    'gobu-renderer-draw.c',
    'gobu-text.c',
    'gobu-texture.c',
    'gobu-audio.c',
)

libgobu_header = files(
    'gobu.h',
    'gobu-backend-sokol.h',
    'gobu-util.h',
    'gobu-ecs.h',
    'gobu-gfx.h',
    'gobu-input.h',
    'gobu-renderer-draw.h',
    'gobu-gfx-internal.h',
    'gobu-text.h',
    'gobu-texture.h',
    'gobu-audio.h',
)

message('Renderer selected: ' + backends_defines[renderer_selected])

# headers
install_headers(libgobu_header, subdir: 'gobu')

libgobu = static_library('gobu', libgobu_src, dependencies: deps, install: true, c_args : backends_defines[renderer_selected])

