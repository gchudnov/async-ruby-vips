#ifndef ASYNC_VIPS_TRANSFORM_DATA_H
#define ASYNC_VIPS_TRANSFORM_DATA_H

#include "ruby.h"

typedef struct _transform_data_t
{
    char* src_path;
    char* dst_path;

    int target_width;
    int target_height;

    VALUE proc;
    char* err_str;

    struct _transform_data_t* next;

} transform_data_t;


transform_data_t* av_make_transform_data(const char* src_path, const char* dst_path);
void av_free_transform_data(transform_data_t* tdata);


#endif
