/*
Copyright (c) 2013, Shodruky Rhyammer
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

  Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

  Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

  Neither the name of the copyright holders nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once
#include <stdint.h>

#include "EGL/egl.h"
#include "GLES2/gl2.h"

#define GLS_TMP_BUFFER_SIZE 2097152
#define GLS_DATA_SIZE 356
#define GLS_STRING_SIZE_PLUS 10240 // 256
#define GLS_STRING_SIZE 10160 // 254
#define GLS_ALIGNMENT_BITS 3


// Even versions are stable, should not change a single bit or
// semantic of protocol without bumping version.
// Odd versions are development ones to prepare the next even version,
// change at dev's whim, deploy at your own risk.
#define GLS_VERSION 11

// #define GL_MAJOR_VERSION 1
// #define GL_MINOR_VERSION 2

// FIXME: hack for emacs26 to consistently indent the rest, because of
// lack of separator in the lists
#define EMPTY()

// FIXME: commands should be further split between batchable and non-batchable

#define GLS_COMMANDS()                          \
  EMPTY()                                       \
    GLS_GLS_COMMANDS()                          \
    GLS_EGL_COMMANDS()                          \
    GLS_GLES2_COMMANDS()                        \
    GLS_GLES2_EXT_COMMANDS()                    \
  //

#define GLS_GLS_COMMANDS()                      \
  EMPTY()                                       \
    X(BREAK)                                    \
    X(SEND_DATA)                                \
    X(FLUSH)                                    \
    X(get_context)                              \
  //

#define GLS_EGL_COMMANDS()                      \
  EMPTY()                                       \
    GLS_EGL10_COMMANDS()                        \
    GLS_EGL11_COMMANDS()                        \
    GLS_EGL12_COMMANDS()                        \
    GLS_EGL14_COMMANDS()                        \
    GLS_EGL15_COMMANDS()                        \
  //

#define GLS_EGL10_COMMANDS()                    \
  EMPTY()                                       \
    X(eglChooseConfig)                          \
    X(eglCopyBuffers)                           \
    X(eglCreateContext)                         \
    X(eglCreatePbufferSurface)                  \
    X(eglCreatePixmapSurface)                   \
    X(eglCreateWindowSurface)                   \
    X(eglDestroyContext)                        \
    X(eglDestroySurface)                        \
    X(eglGetConfigAttrib)                       \
    X(eglGetConfigs)                            \
    X(eglGetCurrentDisplay)                     \
    X(eglGetCurrentSurface)                     \
    X(eglGetDisplay)                            \
    X(eglGetError)                              \
    X(eglGetProcAddress)                        \
    X(eglInitialize)                            \
    X(eglMakeCurrent)                           \
    X(eglQueryContext)                          \
    X(eglQueryString)                           \
    X(eglQuerySurface)                          \
    X(eglSwapBuffers)                           \
    X(eglTerminate)                             \
    X(eglWaitGL)                                \
    X(eglWaitNative)                            \
  //

#define GLS_EGL11_COMMANDS()                    \
  EMPTY()                                       \
    X(eglBindTexImage)                          \
    X(eglReleaseTexImage)                       \
    X(eglSurfaceAttrib)                         \
    X(eglSwapInterval)                          \
  //

#define GLS_EGL12_COMMANDS()                    \
  EMPTY()                                       \
    X(eglBindAPI)                               \
    X(eglQueryAPI)                              \
    X(eglCreatePbufferFromClientBuffer)         \
    X(eglReleaseThread)                         \
    X(eglWaitClient)                            \
  //

// no new commands in EGL 1.3

#define GLS_EGL14_COMMANDS()                    \
  EMPTY()                                       \
    X(eglGetCurrentContext)                     \
  //

#define GLS_EGL15_COMMANDS()                    \
  EMPTY()                                       \
    X(eglCreateSync)                            \
    X(eglDestroySync)                           \
    X(eglClientWaitSync)                        \
    X(eglGetSyncAttrib)                         \
    X(eglCreateImage)                           \
    X(eglDestroyImage)                          \
    X(eglGetPlatformDisplay)                    \
    X(eglCreatePlatformPixmapSurface)           \
    X(eglCreatePlatformWindowSurface)           \
    X(eglWaitSync)                              \
  //

#define GLS_GLES2_COMMANDS()                    \
  EMPTY()                                       \
    X(glActiveTexture)                          \
    X(glAttachShader)                           \
    X(glBindAttribLocation)                     \
    X(glBindBuffer)                             \
    X(glBindFramebuffer)                        \
    X(glBindRenderbuffer)                       \
    X(glBindTexture)                            \
    X(glBlendColor)                             \
    X(glBlendEquation)                          \
    X(glBlendEquationSeparate)                  \
    X(glBlendFunc)                              \
    X(glBlendFuncSeparate)                      \
    X(glBufferData)                             \
    X(glBufferSubData)                          \
    X(glCheckFramebufferStatus)                 \
    X(glClear)                                  \
    X(glClearColor)                             \
    X(glClearDepthf)                            \
    X(glClearStencil)                           \
    X(glColorMask)                              \
    X(glCompileShader)                          \
    X(glCompressedTexImage2D)                   \
    X(glCompressedTexSubImage2D)                \
    X(glCopyTexImage2D)                         \
    X(glCopyTexSubImage2D)                      \
    X(glCreateProgram)                          \
    X(glCreateShader)                           \
    X(glCullFace)                               \
    X(glDeleteBuffers)                          \
    X(glDeleteFramebuffers)                     \
    X(glDeleteProgram)                          \
    X(glDeleteRenderbuffers)                    \
    X(glDeleteShader)                           \
    X(glDeleteTextures)                         \
    X(glDepthFunc)                              \
    X(glDepthMask)                              \
    X(glDepthRangef)                            \
    X(glDetachShader)                           \
    X(glDisable)                                \
    X(glDisableVertexAttribArray)               \
    X(glDrawArrays)                             \
    X(glDrawElements)                           \
    X(glEnable)                                 \
    X(glEnableVertexAttribArray)                \
    X(glFinish)                                 \
    X(glFlush)                                  \
    X(glFramebufferRenderbuffer)                \
    X(glFramebufferTexture2D)                   \
    X(glFrontFace)                              \
    X(glGenBuffers)                             \
    X(glGenerateMipmap)                         \
    X(glGenFramebuffers)                        \
    X(glGenRenderbuffers)                       \
    X(glGenTextures)                            \
    X(glGetActiveAttrib)                        \
    X(glGetActiveUniform)                       \
    X(glGetAttachedShaders)                     \
    X(glGetAttribLocation)                      \
    X(glGetBooleanv)                            \
    X(glGetBufferParameteriv)                   \
    X(glGetError)                               \
    X(glGetFloatv)                              \
    X(glGetFramebufferAttachmentParameteriv)    \
    X(glGetIntegerv)                            \
    X(glGetProgramiv)                           \
    X(glGetProgramInfoLog)                      \
    X(glGetRenderbufferParameteriv)             \
    X(glGetShaderiv)                            \
    X(glGetShaderInfoLog)                       \
    X(glGetShaderPrecisionFormat)               \
    X(glGetShaderSource)                        \
    X(glGetString)                              \
    X(glGetTexParameterfv)                      \
    X(glGetTexParameteriv)                      \
    X(glGetUniformfv)                           \
    X(glGetUniformiv)                           \
    X(glGetUniformLocation)                     \
    X(glGetVertexAttribfv)                      \
    X(glGetVertexAttribiv)                      \
    X(glGetVertexAttribPointerv)                \
    X(glHint)                                   \
    X(glIsBuffer)                               \
    X(glIsEnabled)                              \
    X(glIsFramebuffer)                          \
    X(glIsProgram)                              \
    X(glIsRenderbuffer)                         \
    X(glIsShader)                               \
    X(glIsTexture)                              \
    X(glLineWidth)                              \
    X(glLinkProgram)                            \
    X(glPixelStorei)                            \
    X(glPolygonOffset)                          \
    X(glReadPixels)                             \
    X(glReleaseShaderCompiler)                  \
    X(glRenderbufferStorage)                    \
    X(glSampleCoverage)                         \
    X(glScissor)                                \
    X(glShaderBinary)                           \
    X(glShaderSource)                           \
    X(glStencilFunc)                            \
    X(glStencilFuncSeparate)                    \
    X(glStencilMask)                            \
    X(glStencilMaskSeparate)                    \
    X(glStencilOp)                              \
    X(glStencilOpSeparate)                      \
    X(glTexImage2D)                             \
    X(glTexParameterf)                          \
    X(glTexParameterfv)                         \
    X(glTexParameteri)                          \
    X(glTexParameteriv)                         \
    X(glTexSubImage2D)                          \
    X(glUniform1f)                              \
    X(glUniform1fv)                             \
    X(glUniform1i)                              \
    X(glUniform1iv)                             \
    X(glUniform2f)                              \
    X(glUniform2fv)                             \
    X(glUniform2i)                              \
    X(glUniform2iv)                             \
    X(glUniform3f)                              \
    X(glUniform3fv)                             \
    X(glUniform3i)                              \
    X(glUniform3iv)                             \
    X(glUniform4f)                              \
    X(glUniform4fv)                             \
    X(glUniform4i)                              \
    X(glUniform4iv)                             \
    X(glUniformMatrix2fv)                       \
    X(glUniformMatrix3fv)                       \
    X(glUniformMatrix4fv)                       \
    X(glUseProgram)                             \
    X(glValidateProgram)                        \
    X(glVertexAttribFloat)                      \
    X(glVertexAttribPointer)                    \
    X(glViewport)                               \
  //

#define GLS_GLES2_EXT_COMMANDS()                \
  EMPTY()                                       \
    X(glMapBufferOES)                           \
    X(glUnmapBufferOES)                         \
  //

#define X(func) GLSC_##func,
enum GL_Server_Command
{
 GLSC_GLS_UNDEF = 0,
 GLS_GLS_COMMANDS()
 GLSC_GLS_LASTCMD = 0x0000ffff,
 GLS_EGL_COMMANDS()
 GLSC_EGL_LASTCMD = 0x0001ffff,
 GLS_GLES2_COMMANDS()
 GLSC_GLES2_LASTCMD = 0x0002ffff,
 GLS_GLES2_EXT_COMMANDS()
};
#undef X

#define X(func) case GLSC_##func: return #func;
static inline const char* GLSC_tostring(enum GL_Server_Command e)
{
  switch(e) {
    GLS_COMMANDS()
  default:
    return "unknown command";
  }
}
#undef X

typedef struct
{
  char *buf;
  size_t size;
  uint32_t ptr;
} gls_buffer_t;


typedef struct
{
  uint32_t cmd;
} gls_command_t;


typedef struct
{
  uint32_t cmd;
} gls_cmd_get_context_t;


typedef struct
{
  uint32_t cmd;
  uint32_t server_version;
  uint32_t screen_width;
  uint32_t screen_height;
} gls_ret_get_context_t;


typedef union
{
  float data_float[GLS_DATA_SIZE];
  uint32_t data_uint[GLS_DATA_SIZE];
  int32_t data_int[GLS_DATA_SIZE];
  char data_char[GLS_DATA_SIZE * 36]; // * 4
} gls_data_t;


typedef struct
{
  uint32_t cmd;
  uint32_t offset;
  uint32_t size;
  int32_t isLast;
  gls_data_t data;
} gls_cmd_send_data_t;

/* Conversation
 *
 * - boolean, enum, unsigned int
 * '-> uint32_t
 */

