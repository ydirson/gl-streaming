project(
  'gls-streaming', 'c',
  #meson_version: '>= 0.28',
  default_options: [
    'werror=true',
    'warning_level=2',
    'buildtype=debugoptimized',
  ],
)

add_project_arguments(
  [
    '-DUSE_X11',
    '-Wmissing-prototypes',
    '-Wold-style-definition',
  ],
  language : 'c')

common_incdir = include_directories('common')
common_srcs = files(
  [
    'common/ring.c',
    'common/recvr.c',
    'common/transport.c',
    'common/transport-tcp.c',
    'common/transport-unix.c',
    'common/transport-stdio.c',
    'common/transport-qrexecpipe.c',
  ])

thread_dep = dependency('threads')
x11_dep = dependency('x11')

subdir('gl_client')
subdir('gl_server')
