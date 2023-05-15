
#ifndef CLIENT_H
#define CLIENT_H

void chat_client_TCP_IPv4(char *ip_addr, int port);
void client_TCP_IPv4(char *ip_addr, int PORT) ;
unsigned int checksum(const char *buf, size_t len);
void client_TCP_IPv6(char *ip_addr, int PORT);
void client_UDP_IPv4(char *ip_addr, int PORT);
void client_UDP_IPv6(char *ip_addr, int PORT);
void client_UDS_stream();
void client_mmap() ;
void client_pipe();

#endif
