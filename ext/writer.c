#include "writer.h"
#include "transform_data.h"
#include <memory.h>
#include <stdlib.h>
#include <pthread.h>

pthread_t av_writer_thread;
pthread_once_t qt_once = PTHREAD_ONCE_INIT;

typedef struct _av_write_task_t
{
    transform_func_t func;
    void* data;
    struct _av_write_task_t* next;
} av_write_task_t;


/* Queue of tasks for processing. */
pthread_mutex_t av_writer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t av_writer_cond = PTHREAD_COND_INITIALIZER;
av_write_task_t* av_writer_queue = NULL;


/* Dumps write task */
static void dump_task(const char* title, av_write_task_t* wtask)
{
    if(wtask)
    {
        fprintf(stderr, "{%s}, WRITE_TASK: [%p], next: [%p]\n", title, wtask, wtask->next);
    }
    else
    {
        fprintf(stderr, "{%s}, WRITE_TASK: [NULL]\n", title);
    }
}

/* Push new task to front of the queue */
static void av_write_task_queue_push(av_write_task_t* wtask)
{
    //dump_task("av_write_task_queue_push(1)", wtask);
    wtask->next = av_writer_queue;
    av_writer_queue = wtask;
    //dump_task("av_write_task_queue_push(2)", av_writer_queue);
}

/* Pop next task from the queue; Returns NULL, when the queue is empty */
static av_write_task_t* av_write_task_queue_pop(void)
{
    av_write_task_t* wtask = av_writer_queue;
    if(wtask)
    {
        //dump_task("av_write_task_queue_pop(1)", wtask);
        av_writer_queue = wtask->next;
        //dump_task("av_write_task_queue_pop(2)", av_writer_queue);
    }

    return wtask;
}


/* Task processing queue */
static void* av_writer_thread_func(void* data)
{
    int is_running = 1;
    av_write_task_t* wtask = NULL;

    while(is_running)
    {
        pthread_mutex_lock(&av_writer_mutex);
        while (is_running && (wtask = av_write_task_queue_pop()) == NULL)
        {
            pthread_cond_wait(&av_writer_cond, &av_writer_mutex);
        }
        pthread_mutex_unlock(&av_writer_mutex);

        if(wtask)
        {
            if(wtask->func)
            {
                wtask->func(wtask->data);
            }

            free(wtask);
        }
    }

    return NULL;
}


/* Initialize the writer thread. The thread will wait for tasks to process */
static void av_init_writer_thread(void)
{
    pthread_create(&av_writer_thread, NULL, av_writer_thread_func, NULL);
    pthread_detach(av_writer_thread);
}

/* asynchronously invoke the func with the provided data */
void av_enqueue_task(transform_func_t func, void* data)
{
  pthread_once(&qt_once, av_init_writer_thread);

  pthread_mutex_lock(&av_writer_mutex);

  av_write_task_t* wtask = (av_write_task_t*)malloc(sizeof(av_write_task_t));
  memset(wtask, 0, sizeof(av_write_task_t));
  wtask->func = func;
  wtask->data = data;
  av_write_task_queue_push(wtask);

  pthread_mutex_unlock(&av_writer_mutex);
  pthread_cond_signal(&av_writer_cond);
}
