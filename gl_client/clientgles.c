// This file declare OpenGL ES methods for streaming

#include <stdio.h>
#include <string.h>

#include "glclient.h"
#include "GLES2/gl2.h"

#ifdef GLS_EMULATE_VBO
static struct vbo_state vbo;
#endif

GL_APICALL void GL_APIENTRY glActiveTexture (GLenum texture)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glActiveTexture);
  c->texture = texture;
  GLS_PUSH_BATCH(glActiveTexture);
}


GL_APICALL void GL_APIENTRY glAttachShader (GLuint program, GLuint shader)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glAttachShader);
  c->program = program;
  c->shader = shader;
  GLS_PUSH_BATCH(glAttachShader);
}


GL_APICALL void GL_APIENTRY glBindAttribLocation (GLuint program, GLuint index, const GLchar* name)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glBindAttribLocation);
  c->program = program;
  c->index = index;
  // c->name[GLS_STRING_SIZE_PLUS - 1] = '\0';
  strncpy(c->name, name, GLS_STRING_SIZE);
  GLS_PUSH_BATCH(glBindAttribLocation);
}

GL_APICALL void GL_APIENTRY glBindBuffer (GLenum target, GLuint buffer)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glBindBuffer);
  c->target = target;
  c->buffer = buffer;
#ifdef GLS_EMULATE_VBO
  if(target == GL_ARRAY_BUFFER) {
      vbo.vbo = buffer;
  } else if(target == GL_ELEMENT_ARRAY_BUFFER) {
      vbo.ibo = buffer;
  } else printf("gls error: unsupported buffer type!\n");
#endif // GLS_EMULATE_VBO
  GLS_PUSH_BATCH(glBindBuffer);
}


GL_APICALL void GL_APIENTRY glBindFramebuffer (GLenum target, GLuint framebuffer)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glBindFramebuffer);
  c->target = target;
  c->framebuffer = framebuffer;
  GLS_PUSH_BATCH(glBindFramebuffer);
}


GL_APICALL void GL_APIENTRY glBindTexture (GLenum target, GLuint texture)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glBindTexture);
  c->target = target;
  c->texture = texture;
  GLS_PUSH_BATCH(glBindTexture);
}


GL_APICALL void GL_APIENTRY glBlendFuncSeparate (GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glBlendFuncSeparate);
  c->srcRGB = srcRGB;
  c->dstRGB = dstRGB;
  c->srcAlpha = srcAlpha;
  c->dstAlpha = dstAlpha;
  GLS_PUSH_BATCH(glBlendFuncSeparate);
}


GL_APICALL void GL_APIENTRY glBlendEquationSeparate (GLenum modeRGB, GLenum modeAlpha)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glBlendEquationSeparate);
  c->modeRGB = modeRGB;
  c->modeAlpha = modeAlpha;
  GLS_PUSH_BATCH(glBlendEquationSeparate);
}


GL_APICALL void GL_APIENTRY glBlendFunc (GLenum sfactor, GLenum dfactor)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glBlendFunc);
  c->sfactor = sfactor;
  c->dfactor = dfactor;
  GLS_PUSH_BATCH(glBlendFunc);
}


GL_APICALL void GL_APIENTRY glBufferData (GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
{
    gls_cmd_flush();
    // printf("GL_DBG: glBufferData size=%i realsize=%i\n",size,sizeof(data));
    gls_cmd_send_data(0, (uint32_t)size, (void *)data);
    GLS_SET_COMMAND_PTR(c, glBufferData);
    c->target = target;
    c->size = size;
    c->usage = usage;
    GLS_SEND_PACKET(glBufferData);
}


GL_APICALL void GL_APIENTRY glBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data)
{
    gls_cmd_flush();
    gls_cmd_send_data(0, (uint32_t)size, (void *)data);

    GLS_SET_COMMAND_PTR(c, glBufferSubData);
    c->target = target;
    c->offset = offset;
    c->size = size;
    GLS_SEND_PACKET(glBufferSubData);
}


GL_APICALL void GL_APIENTRY glClear (GLbitfield mask)
{
    GLS_SET_COMMAND_PTR_BATCH(c, glClear);
    c->mask = mask;
    GLS_PUSH_BATCH(glClear);
}


GL_APICALL void GL_APIENTRY glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    GLS_SET_COMMAND_PTR_BATCH(c, glClearColor);
    c->red = red;
    c->green = green;
    c->blue = blue;
    c->alpha = alpha;
    GLS_PUSH_BATCH(glClearColor);
}

GL_APICALL void GL_APIENTRY glClearDepthf (GLclampf depth)
{
    GLS_SET_COMMAND_PTR_BATCH(c, glClearDepthf);
    c->depth = depth;
    GLS_PUSH_BATCH(glClearDepthf);
}


GL_APICALL void GL_APIENTRY glClearStencil (GLint s)
{
    GLS_SET_COMMAND_PTR_BATCH(c, glClearStencil);
    c->s = s;
    GLS_PUSH_BATCH(glClearStencil);
}

GL_APICALL void GL_APIENTRY glColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
    GLS_SET_COMMAND_PTR_BATCH(c, glColorMask);
    c->red = red;
    c->green = green;
    c->blue = blue;
    c->alpha = alpha;
    GLS_PUSH_BATCH(glColorMask);
}

GL_APICALL void GL_APIENTRY glCompileShader (GLuint shader)
{
    GLS_SET_COMMAND_PTR_BATCH(c, glCompileShader);
    c->shader = shader;
    GLS_PUSH_BATCH(glCompileShader);
}


