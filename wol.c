/*
 * Copyright (c) 2012 iamrekcah
 * Copyright (c) 2023 Mario Campos
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
 * Wake-On-LAN tool
 * This program sends a "magic packet" to the specified
 * -- via the MAC address -- computer on the Ethernet LAN.
 */

#include "config.h"

#include <sysexits.h>                          /* EX_DATAERR, EX_NOINPUT */
#include <string.h>                            /* memset() */
#include <linux/if_packet.h>                   /* sockaddr_ll */
#include <netinet/ether.h>                     /* ether_aton() */
#include <net/if.h>                            /* if_nametoindex() */
#include <unistd.h>                            /* close() */
#include <stdio.h>                             /* perror(), puts() */
#include <errno.h>                             /* errno */
#include <error.h>                             /* error() */
#include <stdlib.h>                            /* exit() */
#include <arpa/inet.h>                         /* htons() */
#include <stdbool.h>                           /* true, false, bool */
#include <argp.h>                              /* ArgP */

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
 * A structure for containing the results of parsing the argument vector.
 */
struct arguments {
    bool use_q;
    bool use_p;
    bool use_i;
    const char *ifacename;
    volatile const char *password;
    const char *target;
};

const char *argp_program_version = VERSION;
const char *argp_program_bug_address = "<https://github.com/mario-campos/wol>";

error_t
parser(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;

    switch(key) {
	/* parsed --{quiet|silent} | -{q|s} */
	case 'q':
	case 's':
	    arguments->use_q = true;
	    break;

	    /* parsed --interface | -i */
	case 'i':
	    arguments->use_i = true;
	    arguments->ifacename = arg;
	    break;

	    /* parsed --password | -p */
	case 'p':
	    arguments->use_p = true;
	    arguments->password = arg;
	    break;

	    /* parsed <target mac addr> */
	case ARGP_KEY_ARG:
	    if(state->arg_num >= 1) {
		argp_usage(state);
		return EINVAL;
	    }
	    arguments->target = arg;
	    break;

	    /* <target mac addr> not provided */
	case ARGP_KEY_NO_ARGS:
	    argp_usage(state);
	    return EINVAL;

	case ARGP_KEY_ERROR:
	    return errno;
    }

    return 0;
}


/*
 * parse_cmdline will process the command-line and store the results
 * in the given arguments struct.
 *
 * params
 *    struct arguments * : A pointer to struct arguments for results.
 *    char ** : An array of C-strings (command-line args).
 *    size_t : The length of the array.
 *
 * returns
 *    0 : on success.
 */
int
parse_cmdline(struct arguments *args, char **argv, size_t argc) {

    /* set defaults */
    args->use_p = false;
    args->use_i = false;
    args->use_q = false;

    /* expected switches */
    struct argp_option options[] = {
	    {"quiet"    , 'q', 0,            0, "No output"},
	    {"silent"   , 's', 0, OPTION_ALIAS, NULL},
	    {"password" , 'p', "PASSWORD",   0, "Specify the WOL password"},
	    {"interface", 'i', "NAME",       0, "Specify the net interface to use"},
	    { 0 }
    };

    const char args_doc[] = "<mac address>";
    const char doc[] = "Wake-On-LAN packet sender";
    struct argp argp = { options, parser, args_doc, doc };
    return argp_parse(&argp, (int)argc, argv, 0, 0, args);
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

int main(int argc, char **argv) {
    struct arguments args;
    parse_cmdline(&args, argv, argc);

    /* validate interface input */
    int iface_index;
    if(args.use_i) {
    iface_index = if_nametoindex(args.ifacename);
    if(iface_index == 0)
	error(EX_NOINPUT, errno, "invalid interface");
    } else {
    	iface_index = if_nametoindex("eth0");
    }

    /* validate MAC Address */
    struct ether_addr *addr = ether_aton(args.target);
    struct ether_addr macaddr;
    if(addr == NULL) {
    	error(EX_DATAERR, errno, "invalid MAC address");
    } else {
    	macaddr = *addr;
    }

    /* configure destination */
    struct sockaddr_ll dest_addr;
    dest_addr.sll_family   = AF_PACKET;
    dest_addr.sll_protocol = htons(ETH_P_WOL);
    dest_addr.sll_ifindex  = iface_index;
    dest_addr.sll_hatype   = ARPHRD_ETHER;
    dest_addr.sll_pkttype  = PACKET_BROADCAST;
    dest_addr.sll_halen    = ETH_ALEN;
    memset(dest_addr.sll_addr, 0xFF, ETH_ALEN);

    int sockfd = socket(PF_PACKET, SOCK_DGRAM, htons(ETH_P_WOL));
    if(sockfd == -1) {
	perror("socket");
	exit errno;
    }
    volatile char buf[WOL_DATA_LEN + WOL_PASSWD_LEN];
    size_t buf_len;

    // set frame payload with password
    if(args.use_p) {
	buf_len = WOL_DATA_LEN + WOL_PASSWD_LEN;
	set_payload_wp(&buf, &macaddr, pconvert((const char *)args.password));
    }

    // set frame payload without password
    else {
	buf_len = WOL_DATA_LEN;
	set_payload(&buf, &macaddr);
    }

    if(-1 == sendto(sockfd, buf, buflen, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr))) {
	perror("sendto");
	exit errno;
    }

    /* clean up */
    close(sockfd);

    /* wipe password buffers */
    memset((void*)&buf, 0, buf_len);
    if(args.use_p) {
	memset((void*)args.password, 0, strlen((const char *)args.password));
    }
    return EX_OK;
}
