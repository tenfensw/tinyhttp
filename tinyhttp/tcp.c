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
#include <errno.h>
#include <unistd.h>
#include "privutil.h"
#include "intarray.h"
#include "tcp.h"

//
// private
//

struct tf_tcp_s {
    // main TCP socket to listen on
    tf_socket_t main_socket;
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
    \
    if (server->main_socket >= 0) \
        close(server->main_socket); \
    \
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
    if (server->main_socket < 0)
        TF_TCP_INIT_DESTROY_PROGRESS("Socket creation failed, see errno for more info")
    
    TF_LOG("server = <%p>, main_socket = %d", server, server->main_socket);
        
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

bool tf_tcp_listen(tf_tcp_ref tcp, const tf_tcp_callback_t cb,
                   tf_data_ref cbmeta) {
    if (!tcp || !cb)
        return false; // a valid callback is required
    
    if (listen(tcp->main_socket, tcp->max_connections) < 0) {
        perror(strerror(errno));
        TF_LOG("Listen failed, returning false");
        
        return false;
    }
    
    while (true) {
        // clean all the FDs and client sockets
        FD_ZERO(&tcp->client_descs);
        
        // fill in fdset with valid sockets, starting with the main socket
        FD_SET(tcp->main_socket, &tcp->client_descs);
        
        // determine client connection sockets' statuses
        tf_socket_t recent_conn = tcp->main_socket;
        
        for (tf_index_t index = 0; index < tf_int_array_get_count(tcp->client_sockets);
             index++) {
            tf_socket_t desc = tf_int_array_get_at(tcp->client_sockets,
                                                   index, -1);
            
            // if socket is valid, then track it
            if (desc > 0)
                FD_SET(desc, &tcp->client_descs);
            
            recent_conn = tf_keep_greater(recent_conn, desc);
        }
        
        // wait for activity/new connections via select
        if (select(recent_conn + 1, &tcp->client_descs, NULL, NULL, NULL) < 0) {
            if (errno == EINTR)
                TF_LOG("User interrupt received, continuing nevertheless");
            else {
                perror(strerror(errno));
                TF_LOG("Select failed, closing server connection and exiting...");
                
                // TODO: maybe make a function that will close all client connections
                // too, as dirty cleanup might be unacceptable for non-standalone TCP
                // server apps
                return false;
            }
        }
        
        if (FD_ISSET(tcp->main_socket, &tcp->client_descs)) {
            // incoming connection
            
            // TODO: maybe cb should have sth like a struct for storing IP address
            // meta which will be passed on every occasion?
            struct sockaddr claddr;
            socklen_t clalen = 0;
            
            tf_socket_t newcl = accept(tcp->main_socket,
                                       &claddr, &clalen);
            if (newcl >= 0) {
                // accepted, call the callback for proper backend-side handling
                cb(tcp, TF_TCP_CONNECTION_NEW, newcl, cbmeta);
                // then save the socket for further use
                tf_int_array_push_replacing_zeroes(tcp->client_sockets,
                                                   newcl);
            } else
                TF_LOG("warning! connection accept failed, errno = %s, will continue",
                       strerror(errno));
        } else {
            // existing connection update (probably)
            for (tf_index_t iter = 0; iter < tf_int_array_get_count(tcp->client_sockets);
                 iter++) {
                tf_socket_t current = tf_int_array_get_at(tcp->client_sockets, iter, 0);
                
                if (FD_ISSET(current, &tcp->client_descs)) {
                    // read incoming data
                    // TODO
                }
            }
        }
    }
    
    TF_LOG("Listen intact, waiting for connections...");
}

void tf_tcp_release(tf_tcp_ref tcp) {
    if (!tcp)
        return;
    
    // close all client connections if active
    for (tf_index_t index = 0; index < tf_int_array_get_count(tcp->client_sockets);
         index++) {
        tf_socket_t sock = tf_int_array_get_at(tcp->client_sockets,
                                               index, -1);
        
        if (sock > 0)
            close(sock);
    }
    
    // cleanup with all the client-related stuff
    tf_int_array_release(tcp->client_sockets);
    FD_ZERO(&tcp->client_descs);
    
    // close main socket too
    close(tcp->main_socket);
    free(tcp);
}
