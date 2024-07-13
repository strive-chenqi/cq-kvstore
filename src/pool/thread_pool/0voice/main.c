
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "thrd_pool.h"

/**
 * author: mark 
 * QQ: 2548898954
 * shell: g++ taskqueue_test.cc -o taskqueue_test -lgtest -lgtest_main -lpthread
 */

int done = 0;

pthread_mutex_t lock;

void do_task(void *arg) {
    thrdpool_t *pool = (thrdpool_t*)arg;
    pthread_mutex_lock(&lock);
    done++;
    printf("doing %d task\n", done);
    pthread_mutex_unlock(&lock);
    if (done >= 1000) {
        thrdpool_terminate(pool);
    }
}

void test_thrdpool_basic() {
    int threads = 8;
    pthread_mutex_init(&lock, NULL);
    thrdpool_t *pool = thrdpool_create(threads);
    if (pool == NULL) {
        perror("thread pool create error!\n");
        exit(-1);
    }

    while (thrdpool_post(pool, &do_task, pool) == 0) {
    }

    thrdpool_waitdone(pool);
    pthread_mutex_destroy(&lock);
}

int main(int argc, char **argv) {
    test_thrdpool_basic();
    return 0;
}
