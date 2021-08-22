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

#include "gls_command_gles2.h"
#include "glserver.h"
#include "fastlog.h"

#include <alloca.h>
#include <string.h>


void glse_glActiveTexture(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glActiveTexture);
  glActiveTexture(c->texture);
}


void glse_glAttachShader(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glAttachShader);
  glAttachShader(c->program, c->shader);
}


void glse_glBindAttribLocation(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glBindAttribLocation);
  glBindAttribLocation (c->program, c->index, (const GLchar*) c->name);
}


void glse_glBindBuffer(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glBindBuffer);
  glBindBuffer(c->target, c->buffer);
}


void glse_glBindFramebuffer(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glBindFramebuffer);
  glBindFramebuffer(c->target, c->framebuffer);
}


void glse_glBindTexture(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glBindTexture);
  glBindTexture(c->target, c->texture);
}


void glse_glBlendEquationSeparate(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glBlendEquationSeparate);
  glBlendEquationSeparate(c->modeRGB, c->modeAlpha);
}


void glse_glBlendFunc(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glBlendFunc);
  glBlendFunc(c->sfactor, c->dfactor);
}


void glse_glBlendFuncSeparate(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glBlendFuncSeparate);
  glBlendFuncSeparate(c->srcRGB, c->dstRGB, c->srcAlpha, c->dstAlpha);
}


void glse_glBufferData(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glBufferData);
  GLSE_SET_RAWDATA_PTR(dat, void, c->has_data);
  glBufferData(c->target, c->size, dat, c->usage);
}


void glse_glBufferSubData(gls_command_t* buf)
{
  GLSE_SET_RAWDATA_PTR(dat, void, 1);
  GLSE_SET_COMMAND_PTR(c, glBufferSubData);
  glBufferSubData(c->target, c->offset, c->size, dat);
}


void glse_glClear(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glClear);
  glClear(c->mask);
}


void glse_glClearColor(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glClearColor);
  glClearColor(c->red, c->green, c->blue, c->alpha);
}


void glse_glClearDepthf(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glClearDepthf);
  glClearDepthf(c->depth);
}


void glse_glColorMask(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glColorMask);
  glColorMask(c->red, c->green, c->blue, c->alpha);
}


void glse_glCompileShader(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glCompileShader);
  glCompileShader(c->shader);
}


void glse_glCopyTexSubImage2D(gls_command_t* buf)
{
    GLSE_SET_COMMAND_PTR(c, glCopyTexSubImage2D);
    glCopyTexSubImage2D(c->target, c->level, c->xoffset, c->yoffset, c->x, c->y, c->width, c->height);
}


void glse_glCreateProgram(gls_command_t* buf)
{
  (void)buf;
  GLuint program = glCreateProgram();
  gls_ret_glCreateProgram_t *ret = (gls_ret_glCreateProgram_t *)glsec_global.pool.tmp_buf.buf;
  ret->cmd = GLSC_glCreateProgram;
  ret->program = program;
  glse_cmd_send_data(sizeof(gls_ret_glCreateProgram_t), (char *)glsec_global.pool.tmp_buf.buf);
}


void glse_glCreateShader(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glCreateShader);
  uint32_t obj = glCreateShader(c->type);
  gls_ret_glCreateShader_t *ret = (gls_ret_glCreateShader_t *)glsec_global.pool.tmp_buf.buf;
  ret->cmd = GLSC_glCreateShader;
  ret->obj = obj;
  glse_cmd_send_data(sizeof(gls_ret_glCreateShader_t), (char *)glsec_global.pool.tmp_buf.buf);
}


void glse_glCullFace(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glCullFace);
  glCullFace(c->mode);
}


void glse_glDeleteBuffers(gls_command_t* buf)
{
  GLSE_SET_RAWDATA_PTR(dat, GLuint, 1);
  GLSE_SET_COMMAND_PTR(c, glDeleteBuffers);
  glDeleteBuffers (c->n, dat);
}


void glse_glDeleteProgram(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glDeleteProgram);
  glDeleteProgram(c->program);
}


void glse_glDeleteShader(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glDeleteShader);
  glDeleteShader(c->shader);
}


void glse_glDeleteTextures(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glDeleteTextures);
  glDeleteTextures(c->n, c->textures);
}


