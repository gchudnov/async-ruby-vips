#ifndef ASYNC_VIPS_IMAGE_H
#define ASYNC_VIPS_IMAGE_H

#include "async_vips.h"
#include "transform_data.h"

extern VALUE cImage;

void av_image_init(VALUE self, const transform_data_t* tdata);

void init_async_vips_image();

#endif
