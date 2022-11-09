//
//  privutil.c
//  tinyhttp
//
//  Created by Tim K. on 09.11.22.
//  Copyright © 2022 Tim K. All rights reserved.
//

#include <stdlib.h>
#include <string.h>
#include "privutil.h"

#undef tf_struct_alloc

void tf_log(const char* fn, const tf_index_t line, const char* fnn,
            const char* fmt, ...) {
#ifndef DEBUG
#error "Too unstable to be undebugged"
    return;
#endif
    
    fprintf(stderr, "[DEBUG/%s/%u/%s] ", fn, line, fnn);
    
    va_list vl;
    va_start(vl, fmt);
    vfprintf(stderr, fmt, vl);
    va_end(vl);
    
    fprintf(stderr, "%c", '\n');
}

tf_data_ref tf_struct_alloc(const tf_index_t size) {
    tf_data_ref result = malloc(size);
    bzero(result, size);
    
    return result;
}