// EGL command data
typedef struct
{
  int32_t attrib_list[GLS_DATA_SIZE];
} gls_data_egl_attriblist_t;
 
// EGL commands

typedef struct
{
  uint32_t cmd;
  uint32_t api;
} gls_eglBindAPI_t;


typedef struct
{
  uint32_t cmd;
  uint32_t success;
} gls_ret_eglBindAPI_t;


typedef struct
{
  uint32_t cmd;
  uint64_t dpy;
  uint64_t surface;
  uint32_t buffer;
} gls_eglBindTexImage_t;


typedef struct
{
  uint32_t cmd;
  uint32_t success;
} gls_ret_eglBindTexImage_t;


typedef struct
{
  uint32_t cmd;
  uint64_t dpy;
  uint32_t config_size;
  uint32_t has_attribs;
} gls_eglChooseConfig_t;


typedef struct
{
  uint32_t cmd;
  uint64_t dpy;
  uint64_t config;
  uint64_t share_list;
  uint32_t has_attribs;
} gls_eglCreateContext_t;


typedef struct
{
  uint32_t cmd;
  uint64_t context;
} gls_ret_eglCreateContext_t;


typedef struct
{
  uint32_t cmd;
  uint64_t dpy;
  uint64_t config;
  uint32_t has_attribs;
} gls_eglCreatePbufferSurface_t;


