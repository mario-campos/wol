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
#include <sys/socket.h>                        /* socket */
#include <linux/if_packet.h>                   /* sockaddr_ll */
#include <linux/if_ether.h>                    /* ETH_ALEN */
#include <net/ethernet.h>                      /* struct ether_addr */
#include <net/if.h>                            /* if_nametoindex() */
#include <netinet/ether.h>                     /* ether_aton() */

#define TRUE               1
#define FALSE              0
#define EXIT_ERR           1
#define EXIT_SUCC          0
#define NO_INDEX           -1                  /* Illegal Array Index */
#define VERSION            "1.0b"              /* Current version number */
#define WOL_DATA_LEN       102                 /* Max Length of a Wake-On-LAN packet */
#define WOL_PASSWD_LEN     6                   /* Max Length of a Wake-On-LAN password */
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
  puts("wol [--help|-h] [--quiet|-q] [--version|-v] [--interface|-i <name>] [--password|-p <passwd>] <mac address>\n");
  puts("--help|-h\t\tPrints this help message and exit.");
  puts("--quiet|-q\t\tDisable output.");
  puts("--version|-v\t\tPrints version number and exit.");
  puts("--interface|-i <name>\tSpecify the interface for the packet (default eth0).");
  puts("--password|-p <passwd>\tSend a colon-delimited hex password (default none).");
  puts("<mac address>\t\tMAC-48 address (colon delimited).");
}

int main(unsigned int argc, char *argv[])
{
  int index;
  unsigned char i;
  int buflen = WOL_DATA_LEN;                   /* buffer length */
  int sockfd;                                  /* socket file descripter */
  unsigned char iface_index;                   /* interface index number */
  unsigned char use_passwd = FALSE;            /* assume no password */
  unsigned char quiet = FALSE;                 /* assume verbose output */
  void *buf, *payload;                         /* Buffer pointers */
  struct sockaddr_ll dest_addr;                /* ethernet frame dest address */
  struct ether_addr *wol_addr;                 /* wol-target mac address */
  struct ether_addr *password;                 /* wol password in mac-48 format */

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
    iface_index = if_nametoindex(argv[index+1]);
  else 
    iface_index = 2;

  if(
     (index = aindex("--password", argv, argc)) != NO_INDEX ||
     (index = aindex("-p", argv, argc))         != NO_INDEX
     ) {
    if((password = ether_aton(argv[index+1])) == NULL) {
      fprintf(stderr, "error: invalid password; follow password format\n");
      return EXIT_ERR;
    }
    buflen = WOL_DATA_LEN + WOL_PASSWD_LEN;
    use_passwd = TRUE;
  }

  /* input validation and serialization */
  if((wol_addr = ether_aton(argv[argc-1])) == NULL) {
    fprintf(stderr, "error: target isn't a MAC-48 address\n");
    return EXIT_ERR;
  }

  /* create "packet" (ethernet frame) socket */
  if((sockfd = socket(PF_PACKET, SOCK_DGRAM, htons(ETH_P_WOL))) == -1) {
      perror("socket");
      return errno;
  }

  /* create buffer for payload */
  buf = (void *) malloc(buflen);
  payload = buf;

  /* prepare frame payload */
  memset(payload, 0xFF, ETH_ALEN);
  for(i=0, payload += ETH_ALEN; i < 16; ++i, payload += ETH_ALEN)
    memcpy(payload, wol_addr, ETH_ALEN);
  if(use_passwd)
    memcpy(payload, password, WOL_PASSWD_LEN);

  /* prepare socket destination struct */
  dest_addr.sll_family = AF_PACKET;
  dest_addr.sll_protocol = htons(ETH_P_WOL);
  dest_addr.sll_ifindex = iface_index;
  dest_addr.sll_hatype = ARPHRD_ETHER;
  dest_addr.sll_pkttype = PACKET_BROADCAST;
  dest_addr.sll_halen = ETH_ALEN;
  memset(dest_addr.sll_addr, 0xFF, ETH_ALEN);

  /* send "packet" (frame) */
  if(sendto(sockfd, buf, buflen, 0, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_ll)) == -1) {
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