GL_APICALL void GL_APIENTRY glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    GLS_SET_COMMAND_PTR_BATCH(c, glCopyTexSubImage2D);
    c->target = target;
    c->level = level;
    c->xoffset = xoffset;
    c->yoffset = yoffset;
    c->x = x;
    c->y = y;
    c->width = width;
    c->height = height;
    GLS_PUSH_BATCH(glCopyTexSubImage2D);
}


GL_APICALL GLuint GL_APIENTRY glCreateProgram (void)
{
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, glCreateProgram);
    GLS_SEND_PACKET(glCreateProgram);

    wait_for_data("timeout:glCreateProgram");
    gls_ret_glCreateProgram_t *ret = (gls_ret_glCreateProgram_t *)glsc_global.tmp_buf.buf;
    return ret->program;
}


GL_APICALL GLuint GL_APIENTRY glCreateShader (GLenum type)
{
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, glCreateShader);
    c->type = type;
    GLS_SEND_PACKET(glCreateShader);

    wait_for_data("timeout:glCreateShader");
    gls_ret_glCreateShader_t *ret = (gls_ret_glCreateShader_t *)glsc_global.tmp_buf.buf;
    return ret->obj;
}


GL_APICALL void GL_APIENTRY glCullFace (GLenum mode)
{
    GLS_SET_COMMAND_PTR_BATCH(c, glCullFace);
    c->mode = mode;
    GLS_PUSH_BATCH(glCullFace);
}


GL_APICALL void GL_APIENTRY glDeleteBuffers (GLsizei n, const GLuint* buffers)
{
  gls_cmd_flush();
  uint32_t size = n * sizeof(uint32_t);
  gls_cmd_send_data(0, size, (void *)buffers);

  GLS_SET_COMMAND_PTR(c, glDeleteBuffers);
  c->n = n;
  GLS_SEND_PACKET(glDeleteBuffers);
}


GL_APICALL void GL_APIENTRY glDeleteShader (GLuint shader)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glDeleteShader);
  c->shader = shader;
  GLS_PUSH_BATCH(glDeleteShader);
}


GL_APICALL void GL_APIENTRY glDeleteProgram (GLuint program)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glDeleteProgram);
  c->program = program;
  GLS_PUSH_BATCH(glDeleteProgram);
}


GL_APICALL void GL_APIENTRY glDeleteTextures (GLsizei n, const GLuint* textures)
{
  uint32_t datasize = n * sizeof(uint32_t);
  GLS_SET_COMMAND_PTR_BATCH(c, glDeleteTextures);
  uint32_t cmd_size = (uint32_t)(((char *)c->textures + datasize) - (char *)c);
  if (check_batch_overflow(cmd_size, "glDeleteTextures: buffer overflow") != TRUE) {
    return;
  }
  c->cmd_size = cmd_size;
  c->n = n;
  memcpy(c->textures, textures, datasize);
  push_batch_command(cmd_size);
  gls_cmd_flush();
}


GL_APICALL void GL_APIENTRY glDisableVertexAttribArray (GLuint index)
{
#ifdef GLS_EMULATE_VBO
  vt_attrib_pointer[index].isenabled = GL_FALSE;
#endif // GLS_EMULATE_VBO
  GLS_SET_COMMAND_PTR_BATCH(c, glDisableVertexAttribArray);
  c->index = index;
  GLS_PUSH_BATCH(glDisableVertexAttribArray);
}


GL_APICALL void GL_APIENTRY glDisable (GLenum cap)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glDisable);
  c->cap = cap;
  GLS_PUSH_BATCH(glDisable);
}


GL_APICALL void GL_APIENTRY glDepthFunc (GLenum func)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glDepthFunc);
  c->func = func;
  GLS_PUSH_BATCH(glDepthFunc);
}


GL_APICALL void GL_APIENTRY glDepthMask (GLboolean flag)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glDepthMask);
  c->flag = flag;
  GLS_PUSH_BATCH(glDepthMask);
}


GL_APICALL void GL_APIENTRY glDepthRangef (GLclampf zNear, GLclampf zFar)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glDepthRangef);
  c->zNear = zNear;
  c->zFar = zFar;
  GLS_PUSH_BATCH(glDepthRangef);
}


#ifdef GLS_EMULATE_VBO
static void wes_vertex_attrib_pointer(int i, int count)
{
    long ptrdiff;
    int stride;
    
    if( !vt_attrib_pointer[i].isenabled || vt_attrib_pointer[i].vbo_id )
        return;

    if( !vt_attrib_pointer[i].webgl_vbo_id )
        glGenBuffers(1, &vt_attrib_pointer[i].webgl_vbo_id );

    // detect if we can fit multiple arrays to single VBO
    ptrdiff = (char *)vt_attrib_pointer[i].ptr - (char *)vt_attrib_pointer[0].ptr;
    stride = vt_attrib_pointer[i].stride;

    // detect stride by type
    if( stride == 0 )
    {
        if( vt_attrib_pointer[i].type == GL_UNSIGNED_BYTE )
            stride = vt_attrib_pointer[i].size;
        else
            stride = vt_attrib_pointer[i].size * 4;
    }

    if( i && vt_attrib_pointer[0].isenabled && !vt_attrib_pointer[0].vbo_id && ptrdiff > 0 && ptrdiff < stride )
    {
        // reuse existing array
        glBindBuffer( GL_ARRAY_BUFFER, vt_attrib_pointer[0].webgl_vbo_id );
        glVertexAttribPointer(i, vt_attrib_pointer[i].size, vt_attrib_pointer[i].type, vt_attrib_pointer[i].normalized, vt_attrib_pointer[i].stride, (void*)ptrdiff);
    }
    else
    {
        glBindBuffer( GL_ARRAY_BUFFER, vt_attrib_pointer[i].webgl_vbo_id );
        //printf("BufferData %d %d\n",vt_attrib_pointer[i].webgl_vbo_id, (count + 4) * stride );
        glBufferData( GL_ARRAY_BUFFER, (count + 4) * stride, (void*)vt_attrib_pointer[i].ptr, GL_STREAM_DRAW);
        glVertexAttribPointer(i, vt_attrib_pointer[i].size, vt_attrib_pointer[i].type, vt_attrib_pointer[i].normalized, vt_attrib_pointer[i].stride, 0);
    }
}
#endif // GLS_EMULATE_VBO


