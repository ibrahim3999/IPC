# Inter-process communication Project

This project is a network communication application that enables communication between a client and a server using different protocols and transport mechanisms.

## Features
- chat between server and client use poll  file descriptor(IPC)
- TCP/IP communication using both IPv4 and IPv6
- UDP/IP communication using both IPv4 and IPv6
- Unix Domain Socket (UDS) communication using datagram and stream sockets
- Memory-mapped file communication
- Inter-process communication using pipes

## Requirements

- Linux operating system
- GCC compiler
- Socket programming knowledge

## Getting Started

1) ` git clone https://github.com/ibrahim3999/IPC.git `
   
2) ` make ` 
   
3) ` ./stnc -s PORT -p(for performance test)-q (for quiet) `
  
4) `  ./stnc -c IP PORT -p <type> <param> (type-> ipv4,ipv6,mmap,... param-> udp,tcpor dgram/stream or file name) `
   
 ## Example
 * start server onport 1234 ` ./stnc -s 1234 -p -q `
 * Connect to the TCP server(IPv4 at 127.0.0.1) ` ./stnc -c 127.0.0.1 1234 -p ipv4 tcp `
   
   
## Research:
` The examination focused on transferring a 100MB file and comparing the transfer times using different protocols and       mechanisms. The software under examination included TCP (IPv4 and IPv6),
Unix Domain Sockets (UDS) with datagram and stream sockets,
pipe, and Memory-Mapped Files (MMAP).
Finally  MMAP is  the examination investigated the use of Memory-Mapped Files (MMAP). 
This mechanism achieved the fastest transfer time of only 0.67 ms. 
MMAP allows direct access to files in memory, eliminating the need for data copying and resulting in faster data transfer.
In summary, based on the examination of the software and protocols, the research findings demonstrate that Memory-Mapped Files (MMAP) provided the fastest transfer time for the 100MB file, followed by UDS (stream and datagram), pipe, and TCP (IPv4 and IPv6).
This deductive analysis helps to understand the performance characteristics of each protocol 
and mechanism, aiding in the selection of the most suitable option based on specific requirements. `
