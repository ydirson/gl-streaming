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

#include "gls_command.h"

#include <stdint.h>
#include <stddef.h>

#define GLS_GLES2_MAX_ATTRIBVAR_NAMELEN 128
#define GLS_GLES2_MAX_UNIFORM_NAMELEN 128

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t texture;
} gls_glActiveTexture_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t program;
  uint32_t shader;
} gls_glAttachShader_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t program;
  uint32_t index;
  char name[GLS_GLES2_MAX_ATTRIBVAR_NAMELEN];
} gls_glBindAttribLocation_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t target;
  uint32_t buffer;
} gls_glBindBuffer_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t target;
  uint32_t framebuffer;
} gls_glBindFramebuffer_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t target;
  uint32_t renderbuffer;
} gls_glBindRenderbuffer_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t target;
  uint32_t texture;
} gls_glBindTexture_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t modeRGB;
  uint32_t modeAlpha;
} gls_glBlendEquationSeparate_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t sfactor;
  uint32_t dfactor;
} gls_glBlendFunc_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t srcRGB;
  uint32_t dstRGB;
  uint32_t srcAlpha;
  uint32_t dstAlpha;
} gls_glBlendFuncSeparate_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t target;
  int32_t size;
  uint32_t usage;
  int has_data;
} gls_glBufferData_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t target;
  int32_t offset;
  int32_t size;
} gls_glBufferSubData_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t target;
} gls_glCheckFramebufferStatus_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t status;
} gls_ret_glCheckFramebufferStatus_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t mask;
} gls_glClear_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  float red;
  float green;
  float  blue;
  float alpha;
} gls_glClearColor_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  float depth;
} gls_glClearDepthf_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t s;
} gls_glClearStencil_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t red;
  uint32_t green;
  uint32_t blue;
  uint32_t alpha;
} gls_glColorMask_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t shader;
} gls_glCompileShader_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
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
  GLSCOMMAND_FIELDS();
} gls_glCreateProgram_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t program;
} gls_ret_glCreateProgram_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t type;
} gls_glCreateShader_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t obj;
} gls_ret_glCreateShader_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t mode;
} gls_glCullFace_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  int32_t n;
} gls_glDeleteBuffers_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  int32_t n;
  uint32_t framebuffers[];
} gls_glDeleteFramebuffers_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t program;
} gls_glDeleteProgram_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  int32_t n;
  uint32_t renderbuffers[];
} gls_glDeleteRenderbuffers_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t shader;
} gls_glDeleteShader_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  int32_t n;
  uint32_t textures[];
} gls_glDeleteTextures_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t func;
} gls_glDepthFunc_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t flag;
} gls_glDepthMask_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t zNear;
  uint32_t zFar;
} gls_glDepthRangef_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t cap;
} gls_glDisable_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t index;
} gls_glDisableVertexAttribArray_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t mode;
  int32_t first;
  int32_t count;
} gls_glDrawArrays_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t mode;
  int32_t count;
  uint32_t type;
} gls_glDrawElements_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t cap;
} gls_glEnable_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t index;
} gls_glEnableVertexAttribArray_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
} gls_glFinish_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
} gls_glFlush_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t target;
  uint32_t attachment;
  uint32_t renderbuffertarget;
  uint32_t renderbuffer;
} gls_glFramebufferRenderbuffer_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t target;
  uint32_t attachment;
  uint32_t textarget;
  uint32_t texture;
  uint32_t level;
} gls_glFramebufferTexture2D_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  int32_t n;
} gls_glGenBuffers_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  int32_t target;
} gls_glGenerateMipmap_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  int32_t n;
} gls_glGenFramebuffers_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  int32_t n;
} gls_glGenRenderbuffers_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  int32_t n;
} gls_glGenTextures_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t program;
  uint32_t index;
  int32_t bufsize;
} gls_glGetActiveAttrib_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  int32_t length;
  int32_t size;
  uint32_t type;
  char name[GLS_GLES2_MAX_ATTRIBVAR_NAMELEN];
} gls_ret_glGetActiveAttrib_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t program;
  uint32_t index;
  int32_t bufsize;
} gls_glGetActiveUniform_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  int32_t length;
  int32_t size;
  uint32_t type;
  char name[GLS_GLES2_MAX_UNIFORM_NAMELEN];
} gls_ret_glGetActiveUniform_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t program;
  char name[GLS_GLES2_MAX_ATTRIBVAR_NAMELEN];
} gls_glGetAttribLocation_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  int32_t index;
} gls_ret_glGetAttribLocation_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
} gls_glGetError_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t error;
} gls_ret_glGetError_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t name;
  float params;
} gls_glGetFloatv_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  float params;
} gls_ret_glGetFloatv_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t name;
  int32_t params;
} gls_glGetIntegerv_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  int32_t params;
} gls_ret_glGetIntegerv_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t program;
  uint32_t pname;
} gls_glGetProgramiv_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  int32_t params;
} gls_ret_glGetProgramiv_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t program;
  int32_t bufsize;
} gls_glGetProgramInfoLog_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  int32_t length;
  char infolog[GLS_STRING_SIZE_PLUS];
} gls_ret_glGetProgramInfoLog_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t shader;
  uint32_t pname;
} gls_glGetShaderiv_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  int32_t params;
} gls_ret_glGetShaderiv_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t shader;
  int32_t bufsize;
} gls_glGetShaderInfoLog_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  int32_t length;
  char infolog[GLS_STRING_SIZE_PLUS];
} gls_ret_glGetShaderInfoLog_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t name;
} gls_glGetString_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t success;
  unsigned char params[GLS_STRING_SIZE_PLUS];
} gls_ret_glGetString_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t program;
  char name[100];
} gls_glGetUniformLocation_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  int32_t location;
} gls_ret_glGetUniformLocation_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t target;
  uint32_t mode;
} gls_glHint_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t buffer;
} gls_glIsBuffer_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t isbuffer;
} gls_ret_glIsBuffer_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t cap;
} gls_glIsEnabled_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t isenabled;
} gls_ret_glIsEnabled_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  float width;
} gls_glLineWidth_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t program;
} gls_glLinkProgram_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t pname;
  int32_t param;
} gls_glPixelStorei_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  float factor;
  float units;
} gls_glPolygonOffset_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  int32_t x;
  int32_t y;
  int32_t width;
  int32_t height;
  uint32_t format;
  uint32_t type;
} gls_glReadPixels_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  char pixels[4];
} gls_ret_glReadPixels_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  int32_t target;
  int32_t internalformat;
  int32_t width;
  int32_t height;
} gls_glRenderbufferStorage_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
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
  GLSCOMMAND_FIELDS();
  uint32_t func;
  int32_t r;
  uint32_t m;
} gls_glStencilFunc_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t mask;
} gls_glStencilMask_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t fail;
  uint32_t zfail;
  uint32_t zpass;
} gls_glStencilOp_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
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
  GLSCOMMAND_FIELDS();
  uint32_t target;
  uint32_t pname;
  int32_t param;
} gls_glTexParameteri_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
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
  GLSCOMMAND_FIELDS();
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
  GLSCOMMAND_FIELDS();
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
  GLSCOMMAND_FIELDS();
  int32_t location;
  float x;
} gls_glUniform1f_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  int32_t location;
  int32_t x;
} gls_glUniform1i_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  int32_t location;
  float x;
  float y;
} gls_glUniform2f_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  int32_t location;
  int32_t x;
  int32_t y;
} gls_glUniform2i_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  int32_t location;
  float x;
  float y;
  float z;
} gls_glUniform3f_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  int32_t location;
  int32_t x;
  int32_t y;
  int32_t z;
} gls_glUniform3i_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  int32_t location;
  float x;
  float y;
  float z;
  float w;
} gls_glUniform4f_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  int32_t location;
  int32_t x;
  int32_t y;
  int32_t z;
  int32_t w;
} gls_glUniform4i_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
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
  GLSCOMMAND_FIELDS();
  uint32_t program;
} gls_glUseProgram_t;


// glVertexAttrib*f and glVertexAttrib*fv combine
typedef struct
{
  GLSCOMMAND_FIELDS();
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
  GLSCOMMAND_FIELDS();
  uint32_t indx;
  int32_t size;
  uint32_t type;
  uint32_t normalized;
  int32_t stride;
  uint64_t ptr_uint;
} gls_glVertexAttribPointer_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
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
  GLSCOMMAND_FIELDS();
  uint32_t target;
  uint32_t internalformat;
  int32_t width;
  uint32_t format;
  uint32_t type;
  char image[GLS_STRING_SIZE_PLUS];
} gls_glConvolutionFilter1D_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t target;
  uint32_t access;
} gls_glMapBufferOES_t;


typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t target;
} gls_glUnmapBufferOES_t;

typedef struct
{
  GLSCOMMAND_FIELDS();
  uint32_t success;
} gls_ret_glUnmapBufferOES_t;


/*
typedef struct
{
  GLSCOMMAND_FIELDS();
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