typedef struct
{
  uint32_t cmd;
  uint64_t surface;
} gls_ret_eglCreatePbufferSurface_t;


typedef struct
{
  uint32_t cmd;
  uint64_t dpy;
  uint64_t config;
  //uint32_t pixmap; // FIXME
  uint32_t has_attribs;
} gls_eglCreatePixmapSurface_t;


typedef struct
{
  uint32_t cmd;
  uint64_t surface;
} gls_ret_eglCreatePixmapSurface_t;


typedef struct
{
  uint32_t cmd;
  uint64_t dpy;
  uint64_t config;
  uint32_t window;
  uint32_t has_attribs;
} gls_eglCreateWindowSurface_t;


typedef struct
{
  uint32_t cmd;
  uint64_t surface;
} gls_ret_eglCreateWindowSurface_t;


typedef struct
{
  uint32_t cmd;
  uint64_t dpy;
  uint64_t ctx;
} gls_eglDestroyContext_t;


typedef struct
{
  uint32_t cmd;
  uint32_t success;
} gls_ret_eglDestroyContext_t;


typedef struct
{
  uint32_t cmd;
  uint64_t configs[GLS_DATA_SIZE];
  uint32_t num_config;
  uint32_t success;
} gls_ret_eglChooseConfig_t;


typedef struct
{
  uint32_t cmd;
  uint64_t dpy;
  uint64_t surface;
} gls_eglDestroySurface_t;