void glse_glDepthFunc(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glDepthFunc);
  glDepthFunc(c->func);
}


void glse_glDepthMask(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glDepthMask);
  glDepthMask(c->flag);
}


void glse_glDepthRangef(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glDepthRangef);
  glDepthRangef(c->zNear, c->zFar);
}


void glse_glDisableVertexAttribArray(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glDisableVertexAttribArray);
  glDisableVertexAttribArray(c->index);
}


void glse_glDisable(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glDisable);
  glDisable(c->cap);
}

void glse_glDrawArrays(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glDrawArrays);
  glDrawArrays(c->mode, c->first, c->count);
}


void glse_glDrawElements(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glDrawElements);
  glDrawElements(c->mode, c->count, c->type, NULL);
}



void glse_glEnable(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glEnable);
  glEnable(c->cap);
}


void glse_glEnableVertexAttribArray(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glEnableVertexAttribArray );
  glEnableVertexAttribArray(c->index);
}


void glse_glFinish(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glFinish);
  glFinish();
  gls_command_t *ret = (gls_command_t *)glsec_global.pool.tmp_buf.buf;
  ret->cmd = c->cmd;
  size_t size = sizeof(gls_command_t);
  glse_cmd_send_data(size, glsec_global.pool.tmp_buf.buf);
}


void glse_glFlush(gls_command_t* buf)
{
  (void)buf;
  glFlush();
}


void glse_glGenBuffers(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glGenBuffers);
  GLuint *ret = (GLuint*)glsec_global.pool.tmp_buf.buf;
  glGenBuffers(c->n, ret);
  uint32_t size = c->n * sizeof(uint32_t);
  glse_cmd_send_data(size, ret);
}


void glse_glGenTextures(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glGenTextures);
  GLuint* ret = (GLuint*)glsec_global.pool.tmp_buf.buf;
  glGenTextures(c->n, ret);
  uint32_t size = c->n * sizeof(uint32_t);
  glse_cmd_send_data(size, ret);
}


void glse_glGetActiveAttrib(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glGetActiveAttrib);
  gls_ret_glGetActiveAttrib_t *ret = (gls_ret_glGetActiveAttrib_t *)glsec_global.pool.tmp_buf.buf;
  glGetActiveAttrib (c->program, c->index, c->bufsize, &ret->length, &ret->size, &ret->type, (GLchar*)ret->name);
  ret->cmd = GLSC_glGetActiveAttrib;
  glse_cmd_send_data(sizeof(gls_ret_glGetActiveAttrib_t), (char *)glsec_global.pool.tmp_buf.buf);
}


void glse_glGetActiveUniform(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glGetActiveUniform);
  gls_ret_glGetActiveUniform_t *ret = (gls_ret_glGetActiveUniform_t *)glsec_global.pool.tmp_buf.buf;
  glGetActiveUniform (c->program, c->index, c->bufsize, &ret->length, &ret->size, &ret->type, (GLchar*)ret->name);
  ret->cmd = GLSC_glGetActiveUniform;
  glse_cmd_send_data(sizeof(gls_ret_glGetActiveUniform_t), (char *)glsec_global.pool.tmp_buf.buf);
}


void glse_glGetAttribLocation(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glGetAttribLocation);
  int index = glGetAttribLocation (c->program, c->name);
  gls_ret_glGetAttribLocation_t *ret = (gls_ret_glGetAttribLocation_t *)glsec_global.pool.tmp_buf.buf;
  ret->cmd = GLSC_glGetAttribLocation;
  ret->index = index;
  glse_cmd_send_data(sizeof(gls_ret_glGetAttribLocation_t), (char *)glsec_global.pool.tmp_buf.buf);
}


void glse_glGetError(gls_command_t* buf)
{
  (void)buf;
  GLuint error = glGetError();
  gls_ret_glGetError_t *ret = (gls_ret_glGetError_t *)glsec_global.pool.tmp_buf.buf;
  ret->cmd = GLSC_glGetError;
  ret->error = error;
  glse_cmd_send_data(sizeof(gls_ret_glGetError_t), (char *)glsec_global.pool.tmp_buf.buf);
}


