// This file declare OpenGL ES methods for streaming

#include "gls_command_gles2.h"
#include "glclient.h"

#include <GLES2/gl2.h>

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Define core API functions as __GLS_##FUNC with FUNC exported as alias
#define GLS_DEF_CORE_API(RET, FUNC, ...)                                \
  __asm__(".global " #FUNC "; .set " #FUNC ", __GLS_"#FUNC);            \
  static GL_APICALL RET GL_APIENTRY __GLS_##FUNC (__VA_ARGS__)

static struct
{
  GLuint vbo, ibo, ibo_emu;
  struct attrib_pointer_t
  {
    GLboolean   isenabled;
    GLint       size;
    GLenum      type;
    GLsizei     stride;
    GLboolean   normalized;
    const GLvoid* ptr;
    GLuint vbo_id;
    GLuint emul_vbo_id;
  } attrib_pointer[16]; // FIXME: GL_MAX_VERTEX_ATTRIBS has no upper limit
} buffer_objs;

static unsigned _type_bytesize(GLenum type)
{
  switch (type) {
  case GL_BOOL: return sizeof(GLboolean);
  case GL_UNSIGNED_BYTE: return sizeof(GLubyte);
  case GL_BYTE: return sizeof(GLbyte);
  case GL_SHORT: return sizeof(GLshort);
  case GL_UNSIGNED_SHORT: return sizeof(GLushort);
  case GL_UNSIGNED_INT: return sizeof(GLuint);
  case GL_INT: return sizeof(GLint);
  case GL_FIXED: return sizeof(GLfixed);
  case GL_FLOAT: return sizeof(GLfloat);
  default:
    fprintf(stderr, "GLS WARNING: %s: unhandled data type %x\n", __FUNCTION__, type);
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
      fprintf(stderr, "GLS WARNING: unhandled pixel format %x\n", format);
      return 4;
    }
  case GL_UNSIGNED_SHORT_5_6_5:
    return 2;
  case GL_UNSIGNED_SHORT_4_4_4_4:
    return 2;
  case GL_UNSIGNED_SHORT_5_5_5_1:
    return 2;
  default:
    fprintf(stderr, "GLS WARNING: unhandled pixel type %x\n", type);
    return 4;
  }
}


GLS_DEF_CORE_API(void, glActiveTexture, GLenum texture)
{
  GLS_SET_COMMAND_PTR(c, glActiveTexture);
  c->texture = texture;
  GLS_SEND_PACKET(glActiveTexture);
}


GLS_DEF_CORE_API(void, glAttachShader, GLuint program, GLuint shader)
{
  GLS_SET_COMMAND_PTR(c, glAttachShader);
  c->program = program;
  c->shader = shader;
  GLS_SEND_PACKET(glAttachShader);
}


GLS_DEF_CORE_API(void, glBindAttribLocation, GLuint program, GLuint index, const GLchar* name)
{
  GLS_SET_COMMAND_PTR(c, glBindAttribLocation);
  c->program = program;
  c->index = index;
  if (strlen(name) + 1 > sizeof(c->name)) {
    fprintf(stderr, "GLS ERROR: %s passed a name too long for protocol, len(%s) > %zu\n",
            __FUNCTION__, name, sizeof(c->name));
    client_gles_error = GL_INVALID_OPERATION;
    return;
  }
  strcpy(c->name, name);
  GLS_SEND_PACKET(glBindAttribLocation);
}


GLS_DEF_CORE_API(void, glBindBuffer, GLenum target, GLuint buffer)
{
  GLS_SET_COMMAND_PTR(c, glBindBuffer);
  c->target = target;
  c->buffer = buffer;
  GLS_SEND_PACKET(glBindBuffer);

  // FIXME: should we wait and check for error ?
  if (target == GL_ARRAY_BUFFER) {
    buffer_objs.vbo = buffer;
  } else if (target == GL_ELEMENT_ARRAY_BUFFER) {
    buffer_objs.ibo = buffer;
  } else
    fprintf(stderr, "GLS ERROR: unsupported buffer type!\n");
}


GLS_DEF_CORE_API(void, glBindFramebuffer, GLenum target, GLuint framebuffer)
{
  GLS_SET_COMMAND_PTR(c, glBindFramebuffer);
  c->target = target;
  c->framebuffer = framebuffer;
  GLS_SEND_PACKET(glBindFramebuffer);
}


GLS_DEF_CORE_API(void, glBindRenderbuffer, GLenum target, GLuint renderbuffer)
{
  GLS_SET_COMMAND_PTR(c, glBindRenderbuffer);
  c->target = target;
  c->renderbuffer = renderbuffer;
  GLS_SEND_PACKET(glBindRenderbuffer);
}


GLS_DEF_CORE_API(void, glBindTexture, GLenum target, GLuint texture)
{
  GLS_SET_COMMAND_PTR(c, glBindTexture);
  c->target = target;
  c->texture = texture;
  GLS_SEND_PACKET(glBindTexture);
}


GLS_DEF_CORE_API(void, glBlendColor, GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
  (void)red; (void)green; (void)blue; (void)alpha;
  WARN_STUBBED();
}


GLS_DEF_CORE_API(void, glBlendEquation,  GLenum mode )
{
  (void)mode;
  WARN_STUBBED();
}


GLS_DEF_CORE_API(void, glBlendEquationSeparate, GLenum modeRGB, GLenum modeAlpha)
{
  GLS_SET_COMMAND_PTR(c, glBlendEquationSeparate);
  c->modeRGB = modeRGB;
  c->modeAlpha = modeAlpha;
  GLS_SEND_PACKET(glBlendEquationSeparate);
}


GLS_DEF_CORE_API(void, glBlendFunc, GLenum sfactor, GLenum dfactor)
{
  GLS_SET_COMMAND_PTR(c, glBlendFunc);
  c->sfactor = sfactor;
  c->dfactor = dfactor;
  GLS_SEND_PACKET(glBlendFunc);
}


