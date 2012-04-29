/*
** Wake-On-LAN tool
** sends a "magic packet" to the specified
** computer on the ethernet LAN.
**
** Copyright 2012 Mario Campos
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>                    /* ETH_ALEN */
#include <net/ethernet.h>                      /* struct ether_addr */
#include <netinet/ether.h>                     /* ether_aton() */

#define TRUE               1
#define FALSE              0
#define EXIT_ERR           1
#define EXIT_SUCC          0
#define WOL_FRAME_LEN      108                 /* Max Length of a Wake-On-LAN packet */
#define ETH_P_WOL          0x0842              /* Ethernet Protocol ID for Wake-On-LAN */

int main(unsigned int argc, char *argv[])
{
  unsigned char i;

  /* socket file descripter */
  int sockfd;

  /* wake-on-lan target mac address */
  struct ether_addr *wol_addr;

  /* pointers to ethernet frame payload*/
  void *buf, *payload;

  /* packet destination */
  struct sockaddr_ll dest_addr;
  
  if(argc != 2) {
    fprintf(stderr, "Usage: %s <mac address>\n", argv[0]);
    return EXIT_ERR;
  }

  /* input validation and serialization */
  if((wol_addr = ether_aton(argv[1])) == NULL) {
    fprintf(stderr, "error: argument doesn't match MAC-48 address format\n");
    return EXIT_ERR;
  }

  /* create "packet" (ethernet frame) socket */
  if((sockfd = socket(PF_PACKET, SOCK_DGRAM, htons(ETH_P_WOL))) == -1) {
      perror("socket");
      return errno;
  }

  /* create buffer for payload */
  buf = (void *) malloc(WOL_FRAME_LEN);
  payload = buf;

  /* prepare frame payload */
  memset(payload, 0xFF, ETH_ALEN);
  for(i=0, payload += ETH_ALEN; i < 16; ++i, payload += ETH_ALEN)
    memcpy(payload, wol_addr->ether_addr_octet, ETH_ALEN);

  /* prepare socket destination struct */
  dest_addr.sll_family = AF_PACKET;
  dest_addr.sll_protocol = htons(ETH_P_WOL);
  dest_addr.sll_ifindex = 2;
  dest_addr.sll_hatype = ARPHRD_ETHER;
  dest_addr.sll_pkttype = PACKET_BROADCAST;
  dest_addr.sll_halen = ETH_ALEN;
  memset(dest_addr.sll_addr, 0xFF, ETH_ALEN);

  /* send "packet" (frame) */
  if(sendto(sockfd, buf, WOL_FRAME_LEN, 0, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_ll)) == -1) {
    perror("sendto");
    return errno;
  }

  printf("Magic Packet sent to %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",
	 wol_addr->ether_addr_octet[0], wol_addr->ether_addr_octet[1], wol_addr->ether_addr_octet[2],
	 wol_addr->ether_addr_octet[3], wol_addr->ether_addr_octet[4], wol_addr->ether_addr_octet[5]);

  /* clean up */
  free(buf);
  close(sockfd);
  return EXIT_SUCC;
}
