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

   ```bash
   git clone https://github.com/ibrahim3999/IPC.git
   
   ```bash
   make
   
   ```bash
   ./stnc -s PORT -p(for performance test)-q (for quiet)
   
   ```bash
   ./stnc -c IP PORT -p <type> <param> (type-> ipv4,ipv6,mmap,... param-> udp,tcpor dgram/stream or file name)
   
   
   
   
