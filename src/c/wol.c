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
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <regex.h>

#define TRUE               1
#define FALSE              0
#define EXIT_ERR           1
#define EXIT_SUCC          0
#define WOL_FRAME_LEN      108                 /* Max Length of a Wake-On-LAN packet */
#define ETH_P_WOL          0x0842              /* Ethernet Protocol ID for Wake-On-LAN */

int isMacAddress(const char *addr_str)
{
  int retval;
  regex_t mac48_regex;

  /* compile regex */
  if(regcomp(&mac48_regex, "^([:xdigit:]\\{1,2\\}:)\\{5,5\\}[:xdigit:]\\{2,2\\}$", 0) != 0) {
    perror("regcomp");
    exit(errno);
  }
    
  if(regexec(&mac48_regex, addr_str, 0, NULL, 0))
    retval = TRUE;
  else 
    retval = FALSE;

  regfree(&mac48_regex);
  return retval;
}

int serializeMacAddress(const char *str, char *buf)
{
  unsigned char c, bigend, bufi, i;
  const unsigned int len = strlen(str);

  for(i=0, bufi=0, bigend=TRUE; i < len; ++i) {
    if(str[i] == ':' || str[i] == '-') {
      c = 0;
      bufi++;
      bigend = TRUE;
      continue;
    }
    
    if(str[i] == 'F' || str[i] == 'f') c += 15;
    if(str[i] == 'E' || str[i] == 'e') c += 14;
    if(str[i] == 'D' || str[i] == 'd') c += 13;
    if(str[i] == 'C' || str[i] == 'c') c += 12;
    if(str[i] == 'B' || str[i] == 'b') c += 11;
    if(str[i] == 'A' || str[i] == 'a') c += 10;
    if(str[i] == '9') c += 9;
    if(str[i] == '8') c += 8;
    if(str[i] == '7') c += 7;
    if(str[i] == '6') c += 6;
    if(str[i] == '5') c += 5;
    if(str[i] == '4') c += 4;
    if(str[i] == '3') c += 3;
    if(str[i] == '2') c += 2;
    if(str[i] == '1') c += 1;

    if(bigend == TRUE) {
      c *= 16;
      bigend = FALSE;
    }
    buf[bufi] = c;
  }

  return TRUE;
}

int main(unsigned int argc, char *argv[])
{
  unsigned char i;

  /* socket file descripter */
  int sockfd;

  /* wake-on-lan target mac address */
  unsigned char wol_mac[ETH_ALEN];

  /* ethernet broadcast address */
  unsigned char bcast_mac[ETH_ALEN] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

  /* ethernet host mac address */
  unsigned char host_mac[ETH_ALEN] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE };

  /* pointers to ethernet frame payload*/
  void *buf, *payload;

  /* packet destination */
  struct sockaddr_ll dest_addr;
  
  if(argc != 2) {
    fprintf(stderr, "Usage: %s <mac address>\n", argv[0]);
    return EXIT_ERR;
  }

  /* input validation and serialization */
  if(isMacAddress(argv[1]) == TRUE) serializeMacAddress(argv[1], wol_mac);
  else {
    fprintf(stderr, "error: argument doesn't match MAC-48 MAC-Address format\n");
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
  memcpy(payload, bcast_mac, ETH_ALEN);
  for(i=0, payload += ETH_ALEN; i < 16; ++i, payload += ETH_ALEN)
    memcpy(payload, wol_mac, ETH_ALEN);
  payload = NULL;

  /* prepare socket destination struct */
  dest_addr.sll_family = AF_PACKET;
  dest_addr.sll_protocol = htons(ETH_P_WOL);
  dest_addr.sll_ifindex = 2;
  dest_addr.sll_hatype = ARPHRD_ETHER;
  dest_addr.sll_pkttype = PACKET_BROADCAST;
  dest_addr.sll_halen = ETH_ALEN;
  memcpy(dest_addr.sll_addr, bcast_mac, ETH_ALEN);

  /* send "packet" (frame) */
  if(sendto(sockfd, buf, WOL_FRAME_LEN, 0, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_ll)) == -1) {
    perror("sendto");
    return errno;
  }

  printf("Magic Packet sent to %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",
	 wol_mac[0], wol_mac[1], wol_mac[2],
	 wol_mac[3], wol_mac[4], wol_mac[5]);

  /* clean up */
  free(buf);
  close(sockfd);
  return EXIT_SUCC;
}
