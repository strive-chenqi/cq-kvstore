#include "thrd_pool_c.h"
#include <bits/types/time_t.h>
#include <unistd.h>
#include "spinlock.h"

#include <gtest/gtest.h>

typedef struct thrdpool_s thrdpool_t;
typedef struct task_queue_s task_queue_t;
// 任务执行的规范 ctx 上下文
typedef void (*handler_pt)(void * /* ctx */);

#ifdef __cplusplus
extern "C"
{
#endif


extern int 
__threads_create(thrdpool_t *pool, unsigned long thrd_count);

extern void 
__threads_terminate(thrdpool_t * pool);

extern void *
__thrdpool_worker(void *arg);

extern void
__taskqueue_destroy(task_queue_t *queue);

extern void* __get_task(task_queue_t *queue);

extern void * 
__pop_task(task_queue_t *queue);

extern void 
__add_task(task_queue_t *queue, void *task);

extern void
__nonblock(task_queue_t *queue);

extern task_queue_t *
__taskqueue_create();

// 对称处理      接口：创建线程池、销毁线程池（这里实现的是暂停线程池）、提交任务、线程池线程运行函数   ，waitdone是等线程运行完安全退出的一个测试
thrdpool_t *thrdpool_create(int thrd_count);

void thrdpool_terminate(thrdpool_t * pool);

int thrdpool_post(thrdpool_t *pool, handler_pt func, void *arg);

void thrdpool_waitdone(thrdpool_t *pool);

#ifdef __cplusplus
}
#endif

#endif