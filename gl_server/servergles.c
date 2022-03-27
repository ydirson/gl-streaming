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

// This file declare OpenGL ES methods on server side

#define _GNU_SOURCE
#include "gls_command_gles2.h"
#include "glserver.h"
#include "fastlog.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <alloca.h>
#include <string.h>

static const char* GLS_GLES_EXTENSIONS[] = {
  "GL_OES_EGL_image",               // 23
  "GL_OES_depth_texture",           // 43
  "GL_EXT_texture_format_BGRA8888", // 51
  "GL_OES_EGL_image_external",      // 87
  "GL_OES_depth_texture_cube_map",  // 136
  NULL,
};

static struct
{
  // GL_OES_EGL_image
  PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES;
  PFNGLEGLIMAGETARGETRENDERBUFFERSTORAGEOESPROC glEGLImageTargetRenderbufferStorageOES;
} gles_context;

// GLES 2.0

static void glse_glActiveTexture(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glActiveTexture);
  glActiveTexture(c->texture);
}


static void glse_glAttachShader(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glAttachShader);
  glAttachShader(c->program, c->shader);
}


static void glse_glBindAttribLocation(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glBindAttribLocation);
  glBindAttribLocation (c->program, c->index, (const GLchar*) c->name);
}


static void glse_glBindBuffer(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glBindBuffer);
  glBindBuffer(c->target, c->buffer);
}


static void glse_glBindRenderbuffer(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glBindRenderbuffer);
  glBindRenderbuffer(c->target, c->renderbuffer);
}


static void glse_glBindFramebuffer(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glBindFramebuffer);
  glBindFramebuffer(c->target, c->framebuffer);
}


static void glse_glBindTexture(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glBindTexture);
  glBindTexture(c->target, c->texture);
}


static void glse_glBlendEquationSeparate(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glBlendEquationSeparate);
  glBlendEquationSeparate(c->modeRGB, c->modeAlpha);
}


static void glse_glBlendFunc(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glBlendFunc);
  glBlendFunc(c->sfactor, c->dfactor);
}


static void glse_glBlendFuncSeparate(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glBlendFuncSeparate);
  glBlendFuncSeparate(c->srcRGB, c->dstRGB, c->srcAlpha, c->dstAlpha);
}


static void glse_glBufferData(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glBufferData);
  GLSE_SET_RAWDATA_PTR(dat, void, c->has_data);
  glBufferData(c->target, c->size, dat, c->usage);
  GLSE_RELEASE_DATA();
}


static void glse_glBufferSubData(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glBufferSubData);
  GLSE_SET_RAWDATA_PTR(dat, void, c->has_data);
  glBufferSubData(c->target, c->offset, c->size, dat);
  GLSE_RELEASE_DATA();
}


static void glse_glCheckFramebufferStatus(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glCheckFramebufferStatus);
  GLSE_SET_RET_PTR(ret, glCheckFramebufferStatus);
  ret->status = glCheckFramebufferStatus(c->target);
  GLSE_SEND_RET(ret, glCheckFramebufferStatus);
}


static void glse_glClear(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glClear);
  glClear(c->mask);
}


static void glse_glClearColor(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glClearColor);
  glClearColor(c->red, c->green, c->blue, c->alpha);
}


static void glse_glClearDepthf(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glClearDepthf);
  glClearDepthf(c->depth);
}


static void glse_glClearStencil(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glClearStencil);
  glClearStencil(c->s);
}


static void glse_glColorMask(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glColorMask);
  glColorMask(c->red, c->green, c->blue, c->alpha);
}


static void glse_glCompileShader(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glCompileShader);
  glCompileShader(c->shader);
}


static void glse_glCopyTexSubImage2D(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glCopyTexSubImage2D);
  glCopyTexSubImage2D(c->target, c->level, c->xoffset, c->yoffset, c->x, c->y, c->width, c->height);
}


static void glse_glCreateProgram(gls_command_t* buf)
{
  (void)buf;
  GLuint program = glCreateProgram();
  GLSE_SET_RET_PTR(ret, glCreateProgram);
  ret->program = program;
  GLSE_SEND_RET(ret, glCreateProgram);
}


