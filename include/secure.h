#define SEC_CDUMP_FAIL     -1
#define SEC_CDUMP_SUCC      0

/*
 * Core dumps are a security risk
 * because they leave the contents
 * of memory in text files.
 *
 * This disables the use of core dumps
 */
int no_core_dumps();
