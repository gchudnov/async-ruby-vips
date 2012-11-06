#include "transform_data.h"

/* Creates a new transform_data structure */
transform_data_t* av_make_transform_data_src(const char* src_path)
{
    transform_data_t* tdata = (transform_data_t*)malloc(sizeof(transform_data_t));
    if(tdata)
    {
        memset(tdata, 0, sizeof(transform_data_t));

        // Source
        tdata->src_path = (char*)malloc(strlen(src_path) + 1);
        if(tdata->src_path)
        {
            strcpy(tdata->src_path, src_path);
        }
    }

    return tdata;
}

/* Creates a new transform_data structure */
transform_data_t* av_make_transform_data(const char* src_path, const char* dst_path)
{
    transform_data_t* tdata = (transform_data_t*)malloc(sizeof(transform_data_t));
    if(tdata)
    {
        memset(tdata, 0, sizeof(transform_data_t));

        // Source
        tdata->src_path = (char*)malloc(strlen(src_path) + 1);
        if(tdata->src_path)
        {
            strcpy(tdata->src_path, src_path);
        }

        // Destination
        tdata->dst_path = (char*)malloc(strlen(dst_path) + 1);
        if(tdata->dst_path)
        {
            strcpy(tdata->dst_path, dst_path);
        }
    }

    return tdata;
}

/* Frees transform data */
void av_free_transform_data(transform_data_t* tdata)
{
    if(!tdata)
        return;

    if(tdata->src_path)
    {
        free(tdata->src_path);
        tdata->src_path = NULL;
    }

    if(tdata->dst_path)
    {
        free(tdata->dst_path);
        tdata->dst_path = NULL;
    }

    if(tdata->err_str)
    {
        free(tdata->err_str);
        tdata->err_str = NULL;
    }

    free(tdata);
}

