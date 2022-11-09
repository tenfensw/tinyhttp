//
//  tcp.h
//  tinyhttp
//
//  Created by Tim K. on 09.11.22.
//  Copyright © 2022 Tim K. All rights reserved.
//

#pragma once

#include "types.h"

//
// TCP server
//

#define TF_TCP_IP_LISTEN_ANY NULL

tf_tcp_ref tf_tcp_init(const char* ipv4a,
                       const tf_port_t port,
                       const tf_index_t max_clients);

bool tf_tcp_listen(tf_tcp_ref tcp, const tf_tcp_callback_t cb,
                   tf_data_ref cbmeta);

void tf_tcp_release(tf_tcp_ref tcp);

//
// socket ops
//

tf_data_ref tf_socket_read_data(tf_socket_t socket,
                                tf_index_t* sizep);
bool tf_socket_send_data(tf_socket_t socket,
                         const tf_data_ref data,
                         const tf_index_t dlen);

char* tf_socket_get_client_ip(tf_socket_t socket,
                              tf_port_t* portp);