typedef struct
{
  uint32_t cmd;
  uint32_t success;
} gls_ret_eglDestroySurface_t;


typedef struct
{
  uint32_t cmd;
  uint64_t dpy;
  uint64_t config;
  int32_t attribute;
} gls_eglGetConfigAttrib_t;


typedef struct
{
  uint32_t cmd;
  int32_t value;
  uint32_t success;
} gls_ret_eglGetConfigAttrib_t;


typedef struct
{
  uint32_t cmd;
  uint64_t dpy;
  uint32_t config_size;
} gls_eglGetConfigs_t;


typedef struct
{
  uint32_t cmd;
  uint64_t configs[GLS_DATA_SIZE];
  int32_t num_config;
  uint32_t success;
} gls_ret_eglGetConfigs_t;


typedef struct
{
  uint32_t cmd;
} gls_eglGetCurrentContext_t;


typedef struct
{
  uint32_t cmd;
  uint64_t context;
} gls_ret_eglGetCurrentContext_t;


typedef struct
{
  uint32_t cmd;
} gls_eglGetCurrentDisplay_t;


typedef struct
{
  uint32_t cmd;
  uint64_t display;
} gls_ret_eglGetCurrentDisplay_t;


typedef struct
{
  uint32_t cmd;
  uint32_t readdraw;
} gls_eglGetCurrentSurface_t;


typedef struct
{
  uint32_t cmd;
  uint64_t surface;
} gls_ret_eglGetCurrentSurface_t;


typedef struct
{
  uint32_t cmd;
  uint64_t native_display;
} gls_eglGetDisplay_t;


typedef struct
{
  uint32_t cmd;
  uint64_t display;
} gls_ret_eglGetDisplay_t;


typedef struct
{
  uint32_t cmd;
} gls_eglGetError_t;


typedef struct
{
  uint32_t cmd;
  uint32_t error;
} gls_ret_eglGetError_t;


typedef struct
{
  uint32_t cmd;
  uint64_t dpy;
} gls_eglInitialize_t;


typedef struct
{
  uint32_t cmd;
  uint32_t major;
  uint32_t minor;
  uint32_t success;
} gls_ret_eglInitialize_t;


typedef struct
{
  uint32_t cmd;
  uint64_t dpy;
  uint64_t draw;
  uint64_t read;
  uint64_t ctx;
} gls_eglMakeCurrent_t;


typedef struct
{
  uint32_t cmd;
  uint32_t success;
} gls_ret_eglMakeCurrent_t;


typedef struct
{
  uint32_t cmd;
  uint64_t dpy;
  uint64_t ctx;
  uint32_t attribute;
} gls_eglQueryContext_t;


typedef struct
{
  uint32_t cmd;
  int32_t value;
  uint32_t success;
} gls_ret_eglQueryContext_t;


typedef struct
{
  uint32_t cmd;
  uint64_t dpy;
  uint32_t name;
} gls_eglQueryString_t;


typedef struct
{
  uint32_t cmd;
  uint32_t success;
  char params[GLS_STRING_SIZE_PLUS];
} gls_ret_eglQueryString_t;


typedef struct
{
  uint32_t cmd;
  uint64_t dpy;
  uint64_t surface;
  uint32_t attribute;
} gls_eglQuerySurface_t;


