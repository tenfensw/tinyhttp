//
//  hash.h
//  tinyhttp
//
//  Created by Tim K. on 09.11.22.
//  Copyright © 2022 Tim K. All rights reserved.
//

#pragma once

#include "types.h"

tf_hash_ref tf_hash_init_empty(void);

/// adds or modifies value stored in the hash under the specified key
bool tf_hash_set(tf_hash_ref hash, const char* key, tf_data_ref value,
                 const tf_deallocator_t autorelease);

/// checks if the specified labeled value exists in the hash
bool tf_hash_has(const tf_hash_ref hash, const char* key);
/// retreives the value with the specified value from the hash
tf_data_ref tf_hash_get(const tf_hash_ref hash, const char* key);

//
// hash iteration
//
void tf_hash_iter_reset(tf_hash_ref hash);
bool tf_hash_iter_next(tf_hash_ref hash);
const char* tf_hash_iter_get_key(tf_hash_ref hash);
tf_data_ref tf_hash_iter_get_value(tf_hash_ref hash);

/// destroys the specified hash instance
void tf_hash_release(tf_hash_ref hash);
