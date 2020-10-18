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

#include "glserver.h"


void glse_glActiveTexture()
{
  GLSE_SET_COMMAND_PTR(c, glActiveTexture);
  glActiveTexture(c->texture);
}


void glse_glAttachShader()
{
  GLSE_SET_COMMAND_PTR(c, glAttachShader);
  glAttachShader(c->program, c->shader);
}


void glse_glBindAttribLocation()
{
  GLSE_SET_COMMAND_PTR(c, glBindAttribLocation);
  glBindAttribLocation (c->program, c->index, (const GLchar*) c->name);
}


void glse_glBindBuffer()
{
  GLSE_SET_COMMAND_PTR(c, glBindBuffer);
  glBindBuffer(c->target, c->buffer);
}


void glse_glBindFramebuffer()
{
  GLSE_SET_COMMAND_PTR(c, glBindFramebuffer);
  glBindFramebuffer(c->target, c->framebuffer);
}


void glse_glBindTexture()
{
  GLSE_SET_COMMAND_PTR(c, glBindTexture);
  glBindTexture(c->target, c->texture);
}


void glse_glBlendEquationSeparate()
{
  GLSE_SET_COMMAND_PTR(c, glBlendEquationSeparate);
  glBlendEquationSeparate(c->modeRGB, c->modeAlpha);
}


void glse_glBlendFunc()
{
  GLSE_SET_COMMAND_PTR(c, glBlendFunc);
  glBlendFunc(c->sfactor, c->dfactor);
}


void glse_glBlendFuncSeparate()
{
  GLSE_SET_COMMAND_PTR(c, glBlendFuncSeparate);
  glBlendFuncSeparate(c->srcRGB, c->dstRGB, c->srcAlpha, c->dstAlpha);
}


void glse_glBufferData()
{
  GLSE_SET_COMMAND_PTR(c,glBufferData );
  glBufferData(c->target, c->size, glsec_global.tmp_buf.buf, c->usage);
}


void glse_glBufferSubData()
{
  GLSE_SET_COMMAND_PTR(c, glBufferSubData);
  glBufferSubData(c->target, c->offset, c->size, glsec_global.tmp_buf.buf);
}


void glse_glClear()
{
  GLSE_SET_COMMAND_PTR(c, glClear);
  glClear(c->mask);
}


void glse_glClearColor()
{
  GLSE_SET_COMMAND_PTR(c, glClearColor);
  glClearColor(c->red, c->green, c->blue, c->alpha);
}


void glse_glClearDepthf()
{
  GLSE_SET_COMMAND_PTR(c, glClearDepthf);
  glClearDepthf(c->depth);
}


void glse_glColorMask()
{
  GLSE_SET_COMMAND_PTR(c, glColorMask);
  glColorMask(c->red, c->green, c->blue, c->alpha);
}


void glse_glCompileShader()
{
  GLSE_SET_COMMAND_PTR(c, glCompileShader);
  glCompileShader(c->shader);
}


void glse_glCopyTexSubImage2D()
{
	GLSE_SET_COMMAND_PTR(c, glCopyTexSubImage2D);
	glCopyTexSubImage2D(c->target, c->level, c->xoffset, c->yoffset, c->x, c->y, c->width, c->height);
}


void glse_glCreateProgram()
{
  GLuint program = glCreateProgram();
  gls_ret_glCreateProgram_t *ret = (gls_ret_glCreateProgram_t *)glsec_global.tmp_buf.buf;
  ret->cmd = GLSC_glCreateProgram;
  ret->program = program;
  glse_cmd_send_data(0, sizeof(gls_ret_glCreateProgram_t), (char *)glsec_global.tmp_buf.buf);
}


void glse_glCreateShader()
{
  GLSE_SET_COMMAND_PTR(c, glCreateShader);
  uint32_t obj = glCreateShader(c->type);
  gls_ret_glCreateShader_t *ret = (gls_ret_glCreateShader_t *)glsec_global.tmp_buf.buf;
  ret->cmd = GLSC_glCreateShader;
  ret->obj = obj;
  glse_cmd_send_data(0, sizeof(gls_ret_glCreateShader_t), (char *)glsec_global.tmp_buf.buf);
}


void glse_glCullFace()
{
  GLSE_SET_COMMAND_PTR(c, glCullFace);
  glActiveTexture(c->mode);
}


void glse_glDeleteBuffers()
{
  GLSE_SET_COMMAND_PTR(c, glDeleteBuffers);
  glDeleteBuffers (c->n, (GLuint *)glsec_global.tmp_buf.buf);
}


void glse_glDeleteProgram()
{
  GLSE_SET_COMMAND_PTR(c, glDeleteProgram);
  glDeleteProgram(c->program);
}