typedef struct
{
  uint32_t cmd;
  int32_t value;
  uint32_t success;
} gls_ret_eglQuerySurface_t;


typedef struct
{
  uint32_t cmd;
  uint64_t dpy;
  uint64_t surface;
  uint32_t buffer;
} gls_eglReleaseTexImage_t;


typedef struct
{
  uint32_t cmd;
  uint32_t success;
} gls_ret_eglReleaseTexImage_t;


typedef struct
{
  uint32_t cmd;
  uint64_t dpy;
  uint64_t surface;
  uint32_t attribute;
  uint32_t value;
} gls_eglSurfaceAttrib_t;


typedef struct
{
  uint32_t cmd;
  uint32_t success;
} gls_ret_eglSurfaceAttrib_t;


typedef struct
{
  uint32_t cmd;
  uint64_t dpy;
  uint64_t draw;
} gls_eglSwapBuffers_t;


typedef struct
{
  uint32_t cmd;
  uint32_t success;
} gls_ret_eglSwapBuffers_t;


typedef struct
{
  uint32_t cmd;
  uint64_t dpy;
} gls_eglTerminate_t;


typedef struct
{
  uint32_t cmd;
  uint32_t success;
} gls_ret_eglTerminate_t;

// OpenGL ES commands
typedef struct
{
  uint32_t cmd;
  uint32_t texture;
} gls_glActiveTexture_t;


typedef struct
{
  uint32_t cmd;
  uint32_t program;
  uint32_t shader;
} gls_glAttachShader_t;


typedef struct
{
  uint32_t cmd;
  uint32_t program;
  uint32_t index;
  char name[GLS_STRING_SIZE_PLUS];
} gls_glBindAttribLocation_t;


typedef struct
{
  uint32_t cmd;
  uint32_t target;
  uint32_t buffer;
} gls_glBindBuffer_t;


typedef struct
{
  uint32_t cmd;
  uint32_t target;
  uint32_t texture;
} gls_glBindTexture_t;


typedef struct
{
  uint32_t cmd;
  uint32_t target;
  uint32_t framebuffer;
} gls_glBindFramebuffer_t;


typedef struct
{
  uint32_t cmd;
  uint32_t srcRGB;
  uint32_t dstRGB;
  uint32_t srcAlpha;
  uint32_t dstAlpha;
} gls_glBlendFuncSeparate_t;


typedef struct
{
  uint32_t cmd;
  uint32_t modeRGB;
  uint32_t modeAlpha;
} gls_glBlendEquationSeparate_t;


typedef struct
{
  uint32_t cmd;
  uint32_t sfactor;
  uint32_t dfactor;
} gls_glBlendFunc_t;


typedef struct
{
  uint32_t cmd;
  uint32_t target;
  int32_t size;
  uint32_t usage;
} gls_glBufferData_t;


typedef struct
{
  uint32_t cmd;
  uint32_t target;
  int32_t offset;
  int32_t size;
} gls_glBufferSubData_t;


typedef struct
{
  uint32_t cmd;
  uint32_t mask;
} gls_glClear_t;


typedef struct
{
  uint32_t cmd;
  float red;
  float green;
  float  blue;
  float alpha;
} gls_glClearColor_t;


typedef struct
{
  uint32_t cmd;
  float depth;
} gls_glClearDepthf_t;


typedef struct
{
  uint32_t cmd;
  uint32_t s;
} gls_glClearStencil_t;


typedef struct
{
  uint32_t cmd;
  uint32_t red;
  uint32_t green;
  uint32_t blue;
  uint32_t alpha;
} gls_glColorMask_t;


typedef struct
{
  uint32_t cmd;
  uint32_t shader;
} gls_glCompileShader_t;


typedef struct
{
  uint32_t cmd;
  uint32_t target;
  int32_t level;
  int32_t xoffset;
  int32_t yoffset;
  int32_t x;
  int32_t y;
  int32_t width;
  int32_t height;
} gls_glCopyTexSubImage2D_t;


typedef struct
{
  uint32_t cmd;
} gls_glCreateProgram_t;


typedef struct
{
  uint32_t cmd;
  uint32_t program;
} gls_ret_glCreateProgram_t;


typedef struct
{
  uint32_t cmd;
  uint32_t type;
} gls_glCreateShader_t;


typedef struct
{
  uint32_t cmd;
  uint32_t obj;
} gls_ret_glCreateShader_t;


typedef struct
{
  uint32_t cmd;
  uint32_t mode;
} gls_glCullFace_t;


