//
//  main.c
//  tinyhttp
//
//  Created by Tim K. on 09.11.22.
//  Copyright © 2022 Tim K. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include "tcp.h"

void tinyhttp_listen(tf_tcp_ref server,
                     tf_tcp_connection_type_t ctype,
                     tf_data_ref const rdt,
                     const tf_index_t rdl,
                     tf_socket_t lsock,
                     tf_data_ref meta) {
    (void)(meta);
    (void)(server);
    
    switch (ctype) {
        case TF_TCP_CONNECTION_NEW: {
            printf("New connection from socket %d\n", lsock);
            break;
        }
        case TF_TCP_CONNECTION_CONTINUE: {
            printf("Raw input (%u bytes):\n%s\n", rdl, rdt);
            
            const char* msg = "HTTP/1.0 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nServer: tinyhttp\r\nContent-Length: 5\r\n\r\nhello";
            
            tf_socket_send_data(lsock, (const tf_data_ref)msg, (tf_index_t)strlen(msg));
        }
        case TF_TCP_CONNECTION_CLOSE: {
            printf("Goodbye from %d\n", lsock);
            break;
        }
    }
}

int main(const int argc, const char** argv) {
    (void)(argc);
    (void)(argv);
    
    tf_tcp_ref tcp = tf_tcp_init(TF_TCP_IP_LISTEN_ANY, 5643, 3);
    
    if (!tcp || !tf_tcp_listen(tcp, tinyhttp_listen, NULL)) {
        perror("Failed to init, exiting...");
        return 1;
    }
    
    tf_tcp_release(tcp);
    return 0;
}