void glse_glDeleteShader()
{
  GLSE_SET_COMMAND_PTR(c, glDeleteShader);
  glDeleteShader(c->shader);
}


void glse_glDeleteTextures()
{
  GLSE_SET_COMMAND_PTR(c, glDeleteTextures);
  glDeleteTextures(c->n, c->textures);
}


void glse_glDepthFunc()
{
  GLSE_SET_COMMAND_PTR(c, glDepthFunc);
  glDepthFunc(c->func);
}


void glse_glDepthMask()
{
  GLSE_SET_COMMAND_PTR(c, glDepthMask);
  glDepthMask(c->flag);
}


void glse_glDepthRangef()
{
  GLSE_SET_COMMAND_PTR(c, glDepthRangef);
  glDepthRangef(c->zNear, c->zFar);
}


void glse_glDisableVertexAttribArray()
{
  GLSE_SET_COMMAND_PTR(c, glDisableVertexAttribArray);
  glDisableVertexAttribArray(c->index);
}


void glse_glDisable()
{
  GLSE_SET_COMMAND_PTR(c, glDisable);
  glDisable(c->cap);
}

void glse_glDrawArrays()
{
  GLSE_SET_COMMAND_PTR(c, glDrawArrays);
  glDrawArrays(c->mode, c->first, c->count);
}


void glse_glDrawElements()
{
	GLSE_SET_COMMAND_PTR(c, glDrawElements);
	
	if (c->indices_isnull == TRUE) {
		glDrawElements(c->mode, c->count, c->type, (GLvoid *) c->indices_uint);
	} else {
		glDrawElements (c->mode, c->count, c->type, (const GLvoid*)c->indices);
	}
}



void glse_glEnable()
{
  GLSE_SET_COMMAND_PTR(c, glEnable);
  glEnable(c->cap);
}


void glse_glEnableVertexAttribArray()
{
  GLSE_SET_COMMAND_PTR(c, glEnableVertexAttribArray );
  glEnableVertexAttribArray(c->index);
}


void glse_glFinish()
{
  GLSE_SET_COMMAND_PTR(c, glFinish);
  glFinish();
  gls_command_t *ret = (gls_command_t *)glsec_global.tmp_buf.buf;
  ret->cmd = c->cmd;
  size_t size = sizeof(gls_command_t);
  glse_cmd_send_data(0, size, glsec_global.tmp_buf.buf);
}


void glse_glFlush()
{
  glFlush();
}


void glse_glGenBuffers()
{
  GLSE_SET_COMMAND_PTR(c, glGenBuffers);
  glGenBuffers(c->n, (GLuint*)glsec_global.tmp_buf.buf);
  uint32_t size = c->n * sizeof(uint32_t);
  glse_cmd_send_data(0, size, (char *)glsec_global.tmp_buf.buf);
}


void glse_glGenTextures()
{
  GLSE_SET_COMMAND_PTR(c, glGenTextures);
  glGenTextures(c->n, (GLuint*)glsec_global.tmp_buf.buf);
  uint32_t size = c->n * sizeof(uint32_t);
  glse_cmd_send_data(0, size, (char *)glsec_global.tmp_buf.buf);
}


void glse_glGetActiveAttrib()
{
  GLSE_SET_COMMAND_PTR(c, glGetActiveAttrib);
  gls_ret_glGetActiveAttrib_t *ret = (gls_ret_glGetActiveAttrib_t *)glsec_global.tmp_buf.buf;
  glGetActiveAttrib (c->program, c->index, c->bufsize, &ret->length, &ret->size, &ret->type, (GLchar*)ret->name);
  ret->cmd = GLSC_glGetActiveAttrib;
  glse_cmd_send_data(0, sizeof(gls_ret_glGetActiveAttrib_t), (char *)glsec_global.tmp_buf.buf);
}


void glse_glGetActiveUniform()
{
  GLSE_SET_COMMAND_PTR(c, glGetActiveUniform);
  gls_ret_glGetActiveUniform_t *ret = (gls_ret_glGetActiveUniform_t *)glsec_global.tmp_buf.buf;
  glGetActiveUniform (c->program, c->index, c->bufsize, &ret->length, &ret->size, &ret->type, (GLchar*)ret->name);
  ret->cmd = GLSC_glGetActiveUniform;
  glse_cmd_send_data(0, sizeof(gls_ret_glGetActiveUniform_t), (char *)glsec_global.tmp_buf.buf);
}


