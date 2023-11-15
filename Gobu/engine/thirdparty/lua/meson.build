c = meson.get_compiler('c')

lua_dep = [c.find_library('m')]

lua_core = ['src/lapi.c', 'src/lcode.c', 'src/lctype.c', 'src/ldebug.c', 'src/ldo.c', 'src/ldump.c', 'src/lfunc.c', 'src/lgc.c', 'src/llex.c', 'src/lmem.c', 'src/lobject.c', 'src/lopcodes.c', 'src/lparser.c', 'src/lstate.c', 'src/lstring.c', 'src/ltable.c', 'src/ltm.c', 'src/lundump.c', 'src/lvm.c', 'src/lzio.c']
lua_lib = ['src/lauxlib.c', 'src/lbaselib.c', 'src/lcorolib.c', 'src/ldblib.c', 'src/liolib.c', 'src/lmathlib.c', 'src/loadlib.c', 'src/loslib.c', 'src/lstrlib.c', 'src/ltablib.c', 'src/lutf8lib.c', 'src/linit.c']

lus_src = [lua_core, lua_lib]

argst = ['-O2', '-Wall', '-Wextra','-DLUA_COMPAT_5_3']

liblua = static_library('lua',
    lus_src,
    dependencies: lua_dep,
    include_directories:['src/'],
    c_args : argst,
)
