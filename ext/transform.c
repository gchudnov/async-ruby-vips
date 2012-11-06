#include "transform.h"
#include "callback.h"
#include "details.h"
#include <pthread.h>


ID av_t_id_load;
ID av_t_id_save;
ID av_t_id_scale_x;
ID av_t_id_scale_y;

/* Get the size of a file */
static void av_get_image_file_size(const char* file_path, long long* file_size)
{
    long long sz = 0;
    if(file_path)
    {
        struct stat sb;
        memset(&sb, 0, sizeof(stat));
        int err = stat(file_path, &sb);
        if(!err)
        {
            sz = sb.st_size;
        }
    }

    *file_size = sz;
}

/* Shrinks image using the provided width and height ratios */
static VipsImage* av_internal_shrink_image(VipsImage* image, double width_ratio, double height_ratio, char** err_str)
{
    VipsImage* t = im_open("temp-image", "p");
    if(!t)
    {
        *err_str = copy_vips_error();
        return NULL;
    }

    if(im_shrink(image, t, width_ratio, height_ratio))
    {
        *err_str = copy_vips_error();
        im_close(t);
        return NULL;
    }

    return t;
}

/* Thread function for image processing */
void* av_build_image_thread_func(void* data)
{
    transform_data_t* tdata = (transform_data_t*)data;
    if(!tdata)
        return NULL;

    //fprintf(stderr, "+THREAD_FUNC [%s] [%s]\n", tdata->src_path, tdata->dst_path);

    VipsImage* image;
    image = vips_image_new_mode(tdata->src_path, "r");
    if(image)
    {
        //fprintf(stderr, "(1)\n");

        // SHRINK
        if(tdata->dst_path && tdata->target_width && tdata->target_height)
        {
            double width_ratio = 0, height_ratio = 0;

            av_get_scale_transform2(image->Xsize, image->Ysize, tdata->target_width, tdata->target_height, image_scale_fit_no_scale_up, &width_ratio, &height_ratio);

            //fprintf(stderr, "WIDTH_RATIO: %f, HEIGHT_RATIO: %f\n", width_ratio, height_ratio);
            //fprintf(stderr, "(2)\n");

            VipsImage* t = av_internal_shrink_image(image, width_ratio, height_ratio, &tdata->err_str);
            if(t)
            {
                //fprintf(stderr, "(3)\n");

                VipsImage* out = vips_image_new_mode(tdata->dst_path, "w");
                if(out)
                {
                    //fprintf(stderr, "(4)\n");

                    if(im_copy(t, out))
                    {
                        tdata->err_str = copy_vips_error();
                    }

                    tdata->final_width = out->Xsize;
                    tdata->final_height = out->Ysize;

                    im_close(out);

                    // DST INFO
                    av_get_image_file_size(tdata->dst_path, &tdata->final_size);

                    //fprintf(stderr, "(5)\n");
                }
                else
                {
                    tdata->err_str = copy_vips_error();
                }

                im_close(t);
            }
        }
        else
        {
            // SRC INFO
            tdata->final_width = image->Xsize;
            tdata->final_height = image->Ysize;
            av_get_image_file_size(tdata->src_path, &tdata->final_size);
        }

        im_close(image);
    }

    //fprintf(stderr, "-THREAD_FUNC ERR_STR:%s\n", tdata->err_str ? tdata->err_str : "<NONE>");

    av_add_to_event_qeueue(tdata);

    return NULL;
}

/* Image transformation */
static VALUE av_transform(int argc, VALUE *argv, VALUE self)
{
    rb_need_block();
    VALUE proc = rb_block_proc();

    VALUE rest;
    rb_scan_args(argc, argv, "*", &rest);

    VALUE params = rb_ary_pop(rest);
    VALUE tmp = rb_check_hash_type(params);
    if (NIL_P(tmp))
        rb_raise(rb_eArgError, "No transformation parameters specified");

    // Parse options
    VALUE load = rb_hash_aref(params, ID2SYM(av_t_id_load));
    if(NIL_P(load))
        rb_raise(rb_eArgError, "No image source specified: transform(:load => 'image.jpg')");

    VALUE save = rb_hash_aref(params, ID2SYM(av_t_id_save));
    if(NIL_P(save))
        rb_raise(rb_eArgError, "No image destination specified: transform(:save => 'output.jpg')");

    VALUE scale_x = rb_hash_aref(params, ID2SYM(av_t_id_scale_x));
    VALUE scale_y = rb_hash_aref(params, ID2SYM(av_t_id_scale_y));

    if(NIL_P(scale_x) && NIL_P(scale_y))
        rb_raise(rb_eArgError, "No scale width or height specified of the source image: transform(:scale_x => 800, :scale_y => 600)");

    if(NIL_P(scale_y))
        scale_y = scale_x;

    if(NIL_P(scale_x))
        scale_x = scale_y;

    transform_data_t* tdata = av_make_transform_data(StringValuePtr(load), StringValuePtr(save));
    tdata->target_width = NUM2INT(scale_x);
    tdata->target_height = NUM2INT(scale_y);
    tdata->proc = proc;

    rb_gc_register_address(&tdata->proc);

    av_enqueue_task(av_build_image_thread_func, tdata);

    return self;
}

/* Initialize transformation pipeline */
void init_async_vips_transform(void)
{
    rb_define_singleton_method(mAsyncVips, "transform", av_transform, -1);

    av_t_id_load = rb_intern("load");
    av_t_id_save = rb_intern("save");
    av_t_id_scale_x = rb_intern("scale_x");
    av_t_id_scale_y = rb_intern("scale_y");
}
