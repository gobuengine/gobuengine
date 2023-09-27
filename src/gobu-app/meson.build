gtkdep = dependency('gtk4')
gtksourceview5 = dependency('gtksourceview-5')
json_glib = dependency('json-glib-1.0')

gobu_app_deps = [gtkdep, gtksourceview5, json_glib]
gobuapp_src = [
    # MODULE PROJECT MANAGER
    'ProjectManager/GobuProjectManager.c',
    'ProjectManager/GobuUiProjectManager.c',
    'ProjectManager/GobuSignalChangePath.c',
    'ProjectManager/GobuSignalCheckNameProject.c',
    'ProjectManager/GobuSignalCreateProject.c',
    'ProjectManager/GobuSignalOpenProject.c',
    'ProjectManager/GobuFnCreateProject.c',

    # MODULE COMPONENTSYSTEM
    'ProjectEditor/ComponentSystem/GobuComponentSystem.c',
    'ProjectEditor/ComponentSystem/GobuUiComponentSystem.c',
    'ProjectEditor/ComponentSystem/GobuSignalCreateComponent.c',
    'ProjectEditor/ComponentSystem/GobuSignalListComponent.c',
    'ProjectEditor/ComponentSystem/GobuSignalListComponentSelected.c',
    'ProjectEditor/ComponentSystem/GobuSignalListComponentSave.c',
    'ProjectEditor/ComponentSystem/GobuSignalListComponentSaveVirtual.c',
    'ProjectEditor/ComponentSystem/GobuFnListComponentSave.c',

    # MODULE WORLD EDITOR
    'ProjectEditor/GobuProjectEditor.c',
    'ProjectEditor/GobuUiProjectEditor.c',
    'ProjectEditor/WorldEditor/GobuWorldEditor.c',
    'ProjectEditor/WorldEditor/GobuUiWorldEditor.c',
    'ProjectEditor/WorldEditor/GobuUiMenuBar.c',
    # 'ProjectEditor/WorldEditor/ContentBrowser/gobu-browser-content.c',
    # 'ProjectEditor/WorldEditor/EntityInspector/gobu-entity-inspector.c',
    # 'ProjectEditor/WorldEditor/WorldOutliner/gobu-world-outliner.c',
    # 'ProjectEditor/WorldEditor/WorlrdViewport/gobu-scene-viewport.c',
    # 'ProjectEditor/WorldEditor/gobu-project-editor.c',
    # MAIN
    'GobuWidgets.c',
    # 'ProjectEditor/gobu-editor-menubar.c', 
    # 'ProjectEditor/gobu-editor.c',
    'main.c'
]

executable('gobueditor',
    gobuapp_src,
    dependencies: gobu_app_deps,
    link_with: [libgobu],
    include_directories:['../.','ProjectEditor','.'],
    install : true,
    install_dir:base_dir+'/bin/',
    cpp_args : '-W -g'
)
