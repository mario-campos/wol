/*
 * Copyright 2012 iamrekcah
 *
 * This file is part of wol.
 *
 * wol is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * wol is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with wol.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * secure.c
 *
 * Implementations of the linux security methods.
 */

#include "config.h"
#include "secure.h"

#include <sys/resource.h>

void
no_core_dumps()
{
  struct rlimit rlim = {
      .rlim_max = 0,    // hard limit (rlim_cur ceiling)
      .rlim_cur = 0     // soft limit (actual process limit)
  };
  setrlimit(RLIMIT_CORE, &rlim);
}
