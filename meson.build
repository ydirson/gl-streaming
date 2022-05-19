project(
  'gls-streaming', 'c',
  #meson_version: '>= 0.28',
  default_options: [
    'werror=true',
    'warning_level=2',
    'buildtype=debugoptimized',
  ],
)

use_ltt = get_option('ltt')

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
    'common/transport-stdio.c',
    'common/transport-qrexecpipe.c',
  ])

if use_ltt
  add_project_arguments(
    [
      '-DUSE_LTT',
    ],
    language : 'c')
  common_srcs += files(
    [
    'common/ltt_ring.c',
    'common/ltt_tport.c',
    ])
endif

thread_dep = dependency('threads')
x11_dep = dependency('x11')
dl_dep = meson.get_compiler('c').find_library('dl')

lttust_dep = use_ltt ? [ dependency('lttng-ust'), dl_dep ] : []

subdir('gl_client')
subdir('gl_server')