static void glse_glCreateShader(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glCreateShader);
  uint32_t obj = glCreateShader(c->type);
  GLSE_SET_RET_PTR(ret, glCreateShader);
  ret->obj = obj;
  GLSE_SEND_RET(ret, glCreateShader);
}


static void glse_glCullFace(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glCullFace);
  glCullFace(c->mode);
}


static void glse_glDeleteBuffers(gls_command_t* buf)
{
  GLSE_SET_RAWDATA_PTR(dat, GLuint, 1);
  GLSE_SET_COMMAND_PTR(c, glDeleteBuffers);
  glDeleteBuffers (c->n, dat);
  GLSE_RELEASE_DATA();
}


static void glse_glDeleteFramebuffers(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glDeleteFramebuffers);
  glDeleteFramebuffers(c->n, c->framebuffers);
}


static void glse_glDeleteProgram(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glDeleteProgram);
  glDeleteProgram(c->program);
}


static void glse_glDeleteRenderbuffers(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glDeleteRenderbuffers);
  glDeleteRenderbuffers(c->n, c->renderbuffers);
}


static void glse_glDeleteShader(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glDeleteShader);
  glDeleteShader(c->shader);
}


static void glse_glDeleteTextures(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glDeleteTextures);
  glDeleteTextures(c->n, c->textures);
}


static void glse_glDepthFunc(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glDepthFunc);
  glDepthFunc(c->func);
}


static void glse_glDepthMask(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glDepthMask);
  glDepthMask(c->flag);
}


static void glse_glDepthRangef(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glDepthRangef);
  glDepthRangef(c->zNear, c->zFar);
}


static void glse_glDisableVertexAttribArray(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glDisableVertexAttribArray);
  glDisableVertexAttribArray(c->index);
}


static void glse_glDisable(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glDisable);
  glDisable(c->cap);
}

static void glse_glDrawArrays(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glDrawArrays);
  glDrawArrays(c->mode, c->first, c->count);
}


static void glse_glDrawElements(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glDrawElements);
  glDrawElements(c->mode, c->count, c->type, (const GLvoid*)(GLintptr)c->indices);
}



static void glse_glEnable(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glEnable);
  glEnable(c->cap);
}


static void glse_glEnableVertexAttribArray(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glEnableVertexAttribArray );
  glEnableVertexAttribArray(c->index);
}


static void glse_glFinish(gls_command_t* buf)
{
  (void)buf;
  glFinish();
}


static void glse_glFlush(gls_command_t* buf)
{
  (void)buf;
  glFlush();
}


static void glse_glFramebufferRenderbuffer(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glFramebufferRenderbuffer);
  glFramebufferRenderbuffer(c->target, c->attachment, c->renderbuffertarget, c->renderbuffer);
}


static void glse_glFramebufferTexture2D(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glFramebufferTexture2D);
  glFramebufferTexture2D(c->target, c->attachment, c->textarget, c->texture, c->level);
}


static void glse_glGenBuffers(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glGenBuffers);
  GLSE_SET_RAWRET_PTR(ret, GLuint);
  glGenBuffers(c->n, ret);
  GLSE_SEND_RAWRET(ret, c->n * sizeof(uint32_t));
}


static void glse_glGenerateMipmap(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glGenerateMipmap);
  glGenerateMipmap(c->target);
}


static void glse_glGenFramebuffers(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glGenFramebuffers);
  GLSE_SET_RAWRET_PTR(ret, GLuint);
  glGenFramebuffers(c->n, ret);
  GLSE_SEND_RAWRET(ret, c->n * sizeof(uint32_t));
}


static void glse_glGenRenderbuffers(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glGenRenderbuffers);
  GLSE_SET_RAWRET_PTR(ret, GLuint);
  glGenRenderbuffers(c->n, ret);
  GLSE_SEND_RAWRET(ret, c->n * sizeof(uint32_t));
}


static void glse_glGenTextures(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glGenTextures);
  GLSE_SET_RAWRET_PTR(ret, GLuint);
  glGenTextures(c->n, ret);
  GLSE_SEND_RAWRET(ret, c->n * sizeof(uint32_t));
}