typedef struct
{
  uint32_t cmd;
  int32_t n;
} gls_glDeleteBuffers_t;


typedef struct
{
  uint32_t cmd;
  uint32_t program;
} gls_glDeleteProgram_t;


typedef struct
{
  uint32_t cmd;
  uint32_t shader;
} gls_glDeleteShader_t;


typedef struct
{
  uint32_t cmd;
  uint32_t cmd_size;
  int32_t n;
  uint32_t textures[1];
} gls_glDeleteTextures_t;


typedef struct
{
  uint32_t cmd;
  uint32_t func;
} gls_glDepthFunc_t;


typedef struct
{
  uint32_t cmd;
  uint32_t flag;
} gls_glDepthMask_t;


typedef struct
{
  uint32_t cmd;
  uint32_t zNear;
  uint32_t zFar;
} gls_glDepthRangef_t;


typedef struct
{
  uint32_t cmd;
  uint32_t cap;
} gls_glDisable_t;


typedef struct
{
  uint32_t cmd;
  uint32_t index;
} gls_glDisableVertexAttribArray_t;


typedef struct
{
  uint32_t cmd;
  uint32_t mode;
  int32_t first;
  int32_t count;
} gls_glDrawArrays_t;


typedef struct
{
  uint32_t cmd;
  uint32_t mode;
  int32_t count;
  uint32_t type;
} gls_glDrawElements_t;


typedef struct
{
  uint32_t cmd;
  uint32_t cap;
} gls_glEnable_t;


typedef struct
{
  uint32_t cmd;
  uint32_t index;
} gls_glEnableVertexAttribArray_t;


typedef struct
{
  uint32_t cmd;
} gls_glFinish_t;


typedef struct
{
  uint32_t cmd;
} gls_glFlush_t;


typedef struct
{
  uint32_t cmd;
  int32_t n;
} gls_glGenBuffers_t;


typedef struct
{
  uint32_t cmd;
  int32_t n;
} gls_glGenTextures_t;


typedef struct
{
  uint32_t cmd;
  uint32_t program;
  uint32_t index;
  int32_t bufsize;
} gls_glGetActiveAttrib_t;


typedef struct
{
  uint32_t cmd;
  int32_t length;
  int32_t size;
  uint32_t type;
  char name[GLS_STRING_SIZE_PLUS];
} gls_ret_glGetActiveAttrib_t;



typedef struct
{
  uint32_t cmd;
  uint32_t program;
  uint32_t index;
  int32_t bufsize;
} gls_glGetActiveUniform_t;


typedef struct
{
  uint32_t cmd;
  int32_t length;
  int32_t size;
  uint32_t type;
  char name[GLS_STRING_SIZE_PLUS];
} gls_ret_glGetActiveUniform_t;


typedef struct
{
  uint32_t cmd;
  uint32_t program;
  char name[50];
} gls_glGetAttribLocation_t;


typedef struct
{
  uint32_t cmd;
  int32_t index;
} gls_ret_glGetAttribLocation_t;


typedef struct
{
  uint32_t cmd;
} gls_glGetError_t;


typedef struct
{
  uint32_t cmd;
  uint32_t error;
} gls_ret_glGetError_t;


typedef struct
{
  uint32_t cmd;
  uint32_t name;
  int32_t params;
} gls_glGetIntegerv_t;


typedef struct
{
  uint32_t cmd;
  int32_t params;
} gls_ret_glGetIntegerv_t;


typedef struct
{
  uint32_t cmd;
  uint32_t name;
  float params;
} gls_glGetFloatv_t;


typedef struct
{
  uint32_t cmd;
  float params;
} gls_ret_glGetFloatv_t;


typedef struct
{
  uint32_t cmd;
  uint32_t name;
} gls_glGetString_t;


typedef struct
{
  uint32_t cmd;
  uint32_t success;
  unsigned char params[GLS_STRING_SIZE_PLUS];
} gls_ret_glGetString_t;


typedef struct
{
  uint32_t cmd;
  uint32_t program;
  int32_t bufsize;
} gls_glGetProgramInfoLog_t;


typedef struct
{
  uint32_t cmd;
  int32_t length;
  char infolog[GLS_STRING_SIZE_PLUS];
} gls_ret_glGetProgramInfoLog_t;


typedef struct
{
  uint32_t cmd;
  uint32_t program;
  uint32_t pname;
} gls_glGetProgramiv_t;


