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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "glclient.h"

gls_context_t glsc_global;
uint32_t client_egl_error;

static server_context_t sc;

static float get_diff_time(struct timeval start, struct timeval end)
{
  float dt = (float)(end.tv_sec - start.tv_sec) + (float)(end.tv_usec - start.tv_usec) * 0.000001f;
  return dt;
}


int check_batch_overflow(size_t size, const char *msg)
{
  if (size >= GLS_TMP_BUFFER_SIZE - BATCH_AUTO_FLUSH_SIZE)
  {
    perror(msg);
    return FALSE;
  }
  uint32_t ptr = next_ptr(glsc_global.tmp_buf.ptr, size, GLS_ALIGNMENT_BITS);
  if (ptr >= GLS_TMP_BUFFER_SIZE - BATCH_AUTO_FLUSH_SIZE)
  {
    gls_cmd_flush();
  }
  return TRUE;
}


void push_batch_command(size_t size)
{
  glsc_global.tmp_buf.ptr = next_ptr(glsc_global.tmp_buf.ptr, size, GLS_ALIGNMENT_BITS);
  if (glsc_global.tmp_buf.ptr > BATCH_AUTO_FLUSH_SIZE)
  {
    gls_cmd_flush();
  }
}


static int gls_init(server_context_t *arg)
{
  memset(&glsc_global, 0, sizeof(glsc_global));
  
  if (GLS_VERSION & 1)
    fprintf(stderr, "WARNING: this is a development GLS protocol, "
            "make sure client and server match\n");

  const char* env_isDebugStr = getenv("GLS_DEBUG");
  int env_isDebug;
  if (env_isDebugStr == NULL) {
      env_isDebug = FALSE;
  } else {
      env_isDebug = atoi(env_isDebugStr);
  }
  
  if (env_isDebug == 0 || env_isDebug == 1) {
      glsc_global.is_debug = env_isDebug;
  } else {
      printf("gls error: GLS_DEBUG variable must be 0 or 1\n");
      exit(EXIT_FAILURE);
      return FALSE;
  }
  
  glsc_global.sta = arg;
  glsc_global.pack_alignment = 4;
  glsc_global.unpack_alignment = 4;
  glsc_global.screen_width = 1280;
  glsc_global.screen_height = 720;
  glsc_global.out_buf.buf = (char *)malloc(GLS_OUT_BUFFER_SIZE);
  if (glsc_global.out_buf.buf == NULL)
  {
    return FALSE;
  }
  glsc_global.tmp_buf.buf = (char *)malloc(GLS_TMP_BUFFER_SIZE);
  if (glsc_global.tmp_buf.buf == NULL)
  {
    free(glsc_global.out_buf.buf);
    return FALSE;
  }
  glsc_global.out_buf.size = GLS_OUT_BUFFER_SIZE;
  glsc_global.tmp_buf.size = GLS_TMP_BUFFER_SIZE;
  glsc_global.out_buf.ptr = 0;
  glsc_global.tmp_buf.ptr = 0;
  client_egl_error = EGL_SUCCESS;
  
  return TRUE;
}


static int gls_free()
{
    free(glsc_global.out_buf.buf);
    free(glsc_global.tmp_buf.buf);
  
    return TRUE;
}


int send_packet(size_t size)
{
  server_context_t *a = glsc_global.sta;
  if (sendto(a->sock_fd, glsc_global.out_buf.buf, size, 0, (struct sockaddr *)&a->sai, sizeof(struct sockaddr_in)) == -1)
  {
    return FALSE;
  }
  return TRUE;
}


static int gls_cmd_recv_data()
{
  gls_cmd_send_data_t *c = (gls_cmd_send_data_t *)glsc_global.cmd_data;
  if ((c->offset + c->size > glsc_global.tmp_buf.size) || (glsc_global.tmp_buf.size == 0))
  {
    return c->isLast;
  }
  memcpy(&glsc_global.tmp_buf.buf[c->offset], c->data.data_char, c->size);
  return c->isLast;
}


int wait_for_data(char *str)
{
  server_context_t *a = glsc_global.sta;
  struct timeval start_time, end_time;
  gettimeofday(&start_time, NULL);
  int quit = FALSE;
  while (quit == FALSE)
  {
    void *popptr = (void *)fifo_pop_ptr_get(&a->fifo);
    if (popptr == NULL)
    {
      gettimeofday(&end_time, NULL);
      float diff_time = get_diff_time(start_time, end_time);
      if (diff_time > GLS_TIMEOUT_SEC)
      {
        printf("\n%s\n", str);
        exit(EXIT_FAILURE);
        return FALSE;
      }
      usleep(a->sleep_usec);
    }
    else
    {
      gls_command_t *c = (gls_command_t *)popptr;
      glsc_global.cmd_data = popptr;
      switch (c->cmd)
      {
        case GLSC_SEND_DATA:
          if (gls_cmd_recv_data() == TRUE) {
            quit = TRUE;
          }
          break;
        default:
          break;
      }
      fifo_pop_ptr_next(&a->fifo);
    }
  }
  return TRUE;
}


