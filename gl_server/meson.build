egl_dep = dependency('egl')
gles2_dep = dependency('glesv2')

executable('gl_server',
           sources: ['main.c', 'glcontrol.c', 'glserver.c',
                     'serveregl.c', 'servergles.c',
                     'list.c',
                     common_srcs,
                    ],
           include_directories : [common_incdir],
           dependencies: [egl_dep, gles2_dep, x11_dep, thread_dep, rt_dep],
           c_args : '-DGLS_SERVER',
           install: true,
          )
