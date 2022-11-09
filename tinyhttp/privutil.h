//
//  privutil.h
//  tinyhttp
//
//  Created by Tim K. on 09.11.22.
//  Copyright © 2022 Tim K. All rights reserved.
//

#pragma once

#include <stdio.h>
#include <stdarg.h>
#include "types.h"

void tf_log(const char* fn, const tf_index_t line, const char* fnn,
            const char* fmt, ...);

#ifndef __FILE_NAME__
// only clang has this
#define __FILE_NAME__ __FILE__
#endif

#define TF_LOG(...) tf_log(__FILE_NAME__, __LINE__, __FUNCTION__, __VA_ARGS__)

tf_data_ref tf_struct_alloc(const tf_index_t size);

/// allocates NULL-ed memory for the specified struct type
#define tf_struct_alloc(type) tf_struct_alloc(sizeof(struct type))

#define TF_PTR_SET(ptr, value) \
{ \
    if (ptr) \
        (*ptr) = value; \
}
