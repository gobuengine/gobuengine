message('Build: module::libgobu')

cc = meson.get_compiler('c')

renderer_selected = get_option('brender')

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
    'gb_audio.c',
    'gb_ecs.c',
    'gfx_backend.c', # TODO: Move to renderer
    'gobu.c',
    'gb_shapes.c',
    'gb_text.c',
    'gb_texture.c',
    'gb_util.c',
)

libgobu = static_library('gobu', libgobu_src, dependencies: deps, install: true, c_args : backends_defines[renderer_selected])

