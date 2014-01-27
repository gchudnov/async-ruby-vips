#ifndef ASYNC_VIPS_CALLBACK_H
#define ASYNC_VIPS_CALLBACK_H

#include "transform_data.h"

void av_add_to_event_queue(transform_data_t* tdata);
void init_async_vips_event_thread(void);

#endif
