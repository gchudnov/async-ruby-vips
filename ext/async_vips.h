#ifndef ASYNC_VIPS_H
#define ASYNC_VIPS_H

#include "ruby.h"
#include "vips/vips.h"

extern VALUE mAsyncVips, eVipsError;

char* copy_vips_error(void);

#endif
