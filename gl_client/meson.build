mylibdir = get_option('libdir') / 'gl-streaming'

libglstreaming = shared_library(
  'gl-streaming',
  sources: ['glclient.c',
            common_srcs,
           ],
  include_directories : [common_incdir],
  dependencies: [thread_dep, lttust_dep, xenvchan_dep],
  link_args: ['-Wl,-init,gls_init_library,-fini,gls_cleanup_library'],
  install: true,
)

libgles = shared_library(
  'GLESv2',
  soversion: '2',
  sources: ['clientgles.c',
           ],
  include_directories : [common_incdir],
  dependencies: [lttust_dep],
  link_with: [libglstreaming],
  install: true,
  install_dir: mylibdir,
)

libegl = shared_library(
  'EGL',
  soversion: '1',
  sources: ['clientegl.c',
           ],
  include_directories : [common_incdir],
  dependencies: [x11_dep, lttust_dep],
  link_with: [libglstreaming, libgles],
  install: true,
  install_dir: mylibdir,
)