int gls_cmd_send_data(uint32_t offset, uint32_t size, void *data)
{
  if (glsc_global.is_debug) fprintf(stderr, "%s\n", __FUNCTION__);
  if (data == NULL) {
    return TRUE;
  }
    
  gls_cmd_send_data_t *c = (gls_cmd_send_data_t *)glsc_global.out_buf.buf;
  c->cmd = GLSC_SEND_DATA;

  int success = TRUE;
  uint32_t glssize = GLS_DATA_SIZE * 4;
  char *data1 = (char *)data;
  uint32_t offset1;
  for (offset1 = 0; offset1 < size; offset1 += glssize)
  {
    unsigned int size1 = size - offset1;
    c->isLast = (size1 > glssize) ? FALSE : TRUE;
    size1 = (size1 > glssize) ? glssize : size1;
    memcpy(c->data.data_char, data1, size1);
    size_t sendbytes = (size_t)(&c->data.data_char[size1] - (char *)c);
    c->offset = offset + offset1;
    c->size = size1;
    if (send_packet(sendbytes) == FALSE)
    {
      success = FALSE;
    }
    data1 += glssize;
  }
  return success;
}


static int gls_cmd_get_context()
{
  if (glsc_global.is_debug) fprintf(stderr, "%s\n", __FUNCTION__);
  gls_cmd_flush();
  if (glsc_global.is_debug) fprintf(stderr, "%s sending\n", __FUNCTION__);
  gls_cmd_get_context_t *c = (gls_cmd_get_context_t *)glsc_global.out_buf.buf;
  c->cmd = GLSC_get_context;
  if (send_packet(sizeof(gls_cmd_get_context_t)) == FALSE)
  {
    return FALSE;
  }

  wait_for_data("timeout:gls_cmd_get_context");
  gls_ret_get_context_t *ret = (gls_ret_get_context_t *)glsc_global.tmp_buf.buf;
  if (ret->cmd == GLSC_get_context)
  {
    glsc_global.screen_width = ret->screen_width;
    glsc_global.screen_height = ret->screen_height;
    printf("GLS INFO: width=%i, height=%i\n", ret->screen_width, ret->screen_height);
    if (ret->server_version != GLS_VERSION) {
        printf("GLS ERROR: Incompatible version, server version %i but client version %i.\n", ret->server_version, GLS_VERSION);
        exit(EXIT_FAILURE);
        return FALSE;
    }
  }
  return TRUE;
}


void gls_init_library()
{
    static int init = FALSE;
    if(init)
        return;
    char my_ip[30]; // GLS_STRING_SIZE_PLUS
    char his_ip[30]; // GLS_STRING_SIZE_PLUS
    uint16_t my_port = 18146;
    uint16_t his_port = 18145;
    
    const char* env_serverAddr = getenv("GLS_SERVER_ADDR");
    if (env_serverAddr == NULL) {
        strncpy(his_ip, "127.0.0.1", 10);
    } else {
        printf("GLS_SERVER_ADDR is set to %s\n", env_serverAddr);
        
        char* env_serverAddr_search = ":";
        int env_serverAddr_length = strnlen(env_serverAddr, 0xA00000);
        char env_serverAddr_arr[env_serverAddr_length];
        strncpy(env_serverAddr_arr, env_serverAddr, env_serverAddr_length + 1);
        
        char* env_serverIp = strtok(env_serverAddr_arr, env_serverAddr_search);
        strncpy(his_ip, env_serverIp, strnlen(env_serverIp, 0xA00000) + 1);
        his_port = atoi(strtok(NULL, env_serverAddr_search));
        
    }

    strncpy(my_ip, "127.0.0.1", 10);
    server_init(&sc);
    set_client_address_port(&sc, my_ip, my_port);
    set_server_address_port(&sc, his_ip, his_port);

    server_start(&sc);
    gls_init(&sc);
    gls_cmd_get_context();
    
    init = TRUE;
}

void gls_cleanup_library()
{
  server_stop(&sc);
  gls_free();
}

int gls_cmd_flip(unsigned int frame)
{
  if (glsc_global.is_debug) fprintf(stderr, "%s\n", __FUNCTION__);
  gls_cmd_flush();
  if (glsc_global.is_debug) fprintf(stderr, "%s sending\n", __FUNCTION__);
  gls_cmd_flip_t *c = (gls_cmd_flip_t *)glsc_global.out_buf.buf;
  c->cmd = GLSC_FLIP;
  c->frame = frame;
  if (send_packet(sizeof(gls_cmd_flip_t)) == FALSE)
  {
    return FALSE;
  }

  wait_for_data("timeout:gls_cmd_flip");
  return TRUE;
}

int gls_cmd_flush()
{
  if (glsc_global.is_debug) fprintf(stderr, "%s\n", __FUNCTION__);
  if (glsc_global.tmp_buf.ptr == 0)
  {
    return FALSE;
  }
  if (glsc_global.is_debug) fprintf(stderr, "%s sending\n", __FUNCTION__);
  gls_command_t *c = (gls_command_t *)(glsc_global.tmp_buf.buf + glsc_global.tmp_buf.ptr);
  c->cmd = GLSC_BREAK;
  glsc_global.tmp_buf.ptr = next_ptr(glsc_global.tmp_buf.ptr, sizeof(gls_command_t), GLS_ALIGNMENT_BITS);
  gls_cmd_send_data(0, glsc_global.tmp_buf.ptr, (void *)glsc_global.tmp_buf.buf);
  glsc_global.tmp_buf.ptr = 0;

  c = (gls_command_t *)glsc_global.out_buf.buf;
  c->cmd = GLSC_FLUSH;
  send_packet(sizeof(gls_command_t));
  return TRUE;
}
