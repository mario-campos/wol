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
 * wol.c
 *
 * Wake-On-LAN protocol function implementations and definitions
 */

#include "config.h"

#include <stdlib.h>                            /* exit() */
#include <error.h>                             /* error() */
#include <errno.h>                             /* errno */
#include <stdio.h>                             /* perror(), puts() */
#include <string.h>                            /* memcpy() */
#include <arpa/inet.h>                         /* htons() */
#include <linux/if_packet.h>                   /* sockaddr_ll */
#include <netinet/ether.h>                     /* ether_aton() */

#include "wol.h"

/*
 * Socket wraps around socket() to abstract the process 
 * of creating a layer-2 (Ethernet Frame) socket.
 *
 * returns
 *    socket file descriptor
 */
int
Socket() {
  int sockfd = socket(PF_PACKET, SOCK_DGRAM, htons(ETH_P_WOL));
  if(sockfd == -1) {
    perror("socket");
    exit errno;
  }
  return sockfd;
}

/*
 * Sends a packet given the payload, socket file descriptor, 
 * and destination address.
 *
 * params
 *    sockfd : socket file descriptor.
 *    buf : a void pointer to payload buffer.
 *    buflen : length of payload buffer.
 *    dest_addr : target address structure.
 *    daddr_len : length of target-address struct.
 */
void
Sendto(int sockfd, void *buf, size_t buflen, 
       struct sockaddr_ll *dest_addr, size_t daddr_len) {
  if(buf == NULL || dest_addr == NULL) {
    error(1, 0, "Unable to send due to NULL reference.");
  }
  int retval = sendto(sockfd, buf, buflen, 0, (struct sockaddr *)dest_addr, daddr_len);
  if(retval == -1) {
   perror("sendto");
   exit errno;
  }
}

/*
 * set_payload_wp writes the provided address into the provided
 * buffer according to the Wake-On-LAN protocol. Then, writes the
 * password at the end of the payload, as per the WOL protocol.
 *
 * params
 *    pointer to buffer (payload)
 *    pointer to a struct ether_addr
 *    pointer to password structure
 */
void
set_payload_wp(void *payload_ptr, struct ether_addr *macaddr, struct password *passwd) {
  if(payload_ptr == NULL || macaddr == NULL || passwd == NULL) {
    error(1, 0, "Unable to set payload due to NULL reference.");
  }
  set_payload(payload_ptr, macaddr);
  memcpy(payload_ptr + WOL_DATA_LEN, passwd, WOL_PASSWD_LEN);
}

/*
 * pconvert returns a password (in the form of a password_t struct)
 * given the c-string password. This function implements ether_aton.
 *
 * params
 *    The password char string
 *
 * returns
 *    A pointer to a populated password struct
 */
struct password *
pconvert(const char *passwd_str) {
  if(passwd_str == NULL) {
    error(1, 0, "Unable to convert password due to NULL reference.");
  }
  static struct password result;
  struct ether_addr *hex_pass = ether_aton(passwd_str);
  memcpy(&result, hex_pass, WOL_PASSWD_LEN);
  return &result;
}

/*
 * set_payload writes the provided address into the provided
 * buffer according to the Wake-On-LAN protocol.
 *
 * params
 *    buf : pointer to payload buffer
 *    addr : pointer to a struct ether_addr.
 */
void
set_payload(void *buf, struct ether_addr *addr) {
  if(buf == NULL || addr == NULL) {
    error(1, 0, "Unable to set payload due to NULL reference.");
  }

  int i;
  void *ptr = buf;
  memset(ptr, 0xFF, ETH_ALEN);
  for(i=0, ptr += ETH_ALEN; i < 16; ++i, ptr += ETH_ALEN) {
    memcpy(ptr, addr, ETH_ALEN);
  }
}

/*
 * prepare_da configures the destination-addres structure
 * sockaddr_ll accordingly for Ethernet frames.
 *
 * params
 *    dest_addr : Pointer to destination address structure, sockaddr_ll
 *    iface_index : integer number representing the network interface
 */
void
prepare_da(struct sockaddr_ll *dest_addr, int iface_index) {
  if(dest_addr == NULL) {
    error(1, 0, "Unable to prepare destination due to NULL reference.");
  }

  dest_addr->sll_family   = AF_PACKET;
  dest_addr->sll_protocol = htons(ETH_P_WOL);
  dest_addr->sll_ifindex  = iface_index;
  dest_addr->sll_hatype   = ARPHRD_ETHER;
  dest_addr->sll_pkttype  = PACKET_BROADCAST;
  dest_addr->sll_halen    = ETH_ALEN;
  memset(dest_addr->sll_addr, 0xFF, ETH_ALEN);
}
