#include "callback.h"
#include "async_vips.h"
#include "image.h"
#include "transform_data.h"
#include <pthread.h>


typedef struct _tdata_wait_t
{
    transform_data_t* tdata;
    int abort;
} tdata_wait_t;


/* Queue of callbacks; each one invoked on a ruby thread */
pthread_mutex_t av_proc_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t av_proc_cond = PTHREAD_COND_INITIALIZER;
transform_data_t* av_proc_queue = NULL;

/* Push new callback to front of the queue */
static void av_proc_queue_push(transform_data_t* tdata)
{
    tdata->next = av_proc_queue;
    av_proc_queue = tdata;
}

/* Pop next callback from the queue; Returns NULL, when the queue is empty */
static transform_data_t* av_proc_queue_pop(void)
{
    transform_data_t* tdata = av_proc_queue;
    if(tdata)
    {
        av_proc_queue = tdata->next;
    }

    return tdata;
}

/* Callback executed by Ruby Thread */
static VALUE av_handle_proc(void *d)
{
    transform_data_t* tdata = (transform_data_t*)d;

    // Invoke callback with task argument
    VALUE proc = (VALUE)tdata->proc;
    VALUE img = rb_class_new_instance(0, NULL, cImage);

    av_image_init(img, tdata);
    rb_funcall2(proc, rb_intern("call"), 1, &img);
    rb_gc_unregister_address(&tdata->proc);

    av_free_transform_data(tdata);

    return Qnil;
}


/* Wait until we have some callbacks to process  */
static VALUE av_wait_for_tdata(void* w)
{
    tdata_wait_t* waiter = (tdata_wait_t*)w;

    pthread_mutex_lock(&av_proc_mutex);
    while (!waiter->abort && (waiter->tdata = av_proc_queue_pop()) == NULL)
    {
        pthread_cond_wait(&av_proc_cond, &av_proc_mutex);
    }
    pthread_mutex_unlock(&av_proc_mutex);

    return Qnil;
}

/* Stop waiting for callbacks */
static void av_stop_waiting_for_tdata(void* w)
{
    tdata_wait_t* waiter = (tdata_wait_t*)w;

    pthread_mutex_lock(&av_proc_mutex);
    waiter->abort = 1;
    pthread_mutex_unlock(&av_proc_mutex);
    pthread_cond_signal(&av_proc_cond);
}


/* ruby event thread, waiting for processed transformations (invokes callbacks) */
static VALUE av_event_thread(void *unused)
{
    tdata_wait_t waiter = { .tdata = NULL, .abort = 0 };
    while (!waiter.abort)
    {
        rb_thread_blocking_region(av_wait_for_tdata, &waiter, av_stop_waiting_for_tdata, &waiter);
        if (waiter.tdata)
        {
            rb_thread_create(av_handle_proc, waiter.tdata);
        }
    }

    return Qnil;
}

/* Initialize Ruby Event Thread for invokation of user-provider callbacks */
static void av_create_event_thread(void)
{
    rb_thread_create(av_event_thread, NULL);
}

/* Add the transform data to the event queue */
void av_add_to_event_queue(transform_data_t* tdata)
{
    pthread_mutex_lock(&av_proc_mutex);
    av_proc_queue_push(tdata);
    pthread_mutex_unlock(&av_proc_mutex);
    pthread_cond_signal(&av_proc_cond);
}

void init_async_vips_event_thread(void)
{
    av_create_event_thread();
}
