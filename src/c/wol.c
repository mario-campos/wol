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
#define NO_INDEX           -1                  /* Illegal Array Index */
#define VERSION            "1.0b"              /* Current version number */
#define WOL_FRAME_LEN      108                 /* Max Length of a Wake-On-LAN packet */
#define ETH_P_WOL          0x0842              /* Ethernet Protocol ID for Wake-On-LAN */

int aindex(char *str, char **array, unsigned int arraylen)
{
  unsigned int i;
  for(i=0; i < arraylen; ++i)
    if(strcmp(str, array[i]) == 0) return i;
  return NO_INDEX;
}

void printHelp()
{
  puts("wol [--help|-h] [--quiet|-q] [--version|-v] [--interface|-i <name>] [--password|-p <passwd>] <mac address>");
  puts("\t--help|-h\t\tPrints this help message and exit.");
  puts("\t--quiet|-q\t\tDisable output.");
  puts("\t--version|-v\t\tPrints version number and exit.");
  puts("\t--interface|-i <name>\tSpecify the interface for the packet.");
  puts("\t--password|-p <passwd>\tSend a hex password in the packet.");
  puts("\t<mac address>\t\tMAC-48 address (colon or hyphen delimited).");
}

int main(unsigned int argc, char *argv[])
{
  int index, sockfd;
  unsigned char i, quiet, *iface_name;
  void *buf, *payload;
  struct sockaddr_ll dest_addr;
  struct ether_addr *password, *wol_addr;

  /* parse command-line for switches */
  if(aindex("-h", argv, argc) != NO_INDEX || aindex("--help", argv, argc) != NO_INDEX) {
    printHelp();
    return EXIT_SUCC;
  }
  
  if(aindex("-v", argv, argc) != NO_INDEX || aindex("--version", argv, argc) != NO_INDEX) {
    printf("%s\n", VERSION);
    return EXIT_SUCC;
  }

  if(aindex("-q", argv, argc) != NO_INDEX || aindex("--quiet", argv, argc) != NO_INDEX)
    quiet = TRUE;

  if(
     (index = aindex("--interface", argv, argc)) != NO_INDEX ||
     (index = aindex("-i", argv, argc))          != NO_INDEX
    )
    iface_name = argv[index+1];

  if(
     (index = aindex("--password", argv, argc)) != NO_INDEX ||
     (index = aindex("-p", argv, argc))         != NO_INDEX
    )
    password = ether_aton(argv[index+1]);

  /* input validation and serialization */
  if((wol_addr = ether_aton(argv[argc-1])) == NULL) {
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

  if(quiet != TRUE) 
    printf("Magic Packet sent to %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",
	   wol_addr->ether_addr_octet[0], wol_addr->ether_addr_octet[1], wol_addr->ether_addr_octet[2],
	   wol_addr->ether_addr_octet[3], wol_addr->ether_addr_octet[4], wol_addr->ether_addr_octet[5]);

  /* clean up */
  free(buf);
  close(sockfd);
  return EXIT_SUCC;
}
