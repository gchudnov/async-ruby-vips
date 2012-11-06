#ifndef ASYNC_VIPS_TRANSFORM_H
#define ASYNC_VIPS_TRANSFORM_H

#include "async_vips.h"
#include "transform_data.h"

void init_async_vips_transform();

void* av_build_image_thread_func(void* data);

#endif
