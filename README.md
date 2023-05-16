# Inter-process communication Project

This project is a network communication application that enables communication between a client and a server using different protocols and transport mechanisms.

## Features

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
   
   
