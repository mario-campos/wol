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
 * Wake-On-LAN tool
 * This program sends a "magic packet" to the specified
 * -- via the MAC address -- computer on the Ethernet LAN.
 */

#include "config.h"

#include <sysexits.h>                          /* EX_DATAERR, EX_NOINPUT */
#include <string.h>                            /* memset() */
#include <linux/if_packet.h>                   /* sockaddr_ll */
#include <netinet/ether.h>                     /* ether_aton() */
#include <net/if.h>                            /* if_nametoindex() */
#include <unistd.h>                            /* close() */
#include <stdio.h>                             /* perror(), puts() */
#include <errno.h>                             /* errno */
#include <error.h>                             /* error() */

#include "wol.h"
#include "usage.h"
#include "secure.h"

int main(int argc, char **argv) {
  no_core_dumps();

  struct arguments args;
  parse_cmdline(&args, argv, argc);

  /* validate interface input */
  int iface_index;
  if(args.use_i) {
    iface_index = if_nametoindex(args.ifacename);
    if(iface_index == 0)
      error(EX_NOINPUT, errno, "invalid interface");
  } else {
    iface_index = if_nametoindex("eth0");
  }

  /* validate MAC Address */
  struct ether_addr *addr = ether_aton(args.target);
  struct ether_addr macaddr;
  if(addr == NULL) {
    error(EX_DATAERR, errno, "invalid MAC address");
  } else {
    macaddr = *addr;
  }

  /* configure destination */
  struct sockaddr_ll dest_addr;
  prepare_da(&dest_addr, iface_index);

  int sockfd = Socket();
  volatile char buf[WOL_DATA_LEN + WOL_PASSWD_LEN];
  size_t buf_len;

  // set frame payload with password
  if(args.use_p) {
    buf_len = WOL_DATA_LEN + WOL_PASSWD_LEN;
    set_payload_wp(&buf, &macaddr, pconvert((const char *)args.password));
  }

  // set frame payload without password
  else {
    buf_len = WOL_DATA_LEN;
    set_payload(&buf, &macaddr);
  }

  Sendto(sockfd, &buf, buf_len, &dest_addr, sizeof(dest_addr));

  /* clean up */
  close(sockfd);

  /* wipe password buffers */
  memset((void*)&buf, 0, buf_len);
  if(args.use_p) {
    memset((void*)args.password, 0, strlen((const char *)args.password));
  }
  return EX_OK;
}
