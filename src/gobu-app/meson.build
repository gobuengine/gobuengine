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
    'ProjectEditor/ComponentSystem/GobuSignalListComponentBuild.c',
    'ProjectEditor/ComponentSystem/GobuFnListComponentSave.c',
    'ProjectEditor/ComponentSystem/GobuFnListComponentBuild.c',

    # MODULE WORLD EDITOR
    'ProjectEditor/GobuProjectEditor.c',
    'ProjectEditor/GobuUiProjectEditor.c',
    'ProjectEditor/GobuProjectEditorResource.c',
    # MODULE WORLD EDITOR:WorldEditor
    'ProjectEditor/WorldEditor/GobuWorldEditor.c',
    'ProjectEditor/WorldEditor/GobuUiWorldEditor.c',
    'ProjectEditor/WorldEditor/GobuUiMenuBar.c',
    # MODULE WORLD EDITOR:WorldEditor->ContentBrowser
    'ProjectEditor/WorldEditor/ContentBrowser/GobuContentBrowser.c',
    'ProjectEditor/WorldEditor/ContentBrowser/GobuFnContentBrowserGetIconFile.c',
    'ProjectEditor/WorldEditor/ContentBrowser/GobuFnContentBrowserHistoryPath.c',
    'ProjectEditor/WorldEditor/ContentBrowser/GobuFnContentBrowserOpenFolder.c',
    'ProjectEditor/WorldEditor/ContentBrowser/GobuSignalContentBrowserCreateEntity.c',
    'ProjectEditor/WorldEditor/ContentBrowser/GobuSignalContentBrowserCreateFolder.c',
    'ProjectEditor/WorldEditor/ContentBrowser/GobuSignalContentBrowserCreateLevel.c',
    'ProjectEditor/WorldEditor/ContentBrowser/GobuSignalContentBrowserDeleteFile.c',
    'ProjectEditor/WorldEditor/ContentBrowser/GobuSignalContentBrowserDragSource.c',
    'ProjectEditor/WorldEditor/ContentBrowser/GobuSignalContentBrowserListItemFactory.c',
    'ProjectEditor/WorldEditor/ContentBrowser/GobuSignalContentBrowserNavBack.c',
    'ProjectEditor/WorldEditor/ContentBrowser/GobuSignalContentBrowserNavForward.c',
    'ProjectEditor/WorldEditor/ContentBrowser/GobuSignalContentBrowserNavHome.c',
    'ProjectEditor/WorldEditor/ContentBrowser/GobuSignalContentBrowserPopover.c',
    'ProjectEditor/WorldEditor/ContentBrowser/GobuSignalContentBrowserViewFilePopover.c',
    'ProjectEditor/WorldEditor/ContentBrowser/GobuSignalContentBrowserViewFileSelected.c',
    'ProjectEditor/WorldEditor/ContentBrowser/GobuUiContentBrowser.c',
    'ProjectEditor/WorldEditor/ContentBrowser/GobuUiContentBrowserNavToolbar.c',
    'ProjectEditor/WorldEditor/ContentBrowser/GobuUiContentBrowserToolbar.c',
    'ProjectEditor/WorldEditor/ContentBrowser/GobuUiContentBrowserViewFile.c',
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
    cpp_args : '-W -g -w'
)
