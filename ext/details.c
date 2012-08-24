#include "details.h"

int image_scale_none = 0x00;
int image_scale_stretch = 0x01;
int image_scale_fit = 0x02;
int image_scale_no_scale_up = 0x04;
int image_scale_detect_proportions = 0x08; // detect if the picture width & height are need to be swapped
int image_scale_fit_no_scale_up = 0x06; // = (fit | no scale_up)

/* Get the factors for scale transformation */
void av_get_scale_transform2(int original_width, int original_height, int desired_width, int desired_height, int scale_mode, double* factor_x, double* factor_y)
{
    if(scale_mode & image_scale_none) // NONE
    {
        *factor_x = *factor_y = 1.0;
    }
    else if(scale_mode & image_scale_stretch) // STRETCH
    {
        *factor_x = (double) (original_width) / (double) (desired_width);
        *factor_y = (double) (original_height) / (double) (desired_height);
    }
    else if(scale_mode & image_scale_fit) // FIT
    {
        if(scale_mode & image_scale_detect_proportions)
        {
            double p1 = (double) (desired_width) / (double) (desired_height);
            double p2 = (double) (original_width) / (double) (original_height);

            if((p1 > 1.0 && p2 < 1.0) || (p1 < 1.0 && p2 > 1.0))
            {
                int temp = desired_width;
                desired_width = desired_height;
                desired_height = temp;
            }
        }

        double k1 = (double) (original_width) / (double) (desired_width);
        double k2 = (double) (original_height) / (double) (desired_height);

        double k = (k1 > k2 ? k1 : k2);

        if(scale_mode & image_scale_no_scale_up)
        {
            if(k < 1.0)
                k = 1.0; // do not scale up small images at all.
        }

        *factor_x = *factor_y = k;
    }
}