GLS_DEF_CORE_API(void, glBlendFuncSeparate, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
  GLS_SET_COMMAND_PTR(c, glBlendFuncSeparate);
  c->srcRGB = srcRGB;
  c->dstRGB = dstRGB;
  c->srcAlpha = srcAlpha;
  c->dstAlpha = dstAlpha;
  GLS_SEND_PACKET(glBlendFuncSeparate);
}


GLS_DEF_CORE_API(void, glBufferData, GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
{
  if (data)
    gls_cmd_send_data((uint32_t)size, (void*)data);
  GLS_SET_COMMAND_PTR(c, glBufferData);
  c->target = target;
  c->size = size;
  c->usage = usage;
  c->has_data = (data != NULL);
  GLS_SEND_PACKET(glBufferData);
}


GLS_DEF_CORE_API(void, glBufferSubData, GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data)
{
  if (data)
    gls_cmd_send_data((uint32_t)size, (void*)data);

  GLS_SET_COMMAND_PTR(c, glBufferSubData);
  c->target = target;
  c->offset = offset;
  c->size = size;
  c->has_data = (data != NULL);
  GLS_SEND_PACKET(glBufferSubData);
}


GLS_DEF_CORE_API(GLenum, glCheckFramebufferStatus, GLenum target)
{
  GLS_SET_COMMAND_PTR(c, glCheckFramebufferStatus);
  c->target = target;
  GLS_SEND_PACKET(glCheckFramebufferStatus);

  GLS_WAIT_SET_RET_PTR(ret, glCheckFramebufferStatus);
  GLS_RELEASE_RETURN_RET(GLenum, ret, status);
}


GLS_DEF_CORE_API(void, glClear, GLbitfield mask)
{
  GLS_SET_COMMAND_PTR(c, glClear);
  c->mask = mask;
  GLS_SEND_PACKET(glClear);
}


GLS_DEF_CORE_API(void, glClearColor, GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
  GLS_SET_COMMAND_PTR(c, glClearColor);
  c->red = red;
  c->green = green;
  c->blue = blue;
  c->alpha = alpha;
  GLS_SEND_PACKET(glClearColor);
}

GLS_DEF_CORE_API(void, glClearDepthf, GLclampf depth)
{
  GLS_SET_COMMAND_PTR(c, glClearDepthf);
  c->depth = depth;
  GLS_SEND_PACKET(glClearDepthf);
}


GLS_DEF_CORE_API(void, glClearStencil, GLint s)
{
  GLS_SET_COMMAND_PTR(c, glClearStencil);
  c->s = s;
  GLS_SEND_PACKET(glClearStencil);
}


GLS_DEF_CORE_API(void, glColorMask, GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
  GLS_SET_COMMAND_PTR(c, glColorMask);
  c->red = red;
  c->green = green;
  c->blue = blue;
  c->alpha = alpha;
  GLS_SEND_PACKET(glColorMask);
}


GLS_DEF_CORE_API(void, glCompileShader, GLuint shader)
{
  GLS_SET_COMMAND_PTR(c, glCompileShader);
  c->shader = shader;
  GLS_SEND_PACKET(glCompileShader);
}


GLS_DEF_CORE_API(void, glCompressedTexImage2D, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid* data)
{
  (void) target; (void) level; (void) internalformat; (void) width; (void) height;
  (void) border; (void) imageSize; (void) data;
  WARN_STUBBED();
}

GLS_DEF_CORE_API(void, glCompressedTexSubImage2D, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid* data)
{
  (void) target; (void) level; (void) xoffset; (void)yoffset; (void) width; (void) height;
  (void) format; (void) imageSize; (void) data;
  WARN_STUBBED();
}


GLS_DEF_CORE_API(void, glCopyTexImage2D, GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
  (void) target; (void) level; (void) internalformat; (void) x; (void) y;
  (void) width; (void) height; (void) border;
  WARN_STUBBED();
}


GLS_DEF_CORE_API(void, glCopyTexSubImage2D, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
  GLS_SET_COMMAND_PTR(c, glCopyTexSubImage2D);
  c->target = target;
  c->level = level;
  c->xoffset = xoffset;
  c->yoffset = yoffset;
  c->x = x;
  c->y = y;
  c->width = width;
  c->height = height;
  GLS_SEND_PACKET(glCopyTexSubImage2D);
}


GLS_DEF_CORE_API(GLuint, glCreateProgram, void)
{
  GLS_SET_COMMAND_PTR(c, glCreateProgram);
  GLS_SEND_PACKET(glCreateProgram);

  GLS_WAIT_SET_RET_PTR(ret, glCreateProgram);
  GLS_RELEASE_RETURN_RET(GLuint, ret, program);
}


GLS_DEF_CORE_API(GLuint, glCreateShader, GLenum type)
{
  GLS_SET_COMMAND_PTR(c, glCreateShader);
  c->type = type;
  GLS_SEND_PACKET(glCreateShader);

  GLS_WAIT_SET_RET_PTR(ret, glCreateShader);
  GLS_RELEASE_RETURN_RET(GLuint, ret, obj);
}


GLS_DEF_CORE_API(void, glCullFace, GLenum mode)
{
  GLS_SET_COMMAND_PTR(c, glCullFace);
  c->mode = mode;
  GLS_SEND_PACKET(glCullFace);
}


GLS_DEF_CORE_API(void, glDeleteBuffers, GLsizei n, const GLuint* buffers)
{
  _Static_assert(sizeof(GLuint) == sizeof(uint32_t), "int size mismatch");

  if (buffer_objs.vbo || buffer_objs.ibo)
    for (int i = 0; i < n; i++) {
      // deleting a buffer causes bindings to be reset to 0, emulate that
      if (buffers[i] == buffer_objs.vbo) buffer_objs.vbo = 0;
      if (buffers[i] == buffer_objs.ibo) buffer_objs.ibo = 0;
    }

  uint32_t size = n * sizeof(uint32_t);
  gls_cmd_send_data(size, (void*)buffers);

  GLS_SET_COMMAND_PTR(c, glDeleteBuffers);
  c->n = n;
  GLS_SEND_PACKET(glDeleteBuffers);
}


GLS_DEF_CORE_API(void, glDeleteFramebuffers, GLsizei n, const GLuint* framebuffers)
{
  uint32_t datasize = n * sizeof(uint32_t);
  GLS_SET_COMMAND_PTR(c, glDeleteFramebuffers);
  c->n = n;
  GLS_VARIABLE_PAYLOAD(c, framebuffers, datasize);
  send_packet();
}


GLS_DEF_CORE_API(void, glDeleteProgram, GLuint program)
{
  GLS_SET_COMMAND_PTR(c, glDeleteProgram);
  c->program = program;
  GLS_SEND_PACKET(glDeleteProgram);
}


GLS_DEF_CORE_API(void, glDeleteRenderbuffers, GLsizei n, const GLuint* renderbuffers)
{
  uint32_t datasize = n * sizeof(uint32_t);
  GLS_SET_COMMAND_PTR(c, glDeleteRenderbuffers);
  c->n = n;
  GLS_VARIABLE_PAYLOAD(c, renderbuffers, datasize);
  send_packet();
}


GLS_DEF_CORE_API(void, glDeleteShader, GLuint shader)
{
  GLS_SET_COMMAND_PTR(c, glDeleteShader);
  c->shader = shader;
  GLS_SEND_PACKET(glDeleteShader);
}


GLS_DEF_CORE_API(void, glDeleteTextures, GLsizei n, const GLuint* textures)
{
  uint32_t datasize = n * sizeof(uint32_t);
  GLS_SET_COMMAND_PTR(c, glDeleteTextures);
  c->n = n;
  GLS_VARIABLE_PAYLOAD(c, textures, datasize);
  send_packet();
}


GLS_DEF_CORE_API(void, glDepthFunc, GLenum func)
{
  GLS_SET_COMMAND_PTR(c, glDepthFunc);
  c->func = func;
  GLS_SEND_PACKET(glDepthFunc);
}


GLS_DEF_CORE_API(void, glDepthMask, GLboolean flag)
{
  GLS_SET_COMMAND_PTR(c, glDepthMask);
  c->flag = flag;
  GLS_SEND_PACKET(glDepthMask);
}


GLS_DEF_CORE_API(void, glDepthRangef, GLclampf zNear, GLclampf zFar)
{
  GLS_SET_COMMAND_PTR(c, glDepthRangef);
  c->zNear = zNear;
  c->zFar = zFar;
  GLS_SEND_PACKET(glDepthRangef);
}


GLS_DEF_CORE_API(void, glDetachShader, GLuint program, GLuint shader)
{
  (void)program; (void)shader;
  WARN_STUBBED();
}


GLS_DEF_CORE_API(void, glDisable, GLenum cap)
{
  GLS_SET_COMMAND_PTR(c, glDisable);
  c->cap = cap;
  GLS_SEND_PACKET(glDisable);
}


GLS_DEF_CORE_API(void, glDisableVertexAttribArray, GLuint index)
{
  GLS_SET_COMMAND_PTR(c, glDisableVertexAttribArray);
  c->index = index;
  GLS_SEND_PACKET(glDisableVertexAttribArray);

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
  GLS_SET_COMMAND_PTR(c, glVertexAttribPointer);
  c->indx = indx;
  c->size = size;
  c->type = type;
  c->normalized = normalized;
  c->stride = stride;

  c->ptr_uint = (uint64_t)ptr;
  GLS_SEND_PACKET(glVertexAttribPointer);
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

  if (buffer_objs.ibo && !attrib->vbo_id)
      // FIXME: could be done, would require scanning scanning IBO,
      // which requires work in in glBuffer(Sub)Data ... but who needs
      // that for real ?
      WARN_ONCE("GLS WARNING: %s use of client-data vertex buffer together with an IBO is not implemented\n", __FUNCTION__);

  if (!attrib->emul_vbo_id)
    glGenBuffers(1, &attrib->emul_vbo_id);

  stride = attrib->stride;
  if (stride == 0)
    stride = attrib->size * _type_bytesize(attrib->type);

  if (glsc_global.is_debug)
    fprintf(stderr, "GLS DBG: %s: uploading %u bytes to emulation VBO\n", __FUNCTION__,
            count * stride);
  glBindBuffer(GL_ARRAY_BUFFER, attrib->emul_vbo_id);
  glBufferData(GL_ARRAY_BUFFER, count * stride, (void*)attrib->ptr, GL_STREAM_DRAW);
  _send_glVertexAttribPointer(i, attrib->size, attrib->type, attrib->normalized, attrib->stride, 0);
}


GLS_DEF_CORE_API(void, glDrawArrays, GLenum mode, GLint first, GLsizei count)
{
  GLuint vbo_bkp = buffer_objs.vbo;
  int i;
  if (!buffer_objs.vbo)
    WARN_ONCE("GLS WARNING: %s uses client-data vertex buffer, consider using a VBO\n", __FUNCTION__);
  for (i = 0; i < 16; i++)
    defered_vertex_attrib_pointer(i, first + count);
  if (vbo_bkp != buffer_objs.vbo)
    glBindBuffer( GL_ARRAY_BUFFER, vbo_bkp );

  GLS_SET_COMMAND_PTR(c, glDrawArrays);
  c->mode = mode;
  c->first = first;
  c->count = count;
  GLS_SEND_PACKET(glDrawArrays);

  // FIXME should free VBO's emulating client arrays
}


GLS_DEF_CORE_API(void, glDrawElements, GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)
{
  uint32_t sizeoftype = _type_bytesize(type);

  GLuint vbo_bkp = buffer_objs.vbo;
  GLuint ibo_bkp = buffer_objs.ibo;
  int i;
  // how much client-data is needed depends on max index in indices
  int max_idx = -1;
  if (!buffer_objs.ibo) {
    for (i = 0; i < count; i++) {
      int idx = GL_UNSIGNED_BYTE ? ((unsigned char*)indices)[i] : ((unsigned short*)indices)[i];
      if (idx > max_idx) max_idx = idx;
    }
    if (max_idx < 0) {
      client_gles_error = GL_INVALID_OPERATION;
      fprintf(stderr, "GLS ERROR: %s: failed to compute max index in IBO\n", __FUNCTION__);
      return;
    }
  }
  // transfer client-data
  for (i = 0; i < 16; i++) {
    defered_vertex_attrib_pointer(i, max_idx + 1);
  }
  if (!buffer_objs.ibo) {
    WARN_ONCE("GLS WARNING: %s uses client-data index buffer, consider using an IBO\n", __FUNCTION__);
    if ( !buffer_objs.ibo_emu ) {
      glGenBuffers(1, &buffer_objs.ibo_emu);
    }
    if (glsc_global.is_debug)
      fprintf(stderr, "GLS DBG: %s: uploading %u bytes to emulation IBO\n", __FUNCTION__,
              count * sizeoftype);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_objs.ibo_emu);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeoftype, indices, GL_STREAM_DRAW);
  }

  GLS_SET_COMMAND_PTR(c, glDrawElements);
  c->mode = mode;
  c->count = count;
  c->type = type;

  GLS_SEND_PACKET(glDrawElements);

  if (ibo_bkp != buffer_objs.ibo)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_bkp);
  if (vbo_bkp != buffer_objs.vbo)
    glBindBuffer(GL_ARRAY_BUFFER, vbo_bkp);

  // FIXME should free VBO's and IBO emulating client arrays
}


