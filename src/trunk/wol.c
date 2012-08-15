/*
** Wake-On-LAN tool
** sends a "magic packet" to the specified
** computer on the ethernet LAN.
**
** Copyright 2012 Mario Campos
*/

#include "common.h"
#include "secure.c"
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <argp.h>
#include "wol.h"                               /* WOL definitions */
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

const char *argp_program_version = "1.0 beta";
const char *argp_program_bug_address = "<mario.andres.campos@gmail.com>";

struct arguments {
  int use_q;
  int use_p;
  int use_i;
  const char *ifacename;
  const char *password;
  const char *target;
};

error_t parser(int key, char *arg, struct argp_state *state)
{
  int retval;
  struct arguments *arguments = state->input;
  
  switch(key) {
    /* parsed --{quiet|silent} | -{q|s} */
  case 'q':
  case 's':
    arguments->use_q = TRUE;
    goto EXIT0;
 
    /* parsed --interface | -i */
  case 'i':
    arguments->use_i = TRUE;
    arguments->ifacename = arg;
    goto EXIT0;

    /* parsed --password | -p */
  case 'p':
    arguments->use_p = TRUE;
    arguments->password = arg;
    goto EXIT0;

    /* parsed <target mac addr> */
  case ARGP_KEY_ARG:
    arguments->target = arg;
    goto EXIT0;

    /* <target mac addr> not provided */
  case ARGP_KEY_NO_ARGS:
    goto EXITFAIL;

  default:
    goto EXITFAIL;
  }

 EXIT0:
  retval = 0;
  goto EXIT;

 EXITFAIL:
  retval = ARGP_ERR_UNKNOWN;
  goto EXIT;

 EXIT:
  return retval;
}

int main(int argc, char *argv[])
{
  int retval;
  unsigned char i;
  int buflen = WOL_DATA_LEN;                        /* buffer length */
  int sockfd = 0;                                   /* socket file descripter */
  unsigned int iface_index;                         /* interface index number */


  void *buf = NULL, *payload;                       /* Buffer pointers */
  struct sockaddr_ll dest_addr;                     /* ethernet frame dest address */
  struct ether_addr *mac_addr, wol_addr, password;  /* mac address structure (6 bytes structs) */

  /* disable the use of core memory dumps */
  no_core_dumps();
  
  /* define command-line switches */
  struct argp_option options[] = {
    {"quiet"    , 'q', 0, 0, "No output"},
    {"silent"   , 's', 0, OPTION_ALIAS, NULL},
    {"password" , 'p', "PASSWORD", 0, "Submit the WOL password"},
    {"interface", 'i', "NAME",     0, "Specify the net interface to use"},
    { 0 }
  };

  struct arguments arguments = { 0 };

  const char args_doc[] = "{mac address}";

  const char doc[] = "Ethernet-frame based Wake-On-LAN client";

  struct argp argp = { options, parser, args_doc, doc };
  
  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  if(arguments.use_i)
    iface_index = if_nametoindex(arguments.ifacename);
  else 
    iface_index = 2;

  if(arguments.use_p) {
    buflen = WOL_DATA_LEN + WOL_PASSWD_LEN;
    memcpy(&password, mac_addr, WOL_PASSWD_LEN);
  }

  /* input validation and serialization */
  if((mac_addr = ether_aton(arguments.target)) == NULL) {
    fprintf(stderr, "error: target isn't a MAC-48 address\n");
    retval = EXIT_ERR;
    goto exit;
  }

  memcpy(&wol_addr, mac_addr, WOL_PASSWD_LEN);

  /* create "packet" (ethernet frame) socket */
  if((sockfd = socket(PF_PACKET, SOCK_DGRAM, htons(ETH_P_WOL))) == -1) {
      perror("socket");
      retval = errno;
      goto exit;
  }

  /* create buffer for payload */
  buf = (void *) malloc(buflen);
  payload = buf;

  /* prepare frame payload */
  memset(payload, 0xFF, ETH_ALEN);
  for(i=0, payload += ETH_ALEN; i < 16; ++i, payload += ETH_ALEN)
    memcpy(payload, &wol_addr, ETH_ALEN);
  if(arguments.use_p)
    memcpy(payload, &password, WOL_PASSWD_LEN);

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
    retval = errno;
    goto exit;
  }

  if(arguments.use_q == FALSE) 
    printf("Magic Packet sent to %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",
	   wol_addr.ether_addr_octet[0], wol_addr.ether_addr_octet[1], wol_addr.ether_addr_octet[2],
	   wol_addr.ether_addr_octet[3], wol_addr.ether_addr_octet[4], wol_addr.ether_addr_octet[5]);

  /* clean up */
 exit:
  if(buf != NULL) free(buf);
  if(sockfd != 0) close(sockfd);
  return EXIT_SUCC;
}