static void glse_glGetActiveAttrib(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glGetActiveAttrib);
  GLSE_SET_RET_PTR(ret, glGetActiveAttrib);
  glGetActiveAttrib (c->program, c->index, c->bufsize,
                     &ret->length, &ret->size, &ret->type, (GLchar*)ret->name);
  GLSE_SEND_RET(ret, glGetActiveAttrib);
}


static void glse_glGetActiveUniform(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glGetActiveUniform);
  GLSE_SET_RET_PTR(ret, glGetActiveUniform);
  glGetActiveUniform (c->program, c->index, c->bufsize, &ret->length, &ret->size, &ret->type, (GLchar*)ret->name);
  GLSE_SEND_RET(ret, glGetActiveUniform);
}


static void glse_glGetAttribLocation(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glGetAttribLocation);
  int index = glGetAttribLocation (c->program, c->name);
  GLSE_SET_RET_PTR(ret, glGetAttribLocation);
  ret->index = index;
  GLSE_SEND_RET(ret, glGetAttribLocation);
}


static void glse_glGetError(gls_command_t* buf)
{
  (void)buf;
  GLuint error = glGetError();
  GLSE_SET_RET_PTR(ret, glGetError);
  ret->error = error;
  GLSE_SEND_RET(ret, glGetError);
}


static void glse_glGetFloatv(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glGetFloatv);
  GLSE_SET_RET_PTR(ret, glGetFloatv);
  glGetFloatv(c->name, &ret->params);
  GLSE_SEND_RET(ret, glGetFloatv);
}


static void glse_glGetIntegerv(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glGetIntegerv);
  GLSE_SET_RET_PTR(ret, glGetIntegerv);
  glGetIntegerv(c->name, &ret->params);
  GLSE_SEND_RET(ret, glGetIntegerv);
}


static void glse_glGetProgramInfoLog(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glGetProgramInfoLog);
  GLSE_SET_RET_PTR(ret, glGetProgramInfoLog);
  int32_t maxsize = glsec_global.pool.out_buf.size - sizeof(gls_ret_glGetProgramInfoLog_t);
  if (c->bufsize > maxsize) {
    LOGW("lowering %s buffer size to %u\n",
         __FUNCTION__, maxsize);
    c->bufsize = maxsize;
  }
  _Static_assert(sizeof(GLsizei) == sizeof(ret->length), "int size mismatch");
  glGetProgramInfoLog(c->program, c->bufsize, (GLsizei*)&ret->length, (GLchar*)ret->infolog);
  uint32_t size = sizeof(gls_ret_glGetProgramInfoLog_t) + ret->length + 1;
  GLSE_SEND_RAWRET(ret, size);
}


static void glse_glGetProgramiv(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glGetProgramiv);
  GLSE_SET_RET_PTR(ret, glGetProgramiv);
  glGetProgramiv(c->program, c->pname, &ret->params);
  GLSE_SEND_RET(ret, glGetProgramiv);
}


static void glse_glGetShaderInfoLog(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glGetShaderInfoLog);
  GLSE_SET_RET_PTR(ret, glGetShaderInfoLog);
  int32_t maxsize = glsec_global.pool.out_buf.size - sizeof(gls_ret_glGetShaderInfoLog_t);
  if (c->bufsize > maxsize) {
    LOGW("lowering %s buffer size to %u\n",
         __FUNCTION__, maxsize);
    c->bufsize = maxsize;
  }
  _Static_assert(sizeof(GLsizei) == sizeof(ret->length), "int size mismatch");
  glGetShaderInfoLog(c->shader, c->bufsize, (GLsizei*)&ret->length, (GLchar*)ret->infolog);
  uint32_t size = sizeof(gls_ret_glGetShaderInfoLog_t) + ret->length + 1;
  GLSE_SEND_RAWRET(ret, size);
}


static void glse_glGetShaderiv(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glGetShaderiv);
  GLSE_SET_RET_PTR(ret, glGetShaderiv);
  glGetShaderiv(c->shader, c->pname, &ret->params);
  GLSE_SEND_RET(ret, glGetShaderiv);
}


