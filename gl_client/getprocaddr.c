#include <EGL/egl.h>
#include <dlfcn.h>

EGLAPI __eglMustCastToProperFunctionPointerType EGLAPIENTRY eglGetProcAddress( const char* procname )
{
  // Do not stream this command
  // FIXME: should query and return NULL when the server does
  void* proc = dlsym(NULL, procname);

  // WTF only works with -fno-optimize-sibling-calls, or anything like:

  //assert(proc);
  //fprintf(stderr, "GLS DGB: %s: %s is %p (%s)\n",
  //        __FUNCTION__, procname, proc, dlerror());

  return proc;
}
