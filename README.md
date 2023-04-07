# wol [![Build](https://github.com/mario-campos/wol/actions/workflows/build.yml/badge.svg)](https://github.com/mario-campos/wol/actions/workflows/build.yml)

A Wake-On-LAN utility

### DESCRIPTION

`wol` is a command-line tool for implementing the Wake-On-LAN protocol over a local-area network.

### INSTALL

```shell
./autogen.sh
./configure
make
sudo make install
```

### USAGE

```shell
Usage: wol [OPTION...] <mac address>
Wake-On-LAN packet sender

  -i, --interface=NAME       Specify the network interface through which to send [required]
  -p, --password=PASSWORD    Specify the SecureOn password
  -q, -s, --quiet, --silent  No output
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version

Report bugs to <https://github.com/mario-campos/wol>.
```

In its simplest form, simply pass a MAC-48 address in the typical colon-separated format (xx:xx:xx:xx:xx:xx) as an argument. Without specifying an interface, this command will broadcast the Wake-on-LAN magic packet across all supported interfaces:

```shell
wol de:ad:be:ef:ca:fe
```
You can limit the activity to a single network interface by specifying it with the `-i`/`--interface` flags:

```shell
wol --interface eth0 de:ad:be:ef:ca:fe
```

You can also supply a SecureOn password, with the `-p`/`--password` flags, for Wake-on-LAN hosts that support it. But beware that the password must be 6 characters or less and it is transmitted in cleartext.

```shell
wol --password foobar de:ad:be:ef:ca:fe
```

### CAVEATS

The Wake-On-LAN protocol requires the target computer to support WOL. This is typically a hardware requirement.

### LICENSE

_wol_ is licensed with [GNU GPL 3.0][1]

[1]:http://www.gnu.org/licenses/gpl-3.0.txt
