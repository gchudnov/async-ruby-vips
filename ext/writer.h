#ifndef ASYNC_VIPS_WRITER_H
#define ASYNC_VIPS_WRITER_H

typedef void* (*transform_func_t)(void*);
void av_enqueue_task(transform_func_t func, void* data);

#endif