GL_APICALL void GL_APIENTRY glDrawArrays (GLenum mode, GLint first, GLsizei count)
{
#ifdef GLS_EMULATE_VBO
    int vbo_bkp = vbo.vbo;
    int i;
    for( i = 0;i < 16; i++ )
    {
        if( vt_attrib_pointer[i].isenabled )
            wes_vertex_attrib_pointer(i, first + count);
        // else printf("Vertex attrib pointer %i was not enabled\n", i);
    }
    glBindBuffer( GL_ARRAY_BUFFER, vbo_bkp );
#endif // GLS_EMULATE_VBO

  GLS_SET_COMMAND_PTR_BATCH(c, glDrawArrays);
  c->mode = mode;
  c->first = first;
  c->count = count;
  GLS_PUSH_BATCH(glDrawArrays);
}


GL_APICALL void GL_APIENTRY glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)
{
#ifdef GLS_EMULATE_VBO
    int vbo_bkp = vbo.vbo;
    int ibo_bkp = vbo.ibo;
    int i;
    for (i = 0; i < 16; i++) {
        if( vt_attrib_pointer[i].isenabled ) {
            wes_vertex_attrib_pointer(i, 65536);
        }
    }
    if( !vbo.ibo ) {
        if( !vbo.ibo_emu ) {
            glGenBuffers(1, &vbo.ibo_emu);
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo.ibo_emu);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, type == GL_UNSIGNED_SHORT?count * 2:count*4, indices, GL_STREAM_DRAW);
      
        // why?
        indices = 0;
    }
#endif // GLS_EMULATE_VBO

    uint32_t sizeoftype;
    if (type == GL_UNSIGNED_BYTE) {
        sizeoftype = sizeof(GLubyte);
    } else { // GL_UNSIGNED_SHORT
        sizeoftype = sizeof(GLushort);
    }
    
    uint32_t datasize = count * 4 * sizeoftype;
    GLS_SET_COMMAND_PTR_BATCH(c, glDrawElements);
    uint32_t cmd_size = (uint32_t)(((char *)c->indices + datasize) - (char *)c);
    if (check_batch_overflow(cmd_size, "glDrawElements: buffer overflow") != TRUE) {
        return;
    }
    c->cmd_size = cmd_size;
    c->mode = mode;
    c->count = count;
    c->type = type;

#ifdef GLS_EMULATE_VBO
    c->indices_isnull = 1;
    c->indices[0] = '\0';
#else // !GLS_EMULATE_VBO
    c->indices_isnull = (indices == NULL);
    if (c->indices_isnull == FALSE) {
        const GLchar *indices_str = (const GLchar *)indices;
        memcpy(c->indices, indices_str, GLS_STRING_SIZE);
    } else {
        c->indices[0] = '\0';
    }
#endif // !GLS_EMULATE_VBO

#if __WORDSIZE == 64
    c->indices_uint = (uint32_t)(uint64_t)indices;
#else // __WORDSIZE == 32
    c->indices_uint = (uint32_t)indices;
#endif // __WORDSIZE == 32
    
    GLS_PUSH_BATCH(glDrawElements);

#ifdef GLS_EMULATE_VBO
    if( !ibo_bkp ) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    glBindBuffer( GL_ARRAY_BUFFER, vbo_bkp );
#endif // GLS_EMULATE_VBO
}

#ifdef GLS_EMULATE_VBO
GLvoid glDrawRangeElements( GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices )
{
    int vbo_bkp = vbo.vbo;
    int ibo_bkp = vbo.ibo;
    int i;
    for (i = 0; i < 16; i++) {
        wes_vertex_attrib_pointer(i, end);
    }
    if (!vbo.ibo) {
        if (!vbo.ibo_emu) {
            glGenBuffers(1, &vbo.ibo_emu);
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo.ibo_emu);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * (type == GL_UNSIGNED_SHORT ? 2 : 4), indices, GL_STREAM_DRAW);
        indices = 0;
    }


    GLS_SET_COMMAND_PTR_BATCH(c, glDrawElements);
    c->mode = mode;
    c->count = count;
    c->type = type;

#ifdef GLS_EMULATE_VBO
    c->indices_isnull = 1;
    c->indices[0] = '\0';
#else // !GLS_EMULATE_VBO
    c->indices_isnull = (indices == NULL);
    if (c->indices_isnull == FALSE) {
        const GLchar *indices_str = (const GLchar *)indices;
        memcpy(c->indices, indices_str, GLS_STRING_SIZE);
    } else {
        c->indices[0] = '\0';
    }
#endif // !GLS_EMULATE_VBO

#if __WORDSIZE == 64
    c->indices_uint = (uint32_t)(uint64_t)indices;
