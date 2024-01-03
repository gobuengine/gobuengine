c = meson.get_compiler('c')

message('Build module: Windows platform')
gobugame_deps = [
        c.find_library('m'),
        dependency('epoxy'), 
        dependency('glfw3'), 
        dependency('glib-2.0')
    ]

gobugame_src = []
gobuapp_header = ['../../thirdparty/', '../../gobue/']

if host_machine.system() == 'windows'
    gobugame_deps += c.find_library('gdi32')
    gobugame_deps += c.find_library('winmm')
endif

gobugame_src += [
    'gobu_desktop.c',
    libcore_source,
    libraygo_source
]

executable('gobu-game',
    gobugame_src,
    dependencies: gobugame_deps,
    link_with: [libflecs, libbinn],
    include_directories:[gobuapp_header],
    install : true,
    install_dir:base_dir+'/bin/',
    c_args : ['-DPLATFORM_DESKTOP', '-DGRAPHICS_API_OPENGL_33', '-DBUILD_LIBTYPE_SHARED']
)
