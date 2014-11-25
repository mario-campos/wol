# wol [![Build Status](https://travis-ci.org/iamrekcah/wol.svg?branch=master)](https://travis-ci.org/iamrekcah/wol)


A Wake-On-LAN utility

### DESCRIPTION

_wol_ is a command-line tool for implementing the Wake-On-LAN protocol over a local-area network.

### INSTALL

```bash
./autogen.sh
./configure
make
sudo make install
```

### USAGE

```bash
Usage: wol [OPTION...] <mac address>
Wake-On-LAN packet sender

  -i, --interface=NAME       Specify the net interface to use
  -p, --password=PASSWORD    Specify the WOL password
  -q, -s, --quiet, --silent  No output
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version

Report bugs to <http://github.com/iamrekcah/wol>.
```

### CAVEATS

The Wake-On-LAN protocol requires the target computer to support WOL. This is typically a hardware requirement.

### LICENSE

_wol_ is licensed with [GNU GPL 3.0][1]

[1]:http://www.gnu.org/licenses/gpl-3.0.txt
