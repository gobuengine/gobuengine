gtkdep = dependency('gtk4')
gtksourceview5 = dependency('gtksourceview-5')
json_glib = dependency('json-glib-1.0')

gobu_app_deps = [gtkdep, gtksourceview5, json_glib]
gobuapp_src = [
    'project_manager.c',
    'gobu_project_editor.c',
    'gobu_script_editor.c',
    'gobu_widget.c',
    'gobu-app.c'
]

executable('gobueditor',
    gobuapp_src,
    dependencies: gobu_app_deps,
    link_with: [libgobu],
    include_directories:['../.','ProjectEditor','.'],
    install : true,
    install_dir:base_dir+'/bin/',
    cpp_args : '-W -g -w'
)
