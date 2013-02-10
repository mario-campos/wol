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
 * usage.h
 *
 * This file provides command-line switch parsing and usage functionality.
 */

#ifndef USAGE_H
#define USAGE_H

/*
 * A structure for containing the results of parsing the argument vector.
 */
struct arguments {
  int use_q;
  int use_p;
  int use_i;
  const char *ifacename;
  const char *password;
  const char *target;
};

/*
 * parse_cmdline will process the command-line and store the results
 * in the given arguments struct.
 *
 * params
 *    struct arguments * : A pointer to struct arguments for results.
 *    char ** : An array of C-strings (command-line args).
 *    int : The length of the array.
 *
 * returns
 *    0 : on success.
 */
int
parse_cmdline(struct arguments *args, char *argv[], size_t len);

#endif /* USAGE_H */
