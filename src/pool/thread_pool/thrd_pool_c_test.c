#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "thrd_pool_c.h"

void test_task(void *arg)
{
    int id = *(int *)arg;

    printf("task[%d] enter\r\n", id);
    sleep(3);

    free(arg); // 释放分配的内存
    return;
}

int32_t main(int32_t argc, char *argv[])
{
    threadpool_t *threadpool;
    int id;

    //flags暂时未用到
    threadpool = threadpool_create(10, 20, 0);
    if (NULL == threadpool) {
        printf("threadpool_create failed\r\n");
        return -1;
    }

    for (id = 1; id <= 20;)
    {
        int *arg = (int *)malloc(sizeof(int)); // 为每个任务分配一个唯一的id
        if (arg == NULL) {
            break;
        }
        *arg = id;
        if (0 != threadpool_add(threadpool, test_task, arg, 0))
        {
            printf("threadpool_add failed\r\n");
            sleep(1);
            continue;
        }

        id++;
    }

    sleep(5);

    threadpool_destroy(threadpool,2);
    return 0;
}






