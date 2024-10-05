
message('Build: module::gobu-base')

cc = meson.get_compiler('c')

deps = []
deps += dependency('glib-2.0')
deps += cc.find_library('m')

libgobu_gbase_src = files([
    # 'gobu_project_config.c',
    'gobu.c',
    ])

libgobu_gbase = static_library('gobu-base', libgobu_gbase_src, dependencies: deps, install : true)

