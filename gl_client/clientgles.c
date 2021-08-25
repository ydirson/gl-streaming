// This file declare OpenGL ES methods for streaming

#include "gls_command_gles2.h"
#include "glclient.h"
#include "GLES2/gl2.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static struct
{
    GLuint vbo, ibo, ibo_emu;
    struct attrib_pointer_t {
        GLboolean   isenabled;
        GLint       size;
        GLenum      type;
        GLsizei     stride;
        GLboolean   normalized;
        const GLvoid *ptr;
        GLuint vbo_id;
        GLuint emul_vbo_id;
    } attrib_pointer[16]; // FIXME: GL_MAX_VERTEX_ATTRIBS has no upper limit
} buffer_objs;

static unsigned _type_bytesize(GLenum type)
{
    switch(type) {
    case GL_BOOL: return sizeof(GLboolean);
    case GL_UNSIGNED_BYTE: return sizeof(GLubyte);
    case GL_BYTE: return sizeof(GLbyte);
    case GL_SHORT: return sizeof(GLshort);
    case GL_UNSIGNED_SHORT: return sizeof(GLushort);
    case GL_FIXED: return sizeof(GLfixed);
    case GL_FLOAT: return sizeof(GLfloat);
    default:
        fprintf(stderr, "%s: unhandled data type %x\n", __FUNCTION__, type);
        assert(0);
    }
}

static unsigned _pixelformat_to_bytes(GLenum format, GLenum type)
{
    switch (type) {
    case GL_UNSIGNED_BYTE:
        switch (format) {
        case GL_ALPHA:
            return 1;
        case GL_RGB:
            return 3;
        case GL_RGBA:
            return 4;
        case GL_LUMINANCE:
            return 1;
        case GL_LUMINANCE_ALPHA:
            return 2;
        default:
            fprintf(stderr, "WARNING: unhandled pixel format %x\n", format);
            return 4;
        }
    case GL_UNSIGNED_SHORT_5_6_5:
        return 2;
    case GL_UNSIGNED_SHORT_4_4_4_4:
        return 2;
    case GL_UNSIGNED_SHORT_5_5_5_1:
        return 2;
    default:
        fprintf(stderr, "WARNING: unhandled pixel type %x\n", type);
        return 4;
    }
}


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
  if (strlen(name) + 1 > sizeof(c->name)) {
    fprintf(stderr, "GLS ERROR: %s passed a name too long for protocol, len(%s) > %zu\n",
            __FUNCTION__, name, sizeof(c->name));
    return;
  }
  strcpy(c->name, name);
  GLS_PUSH_BATCH(glBindAttribLocation);
}


GL_APICALL void GL_APIENTRY glBindBuffer (GLenum target, GLuint buffer)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glBindBuffer);
  c->target = target;
  c->buffer = buffer;
  GLS_PUSH_BATCH(glBindBuffer);

  // FIXME: should we wait and check for error ?
  if(target == GL_ARRAY_BUFFER) {
      buffer_objs.vbo = buffer;
  } else if(target == GL_ELEMENT_ARRAY_BUFFER) {
      buffer_objs.ibo = buffer;
  } else printf("gls error: unsupported buffer type!\n");
}


GL_APICALL void GL_APIENTRY glBindFramebuffer (GLenum target, GLuint framebuffer)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glBindFramebuffer);
  c->target = target;
  c->framebuffer = framebuffer;
  GLS_PUSH_BATCH(glBindFramebuffer);
}


GL_APICALL void GL_APIENTRY glBindRenderbuffer(GLenum target, GLuint renderbuffer)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glBindRenderbuffer);
  c->target = target;
  c->renderbuffer = renderbuffer;
  GLS_PUSH_BATCH(glBindRenderbuffer);
}


GL_APICALL void GL_APIENTRY glBindTexture (GLenum target, GLuint texture)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glBindTexture);
  c->target = target;
  c->texture = texture;
  GLS_PUSH_BATCH(glBindTexture);
}


GL_APICALL void GL_APIENTRY glBlendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
  (void)red; (void)green; (void)blue; (void)alpha;
  WARN_STUBBED();
}


GL_APICALL void GL_APIENTRY glBlendEquation ( GLenum mode )
{
  (void)mode;
  WARN_STUBBED();
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


GL_APICALL void GL_APIENTRY glBlendFuncSeparate (GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glBlendFuncSeparate);
  c->srcRGB = srcRGB;
  c->dstRGB = dstRGB;
  c->srcAlpha = srcAlpha;
  c->dstAlpha = dstAlpha;
  GLS_PUSH_BATCH(glBlendFuncSeparate);
}


GL_APICALL void GL_APIENTRY glBufferData (GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
{
    gls_cmd_flush();
    if (data)
      gls_cmd_send_data((uint32_t)size, (void *)data);
    GLS_SET_COMMAND_PTR(c, glBufferData);
    c->target = target;
    c->size = size;
    c->usage = usage;
    c->has_data = (data != NULL);
    GLS_SEND_PACKET(glBufferData);
}


GL_APICALL void GL_APIENTRY glBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data)
{
    gls_cmd_flush();
    if (data)
      gls_cmd_send_data((uint32_t)size, (void *)data);

    GLS_SET_COMMAND_PTR(c, glBufferSubData);
    c->target = target;
    c->offset = offset;
    c->size = size;
    c->has_data = (data != NULL);
    GLS_SEND_PACKET(glBufferSubData);
}


