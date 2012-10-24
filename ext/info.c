#include "info.h"
#include "image.h"
#include <sys/types.h>
#include <sys/stat.h>

ID id_load;

/* Returns width & height of an image */
static void av_get_image_width_height(const char* file_path, int* width, int* height)
{
    VipsImage* image;
    image = vips_image_new_mode(file_path, "r");
    if(image)
    {
        *width = image->Xsize;
        *height =  image->Ysize;

        im_close(image);
    }
}

/* Returns image information: width, height, file size */
static VALUE av_info(int argc, VALUE *argv, VALUE self)
{
    VALUE rest;
    rb_scan_args(argc, argv, "*", &rest);

    VALUE params = rb_ary_pop(rest);
    VALUE tmp = rb_check_hash_type(params);
    if (NIL_P(tmp))
        rb_raise(rb_eArgError, "No parameters specified");

    // Parse options
    VALUE load = rb_hash_aref(params, ID2SYM(id_load));
    if(NIL_P(load))
        rb_raise(rb_eArgError, "No image source specified: info(:load => 'image.jpg')");

    // Extract image information (Width, Height)
    int width = 0, height = 0;
    const char* file_path = StringValuePtr(load);
    av_get_image_width_height(file_path, &width, &height);

    VALUE img = rb_class_new_instance(0, NULL, cImage);
    av_image_init(img, file_path, NULL, NULL, width, height);

    return img;
}

void init_async_vips_info()
{
    rb_define_singleton_method(mAsyncVips, "info", av_info, -1);
    id_load = rb_intern("load");
}
