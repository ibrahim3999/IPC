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
   
   
## Research:
   ``` The examination focused on transferring a 100MB file and comparing the transfer      times using different protocols and mechanisms. The software under examination included TCP (IPv4 and IPv6), Unix Domain Sockets (UDS) with datagram and stream sockets, pipe, and Memory-Mapped Files (MMAP).

    TCP IPv4: When transferring the 100MB file using TCP over IPv4, the observed transfer time was approximately 663.01 ms. TCP (Transmission Control Protocol) is a reliable, connection-oriented protocol widely used for data transfer over IP networks.

    TCP IPv6: Next, the examination moved to TCP over IPv6, resulting in a longer transfer time of around 1591.97 ms. IPv6 (Internet Protocol version 6) is the newer version of the IP protocol, designed to address the limitations of IPv4 and accommodate the growing number of devices connected to the internet.

    UDS Datagram: The examination then explored UDS with datagram sockets. The transfer time observed for sending the 100MB file using UDS datagram sockets was approximately 346.78 ms. UDS provides communication channels between processes running on the same host, typically offering faster communication compared to TCP/IP.

    UDS Stream: Moving on to UDS with stream sockets, the transfer time improved to about 146.65 ms. UDS stream sockets provide reliable, connection-oriented communication, which can enhance performance when compared to datagram sockets.

    Pipe: The examination included the use of a pipe for inter-process communication. The observed transfer time for the 100MB file using a pipe was approximately 139.99 ms. Pipes are a mechanism for communication between processes running on the same host, known for their efficiency.

    MMAP: Finally, the examination investigated the use of Memory-Mapped Files (MMAP). This mechanism achieved the fastest transfer time of only 0.67 ms. MMAP allows direct access to files in memory, eliminating the need for data copying and resulting in faster data transfer.

In summary, based on the examination of the software and protocols, the research findings demonstrate that Memory-Mapped Files (MMAP) provided the fastest transfer time for the 100MB file, followed by UDS (stream and datagram), pipe, and TCP (IPv4 and IPv6). This deductive analysis helps to understand the performance characteristics of each protocol and mechanism, aiding in the selection of the most suitable option based on specific requirements. ```