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

#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <argp.h>
#include <stdio.h>                             /* perror(), puts() */
#include <string.h>                            /* memcpy() */
#include <unistd.h>                            /* close() */
#include <arpa/inet.h>                         /* htons() */
#include <sys/socket.h>                        /* socket() */
#include <linux/if_packet.h>                   /* sockaddr_ll */
#include <linux/if_ether.h>                    /* ETH_ALEN */
#include <net/ethernet.h>                      /* struct ether_addr */
#include <net/if.h>                            /* if_nametoindex() */
#include <netinet/ether.h>                     /* ether_aton() */

#include "wol.h"


/*
 * Send a "magic" (WOL) Ethernet frame to the given MAC address.
 *
 * params:
 *   mac_addr : A pointer to the target's MAC address struct.
 *
 * returns:
 *   0 : on success.
 */
int
send_magicpkt(struct ether_addr *macaddr) {

  return 0;
}

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
       struct sockaddr *dest_addr, size_t daddr_len) {
  int retval = sendto(sockfd, buf, buflen, 0, dest_addr, daddr_len);

  if(retval == -1) {
   perror("sendto");
   exit errno;
  }
}

/*
 * prepare_payload dynamically allocates a buffer for
 * the frame payload, and sets the payload to 
 * match the WOL protocol for the given MAC Address.
 *
 * params
 *    macaddr : The MAC address structure of target.
 *
 * returns
 *    A pointer to the buffer.
 */
void *
prepare_payload(struct ether_addr *macaddr) {
  void *payload_ptr = malloc(WOL_DATA_LEN);

  if(payload_ptr == NULL) {
    perror("malloc");
    exit errno;
  }

  set_payload(payload_ptr, macaddr);
  return payload_ptr;
}

/*
 * prepare_payload_wp functions exactly like prepare_payload
 * except that it appends the password to the end of the
 * payload.
 *
 * params
 *    The MAC address structure of target.
 *    A pointer to the password_t datatype.
 *
 * returns
 *    A pointer to the buffer.
 */
void *
prepare_payload_wp(struct ether_addr *macaddr, struct password *passwd) {
  void *payload_ptr = malloc(WOL_DATA_LEN + WOL_PASSWD_LEN);

  if(payload_ptr == NULL) {
    perror("malloc");
    exit errno;
  }

  set_payload(payload_ptr, macaddr);
  memcpy(payload_ptr + WOL_DATA_LEN, passwd, WOL_PASSWD_LEN);
  return payload_ptr;
}

struct password *
pconvert(const char *passwd_str) {
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
  int i;
  void *ptr = buf;
  memset(ptr, 0xFF, ETH_ALEN);
  for(i=0, ptr += ETH_ALEN; i < 16; ++i, ptr += ETH_ALEN)
    memcpy(ptr, addr, ETH_ALEN);  
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
  dest_addr->sll_family = AF_PACKET;
  dest_addr->sll_protocol = htons(ETH_P_WOL);
  dest_addr->sll_ifindex = iface_index;
  dest_addr->sll_hatype = ARPHRD_ETHER;
  dest_addr->sll_pkttype = PACKET_BROADCAST;
  dest_addr->sll_halen = ETH_ALEN;
  memset(dest_addr->sll_addr, 0xFF, ETH_ALEN);
}

/*
int old_main(int argc, char *argv[])
{
  int retval;
  unsigned char i;
  int buflen = WOL_DATA_LEN;
  int sockfd = 0;
  unsigned int iface_index;


  void *buf = NULL, *payload;
  struct sockaddr_ll dest_addr;
  struct ether_addr *mac_addr, wol_addr, password;

  if(arguments.use_i)
    iface_index = if_nametoindex(arguments.ifacename);
  else 
    iface_index = 2;

  if(arguments.use_p) {
    buflen = WOL_DATA_LEN + WOL_PASSWD_LEN;
    memcpy(&password, mac_addr, WOL_PASSWD_LEN);
  }


  if((mac_addr = ether_aton(arguments.target)) == NULL) {
    fprintf(stderr, "error: target isn't a MAC-48 address\n");
    retval = EXIT_ERR;
    goto exit;
  }

  memcpy(&wol_addr, mac_addr, WOL_PASSWD_LEN);


  if((sockfd = socket(PF_PACKET, SOCK_DGRAM, htons(ETH_P_WOL))) == -1) {
      perror("socket");
      retval = errno;
      goto exit;
  }


  buf = (void *) malloc(buflen);
  payload = buf;


  memset(payload, 0xFF, ETH_ALEN);
  for(i=0, payload += ETH_ALEN; i < 16; ++i, payload += ETH_ALEN)
    memcpy(payload, &wol_addr, ETH_ALEN);
  if(arguments.use_p)
    memcpy(payload, &password, WOL_PASSWD_LEN);


  dest_addr.sll_family = AF_PACKET;
  dest_addr.sll_protocol = htons(ETH_P_WOL);
  dest_addr.sll_ifindex = iface_index;
  dest_addr.sll_hatype = ARPHRD_ETHER;
  dest_addr.sll_pkttype = PACKET_BROADCAST;
  dest_addr.sll_halen = ETH_ALEN;
  memset(dest_addr.sll_addr, 0xFF, ETH_ALEN);


  if(sendto(sockfd, buf, buflen, 0, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_ll)) == -1) {
    perror("sendto");
    retval = errno;
    goto exit;
  }

  if(arguments.use_q == false) 
    printf("Magic Packet sent to %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",
	   wol_addr.ether_addr_octet[0], wol_addr.ether_addr_octet[1], wol_addr.ether_addr_octet[2],
	   wol_addr.ether_addr_octet[3], wol_addr.ether_addr_octet[4], wol_addr.ether_addr_octet[5]);


 exit:
  if(buf != NULL) free(buf);
  if(sockfd != 0) close(sockfd);
  return EXIT_SUCC;
}
*/