GL_APICALL GLenum GL_APIENTRY glCheckFramebufferStatus (GLenum target)
{
  gls_cmd_flush();
  GLS_SET_COMMAND_PTR(c, glCheckFramebufferStatus);
  c->target = target;
  GLS_SEND_PACKET(glCheckFramebufferStatus);

  GLS_WAIT_SET_RET_PTR(ret, glCheckFramebufferStatus);
  GLS_RELEASE_RETURN_RET(GLenum, ret, status);
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


GL_APICALL void GL_APIENTRY glCompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid* data)
{
  (void) target; (void) level; (void) internalformat; (void) width; (void) height;
  (void) border; (void) imageSize; (void) data;
  WARN_STUBBED();
}

GL_APICALL void GL_APIENTRY glCompressedTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid* data)
{
  (void) target; (void) level; (void) xoffset; (void)yoffset; (void) width; (void) height;
  (void) format; (void) imageSize; (void) data;
  WARN_STUBBED();
}


GL_APICALL void GL_APIENTRY glCopyTexImage2D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
  (void) target; (void) level; (void) internalformat; (void) x; (void) y;
  (void) width; (void) height; (void) border;
  WARN_STUBBED();
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

    GLS_WAIT_SET_RET_PTR(ret, glCreateProgram);
    GLS_RELEASE_RETURN_RET(GLuint, ret, program);
}


GL_APICALL GLuint GL_APIENTRY glCreateShader (GLenum type)
{
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, glCreateShader);
    c->type = type;
    GLS_SEND_PACKET(glCreateShader);

    GLS_WAIT_SET_RET_PTR(ret, glCreateShader);
    GLS_RELEASE_RETURN_RET(GLuint, ret, obj);
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
  _Static_assert(sizeof(GLuint) == sizeof(uint32_t), "int size mismatch");

  if (buffer_objs.vbo || buffer_objs.ibo)
    for (int i=0; i<n; i++) {
      // deleting a buffer causes bindings to be reset to 0, emulate that
      if (buffers[i] == buffer_objs.vbo) buffer_objs.vbo = 0;
      if (buffers[i] == buffer_objs.ibo) buffer_objs.ibo = 0;
    }

  uint32_t size = n * sizeof(uint32_t);
  gls_cmd_send_data(size, (void *)buffers);

  GLS_SET_COMMAND_PTR(c, glDeleteBuffers);
  c->n = n;
  GLS_SEND_PACKET(glDeleteBuffers);
}


GL_APICALL void GL_APIENTRY glDeleteFramebuffers (GLsizei n, const GLuint* framebuffers)
{
  uint32_t datasize = n * sizeof(uint32_t);
  GLS_SET_COMMAND_PTR_BATCH(c, glDeleteFramebuffers);
  c->cmd_size += datasize;
  if (!check_batch_overflow(c->cmd_size, "glDeleteFramebuffers")) {
    return;
  }
  c->n = n;
  memcpy(c->framebuffers, framebuffers, datasize);
  push_batch_command();
  gls_cmd_flush();
}


GL_APICALL void GL_APIENTRY glDeleteProgram (GLuint program)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glDeleteProgram);
  c->program = program;
  GLS_PUSH_BATCH(glDeleteProgram);
}


GL_APICALL void GL_APIENTRY glDeleteRenderbuffers (GLsizei n, const GLuint* renderbuffers)
{
  uint32_t datasize = n * sizeof(uint32_t);
  GLS_SET_COMMAND_PTR_BATCH(c, glDeleteRenderbuffers);
  c->cmd_size += datasize;
  if (!check_batch_overflow(c->cmd_size, "glDeleteRenderbuffers")) {
    return;
  }
  c->n = n;
  memcpy(c->renderbuffers, renderbuffers, datasize);
  push_batch_command();
  gls_cmd_flush();
}


GL_APICALL void GL_APIENTRY glDeleteShader (GLuint shader)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glDeleteShader);
  c->shader = shader;
  GLS_PUSH_BATCH(glDeleteShader);
}


GL_APICALL void GL_APIENTRY glDeleteTextures (GLsizei n, const GLuint* textures)
{
  uint32_t datasize = n * sizeof(uint32_t);
  GLS_SET_COMMAND_PTR_BATCH(c, glDeleteTextures);
  c->cmd_size += datasize;
  if (!check_batch_overflow(c->cmd_size, "glDeleteTextures")) {
    return;
  }
  c->n = n;
  memcpy(c->textures, textures, datasize);
  push_batch_command();
  gls_cmd_flush();
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


GL_APICALL void GL_APIENTRY glDetachShader (GLuint program, GLuint shader)
{
  (void)program; (void)shader;
  WARN_STUBBED();
}


GL_APICALL void GL_APIENTRY glDisable (GLenum cap)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glDisable);
  c->cap = cap;
  GLS_PUSH_BATCH(glDisable);
}


GL_APICALL void GL_APIENTRY glDisableVertexAttribArray (GLuint index)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glDisableVertexAttribArray);
  c->index = index;
  GLS_PUSH_BATCH(glDisableVertexAttribArray);

  // FIXME: should we wait and check for error ?
  buffer_objs.attrib_pointer[index].isenabled = GL_FALSE;
}


/*
 * Send defered glVertexAttribPointer after we set up a VBO
 */
static void _send_glVertexAttribPointer(
    GLuint indx, GLint size, GLenum type, GLboolean normalized,
    GLsizei stride, const GLvoid* ptr)
{
    GLS_SET_COMMAND_PTR_BATCH(c, glVertexAttribPointer);
    c->indx = indx;
    c->size = size;
    c->type = type;
    c->normalized = normalized;
    c->stride = stride;

    c->ptr_uint = (uint64_t)ptr;
    GLS_PUSH_BATCH(glVertexAttribPointer);
}

