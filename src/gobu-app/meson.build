gtkdep = dependency('gtk4')
gtksourceview5 = dependency('gtksourceview-5')
json_glib = dependency('json-glib-1.0')

gobu_app_deps = [gtkdep, gtksourceview5, json_glib]
gobuapp_src = [
    'project_manager.c',
    'gobu_project_editor.c',
    'gobu_script_editor.c',
    'gobu_script_node_viewport.c',

    'gobu_editor_world_outliner_item_column.c',

    'gobu_editor_world.c',
    'gobu_editor_world_browser.c',
    'gobu_editor_world_inspector.c',
    'gobu_editor_world_outliner.c',
    'gobu_editor_world_spriteanim.c',
    'gobu_editor_world_tileeditor.c',
    'gobu_editor_world_timeline.c',
    'gobu_editor_world_viewport.c',
    
    'gobu_gobu_embed.c',
    'gobu_widget.c',
    'gobu-app.c'
]

executable('gobueditor',
    gobuapp_src,
    dependencies: gobu_app_deps,
    link_with: [libgobu, libgobuscript],
    include_directories:['../.','ProjectEditor','.'],
    install : true,
    install_dir:base_dir+'/bin/',
    c_args : ['-W','-g','-w']
)
