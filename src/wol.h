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
 * wol.h
 *
 * Wake-On-LAN protocol functions and definitions
 */

#ifndef WOL_H
#define WOL_H

#include <linux/if_packet.h>                   /* sockaddr_ll */
#include <netinet/ether.h>                     /* ether_aton() */

#include "common.h"

#define WOL_DATA_LEN       102         /* Max Length of a Wake-On-LAN packet */
#define WOL_PASSWD_LEN     6           /* Max Length of a Wake-On-LAN password */
#define ETH_P_WOL          0x0842      /* Ethernet Protocol ID for Wake-On-LAN */

/*
 * Send a "magic" (WOL) Ethernet frame to the given MAC address.
 *
 * params:
 *   A pointer to the target's MAC address struct.
 *
 * returns:
 *   0 : on success.
 */
int
send_magicpkt(struct ether_addr *);

/*
 * Socket wraps around socket() to abstract the process 
 * of creating a layer-2 (Ethernet Frame) socket.
 *
 * returns
 *    socket file descriptor
 */
int
Socket();

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
Sendto(int, void *, size_t, struct sockaddr *, size_t);

/*
 * prepare_payload dynamically allocates a buffer for
 * the frame payload, and sets the payload to 
 * match the WOL protocol for the given MAC Address.
 *
 * params
 *    The MAC address structure of target.
 *
 * returns
 *    A pointer to the buffer.
 */
void *
prepare_payload(struct ether_addr *);

/*
 * set_payload writes the provided address into the provided
 * buffer according to the Wake-On-LAN protocol.
 *
 * params
 *    void * : pointer to payload buffer
 *    struct ether_addr * : pointer to a struct ether_addr.
 */
void
set_payload(void *, struct ether_addr *);

/*
 * prepare_da configures the destination-addres structure
 * sockaddr_ll accordingly for Ethernet frames.
 *
 * params
 *    struct sockaddr_ll : Pointer to destination address structure, sockaddr_ll
 *    int : integer number representing the network interface
 */
void
prepare_da(struct sockaddr_ll *, int);

#endif /* WOL_H */
