#include <pthread.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdlib.h>
#include "thrd_pool.h"
#include "spinlock.h"

/**
 * author: mark 
 * QQ: 2548898954
 * shell: gcc thrd_pool.c -c -fPIC
 * shell: gcc -shared thrd_pool.o -o libthrd_pool.so -I./ -L./ -lpthread
 * usage: include thrd_pool.h & link libthrd_pool.so
 */

typedef struct spinlock spinlock_t;

//任务对象
typedef struct task_s {
    void *next;
    handler_pt func;
    void *arg;
} task_t;

//任务队列
typedef struct task_queue_s {
    void *head;
    void **tail; 
    int block;
    spinlock_t lock;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} task_queue_t;

//线程池
struct thrdpool_s {
    task_queue_t *task_queue;
    atomic_int quit;
    int thrd_count;
    pthread_t *threads; //线程池线程数组
};

// 对称
// 资源的创建    -->回滚式编程写法-->成功才做内容
// 业务逻辑    -->防御式编程写法
//static本文件可见
static task_queue_t *
__taskqueue_create() {
    int ret;
    task_queue_t *queue = (task_queue_t *)malloc(sizeof(task_queue_t));
    if (queue) { //成功
        ret = pthread_mutex_init(&queue->mutex, NULL);
        if (ret == 0) { //成功
            ret = pthread_cond_init(&queue->cond, NULL);
            if (ret == 0) { //成功
                spinlock_init(&queue->lock);  //自旋锁
                queue->head = NULL;
                queue->tail = &queue->head;
                queue->block = 1;
                return queue;
            }
            pthread_mutex_destroy(&queue->mutex); //失败回滚
        }
        free(queue); //失败回滚
    }
    return NULL; //失败返回NULL
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
    // 这样做的好处：不限定任务类型，只要该任务的结构起始内存是一个用于链接下一个节点的指针
    void **link = (void**)task;
    *link = NULL;  //*link就是next指针

    spinlock_lock(&queue->lock);
    *queue->tail /* 等价于 queue->tail->next */ = link;
    queue->tail = link;
    spinlock_unlock(&queue->lock);
    pthread_cond_signal(&queue->cond);
}

static inline void * 
__pop_task(task_queue_t *queue) {
	//防御式编程
    spinlock_lock(&queue->lock);
    if (queue->head == NULL) { //如果失败
        spinlock_unlock(&queue->lock);
        return NULL;
    }
	//之后都是成功
    task_t *task;
    task = queue->head;

    void **link = (void**)task;
    queue->head = *link;

    if (queue->head == NULL) {
        queue->tail = &queue->head;
    }
    spinlock_unlock(&queue->lock);
    return task;
}


static inline void * 
__get_task(task_queue_t *queue) {
    task_t *task;
    // 虚假唤醒
    while ((task = __pop_task(queue)) == NULL) {
        pthread_mutex_lock(&queue->mutex); 
		//队列为空->休眠(cond_wait)
        if (queue->block == 0) {
            pthread_mutex_unlock(&queue->mutex);
            return NULL;
        }
		
        // 1. 先 unlock(&mtx)
        // 2. 在 cond 休眠
        // --- __add_task 时唤醒
        // 3. 在 cond 唤醒
        // 4. 加上 lock(&mtx);
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
    spinlock_destroy(&queue->lock);
    pthread_cond_destroy(&queue->cond);
    pthread_mutex_destroy(&queue->mutex);
    free(queue);
}

//线程池工作函数
static void *
__thrdpool_worker(void *arg) {
    thrdpool_t *pool = (thrdpool_t*) arg;
    task_t *task;
    void *ctx;

    while (atomic_load(&pool->quit) == 0) {
        task = (task_t*)__get_task(pool->task_queue);
        if (!task) break;
        handler_pt func = task->func;
        ctx = task->arg;
        free(task);
        func(ctx);
    }
    
    return NULL;
}

static void 
__threads_terminate(thrdpool_t * pool) {
    atomic_store(&pool->quit, 1);
    __nonblock(pool->task_queue);
    int i;
    for (i=0; i<pool->thrd_count; i++) {
        pthread_join(pool->threads[i], NULL);
    }
}

static int 
__threads_create(thrdpool_t *pool, size_t thrd_count) {
    pthread_attr_t attr;
	int ret;

    ret = pthread_attr_init(&attr);

    if (ret == 0) {
        pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * thrd_count);
        if (pool->threads) {
            int i = 0;
            for (; i < thrd_count; i++) {
                if (pthread_create(&pool->threads[i], &attr, __thrdpool_worker, pool) != 0) {
                    break;
                }
            }
            pool->thrd_count = i;
            pthread_attr_destroy(&attr);
            if (i == thrd_count)
                return 0;
            __threads_terminate(pool);
            free(pool->threads);
        }
        ret = -1;
    }
    return ret; 
}

void
thrdpool_terminate(thrdpool_t * pool) {
    atomic_store(&pool->quit, 1);
    __nonblock(pool->task_queue);
}

thrdpool_t *
thrdpool_create(int thrd_count) {
    thrdpool_t *pool;

    pool = (thrdpool_t*)malloc(sizeof(*pool));
    if (pool) {
        task_queue_t *queue = __taskqueue_create();
        if (queue) {
            pool->task_queue = queue;
            atomic_init(&pool->quit, 0);
            if (__threads_create(pool, thrd_count) == 0)
                return pool;
            __taskqueue_destroy(queue);
        }
        free(pool);
    }
    return NULL;
}

int
thrdpool_post(thrdpool_t *pool, handler_pt func, void *arg) {
    if (atomic_load(&pool->quit) == 1) 
        return -1;
    task_t *task = (task_t*) malloc(sizeof(task_t));
    if (!task) return -1;
    task->func = func;
    task->arg = arg;
    __add_task(pool->task_queue, task);
    return 0;
}

void
thrdpool_waitdone(thrdpool_t *pool) {
    int i;
    for (i=0; i<pool->thrd_count; i++) {
        pthread_join(pool->threads[i], NULL);
    }
    __taskqueue_destroy(pool->task_queue);
    free(pool->threads);
    free(pool);
}
