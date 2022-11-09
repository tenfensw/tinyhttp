//
//  intarray.h
//  tinyhttp
//
//  Created by Tim K. on 09.11.22.
//  Copyright © 2022 Tim K. All rights reserved.
//

#pragma once

#include "types.h"

tf_int_array_ref tf_int_array_init(const tf_index_t capacity,
                                   const bool autoextend);

bool tf_int_array_push(tf_int_array_ref array, const int value);
bool tf_int_array_push_replacing_zeroes(tf_int_array_ref array,
                                        const int value);
int tf_int_array_pop(tf_int_array_ref array);

int tf_int_array_get_at(const tf_int_array_ref array, const tf_index_t index,
                        const int defv);
bool tf_int_array_set_at(tf_int_array_ref array, const tf_index_t index,
                         const int value);

int tf_int_array_get_max(const tf_int_array_ref array);
int tf_int_array_get_min(const tf_int_array_ref array);

double tf_int_array_get_average_precise(const tf_int_array_ref array);
int tf_int_array_get_average(const tf_int_array_ref array);

tf_index_t tf_int_array_get_count(const tf_int_array_ref array);
tf_index_t tf_int_array_get_capacity(const tf_int_array_ref array);

int* tf_int_array_get_raw(tf_int_array_ref array);
void tf_int_array_dump(tf_data_ref fdesc, const tf_int_array_ref array);

void tf_int_array_release(tf_int_array_ref array);