GLS_DEF_CORE_API(void, glEnable, GLenum cap)
{
  GLS_SET_COMMAND_PTR(c, glEnable);
  c->cap = cap;
  GLS_SEND_PACKET(glEnable);
}


GLS_DEF_CORE_API(void, glEnableVertexAttribArray, GLuint index)
{
  GLS_SET_COMMAND_PTR(c, glEnableVertexAttribArray);
  c->index = index;
  GLS_SEND_PACKET(glEnableVertexAttribArray);

  // FIXME: should we wait and check for error ?
  buffer_objs.attrib_pointer[index].isenabled = GL_TRUE;
}


GLS_DEF_CORE_API(void, glFinish, void)
{
  WARN_STUBBED(); // wrong semantics
  GLS_SET_COMMAND_PTR(c, glFinish);
  GLS_SEND_PACKET(glFinish);
}


GLS_DEF_CORE_API(void, glFlush, void)
{
  GLS_SET_COMMAND_PTR(c, glFlush);
  GLS_SEND_PACKET(glFlush);
}

GLS_DEF_CORE_API(void, glFramebufferRenderbuffer, GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
  GLS_SET_COMMAND_PTR(c, glFramebufferRenderbuffer);
  c->target = target;
  c->attachment = attachment;
  c->renderbuffertarget = renderbuffertarget;
  c->renderbuffer = renderbuffer;
  GLS_SEND_PACKET(glFramebufferRenderbuffer);
}


