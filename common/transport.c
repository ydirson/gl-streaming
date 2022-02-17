#include "transport.h"
#include "fastlog.h"

#include <assert.h>
#include <string.h>

static struct gls_transport* transports[] = {
  &gls_tport_tcp,
};

struct gls_transport* the_tport;

int tport_select(const char* name)
{
  unsigned i;
  assert(!the_tport);

  if (!name) {
    // arbitrary default value
    the_tport = transports[0];
    LOGI("GLS INFO: transport defaulting to '%s'\n", the_tport->name);
    return 0;
  }

  for (i = 0; i < sizeof(transports)/sizeof(transports[0]); i++) {
    if (!transports[i]->name) {
      LOGE("GLS ERROR: transports[i] is unnamed\n");
      continue;
    }
    if (strcmp(transports[i]->name, name) != 0)
      continue; // does not match
    // we have a match
    the_tport = transports[i];
    LOGI("GLS INFO: using transport '%s'\n", the_tport->name);
    return 0;
  }

  LOGE("GLS ERROR: no transport named '%s'\n", name);
  return -1;
}