void glse_glGetAttribLocation()
{
  GLSE_SET_COMMAND_PTR(c, glGetAttribLocation);
  int index = glGetAttribLocation (c->program, c->name);
  gls_ret_glGetAttribLocation_t *ret = (gls_ret_glGetAttribLocation_t *)glsec_global.tmp_buf.buf;
  ret->cmd = GLSC_glGetAttribLocation;
  ret->index = index;
  glse_cmd_send_data(0, sizeof(gls_ret_glGetAttribLocation_t), (char *)glsec_global.tmp_buf.buf);
}


void glse_glGetError()
{
  GLuint error = glGetError();
  gls_ret_glGetError_t *ret = (gls_ret_glGetError_t *)glsec_global.tmp_buf.buf;
  ret->cmd = GLSC_glGetError;
  ret->error = error;
  glse_cmd_send_data(0, sizeof(gls_ret_glGetError_t), (char *)glsec_global.tmp_buf.buf);
}


void glse_glGetFloatv()
{
  GLSE_SET_COMMAND_PTR(c, glGetFloatv);
  gls_ret_glGetFloatv_t *ret = (gls_ret_glGetFloatv_t *)glsec_global.tmp_buf.buf;
  glGetFloatv(c->name, &ret->params);
  ret->cmd = GLSC_glGetFloatv;
  glse_cmd_send_data(0,sizeof(gls_ret_glGetFloatv_t),(char *)glsec_global.tmp_buf.buf);
}


void glse_glGetIntegerv()
{
  GLSE_SET_COMMAND_PTR(c, glGetIntegerv);
  gls_ret_glGetIntegerv_t *ret = (gls_ret_glGetIntegerv_t *)glsec_global.tmp_buf.buf;
  glGetIntegerv(c->name, &ret->params);
  ret->cmd = GLSC_glGetIntegerv;
  glse_cmd_send_data(0,sizeof(gls_ret_glGetIntegerv_t),(char *)glsec_global.tmp_buf.buf);
}


void glse_glGetProgramInfoLog()
{
  GLSE_SET_COMMAND_PTR(c, glGetProgramInfoLog);
  gls_ret_glGetProgramInfoLog_t *ret = (gls_ret_glGetProgramInfoLog_t *)glsec_global.tmp_buf.buf;
  uint32_t maxsize = GLSE_TMP_BUFFER_SIZE - (uint32_t)((char*)ret->infolog - (char*)ret) - 256;
  if (c->bufsize > maxsize)
  {
    c->bufsize = maxsize;
  }
  glGetProgramInfoLog(c->program, c->bufsize, (GLsizei*)&ret->length, (GLchar*)ret->infolog);
  ret->cmd = GLSC_glGetProgramInfoLog;
  uint32_t size = (uint32_t)((char*)ret->infolog - (char*)ret) + ret->length + 1;
  glse_cmd_send_data(0, size, (char *)glsec_global.tmp_buf.buf);
}


void glse_glGetProgramiv()
{
	GLSE_SET_COMMAND_PTR(c, glGetProgramiv);
	gls_ret_glGetProgramiv_t *ret = (gls_ret_glGetProgramiv_t *)glsec_global.tmp_buf.buf;
	glGetProgramiv(c->program, c->pname, &ret->params);
	
	// LOGD("glGetProgramiv(program=%p, pname=%p) -> ptr=%p, value=%p\n", c->program, c->pname, &ret->params, ret->params);
	
	ret->cmd = GLSC_glGetProgramiv;
	glse_cmd_send_data(0,sizeof(gls_ret_glGetProgramiv_t),(char *)glsec_global.tmp_buf.buf);
}


void glse_glGetShaderInfoLog()
{
  GLSE_SET_COMMAND_PTR(c, glGetShaderInfoLog);
  gls_ret_glGetShaderInfoLog_t *ret = (gls_ret_glGetShaderInfoLog_t *)glsec_global.tmp_buf.buf;
  uint32_t maxsize = GLSE_TMP_BUFFER_SIZE - (uint32_t)((char*)ret->infolog - (char*)ret) - 256;
  if (c->bufsize > maxsize)
  {
    c->bufsize = maxsize;
  }
  glGetShaderInfoLog(c->shader, c->bufsize, (GLsizei*)&ret->length, (GLchar*)ret->infolog);
  ret->cmd = GLSC_glGetShaderInfoLog;
  uint32_t size = (uint32_t)((char*)ret->infolog - (char*)ret) + ret->length + 1;
  glse_cmd_send_data(0, size, (char *)glsec_global.tmp_buf.buf);
}


