c = meson.get_compiler('c')

message('Build module: Editor')

gapp_deps = [
    c.find_library('m'),
    # dependency('epoxy'),
    # dependency('gtksourceview-5'),
    dependency('gtk4'),
    # dependency('glib-2.0'),
    ]

if host_machine.system() == 'windows'
    gapp_deps += [c.find_library('gdi32'), c.find_library('winmm')]
endif

subdir('editor')
subdir('project_manager')

gapp_header = [
        '../thirdparty/libgobu/',
        '../thirdparty/libgtkray/',
    ]
gapp_link_with = [libgobu, libraygtk]

gapp_source = [
    module_project_manager,
    module_project_editor,
    'gapp_widget.c',
    'gapp_config.c',
    'gapp.c',
]

executable('gobu-editor',
    gapp_source,
    dependencies: gapp_deps,
    link_with: gapp_link_with,
    include_directories:[gapp_header],
    install : true,
    install_dir:base_dir+'/bin/',
    c_args : ['-ggdb','-Wall','-w','-Wunused-parameter'],
)