void glse_glGetFloatv(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glGetFloatv);
  gls_ret_glGetFloatv_t *ret = (gls_ret_glGetFloatv_t *)glsec_global.pool.tmp_buf.buf;
  glGetFloatv(c->name, &ret->params);
  ret->cmd = GLSC_glGetFloatv;
  glse_cmd_send_data(sizeof(gls_ret_glGetFloatv_t),(char *)glsec_global.pool.tmp_buf.buf);
}


void glse_glGetIntegerv(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glGetIntegerv);
  gls_ret_glGetIntegerv_t *ret = (gls_ret_glGetIntegerv_t *)glsec_global.pool.tmp_buf.buf;
  glGetIntegerv(c->name, &ret->params);
  ret->cmd = GLSC_glGetIntegerv;
  glse_cmd_send_data(sizeof(gls_ret_glGetIntegerv_t),(char *)glsec_global.pool.tmp_buf.buf);
}


void glse_glGetProgramInfoLog(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glGetProgramInfoLog);
  gls_ret_glGetProgramInfoLog_t *ret = (gls_ret_glGetProgramInfoLog_t *)glsec_global.pool.tmp_buf.buf;
  int32_t maxsize = GLSE_TMP_BUFFER_SIZE - (uint32_t)((char*)ret->infolog - (char*)ret) - 256;
  if (c->bufsize > maxsize)
  {
    c->bufsize = maxsize;
  }
  glGetProgramInfoLog(c->program, c->bufsize, (GLsizei*)&ret->length, (GLchar*)ret->infolog);
  ret->cmd = GLSC_glGetProgramInfoLog;
  uint32_t size = (uint32_t)((char*)ret->infolog - (char*)ret) + ret->length + 1;
  glse_cmd_send_data(size, (char *)glsec_global.pool.tmp_buf.buf);
}


void glse_glGetProgramiv(gls_command_t* buf)
{
    GLSE_SET_COMMAND_PTR(c, glGetProgramiv);
    gls_ret_glGetProgramiv_t *ret = (gls_ret_glGetProgramiv_t *)glsec_global.pool.tmp_buf.buf;
    glGetProgramiv(c->program, c->pname, &ret->params);
    
    // LOGD("glGetProgramiv(program=%p, pname=%p) -> ptr=%p, value=%p\n", c->program, c->pname, &ret->params, ret->params);
    
    ret->cmd = GLSC_glGetProgramiv;
    glse_cmd_send_data(sizeof(gls_ret_glGetProgramiv_t),(char *)glsec_global.pool.tmp_buf.buf);
}


void glse_glGetShaderInfoLog(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glGetShaderInfoLog);
  gls_ret_glGetShaderInfoLog_t *ret = (gls_ret_glGetShaderInfoLog_t *)glsec_global.pool.tmp_buf.buf;
  int32_t maxsize = GLSE_TMP_BUFFER_SIZE - (uint32_t)((char*)ret->infolog - (char*)ret) - 256;
  if (c->bufsize > maxsize)
  {
    c->bufsize = maxsize;
  }
  glGetShaderInfoLog(c->shader, c->bufsize, (GLsizei*)&ret->length, (GLchar*)ret->infolog);
  ret->cmd = GLSC_glGetShaderInfoLog;
  uint32_t size = (uint32_t)((char*)ret->infolog - (char*)ret) + ret->length + 1;
  glse_cmd_send_data(size, (char *)glsec_global.pool.tmp_buf.buf);
}


void glse_glGetShaderiv(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glGetShaderiv);
  gls_ret_glGetShaderiv_t *ret = (gls_ret_glGetShaderiv_t *)glsec_global.pool.tmp_buf.buf;
  glGetShaderiv(c->shader, c->pname, &ret->params);
  ret->cmd = GLSC_glGetShaderiv;
  glse_cmd_send_data(sizeof(gls_ret_glGetShaderiv_t),(char *)glsec_global.pool.tmp_buf.buf);
}


