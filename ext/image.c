#include "image.h"
#include <sys/types.h>
#include <sys/stat.h>


VALUE cImage;

/* Get the source parameter */
static VALUE av_image_src(VALUE self)
{
    return rb_iv_get(self, "@src");
}

/* Get the destination parameter */
static VALUE av_image_dst(VALUE self)
{
    return rb_iv_get(self, "@dst");
}

/* Get the error parameter */
static VALUE av_image_error(VALUE self)
{
    return rb_iv_get(self, "@error");
}

/* Get the image width */
static VALUE av_image_width(VALUE self)
{
    return rb_iv_get(self, "@width");
}

/* Get the image height */
static VALUE av_image_height(VALUE self)
{
    return rb_iv_get(self, "@height");
}

/* Get the size of the dst-image file */
static VALUE av_image_size(VALUE self)
{
    return rb_iv_get(self, "@size");
}

/* Initialize the object, called before the callback is invoked */
void av_image_init(VALUE self, const char* src_path, const char* dst_path, const char* err_str, int width, int height)
{
    rb_iv_set(self, "@src", (src_path ? rb_str_new2(src_path) : Qnil));
    rb_iv_set(self, "@dst", (dst_path ? rb_str_new2(dst_path) : Qnil));
    rb_iv_set(self, "@error", (err_str ? rb_str_new2(err_str) : Qnil));
    rb_iv_set(self, "@width", INT2FIX(width));
    rb_iv_set(self, "@height", INT2FIX(height));
    
    const char* file_path = (dst_path ? dst_path : src_path);
    long long file_size = 0;
    if(file_path)
    {
        struct stat sb;
        memset(&sb, 0, sizeof(stat));
        stat(file_path, &sb);
        file_size = sb.st_size;
    }
    rb_iv_set(self, "@size", INT2FIX(file_size));
}


void init_async_vips_image()
{
    cImage = rb_define_class_under(mAsyncVips, "Image", rb_cObject);

    rb_define_method(cImage, "src", av_image_src, 0);
    rb_define_method(cImage, "dst", av_image_dst, 0);
    rb_define_method(cImage, "error", av_image_error, 0);
    rb_define_method(cImage, "width", av_image_width, 0);
    rb_define_method(cImage, "height", av_image_height, 0);
    rb_define_method(cImage, "size", av_image_size, 0);
}
