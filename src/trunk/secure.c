#include "secure.h"
#include <sys/time.h>
#include <sys/resource.h>

int no_core_dumps()
{
  /* resource limit structure */
  struct rlimit rlim;

  getrlimit(RLIMIT_CORE, &rlim);
  rlim.rlim_max = rlim.rlim_cur = 0;
  return setrlimit(RLIMIT_CORE, &rlim);
}
