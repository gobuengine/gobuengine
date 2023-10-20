c = meson.get_compiler('c')

lua_dep = [c.find_library('m')]

lua_core = ['lapi.c', 'lcode.c', 'lctype.c', 'ldebug.c', 'ldo.c', 'ldump.c', 'lfunc.c', 'lgc.c', 'llex.c', 'lmem.c', 'lobject.c', 'lopcodes.c', 'lparser.c', 'lstate.c', 'lstring.c', 'ltable.c', 'ltm.c', 'lundump.c', 'lvm.c', 'lzio.c']
lua_lib = ['lauxlib.c', 'lbaselib.c', 'lcorolib.c', 'ldblib.c', 'liolib.c', 'lmathlib.c', 'loadlib.c', 'loslib.c', 'lstrlib.c', 'ltablib.c', 'lutf8lib.c', 'linit.c']

lus_src = [lua_core, lua_lib]

argst = ['-O2', '-Wall', '-Wextra','-DLUA_COMPAT_5_3']

liblua = static_library('lua',
    lus_src,
    dependencies: lua_dep,
    include_directories:['.'],
    c_args : argst,
)