// we can do complex computations from here, it will be called at most
// once for each string name
static void glse_glGetString(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glGetString);
  GLSE_SET_RET_PTR(ret, glGetString);

  const char* params = (const char*)glGetString(c->name);
  if (params) {
    ret->success = TRUE;
    switch (c->name) {
    case GL_EXTENSIONS: {
        size_t outlen = 0;
        while (1) {
          size_t len = strcspn(params, " ");
          if (!len) break;
          if (glse_extension_supported(GLS_GLES_EXTENSIONS, params, len)) {
            if (outlen + len + 1 > glsec_global.pool.out_buf.size - sizeof(gls_ret_glGetString_t)) {
              LOGW("%s: not enough buffer space for all extensions, truncating\n",
                   __FUNCTION__);
              break;
            }
            strncpy(ret->params + outlen, params, len);
            ret->params[outlen + len] = ' ';
            outlen += len + 1;
          }
          // skip this ext and any sep
          params += len;
          params += strspn(params, " ");
        }
        if (outlen)
          // overwrite last space
          ret->params[outlen - 1] = '\0';
        else
          ret->params[0] = '\0';
        break;
      }
    case GL_VERSION:
      // we just don't support more than 2.0
      strcpy(ret->params, "OpenGL ES 2.0 GLS");
      break;
    case GL_SHADING_LANGUAGE_VERSION:
      // matches GLES 2.0
      strcpy(ret->params, "OpenGL ES GLSL ES 1.00 GLS");
      break;
    case GL_VENDOR:
    default: {
        if (params) {
          strncpy((char*)ret->params, (char*)params, GLS_STRING_SIZE);
        }
      }
    }
  } else {
    ret->params[0] = '\0';
    ret->success = FALSE;
  }

  GLSE_SEND_RET(ret, glGetString);
}


static void glse_glGetUniformLocation(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glGetUniformLocation);
  GLSE_SET_RET_PTR(ret, glGetUniformLocation);
  ret->location = glGetUniformLocation (c->program, (const GLchar*)c->name);
  GLSE_SEND_RET(ret, glGetUniformLocation);
}


static void glse_glHint(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glHint);
  glHint(c->target, c->mode);
}


static void glse_glIsBuffer(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glIsBuffer);
  GLSE_SET_RET_PTR(ret, glIsBuffer);
  ret->isbuffer = glIsBuffer(c->buffer);
  GLSE_SEND_RET(ret, glIsBuffer);
}


static void glse_glIsEnabled(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glIsEnabled);
  GLSE_SET_RET_PTR(ret, glIsEnabled);
  ret->isenabled = glIsEnabled(c->cap);
  GLSE_SEND_RET(ret, glIsEnabled);
}


static void glse_glLineWidth(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glLineWidth);
  glLineWidth(c->width);
}


static void glse_glLinkProgram(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glLinkProgram);
  glLinkProgram(c->program);
}


static void glse_glPixelStorei(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glPixelStorei);
  glPixelStorei(c->pname, c->param);
}


static void glse_glPolygonOffset(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glPolygonOffset);
  glPolygonOffset(c->factor, c->units);
}


static void glse_glReadPixels(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glReadPixels);
  GLSE_SET_RET_PTR(ret, glReadPixels);
  glReadPixels(c->x, c->y, c->width, c->height, c->format, c->type, &ret->pixels);
  GLSE_SEND_RET(ret, glReadPixels);
}


static void glse_glRenderbufferStorage(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glRenderbufferStorage);
  glRenderbufferStorage(c->target, c->internalformat, c->width, c->height);
}


static void glse_glShaderSource(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glShaderSource);
  GLSE_SET_DATA_PTR(dat, glShaderSource, 1);
  int i;
  const GLchar** strings = alloca(c->count * sizeof(GLchar*));
  if (!strings) {
    LOGE("gls error: failed to allocate shader length array for %d items\n", c->count);
    return;
  }

  for (i = 0; i < c->count; i++)
    strings[i] = dat->data + dat->offsets[i];

  if (0) { // Debug: print shader to log
    LOGD("\n ----- BEGIN SHADER CONTENT -----\n");

    for (i = 0; i < c->count; i++) {
      size_t strsize = dat->length[i];
      if (strsize == 0)
        strsize = strlen(strings[i]);
      LOGD("gls debug: shader length = %zd\n", strsize);
      LOGD("%s\n", strings[i]);
    }
    LOGD(" ----- ENDED SHADER CONTENT -----\n");
  }

  glShaderSource(c->shader, c->count, strings, dat->length);
  GLSE_RELEASE_DATA();
}


