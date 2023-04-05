#include <assert.h>
#include <string.h>
#include <sys/socket.h>

// wol.c calls socket(2) with `PF_PACKET` for the `domain`. `PF_PACKET`
// requires root permission. So, socket(2) is preloaded here to avoid
// needing root to run this test case.
int socket(int domain, int type, int protocol)
{
    return 0;
}

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen)
{
    // The magic packet is a frame that is most often sent as a broadcast and that contains
    // anywhere within its payload 6 bytes of all 255 (FF FF FF FF FF FF in hexadecimal)
    assert(0 == memcmp(buf, "\xFF\xFF\xFF\xFF\xFF\xFF", 6));

    // followed by sixteen repetitions of the target computer's 48-bit MAC address, for a total of 102 bytes.
    const char *target_mac = "\x11\x22\x33\x44\x55\x66"
			     "\x11\x22\x33\x44\x55\x66"
			     "\x11\x22\x33\x44\x55\x66"
			     "\x11\x22\x33\x44\x55\x66"
			     "\x11\x22\x33\x44\x55\x66"
			     "\x11\x22\x33\x44\x55\x66"
			     "\x11\x22\x33\x44\x55\x66"
			     "\x11\x22\x33\x44\x55\x66"
			     "\x11\x22\x33\x44\x55\x66"
			     "\x11\x22\x33\x44\x55\x66"
			     "\x11\x22\x33\x44\x55\x66"
			     "\x11\x22\x33\x44\x55\x66"
			     "\x11\x22\x33\x44\x55\x66"
			     "\x11\x22\x33\x44\x55\x66"
			     "\x11\x22\x33\x44\x55\x66"
			     "\x11\x22\x33\x44\x55\x66";
    assert(0 == memcmp(buf + 6, target_mac, 96));

    return len;
}