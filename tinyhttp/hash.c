//
//  hash.c
//  tinyhttp
//
//  Created by Tim K. on 09.11.22.
//  Copyright © 2022 Tim K. All rights reserved.
//

#include <stdlib.h>
#include <string.h>
#include "privutil.h"
#include "hash.h"

//
// private
//

/// internally-used linked list-behaving hash item
typedef struct tf_item_s* tf_item_ref;
struct tf_item_s {
    // item label, required value
    char* key;
    
    // must be not NULL
    void* value;
    // optional, points to a deallocator method for the specified
    // value
    tf_deallocator_t autorelease;
    
    // if NULL, this means that we are at the end of the hash
    tf_item_ref next;
};

struct tf_hash_s {
    tf_item_ref first;
    // this one is used for iterating over hashes
    tf_item_ref iterator;
};

tf_item_ref tf_item_init(tf_item_ref previous, const char* key,
                         tf_data_ref data, const tf_deallocator_t dlc) {
    tf_item_ref result = tf_struct_alloc(tf_item_s);
    
    // fill in the fields
    result->key = strdup(key);
    result->value = data;
    result->autorelease = dlc;
    
    // organize previous item to point to this new one if specified
    if (previous)
        previous->next = result;
    
    return result;
}

tf_item_ref tf_item_find_by_key(tf_item_ref base, const char* key,
                                tf_item_ref* lastp) {
    if (!base)
        return NULL;
    
    TF_PTR_SET(lastp, base);
    tf_item_ref current = base;
    
    while (current) {
        if (strcmp(current->key, key) == 0)
            break;
        
        TF_PTR_SET(lastp, current);
        current = current->next;
    }
    
    return current;
}

void tf_item_set_in_tree(tf_item_ref* basep, const char* key, tf_data_ref vv,
                         const tf_deallocator_t dlc) {
    if (!basep || !key || !vv)
        return; // all these need to be valid
    
    if (*basep) {
        tf_item_ref last = NULL;
        tf_item_ref wrk = tf_item_find_by_key(*basep, key, &last);
        
        if (wrk) {
            // existing item, need to clean its contents first
            if (wrk->autorelease)
                wrk->autorelease(wrk->value);
            
            wrk->value = vv;
            wrk->autorelease = dlc;
        } else
            wrk = tf_item_init(last, key, vv, dlc);
    } else
        TF_PTR_SET(basep, tf_item_init(NULL, key, vv, dlc))
}

//
// public
//

tf_hash_ref tf_hash_init_empty() {
    tf_hash_ref hash = tf_struct_alloc(tf_hash_s);
    return hash;
}

bool tf_hash_set(tf_hash_ref hash, const char* key, tf_data_ref value,
                 const tf_deallocator_t autorelease) {
    if (!hash || !key || strlen(key) < 1 || !value)
        return false; // invalid params
    
    tf_item_set_in_tree(&hash->first, key, value, autorelease);
    return true;
}

bool tf_hash_has(const tf_hash_ref hash, const char* key) {
    if (tf_hash_get(hash, key))
        return true;
    
    return false;
}

tf_data_ref tf_hash_get(const tf_hash_ref hash, const char* key) {
    if (!hash || !hash->first || !key)
        return NULL; // empty hash or invalid params
    
    tf_item_ref wrk = tf_item_find_by_key(hash->first, key, NULL);
    return (wrk ? wrk->value : NULL);
}

void tf_hash_iter_reset(tf_hash_ref hash) {
    if (hash)
        hash->iterator = NULL;
}

bool tf_hash_iter_next(tf_hash_ref hash) {
    // TODO: rewrite and optimize
    
    if (hash) {
        if (hash->iterator)
            hash->iterator = hash->iterator->next;
        else
            hash->iterator = hash->first;
        
        if (!hash->iterator)
            return false;
        
        return true;
    }
    
    return false;
}

const char* tf_hash_iter_get_key(tf_hash_ref hash) {
    return ((hash && hash->iterator) ? hash->iterator->key : NULL);
}

tf_data_ref tf_hash_iter_get_value(tf_hash_ref hash) {
    return ((hash && hash->iterator) ? hash->iterator->value : NULL);
}

void tf_hash_release(tf_hash_ref hash) {
    if (!hash)
        return;
    
    tf_hash_iter_reset(hash);
    
    // deallocate all items
    tf_item_ref current = hash->first;
    while (current) {
        tf_item_ref next = current->next;
        
        if (current->autorelease)
            current->autorelease(current->value);
        
        free(current->key);
        free(current);
        
        current = next;
    }
    
    free(hash);
}