static void defered_vertex_attrib_pointer(int i, int count)
{
    int stride;
    struct attrib_pointer_t* const attrib = &buffer_objs.attrib_pointer[i];

    if (!attrib->isenabled)
        return;
    if (attrib->vbo_id)
        // uses a VBO, already sent
        return;

    if (!attrib->emul_vbo_id)
        glGenBuffers(1, &attrib->emul_vbo_id);

    stride = attrib->stride;
    if (stride == 0)
        stride = attrib->size * _type_bytesize(attrib->type);

    glBindBuffer(GL_ARRAY_BUFFER, attrib->emul_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, count * stride, (void*)attrib->ptr, GL_STREAM_DRAW);
    _send_glVertexAttribPointer(i, attrib->size, attrib->type, attrib->normalized, attrib->stride, 0);
}


GL_APICALL void GL_APIENTRY glDrawArrays (GLenum mode, GLint first, GLsizei count)
{
  int vbo_bkp = buffer_objs.vbo;
  int i;
  for (i = 0;i < 16; i++)
    defered_vertex_attrib_pointer(i, first + count);
  glBindBuffer( GL_ARRAY_BUFFER, vbo_bkp );

  GLS_SET_COMMAND_PTR_BATCH(c, glDrawArrays);
  c->mode = mode;
  c->first = first;
  c->count = count;
  GLS_PUSH_BATCH(glDrawArrays);

  // FIXME should free VBO's emulating client arrays
}


GL_APICALL void GL_APIENTRY glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)
{
  uint32_t sizeoftype;
  if (type == GL_UNSIGNED_BYTE)
    sizeoftype = sizeof(GLubyte);
  else // GL_UNSIGNED_SHORT
    sizeoftype = sizeof(GLushort);

  int vbo_bkp = buffer_objs.vbo;
  int ibo_bkp = buffer_objs.ibo;
  int i;
  for (i = 0; i < 16; i++) {
    defered_vertex_attrib_pointer(i, 65536); // FIXME count
  }
  if( !buffer_objs.ibo ) {
    if( !buffer_objs.ibo_emu ) {
      glGenBuffers(1, &buffer_objs.ibo_emu);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_objs.ibo_emu);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeoftype, indices, GL_STREAM_DRAW);
  }
    
  GLS_SET_COMMAND_PTR_BATCH(c, glDrawElements);
  c->mode = mode;
  c->count = count;
  c->type = type;
    
  GLS_PUSH_BATCH(glDrawElements);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_bkp);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_bkp);

  // FIXME should free VBO's and IBO emulating client arrays
}


GL_APICALL void GL_APIENTRY glEnable (GLenum cap)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glEnable);
  c->cap = cap;
  GLS_PUSH_BATCH(glEnable);
}


GL_APICALL void GL_APIENTRY glEnableVertexAttribArray (GLuint index)
{
    GLS_SET_COMMAND_PTR_BATCH(c, glEnableVertexAttribArray);
    c->index = index;
    GLS_PUSH_BATCH(glEnableVertexAttribArray);

    // FIXME: should we wait and check for error ?
    buffer_objs.attrib_pointer[index].isenabled = GL_TRUE;
}


GL_APICALL void GL_APIENTRY glFinish (void)
{
  gls_cmd_flush();
  GLS_SET_COMMAND_PTR(c, glFinish);
  GLS_SEND_PACKET(glFinish);
}


GL_APICALL void GL_APIENTRY glFlush (void)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glFlush);
  GLS_PUSH_BATCH(glFlush);
}

GL_APICALL void GL_APIENTRY glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glFramebufferRenderbuffer);
  c->target = target;
  c->attachment = attachment;
  c->renderbuffertarget = renderbuffertarget;
  c->renderbuffer = renderbuffer;
  GLS_PUSH_BATCH(glFramebufferRenderbuffer);
}


GL_APICALL void GL_APIENTRY glFramebufferTexture2D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glFramebufferTexture2D);
  c->target = target;
  c->attachment = attachment;
  c->textarget = textarget;
  c->texture = texture;
  c->level = level;
  GLS_PUSH_BATCH(glFramebufferTexture2D);
}


GL_APICALL void GL_APIENTRY glFrontFace (GLenum mode)
{
  (void)mode;
  WARN_STUBBED();
}


GL_APICALL void GL_APIENTRY glGenBuffers (GLsizei n, GLuint* buffers)
{
  gls_cmd_flush();
  GLS_SET_COMMAND_PTR(c, glGenBuffers);
  c->n = n;
  GLS_SEND_PACKET(glGenBuffers);

  GLS_WAIT_SET_RAWRET_PTR(ret, void, glGenBuffers);
  memcpy(buffers, ret, c->n * sizeof(uint32_t));
}


GL_APICALL void GL_APIENTRY glGenerateMipmap (GLenum target)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glGenerateMipmap);
  c->target = target;
  GLS_PUSH_BATCH(glGenerateMipmap);
}


GL_APICALL void GL_APIENTRY glGenFramebuffers (GLsizei n, GLuint* framebuffers)
{
  gls_cmd_flush();
  GLS_SET_COMMAND_PTR(c, glGenFramebuffers);
  c->n = n;
  GLS_SEND_PACKET(glGenFramebuffers);

  GLS_WAIT_SET_RAWRET_PTR(ret, void, glGenFramebuffers);
  memcpy(framebuffers, ret, c->n * sizeof(uint32_t));
}


GL_APICALL void GL_APIENTRY glGenRenderbuffers(GLsizei n, GLuint* renderbuffers)
{
  gls_cmd_flush();
  GLS_SET_COMMAND_PTR(c, glGenRenderbuffers);
  c->n = n;
  GLS_SEND_PACKET(glGenRenderbuffers);

  GLS_WAIT_SET_RAWRET_PTR(ret, void, glGenRenderbuffers);
  memcpy(renderbuffers, ret, c->n * sizeof(uint32_t));
}