GLS_DEF_CORE_API(void, glFramebufferTexture2D, GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
  GLS_SET_COMMAND_PTR(c, glFramebufferTexture2D);
  c->target = target;
  c->attachment = attachment;
  c->textarget = textarget;
  c->texture = texture;
  c->level = level;
  GLS_SEND_PACKET(glFramebufferTexture2D);
}


GLS_DEF_CORE_API(void, glFrontFace, GLenum mode)
{
  (void)mode;
  WARN_STUBBED();
}


GLS_DEF_CORE_API(void, glGenBuffers, GLsizei n, GLuint* buffers)
{
  GLS_SET_COMMAND_PTR(c, glGenBuffers);
  c->n = n;
  GLS_SEND_PACKET(glGenBuffers);

  GLS_WAIT_SET_RAWRET_PTR(ret, void, glGenBuffers);
  _Static_assert(sizeof(GLuint) == sizeof(uint32_t), "int size mismatch");
  memcpy(buffers, ret, c->n * sizeof(uint32_t));
}


GLS_DEF_CORE_API(void, glGenerateMipmap, GLenum target)
{
  GLS_SET_COMMAND_PTR(c, glGenerateMipmap);
  c->target = target;
  GLS_SEND_PACKET(glGenerateMipmap);
}


GLS_DEF_CORE_API(void, glGenFramebuffers, GLsizei n, GLuint* framebuffers)
{
  GLS_SET_COMMAND_PTR(c, glGenFramebuffers);
  c->n = n;
  GLS_SEND_PACKET(glGenFramebuffers);

  GLS_WAIT_SET_RAWRET_PTR(ret, void, glGenFramebuffers);
  _Static_assert(sizeof(GLuint) == sizeof(uint32_t), "int size mismatch");
  memcpy(framebuffers, ret, c->n * sizeof(uint32_t));
}


GLS_DEF_CORE_API(void, glGenRenderbuffers, GLsizei n, GLuint* renderbuffers)
{
  GLS_SET_COMMAND_PTR(c, glGenRenderbuffers);
  c->n = n;
  GLS_SEND_PACKET(glGenRenderbuffers);

  GLS_WAIT_SET_RAWRET_PTR(ret, void, glGenRenderbuffers);
  _Static_assert(sizeof(GLuint) == sizeof(uint32_t), "int size mismatch");
  memcpy(renderbuffers, ret, c->n * sizeof(uint32_t));
}


GLS_DEF_CORE_API(void, glGenTextures, GLsizei n, GLuint* textures)
{
  GLS_SET_COMMAND_PTR(c, glGenTextures);
  c->n = n;
  GLS_SEND_PACKET(glGenTextures);

  GLS_WAIT_SET_RAWRET_PTR(ret, void, glGenTextures);
  _Static_assert(sizeof(GLuint) == sizeof(uint32_t), "int size mismatch");
  memcpy(textures, ret, c->n * sizeof(uint32_t));
}


