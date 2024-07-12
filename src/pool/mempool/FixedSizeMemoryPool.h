#ifndef _FIXED_SIZE_MEMPOOL_H_
#define _FIXED_SIZE_MEMPOOL_H_

#include "mempool.h"
#include <memory>
#include <mutex>


template<int objectslotsize, int numofobjectslots = 20>  //objectslotsize; 对象槽大小    numofobjectslots对象槽的个数
class FixSizeMemoryPool : public MemoryPool{
private:

    //空闲对象槽结构体
    typedef struct free_objectslot_s {
        unsigned char data[objectslotsize];
        free_objectslot_s *next;
    }free_objectslot_t;

    //内存块结构体
    typedef struct memory_block_s
    {
        struct memory_block_s *next; //指向下一个memory_block 对象槽链表
        free_objectslot_s blocks[numofobjectslots];

    } memory_block_t;


public:
    FixSizeMemoryPool(){
        memory_block_ = nullptr;
        free_objectslot_ = nullptr;
    }
    ~FixSizeMemoryPool() = default;

    void* allocate(size_t size){
        //无空闲节点，申请新内存块
        if (!free_objectslot_) {
            memory_block_t* new_block = new memory_block_t;

            new_block->next = nullptr;

            free_objectslot_ = &new_block->blocks[0]; //设置内存块的第一个节点为空闲节点链表的首节点
            //将内存块的其它节点串起来
            for (int i = 1; i < numofobjectslots; ++i) {
                new_block->blocks[i-1].next = &new_block->blocks[i]; //i+1会越界，改为i，前面就要改为i-1
            }
            new_block->blocks[numofobjectslots - 1].next = nullptr;

            // 首次申请内存块
            if (memory_block_ == nullptr) {
                memory_block_ = new_block;
            } else {
                new_block->next = memory_block_; //头插法
                memory_block_ = new_block;  //更新头节点
            }
        }

        void* freeNode = free_objectslot_;
        free_objectslot_ = free_objectslot_->next;
        return freeNode;
    }

    void deallocate(void* p){
        free_objectslot_t* freeNode = static_cast<free_objectslot_t*>(p);
        freeNode->next = free_objectslot_;   //将释放的节点插入空闲节点头部
        free_objectslot_ = freeNode;
    }


private:
    //std::unique_ptr<memory_block_t> memory_pool_; 疑问1：这里能不能用智能指针

    memory_block_t* memory_block_;
    free_objectslot_t* free_objectslot_;

    std::mutex mtx_;


};

#endif // _FIXED_SIZE_MEMPOOL_H_
