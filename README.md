# Portscanner

* This program, as the name suggests, is a port scanner written in c.

* Much of the information I required to be able to build this came from a simple (and extremely long) guide on how to do network programming. This can be found at: 
  * http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#getaddrinfo

* To note, I used the guide to get a better overview of the usage of system calls via sys/types and sys/socket. Any supplemental material that was referenced was found in the man pages.

-------------------------------------------------------------------------------
## Usage:

* To use this program, you may supply any number of arguments in the order specfied above. 
* An IP address may be a domain name or an actual IP address. 
* The maximum port is by default set to 100, but it may be set as high as 65535. 
* Lastly, suppling more than two arguments will cause the program to only print out the open ports, rather than any ports that may be closed or blocked.

### Example:

`./port_scanner IPADDRESS MAXPORT --only_open`

* `IPADDRESS` = the ip address of the host to scan (can be a domain name or an IP)
* `MAXPORT` = the highest port to scan up to (highest possible is 65535)
* `--only_open` = If there is even a 3rd argument, closed ports are ignored.

