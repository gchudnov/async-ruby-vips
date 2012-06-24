#ifndef ASYNC_VIPS_IMAGE_H
#define ASYNC_VIPS_IMAGE_H

extern int image_scale_none;
extern int image_scale_stretch;
extern int image_scale_fit;
extern int image_scale_no_scale_up;
extern int image_scale_detect_proportions;
extern int image_scale_fit_no_scale_up;


void av_get_scale_transform2(int original_width, int original_height, int desired_width, int desired_height, int scale_mode, double* factor_x, double* factor_y);


#endif
