#ifndef _KV_STORE_ENGINE_H_
#define _KV_STORE_ENGINE_H_


#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <map>

class KVStoreEngine {
public:
    virtual ~KVStoreEngine() = default;
    virtual void put(const std::string& key, const std::string& value) = 0;
    virtual std::string get(const std::string& key) = 0;
    virtual void remove(const std::string& key) = 0;
};

// 使用哈希表实现
class HashMapEngine : public KVStoreEngine {
public:
    void put(const std::string& key, const std::string& value) override;
    std::string get(const std::string& key) override;
    void remove(const std::string& key) override;

private:
    std::unordered_map<std::string, std::string> store;
};

// 使用数组实现
class ArrayEngine : public KVStoreEngine {
public:
    void put(const std::string& key, const std::string& value) override;
    std::string get(const std::string& key) override;
    void remove(const std::string& key) override;

private:
    std::vector<std::pair<std::string, std::string>> store;
};

// 使用红黑树实现
class RedBlackTreeEngine : public KVStoreEngine {
public:
    void put(const std::string& key, const std::string& value) override;
    std::string get(const std::string& key) override;
    void remove(const std::string& key) override;

private:
    std::map<std::string, std::string> store;
};




#endif //_KV_STORE_ENGINE_H_