GL_APICALL void GL_APIENTRY glGenTextures (GLsizei n, GLuint* textures)
{
  gls_cmd_flush();
  GLS_SET_COMMAND_PTR(c, glGenTextures);
  c->n = n;
  GLS_SEND_PACKET(glGenTextures);

  GLS_WAIT_SET_RAWRET_PTR(ret, void, glGenTextures);
  memcpy(textures, ret, c->n * sizeof(uint32_t));
}


GL_APICALL void GL_APIENTRY glGetActiveAttrib (GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name)
{
    WARN_UNTESTED();
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, glGetActiveAttrib);
    c->program = program;
    c->index = index;
    c->bufsize = bufsize;

    GLS_SET_RET_PTR(ret, glGetActiveAttrib);
    if ((unsigned)bufsize > sizeof(ret->name)) {
        c->bufsize = sizeof(ret->name);
        fprintf(stderr,
                "GLS WARNING: %s: buffer for attrib name limited by protocol: %u => %u\n",
                __FUNCTION__, bufsize, c->bufsize);
    }
    GLS_SEND_PACKET(glGetActiveAttrib);
    
    GLS_WAIT_RET(glGetActiveAttrib);
    
    if (length != NULL) {
        *length = ret->length;
    }
    *size = ret->size;
    *type = ret->type;
    if (ret->length == 0) {
        ret->name[0] = '\0';
    }
    strncpy(name, ret->name, (size_t)bufsize);
    GLS_RELEASE_RET();
}


GL_APICALL void GL_APIENTRY glGetActiveUniform (GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name)
{
    WARN_UNTESTED();
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, glGetActiveUniform);
    c->program = program;
    c->index = index;
    c->bufsize = bufsize;

    GLS_SET_RET_PTR(ret, glGetActiveUniform);
    if ((unsigned)bufsize > sizeof(ret->name)) {
        c->bufsize = sizeof(ret->name);
        fprintf(stderr,
                "GLS WARNING: %s: buffer for uniform name limited by protocol: %u => %u\n",
                __FUNCTION__, bufsize, c->bufsize);
    }
    GLS_SEND_PACKET(glGetActiveUniform);
    
    GLS_WAIT_RET(glGetActiveUniform);
    
    if (length != NULL) {
        *length = ret->length;
    }
    *size = ret->size;
    *type = ret->type;
    
    if (ret->length == 0) {
        ret->name[0] = '\0';
    }
    strncpy(name, ret->name, (size_t)bufsize);
    GLS_RELEASE_RET();
}


GL_APICALL void GL_APIENTRY glGetAttachedShaders (GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders)
{
  (void)program; (void)maxcount; (void)count; (void)shaders;
  WARN_STUBBED();
}


GL_APICALL GLint GL_APIENTRY glGetAttribLocation (GLuint program, const GLchar* name)
{
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, glGetAttribLocation);
    c->program = program;
    if (strlen(name) + 1 > sizeof(c->name)) {
        fprintf(stderr, "GLS ERROR: %s passed a name too long for protocol, len(%s) > %zu\n",
                __FUNCTION__, name, sizeof(c->name));
        client_gles_error = GL_INVALID_OPERATION;
        return -1;
    }
    strcpy(c->name, name);
    GLS_SEND_PACKET(glGetAttribLocation);

    GLS_WAIT_SET_RET_PTR(ret, glGetAttribLocation);
    GLS_RELEASE_RETURN_RET(GLint, ret, index);
}

GL_APICALL void GL_APIENTRY glGetBooleanv (GLenum pname, GLboolean* params)
{
  (void)pname; (void)params;
  WARN_STUBBED();
}


GL_APICALL void GL_APIENTRY glGetBufferParameteriv (GLenum target, GLenum pname, GLint* params)
{
  (void)target; (void)pname; (void)params;
  WARN_STUBBED();
}


GL_APICALL GLenum GL_APIENTRY glGetError()
{
    if (client_gles_error != GL_NO_ERROR)
        return client_gles_error;

    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, glGetError);
    GLS_SEND_PACKET(glGetError);
    
    GLS_WAIT_SET_RET_PTR(ret, glGetError);
    GLS_RELEASE_RETURN_RET(GLenum, ret, error);
}


GL_APICALL void GL_APIENTRY glGetFloatv(GLenum name, GLfloat *params)
{
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, glGetFloatv);
    c->name = name;
    GLS_SEND_PACKET(glGetFloatv);
    
    GLS_WAIT_SET_RET_PTR(ret, glGetFloatv);
    *params = ret->params;
    GLS_RELEASE_RET();
}


GL_APICALL void GL_APIENTRY glGetFramebufferAttachmentParameteriv (GLenum target, GLenum attachment, GLenum pname, GLint* params)
{
  (void)target; (void)attachment; (void)pname; (void)params;
  WARN_STUBBED();
}


GL_APICALL void GL_APIENTRY glGetIntegerv(GLenum name, GLint *params)
{
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, glGetIntegerv);
    c->name = name;
    GLS_SEND_PACKET(glGetIntegerv);
    
    GLS_WAIT_SET_RET_PTR(ret, glGetIntegerv);
    *params = ret->params;
    GLS_RELEASE_RET();
}


GL_APICALL void GL_APIENTRY glGetProgramiv (GLuint program, GLenum pname, GLint* params)
{
    gls_cmd_flush();
    GLS_SET_COMMAND_PTR(c, glGetProgramiv);
    c->program = program;
    c->pname = pname;
    GLS_SEND_PACKET(glGetProgramiv);

    GLS_WAIT_SET_RET_PTR(ret, glGetProgramiv);
    *params = ret->params;
    GLS_RELEASE_RET();
}