void glse_glGetString(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glGetString);
  gls_ret_glGetString_t *ret = (gls_ret_glGetString_t *)glsec_global.pool.tmp_buf.buf;
  switch (c->name) {
  case GL_EXTENSIONS:
    // we don't support any right now
    // FIXME later will need to query and filter those we support
    strcpy((char*)ret->params, "GL_GLS_dummy");
    ret->success = TRUE;
    break;
  case GL_VENDOR:
    // Change vendor name to gl-streaming.
    // If want to get hardware vendor, comment out below
    // return "gl-streaming wrapper";
    // break;
  default:
    {
      const unsigned char *params = glGetString(c->name);
      if (params) {
        strncpy((char*)ret->params, (char*)params, GLS_STRING_SIZE);
        ret->success = TRUE;
      } else {
        ret->params[0] = '\0';
        ret->success = FALSE;
      }
    }
  }
  ret->cmd = GLSC_glGetString;
  // LOGD("Client asking for %i, return %s\n", c->name, params);
  // ret->params[GLS_STRING_SIZE_PLUS - 1] = '\0';
  glse_cmd_send_data(sizeof(gls_ret_glGetString_t),(char *)glsec_global.pool.tmp_buf.buf);
}


void glse_glGetUniformLocation(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glGetUniformLocation);
  GLint location = glGetUniformLocation (c->program, (const GLchar*)c->name);
  gls_ret_glGetUniformLocation_t *ret = (gls_ret_glGetUniformLocation_t *)glsec_global.pool.tmp_buf.buf;
  ret->cmd = GLSC_glGetUniformLocation;
  ret->location = location;
  glse_cmd_send_data(sizeof(gls_ret_glGetUniformLocation_t), (char *)glsec_global.pool.tmp_buf.buf);
}


void glse_glHint(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glHint);
  glHint(c->target, c->mode);
}


void glse_glIsBuffer(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glIsBuffer);
  gls_ret_glIsBuffer_t *ret = (gls_ret_glIsBuffer_t *)glsec_global.pool.tmp_buf.buf;
  ret->isbuffer = glIsBuffer(c->buffer);
  glse_cmd_send_data(sizeof(gls_ret_glIsBuffer_t), (char *)glsec_global.pool.tmp_buf.buf);
}


void glse_glIsEnabled(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glIsEnabled);
  gls_ret_glIsEnabled_t *ret = (gls_ret_glIsEnabled_t *)glsec_global.pool.tmp_buf.buf;
  ret->isenabled = glIsEnabled(c->cap);
  glse_cmd_send_data(sizeof(gls_ret_glIsEnabled_t), (char *)glsec_global.pool.tmp_buf.buf);
}


void glse_glLineWidth(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glLineWidth);
  glLineWidth(c->width);
}


void glse_glLinkProgram(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glLinkProgram);
  glLinkProgram(c->program);
}


void glse_glPixelStorei(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glPixelStorei);
  glPixelStorei(c->pname, c->param);
}


void glse_glPolygonOffset(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glPolygonOffset);
  glPolygonOffset(c->factor, c->units);
}


void glse_glReadPixels(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glReadPixels);
  gls_ret_glReadPixels_t *ret = (gls_ret_glReadPixels_t *)glsec_global.pool.tmp_buf.buf;
  glReadPixels(c->x, c->y, c->width, c->height, c->format, c->type, &ret->pixels);
  ret->cmd = GLSC_glReadPixels;
  glse_cmd_send_data(sizeof(gls_ret_glReadPixels_t), (char *)glsec_global.pool.tmp_buf.buf);
  
  // gls_cmd_send_data(0, (uint32_t) (c->width * c->height) /* correct??? */ , (void *)ret->pixels);
}


void glse_glShaderSource(gls_command_t* buf)
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
}


void glse_glStencilFunc(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glStencilFunc);
  glStencilFunc(c->func, c->r, c->m);
}


void glse_glStencilMask(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glStencilMask);
  glStencilMask(c->mask);
}


void glse_glStencilOp(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glStencilOp);
  glStencilOp(c->fail, c->zfail, c->zpass);
}


void glse_glTexParameteri(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glTexParameteri);
  glTexParameteri (c->target, c->pname, c->param);
}


void glse_glTexImage2D(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glTexImage2D);
  GLSE_SET_RAWDATA_PTR(dat, void, c->has_pixels);
  glTexImage2D(c->target, c->level, c->internalformat, c->width, c->height, c->border,
               c->format, c->type, dat);
}


void glse_glTexSubImage2D(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glTexSubImage2D);
  GLSE_SET_RAWDATA_PTR(dat, void, c->has_pixels);
  glTexSubImage2D(c->target, c->level, c->xoffset, c->yoffset, c->width, c->height,
                  c->format, c->type, dat);
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

