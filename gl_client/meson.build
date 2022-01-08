mylibdir = get_option('libdir') / 'gl-streaming'

dl_dep = meson.get_compiler('c').find_library('dl', required : false)

libglstreaming = shared_library(
  'gl-streaming',
  sources: ['glclient.c',
            common_srcs,
           ],
  include_directories : [common_incdir],
  dependencies: [thread_dep],
  link_args: '-Wl,-init,gls_init_library,-fini,gls_cleanup_library',
  install: true,
)

libgles = shared_library(
  'GLESv2',
  soversion: '2',
  sources: ['clientgles.c',
           ],
  include_directories : [common_incdir],
  link_with: [libglstreaming],
  install: true,
  install_dir: mylibdir,
)

libegl = shared_library(
  'EGL',
  soversion: '1',
  sources: ['clientegl.c',
            common_srcs,
           ],
  include_directories : [common_incdir],
  dependencies: [thread_dep, dl_dep, x11_dep],
  link_with: [libglstreaming, libgles],
  install: true,
  install_dir: mylibdir,
)
