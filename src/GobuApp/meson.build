# Compilador C

cc = meson.get_compiler('c')

# Dependencias

deps = []
deps += dependency('gtk4')
deps += dependency('glib-2.0')
deps += dependency('epoxy')
deps += dependency('gtksourceview-5')
deps += dependency('libadwaita-1')
deps += declare_dependency(link_with: libgobu)
deps += declare_dependency(link_with: libgobu_binn)
# deps += declare_dependency(link_with: libgobu_pixi)
# deps += declare_dependency(link_with: libgobu_pixio)
# deps += declare_dependency(link_with: libgobu_gbase)

# Fuentes

sources = [
  'types/type_enum.c',
  'types/type_outliner_item.c',
  'types/type_outliner_popover_item.c',
  'gapp_inspector_widgets.c',
  'gapp_inspector.c',
  'gapp_prefab.c',
  'gapp_scene_hierarchy.c',
  'gapp_scene_manager.c',
  'gapp_scene_viewport.c',
  'gapp_project_manager.c',
  'gapp_project_setting.c',
  'gapp_widget.c',
  'gapp_wviewport.c',
  'gapp_whierarchy.c',
  'gapp.c',
]

# Headers

headers = [
  'config.h',
  'types/type_enum.h',
  'types/type_outliner_item.h',
  'types/type_outliner_popover_item.h',
  'gapp_inspector_widgets.h',
  'gapp_inspector.h',
  'gapp_prefab.h',
  'gapp_scene_hierarchy.h',
  'gapp_scene_manager.h',
  'gapp_scene_viewport.h',
  'gapp_project_manager.h',
  'gapp_project_setting.h',
  'gapp_widget.h',
  'gapp_wviewport.h',
  'gapp_whierarchy.h',
  'gapp.h',
]

# Incluyir directorio actual

inc = include_directories(['.', '../common/'])

# Ejecutable

executable(
  'gobu',
  sources: sources,
  include_directories: inc,
  dependencies: deps,
  install: true,
  install_dir: base_dir + '/bin/'
)

# Instalar headers

install_headers(headers)

# Configuración para pkg-config

pkg = import('pkgconfig')
pkg.generate(
  name: 'gobu',
  filebase: 'gobu',
  description: 'GTK4 based application',
  version: '0.1',
)
