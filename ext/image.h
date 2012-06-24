#ifndef ASYNC_VIPS_IMAGE_H
#define ASYNC_VIPS_IMAGE_H

#include "async_vips.h"

extern VALUE cImage;

void av_image_init(VALUE self, const char* src_path, const char* dst_path, const char* err_str);

void init_async_vips_image();

#endif
