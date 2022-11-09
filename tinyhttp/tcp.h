//
//  tcp.h
//  tinyhttp
//
//  Created by Tim K. on 09.11.22.
//  Copyright © 2022 Tim K. All rights reserved.
//

#pragma once

#include "types.h"

#define TF_TCP_IP_LISTEN_ANY NULL

tf_tcp_ref tf_tcp_init(const char* ipv4a,
                       const tf_port_t port,
                       const tf_index_t max_clients);

bool tf_tcp_listen(tf_tcp_ref tcp, const tf_tcp_callback_t cb);

void tf_tcp_release(tf_tcp_ref tcp);
