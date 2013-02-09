/*
 * Copyright 2012 iamrekcah
 *
 * This file is part of wol.
 *
 * wol is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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
 * usage.c
 *
 * This file implements command-line switch parsing and usage functionality.
 */

#include "config.h"

#include <argp.h>
#include <errno.h>

#include "usage.h"
#include "common.h"

#define VERSION "1.0b"

const char *argp_program_version = VERSION;
const char *argp_program_bug_address = "<http://github.com/iamrekcah/wol>";

error_t
parser(int key, char *arg, struct argp_state *state) {
  struct arguments *arguments = state->input;
  
  switch(key) {
    /* parsed --{quiet|silent} | -{q|s} */
  case 'q':
  case 's':
    arguments->use_q = true;
    break;
 
    /* parsed --interface | -i */
  case 'i':
    arguments->use_i = true;
    arguments->ifacename = arg;
    break;

    /* parsed --password | -p */
  case 'p':
    arguments->use_p = true;
    arguments->password = arg;
    break;

    /* parsed <target mac addr> */
  case ARGP_KEY_ARG:
    if(state->arg_num >= 1) {
      argp_usage(state);
      return EINVAL;
    }
    arguments->target = arg;
    break;

    /* <target mac addr> not provided */
  case ARGP_KEY_NO_ARGS:
    argp_usage(state);
    return EINVAL;

  case ARGP_KEY_ERROR:
    return errno;
  }

  return 0;
}


/*
 * parse_cmdline will process the command-line and store the results
 * in the given arguments struct.
 *
 * params
 *    struct arguments * : A pointer to struct arguments for results.
 *    char ** : An array of C-strings (command-line args).
 *    size_t : The length of the array.
 *
 * returns
 *    0 : on success.
 */
int
parse_cmdline(struct arguments *args, char **argv, size_t argc) {

  /* expected switches */
  struct argp_option options[] = {
    {"quiet"    , 'q', 0,            0, "No output"},
    {"silent"   , 's', 0, OPTION_ALIAS, NULL},
    {"password" , 'p', "PASSWORD",   0, "Specify the WOL password"},
    {"interface", 'i', "NAME",       0, "Specify the net interface to use"},
    { 0 }
  };

  const char args_doc[] = "<mac address>";
  const char doc[] = "Wake-On-LAN packet sender";

  struct argp argp = { options, parser, args_doc, doc };

  int retval = argp_parse(&argp, (int)argc, argv, 0, 0, args);

  return retval == 0 ? 0 : retval;
}
