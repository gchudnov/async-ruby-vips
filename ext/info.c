#include "info.h"
#include "transform.h"
#include "writer.h"
#include <sys/types.h>
#include <sys/stat.h>

ID av_i_id_load;

/* Returns image information: width, height, file size */
static VALUE av_info(int argc, VALUE *argv, VALUE self)
{
    rb_need_block();
    VALUE proc = rb_block_proc();

    VALUE rest;
    rb_scan_args(argc, argv, "*", &rest);

    VALUE params = rb_ary_pop(rest);
    VALUE tmp = rb_check_hash_type(params);
    if (NIL_P(tmp))
        rb_raise(rb_eArgError, "No info parameters specified");

    // Parse options
    VALUE load = rb_hash_aref(params, ID2SYM(av_i_id_load));
    if(NIL_P(load))
        rb_raise(rb_eArgError, "No image source specified: info(:load => 'image.jpg')");

    transform_data_t* tdata = av_make_transform_data_src(StringValuePtr(load));
    tdata->proc = proc;

    rb_gc_register_address(&tdata->proc);

    av_enqueue_task(av_build_image_thread_func, tdata);

    return self;
}

void init_async_vips_info()
{
    rb_define_singleton_method(mAsyncVips, "info", av_info, -1);
    av_i_id_load = rb_intern("load");
}
