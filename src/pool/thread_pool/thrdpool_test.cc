#include "thrd_pool.h"
#include <bits/types/time_t.h>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <atomic>
#include <thread>
#include <iostream>
#include <unistd.h>

/**
 * author: mark 
 * QQ: 2548898954
 * shell: g++ -Wl,-rpath=./ thrdpool_test.cc -o thrdpool_test -I./ -L./ -lthrdpool -lpthread
 */

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

int main() {
    // test_thrdpool(1, 8);
    test_thrdpool(4, 4);
    return 0;
}
