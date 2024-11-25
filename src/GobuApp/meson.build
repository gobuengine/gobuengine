# Compilador C

cc = meson.get_compiler('c')

# Dependencias

deps = []
deps += dependency('gtk4')
deps += dependency('glib-2.0')
deps += dependency('epoxy')
deps += dependency('gtksourceview-5')
deps += dependency('libadwaita-1')
deps += declare_dependency(link_with: libgobu_binn)
deps += declare_dependency(link_with: libgobu_pixi)
deps += declare_dependency(link_with: libgobu_pixio)
deps += declare_dependency(link_with: libgobu_gbase)

# Fuentes

sources = [
  'types/type_enum.c',
  'types/type_outliner_item.c',
  'types/type_outliner_popover_item.c',
  'gapp_asprite.c',
  'gapp_console.c',
  'gapp_inspector_widgets.c',
  'gapp_inspector.c',
  'gapp_scene.c',
  'gapp_project_manager.c',
  'gapp_project_setting.c',
  'gapp_project_config.c',
  'gapp_script.c',
  'gapp_viewport.c',
  'gapp_browser.c',
  'gapp_common.c',
  'gapp_widget.c',
  'gapp.c',
]

# Headers

headers = [
  'config.h',
  'types/type_enum.h',
  'types/type_outliner_item.h',
  'types/type_outliner_popover_item.h',
  'gapp_asprite.h',
  'gapp_console.h',
  'gapp_inspector_widgets.h',
  'gapp_inspector.h',
  'gapp_scene.h',
  'gapp_project_manager.h',
  'gapp_project_setting.h',
  'gapp_project_config.h',
  'gapp_script.h',
  'gapp_viewport.h',
  'gapp_browser.h',
  'gapp_common.h',
  'gapp_widget.h',
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
