//
//  tcp.c
//  tinyhttp
//
//  Created by Tim K. on 09.11.22.
//  Copyright © 2022 Tim K. All rights reserved.
//

#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include "privutil.h"
#include "intarray.h"
#include "tcp.h"

//
// private
//

struct tf_tcp_s {
    // main TCP socket to listen on
    int main_socket;
    // IPv4 address to listen on
    struct sockaddr_in main_address;
    
    // client sockets
    tf_int_array_ref client_sockets;
    // client socket descriptors
    fd_set client_descs;
    
    // max client count
    tf_index_t max_clients;
    // max connections count
    tf_index_t max_connections;
};

//
// public
//

bool tf_make_ipv4_sockaddr(const char* address, const tf_port_t port,
                           struct sockaddr_in* resultp) {
    struct sockaddr_in result;
    bzero(&result, sizeof(struct sockaddr_in));
    
    if (address) {
        // read it into result
        if (inet_aton(address, &result.sin_addr) == 0)
            return false; // invalid IP
    } else
        result.sin_addr.s_addr = INADDR_ANY;
    
    // import port info
    result.sin_family = AF_INET;
    result.sin_port = htons(port);
    
    TF_PTR_SET(resultp, result);
    return true;
}

#define TF_TCP_INIT_DESTROY_PROGRESS(msg) \
{ \
    TF_LOG(msg); \
    \
    tf_int_array_release(server->client_sockets); \
    FD_ZERO(&server->client_descs); \
    free(server); \
    return NULL; \
}

tf_tcp_ref tf_tcp_init(const char* ipv4a,
                       const tf_port_t port,
                       const tf_index_t max_clients) {
    tf_tcp_ref server = tf_struct_alloc(tf_tcp_s);
    
    // convert IPv4 string into sockaddr_in
    if (!tf_make_ipv4_sockaddr(ipv4a, port, &server->main_address))
        TF_TCP_INIT_DESTROY_PROGRESS("Invalid IPv4 address format")
    
    // TODO: make const
    server->max_clients = (max_clients >= 1 ? max_clients : 3);
    server->client_sockets = tf_int_array_init(server->max_clients, false);
    
    // initialize server socket
    server->main_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server->main_socket == 0)
        TF_TCP_INIT_DESTROY_PROGRESS("Socket creation failed, see errno for more info")
    
    // we need to point to this while setting the REUSEADDR flag
    bool truev = true;
    setsockopt(server->main_socket, SOL_SOCKET, SO_REUSEADDR, &truev, sizeof(truev));
    
    // now bind the main socket
    if (bind(server->main_socket, (struct sockaddr*)&server->main_address,
             sizeof(server->main_address)) < 0)
        TF_TCP_INIT_DESTROY_PROGRESS("Bind failed, see errno for more info")
        
    return server;
}

#undef TF_TCP_INIT_DESTROY_PROGRESS

void tf_tcp_release(tf_tcp_ref tcp);
