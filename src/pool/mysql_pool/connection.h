#include <string>
//加上该宏才能正常使用mysql.h,不然会报错
//经查询好像是因为mysql.h依赖的东西根据平台不同包括在不同的头文件下
#ifdef _MSC_VER
#ifdef _WIN64
#include <WinSock2.h>
#elif _WIN32
#include <winsock.h>
#endif
#endif
#include <mysql/mysql.h>


using namespace std;
/*
实现MySQL数据库的操作
*/
class Connection
{
public:
    // 初始化数据库连接
    Connection();
    // 释放数据库连接资源
    ~Connection();
    // 连接数据库
    bool connect(string ip,
                 unsigned short port,
                 string user,
                 string password,
                 string dbname);
    // 更新操作 insert、delete、update
    bool update(string sql);
    // 查询操作 select
    MYSQL_RES* query(string sql);
    
    // 刷新一下连接的起始的空闲时间点
    void refreshAliveTime() { _alivetime = clock(); }
    // 返回存活的时间
    clock_t getAliveeTime()const { return clock() - _alivetime; }
private:
    MYSQL* _conn; // 表示和MySQL Server的一条连接
    clock_t _alivetime; // 记录进入空闲状态后的起始存活时间
};


// void Connection::refreshAliveTime() {
//     _aliveTime = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
// }

// long long Connection::getAliveeTime() {
//     return std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1) - _aliveTime;
// }

