
#ifndef _KV_STORE_H_
#define _KV_STORE_H_

#include "../network/io_uring_network.h"
#include "../network/network_factory.h"
#include "../network/coroutine_network.h"
#include "../kv_engine/kv_store_engine.h"

// KVStore 类
class KVStore {
public:
    enum EngineType {
        HASH_MAP,
        ARRAY,
        RED_BLACK_TREE
        // 其他引擎类型
    };

    KVStore(EngineType type);

    void put(const std::string& key, const std::string& value);
    std::string get(const std::string& key);
    void remove(const std::string& key);

private:
    std::unique_ptr<KVStoreEngine> engine;
};

#endif //_KV_STORE_H_