#else // __WORDSIZE == 32
    c->indices_uint = (uint32_t)indices;
#endif // __WORDSIZE == 32
    
    GLS_PUSH_BATCH(glDrawElements);
    
    if( !ibo_bkp )
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
       glBindBuffer( GL_ARRAY_BUFFER, vbo_bkp );
}
#endif // GLS_EMULATE_VBO


GL_APICALL void GL_APIENTRY glEnableVertexAttribArray (GLuint index)
{
#ifdef GLS_EMULATE_VBO
    vt_attrib_pointer[index].isenabled = GL_TRUE;
#endif // GLS_EMULATE_VBO
    GLS_SET_COMMAND_PTR_BATCH(c, glEnableVertexAttribArray);
    c->index = index;
    GLS_PUSH_BATCH(glEnableVertexAttribArray);
}


GL_APICALL void GL_APIENTRY glEnable (GLenum cap)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glEnable);
  c->cap = cap;
  GLS_PUSH_BATCH(glEnable);
}


GL_APICALL void GL_APIENTRY glFinish (void)
{
  gls_cmd_flush();
  GLS_SET_COMMAND_PTR(c, glFinish);
  GLS_SEND_PACKET(glFinish);
  wait_for_data("timeout:glFinish");
}


GL_APICALL void GL_APIENTRY glFlush (void)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glFlush);
  GLS_PUSH_BATCH(glFlush);
}


GL_APICALL void GL_APIENTRY glGenBuffers (GLsizei n, GLuint* buffers)
{
  gls_cmd_flush();
  GLS_SET_COMMAND_PTR(c, glGenBuffers);
  c->n = n;
  GLS_SEND_PACKET(glGenBuffers);

  wait_for_data("timeout:glGenBuffers");
  memcpy(buffers, glsc_global.tmp_buf.buf, c->n * sizeof(uint32_t));
}


GL_APICALL void GL_APIENTRY glGenTextures (GLsizei n, GLuint* textures)
{
  gls_cmd_flush();
  GLS_SET_COMMAND_PTR(c, glGenTextures);
  c->n = n;
  GLS_SEND_PACKET(glGenTextures);

  wait_for_data("timeout:glGenTextures");
  memcpy(textures, glsc_global.tmp_buf.buf, c->n * sizeof(uint32_t));
}


GL_APICALL void GL_APIENTRY glGetActiveAttrib (GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name)
{
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, glGetActiveAttrib);
    c->program = program;
    c->index = index;
    c->bufsize = bufsize;
    
    GLS_SEND_PACKET(glGetActiveAttrib);
    
    wait_for_data("timeout:glGetActiveAttrib");
    gls_ret_glGetActiveAttrib_t *ret = (gls_ret_glGetActiveAttrib_t *)glsc_global.tmp_buf.buf;
    
    if (length != NULL) {
        *length = ret->length;
    }
    *size = ret->size;
    *type = ret->type;
    if (ret->length == 0) {
        ret->name[0] = '\0';
    }
    strncpy(name, ret->name, (size_t)bufsize);
}


GL_APICALL void GL_APIENTRY glGetActiveUniform (GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name)
{
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, glGetActiveUniform);
    c->program = program;
    c->index = index;
    c->bufsize = bufsize;
    
    GLS_SEND_PACKET(glGetActiveUniform);
    
    wait_for_data("timeout:glGetActiveUniform");
    gls_ret_glGetActiveUniform_t *ret = (gls_ret_glGetActiveUniform_t *)glsc_global.tmp_buf.buf;
    
    if (length != NULL) {
        *length = ret->length;
    }
    *size = ret->size;
    *type = ret->type;
    
    if (ret->length == 0) {
        ret->name[0] = '\0';
    }
    strncpy(name, ret->name, (size_t)bufsize);
}


GL_APICALL GLint GL_APIENTRY glGetAttribLocation (GLuint program, const GLchar* name)
{
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, glGetAttribLocation);
    c->program = program;
    // c->name[GLS_STRING_SIZE_PLUS - 1] = '\0';
    
    int nameLength = strnlen(name, 0xA00000) + 1;
/*
    if (nameLength > 50) {
        printf("gls error: please increase glGetAttribLocation(char name[]) up to %d\n", nameLength);
    }
*/
    strncpy(c->name, name, nameLength);
    GLS_SEND_PACKET(glGetAttribLocation);

    wait_for_data("timeout:glGetAttribLocation");
    gls_ret_glGetAttribLocation_t *ret = (gls_ret_glGetAttribLocation_t *)glsc_global.tmp_buf.buf;
    return ret->index;
}


GL_APICALL GLenum GL_APIENTRY glGetError()
{
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, glGetError);
    GLS_SEND_PACKET(glGetError);
    
    wait_for_data("timeout:glGetError");
    gls_ret_glGetError_t *ret = (gls_ret_glGetError_t *)glsc_global.tmp_buf.buf;
    return ret->error;
}


GL_APICALL void GL_APIENTRY glGetFloatv(GLenum name, GLfloat *params)
{
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, glGetFloatv);
    c->name = name;
    GLS_SEND_PACKET(glGetFloatv);
    
    wait_for_data("timeout:glGetFloatv");
    gls_ret_glGetFloatv_t *ret = (gls_ret_glGetFloatv_t *)glsc_global.tmp_buf.buf;
    *params = ret->params;
}


GL_APICALL void GL_APIENTRY glGetIntegerv(GLenum name, GLint *params)
{
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, glGetIntegerv);
    c->name = name;
    GLS_SEND_PACKET(glGetIntegerv);
    
    wait_for_data("timeout:glGetIntegerv");
    gls_ret_glGetIntegerv_t *ret = (gls_ret_glGetIntegerv_t *)glsc_global.tmp_buf.buf;
    *params = ret->params;
}