GL_APICALL void GL_APIENTRY glGetProgramInfoLog (GLuint program, GLsizei bufsize, GLsizei* length, GLchar* infolog)
{
  gls_cmd_flush();
  GLS_SET_COMMAND_PTR(c, glGetProgramInfoLog);
  c->program = program;
  c->bufsize = bufsize;
  GLS_SEND_PACKET(glGetProgramInfoLog);

  GLS_WAIT_SET_RET_PTR(ret, glGetProgramInfoLog);
  if (length != NULL) {
    *length = ret->length;
  }
  if (ret->length == 0) {
    ret->infolog[0] = '\0';
  }
  strncpy(infolog, ret->infolog, (size_t)bufsize);
  GLS_RELEASE_RET();
}


GL_APICALL void GL_APIENTRY glGetRenderbufferParameteriv (GLenum target, GLenum pname, GLint* params)
{
  (void)target; (void)pname; (void)params;
  WARN_STUBBED();
}


GL_APICALL void GL_APIENTRY glGetShaderiv (GLuint shader, GLenum pname, GLint* params)
{
  gls_cmd_flush();
  GLS_SET_COMMAND_PTR(c, glGetShaderiv);
  c->shader = shader;
  c->pname = pname;
  GLS_SEND_PACKET(glGetShaderiv);

  GLS_WAIT_SET_RET_PTR(ret, glGetShaderiv);
  *params = ret->params;
}


GL_APICALL void GL_APIENTRY glGetShaderInfoLog (GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* infolog)
{
  gls_cmd_flush();
  GLS_SET_COMMAND_PTR(c, glGetShaderInfoLog);
  c->shader = shader;
  c->bufsize = bufsize;
  GLS_SEND_PACKET(glGetShaderInfoLog);

  GLS_WAIT_SET_RET_PTR(ret, glGetShaderInfoLog);
  if (length != NULL) {
    *length = ret->length;
  }
  if (ret->length == 0) {
    ret->infolog[0] = '\0';
  }
  strncpy(infolog, ret->infolog, (size_t)bufsize);
  GLS_RELEASE_RET();
}


GL_APICALL void GL_APIENTRY glGetShaderPrecisionFormat (GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision)
{
  (void)shadertype; (void)precisiontype; (void)range; (void)precision;
  WARN_STUBBED();
}


GL_APICALL void GL_APIENTRY glGetShaderSource (GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* source)
{
  (void)shader; (void)bufsize; (void)length; (void)source;
  WARN_STUBBED();
}

// glGetString (with caching)

static const GLubyte* GL_APIENTRY _real_glGetString(GLenum name)
{
  gls_cmd_flush();
  GLS_SET_COMMAND_PTR(c, glGetString);
  c->name = name;
  GLS_SEND_PACKET(glGetString);

  GLS_WAIT_SET_RET_PTR(ret, glGetString);
  if (!ret->success) {
    GLS_RELEASE_RET();
    return NULL;
  }
  // must defer ret freeing until after copy
  return (GLubyte*)ret->params;
}

#define GLS_GLESGETSTRING_ITEMS()               \
  EMPTY()                                       \
    X(VENDOR)                                   \
    X(RENDERER)                                 \
    X(VERSION)                                  \
    X(SHADING_LANGUAGE_VERSION)                 \
    X(EXTENSIONS)                               \
  //

static struct {
  char* storage;
  size_t allocated;
  size_t nfilled;

#define X(FIELD) const char* FIELD##_str;
  GLS_GLESGETSTRING_ITEMS();
#undef X
} gles_strings;

