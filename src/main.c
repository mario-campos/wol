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
 * Wake-On-LAN tool
 * This program sends a "magic packet" to the specified
 * -- via the MAC address -- computer on the Ethernet LAN.
 */

#include <linux/if_packet.h>
#include <netinet/ether.h>
#include <net/ethernet.h>
#include <string.h>
#include <net/if.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <error.h>

#include "wol.h"
#include "usage.h"
#include "common.h"

int main(int argc, char **argv) {

  struct arguments args = { 0 };

  parse_cmdline(&args, argv, argc);

  /* validate interface input */
  int iface_index;
  if(args.use_i) {
    iface_index = if_nametoindex(args.ifacename);
    if(iface_index == 0)
      error(-1, errno, "invalid interface");
  } else {
    iface_index = 2;
  }

  /* validate MAC Address */
  struct ether_addr *macaddr = ether_aton(args.target);
  if(macaddr == NULL) {
    error(-1, errno, "invalid MAC address");
  }

  struct sockaddr_ll dest_addr;
  int sockfd = Socket();
  void *buf = prepare_payload(macaddr);
  prepare_da(&dest_addr, iface_index);
  Sendto(sockfd, buf, WOL_DATA_LEN, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
  
  /* clean up */
  free(buf);
  close(sockfd);

  return 0;
}