typedef struct
{
  uint32_t cmd;
  int32_t params;
} gls_ret_glGetProgramiv_t;


typedef struct
{
  uint32_t cmd;
  uint32_t shader;
  uint32_t pname;
} gls_glGetShaderiv_t;


typedef struct
{
  uint32_t cmd;
  int32_t params;
} gls_ret_glGetShaderiv_t;


typedef struct
{
  uint32_t cmd;
  uint32_t shader;
  int32_t bufsize;
} gls_glGetShaderInfoLog_t;


typedef struct
{
  uint32_t cmd;
  int32_t length;
  char infolog[GLS_STRING_SIZE_PLUS];
} gls_ret_glGetShaderInfoLog_t;


typedef struct
{
  uint32_t cmd;
  uint32_t program;
  char name[100];
} gls_glGetUniformLocation_t;


typedef struct
{
  uint32_t cmd;
  int32_t location;
} gls_ret_glGetUniformLocation_t;


typedef struct
{
  uint32_t cmd;
  uint32_t target;
  uint32_t mode;
} gls_glHint_t;


typedef struct
{
  uint32_t cmd;
  uint32_t buffer;
} gls_glIsBuffer_t;

typedef struct
{
  uint32_t cmd;
  uint32_t isbuffer;
} gls_ret_glIsBuffer_t;


typedef struct
{
  uint32_t cmd;
  uint32_t cap;
} gls_glIsEnabled_t;

typedef struct
{
  uint32_t cmd;
  uint32_t isenabled;
} gls_ret_glIsEnabled_t;


typedef struct
{
  uint32_t cmd;
  float width;
} gls_glLineWidth_t;


typedef struct
{
  uint32_t cmd;
  uint32_t program;
} gls_glLinkProgram_t;


typedef struct
{
  uint32_t cmd;
  uint32_t target;
  uint32_t access;
} gls_glMapBufferOES_t;


typedef struct
{
  uint32_t cmd;
  uint32_t pname;
  int32_t param;
} gls_glPixelStorei_t;


typedef struct
{
  uint32_t cmd;
  float factor;
  float units;
} gls_glPolygonOffset_t;


typedef struct
{
  uint32_t cmd;
  int32_t x;
  int32_t y;
  int32_t width;
  int32_t height;
  uint32_t format;
  uint32_t type;
} gls_glReadPixels_t;


typedef struct
{
  uint32_t cmd;
  char pixels[4];
} gls_ret_glReadPixels_t;


typedef struct
{
  uint32_t cmd;
  uint32_t shader;
  int32_t count;
} gls_glShaderSource_t;


typedef struct
{
  uint32_t offsets[GLS_STRING_SIZE_PLUS];
  int32_t length[GLS_STRING_SIZE_PLUS];
  char data[4];
} gls_data_glShaderSource_t;


typedef struct
{
  uint32_t cmd;
  uint32_t func;
  int32_t r;
  uint32_t m;
} gls_glStencilFunc_t;


typedef struct
{
  uint32_t cmd;
  uint32_t fail;
  uint32_t zfail;
  uint32_t zpass;
} gls_glStencilOp_t;


typedef struct
{
  uint32_t cmd;
  uint32_t mask;
} gls_glStencilMask_t;


typedef struct
{
  uint32_t cmd;
  uint32_t target;
  int32_t level;
  int32_t internalformat;
  int32_t width;
  int32_t height;
  int32_t border;
  uint32_t format;
  uint32_t type;
  uint32_t has_pixels;
} gls_glTexImage2D_t;


typedef struct
{
  uint32_t cmd;
  uint32_t target;
  uint32_t pname;
  int32_t param;
} gls_glTexParameteri_t;


typedef struct
{
  uint32_t cmd;
  uint32_t target;
  int32_t level;
  int32_t xoffset;
  int32_t yoffset;
  int32_t width;
  int32_t height;
  uint32_t format;
  uint32_t type;
  uint32_t has_pixels;
} gls_glTexSubImage2D_t;


typedef struct
{
  uint32_t cmd;
  uint32_t cmd_size;
  int32_t location;
  int32_t count;
  float v[];
} gls_glUniformNfv_t;

typedef gls_glUniformNfv_t gls_glUniform1fv_t;
typedef gls_glUniformNfv_t gls_glUniform2fv_t;
typedef gls_glUniformNfv_t gls_glUniform3fv_t;
typedef gls_glUniformNfv_t gls_glUniform4fv_t;


