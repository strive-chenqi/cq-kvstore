#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H


// 目前，实施情况：

// 仅适用于 pthreads，但 API 有意不透明以允许其他实现（例如 Windows）。
// 创建线程池时启动所有线程。
// 保留一个任务用于发出队列已满的信号。
// 在销毁时停止并加入所有工作线程。
// 可能的增强
// API 包含额外的未使用的“标志”参数，这些参数允许一些额外的选项：

// 惰性创建线程（简单）
// 自动减少线程数（硬）
// 无限队列大小（中等）
// 销毁时终止工作线程（困难，危险）
// 支持 Windows API（中等）
// 减少锁定争用（中/困难）

#ifdef __cplusplus
extern "C" {
#endif


#define MAX_THREADS 64
#define MAX_QUEUE 65536

typedef struct threadpool_s threadpool_t;

typedef enum {
    threadpool_invalid        = -1,
    threadpool_lock_failure   = -2,
    threadpool_queue_full     = -3,
    threadpool_shutdown       = -4,
    threadpool_thread_failure = -5
} threadpool_error_t;

typedef enum {
    threadpool_graceful       = 1
} threadpool_destroy_flags_t;


threadpool_t *threadpool_create(int thread_count, int queue_size, int flags);

int threadpool_add(threadpool_t *pool, void (*routine)(void *),
                   void *arg, int flags);


int threadpool_destroy(threadpool_t *pool, int flags);

#ifdef __cplusplus
}
#endif


#endif