GL_APICALL void GL_APIENTRY glGetProgramInfoLog (GLuint program, GLsizei bufsize, GLsizei* length, GLchar* infolog)
{
  gls_cmd_flush();
  GLS_SET_COMMAND_PTR(c, glGetProgramInfoLog);
  c->program = program;
  c->bufsize = bufsize;
  GLS_SEND_PACKET(glGetProgramInfoLog);

  wait_for_data("timeout:glGetProgramInfoLog");
  gls_ret_glGetProgramInfoLog_t *ret = (gls_ret_glGetProgramInfoLog_t *)glsc_global.tmp_buf.buf;
  if (length != NULL) {
    *length = ret->length;
  }
  if (ret->length == 0) {
    ret->infolog[0] = '\0';
  }
  strncpy(infolog, ret->infolog, (size_t)bufsize);
}


GL_APICALL void GL_APIENTRY glGetProgramiv (GLuint program, GLenum pname, GLint* params)
{
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, glGetProgramiv);
    c->program = program;
    c->pname = pname;
    GLS_SEND_PACKET(glGetProgramiv);

    wait_for_data("timeout:glGetProgramiv");
    gls_ret_glGetProgramiv_t *ret = (gls_ret_glGetProgramiv_t *)glsc_global.tmp_buf.buf;
    
    *params = ret->params;
    
    // printf("glGetProgramiv(program=%p, pname=%p) -> ptr=%p, value=%p\n", program, pname, params, *params);
}


GL_APICALL void GL_APIENTRY glGetShaderiv (GLuint shader, GLenum pname, GLint* params)
{
  gls_cmd_flush();
  GLS_SET_COMMAND_PTR(c, glGetShaderiv);
  c->shader = shader;
  c->pname = pname;
  GLS_SEND_PACKET(glGetShaderiv);

  wait_for_data("timeout:glGetShaderiv");
  gls_ret_glGetShaderiv_t *ret = (gls_ret_glGetShaderiv_t *)glsc_global.tmp_buf.buf;
  
  *params = ret->params;
  // printf("Done executing glGetShaderiv(%p, %p) with return %i\n", shader, pname, ret->params);
}


GL_APICALL void GL_APIENTRY glGetShaderInfoLog (GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* infolog)
{
  gls_cmd_flush();
  GLS_SET_COMMAND_PTR(c, glGetShaderInfoLog);
  c->shader = shader;
  c->bufsize = bufsize;
  GLS_SEND_PACKET(glGetShaderInfoLog);

  wait_for_data("timeout:glGetShaderInfoLog");
  gls_ret_glGetShaderInfoLog_t *ret = (gls_ret_glGetShaderInfoLog_t *)glsc_global.tmp_buf.buf;
  if (length != NULL) {
    *length = ret->length;
  }
  if (ret->length == 0) {
    ret->infolog[0] = '\0';
  }
  strncpy(infolog, ret->infolog, (size_t)bufsize);
}


GL_APICALL const GLubyte* GL_APIENTRY glGetString(GLenum name)
{
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, glGetString);
    c->name = name;
    GLS_SEND_PACKET(glGetString);

    wait_for_data("timeout:glGetString");
    gls_ret_glGetString_t *ret = (gls_ret_glGetString_t *)glsc_global.tmp_buf.buf;
    // printf("glGetString(%i) return %s\n", name, &ret->params[0]);
    return ret->params;
}


GL_APICALL int GL_APIENTRY glGetUniformLocation (GLuint program, const GLchar* name)
{
  gls_cmd_flush();
  GLS_SET_COMMAND_PTR(c, glGetUniformLocation);
  c->program = program;
  // c->name[GLS_STRING_SIZE_PLUS - 1] = '\0';
  
  int nameLength = strnlen(name, 0xA00000) + 1;
/*
  if (nameLength > 100) {
      printf("gls error: please increase glGetUniformLocation(char name[]) up to %d\n", nameLength);
  }
*/
  strncpy(c->name, name, nameLength);
  GLS_SEND_PACKET(glGetUniformLocation);

  wait_for_data("timeout:glGetUniformLocation");
  gls_ret_glGetUniformLocation_t *ret = (gls_ret_glGetUniformLocation_t *)glsc_global.tmp_buf.buf;
  return ret->location;
}


GL_APICALL void GL_APIENTRY glHint (GLenum target, GLenum mode)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glHint);
  c->target = target;
  c->mode = mode;
  GLS_PUSH_BATCH(glHint);
}


GL_APICALL void GL_APIENTRY glLineWidth (GLfloat width)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glLineWidth);
  c->width = width;
  GLS_PUSH_BATCH(glLineWidth);
}


GL_APICALL void GL_APIENTRY glLinkProgram (GLuint program)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glLinkProgram);
  c->program = program;
  GLS_PUSH_BATCH(glLinkProgram);
}


