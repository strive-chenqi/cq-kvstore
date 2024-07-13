#include <iostream>
#include <unistd.h>
#include <vector>
#include <chrono>

#include "thread_pool_c++11.h"

int main()
{

    ThreadPool pool(4); //线程池
    std::vector< std::future<int> > results;    //线程执行结果
    // 批量执行线程任务
    for(int i = 0; i < 8; ++i) {
        results.emplace_back(   // 保存线程执行结果到results
            pool.enqueue([i] {  // 添加一个新的工作任务到线程池
                std::cout << "hello " << i << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(3));
                // std::cout << "world " << i << std::endl;
                // std::this_thread::sleep_for(std::chrono::seconds(3));
                return i*i;
            })
        );
    }
    sleep(5);

    // 打印线程执行结果
    for(auto && result: results)
        std::cout << "result:" << result.get() << std::endl;
    std::cout << std::endl;
    
    return 0;

}