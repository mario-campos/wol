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
#include <string.h>                            /* memset(), explicit_bzero() */
#include <linux/if_packet.h>                   /* sockaddr_ll */
#include <netinet/ether.h>                     /* ether_aton() */
#include <net/ethernet.h>                      /* ETHER_ADDR_LEN */
#include <net/if.h>                            /* if_nametoindex() */
#include <unistd.h>                            /* close() */
#include <stdio.h>                             /* perror(), puts() */
#include <errno.h>                             /* errno */
#include <error.h>                             /* error() */
#include <stdlib.h>                            /* exit() */
#include <arpa/inet.h>                         /* htons() */
#include <stdbool.h>                           /* true, false, bool */
#include <argp.h>                              /* ArgP */

// The number of MAC addresss repetitions in the payload of the Wake-on-LAN magic packet.
#define WOL_MAGIC_ADDRESS_COUNT 16

// The size of the "0xFF0xFF0xFF0xFF0xFF0xFF" header in the Wake-on-LAN magic packet.
#define WOL_MAGIC_HEADER_SIZE 6

// The size of the SecureOn password field in the Wake-on-LAN magic packet.
#define WOL_MAGIC_PASSWORD_SIZE 6

// The size of the payload of the Wake-on-LAN magic packet.
#define WOL_MAGIC_SIZE (WOL_MAGIC_HEADER_SIZE + (WOL_MAGIC_ADDRESS_COUNT * ETHER_ADDR_LEN))

// The size of the payload of the Wake-on-LAN magic packet with the SecureOn password.
#define WOL_SECURE_MAGIC_SIZE  (WOL_MAGIC_SIZE + WOL_MAGIC_PASSWORD_SIZE)

// The Ethernet Protocol ID for the Wake-on-LAN protocol.
#define ETH_P_WOL 0x0842

struct wol_magic {
    char	       wol_mg_header[WOL_MAGIC_HEADER_SIZE];
    struct ether_addr  wol_mg_macaddr[WOL_MAGIC_ADDRESS_COUNT];
    char               wol_mg_password[WOL_MAGIC_PASSWORD_SIZE];
} __attribute__((__packed__));

struct arguments {
    bool use_q;
    bool use_p;
    bool use_i;
    const char *ifacename;
    const char *password;
    const char *target;
};

const char *argp_program_version = VERSION;
const char *argp_program_bug_address = "<https://github.com/mario-campos/wol>";

error_t parser(int key, char *arg, struct argp_state *state) {
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
int parse_cmdline(struct arguments *args, char **argv, size_t argc) {
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

int main(int argc, char **argv) {
    size_t wol_password_size = 0;
    struct wol_magic magic = { .wol_mg_header = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} };

    struct arguments args;
    parse_cmdline(&args, argv, argc);

    int iface_index;
    if(args.use_i) {
	iface_index = if_nametoindex(args.ifacename);
	if(iface_index == 0)
	    error(EX_NOINPUT, errno, "invalid interface");
    } else {
	// TODO: Don't hardcode the interface name, because this couples the code to a particular
	// platform (Linux). And not even that well, because different GNU/Linux distro can (and have)
	// change the interface-naming convention.
    	iface_index = if_nametoindex("eth0");
    }

    struct ether_addr *target_mac_addr = ether_aton(args.target);
    if(NULL == target_mac_addr) {
    	error(EX_DATAERR, errno, "invalid MAC address");
    }

    if(args.use_p) {
	if (strlen(args.password) > WOL_MAGIC_PASSWORD_SIZE) {
	    error(EX_DATAERR, errno, "the supplied password is too long; please supply a password of length 6 characters or less.");
	}
	wol_password_size = strlen(args.password) < WOL_MAGIC_PASSWORD_SIZE ? strlen(args.password) : WOL_MAGIC_PASSWORD_SIZE;
	strncpy(magic.wol_mg_password, args.password, wol_password_size);
    }

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

    for(size_t i = 0; i < WOL_MAGIC_ADDRESS_COUNT; ++i) {
	magic.wol_mg_macaddr[i] = *target_mac_addr;
    }

    if(-1 == sendto(sockfd, (const void *)&magic, args.use_p ? WOL_MAGIC_SIZE + wol_password_size : WOL_MAGIC_SIZE, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr))) {
	perror("sendto");
	exit errno;
    }

    return EX_OK;
}