GL_APICALL void GL_APIENTRY glShaderSource (GLuint shader, GLsizei count, const GLchar*const* string, const GLint* length)
{
  gls_cmd_flush();
  if (count > 10240) { // 256
    printf("gls warning: shader too large, over 10kb, ignoring.\n"); // FIXME why!?
    return;
  }
  gls_data_glShaderSource_t *dat = (gls_data_glShaderSource_t *)glsc_global.tmp_buf.buf;
  size_t size_all = (size_t)(dat->data - (char *)dat);
  
  // printf("\n ----- BEGIN SHADER CONTENT -----\n");
  uint32_t stroffset = 0;
  unsigned int i;

  // FIXME we're sending both a full length-array *and* NUL terminators
  for (i = 0; i < count; i++) {
    const GLchar *strptr = string[i];
    size_t strsize = length ? length[i] : 0;
    if (strsize == 0)
      strsize = strlen(strptr);
    size_all += strsize + 1;
    if (size_all > GLS_TMP_BUFFER_SIZE) {
      printf("gls error: shader buffer size overflow!\n");
      return;
    }
    dat->string[i] = stroffset;
    dat->length[i] = strsize;
    memcpy(&dat->data[stroffset], strptr, strsize + 1);
    dat->data[stroffset + strsize] = '\0';
    stroffset += strsize + 1;
    
    // printf("gls debug: shader length = %i\n", strsize);
    
    // printf("%s\n", strptr);
  }

  // printf(" ----- ENDED SHADER CONTENT -----\n\n");
  
  gls_cmd_send_data(0, size_all, glsc_global.tmp_buf.buf);
  GLS_SET_COMMAND_PTR(c, glShaderSource);
  c->shader = shader;
  c->count = count;
  GLS_SEND_PACKET(glShaderSource);
}


GL_APICALL void GL_APIENTRY glPixelStorei (GLenum pname, GLint param)
{
  switch (pname)
  {
    case GL_PACK_ALIGNMENT:
      glsc_global.pack_alignment = param;
      break;
    case GL_UNPACK_ALIGNMENT:
      glsc_global.unpack_alignment = param;
      break;
    default:
      break;
  }
  GLS_SET_COMMAND_PTR_BATCH(c, glPixelStorei);
  c->pname = pname;
  c->param = param;
  GLS_PUSH_BATCH(glPixelStorei);
}


GL_APICALL void GL_APIENTRY glPolygonOffset (GLfloat factor, GLfloat units)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glPolygonOffset);
  c->factor = factor;
  c->units = units;
  GLS_PUSH_BATCH(glPolygonOffset);
}


GL_APICALL void GL_APIENTRY glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels)
{
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, glReadPixels);
    c->x = x;
    c->y = y;
    c->width = width;
    c->height = height;
    c->format = format;
    c->type = type;
    GLS_SEND_PACKET(glReadPixels);
    
    wait_for_data("timeout:glReadPixels");
    gls_ret_glReadPixels_t *ret = (gls_ret_glReadPixels_t *)glsc_global.tmp_buf.buf;
    pixels = ret->pixels;
    // memcpy(pixels, ret->pixels, width * height); // width * height = size correct???
}


GL_APICALL void GL_APIENTRY glStencilFunc(GLenum func, GLint r, GLuint m)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glStencilFunc);
  c->func = func;
  c->r = r;
  c->m = m;
  GLS_PUSH_BATCH(glStencilFunc);
}


GL_APICALL void GL_APIENTRY glStencilMask (GLuint mask)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glStencilMask);
  c->mask = mask;
  GLS_PUSH_BATCH(glStencilMask);
}


GL_APICALL void GL_APIENTRY glStencilOp (GLenum fail, GLenum zfail, GLenum zpass)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glStencilOp);
  c->fail = fail;
  c->zfail = zfail;
  c->zpass = zpass;
  GLS_PUSH_BATCH(glStencilOp);
}


GL_APICALL void GL_APIENTRY glTexParameteri (GLenum target, GLenum pname, GLint param)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glTexParameteri);
  c->target = target;
  c->pname = pname;
  c->param = param;
  GLS_PUSH_BATCH(glTexParameteri);
}


GL_APICALL void GL_APIENTRY glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels)
{
    uint32_t pixelbytes, linebytes, datasize;
    switch (type) {
        case GL_UNSIGNED_BYTE:
            switch (format) {
                case GL_ALPHA:
                    pixelbytes = 1;
                    break;
                case GL_RGB:
                    pixelbytes = 3;
                    break;
                case GL_RGBA:
                    pixelbytes = 4;
                    break;
                case GL_LUMINANCE:
                    pixelbytes = 1;
                    break;
                case GL_LUMINANCE_ALPHA:
                    pixelbytes = 2;
                    break;
                default:
                    pixelbytes = 4;
                    break;
            } break;
        case GL_UNSIGNED_SHORT_5_6_5:
            pixelbytes = 2;
            break;
        case GL_UNSIGNED_SHORT_4_4_4_4:
            pixelbytes = 2;
            break;
        case GL_UNSIGNED_SHORT_5_5_5_1:
            pixelbytes = 2;
            break;
        default:
            pixelbytes = 4;
            break;
  }
  linebytes = (pixelbytes * width + glsc_global.unpack_alignment - 1) & (~ (glsc_global.unpack_alignment - 1));
  datasize = linebytes * height;
  GLS_SET_COMMAND_PTR_BATCH(c, glTexImage2D);
  uint32_t cmd_size = (uint32_t)(((char *)c->pixels + datasize) - (char *)c);
  if (check_batch_overflow(cmd_size, "glTexImage2D: buffer overflow") != TRUE) {
      return;
  }
  c->cmd_size = cmd_size;
  c->target = target;
  c->level = level;
  c->internalformat = internalformat;
  c->width = width;
  c->height = height;
  c->border = border;
  c->format = format;
  c->type = type;
  c->pixels_isnull = (pixels == NULL);
  if (c->pixels_isnull == FALSE) {
      memcpy(c->pixels, pixels, datasize);
  }
  push_batch_command(cmd_size);
  gls_cmd_flush();
}