IMPLEM_glUniformNXv(glUniform1fv,GLfloat);
IMPLEM_glUniformNXv(glUniform2fv,GLfloat);
IMPLEM_glUniformNXv(glUniform3fv,GLfloat);
IMPLEM_glUniformNXv(glUniform4fv,GLfloat);

IMPLEM_glUniformNXv(glUniform1iv,GLint);
IMPLEM_glUniformNXv(glUniform2iv,GLint);
IMPLEM_glUniformNXv(glUniform3iv,GLint);
IMPLEM_glUniformNXv(glUniform4iv,GLint);


#define IMPLEM_glUniformMatrixNXv(FUNC)                                 \
  void glse_##FUNC(gls_command_t* buf)                                  \
  {                                                                     \
  GLSE_SET_COMMAND_PTR(c, FUNC);                                        \
  FUNC(c->location, c->count, c->transpose, (const GLfloat*) c->value); \
  }

IMPLEM_glUniformMatrixNXv(glUniformMatrix2fv);
IMPLEM_glUniformMatrixNXv(glUniformMatrix3fv);
IMPLEM_glUniformMatrixNXv(glUniformMatrix4fv);


void glse_glUseProgram(gls_command_t* buf)
{
    GLSE_SET_COMMAND_PTR(c, glUseProgram);
    glUseProgram(c->program);
}


#define CASE_VTXATTR_FLOAT(INDEX, FLOAT_INDX, ...) case FLOAT_INDX: glVertexAttrib##FLOAT_INDX##f(INDEX, __VA_ARGS__); break
#define CASE_VTXATTR_FLOAT_ARR(INDEX, FLOAT_INDX, ARRAY) case FLOAT_INDX: glVertexAttrib##FLOAT_INDX##fv(INDEX, ARRAY); break
void glse_glVertexAttribFloat(gls_command_t* buf)
{
    GLSE_SET_COMMAND_PTR(c, glVertexAttribFloat);
    GLSE_SET_DATA_PTR(dat, glVertexAttribFloat, 1);

    if (c->call_arr) {
        switch (c->num_float) {
            // FIXME improve this code
            CASE_VTXATTR_FLOAT_ARR(c->index, 1, dat->arr);
            CASE_VTXATTR_FLOAT_ARR(c->index, 2, dat->arr);
            CASE_VTXATTR_FLOAT_ARR(c->index, 3, dat->arr);
            CASE_VTXATTR_FLOAT_ARR(c->index, 4, dat->arr);
        }
    } else {
        switch (c->num_float) {
            // FIXME improve this code
            CASE_VTXATTR_FLOAT(c->index, 1, dat->arr[0]);
            CASE_VTXATTR_FLOAT(c->index, 2, dat->arr[0], dat->arr[1]);
            CASE_VTXATTR_FLOAT(c->index, 3, dat->arr[0], dat->arr[1], dat->arr[2]);
            CASE_VTXATTR_FLOAT(c->index, 4, dat->arr[0], dat->arr[1], dat->arr[2], dat->arr[3]);
        }
    }
}
#undef CASE_VTXATTR_FLOAT
#undef CASE_VTXATTR_FLOAT_ARR


void glse_glVertexAttribPointer(gls_command_t* buf)
{
    GLSE_SET_COMMAND_PTR(c, glVertexAttribPointer);
    glVertexAttribPointer(c->indx, c->size, c->type, c->normalized, c->stride,
                          (void*)c->ptr_uint);
}


void glse_glViewport(gls_command_t* buf)
{
    GLSE_SET_COMMAND_PTR(c, glViewport);
    glViewport(c->x, c->y, c->width, c->height);
}


#if 0
/*
 * OES / EXT extension commands
 */
void glse_glMapBufferOES(gls_command_t* buf)
{
    GLSE_SET_COMMAND_PTR(c, glMapBufferOES);
    glMapBufferOES(c->target, c->access);
}


void glse_glUnmapBufferOES(gls_command_t* buf)
{
  GLSE_SET_COMMAND_PTR(c, glUnmapBufferOES);
  GLboolean success = glUnmapBufferOES(c->target);
  gls_ret_glUnmapBufferOES_t *ret = (gls_ret_glUnmapBufferOES_t *)glsec_global.pool.tmp_buf.buf;
  ret->cmd = GLSC_glUnmapBufferOES;
  ret->success = success;
  glse_cmd_send_data(sizeof(gls_ret_glUnmapBufferOES_t), (char *)glsec_global.pool.tmp_buf.buf);
}
#endif