// record a string into gl_strings
static int _registerGlesString(GLenum name, const char** field_p)
{
  const char* value = (const char*)_real_glGetString(name);
  if (!value) {
    GLenum error = glGetError();
    if (error != GL_INVALID_ENUM)
      fprintf(stderr, "GLS ERROR: glGetString(0x%x) failed, error 0x%x\n",
              name, error);
    return 0;
  }
  int valuesize = strlen(value) + 1;
  while (gles_strings.nfilled + valuesize > gles_strings.allocated) {
    gles_strings.allocated *= 2;
    //fprintf(stderr, "GLS DBG: glGetString reallocating %zu\n", gles_strings.allocated);
    void* newstorage = realloc(gles_strings.storage, gles_strings.allocated);
    if (!newstorage) {
      fprintf(stderr, "GLS ERROR: glGetString reallocation failed: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
    gles_strings.storage = newstorage;
  }
  char* field = gles_strings.storage + gles_strings.nfilled;
  strcpy(field, value);
  *field_p = field;
  gles_strings.nfilled += valuesize;
  GLS_RELEASE_RET();
  return 1;
}

static void _populate_gles_strings()
{
  assert(!gles_strings.allocated);
  gles_strings.allocated = 1024; // rather arbitrary
  gles_strings.storage = malloc(gles_strings.allocated);
  if (!gles_strings.storage) {
    fprintf(stderr, "GLS ERROR: glGetString allocation failed: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

#define X(FIELD)                                                    \
  if (!_registerGlesString(GL_##FIELD, &gles_strings.FIELD##_str))  \
    exit(EXIT_FAILURE);                                             \
  //

  GLS_GLESGETSTRING_ITEMS();
#undef X
}

GL_APICALL const GLubyte* GL_APIENTRY glGetString(GLenum name)
{
  // init storage
  if (!gles_strings.storage)
    _populate_gles_strings();

  switch(name) {
#define X(FIELD)                                  \
    case GL_##FIELD:                              \
      if (!gles_strings.FIELD##_str) {            \
        client_gles_error = GL_INVALID_ENUM;      \
        return NULL;                              \
      }                                           \
      client_gles_error = GL_NO_ERROR;            \
      return (GLubyte*)gles_strings.FIELD##_str;  \
      //
    GLS_GLESGETSTRING_ITEMS();
#undef X

  default:
    client_gles_error = GL_INVALID_ENUM;
    return NULL;
  }
}

GL_APICALL void GL_APIENTRY glGetTexParameterfv (GLenum target, GLenum pname, GLfloat* params)
{
  (void)target; (void)pname; (void)params;
  WARN_STUBBED();
}


GL_APICALL void GL_APIENTRY glGetTexParameteriv (GLenum target, GLenum pname, GLint* params)
{
  (void)target; (void)pname; (void)params;
  WARN_STUBBED();
}


GL_APICALL void GL_APIENTRY glGetUniformfv (GLuint program, GLint location, GLfloat* params)
{
  (void)program; (void)location; (void)params;
  WARN_STUBBED();
}


GL_APICALL void GL_APIENTRY glGetUniformiv (GLuint program, GLint location, GLint* params)
{
  (void)program; (void)location; (void)params;
  WARN_STUBBED();
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

  GLS_WAIT_SET_RET_PTR(ret, glGetUniformLocation);
  GLS_RELEASE_RETURN_RET(int, ret, location);
}


GL_APICALL void GL_APIENTRY glGetVertexAttribfv (GLuint index, GLenum pname, GLfloat* params)
{
  (void)index; (void)pname; (void)params;
  WARN_STUBBED();
}


GL_APICALL void GL_APIENTRY glGetVertexAttribiv (GLuint index, GLenum pname, GLint* params)
{
  (void)index; (void)pname; (void)params;
  WARN_STUBBED();
}


GL_APICALL void GL_APIENTRY glGetVertexAttribPointerv (GLuint index, GLenum pname, GLvoid** pointer)
{
  (void)index; (void)pname; (void)pointer;
  WARN_STUBBED();
}


GL_APICALL void GL_APIENTRY glHint (GLenum target, GLenum mode)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glHint);
  c->target = target;
  c->mode = mode;
  GLS_PUSH_BATCH(glHint);
}


GL_APICALL GLboolean GL_APIENTRY glIsBuffer (GLuint buffer)
{
  gls_cmd_flush();
  GLS_SET_COMMAND_PTR(c, glIsBuffer);
  c->buffer = buffer;
  GLS_SEND_PACKET(glIsBuffer);
  GLS_WAIT_SET_RET_PTR(ret, glIsBuffer);
  GLS_RELEASE_RETURN_RET(GLboolean, ret, isbuffer);
}


GL_APICALL GLboolean GL_APIENTRY glIsEnabled (GLenum cap)
{
  gls_cmd_flush();
  GLS_SET_COMMAND_PTR(c, glIsEnabled);
  c->cap = cap;
  GLS_SEND_PACKET(glIsEnabled);
  GLS_WAIT_SET_RET_PTR(ret, glIsEnabled);
  GLS_RELEASE_RETURN_RET(GLboolean, ret, isenabled);
}


GL_APICALL GLboolean GL_APIENTRY glIsFramebuffer (GLuint framebuffer)
{
  (void)framebuffer;
  WARN_STUBBED();
  return FALSE;
}


GL_APICALL GLboolean GL_APIENTRY glIsProgram (GLuint program)
{
  (void)program;
  WARN_STUBBED();
  return FALSE;
}


GL_APICALL GLboolean GL_APIENTRY glIsRenderbuffer (GLuint renderbuffer)
{
  (void)renderbuffer;
  WARN_STUBBED();
  return FALSE;
}


GL_APICALL GLboolean GL_APIENTRY glIsShader (GLuint shader)
{
  (void)shader;
  WARN_STUBBED();
  return FALSE;
}


GL_APICALL GLboolean GL_APIENTRY glIsTexture (GLuint texture)
{
  (void)texture;
  WARN_STUBBED();
  return FALSE;
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
    
    GLS_WAIT_SET_RET_PTR(ret, glReadPixels);
    uint32_t pixelbytes = _pixelformat_to_bytes(format, type);
    uint32_t linebytes = ((pixelbytes * width + glsc_global.pack_alignment - 1) &
                          (~ (glsc_global.pack_alignment - 1)));
    memcpy(pixels, ret->pixels, linebytes * height);
    GLS_RELEASE_RET();
}


GL_APICALL void GL_APIENTRY glReleaseShaderCompiler (void)
{
  WARN_STUBBED();
}


GL_APICALL void GL_APIENTRY glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glRenderbufferStorage);
  c->target = target;
  c->internalformat = internalformat;
  c->width = width;
  c->height = height;
  GLS_PUSH_BATCH(glRenderbufferStorage);
}


GL_APICALL void GL_APIENTRY glSampleCoverage (GLclampf value, GLboolean invert)
{
  (void)value; (void)invert;
  WARN_STUBBED();
}


GL_APICALL void GL_APIENTRY glScissor (GLint x, GLint y, GLsizei width, GLsizei height)
{
  (void)x; (void)y; (void)width; (void)height;
  WARN_STUBBED();
}


GL_APICALL void GL_APIENTRY glShaderBinary (GLsizei n, const GLuint* shaders, GLenum binaryformat, const GLvoid* binary, GLsizei length)
{
  (void)n; (void)shaders; (void)binaryformat; (void)binary; (void)length;
  WARN_STUBBED();
}


GL_APICALL void GL_APIENTRY glShaderSource (GLuint shader, GLsizei count, const GLchar*const* string, const GLint* length)
{
  gls_cmd_flush();
  if (count > 10240) { // 256
    printf("gls warning: shader too large, over 10kb, ignoring.\n"); // FIXME why!?
    return;
  }
  gls_data_glShaderSource_t *dat = (gls_data_glShaderSource_t *)glsc_global.pool.tmp_buf.buf;
  size_t size_all = (size_t)(dat->data - (char *)dat);
  
  // printf("\n ----- BEGIN SHADER CONTENT -----\n");
  uint32_t stroffset = 0;
  int i;

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
    dat->offsets[i] = stroffset;
    dat->length[i] = strsize;
    memcpy(&dat->data[stroffset], strptr, strsize + 1);
    dat->data[stroffset + strsize] = '\0';
    stroffset += strsize + 1;
    
    // printf("gls debug: shader length = %i\n", strsize);
    
    // printf("%s\n", strptr);
  }

  // printf(" ----- ENDED SHADER CONTENT -----\n\n");
  
  gls_cmd_send_data(size_all, glsc_global.pool.tmp_buf.buf);
  GLS_SET_COMMAND_PTR(c, glShaderSource);
  c->shader = shader;
  c->count = count;
  GLS_SEND_PACKET(glShaderSource);
}


GL_APICALL void GL_APIENTRY glStencilFunc(GLenum func, GLint r, GLuint m)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glStencilFunc);
  c->func = func;
  c->r = r;
  c->m = m;
  GLS_PUSH_BATCH(glStencilFunc);
}


