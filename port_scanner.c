//############################
//#	Andrew Kulpa             #
//#	CS410                    #
//#	Programming Assignment 3 #
//############################
// Description: Just like any port scanner, this program will scan specified
// ports of an address you give it. There is a default value set for the
// ipaddress that is to be scanned and the max port to scan up to.
//
// Usage: ./port_scanner IPADDRESS MAXPORT --only_open
//
// IPADDRESS = the ip address of the host to scan (can be a domain name or an IP)
// MAXPORT = the highest port to scan up to (highest possible is 65535)
// --only_open = If there is even a 3rd argument, closed ports are ignored.
//
// Source: http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#getaddrinfo
// Much of the networking information was gleaned from the the referenced hyperlink.
// While nothing explicitly in there describes how to scan ports, it did describe
// how to do a basic connection in C. This simply needs to be looped across many ports
// and the connection status checked. If there is an error, a proper ACK was
// received. Otherwise, it is blocked, closed, etc.
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h> // close()
// most systems can handle 65535. Some can only handle less than half of this.
// Reduced down to 30,000 for compatibility reasons.
#define port_max 30000 // Previous value: 65535 



int ports[port_max]; // ports are unsigned INTs from 0 to 65535
int port_error[port_max];
int port_specified_max = 100;
char ipaddress[100] = "127.0.0.1";
int only_open_ports = 0;

// Print out all ports.
void print_ports_statuses(){
    for(int i = 0; i < port_specified_max; i++){
        if(ports[i] < 0){
            if(!only_open_ports){
                if(port_error[i] == 60){
                    printf("%-10i %s\n", i+1, "Blocked"); // no reply from host.
                } else if (port_error[i] == 61){
                    printf("%-10i %s\n", i+1, "Closed"); // reply: "Denied".
                } else{
                    printf("%-10i %s\n", i+1, strerror(port_error[i])); // ???
                }
            }
        }
        else{
            printf("%-10i Open\n", i+1); // its open!
        }
    }
}

// Attempt to establish connection to a port. Use errno (threadsafe) to 
// return an error if there was one. Put port status into global ports[] array.
void *scan_port(void *port_ptr){
    // setup port references (int and string usage)
    char *port = malloc(10);
    int port_int = *((int *)port_ptr);
    sprintf(port, "%i", port_int);

    // initialize structs to prep socket address structures.
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints); // zeroes out 'hints'.
    hints.ai_family = AF_UNSPEC;     // Use either IPv4 or IPv6 (unspecified)
    hints.ai_socktype = SOCK_STREAM; // preferred socket type = TCP stream
    hints.ai_flags = AI_PASSIVE;

    // getaddrinfo() man page description:
    // returns one or more addrinfo structures, each of which 
    // contains an Internet address that can be specified in a 
    // call to bind(2) or connect(2)
    getaddrinfo(ipaddress, port, &hints, &res);
    // socket() man page description: 
    // creates an endpoint for communication and returns a file
    // descriptor that refers to that endpoint.
    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(sockfd < 0 ){
        perror("Error encountered when creating socket endpoint.");
        exit(1);
    }
    // connect() man page description:
    // connects the socket referred to by the file
    // descriptor sockfd to the address specified by addr.
    int port_status = connect(sockfd, res->ai_addr, res->ai_addrlen);
    if(port_status<0){
        port_error[port_int-1] = errno;// errno is thread safe, as this is thread-specific due to POSIX
    }
    close(sockfd);
    ports[port_int-1] = port_status;
    return 0;
}

int main(int argc, const char* argv[]){
    // parse arguments based upon how many there are.
    // no args: Scan first 100 ports of the localhost. Print all port statuses.
    if(argc < 2){ // 1 arg: Scan first 100 ports of ARGV[1]. Print all port statuses.
        printf("[-] No IP address has been specified. Set to localhost: 127.0.0.1\n");
    }else if ( argc < 3){ // 2 args: Scan first ARGV[2] ports of ARGV[2]. Print all port statuses.
        printf("[!] No upper bound set to port. Default: 100\n");
        strcpy(ipaddress, argv[1]);
    } else { // 3+ args: Scan first ARGV[2] ports of ARGV[2]. Print only open ports.
        strcpy(ipaddress, argv[1]);
        port_specified_max = atoi(argv[2]);
        if(port_specified_max > port_max){
            port_specified_max = port_max;
            printf("[!] Limited to 30,000 ports. This has been reduced.\n");
        } else if(port_specified_max > 1000){
            printf("[!] Scanning more than 1000 ports. This may take some time.\n");
        }
        if (argc > 3){
            printf("[*] 3rd argument provided.. ignoring blocked/refused ports.\n");
            only_open_ports = 1;
        }
    }

    printf("[*] IP address set to: %s\n", ipaddress);
    pthread_t thread_id[port_specified_max];
    int port_num[port_specified_max];
    printf("[*] Scanning ports 0 through %i...\n", port_specified_max);
    // Initialize a thread per port. These will each scan ports.
    for(int i = 0; i < port_specified_max; i++){
        port_num[i] = i+1;
        pthread_create(&thread_id[i], NULL, scan_port, &port_num[i]);
    }
    // Join each created thread.
    for(int j = 0; j < port_specified_max; j++){
        pthread_join(thread_id[j], NULL);
    }
    print_ports_statuses();
    return 0;
}