GL_APICALL void GL_APIENTRY glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels)
{
  uint32_t pixelbytes, linebytes, datasize;
  switch (type)
  {
    case GL_UNSIGNED_BYTE:
      switch (format)
      {
        case GL_ALPHA:
          pixelbytes = 1;
          break;
        case GL_RGB:
          pixelbytes = 3;
          break;
        case GL_RGBA:
          pixelbytes = 4;
          break;
        case GL_LUMINANCE:
          pixelbytes = 1;
          break;
        case GL_LUMINANCE_ALPHA:
          pixelbytes = 2;
          break;
        default:
          pixelbytes = 4;
          break;
      }
      break;
    case GL_UNSIGNED_SHORT_5_6_5:
      pixelbytes = 2;
      break;
    case GL_UNSIGNED_SHORT_4_4_4_4:
      pixelbytes = 2;
      break;
    case GL_UNSIGNED_SHORT_5_5_5_1:
      pixelbytes = 2;
      break;
    default:
      pixelbytes = 4;
      break;
  }
  linebytes = (pixelbytes * width + glsc_global.unpack_alignment - 1) & (~ (glsc_global.unpack_alignment - 1));
  datasize = linebytes * height;
  GLS_SET_COMMAND_PTR_BATCH(c, glTexSubImage2D);
  uint32_t cmd_size = (uint32_t)(((char *)c->pixels + datasize) - (char *)c);
  if (check_batch_overflow(cmd_size, "glTexSubImage2D: buffer overflow") != TRUE)
  {
    return;
  }
  c->cmd_size = cmd_size;
  c->target = target;
  c->level = level;
  c->xoffset = xoffset;
  c->yoffset = yoffset;
  c->width = width;
  c->height = height;
  c->format = format;
  c->type = type;
  c->pixels_isnull = (pixels == NULL);
  if (c->pixels_isnull == FALSE) {
      memcpy(c->pixels, pixels, datasize);
  }
  push_batch_command(cmd_size);
  gls_cmd_flush();
}

GL_APICALL void GL_APIENTRY glUniform1f (GLint location, GLfloat x)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glUniform1f);
  c->location = location;
  c->x = x;
  GLS_PUSH_BATCH(glUniform1f);
}


GL_APICALL void GL_APIENTRY glUniform4fv (GLint location, GLsizei count, const GLfloat* v)
{
  uint32_t datasize = count * 4 * sizeof(GLfloat);
  GLS_SET_COMMAND_PTR_BATCH(c, glUniform4fv);
  uint32_t cmd_size = (uint32_t)(((char *)c->v + datasize) - (char *)c);
  if (check_batch_overflow(cmd_size, "glUniform4fv: buffer overflow") != TRUE)
  {
    return;
  }
  c->cmd_size = cmd_size;
  c->location = location;
  c->count = count;
  memcpy(c->v, v, datasize);
  push_batch_command(cmd_size);
}


GL_APICALL void GL_APIENTRY glUniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
  uint32_t datasize = count * 16 * sizeof(GLfloat);
  GLS_SET_COMMAND_PTR_BATCH(c, glUniformMatrix4fv);
  uint32_t cmd_size = (uint32_t)(((char *)c->value + datasize) - (char *)c);
  if (check_batch_overflow(cmd_size, "glUniformMatrix4fv: buffer overflow") != TRUE)
  {
    return;
  }
  c->cmd_size = cmd_size;
  c->location = location;
  c->count = count;
  c->transpose = transpose;
  memcpy(c->value, value, datasize);
  push_batch_command(cmd_size);
}


GL_APICALL void GL_APIENTRY glUseProgram (GLuint program)
{
    GLS_SET_COMMAND_PTR_BATCH(c, glUseProgram);
    c->program = program;
    GLS_PUSH_BATCH(glUseProgram);
}


static void _glVertexAttribFloat(GLuint index, GLint num_float, GLboolean call_arr, const GLfloat *arr)
{
    gls_cmd_flush();
    gls_data_glVertexAttribFloat_t *dat = (gls_data_glVertexAttribFloat_t *)glsc_global.tmp_buf.buf;
    memcpy(dat->arr, arr, num_float);
    // It's small so use GLS_DATA_SIZE
    gls_cmd_send_data(0, GLS_DATA_SIZE, glsc_global.tmp_buf.buf);

    GLS_SET_COMMAND_PTR(c, glVertexAttribFloat);
    c->index = index;
    c->num_float = num_float;
    // If TRUE, call to glVertexAttrib*fv instead of glVertexAttrib*f
    c->call_arr = call_arr;
    GLS_SEND_PACKET(glVertexAttribFloat);
}


GL_APICALL void GL_APIENTRY glVertexAttrib1f(GLuint index, GLfloat v0)
{
    GLfloat arr[1] = {v0};
    _glVertexAttribFloat(index, 1, GL_FALSE, arr);
}


GL_APICALL void GL_APIENTRY glVertexAttrib2f(GLuint index, GLfloat v0, GLfloat v1)
{
    GLfloat arr[2] = {v0, v1};
    _glVertexAttribFloat(index, 2, GL_FALSE, arr);
}


GL_APICALL void GL_APIENTRY glVertexAttrib3f(GLuint index, GLfloat v0, GLfloat v1, GLfloat v2)
{
    GLfloat arr[3] = {v0, v1, v2};
    _glVertexAttribFloat(index, 3, GL_FALSE, arr);
}


