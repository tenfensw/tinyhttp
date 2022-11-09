//
//  intarray.c
//  tinyhttp
//
//  Created by Tim K. on 09.11.22.
//  Copyright © 2022 Tim K. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "privutil.h"
#include "intarray.h"

//
// private
//

struct tf_int_array_s {
    int* raw;
    
    tf_index_t count;
    tf_index_t capacity;
    
    // in this case with each new item or unknown index the array will
    // automatically extend its capacity and reallocate its memory
    bool autoextend;
};

bool tf_int_array_autoextend_if_necessary(tf_int_array_ref array) {
    if (!array)
        return false;
    
    TF_LOG("autoextend called");
    
    if ((array->count + 1) >= array->capacity) {
        if (!array->autoextend)
            return false; // too bad, this one is a fixed one
        
        // TODO: turn increase step into const
        array->capacity += 5;
        array->raw = realloc(array->raw, array->capacity * sizeof(int));
        
        TF_LOG("new capacity = %u, raw = %p", array->capacity, array->raw);
        
        // zero out all the garbage
        memset(array->raw + array->count, 0, array->capacity - array->count - 1);
    }
    
    return true;
}

#define TF_INT_ARRAY_MAXMIN_IMP(name, sign) \
int name (const tf_int_array_ref array) { \
    if (!array || array->count < 1) \
        return 0; \
    \
    int result = array->raw[0]; \
    \
    for (tf_index_t index = 1; index < array->count; index++) { \
        if (array->raw[index] sign result) \
            result = array->raw[index]; \
    } \
    \
    return result; \
}

//
// public
//

tf_int_array_ref tf_int_array_init(const tf_index_t capacity,
                                   const bool autoextend) {
    tf_int_array_ref array = tf_struct_alloc(tf_int_array_s);
    
    array->count = 0;
    array->capacity = capacity;
    
    array->raw = calloc(array->capacity, sizeof(int));
    array->autoextend = autoextend;
    
    TF_LOG("array created, raw = %p, capacity = %u, count = %u", array->raw,
                                                                 array->capacity,
                                                                 array->count);
    return array;
}

bool tf_int_array_push(tf_int_array_ref array, const int value) {
    if (tf_int_array_autoextend_if_necessary(array)) {
        TF_LOG("autoextend success");
        
        array->raw[array->count++] = value;
        return true;
    }
    
    return false;
}

bool tf_int_array_push_replacing_zeroes(tf_int_array_ref array,
                                        const int value) {
    if (!array)
        return false;
    
    for (tf_index_t index = 0; index < array->count; index++) {
        if (array->raw[index] <= 0) {
            array->raw[index] = value;
            return true;
        }
    }
    
    return tf_int_array_push(array, value);
}

int tf_int_array_pop(tf_int_array_ref array) {
    if (!array || array->count < 1)
        return 0;
    
    int last = array->raw[--array->count];
    return last;
}

int tf_int_array_get_at(const tf_int_array_ref array, const tf_index_t index,
                        const int defv) {
    if (!array || array->count <= index)
        return defv;
    
    return array->raw[index];
}

bool tf_int_array_set_at(tf_int_array_ref array, const tf_index_t index,
                         const int value) {
    if (!array || array->count <= index)
        return false;
    
    array->raw[index] = value;
    return true;
}

TF_INT_ARRAY_MAXMIN_IMP(tf_int_array_get_max, >)
TF_INT_ARRAY_MAXMIN_IMP(tf_int_array_get_min, <)

double tf_int_array_get_average_precise(const tf_int_array_ref array) {
    if (!array)
        return 0;
    
    double result = 0;
    
    // get the sum first
    for (tf_index_t i = 0; i < array->count; i++)
        result += array->raw[i];
    
    // now divide it by the full amount of items in the array
    result /= array->count;
    return result;
}

int tf_int_array_get_average(const tf_int_array_ref array) {
    return ((int)(tf_int_array_get_average_precise(array)));
}

tf_index_t tf_int_array_get_count(const tf_int_array_ref array) {
    return (array ? array->count : 0);
}

tf_index_t tf_int_array_get_capacity(const tf_int_array_ref array) {
    return (array ? array->capacity : 0);
}

int* tf_int_array_get_raw(tf_int_array_ref array) {
    return (array ? array->raw : NULL);
}

void tf_int_array_dump(tf_data_ref fdesc, const tf_int_array_ref array) {
    if (!fdesc)
        fdesc = (tf_data_ref)stdout;
    
    if (array) {
        for (tf_index_t index = 0; index < array->count; index++)
            fprintf((FILE*)fdesc, "[%u] %d\n", index, array->raw[index]);
    } else
        fprintf((FILE*)fdesc, "(null)\n");
}

void tf_int_array_release(tf_int_array_ref array) {
    if (!array)
        return;
    
    free(array->raw);
    free(array);
}
