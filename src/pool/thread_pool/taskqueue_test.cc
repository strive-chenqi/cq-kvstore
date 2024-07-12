#include <pthread.h>
#include "spinlock.h"
#include <gtest/gtest.h>

/**
 * author: mark 
 * QQ: 2548898954
 * shell: g++ taskqueue_test.cc -o taskqueue_test -lgtest -lgtest_main -lpthread
 */

typedef void (*handler_pt)(void *);
typedef struct spinlock spinlock_t;

typedef struct task_s {
    void *next;
    handler_pt func;
    void *arg;
} task_t;

typedef struct task_queue_s {
    void *head;
    void **tail;
    int block;
    spinlock_t lock;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} task_queue_t;


static task_queue_t *
__taskqueue_create() {
    task_queue_t *queue = (task_queue_t*)malloc(sizeof(task_queue_t));
    if (!queue) return NULL;

    int ret;
    ret = pthread_mutex_init(&queue->mutex, NULL);
    if (ret == 0) {
        ret = pthread_cond_init(&queue->cond, NULL);
        if (ret == 0) {
            spinlock_init(&queue->lock);
            queue->head = NULL;
            queue->tail = &queue->head;
            queue->block = 0;
            return queue;
        }
        pthread_cond_destroy(&queue->cond);
    }
    pthread_mutex_destroy(&queue->mutex);
    return NULL;
}

static void
__nonblock(task_queue_t *queue) {
    pthread_mutex_lock(&queue->mutex);
    queue->block = 0;
    pthread_mutex_unlock(&queue->mutex);
    pthread_cond_broadcast(&queue->cond);
}

static inline void 
__add_task(task_queue_t *queue, void *task) {
    void **link = (void **)task;
    *link = NULL;
    spinlock_lock(&queue->lock);
    *queue->tail = link;
    queue->tail = link;
    spinlock_unlock(&queue->lock);
    pthread_cond_signal(&queue->cond);
}

static inline task_t * 
__pop_task(task_queue_t *queue) {
    spinlock_lock(&queue->lock);
    if (queue->head == NULL) {
        spinlock_unlock(&queue->lock);
        return NULL;
    }
    task_t *task;
    task = (task_t *)queue->head;
    queue->head = task->next;
    if (queue->head == NULL) {
        queue->tail = &queue->head;
    }
    spinlock_unlock(&queue->lock);
    return task;
}

static inline task_t * 
__get_task(task_queue_t *queue) {
    task_t *task;
    while ((task = __pop_task(queue)) == NULL) {
        pthread_mutex_lock(&queue->mutex);
        if (queue->block == 0) {
            pthread_mutex_unlock(&queue->mutex);
            break;
        }
        pthread_cond_wait(&queue->cond, &queue->mutex);
        pthread_mutex_unlock(&queue->mutex);
    }
    return task;
}

static void
__taskqueue_destroy(task_queue_t *queue) {
    task_t *task;
    while ((task = __pop_task(queue))) {
        free(task);
    }
    pthread_cond_destroy(&queue->cond);
    pthread_mutex_destroy(&queue->mutex);
    spinlock_destroy(&queue->lock);
    free(queue);
}

TEST(task_queue, normal) {
    int i;
    task_t *task;

    task_queue_t * queue = __taskqueue_create();
    for (i=0; i<10; i++) {
        task_t *task = (task_t*)malloc(sizeof(*task));
        __add_task(queue, task);
    }

    i = 0;
    while (queue->head) {
        task = __pop_task(queue);
        free(task);
        i++;
    }
    // 从有到无
    ASSERT_TRUE(i==10);

    // 从无到有
    for (i=0; i<10; i++) {
        task_t *task = (task_t*)malloc(sizeof(*task));
        __add_task(queue, task);
    }

    i = 0;
    while (queue->head) {
        task = __pop_task(queue);
        free(task);
        i++;
    }
    ASSERT_TRUE(i==10);
}