GL_APICALL void GL_APIENTRY glStencilFuncSeparate (GLenum face, GLenum func, GLint ref, GLuint mask)
{
  (void)face; (void)func; (void)ref; (void)mask;
  WARN_STUBBED();
}


GL_APICALL void GL_APIENTRY glStencilMask (GLuint mask)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glStencilMask);
  c->mask = mask;
  GLS_PUSH_BATCH(glStencilMask);
}


GL_APICALL void GL_APIENTRY glStencilMaskSeparate (GLenum face, GLuint mask)
{
  (void)face; (void)mask;
  WARN_STUBBED();
}


GL_APICALL void GL_APIENTRY glStencilOp (GLenum fail, GLenum zfail, GLenum zpass)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glStencilOp);
  c->fail = fail;
  c->zfail = zfail;
  c->zpass = zpass;
  GLS_PUSH_BATCH(glStencilOp);
}


GL_APICALL void GL_APIENTRY glStencilOpSeparate (GLenum face, GLenum fail, GLenum zfail, GLenum zpass)
{
  (void)face; (void)fail; (void)zfail; (void)zpass;
  WARN_STUBBED();
}


GL_APICALL void GL_APIENTRY glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels)
{
  gls_cmd_flush();
  if (pixels) {
    uint32_t pixelbytes = _pixelformat_to_bytes(format, type);
    uint32_t linebytes = (pixelbytes * width + glsc_global.unpack_alignment - 1) & (~ (glsc_global.unpack_alignment - 1));
    gls_cmd_send_data(linebytes * height, pixels);
  }

  GLS_SET_COMMAND_PTR(c, glTexImage2D);
  c->target = target;
  c->level = level;
  c->internalformat = internalformat;
  c->width = width;
  c->height = height;
  c->border = border;
  c->format = format;
  c->type = type;
  c->has_pixels = (pixels != NULL);
  GLS_SEND_PACKET(glTexImage2D);
}


GL_APICALL void GL_APIENTRY glTexParameterf (GLenum target, GLenum pname, GLfloat param)
{
  (void)target; (void)pname; (void)param;
  WARN_STUBBED();
}


GL_APICALL void GL_APIENTRY glTexParameterfv (GLenum target, GLenum pname, const GLfloat* params)
{
  (void)target; (void)pname; (void)params;
  WARN_STUBBED();
}


GL_APICALL void GL_APIENTRY glTexParameteri (GLenum target, GLenum pname, GLint param)
{
  GLS_SET_COMMAND_PTR_BATCH(c, glTexParameteri);
  c->target = target;
  c->pname = pname;
  c->param = param;
  GLS_PUSH_BATCH(glTexParameteri);
}


GL_APICALL void GL_APIENTRY glTexParameteriv (GLenum target, GLenum pname, const GLint* params)
{
  (void)target; (void)pname; (void)params;
  WARN_STUBBED();
}


GL_APICALL void GL_APIENTRY glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels)
{
  gls_cmd_flush();
  if (pixels) {
    uint32_t pixelbytes = _pixelformat_to_bytes(format, type);
    uint32_t linebytes = (pixelbytes * width + glsc_global.unpack_alignment - 1) & (~ (glsc_global.unpack_alignment - 1));
    gls_cmd_send_data(linebytes * height, pixels);
  }

  GLS_SET_COMMAND_PTR_BATCH(c, glTexSubImage2D);
  c->target = target;
  c->level = level;
  c->xoffset = xoffset;
  c->yoffset = yoffset;
  c->width = width;
  c->height = height;
  c->format = format;
  c->type = type;
  c->has_pixels = (pixels != NULL);
  GLS_SEND_PACKET(glTexSubImage2D);
}


#define IMPLEM_glUniformNX(FUNC, COPYARGS, ...)                         \
  GL_APICALL void GL_APIENTRY FUNC(GLint location, __VA_ARGS__)         \
  {                                                                     \
    GLS_SET_COMMAND_PTR_BATCH(c, FUNC);                                 \
    c->location = location;                                             \
    COPYARGS;                                                           \
    GLS_PUSH_BATCH(glUniform1f);                                        \
  }

