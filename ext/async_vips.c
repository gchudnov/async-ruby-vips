#include "async_vips.h"
#include "transform.h"
#include "writer.h"
#include "image.h"

VALUE mAsyncVips, eVipsError;

/* Initialize VIPS library */
void av_init()
{
    VALUE argv_0 = rb_gv_get("0");

    if(NIL_P(argv_0))
        vips_init("");
    else
        vips_init(RSTRING_PTR(argv_0));
}

/* Print internal debugging information from VIPS, including memory allocation information. */
static VALUE av_debug_info(VALUE self)
{
    vips_object_print_all();
    return Qnil;
}

/* Set the max number of operations to keep in cache. */
static VALUE av_set_cache(VALUE self, VALUE val)
{
    vips_cache_set_max(NUM2INT(val));
    return Qnil;
}

/* Get the version string */
static VALUE av_version_string()
{
    return rb_str_new2(vips_version_string());
}

/* Copy error from vips */
char* copy_vips_error(void)
{
    char* result = NULL;
    const char* err_str = vips_error_buffer();
    if(err_str && strlen(err_str) > 0)
    {
        result = malloc(strlen(err_str) + 1);
        strcpy(result, err_str);
        im_error_clear();
    }

    return result;
}

/* Initialize extension */
void Init_async_vips_ext()
{
    av_init();

    mAsyncVips = rb_define_module("AsyncVips");
    eVipsError = rb_define_class_under(mAsyncVips, "Error", rb_eStandardError);

    rb_define_singleton_method(mAsyncVips, "debug_info", av_debug_info, 0);
    rb_define_singleton_method(mAsyncVips, "set_cache", av_set_cache, 1);
    rb_define_const(mAsyncVips, "LIB_VERSION", av_version_string());

    //
    init_async_vips_transform();
    init_async_vips_image();
}
