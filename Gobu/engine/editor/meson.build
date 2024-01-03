c = meson.get_compiler('c')

message('Build module: Editor')
gobueditor_deps = [
    c.find_library('m'),
    dependency('epoxy'),
    dependency('gtksourceview-5'),
    dependency('gtk4'),
    dependency('glib-2.0'),
    ]

if host_machine.system() == 'windows'
    gobueditor_deps += [c.find_library('gdi32'), c.find_library('winmm')]
endif

gobueditor_src = []
gobuapp_header = ['../gobu/']

gobueditor_src += [
    'gapp_main.c',
    'gapp_widget.c',
    'project_manager.c',
    'project_manager_signal.c',
    'project_manager_fn.c',
    'browser.c',
    'project_editor.c',
    'gapp_gobu_embed.c',
    'gapp_script_editor.c',
    'gapp_tool_console.c',
    'gapp_level_inspector.c',
    'gapp_level_editor.c',
    'gapp_level_outliner.c',
    'gapp_level_viewport.c',
    'gapp_settings.c',
    'gbapp_asprites_editor.c',
    'gapp_extrasprites.c',
    libcore_source,
    libraygo_source,
]

executable('gobu-editor',
    gobueditor_src,
    dependencies: gobueditor_deps,
    link_with: [libflecs, libbinn],
    include_directories:[gobuapp_header],
    install : true,
    install_dir:base_dir+'/bin/',
    c_args : ['-W','-w','-ggdb','-DPLATFORM_VIEWPORT', '-DGRAPHICS_API_OPENGL_33', '-DBUILD_LIBTYPE_SHARED', '-DEDITOR'],
)
