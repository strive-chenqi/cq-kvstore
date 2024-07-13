#include"connection_pool.h"
// #include <mysql/mysql.h>
// #include <stdio.h>

int main() {
	clock_t begin = clock();
	ConnectionPool* p = ConnectionPool::getConnectionPool();

    std::thread t1([]() {
        for (int i = 0; i < 1; ++i) {
            ConnectionPool* pool = ConnectionPool::getConnectionPool();
            std::shared_ptr<Connection> conn = pool->getConnection();
            if (conn) {
                std::string sql = "insert into user1 values(4, 90)";
                conn->update(sql);
            }
        }
    });

    t1.join();


	// std::thread t1([]() {
	// 	for (int i = 0; i < 1; i++) {
	// 	/*Connection conn;
	// 	char sql[1024];
	// 	sprintf(sql," insert into user values(NULL, '%d', '%d')",i,i);
	// 	conn.connect("127.0.0.1", 3306, "root", "245330", "javaweb_login");
	// 	conn.update(sql);*/

    //     ConnectionPool* p = ConnectionPool::getConnectionPool();
	// 	shared_ptr<Connection> conn = p->getConnection();

    //     string sql = "insert into user values(3, 1)";

	// 	// char sql[1024];
	// 	// sprintf(sql, "insert into user values(%d, %d);", i, i);
	// 	//sprintf(sql, " create table user%d(id int primary key, age int)", i);
	// 	conn->connect("127.0.0.1", 3306, "root", "123456", "mysql_test");
	// 	conn->update(sql);


	// 	}
	// 	});
	// thread t2([]() {
	// 	for (int i = 1; i < 4; i++) {
	// 		/*Connection conn;
	// 		char sql[1024];
	// 		sprintf(sql, " insert into user values(NULL, '%d', '%d')", i, i);
	// 		conn.connect("127.0.0.1", 3306, "root", "245330", "javaweb_login");
	// 		conn.update(sql);*/

	// 		ConnectionPool* p = ConnectionPool::getConnectionPool();
	// 		shared_ptr<Connection> conn = p->getConnection();
	// 		char sql[1024];
	// 		sprintf(sql, " insert into user%d values(%d, %d)", i, i, i);
	// 		conn->connect("127.0.0.1", 3306, "root", "123456", "mysql_test");
	// 		conn->update(sql);


	// 	}
	// 	});
	// thread t3([]() {
	// 	for (int i = 1; i < 4; i++) {
	// 	/*	Connection conn;
	// 		char sql[1024];
	// 		sprintf(sql, " insert into user values(NULL, '%d', '%d')", i, i);
	// 		conn.connect("127.0.0.1", 3306, "root", "245330", "javaweb_login");
	// 		conn.update(sql);*/

	// 		ConnectionPool* p = ConnectionPool::getConnectionPool();
	// 		shared_ptr<Connection> conn = p->getConnection();
	// 		char sql[1024];
	// 		sprintf(sql, " insert into user%d values(%d, %d)", i, i+3, i+3);
	// 		conn->connect("127.0.0.1", 3306, "root", "123456", "mysql_test");
	// 		conn->update(sql);


	// 	}
	// 	});
	// thread t4([]() {
	// 	for (int i = 1; i < 4; i++) {
	// 	/*	Connection conn;
	// 		char sql[1024];
	// 		sprintf(sql, " insert into user values(NULL, '%d', '%d')", i, i);
	// 		conn.connect("127.0.0.1", 3306, "root", "245330", "javaweb_login");
	// 		conn.update(sql);*/

	// 		ConnectionPool* p = ConnectionPool::getConnectionPool();
	// 		shared_ptr<Connection> conn = p->getConnection();
	// 		char sql[1024];
	// 		sprintf(sql, "insert into user%d values(%d,%d);", i, i+6, i+6);
	// 		conn->connect("127.0.0.1", 3306, "root", "123456", "mysql_test");
	// 		conn->update(sql);


	// 	}
	// 	});
	// t1.join();
	// t2.join();
	// t3.join();
	// t4.join();

	// MYSQL mysql; //数据库句柄
	// MYSQL_RES* res; //查询结果集 
	// MYSQL_ROW row; //记录结构体

	// //初始化数据库 
	// mysql_init(&mysql);
    
	// //设置字符编码
	// mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "gbk");
    
	// //连接数据库
	// if (mysql_real_connect(&mysql, "127.0.0.1", "root",
	// 	//"password", "database_name"分别填写自己的的数据库登录密码和表所在的数据库名称
	// 	//这里替换成自己的的数据库等路密码和自己所建的数据库名称即可
	// 	"123456", "mysql_test", 3306, NULL, 0) == NULL) {
        
	// 	printf("错误原因： %s\n", mysql_error(&mysql));
	// 	printf("连接失败！\n");
	// 	exit(-1);
	// }
    
	// //查询数据
	// int ret = mysql_query(&mysql, "select * from user1;");
	// //student是自己在数据库中所建的表名
	// printf("ret: %d\n", ret);
    
	// //获取结果集
	// res = mysql_store_result(&mysql);
    
	// //给 ROW 赋值，判断 ROW 是否为空，不为空就打印数据。
	// while (row = mysql_fetch_row(res)) {
	// 	printf("%s ", row[0]); //打印 ID
	// 	printf("%s ", row[1]); //打印姓名
	// }
    
	// //释放结果集 
	// mysql_free_result(res); 
    
    // //关闭数据库
	// mysql_close(&mysql);
	// //system("pause");
	

	clock_t end = clock();
	std::cout << end - begin << "ms" << std::endl;
    return 0;
}