void glse_glGetShaderiv()
{
  GLSE_SET_COMMAND_PTR(c, glGetShaderiv);
  gls_ret_glGetShaderiv_t *ret = (gls_ret_glGetShaderiv_t *)glsec_global.tmp_buf.buf;
  glGetShaderiv(c->shader, c->pname, &ret->params);
  ret->cmd = GLSC_glGetShaderiv;
  glse_cmd_send_data(0,sizeof(gls_ret_glGetShaderiv_t),(char *)glsec_global.tmp_buf.buf);
}


void glse_glGetString()
{
  GLSE_SET_COMMAND_PTR(c, glGetString);
  gls_ret_glGetString_t *ret = (gls_ret_glGetString_t *)glsec_global.tmp_buf.buf;
  const char *params = glGetString(c->name);
  ret->cmd = GLSC_glGetString;
  // LOGD("Client asking for %i, return %s", c->name, params);
  // ret->params[GLS_STRING_SIZE_PLUS - 1] = '\0';
  strncpy(ret->params, params, GLS_STRING_SIZE);
  glse_cmd_send_data(0,sizeof(gls_ret_glGetString_t),(char *)glsec_global.tmp_buf.buf);
}


void glse_glGetUniformLocation()
{
  GLSE_SET_COMMAND_PTR(c, glGetUniformLocation);
  GLint location = glGetUniformLocation (c->program, (const GLchar*)c->name);
  gls_ret_glGetUniformLocation_t *ret = (gls_ret_glGetUniformLocation_t *)glsec_global.tmp_buf.buf;
  ret->cmd = GLSC_glGetUniformLocation;
  ret->location = location;
  glse_cmd_send_data(0, sizeof(gls_ret_glGetUniformLocation_t), (char *)glsec_global.tmp_buf.buf);
}


void glse_glHint()
{
  GLSE_SET_COMMAND_PTR(c, glHint);
  glHint(c->target, c->mode);
}


void glse_glLineWidth()
{
  GLSE_SET_COMMAND_PTR(c, glLineWidth);
  glLineWidth(c->width);
}


void glse_glLinkProgram()
{
  GLSE_SET_COMMAND_PTR(c, glLinkProgram);
  glLinkProgram(c->program);
}


void glse_glPixelStorei()
{
  GLSE_SET_COMMAND_PTR(c, glPixelStorei);
  glPixelStorei(c->pname, c->param);
}


void glse_glPolygonOffset()
{
  GLSE_SET_COMMAND_PTR(c, glPolygonOffset);
  glPolygonOffset(c->factor, c->units);
}


void glse_glReadPixels()
{
  GLSE_SET_COMMAND_PTR(c, glReadPixels);
  gls_ret_glReadPixels_t *ret = (gls_ret_glReadPixels_t *)glsec_global.tmp_buf.buf;
  glReadPixels(c->x, c->y, c->width, c->height, c->format, c->type, &ret->pixels);
  ret->cmd = glReadPixels;
  glse_cmd_send_data(0, sizeof(gls_ret_glReadPixels_t), (char *)glsec_global.tmp_buf.buf);
  
  // gls_cmd_send_data(0, (uint32_t) (c->width * c->height) /* correct??? */ , (void *)ret->pixels);
}


void glse_glShaderSource()
{
  GLSE_SET_COMMAND_PTR(c, glShaderSource);
  gls_data_glShaderSource_t *dat = (gls_data_glShaderSource_t *)glsec_global.tmp_buf.buf;
  unsigned int i;
  for (i = 0; i < c->count; i++)
  {
    dat->string[i] = (uint32_t)(dat->data + dat->string[i]);
  }
  
  glShaderSource(c->shader, c->count, (const GLchar**)dat->string, dat->length);
  
  // Debug: print shader to log
/*
  LOGD("\n ----- BEGIN SHADER CONTENT -----");
  
  size_t size_all = (size_t)(dat->data - (char *)dat);
  uint32_t stroffset = 0;
  
  // unsigned int i;
  for (i = 0; i < c->count; i++)
  {
    char *strptr = (char *)dat->string[i];
    size_t strsize;
    if (dat->length == NULL)
    {
      strsize = 0;
    }
    else
    {
      strsize = dat->length[i];
    }
    if (strsize == 0)
    {
      strsize = strnlen(strptr, 0xA00000);
    }
    if (strsize > 0x100000)
    {
      return;
    }
    size_all += strsize + 1;
    if (size_all > GLS_TMP_BUFFER_SIZE)
    {
      return;
    }
    stroffset = stroffset + strsize + 1;
	LOGD("gls debug: shader length = %i", strsize);
	LOGD("%s", strptr);
  }
  LOGD(" ----- ENDED SHADER CONTENT -----\n");
*/
}


void glse_glStencilFunc()
{
  GLSE_SET_COMMAND_PTR(c, glStencilFunc);
  glStencilFunc(c->func, c->r, c->m);
}