static void glse_glStencilFunc(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glStencilFunc);
  glStencilFunc(c->func, c->r, c->m);
}


static void glse_glStencilMask(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glStencilMask);
  glStencilMask(c->mask);
}


static void glse_glStencilOp(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glStencilOp);
  glStencilOp(c->fail, c->zfail, c->zpass);
}


static void glse_glTexParameteri(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glTexParameteri);
  glTexParameteri (c->target, c->pname, c->param);
}


static void glse_glTexImage2D(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glTexImage2D);
  GLSE_SET_RAWDATA_PTR(dat, void, c->has_pixels);
  glTexImage2D(c->target, c->level, c->internalformat, c->width, c->height, c->border,
               c->format, c->type, dat);
  GLSE_RELEASE_DATA();
}


static void glse_glTexSubImage2D(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glTexSubImage2D);
  GLSE_SET_RAWDATA_PTR(dat, void, c->has_pixels);
  glTexSubImage2D(c->target, c->level, c->xoffset, c->yoffset, c->width, c->height,
                  c->format, c->type, dat);
  GLSE_RELEASE_DATA();
}


#define IMPLEM_glUniformMatrixNX(FUNC, ...)         \
  void glse_##FUNC(gls_command_t* buf)              \
  {                                                 \
    GLSE_SET_COMMAND_PTR(c, FUNC);                  \
    FUNC(c->location, __VA_ARGS__);                 \
  }

IMPLEM_glUniformMatrixNX(glUniform1f, c->x);
IMPLEM_glUniformMatrixNX(glUniform1i, c->x);
IMPLEM_glUniformMatrixNX(glUniform2f, c->x, c->y);
IMPLEM_glUniformMatrixNX(glUniform2i, c->x, c->y);
IMPLEM_glUniformMatrixNX(glUniform3f, c->x, c->y, c->z);
IMPLEM_glUniformMatrixNX(glUniform3i, c->x, c->y, c->z);
IMPLEM_glUniformMatrixNX(glUniform4f, c->x, c->y, c->z, c->w);
IMPLEM_glUniformMatrixNX(glUniform4i, c->x, c->y, c->z, c->w);

#define IMPLEM_glUniformNXv(FUNC,TYPE)                           \
  void glse_##FUNC(gls_command_t* buf)                           \
  {                                                              \
  GLSE_SET_COMMAND_PTR(c, FUNC);                                 \
  FUNC(c->location, c->count, (const TYPE*) c->v);               \
  }

IMPLEM_glUniformNXv(glUniform1fv, GLfloat);
IMPLEM_glUniformNXv(glUniform2fv, GLfloat);
IMPLEM_glUniformNXv(glUniform3fv, GLfloat);
IMPLEM_glUniformNXv(glUniform4fv, GLfloat);

IMPLEM_glUniformNXv(glUniform1iv, GLint);
IMPLEM_glUniformNXv(glUniform2iv, GLint);
IMPLEM_glUniformNXv(glUniform3iv, GLint);
IMPLEM_glUniformNXv(glUniform4iv, GLint);


#define IMPLEM_glUniformMatrixNXv(FUNC)                                 \
  void glse_##FUNC(gls_command_t* buf)                                  \
  {                                                                     \
  GLSE_SET_COMMAND_PTR(c, FUNC);                                        \
  FUNC(c->location, c->count, c->transpose, (const GLfloat*) c->value); \
  }

IMPLEM_glUniformMatrixNXv(glUniformMatrix2fv);
IMPLEM_glUniformMatrixNXv(glUniformMatrix3fv);
IMPLEM_glUniformMatrixNXv(glUniformMatrix4fv);


static void glse_glUseProgram(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glUseProgram);
  glUseProgram(c->program);
}


