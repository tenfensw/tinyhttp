//
//  main.c
//  tinyhttp
//
//  Created by Tim K. on 09.11.22.
//  Copyright © 2022 Tim K. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "intarray.h"

int main(const int argc, const char** argv) {
    (void)(argc);
    (void)(argv);
    
    tf_int_array_ref array = tf_int_array_init(50, true);
    
    // fill it in
    for (int i = 0; i < 100; i++)
        tf_int_array_push(array, i + ((int)(clock() % 10)));
    
    // set the specific item to 1 to detect the change easily
    tf_int_array_set_at(array, 1, 1);
    
    // now dump the array
    tf_int_array_dump(stdout, array);
    tf_int_array_release(array);
    return 0;
}