void glse_glStencilMask()
{
  GLSE_SET_COMMAND_PTR(c, glStencilMask);
  glStencilMask(c->mask);
}


void glse_glStencilOp()
{
  GLSE_SET_COMMAND_PTR(c, glStencilOp);
  glStencilOp(c->fail, c->zfail, c->zpass);
}


void glse_glTexParameteri()
{
  GLSE_SET_COMMAND_PTR(c, glTexParameteri);
  glTexParameteri (c->target, c->pname, c->param);
}


void glse_glTexImage2D()
{
  GLSE_SET_COMMAND_PTR(c, glTexImage2D);
/*
  int i;
  int *pixInt = (int *) c->pixels;
  for (i = 0; i < c->width; i++) {
	  LOGD("Pixel[%i]=%p", i, pixInt[i]);
  }
*/
  glTexImage2D(c->target, c->level, c->internalformat, c->width, c->height, c->border, c->format, c->type, c->pixels_isnull ? NULL : c->pixels);
}


void glse_glTexSubImage2D()
{
  GLSE_SET_COMMAND_PTR(c, glTexSubImage2D);
  glTexSubImage2D(c->target, c->level, c->xoffset, c->yoffset, c->width, c->height, c->format, c->type, c->pixels_isnull ? NULL : c->pixels);
}


void glse_glUniform1f()
{
  GLSE_SET_COMMAND_PTR(c, glUniform1f);
  glUniform1f(c->location, c->x);
}


void glse_glUniform4fv()
{
  GLSE_SET_COMMAND_PTR(c, glUniform4fv);
  glUniform4fv (c->location, c->count, (const GLfloat*) c->v);
}


void glse_glUniformMatrix4fv()
{
  GLSE_SET_COMMAND_PTR(c, glUniformMatrix4fv);
  glUniformMatrix4fv(c->location, c->count, c->transpose, (const GLfloat*) c->value);
}


void glse_glUseProgram()
{
	GLSE_SET_COMMAND_PTR(c, glUseProgram);
	glUseProgram(c->program);
}


