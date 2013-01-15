#include "tap.h"
#include "usage.h"
#include "../../src/usage.c"

/*
 * Tests the short option -i <interface>.
 *
 * The test succeeds if the argument string
 * that is passed to the -i option matches
 * the string that is returned.
 */
int test_option_i_succeeds(void)
{
  struct arguments args = {0};
  char *argv[] = {"programname", "-i", "interfacename", "argument"};
  int retval = parse_cmdline(&args, argv, 4);
  return retval == 0
    &&   args.use_i == 1
    &&   strcmp(argv[2], args.ifacename) == 0;
}

int test_option_interface_succeeds(void)
{
  struct arguments args = {0};
  char *argv[] = {"programname", "--interface", "test", "macaddr"};
  int retval = parse_cmdline(&args, argv, 4);
  return retval == 0
    &&   args.use_i == 1
    &&   strcmp(argv[2], args.ifacename) == 0;
}

int main(void)
{
  plan_tests(2);
  ok(test_option_i_succeeds(), "testing option -i");
  ok(test_option_interface_succeeds(), "testing option --interface");
  return exit_status();
}
