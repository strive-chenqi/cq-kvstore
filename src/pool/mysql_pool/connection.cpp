#include "connection.h"
#include <cstdio>
#include <mysql/mysql.h>
using namespace std;


Connection::Connection()
{
    // 初始化数据库连接
    _conn = mysql_init(nullptr);
    if (!_conn) {
        printf("mysql_init failed\n");
    }
}

Connection::~Connection()
{
	// 释放数据库连接资源
	if (_conn != nullptr)
		mysql_close(_conn);
}

bool Connection::connect(string ip, unsigned short port,
	string username, string password, string dbname)
{
	// 连接数据库
    if(!mysql_real_connect(_conn, ip.c_str(), username.c_str(),
		password.c_str(), dbname.c_str(), port, nullptr, 0)) {
            printf("连接失败");
            return false;
        }     
    else {
        printf("连接成功");
        return true;
    }
       
	
}

bool Connection::update(string sql)
{
    // 更新操作 insert、delete、update
    int ret = mysql_query(_conn, sql.c_str());
    printf("ret: %d\n", ret);

    if (ret == 0) {
        printf("操作成功\n");
        return true;
    } else {
        printf("操作失败: %s\n", mysql_error(_conn));
        return false;
    }
	
}

MYSQL_RES* Connection::query(string sql)
{
    // 查询操作 select
    int ret = mysql_query(_conn, sql.c_str());
    printf("ret: %d\n", ret);

    if (ret == 0) {
        printf("查询成功\n");
        return mysql_use_result(_conn);
    } else {
        printf("查询失败: %s\n", mysql_error(_conn));
        return nullptr;
    }
}
