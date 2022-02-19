#define _GNU_SOURCE
#include "transport.h"
#include "fastlog.h"

#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct gls_connection
{
  pid_t child_pid;
  int read_fd;
  int write_fd;
};

static void qrexec_parse_address(const char* addr, char** domain, char** service)
{
  static char the_domain[64];
  static char the_service[64];

  if (addr == NULL)
    addr = "dom0"; // dom0:qubes.GLS

  size_t domainlen = strcspn(addr, ":");
  if (domainlen >= sizeof(the_domain)) {
    LOGE("GLS ERROR: qrexec server domain name too long (%zd > %zd)\n",
         domainlen, sizeof(the_domain) - 1);
    exit(EXIT_FAILURE);
  }
  strncpy(the_domain, addr, domainlen);
  the_domain[domainlen] = '\0';
  *domain = the_domain;

  if (addr[domainlen] == ':' && addr[domainlen + 1] != '\0') {
    size_t servicelen = strlen(&addr[domainlen + 1]);
    if (servicelen >= sizeof(the_service)) {
      LOGE("GLS ERROR: qrexec service name too long (%zd > %zd)\n",
           servicelen, sizeof(the_service) - 1);
      exit(EXIT_FAILURE);
    }
    strcpy(the_service, &addr[domainlen + 1]);
    *service = the_service;
  } else
    *service = "qubes.GLS";

  LOGI("GLS INFO: using as domain '%s', service '%s'\n", the_domain, the_service);
}

static struct gls_connection* qrexecpipe_tport_client_create(const char* server_addr)
{
  char* gui_domain;
  char* rpc_service;
   LOGI("GLS INFO: initializing qrexec-pipe transport\n");

  qrexec_parse_address(server_addr, &gui_domain, &rpc_service);

  int child_stdin[2];
  int child_stdout[2];
  if (pipe(child_stdin) < 0) {
    LOGE("GLS ERROR: %s: stdin pipe failed: %s\n", __FUNCTION__, strerror(errno));
    goto error0;
  }
  if (pipe(child_stdout) < 0) {
    LOGE("GLS ERROR: %s: stdout pipe failed: %s\n", __FUNCTION__, strerror(errno));
    goto error1;
  }

  char* const command[] = {
    "qrexec-client-vm", gui_domain, rpc_service,
    NULL
  };

  pid_t child_pid = fork();
  switch (child_pid) {
  case -1:
    LOGE("GLS ERROR: %s: fork failed: %s\n", __FUNCTION__, strerror(errno));
    goto error2;
  case 0: // child side
    if (dup2(child_stdin[0], STDIN_FILENO) < 0) {
      LOGE("GLS ERROR: %s: dup2(stdin) failed: %s\n", __FUNCTION__, strerror(errno));
      goto error2;
    }
    if (dup2(child_stdout[1], STDOUT_FILENO) < 0) {
      LOGE("GLS ERROR: %s: dup2(stdout) failed: %s\n", __FUNCTION__, strerror(errno));
      goto error2;
    }
    close(child_stdout[0]);
    close(child_stdout[1]);
    close(child_stdin[0]);
    close(child_stdin[1]);

    execv("/usr/bin/qrexec-client-vm", command);
    LOGE("GLS ERROR: %s: exec(qrexec-client-vm) failed: %s\n", __FUNCTION__, strerror(errno));
    exit(EXIT_FAILURE);
  default:
    ; // parent, continues below
  }

  struct gls_connection* cnx = malloc(sizeof(struct gls_connection));
  if (!cnx) {
    LOGE("GLS ERROR: malloc failed: %s\n", strerror(errno));
    goto error2;
  }

  // close after last "goto error", to avoid any double-close on error
  close(child_stdin[0]);
  close(child_stdout[1]);

  cnx->child_pid = child_pid;
  cnx->read_fd = child_stdout[0];
  cnx->write_fd = child_stdin[1];
  return cnx;
 error2:
  close(child_stdout[0]);
  close(child_stdout[1]);
 error1:
  close(child_stdin[0]);
  close(child_stdin[1]);
 error0:
  return NULL;
}

static int qrexecpipe_tport_connection_fd(struct gls_connection* cnx)
{
  return cnx->read_fd;
}

static ssize_t qrexecpipe_tport_write(struct gls_connection* cnx, void* buffer, size_t size)
{
  ssize_t ret = write(cnx->write_fd, buffer, size);
  if (ret < 0)
    LOGE("GLS ERROR: write failed: %s\n", strerror(errno));
  return ret;
}

static ssize_t qrexecpipe_tport_writev(struct gls_connection* cnx, struct iovec *iov, int iovcnt)
{
  ssize_t ret = writev(cnx->write_fd, iov, iovcnt);
  if (ret < 0)
    LOGE("GLS ERROR: writev failed: %s\n", strerror(errno));
  return ret;
}

static ssize_t qrexecpipe_tport_read(struct gls_connection* cnx, void* buffer, size_t size)
{
  ssize_t ret = read(cnx->read_fd, buffer, size);
  if (ret < 0)
    LOGE("GLS ERROR: read failed: %s\n", strerror(errno));
  return ret;
}

static void qrexecpipe_tport_close(struct gls_connection* cnx)
{
  close(cnx->write_fd);
  kill(cnx->child_pid, SIGTERM); // FIXME necessary?
  close(cnx->read_fd);
}

struct gls_transport gls_tport_qrexecpipe = {
  .name = "qrexecpipe",
  .client_create = qrexecpipe_tport_client_create,
  .connection_fd = qrexecpipe_tport_connection_fd,
  .write = qrexecpipe_tport_write,
  .writev = qrexecpipe_tport_writev,
  .read = qrexecpipe_tport_read,
  .close = qrexecpipe_tport_close,
};
