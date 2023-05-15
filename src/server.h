#ifndef SERVER_H
#define SERVER_H

void chat_server(int port);
void host_server(int,int);
char* generate_data(int size);
void chat_server_TCP_IPV4(int port);
unsigned int checksum_(const char *buf, size_t len);
void start_server_TCP_IPv4(int PORT);
void start_server_TCP_IPv6(int PORT);
void start_server_UDP_IPv4(int PORT);
void start_server_UDP_IPv6(int PORT);
void start_server_UDS_dgram();
void start_server_UDS_stream();
void start_server_mmap();
void start_server_pipe();
void host_server(int PORT ,int host_port);


#endif
