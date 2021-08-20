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
#include <stddef.h>

#define GLS_DATA_SIZE 356
#define GLS_STRING_SIZE_PLUS 10240 // 256
#define GLS_STRING_SIZE 10160 // 254

// Even versions are stable, should not change a single bit or
// semantic of protocol without bumping version.
// Odd versions are development ones to prepare the next even version,
// change at dev's whim, deploy at your own risk.
#define GLS_VERSION 11

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

// command protocol core

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


// buffer management

#define GLS_TMP_BUFFER_SIZE 2097152
#define GLS_ALIGNMENT_BITS 3

typedef struct
{
  char *buf;
  size_t size;
  uint32_t ptr; // for accumulation, only for batching into tmp_buf
} gls_buffer_t;


// for batching too
static inline uint32_t next_ptr(uint32_t ptr, uint32_t size, uint32_t alignment_bits)
{
  uint32_t a = (1 << alignment_bits) - 1;
  return (ptr + size + a) & ~a;
}
