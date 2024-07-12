#ifndef _MEMORYPOOL_H_
#define _MEMORYPOOL_H_

#include <cstddef>

// Base class for memory pools
class MemoryPool {
public:
    virtual ~MemoryPool() {}
    virtual void* allocate(size_t size) = 0;
    virtual void deallocate(void* p) = 0;
};

#endif // _MEMORYPOOL_H_
