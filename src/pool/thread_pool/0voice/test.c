

//g++ taskqueue_test.cc -o taskqueue_test -lgtest -lgtest_main -lpthread

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



time_t GetTick() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()
        ).count();
}

std::atomic<int64_t> g_count{0};
void JustTask(void *ctx) {
    ++g_count;
}

constexpr int64_t n = 1000000;

void producer(thrdpool_t *pool) {
    for(int64_t i=0; i < n; ++i) {
        thrdpool_post(pool, JustTask, NULL);
    }
}

void test_thrdpool(int nproducer, int nconsumer) {
    auto pool = thrdpool_create(nconsumer);
    for (int i=0; i<nproducer; ++i) {
        std::thread(&producer, pool).detach();
    }

    time_t t1 = GetTick();
    // wait for all producer done
    while (g_count.load() != n*nproducer) {
        usleep(100000);
    }

    time_t t2 = GetTick();

    std::cout << t2 << " " << t1 << " " << "used:" << t2-t1 << " exec per sec:"
        << (double)g_count.load()*1000 / (t2-t1) << std::endl;

    thrdpool_terminate(pool);
    thrdpool_waitdone(pool);
}

/**
 * author: mark 
 * QQ: 2548898954
 * shell: g++ -Wl,-rpath=./ thrdpool_test.cc -o thrdpool_test -I./ -L./ -lthrdpool -lpthread
 */
//task_test
task_queue_t *
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
        task = (task_t*)__pop_task(queue);
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
        task = (task_t*)__pop_task(queue);
        free(task);
        i++;
    }
    ASSERT_TRUE(i==10);
}




int main() {
    // test_thrdpool(1, 8);
    test_thrdpool(4, 4);
    return 0;
}