GL_APICALL void GL_APIENTRY glVertexAttrib4f(GLuint index, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
    GLfloat arr[4] = {v0, v1, v2, v3};
    _glVertexAttribFloat(index, 4, GL_FALSE, arr);
}


GL_APICALL void GL_APIENTRY glVertexAttrib1fv(GLuint index, const GLfloat *v)
{
    _glVertexAttribFloat(index, 1, GL_TRUE, v);
}


GL_APICALL void GL_APIENTRY glVertexAttrib2fv(GLuint index, const GLfloat *v)
{
    _glVertexAttribFloat(index, 2, GL_TRUE, v);
}


GL_APICALL void GL_APIENTRY glVertexAttrib3fv(GLuint index, const GLfloat *v)
{
    _glVertexAttribFloat(index, 3, GL_TRUE, v);
}


GL_APICALL void GL_APIENTRY glVertexAttrib4fv(GLuint index, const GLfloat *v)
{
    _glVertexAttribFloat(index, 4, GL_TRUE, v);
}


#ifdef GLS_EMULATE_VBO
void glVertexAttribPointer_vbo (GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* ptr)
{
    vt_attrib_pointer[indx].size = size;
    vt_attrib_pointer[indx].type = type;
    vt_attrib_pointer[indx].stride = stride;
    vt_attrib_pointer[indx].normalized = normalized;
    vt_attrib_pointer[indx].ptr = ptr;
    vt_attrib_pointer[indx].vbo_id = vbo.vbo;
}
#endif // GLS_EMULATE_VBO


GL_APICALL void GL_APIENTRY glVertexAttribPointer (GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* ptr)
{
#ifdef GLS_EMULATE_VBO
    if(!vbo.vbo) {// ignore non-vbo
        glVertexAttribPointer_vbo(indx, size, type, normalized, stride, ptr);
        return;
    }
    vt_attrib_pointer[indx].vbo_id = vbo.vbo;
#endif // GLS_EMULATE_VBO

    gls_cmd_flush();
    GLS_SET_COMMAND_PTR_BATCH(c, glVertexAttribPointer);
    c->indx = indx;
    c->size = size;
    c->type = type;
    c->stride = stride;

#ifdef GLS_EMULATE_VBO
    c->ptr_isnull = 1;
    c->ptr[0] = '\0';
#else // !GLS_EMULATE_VBO
    // 0xA0 glxgears crash?
    c->ptr_isnull = (ptr == NULL || ptr == 0xc || ptr == 0x2A0 || ptr == 0xA20);
    // printf("gls glVertexAttribPointer: type=%p, ptr=%p, unptr_null=%p\n", type, ptr, *&ptr);
    if (c->ptr_isnull == FALSE) {
        const GLchar *ptr_str = (const GLchar *)ptr;
        
        // FIXME it may wrong!
        // c->ptr_isnull = ptr_str[0] == NULL;
        
        // int ptr_str_len = strnlen(ptr_str, 0xA00000) + 1;
        if (c->ptr_isnull == FALSE) {
            memcpy(c->ptr, ptr_str, GLS_STRING_SIZE);
            // ptr_str_len);
        }
    } else {
        c->ptr[0] = '\0';
    }
#endif // !GLS_EMULATE_VBO

/*
    c->ptr_isnull = 1;
    c->ptr[0] = '\0';
*/

#if __WORDSIZE == 64
    c->ptr_uint = (uint32_t)(uint64_t)ptr;
#else // __WORDSIZE == 32
    c->ptr_uint = (uint32_t)ptr;
#endif // __WORDSIZE == 32

    c->normalized = normalized;
    GLS_PUSH_BATCH(glVertexAttribPointer);
}


GL_APICALL void GL_APIENTRY glViewport (GLint x, GLint y, GLsizei width, GLsizei height)
{
    GLS_SET_COMMAND_PTR_BATCH(c, glViewport);
    c->x = x;
    c->y = y;
    c->width = width;
    c->height = height;
    GLS_PUSH_BATCH(glViewport);
}


/*
 * OES / EXT extension commands
 */
GL_APICALL void GL_APIENTRY glMapBufferOES(GLenum target, GLenum access)
{
    GLS_SET_COMMAND_PTR_BATCH(c, glMapBufferOES);
    c->target = target;
    c->access = access;
    GLS_PUSH_BATCH(glMapBufferOES);
}


GL_APICALL int GL_APIENTRY glUnmapBufferOES (GLenum target)
{
  gls_cmd_flush();
  GLS_SET_COMMAND_PTR(c, glUnmapBufferOES);
  c->target = target;
  GLS_SEND_PACKET(glUnmapBufferOES);

  wait_for_data("timeout:glUnmapBufferOES");
  gls_ret_glUnmapBufferOES_t *ret = (gls_ret_glUnmapBufferOES_t *)glsc_global.tmp_buf.buf;
  return ret->success;
}


// Used for return void commands
/*
GL_APICALL void GL_APIENTRY glCommand (GLparam param)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glCommand);
  c->param = param;
  GLS_PUSH_BATCH(glCommand);
}
 */
 
// Used for return functions, icluding modify client array.
 /*
GL_APICALL GLreturn GL_APIENTRY glCommand (GLparam param) 
{
  GLS_SET_COMMAND_PTR(c, glCommand);
  c->param = param;
  GLS_SEND_PACKET(glCommand);
  
  wait_for_data("timeout:glCommand");
  gls_ret_glCommand_t *ret = (gls_ret_glCommand_t *)glsc_global.tmp_buf.buf;
  // *params = ret->params;
  // or below if return
  // return ret->returnVal;
}
 */

