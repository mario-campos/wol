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

#define WOL_DATA_LEN       102         /* Max Length of a Wake-On-LAN packet */
#define WOL_PASSWD_LEN     6           /* Max Length of a Wake-On-LAN password */
#define ETH_P_WOL          0x0842      /* Ethernet Protocol ID for Wake-On-LAN */

/*
 * Password data type.
 *
 * A password is made up of 48 bits. For all intents and purposes,
 * an ether_addr struct can work, but this way is more logical.
 */
struct password { 
  char x[WOL_PASSWD_LEN]; 
};

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
Sendto(int, void *, size_t, struct sockaddr_ll *, size_t);

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
set_payload_wp(void *, struct ether_addr *, struct password *);


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
pconvert(const char *);

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

/*
 * Malloc is a wrapper around malloc() with error checking
 *
 * params
 *    size of memory to allocate
 *
 * returns
 *    address of allocated space
 */
void *
Malloc(size_t);

#endif /* WOL_H */
