//
//  types.h
//  tinyhttp
//
//  Created by Tim K. on 09.11.22.
//  Copyright © 2022 Tim K. All rights reserved.
//

#pragma once

#include <stdbool.h>
#include <stdint.h>

/// indexing type
typedef uint32_t tf_index_t;
/// void* typedef for readability purposes
typedef void* tf_data_ref;

/// deallocator type
typedef void (*tf_deallocator_t)(void*);

/// int* C array wrapper type
typedef struct tf_int_array_s* tf_int_array_ref;

/// Ruby string hash-like type
typedef struct tf_hash_s* tf_hash_ref;

/// port type
typedef uint16_t tf_port_t;
/// TCP server control object
typedef struct tf_tcp_s* tf_tcp_ref;

/// TCP server listen connection type
typedef enum {
    TF_TCP_CONNECTION_NEW,
    TF_TCP_CONNECTION_CONTINUE,
    TF_TCP_CONNECTION_CLOSE
} tf_tcp_connection_type_t;

/// TCP server listen handler callback
typedef void (*tf_tcp_callback_t)(tf_tcp_connection_type_t,
                                  int);