IMPLEM_glUniformNX(glUniform1f, c->x = x;, GLfloat x);
IMPLEM_glUniformNX(glUniform2f, c->x = x; c->y = y;, GLfloat x, GLfloat y);
IMPLEM_glUniformNX(glUniform3f, c->x = x; c->y = y; c->z = z;, GLfloat x, GLfloat y, GLfloat z);
IMPLEM_glUniformNX(glUniform4f, c->x = x; c->y = y; c->z = z; c->w = w;, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
IMPLEM_glUniformNX(glUniform1i, c->x = x;, GLint x);
IMPLEM_glUniformNX(glUniform2i, c->x = x; c->y = y;, GLint x, GLint y);
IMPLEM_glUniformNX(glUniform3i, c->x = x; c->y = y; c->z = z;, GLint x, GLint y, GLint z);
IMPLEM_glUniformNX(glUniform4i, c->x = x; c->y = y; c->z = z; c->w = w;, GLint x, GLint y, GLint z, GLint w);


#define IMPLEM_glUniformNXv(FUNC,N,TYPE)                                \
  GL_APICALL void GL_APIENTRY FUNC(GLint location, GLsizei count, const TYPE* v) \
  {                                                                     \
  uint32_t datasize = count * N * sizeof(TYPE);                         \
  GLS_SET_COMMAND_PTR_BATCH(c, FUNC);                                   \
  c->cmd_size += datasize;                                              \
  if (!check_batch_overflow(c->cmd_size, #FUNC))                        \
    return;                                                             \
  c->location = location;                                               \
  c->count = count;                                                     \
  memcpy(c->v, v, datasize);                                            \
  push_batch_command();                                                 \
  }

IMPLEM_glUniformNXv(glUniform1fv,1,GLfloat);
IMPLEM_glUniformNXv(glUniform2fv,2,GLfloat);
IMPLEM_glUniformNXv(glUniform3fv,3,GLfloat);
IMPLEM_glUniformNXv(glUniform4fv,4,GLfloat);

IMPLEM_glUniformNXv(glUniform1iv,1,GLint);
IMPLEM_glUniformNXv(glUniform2iv,2,GLint);
IMPLEM_glUniformNXv(glUniform3iv,3,GLint);
IMPLEM_glUniformNXv(glUniform4iv,4,GLint);


#define IMPLEM_glUniformMatrixNXv(FUNC,N,TYPE)                          \
  GL_APICALL void GL_APIENTRY FUNC(GLint location, GLsizei count, GLboolean transpose, const TYPE* value) \
  {                                                                     \
    uint32_t datasize = count * N * N * sizeof(TYPE);                   \
    GLS_SET_COMMAND_PTR_BATCH(c, FUNC);                                 \
    c->cmd_size += datasize;                                            \
    if (!check_batch_overflow(c->cmd_size, #FUNC))                      \
      return;                                                           \
    c->location = location;                                             \
    c->count = count;                                                   \
    c->transpose = transpose;                                           \
    memcpy(c->value, value, datasize);                                  \
    push_batch_command();                                               \
  }

IMPLEM_glUniformMatrixNXv(glUniformMatrix2fv,2,GLfloat);
IMPLEM_glUniformMatrixNXv(glUniformMatrix3fv,3,GLfloat);
IMPLEM_glUniformMatrixNXv(glUniformMatrix4fv,4,GLfloat);


GL_APICALL void GL_APIENTRY glUseProgram (GLuint program)
{
    GLS_SET_COMMAND_PTR_BATCH(c, glUseProgram);
    c->program = program;
    GLS_PUSH_BATCH(glUseProgram);
}


static void _glVertexAttribFloat(GLuint index, GLint num_float, GLboolean call_arr, const GLfloat *arr)
{
    gls_cmd_flush();
    gls_data_glVertexAttribFloat_t *dat = (gls_data_glVertexAttribFloat_t *)glsc_global.pool.tmp_buf.buf;
    memcpy(dat->arr, arr, num_float);
    // It's small so use GLS_DATA_SIZE
    gls_cmd_send_data(GLS_DATA_SIZE, glsc_global.pool.tmp_buf.buf);

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


GL_APICALL void GL_APIENTRY glVertexAttribPointer(
    GLuint indx, GLint size, GLenum type, GLboolean normalized,
    GLsizei stride, const GLvoid* ptr)
{
    if (buffer_objs.vbo) {
        // VBO: send now
        _send_glVertexAttribPointer(indx, size, type, normalized, stride, ptr);
        // declare as VBO so it is not sent twice - FIXME use a bool?
        buffer_objs.attrib_pointer[indx].vbo_id = buffer_objs.vbo;
        return;
    }

    /*
     * Defered Vertex Client Array: will upload at glDrawArrays time
     * when data size is known.
     */
    buffer_objs.attrib_pointer[indx].size = size;
    buffer_objs.attrib_pointer[indx].type = type;
    buffer_objs.attrib_pointer[indx].stride = stride;
    buffer_objs.attrib_pointer[indx].normalized = normalized;
    buffer_objs.attrib_pointer[indx].ptr = ptr;
    buffer_objs.attrib_pointer[indx].vbo_id = 0;
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

  GLS_WAIT_SET_RET_PTR(ret, glUnmapBufferOES);
  GLS_RELEASE_RETURN_RET(int, ret, success);
}


// Used for return void commands
/*
GL_APICALL void GL_APIENTRY glCommand (GLparam param)
{
  WARN_STUBBED();
}
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
  
  GLS_WAIT_SET_RET_PTR(ret, glCommand);
  //  *params = ret->params;
  //  GLS_RELEASE_RET();
  // or below if return
  //  GLS_RELEASE_RETURN_RET(GLreturn, ret, returnVal);
}
 */