/*
void glse_()
{
  GLSE_SET_COMMAND_PTR(c, );

}

  gls_ret__t *ret = (gls_ret__t *)glsec_global.pool.tmp_buf.buf;
  ret->cmd = GLSC_;
  ret-> = ;
  glse_cmd_send_data(sizeof(gls_ret__t), (char *)glsec_global.pool.tmp_buf.buf);
*/


#define CASE_EXEC_CMD(FUNCNAME) \
  case GLSC_##FUNCNAME: glse_##FUNCNAME(c); break
#define CASE_BATCH_CMD(FUNCNAME) \
  CASE_EXEC_CMD(FUNCNAME)

int gles_executeCommand(gls_command_t *c) {
#ifdef DEBUG
    LOGD("gles_executeCommand: Executing command %d (%s)\n", c->cmd, GLSC_tostring(c->cmd));
#endif
    switch (c->cmd) {
        CASE_BATCH_CMD(glActiveTexture);
        CASE_BATCH_CMD(glAttachShader);
        CASE_BATCH_CMD(glBindAttribLocation);
        CASE_BATCH_CMD(glBindBuffer);
        CASE_BATCH_CMD(glBindFramebuffer);
        //CASE_BATCH_CMD(glBindRenderbuffer);
        CASE_BATCH_CMD(glBindTexture);
        //CASE_BATCH_CMD(glBlendEquation);
        CASE_BATCH_CMD(glBlendEquationSeparate);
        CASE_BATCH_CMD(glBlendFunc);
        CASE_BATCH_CMD(glBlendFuncSeparate);
        CASE_EXEC_CMD(glBufferData);
        CASE_EXEC_CMD(glBufferSubData);
        //CASE_EXEC_CMD(glCheckFramebufferStatus);
        CASE_BATCH_CMD(glClear);
        CASE_BATCH_CMD(glClearColor);
        CASE_BATCH_CMD(glClearDepthf);
        //CASE_BATCH_CMD(glClearStencil);
        CASE_BATCH_CMD(glColorMask);
        CASE_BATCH_CMD(glCompileShader);
        //CASE_EXEC_CMD(glCompressedTexImage2D);
        //CASE_EXEC_CMD(glCompressedTexSubImage2D);
        //CASE_BATCH_CMD(glCopyTexImage2D);
        CASE_BATCH_CMD(glCopyTexSubImage2D);
        CASE_EXEC_CMD(glCreateProgram);
        CASE_EXEC_CMD(glCreateShader);
        CASE_BATCH_CMD(glCullFace);
        CASE_EXEC_CMD(glDeleteBuffers);
        //CASE_EXEC_CMD(glDeleteFramebuffers);
        CASE_BATCH_CMD(glDeleteProgram);
        //CASE_EXEC_CMD(glDeleteRenderbuffers);
        CASE_BATCH_CMD(glDeleteShader);
        CASE_BATCH_CMD(glDeleteTextures);
        CASE_BATCH_CMD(glDepthFunc);
        CASE_BATCH_CMD(glDepthMask);
        CASE_BATCH_CMD(glDepthRangef);
        //CASE_BATCH_CMD(glDetachShader);
        CASE_BATCH_CMD(glDisable);
        CASE_BATCH_CMD(glDisableVertexAttribArray);
        CASE_BATCH_CMD(glDrawArrays);
        CASE_BATCH_CMD(glDrawElements);
        CASE_BATCH_CMD(glEnable);
        CASE_BATCH_CMD(glEnableVertexAttribArray);
        CASE_EXEC_CMD(glFinish);
        CASE_BATCH_CMD(glFlush);
        //CASE_EXEC_CMD(glFramebufferRenderbuffer);
        //CASE_EXEC_CMD(glFramebufferTexture2D);
        //CASE_EXEC_CMD(glFrontFace);
        CASE_EXEC_CMD(glGenBuffers);
        //CASE_EXEC_CMD(glGenerateMipmap);
        //CASE_EXEC_CMD(glGenFramebuffers);
        //CASE_EXEC_CMD(glGenRenderbuffers);
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
        CASE_EXEC_CMD(glGetShaderInfoLog);
        //CASE_EXEC_CMD(glGetRenderbufferParameteriv);
        CASE_EXEC_CMD(glGetShaderiv);
        //CASE_EXEC_CMD(glGetShaderInfoLog);
        //CASE_EXEC_CMD(glGetShaderPrecisionFormat);
        //CASE_EXEC_CMD(glGetShaderSource);
        CASE_EXEC_CMD(glGetString);
        //CASE_EXEC_CMD(glGetTexParameterfv);
        //CASE_EXEC_CMD(glGetTexParameteriv);
        //CASE_EXEC_CMD(glGetUniformfv);
        //CASE_EXEC_CMD(glGetUniformiv);
        CASE_EXEC_CMD(glGetUniformLocation);
        CASE_BATCH_CMD(glHint);
        CASE_EXEC_CMD(glIsBuffer);
        CASE_EXEC_CMD(glIsEnabled);
        //CASE_EXEC_CMD(glIsFramebuffer);
        //CASE_EXEC_CMD(glIsProgram);
        //CASE_EXEC_CMD(glIsRenderbuffer);
        //CASE_EXEC_CMD(glIsShader);
        //CASE_EXEC_CMD(glIsTexture);
        CASE_BATCH_CMD(glLineWidth);
        CASE_BATCH_CMD(glLinkProgram);
        CASE_BATCH_CMD(glPixelStorei);
        CASE_BATCH_CMD(glPolygonOffset);
        CASE_EXEC_CMD(glReadPixels);
        //CASE_BATCH_CMD(glReleaseShaderCompiler);
        //CASE_BATCH_CMD(glRenderbufferStorage);
        //CASE_BATCH_CMD(glSampleCoverage);
        //CASE_BATCH_CMD(glScissor);
        //CASE_EXEC_CMD(glShaderBinary);
        CASE_EXEC_CMD(glShaderSource);
        CASE_BATCH_CMD(glStencilFunc);
        //CASE_BATCH_CMD(glStencilFuncSeparate);
        CASE_BATCH_CMD(glStencilMask);
        //CASE_BATCH_CMD(glStencilMaskSeparate);
        CASE_BATCH_CMD(glStencilOp);
        //CASE_BATCH_CMD(glStencilOpSeparate);
        CASE_EXEC_CMD(glTexImage2D);
        //CASE_BATCH_CMD(glTexParameterf);
        //CASE_BATCH_CMD(glTexParameterfv);
        CASE_BATCH_CMD(glTexParameteri);
        //CASE_BATCH_CMD(glTexParameteriv);
        CASE_EXEC_CMD(glTexSubImage2D);
        CASE_BATCH_CMD(glUniform1f);
        CASE_BATCH_CMD(glUniform1fv);
        CASE_BATCH_CMD(glUniform1i);
        CASE_BATCH_CMD(glUniform1iv);
        CASE_BATCH_CMD(glUniform2f);
        CASE_BATCH_CMD(glUniform2fv);
        CASE_BATCH_CMD(glUniform2i);
        CASE_BATCH_CMD(glUniform2iv);
        CASE_BATCH_CMD(glUniform3f);
        CASE_BATCH_CMD(glUniform3fv);
        CASE_BATCH_CMD(glUniform3i);
        CASE_BATCH_CMD(glUniform3iv);
        CASE_BATCH_CMD(glUniform4f);
        CASE_BATCH_CMD(glUniform4fv);
        CASE_BATCH_CMD(glUniform4i);
        CASE_BATCH_CMD(glUniform4iv);
        CASE_BATCH_CMD(glUniformMatrix2fv);
        CASE_BATCH_CMD(glUniformMatrix3fv);
        CASE_BATCH_CMD(glUniformMatrix4fv);
        CASE_BATCH_CMD(glUseProgram);
        CASE_EXEC_CMD(glVertexAttribFloat);
        CASE_BATCH_CMD(glVertexAttribPointer);
        CASE_BATCH_CMD(glViewport);

    default:
            return FALSE;
    }
    check_gl_err_cmd(c->cmd);
    return TRUE;
}

#undef CASE_EXEC_CMD
