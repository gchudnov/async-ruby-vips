#include "transform.h"
#include "callback.h"
#include "details.h"
#include <pthread.h>
#include <exif-data.h>
#include <sys/stat.h>


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
        int err;
        struct stat st;
        memset(&st, 0, sizeof(st));
        err = stat(file_path, &st);
        if(!err)
        {
            sz = st.st_size;
        }
    }

    *file_size = sz;
}

/*
Get image orientation from EXIF

     0th Row      0th Column
     1  top          left side
     2  top          right side
     3  bottom       right side
     4  bottom       left side
     5  left side    top
     6  right side   top
     7  right side   bottom
     8  left side    bottom
*/
static int av_get_orientation(VipsImage* image)
{
    int orientation = 0;

    GType exif_type = vips_image_get_typeof(image, VIPS_META_EXIF_NAME);
    if(exif_type != 0)
    {
        unsigned char *data;
        size_t data_length;
        if(!vips_image_get_blob(image, VIPS_META_EXIF_NAME, (void *)&data, &data_length))
        {
            ExifData *ed;
            ed = exif_data_new_from_data(data, data_length);
            if(ed != NULL)
            {
                ExifByteOrder bo = exif_data_get_byte_order(ed);
                ExifEntry *ee = exif_data_get_entry(ed, EXIF_TAG_ORIENTATION);
                if(ee)
                {
                    orientation = exif_get_short(ee->data, bo);
                }

                exif_data_free(ed);
            }
        }
    }

    return orientation;
}

/* Sets the image orientation */
static void av_set_orientation(VipsImage* image, int orientation)
{
    GType exif_type = vips_image_get_typeof(image, VIPS_META_EXIF_NAME);
    if(exif_type != 0)
    {
        unsigned char *data;
        size_t data_length;
        if(!vips_image_get_blob(image, VIPS_META_EXIF_NAME, (void *)&data, &data_length))
        {
            ExifData *ed;
            ed = exif_data_new_from_data(data, data_length);
            if(ed != NULL)
            {
                ExifByteOrder bo = exif_data_get_byte_order(ed);
                ExifEntry *ee = exif_data_get_entry(ed, EXIF_TAG_ORIENTATION);
                if(ee)
                {
                    exif_set_short(ee->data, bo, (short)orientation);
                    vips_image_set_blob(image, VIPS_META_EXIF_NAME, NULL, (void *)ed, data_length);
                }

                exif_data_free(ed);
            }
        }
    }

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

/* Rotates the image */
static VipsImage* av_internal_rotate(VipsImage* in, VipsAngle angle, char** err_str)
{
    VipsImage* t = im_open("temp-image", "p");
    if(!t)
    {
        *err_str = copy_vips_error();
        return NULL;
    }

    if(vips_rot(in, &t, angle, NULL))
    {
        *err_str = copy_vips_error();
        return NULL;
    }

    return t;
}

/* Flips the image */
static VipsImage* av_internal_flip(VipsImage* in, VipsDirection direction, char** err_str)
{
    VipsImage* t = im_open("temp-image", "p");
    if(!t)
    {
        *err_str = copy_vips_error();
        return NULL;
    }

    if(vips_flip(in, &t, direction, NULL))
    {
        *err_str = copy_vips_error();
        return NULL;
    }

    return t;
}

/* Rotates & Flips the image */
static VipsImage* av_internal_rotate_flip(VipsImage* in, VipsAngle angle, VipsDirection direction, char** err_str)
{
    VipsImage* t;
    t = av_internal_rotate(in, angle, err_str);
    if(t)
    {
        VipsImage* f;
        f = av_internal_flip(t, direction, err_str);
        if(f)
        {
            im_close(t);
            t = f;
        }
        else
        {
            im_close(t);
            t = NULL;
        }
    }

    return t;
}

/*
 Transforms image orientation
   1) = NONE
   2) = FLIP_H
   3) = ROT_180
   4) = FLIP_V
   5) = TRANSPOSE
   6) = ROT_90
   7) = TRANSVERSE
   8) = ROT_270
 */
static VipsImage* av_internal_transform_image_orientation(VipsImage* image, int orientation, char** err_str)
{
    VipsImage* res = NULL;

    switch(orientation)
    {
        case 1:
            // no-op
            break;

        case 2:
            res = av_internal_flip(image, VIPS_DIRECTION_HORIZONTAL, err_str);
            break;

        case 3:
            res = av_internal_rotate(image, VIPS_ANGLE_180, err_str);
            break;

        case 4:
            res = av_internal_flip(image, VIPS_DIRECTION_VERTICAL, err_str);
            break;

        case 5:
            res = av_internal_rotate_flip(image, VIPS_ANGLE_90, VIPS_DIRECTION_HORIZONTAL, err_str);
            break;

        case 6:
            res = av_internal_rotate(image, VIPS_ANGLE_90, err_str);
            break;

        case 7:
            res = av_internal_rotate_flip(image, VIPS_ANGLE_270, VIPS_DIRECTION_HORIZONTAL, err_str);
            break;

        case 8:
            res = av_internal_rotate(image, VIPS_ANGLE_270, err_str);
            break;

        default:
            // no-op
            break;
    }

    if(res)
    {
        av_set_orientation(res, 1);
    }

    return res;
}


/* Thread function for image processing */
void* av_build_image_thread_func(void* data)
{
    transform_data_t* tdata = (transform_data_t*)data;
    if(!tdata)
        return NULL;

    VipsImage* image;
    image = vips_image_new_mode(tdata->src_path, "r");
    if(image)
    {
        int is_transform_orientation = 1;

        // ORIENTATION
        int orientation = av_get_orientation(image);
        //fprintf(stderr, "orientation: %d\n", orientation);

        // SHRINK
        if(tdata->dst_path && tdata->target_width && tdata->target_height)
        {
            double width_ratio = 0, height_ratio = 0;

            av_get_scale_transform2(image->Xsize, image->Ysize,
                                    tdata->target_width, tdata->target_height,
                                    image_scale_fit_no_scale_up,
                                    &width_ratio, &height_ratio);

            VipsImage* t;
            t = av_internal_shrink_image(image, width_ratio, height_ratio, &tdata->err_str);
            if(t)
            {
                if(is_transform_orientation && orientation > 1)
                {
                    // Need to transform an image so that orientation becomes 1
                    VipsImage* tran;
                    tran = av_internal_transform_image_orientation(t, orientation, &tdata->err_str);
                    if(tran)
                    {
                        im_close(t);
                        t = tran;
                    }
                }

                VipsImage* out;
                out = vips_image_new_mode(tdata->dst_path, "w");
                if(out)
                {
                    if(im_copy(t, out))
                    {
                        tdata->err_str = copy_vips_error();
                    }

                    tdata->final_width = out->Xsize;
                    tdata->final_height = out->Ysize;

                    im_close(out);

                    av_get_image_file_size(tdata->dst_path, &tdata->final_size);
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
            tdata->final_width = image->Xsize;
            tdata->final_height = image->Ysize;
            av_get_image_file_size(tdata->src_path, &tdata->final_size);
        }

        im_close(image);
    }

    av_add_to_event_queue(tdata);

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
