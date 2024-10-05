
message('Build: module::pixi')

cc = meson.get_compiler('c')

deps = []
deps += dependency('epoxy')
deps += cc.find_library('m')

if host_machine.system() == 'windows'
    deps += cc.find_library('winmm')
endif

libgobu_pixi_src = files([
    'src/raylib.h',
    'src/rcore.c',
    'src/raudio.c',
    'src/rshapes.c',
    'src/rtext.c',
    'src/rtextures.c',
    'src/utils.c',
    'pixi.c',
    ])

libgobu_pixi = static_library('pixi', libgobu_pixi_src, dependencies: deps, install : true)