static void glse_glVertexAttribFloat(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glVertexAttribFloat);
  GLSE_SET_DATA_PTR(dat, glVertexAttribFloat, 1);

  switch (c->cmd) {
  case GLSC_glVertexAttrib1f:
    glVertexAttrib1f(c->index, dat->arr[0]); break;
  case GLSC_glVertexAttrib2f:
    glVertexAttrib2f(c->index, dat->arr[0], dat->arr[1]); break;
  case GLSC_glVertexAttrib3f:
    glVertexAttrib3f(c->index, dat->arr[0], dat->arr[1], dat->arr[2]); break;
  case GLSC_glVertexAttrib4f:
    glVertexAttrib4f(c->index, dat->arr[0], dat->arr[1], dat->arr[2], dat->arr[3]); break;
  case GLSC_glVertexAttrib1fv:
    glVertexAttrib1fv(c->index, dat->arr); break;
  case GLSC_glVertexAttrib2fv:
    glVertexAttrib2fv(c->index, dat->arr); break;
  case GLSC_glVertexAttrib3fv:
    glVertexAttrib3fv(c->index, dat->arr); break;
  case GLSC_glVertexAttrib4fv:
    glVertexAttrib4fv(c->index, dat->arr); break;
  default:
    LOGE("%s: unsupported command 0x%x (%s)\n", __FUNCTION__,
         c->cmd, GLSC_tostring(c->cmd));
  }

  GLSE_RELEASE_DATA();
}


static void glse_glVertexAttribPointer(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glVertexAttribPointer);
  glVertexAttribPointer(c->indx, c->size, c->type, c->normalized, c->stride,
                        (void*)c->ptr_uint);
}


static void glse_glViewport(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glViewport);
  glViewport(c->x, c->y, c->width, c->height);
}


// GL_OES_EGL_image

static void glse_glEGLImageTargetTexture2DOES(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glEGLImageTargetTexture2DOES);

  if (!gles_context.glEGLImageTargetTexture2DOES) {
    LOGE("%s: no function cached\n", __FUNCTION__);
  }
  gles_context.glEGLImageTargetTexture2DOES(c->target, (GLeglImageOES)c->image);
}

static void glse_glEGLImageTargetRenderbufferStorageOES(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glEGLImageTargetRenderbufferStorageOES);

  if (!gles_context.glEGLImageTargetRenderbufferStorageOES) {
    LOGE("%s: no function cached\n", __FUNCTION__);
  }
  gles_context.glEGLImageTargetRenderbufferStorageOES(c->target, (GLeglImageOES)c->image);
}

// eglGetProcAddress support