#define CASE_VTXATTR_FLOAT(INDEX, FLOAT_INDX, ...) case FLOAT_INDX: glVertexAttrib##FLOAT_INDX##f(INDEX, __VA_ARGS__); break;
#define CASE_VTXATTR_FLOAT_ARR(INDEX, FLOAT_INDX, ARRAY) glVertexAttrib##FLOAT_INDX##fv(INDEX, ARRAY);
void glse_glVertexAttribFloat()
{
	GLSE_SET_COMMAND_PTR(c, glVertexAttribFloat);
	gls_data_glVertexAttribFloat_t *dat = (gls_data_glVertexAttribFloat_t *)glsec_global.tmp_buf.buf;

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


void glse_glVertexAttribPointer()
{
	GLSE_SET_COMMAND_PTR(c, glVertexAttribPointer);

	int ptr_str_len = strnlen(c->ptr, 0xA00000);
	// LOGD("PTR Str len = %i", ptr_str_len);
	
	glVertexAttribPointer(c->indx, c->size, c->type, c->normalized, c->stride, ((const GLvoid *) c->ptr_isnull == TRUE ? c->ptr_uint : c->ptr));
}


void glse_glViewport()
{
	GLSE_SET_COMMAND_PTR(c, glViewport);
	glViewport(c->x, c->y, c->width, c->height);
}

/*
void glse_glIsEnabled()
{
	// GLboolean glIsEnabled(	GLenum cap);
	
	
}
*/


/*
 * OES / EXT extension commands
 */
void glse_glMapBufferOES()
{
	GLSE_SET_COMMAND_PTR(c, glMapBufferOES);
	glMapBufferOES(c->target, c->access);
}


void glse_glUnmapBufferOES()
{
  GLSE_SET_COMMAND_PTR(c, glUnmapBufferOES);
  GLboolean success = glUnmapBufferOES(c->target);
  gls_ret_glUnmapBufferOES_t *ret = (gls_ret_glUnmapBufferOES_t *)glsec_global.tmp_buf.buf;
  ret->cmd = GLSC_glUnmapBufferOES;
  ret->success = success;
  glse_cmd_send_data(0, sizeof(gls_ret_glUnmapBufferOES_t), (char *)glsec_global.tmp_buf.buf);
}


/*
void glse_()
{
  GLSE_SET_COMMAND_PTR(c, );

}

  gls_ret__t *ret = (gls_ret__t *)glsec_global.tmp_buf.buf;
  ret->cmd = GLSC_;
  ret-> = ;
  glse_cmd_send_data(0, sizeof(gls_ret__t), (char *)glsec_global.tmp_buf.buf);
*/


int gles_flushCommand(gls_command_t *c) {
	// LOGD("Flushing command %i\n", c->cmd);
	switch (c->cmd) {
      case GLSC_glAttachShader:
        glse_glAttachShader();
        pop_batch_command(sizeof(gls_glAttachShader_t));
        break;
      case GLSC_glActiveTexture:
        glse_glActiveTexture();
        pop_batch_command(sizeof(gls_glActiveTexture_t));
        break;
      case GLSC_glBindBuffer:
        glse_glBindBuffer();
        pop_batch_command(sizeof(gls_glBindBuffer_t));
        break;
      case GLSC_glBindTexture:
        glse_glBindTexture();
        pop_batch_command(sizeof(gls_glBindTexture_t));
        break;
      case GLSC_glBindAttribLocation:
        glse_glBindAttribLocation();
        pop_batch_command(sizeof(gls_glBindAttribLocation_t));
        break;
      case GLSC_glBindFramebuffer:
        glse_glBindFramebuffer();
        pop_batch_command(sizeof(gls_glBindFramebuffer_t));
        break;
      case GLSC_glBlendFuncSeparate:
        glse_glBlendFuncSeparate();
        pop_batch_command(sizeof(gls_glBlendFuncSeparate_t));
        break;
      case GLSC_glBlendEquationSeparate:
        glse_glBlendEquationSeparate();
        pop_batch_command(sizeof(gls_glBlendEquationSeparate_t));
        break;
      case GLSC_glClear:
        glse_glClear();
        pop_batch_command(sizeof(gls_glClear_t));
        break;
      case GLSC_glClearColor:
        glse_glClearColor();
        pop_batch_command(sizeof(gls_glClearColor_t));
        break;
      case GLSC_glColorMask:
        glse_glColorMask();
        pop_batch_command(sizeof(gls_glColorMask_t));
        break;
      case GLSC_glCompileShader:
        glse_glCompileShader();
        pop_batch_command(sizeof(gls_glCompileShader_t));
        break;
      case GLSC_glDeleteProgram:
        glse_glDeleteProgram();
        pop_batch_command(sizeof(gls_glDeleteProgram_t));
        break;
      case GLSC_glDeleteShader:
        glse_glDeleteShader();
        pop_batch_command(sizeof(gls_glDeleteShader_t));
        break;
      case GLSC_glDeleteTextures:
        glse_glDeleteTextures();
        pop_batch_command(((gls_glDeleteTextures_t *)c)->cmd_size);
        break;
      case GLSC_glDepthFunc:
        glse_glDepthFunc();
        pop_batch_command(sizeof(gls_glDepthFunc_t));
        break;
      case GLSC_glDisable:
        glse_glDisable();
        pop_batch_command(sizeof(gls_glDisable_t));
        break;
      case GLSC_glDisableVertexAttribArray:
        glse_glDisableVertexAttribArray();
        pop_batch_command(sizeof(gls_glDisableVertexAttribArray_t));
        break;
      case GLSC_glDrawArrays:
        glse_glDrawArrays();
        pop_batch_command(sizeof(gls_glDrawArrays_t));
        break;
      case GLSC_glDrawElements:
        glse_glDrawElements();
        pop_batch_command(((gls_glDrawElements_t *)c)->cmd_size);
        break;
      case GLSC_glEnable:
        glse_glEnable();
        pop_batch_command(sizeof(gls_glEnable_t));
        break;
      case GLSC_glEnableVertexAttribArray:
        glse_glEnableVertexAttribArray();
        pop_batch_command(sizeof(gls_glEnableVertexAttribArray_t));
        break;
      case GLSC_glFlush:
        glse_glFlush();
        pop_batch_command(sizeof(gls_command_t));
        break;
      case GLSC_glLinkProgram:
        glse_glLinkProgram();
        pop_batch_command(sizeof(gls_glLinkProgram_t));
        break;
      case GLSC_glPixelStorei:
        glse_glPixelStorei();
        pop_batch_command(sizeof(gls_glPixelStorei_t));
        break;
      case GLSC_glTexImage2D:
        glse_glTexImage2D();
        pop_batch_command(((gls_glTexImage2D_t *)c)->cmd_size);
        break;
      case GLSC_glTexParameteri:
        glse_glTexParameteri();
        pop_batch_command(sizeof(gls_glTexParameteri_t));
        break;
      case GLSC_glUniform1f:
        glse_glUniform1f();
        pop_batch_command(sizeof(gls_glUniform1f_t));
        break;
      case GLSC_glUniform4fv:
        glse_glUniform4fv();
        pop_batch_command(((gls_glUniform4fv_t *)c)->cmd_size);
        break;
      case GLSC_glUniformMatrix4fv:
        glse_glUniformMatrix4fv();
        pop_batch_command(((gls_glUniformMatrix4fv_t *)c)->cmd_size);
        break;
      case GLSC_glUseProgram:
        glse_glUseProgram();
        pop_batch_command(sizeof(gls_glUseProgram_t));
        break;
      case GLSC_glVertexAttribPointer:
        glse_glVertexAttribPointer();
        pop_batch_command(sizeof(gls_glVertexAttribPointer_t));
        break;
      case GLSC_glViewport:
        glse_glViewport();
        pop_batch_command(sizeof(gls_glViewport_t));
        break;
		
      case GLSC_glBlendFunc:
        glse_glBlendFunc();
        pop_batch_command(sizeof(gls_glBlendFunc_t));
        break;
      case GLSC_glClearDepthf:
        glse_glClearDepthf();
        pop_batch_command(sizeof(gls_glClearDepthf_t));
        break;
	  case GLSC_glCopyTexSubImage2D:
		glse_glCopyTexSubImage2D();
        pop_batch_command(sizeof(gls_glCopyTexSubImage2D_t));
		break;
      case GLSC_glCullFace:
        glse_glCullFace();
        pop_batch_command(sizeof(gls_glCullFace_t));
        break;
      case GLSC_glDepthMask:
        glse_glDepthMask();
        pop_batch_command(sizeof(gls_glDepthMask_t));
        break;
      case GLSC_glDepthRangef:
        glse_glDepthRangef();
        pop_batch_command(sizeof(gls_glDepthRangef_t));
        break;
      case GLSC_glHint:
        glse_glHint();
        pop_batch_command(sizeof(gls_glHint_t));
        break;
      case GLSC_glLineWidth:
        glse_glLineWidth();
        pop_batch_command(sizeof(gls_glLineWidth_t));
        break;
	  case GLSC_glMapBufferOES:
        glse_glMapBufferOES();
        pop_batch_command(sizeof(gls_glMapBufferOES_t));
        break;
      case GLSC_glPolygonOffset:
        glse_glPolygonOffset();
        pop_batch_command(sizeof(gls_glPolygonOffset_t));
        break;
      case GLSC_glStencilFunc:
        glse_glStencilFunc();
        pop_batch_command(sizeof(gls_glStencilFunc_t));
        break;
      case GLSC_glStencilMask:
        glse_glStencilMask();
        pop_batch_command(sizeof(gls_glStencilMask_t));
        break;
      case GLSC_glStencilOp:
        glse_glStencilOp();
        pop_batch_command(sizeof(gls_glStencilOp_t));
        break;
	  case GLSC_glTexSubImage2D:
		glse_glTexSubImage2D();
        pop_batch_command(((gls_glTexSubImage2D_t *)c)->cmd_size);
		break;
		
/*
      case GLSC_glXXX:
        glse_glXXX();
        pop_batch_command(sizeof(gls_glXXX_t));
        break;
*/
	  default:
	  	return FALSE;
	}
	check_gl_err(c->cmd);
	return TRUE;
}

// FIXME macro below is buggy
/*
#define CASE_FLUSH_CMD(FUNCNAME) case GLSC_##FUNCNAME: { glse_##FUNCNAME(); pop_batch_command(sizeof(gls_##FUNCNAME##_t)); break; }
#define CASE_FLUSH_CMD_SIZE(FUNCNAME) case GLSC_##FUNCNAME: { glse_##FUNCNAME(); pop_batch_command(((gls_##FUNCNAME##_t *)c)->cmd_size); break; }
int gles_flushCommand(gls_command_t *c) {
	// LOGD("Flushing command %i\n", c->cmd);
	switch (c->cmd) {
		CASE_FLUSH_CMD(glAttachShader);
		CASE_FLUSH_CMD(glActiveTexture);
		CASE_FLUSH_CMD(glBindBuffer);
		CASE_FLUSH_CMD(glBindTexture);
		CASE_FLUSH_CMD(glBindAttribLocation);
		CASE_FLUSH_CMD(glBindFramebuffer);
		CASE_FLUSH_CMD(glBlendFunc);
		CASE_FLUSH_CMD(glBlendFuncSeparate);
		CASE_FLUSH_CMD(glBlendEquationSeparate);
		CASE_FLUSH_CMD(glClear);
		CASE_FLUSH_CMD(glClearColor);
		CASE_FLUSH_CMD(glClearDepthf);
		CASE_FLUSH_CMD(glColorMask);
		CASE_FLUSH_CMD(glCompileShader);
		CASE_FLUSH_CMD(glCopyTexSubImage2D);
		CASE_FLUSH_CMD(glCullFace);
		CASE_FLUSH_CMD(glDeleteProgram);
		CASE_FLUSH_CMD(glDeleteShader);
		CASE_FLUSH_CMD(glDeleteTextures);
		CASE_FLUSH_CMD(glDepthFunc);
		CASE_FLUSH_CMD(glDepthMask);
		CASE_FLUSH_CMD(glDepthRangef);
		CASE_FLUSH_CMD(glDisable);
		CASE_FLUSH_CMD(glDisableVertexAttribArray);
		CASE_FLUSH_CMD(glDrawArrays);
		CASE_FLUSH_CMD(glDrawElements);
		CASE_FLUSH_CMD(glEnable);
		CASE_FLUSH_CMD(glEnableVertexAttribArray);
		CASE_FLUSH_CMD(glFlush);
		CASE_FLUSH_CMD(glHint);
		CASE_FLUSH_CMD(glLineWidth);
		CASE_FLUSH_CMD(glLinkProgram);
		CASE_FLUSH_CMD(glPixelStorei);
		CASE_FLUSH_CMD(glPolygonOffset);
		CASE_FLUSH_CMD(glStencilFunc);
		CASE_FLUSH_CMD(glStencilOp);
		CASE_FLUSH_CMD_SIZE(glTexImage2D);
		CASE_FLUSH_CMD_SIZE(glTexSubImage2D);
		CASE_FLUSH_CMD(glTexParameteri);
		CASE_FLUSH_CMD(glUniform1f);
		CASE_FLUSH_CMD(glUniform4fv);
		CASE_FLUSH_CMD(glUniformMatrix4fv);
		CASE_FLUSH_CMD(glUseProgram);
		CASE_FLUSH_CMD(glVertexAttribPointer);
		CASE_FLUSH_CMD(glViewport);
		CASE_FLUSH_CMD(glMapBufferOES);
		
//		CASE_FLUSH_CMD(glCommand);
	  default:
	  	return FALSE;
	}
	check_gl_err(c->cmd);
	return TRUE;
}
#undef CASE_FLUSH_CMD_SIZE
#undef CASE_FLUSH_CMD
*/


int gles_executeCommand(gls_command_t *c) {
	// LOGD("Executing command %i\n", c->cmd);
	switch (c->cmd) {
		case GLSC_glBufferData:
			glse_glBufferData();
			break;
        case GLSC_glBufferSubData:
			glse_glBufferSubData();
			break;
        case GLSC_glCreateProgram:
			glse_glCreateProgram();
			break;
        case GLSC_glCreateShader:
			glse_glCreateShader();
			break;
        case GLSC_glDeleteBuffers:
			glse_glDeleteBuffers();
			break;
        case GLSC_glFinish:
			glse_glFinish();
			break;
        case GLSC_glGenBuffers:
			glse_glGenBuffers();
			break;
        case GLSC_glGenTextures:
			glse_glGenTextures();
			break;
        case GLSC_glGetActiveAttrib:
			glse_glGetActiveAttrib();
			break;
        case GLSC_glGetActiveUniform:
			glse_glGetActiveUniform();
			break;
        case GLSC_glGetAttribLocation:
			glse_glGetAttribLocation();
			break;
		case GLSC_glGetError:
			glse_glGetError();
			break;
        case GLSC_glGetFloatv:
			glse_glGetFloatv();
			break;
        case GLSC_glGetIntegerv:
			glse_glGetIntegerv();
			break;
        case GLSC_glGetProgramInfoLog:
			glse_glGetProgramInfoLog();
			break;
        case GLSC_glGetProgramiv:
			glse_glGetProgramiv();
			break;
        case GLSC_glGetShaderInfoLog:
			glse_glGetShaderInfoLog();
			break;
        case GLSC_glGetShaderiv:
			glse_glGetShaderiv();
			break;
        case GLSC_glGetString:
			glse_glGetString();
			break;
        case GLSC_glGetUniformLocation:
			glse_glGetUniformLocation();
			break;
		case GLSC_glUnmapBufferOES:
			glse_glUnmapBufferOES();
			break;
		// glVertexAttrib*f commands combine
		case GLSC_glVertexAttribFloat:
			glse_glVertexAttribFloat();
			break;
        case GLSC_glReadPixels:
			glse_glReadPixels();
			break;
        case GLSC_glShaderSource:
			glse_glShaderSource();
			break;
		default:
			return FALSE;
	}
	check_gl_err(c->cmd);
	return TRUE;
}