typedef struct
{
  uint32_t cmd;
  uint32_t cmd_size;
  int32_t location;
  int32_t count;
  int32_t v[];
} gls_glUniformNiv_t;

typedef gls_glUniformNiv_t gls_glUniform1iv_t;
typedef gls_glUniformNiv_t gls_glUniform2iv_t;
typedef gls_glUniformNiv_t gls_glUniform3iv_t;
typedef gls_glUniformNiv_t gls_glUniform4iv_t;


typedef struct
{
  uint32_t cmd;
  int32_t location;
  float x;
} gls_glUniform1f_t;


typedef struct
{
  uint32_t cmd;
  int32_t location;
  int32_t x;
} gls_glUniform1i_t;


typedef struct
{
  uint32_t cmd;
  int32_t location;
  float x;
  float y;
} gls_glUniform2f_t;


typedef struct
{
  uint32_t cmd;
  int32_t location;
  int32_t x;
  int32_t y;
} gls_glUniform2i_t;


typedef struct
{
  uint32_t cmd;
  int32_t location;
  float x;
  float y;
  float z;
} gls_glUniform3f_t;


typedef struct
{
  uint32_t cmd;
  int32_t location;
  int32_t x;
  int32_t y;
  int32_t z;
} gls_glUniform3i_t;


typedef struct
{
  uint32_t cmd;
  int32_t location;
  float x;
  float y;
  float z;
  float w;
} gls_glUniform4f_t;


typedef struct
{
  uint32_t cmd;
  int32_t location;
  int32_t x;
  int32_t y;
  int32_t z;
  int32_t w;
} gls_glUniform4i_t;


typedef struct
{
  uint32_t cmd;
  uint32_t cmd_size;
  int32_t location;
  int32_t count;
  uint32_t transpose;
  float value[];
} gls_glUniformMatrixNfv_t;

typedef gls_glUniformMatrixNfv_t gls_glUniformMatrix2fv_t;
typedef gls_glUniformMatrixNfv_t gls_glUniformMatrix3fv_t;
typedef gls_glUniformMatrixNfv_t gls_glUniformMatrix4fv_t;

typedef struct
{
  uint32_t cmd;
  uint32_t target;
} gls_glUnmapBufferOES_t;


typedef struct
{
  uint32_t cmd;
  uint32_t success;
} gls_ret_glUnmapBufferOES_t;


typedef struct
{
  uint32_t cmd;
  uint32_t program;
} gls_glUseProgram_t;


// glVertexAttrib*f and glVertexAttrib*fv combine
typedef struct
{
  uint32_t cmd;
  uint32_t index;
  uint32_t num_float;
  uint32_t call_arr;
} gls_glVertexAttribFloat_t;
typedef struct
{
  float arr[4];
} gls_data_glVertexAttribFloat_t;


typedef struct
{
  uint32_t cmd;
  uint32_t indx;
  int32_t size;
  uint32_t type;
  uint32_t normalized;
  int32_t stride;
  uint64_t ptr_uint;
} gls_glVertexAttribPointer_t;


typedef struct
{
  uint32_t cmd;
  int32_t x;
  int32_t y;
  int32_t width;
  int32_t height;
} gls_glViewport_t;


/*
 * OES / EXT extension commands
 */
typedef struct
{
  uint32_t cmd;
  uint32_t target;
  uint32_t internalformat;
  int32_t width;
  uint32_t format;
  uint32_t type;
  char image[GLS_STRING_SIZE_PLUS];
} gls_glConvolutionFilter1D_t;


/*
typedef struct
{
  uint32_t cmd;
  uint32_t ;
} gls__t;

typedef void             GLvoid;
typedef char             GLchar;
typedef unsigned int     GLenum;
typedef unsigned char    GLboolean;
typedef unsigned int     GLbitfield;
typedef signed   char    GLbyte;
typedef short            GLshort;
typedef int              GLint;
typedef int              GLsizei;
typedef unsigned char    GLubyte;
typedef unsigned short   GLushort;
typedef unsigned int     GLuint;
typedef float            GLfloat;
typedef float            GLclampf;
typedef int32_t          GLfixed;
typedef signed long int  GLintptr;
typedef signed long int  GLsizeiptr;
 */


static inline uint32_t next_ptr(uint32_t ptr, uint32_t size, uint32_t alignment_bits)
{
  uint32_t a = (1 << alignment_bits) - 1;
  return (ptr + size + a) & ~a;
}