void glse_GetGlesProcAddress(const char* procname, void* proc)
{
  if (0) {}
#define X(FUNC)                                 \
  else if (strcmp(procname, #FUNC) == 0)        \
    gles_context.FUNC = proc;                   \
  //
  GLS_GLES2_EXT_COMMANDS()
#undef X
  else {}
}

//

#define CASE_EXEC_CMD(FUNCNAME) \
  case GLSC_##FUNCNAME: glse_##FUNCNAME(c); break

int gles_executeCommand(gls_command_t* c)
{
#ifdef DEBUG
  LOGD("gles_executeCommand: Executing command %d (%s)\n", c->cmd, GLSC_tostring(c->cmd));
#endif
  switch (c->cmd) {
    CASE_EXEC_CMD(glActiveTexture);
    CASE_EXEC_CMD(glAttachShader);
    CASE_EXEC_CMD(glBindAttribLocation);
    CASE_EXEC_CMD(glBindBuffer);
    CASE_EXEC_CMD(glBindFramebuffer);
    CASE_EXEC_CMD(glBindRenderbuffer);
    CASE_EXEC_CMD(glBindTexture);
    //CASE_EXEC_CMD(glBlendColor);
    //CASE_EXEC_CMD(glBlendEquation);
    CASE_EXEC_CMD(glBlendEquationSeparate);
    CASE_EXEC_CMD(glBlendFunc);
    CASE_EXEC_CMD(glBlendFuncSeparate);
    CASE_EXEC_CMD(glBufferData);
    CASE_EXEC_CMD(glBufferSubData);
    CASE_EXEC_CMD(glCheckFramebufferStatus);
    CASE_EXEC_CMD(glClear);
    CASE_EXEC_CMD(glClearColor);
    CASE_EXEC_CMD(glClearDepthf);
    CASE_EXEC_CMD(glClearStencil);
    CASE_EXEC_CMD(glColorMask);
    CASE_EXEC_CMD(glCompileShader);
    //CASE_EXEC_CMD(glCompressedTexImage2D);
    //CASE_EXEC_CMD(glCompressedTexSubImage2D);
    //CASE_EXEC_CMD(glCopyTexImage2D);
    CASE_EXEC_CMD(glCopyTexSubImage2D);
    CASE_EXEC_CMD(glCreateProgram);
    CASE_EXEC_CMD(glCreateShader);
    CASE_EXEC_CMD(glCullFace);
    CASE_EXEC_CMD(glDeleteBuffers);
    CASE_EXEC_CMD(glDeleteFramebuffers);
    CASE_EXEC_CMD(glDeleteProgram);
    CASE_EXEC_CMD(glDeleteRenderbuffers);
    CASE_EXEC_CMD(glDeleteShader);
    CASE_EXEC_CMD(glDeleteTextures);
    CASE_EXEC_CMD(glDepthFunc);
    CASE_EXEC_CMD(glDepthMask);
    CASE_EXEC_CMD(glDepthRangef);
    //CASE_EXEC_CMD(glDetachShader);
    CASE_EXEC_CMD(glDisable);
    CASE_EXEC_CMD(glDisableVertexAttribArray);
    CASE_EXEC_CMD(glDrawArrays);
    CASE_EXEC_CMD(glDrawElements);
    CASE_EXEC_CMD(glEnable);
    CASE_EXEC_CMD(glEnableVertexAttribArray);
    CASE_EXEC_CMD(glFinish);
    CASE_EXEC_CMD(glFlush);
    CASE_EXEC_CMD(glFramebufferRenderbuffer);
    CASE_EXEC_CMD(glFramebufferTexture2D);
    //CASE_EXEC_CMD(glFrontFace);
    CASE_EXEC_CMD(glGenBuffers);
    CASE_EXEC_CMD(glGenerateMipmap);
    CASE_EXEC_CMD(glGenFramebuffers);
    CASE_EXEC_CMD(glGenRenderbuffers);
    CASE_EXEC_CMD(glGenTextures);
    CASE_EXEC_CMD(glGetActiveAttrib);
    CASE_EXEC_CMD(glGetActiveUniform);
    //CASE_EXEC_CMD(glGetAttachedShaders);
    CASE_EXEC_CMD(glGetAttribLocation);
    //CASE_EXEC_CMD(glGetBooleanv);
    //CASE_EXEC_CMD(glGetBufferParameteriv);
    CASE_EXEC_CMD(glGetError);
    CASE_EXEC_CMD(glGetFloatv);
    //CASE_EXEC_CMD(glGetFramebufferAttachmentParameteriv);
    CASE_EXEC_CMD(glGetIntegerv);
    CASE_EXEC_CMD(glGetProgramiv);
    CASE_EXEC_CMD(glGetProgramInfoLog);
    //CASE_EXEC_CMD(glGetRenderbufferParameteriv);
    CASE_EXEC_CMD(glGetShaderiv);
    CASE_EXEC_CMD(glGetShaderInfoLog);
    //CASE_EXEC_CMD(glGetShaderPrecisionFormat);
    //CASE_EXEC_CMD(glGetShaderSource);
    CASE_EXEC_CMD(glGetString);
    //CASE_EXEC_CMD(glGetTexParameterfv);
    //CASE_EXEC_CMD(glGetTexParameteriv);
    //CASE_EXEC_CMD(glGetUniformfv);
    //CASE_EXEC_CMD(glGetUniformiv);
    CASE_EXEC_CMD(glGetUniformLocation);
    //CASE_EXEC_CMD(glGetVertexAttribfv);
    //CASE_EXEC_CMD(glGetVertexAttribiv);
    //CASE_EXEC_CMD(glGetVertexAttribPointerv);
    CASE_EXEC_CMD(glHint);
    CASE_EXEC_CMD(glIsBuffer);
    CASE_EXEC_CMD(glIsEnabled);
    //CASE_EXEC_CMD(glIsFramebuffer);
    //CASE_EXEC_CMD(glIsProgram);
    //CASE_EXEC_CMD(glIsRenderbuffer);
    //CASE_EXEC_CMD(glIsShader);
    //CASE_EXEC_CMD(glIsTexture);
    CASE_EXEC_CMD(glLineWidth);
    CASE_EXEC_CMD(glLinkProgram);
    CASE_EXEC_CMD(glPixelStorei);
    CASE_EXEC_CMD(glPolygonOffset);
    CASE_EXEC_CMD(glReadPixels);
    //CASE_EXEC_CMD(glReleaseShaderCompiler);
    CASE_EXEC_CMD(glRenderbufferStorage);
    //CASE_EXEC_CMD(glSampleCoverage);
    //CASE_EXEC_CMD(glScissor);
    //CASE_EXEC_CMD(glShaderBinary);
    CASE_EXEC_CMD(glShaderSource);
    CASE_EXEC_CMD(glStencilFunc);
    //CASE_EXEC_CMD(glStencilFuncSeparate);
    CASE_EXEC_CMD(glStencilMask);
    //CASE_EXEC_CMD(glStencilMaskSeparate);
    CASE_EXEC_CMD(glStencilOp);
    //CASE_EXEC_CMD(glStencilOpSeparate);
    CASE_EXEC_CMD(glTexImage2D);
    //CASE_EXEC_CMD(glTexParameterf);
    //CASE_EXEC_CMD(glTexParameterfv);
    CASE_EXEC_CMD(glTexParameteri);
    //CASE_EXEC_CMD(glTexParameteriv);
    CASE_EXEC_CMD(glTexSubImage2D);
    CASE_EXEC_CMD(glUniform1f);
    CASE_EXEC_CMD(glUniform1fv);
    CASE_EXEC_CMD(glUniform1i);
    CASE_EXEC_CMD(glUniform1iv);
    CASE_EXEC_CMD(glUniform2f);
    CASE_EXEC_CMD(glUniform2fv);
    CASE_EXEC_CMD(glUniform2i);
    CASE_EXEC_CMD(glUniform2iv);
    CASE_EXEC_CMD(glUniform3f);
    CASE_EXEC_CMD(glUniform3fv);
    CASE_EXEC_CMD(glUniform3i);
    CASE_EXEC_CMD(glUniform3iv);
    CASE_EXEC_CMD(glUniform4f);
    CASE_EXEC_CMD(glUniform4fv);
    CASE_EXEC_CMD(glUniform4i);
    CASE_EXEC_CMD(glUniform4iv);
    CASE_EXEC_CMD(glUniformMatrix2fv);
    CASE_EXEC_CMD(glUniformMatrix3fv);
    CASE_EXEC_CMD(glUniformMatrix4fv);
    CASE_EXEC_CMD(glUseProgram);
  //CASE_EXEC_CMD(glValidateProgram);

  case GLSC_glVertexAttrib1f: glse_glVertexAttribFloat(c); break;
  case GLSC_glVertexAttrib2f: glse_glVertexAttribFloat(c); break;
  case GLSC_glVertexAttrib3f: glse_glVertexAttribFloat(c); break;
  case GLSC_glVertexAttrib4f: glse_glVertexAttribFloat(c); break;
  case GLSC_glVertexAttrib1fv: glse_glVertexAttribFloat(c); break;
  case GLSC_glVertexAttrib2fv: glse_glVertexAttribFloat(c); break;
  case GLSC_glVertexAttrib3fv: glse_glVertexAttribFloat(c); break;
  case GLSC_glVertexAttrib4fv: glse_glVertexAttribFloat(c); break;

    CASE_EXEC_CMD(glVertexAttribPointer);
    CASE_EXEC_CMD(glViewport);

    // GL_OES_EGL_image
    CASE_EXEC_CMD(glEGLImageTargetTexture2DOES);
    CASE_EXEC_CMD(glEGLImageTargetRenderbufferStorageOES);
  default:
    return FALSE;
  }
  check_gl_err_cmd(c->cmd);
  return TRUE;
}

#undef CASE_EXEC_CMD