GLS_DEF_CORE_API(void, glGetActiveAttrib, GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name)
{
  WARN_UNTESTED();
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

  GLS_WAIT_RET(GLSC_glGetActiveAttrib, glGetActiveAttrib);

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


GLS_DEF_CORE_API(void, glGetActiveUniform, GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name)
{
  WARN_UNTESTED();
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

  GLS_WAIT_RET(GLSC_glGetActiveUniform, glGetActiveUniform);

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


GLS_DEF_CORE_API(void, glGetAttachedShaders, GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders)
{
  (void)program; (void)maxcount; (void)count; (void)shaders;
  WARN_STUBBED();
}


GLS_DEF_CORE_API(GLint, glGetAttribLocation, GLuint program, const GLchar* name)
{
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

GLS_DEF_CORE_API(void, glGetBooleanv, GLenum pname, GLboolean* params)
{
  (void)pname; (void)params;
  WARN_STUBBED();
}


GLS_DEF_CORE_API(void, glGetBufferParameteriv, GLenum target, GLenum pname, GLint* params)
{
  (void)target; (void)pname; (void)params;
  WARN_STUBBED();
}


GLS_DEF_CORE_API(GLenum, glGetError, )
{
  if (client_gles_error != GL_NO_ERROR)
    return client_gles_error;

  GLS_SET_COMMAND_PTR(c, glGetError);
  GLS_SEND_PACKET(glGetError);

  GLS_WAIT_SET_RET_PTR(ret, glGetError);
  GLS_RELEASE_RETURN_RET(GLenum, ret, error);
}


GLS_DEF_CORE_API(void, glGetFloatv, GLenum name, GLfloat* params)
{
  GLS_SET_COMMAND_PTR(c, glGetFloatv);
  c->name = name;
  GLS_SEND_PACKET(glGetFloatv);

  GLS_WAIT_SET_RET_PTR(ret, glGetFloatv);
  *params = ret->params;
  GLS_RELEASE_RET();
}


GLS_DEF_CORE_API(void, glGetFramebufferAttachmentParameteriv, GLenum target, GLenum attachment, GLenum pname, GLint* params)
{
  (void)target; (void)attachment; (void)pname; (void)params;
  WARN_STUBBED();
}


GLS_DEF_CORE_API(void, glGetIntegerv, GLenum name, GLint* params)
{
  GLS_SET_COMMAND_PTR(c, glGetIntegerv);
  c->name = name;
  GLS_SEND_PACKET(glGetIntegerv);

  GLS_WAIT_SET_RET_PTR(ret, glGetIntegerv);
  *params = ret->params;
  GLS_RELEASE_RET();
}


GLS_DEF_CORE_API(void, glGetProgramiv, GLuint program, GLenum pname, GLint* params)
{
  GLS_SET_COMMAND_PTR(c, glGetProgramiv);
  c->program = program;
  c->pname = pname;
  GLS_SEND_PACKET(glGetProgramiv);

  GLS_WAIT_SET_RET_PTR(ret, glGetProgramiv);
  *params = ret->params;
  GLS_RELEASE_RET();
}


GLS_DEF_CORE_API(void, glGetProgramInfoLog, GLuint program, GLsizei bufsize, GLsizei* length, GLchar* infolog)
{
  WARN_UNTESTED();
  GLS_SET_COMMAND_PTR(c, glGetProgramInfoLog);
  c->program = program;
  c->bufsize = bufsize;
  GLS_SEND_PACKET(glGetProgramInfoLog);

  GLS_WAIT_SET_RET_PTR(ret, glGetProgramInfoLog);
  if (ret->length > bufsize - 1) {
    client_gles_error = GL_INVALID_VALUE;
    GLS_RELEASE_RET();
    return;
  }
  if (length != NULL)
    *length = ret->length;
  memcpy(infolog, ret->infolog, ret->length);
  ret->infolog[ret->length] = '\0';
  GLS_RELEASE_RET();
}


GLS_DEF_CORE_API(void, glGetRenderbufferParameteriv, GLenum target, GLenum pname, GLint* params)
{
  (void)target; (void)pname; (void)params;
  WARN_STUBBED();
}


GLS_DEF_CORE_API(void, glGetShaderiv, GLuint shader, GLenum pname, GLint* params)
{
  GLS_SET_COMMAND_PTR(c, glGetShaderiv);
  c->shader = shader;
  c->pname = pname;
  GLS_SEND_PACKET(glGetShaderiv);

  GLS_WAIT_SET_RET_PTR(ret, glGetShaderiv);
  *params = ret->params;
}


GLS_DEF_CORE_API(void, glGetShaderInfoLog, GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* infolog)
{
  WARN_UNTESTED();
  GLS_SET_COMMAND_PTR(c, glGetShaderInfoLog);
  c->shader = shader;
  c->bufsize = bufsize;
  GLS_SEND_PACKET(glGetShaderInfoLog);

  GLS_WAIT_SET_RET_PTR(ret, glGetShaderInfoLog);
  if (ret->length > bufsize - 1) {
    client_gles_error = GL_INVALID_VALUE;
    GLS_RELEASE_RET();
    return;
  }
  if (length != NULL)
    *length = ret->length;
  strncpy(infolog, ret->infolog, (size_t)bufsize);
  ret->infolog[ret->length] = '\0';
  GLS_RELEASE_RET();
}


GLS_DEF_CORE_API(void, glGetShaderPrecisionFormat, GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision)
{
  (void)shadertype; (void)precisiontype; (void)range; (void)precision;
  WARN_STUBBED();
}


GLS_DEF_CORE_API(void, glGetShaderSource, GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* source)
{
  (void)shader; (void)bufsize; (void)length; (void)source;
  WARN_STUBBED();
}

// glGetString (with caching)

static const GLubyte* GL_APIENTRY _real_glGetString(GLenum name)
{
  GLS_SET_COMMAND_PTR(c, glGetString);
  c->name = name;
  GLS_SEND_PACKET(glGetString);

  GLS_WAIT_SET_RET_PTR(ret, glGetString);
  if (!ret->success) {
    GLS_RELEASE_RET();
    return NULL;
  }
  // emulating static buffer: must defer freeing until after copied
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

static struct
{
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

GLS_DEF_CORE_API(const GLubyte*, glGetString, GLenum name)
{
  // init storage
  if (!gles_strings.storage)
    _populate_gles_strings();

  switch (name) {
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

GLS_DEF_CORE_API(void, glGetTexParameterfv, GLenum target, GLenum pname, GLfloat* params)
{
  (void)target; (void)pname; (void)params;
  WARN_STUBBED();
}


GLS_DEF_CORE_API(void, glGetTexParameteriv, GLenum target, GLenum pname, GLint* params)
{
  (void)target; (void)pname; (void)params;
  WARN_STUBBED();
}


GLS_DEF_CORE_API(void, glGetUniformfv, GLuint program, GLint location, GLfloat* params)
{
  (void)program; (void)location; (void)params;
  WARN_STUBBED();
}


GLS_DEF_CORE_API(void, glGetUniformiv, GLuint program, GLint location, GLint* params)
{
  (void)program; (void)location; (void)params;
  WARN_STUBBED();
}


GLS_DEF_CORE_API(int, glGetUniformLocation, GLuint program, const GLchar* name)
{
  WARN_ONCE("GLS WARNING: %s should be totally rewritten\n", __FUNCTION__);
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


GLS_DEF_CORE_API(void, glGetVertexAttribfv, GLuint index, GLenum pname, GLfloat* params)
{
  (void)index; (void)pname; (void)params;
  WARN_STUBBED();
}


GLS_DEF_CORE_API(void, glGetVertexAttribiv, GLuint index, GLenum pname, GLint* params)
{
  (void)index; (void)pname; (void)params;
  WARN_STUBBED();
}


GLS_DEF_CORE_API(void, glGetVertexAttribPointerv, GLuint index, GLenum pname, GLvoid** pointer)
{
  (void)index; (void)pname; (void)pointer;
  WARN_STUBBED();
}


GLS_DEF_CORE_API(void, glHint, GLenum target, GLenum mode)
{
  GLS_SET_COMMAND_PTR(c, glHint);
  c->target = target;
  c->mode = mode;
  GLS_SEND_PACKET(glHint);
}


GLS_DEF_CORE_API(GLboolean, glIsBuffer, GLuint buffer)
{
  GLS_SET_COMMAND_PTR(c, glIsBuffer);
  c->buffer = buffer;
  GLS_SEND_PACKET(glIsBuffer);
  GLS_WAIT_SET_RET_PTR(ret, glIsBuffer);
  GLS_RELEASE_RETURN_RET(GLboolean, ret, isbuffer);
}


GLS_DEF_CORE_API(GLboolean, glIsEnabled, GLenum cap)
{
  GLS_SET_COMMAND_PTR(c, glIsEnabled);
  c->cap = cap;
  GLS_SEND_PACKET(glIsEnabled);
  GLS_WAIT_SET_RET_PTR(ret, glIsEnabled);
  GLS_RELEASE_RETURN_RET(GLboolean, ret, isenabled);
}


GLS_DEF_CORE_API(GLboolean, glIsFramebuffer, GLuint framebuffer)
{
  (void)framebuffer;
  WARN_STUBBED();
  return FALSE;
}


GLS_DEF_CORE_API(GLboolean, glIsProgram, GLuint program)
{
  (void)program;
  WARN_STUBBED();
  return FALSE;
}


GLS_DEF_CORE_API(GLboolean, glIsRenderbuffer, GLuint renderbuffer)
{
  (void)renderbuffer;
  WARN_STUBBED();
  return FALSE;
}


GLS_DEF_CORE_API(GLboolean, glIsShader, GLuint shader)
{
  (void)shader;
  WARN_STUBBED();
  return FALSE;
}


GLS_DEF_CORE_API(GLboolean, glIsTexture, GLuint texture)
{
  (void)texture;
  WARN_STUBBED();
  return FALSE;
}


GLS_DEF_CORE_API(void, glLineWidth, GLfloat width)
{
  GLS_SET_COMMAND_PTR(c, glLineWidth);
  c->width = width;
  GLS_SEND_PACKET(glLineWidth);
}


GLS_DEF_CORE_API(void, glLinkProgram, GLuint program)
{
  GLS_SET_COMMAND_PTR(c, glLinkProgram);
  c->program = program;
  GLS_SEND_PACKET(glLinkProgram);
}


GLS_DEF_CORE_API(void, glPixelStorei, GLenum pname, GLint param)
{
  switch (pname) {
  case GL_PACK_ALIGNMENT:
    glsc_global.pack_alignment = param;
    break;
  case GL_UNPACK_ALIGNMENT:
    glsc_global.unpack_alignment = param;
    break;
  default:
    fprintf(stderr, "GLS WARNING: %s called with unknown pname 0x%x\n", __FUNCTION__, pname);
  }
  GLS_SET_COMMAND_PTR(c, glPixelStorei);
  c->pname = pname;
  c->param = param;
  GLS_SEND_PACKET(glPixelStorei);
}


GLS_DEF_CORE_API(void, glPolygonOffset, GLfloat factor, GLfloat units)
{
  GLS_SET_COMMAND_PTR(c, glPolygonOffset);
  c->factor = factor;
  c->units = units;
  GLS_SEND_PACKET(glPolygonOffset);
}


GLS_DEF_CORE_API(void, glReadPixels, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels)
{
  WARN_ONCE("GLS WARNING: %s likely to buffer overflow on both server and client\n",
            __FUNCTION__);
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


GLS_DEF_CORE_API(void, glReleaseShaderCompiler, void)
{
  WARN_STUBBED();
}


GLS_DEF_CORE_API(void, glRenderbufferStorage, GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
  GLS_SET_COMMAND_PTR(c, glRenderbufferStorage);
  c->target = target;
  c->internalformat = internalformat;
  c->width = width;
  c->height = height;
  GLS_SEND_PACKET(glRenderbufferStorage);
}


GLS_DEF_CORE_API(void, glSampleCoverage, GLclampf value, GLboolean invert)
{
  (void)value; (void)invert;
  WARN_STUBBED();
}


GLS_DEF_CORE_API(void, glScissor, GLint x, GLint y, GLsizei width, GLsizei height)
{
  (void)x; (void)y; (void)width; (void)height;
  WARN_STUBBED();
}


GLS_DEF_CORE_API(void, glShaderBinary, GLsizei n, const GLuint* shaders, GLenum binaryformat, const GLvoid* binary, GLsizei length)
{
  (void)n; (void)shaders; (void)binaryformat; (void)binary; (void)length;
  WARN_STUBBED();
}


GLS_DEF_CORE_API(void, glShaderSource, GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length)
{
  if (count > 10240) { // 256
    fprintf(stderr, "GLS WARNING: shader too large, over 10kb, ignoring.\n"); // FIXME why!?
    return;
  }
  gls_data_glShaderSource_t* dat = (gls_data_glShaderSource_t*)glsc_global.pool.tmp_buf.buf;
  size_t size_all = (size_t)(dat->data - (char*)dat);

  // printf("\n ----- BEGIN SHADER CONTENT -----\n");
  uint32_t stroffset = 0;
  int i;

  // FIXME we're sending both a full length-array *and* NUL terminators
  for (i = 0; i < count; i++) {
    const GLchar* strptr = string[i];
    size_t strsize = length ? length[i] : 0;
    if (strsize == 0)
      strsize = strlen(strptr);
    size_all += strsize + 1;
    if (size_all > glsc_global.pool.tmp_buf.size) {
      fprintf(stderr, "GLS ERROR: shader buffer size overflow!\n");
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


GLS_DEF_CORE_API(void, glStencilFunc, GLenum func, GLint r, GLuint m)
{
  GLS_SET_COMMAND_PTR(c, glStencilFunc);
  c->func = func;
  c->r = r;
  c->m = m;
  GLS_SEND_PACKET(glStencilFunc);
}


GLS_DEF_CORE_API(void, glStencilFuncSeparate, GLenum face, GLenum func, GLint ref, GLuint mask)
{
  (void)face; (void)func; (void)ref; (void)mask;
  WARN_STUBBED();
}


GLS_DEF_CORE_API(void, glStencilMask, GLuint mask)
{
  GLS_SET_COMMAND_PTR(c, glStencilMask);
  c->mask = mask;
  GLS_SEND_PACKET(glStencilMask);
}


GLS_DEF_CORE_API(void, glStencilMaskSeparate, GLenum face, GLuint mask)
{
  (void)face; (void)mask;
  WARN_STUBBED();
}


GLS_DEF_CORE_API(void, glStencilOp, GLenum fail, GLenum zfail, GLenum zpass)
{
  GLS_SET_COMMAND_PTR(c, glStencilOp);
  c->fail = fail;
  c->zfail = zfail;
  c->zpass = zpass;
  GLS_SEND_PACKET(glStencilOp);
}


GLS_DEF_CORE_API(void, glStencilOpSeparate, GLenum face, GLenum fail, GLenum zfail, GLenum zpass)
{
  (void)face; (void)fail; (void)zfail; (void)zpass;
  WARN_STUBBED();
}


GLS_DEF_CORE_API(void, glTexImage2D, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels)
{
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


GLS_DEF_CORE_API(void, glTexParameterf, GLenum target, GLenum pname, GLfloat param)
{
  (void)target; (void)pname; (void)param;
  WARN_STUBBED();
}


GLS_DEF_CORE_API(void, glTexParameterfv, GLenum target, GLenum pname, const GLfloat* params)
{
  (void)target; (void)pname; (void)params;
  WARN_STUBBED();
}


GLS_DEF_CORE_API(void, glTexParameteri, GLenum target, GLenum pname, GLint param)
{
  GLS_SET_COMMAND_PTR(c, glTexParameteri);
  c->target = target;
  c->pname = pname;
  c->param = param;
  GLS_SEND_PACKET(glTexParameteri);
}


GLS_DEF_CORE_API(void, glTexParameteriv, GLenum target, GLenum pname, const GLint* params)
{
  (void)target; (void)pname; (void)params;
  WARN_STUBBED();
}


GLS_DEF_CORE_API(void, glTexSubImage2D, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels)
{
  if (pixels) {
    uint32_t pixelbytes = _pixelformat_to_bytes(format, type);
    uint32_t linebytes = (pixelbytes * width + glsc_global.unpack_alignment - 1) & (~ (glsc_global.unpack_alignment - 1));
    gls_cmd_send_data(linebytes * height, pixels);
  }

  GLS_SET_COMMAND_PTR(c, glTexSubImage2D);
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
  GLS_DEF_CORE_API(void, FUNC, GLint location, __VA_ARGS__)         \
  {                                                                     \
    GLS_SET_COMMAND_PTR(c, FUNC);                                       \
    c->location = location;                                             \
    COPYARGS;                                                           \
    GLS_SEND_PACKET(glUniform1f);                                       \
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
  GLS_DEF_CORE_API(void, FUNC, GLint location, GLsizei count, const TYPE* v) \
  {                                                                     \
  uint32_t datasize = count * N * sizeof(TYPE);                         \
  GLS_SET_COMMAND_PTR(c, FUNC);                                         \
  c->location = location;                                               \
  c->count = count;                                                     \
  GLS_VARIABLE_PAYLOAD(c, v, datasize);                                 \
  send_packet();                                                        \
  }

IMPLEM_glUniformNXv(glUniform1fv, 1, GLfloat);
IMPLEM_glUniformNXv(glUniform2fv, 2, GLfloat);
IMPLEM_glUniformNXv(glUniform3fv, 3, GLfloat);
IMPLEM_glUniformNXv(glUniform4fv, 4, GLfloat);

IMPLEM_glUniformNXv(glUniform1iv, 1, GLint);
IMPLEM_glUniformNXv(glUniform2iv, 2, GLint);
IMPLEM_glUniformNXv(glUniform3iv, 3, GLint);
IMPLEM_glUniformNXv(glUniform4iv, 4, GLint);


#define IMPLEM_glUniformMatrixNXv(FUNC,N,TYPE)                          \
  GLS_DEF_CORE_API(void, FUNC, GLint location, GLsizei count, GLboolean transpose, const TYPE* value) \
  {                                                                     \
    uint32_t datasize = count * N * N * sizeof(TYPE);                   \
    GLS_SET_COMMAND_PTR(c, FUNC);                                       \
    c->location = location;                                             \
    c->count = count;                                                   \
    c->transpose = transpose;                                           \
    GLS_VARIABLE_PAYLOAD(c, value, datasize);                           \
    send_packet();                                                      \
  }

IMPLEM_glUniformMatrixNXv(glUniformMatrix2fv, 2, GLfloat);
IMPLEM_glUniformMatrixNXv(glUniformMatrix3fv, 3, GLfloat);
IMPLEM_glUniformMatrixNXv(glUniformMatrix4fv, 4, GLfloat);


GLS_DEF_CORE_API(void, glUseProgram, GLuint program)
{
  GLS_SET_COMMAND_PTR(c, glUseProgram);
  c->program = program;
  GLS_SEND_PACKET(glUseProgram);
}


GLS_DEF_CORE_API(void, glValidateProgram, GLuint program)
{
  (void)program;
  WARN_STUBBED();
}


static void _glVertexAttribFloat(enum GL_Server_Command cmd,
                                 GLuint index, GLint num_float,
                                 const GLfloat* arr)
{
  gls_data_glVertexAttribFloat_t* dat = (gls_data_glVertexAttribFloat_t*)glsc_global.pool.tmp_buf.buf;
  memcpy(dat->arr, arr, num_float);
  // It's small so use GLS_DATA_SIZE
  gls_cmd_send_data(GLS_DATA_SIZE, glsc_global.pool.tmp_buf.buf);

  _GLS_SET_COMMAND_PTR(c, glVertexAttribFloat, cmd);
  c->index = index;
  GLS_SEND_PACKET(glVertexAttribFloat);
}


GLS_DEF_CORE_API(void, glVertexAttrib1f, GLuint index, GLfloat v0)
{
  GLfloat arr[1] = {v0};
  _glVertexAttribFloat(GLSC_glVertexAttrib1f, index, 1, arr);
}


GLS_DEF_CORE_API(void, glVertexAttrib2f, GLuint index, GLfloat v0, GLfloat v1)
{
  GLfloat arr[2] = {v0, v1};
  _glVertexAttribFloat(GLSC_glVertexAttrib2f, index, 2, arr);
}


GLS_DEF_CORE_API(void, glVertexAttrib3f, GLuint index, GLfloat v0, GLfloat v1, GLfloat v2)
{
  GLfloat arr[3] = {v0, v1, v2};
  _glVertexAttribFloat(GLSC_glVertexAttrib3f, index, 3, arr);
}


GLS_DEF_CORE_API(void, glVertexAttrib4f, GLuint index, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
  GLfloat arr[4] = {v0, v1, v2, v3};
  _glVertexAttribFloat(GLSC_glVertexAttrib4f, index, 4, arr);
}


GLS_DEF_CORE_API(void, glVertexAttrib1fv, GLuint index, const GLfloat* v)
{
  _glVertexAttribFloat(GLSC_glVertexAttrib1fv, index, 1, v);
}


GLS_DEF_CORE_API(void, glVertexAttrib2fv, GLuint index, const GLfloat* v)
{
  _glVertexAttribFloat(GLSC_glVertexAttrib2fv, index, 2, v);
}


GLS_DEF_CORE_API(void, glVertexAttrib3fv, GLuint index, const GLfloat* v)
{
  _glVertexAttribFloat(GLSC_glVertexAttrib3fv, index, 3, v);
}


GLS_DEF_CORE_API(void, glVertexAttrib4fv, GLuint index, const GLfloat* v)
{
  _glVertexAttribFloat(GLSC_glVertexAttrib4fv, index, 4, v);
}


GLS_DEF_CORE_API(void, glVertexAttribPointer,
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


GLS_DEF_CORE_API(void, glViewport, GLint x, GLint y, GLsizei width, GLsizei height)
{
  GLS_SET_COMMAND_PTR(c, glViewport);
  c->x = x;
  c->y = y;
  c->width = width;
  c->height = height;
  GLS_SEND_PACKET(glViewport);
}


/*
 * extension commands
 */


/*
 * eglGetProcAddress support
 */

void* gls_GetGlesProcAddress(const char* procname)
{
  void* proc;
  if (0) {}
#define X(FUNC)                            \
  else if (strcmp(procname, #FUNC) == 0) { \
    proc = __GLS_##FUNC;                   \
  }                                        \
  //
  GLS_GLES2_COMMANDS()
  GLS_GLES2_EXT_COMMANDS()
#undef X
  else {
    fprintf(stderr, "GLS WARNING: %s: %s available on server but not supported\n",
            __FUNCTION__, procname);
    proc = NULL;
  }

  return proc;
}

// Used for return void commands
/*
GLS_DEF_CORE_API(void, glCommand, GLparam param)
{
  WARN_STUBBED();
}
GLS_DEF_CORE_API(void, glCommand, GLparam param)
{
  GLS_SET_COMMAND_PTR(c, glCommand);
  c->param = param;
  GLS_SEND_PACKET(glCommand);
}
 */

// Used for return functions, icluding modify client array.
/*
GLS_DEF_CORE_API(GLreturn, glCommand, GLparam param)
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

