#include "connection_pool.h"
#include <cstdio>
#include <fstream>


// 线程安全的懒汉单例函数接口
ConnectionPool* ConnectionPool::getConnectionPool()
{
	static ConnectionPool pool; // lock和unlock
	return &pool;
}

// 从配置文件中加载配置项
bool ConnectionPool::loadConfigFile()
{
   std::ifstream file("mysql.ini");
    if (!file.is_open()) {
        printf("mysql.ini file is not exist!");
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        auto pos = line.find('=');
        if (pos == std::string::npos) continue;

        auto key = line.substr(0, pos);
        auto value = line.substr(pos + 1);

        if (key == "ip") _ip = value;
        else if (key == "port") _port = std::stoi(value);
        else if (key == "username") _username = value;
        else if (key == "password") _password = value;
        else if (key == "dbname") _dbname = value;
        else if (key == "initSize") _initSize = std::stoi(value);
        else if (key == "maxSize") _maxSize = std::stoi(value);
        else if (key == "maxIdleTime") _maxIdleTime = std::stoi(value);
        else if (key == "connectionTimeout") _connectionTimeout = std::stoi(value);
    }
    return true;
}

// 连接池的构造
ConnectionPool::ConnectionPool()
{
	// 加载配置项了
    if (!loadConfigFile()) {
        return;
    }

	// 创建初始数量的连接
	for (int i = 0; i < _initSize; ++i)
	{
        Connection* conn = new Connection();
        if (conn->connect(_ip, _port, _username, _password, _dbname)) {
            conn->refreshAliveTime();
            _connectionQue.push(conn);
            ++_connectionCnt;
        } else {
            delete conn;
        }
	}

	// // 启动一个新的线程，作为连接的生产者 linux thread => pthread_create
	// std::thread produce(std::bind(&ConnectionPool::produceConnectionTask, this));
	// produce.detach();

	// // 启动一个新的定时线程，扫描超过maxIdleTime时间的空闲连接，进行对于的连接回收
	// thread scanner(std::bind(&ConnectionPool::scannerConnectionTask, this));
	// scanner.detach();
    std::thread(&ConnectionPool::produceConnectionTask, this).detach();
    std::thread(&ConnectionPool::scannerConnectionTask, this).detach();
}

// 运行在独立的线程中，专门负责生产新连接
void ConnectionPool::produceConnectionTask() {
    for (;;) {
        std::unique_lock<std::mutex> lock(_queueMutex);
        while (!_connectionQue.empty()) {
            cv.wait(lock);  // 队列不空，此处生产线程进入等待状态
        }

        // 连接数量没有到达上限，继续创建新的连接
        if (_connectionCnt < _maxSize) {
            Connection* conn = new Connection();
            if (conn->connect(_ip, _port, _username, _password, _dbname)) {
                conn->refreshAliveTime();  // 刷新一下开始空闲的起始时间
                _connectionQue.push(conn);
                ++_connectionCnt;
            } else {
                delete conn;
            }
        }
        // 通知消费者线程，可以消费连接了
        cv.notify_all();
    }
}


// 给外部提供接口，从连接池中获取一个可用的空闲连接
shared_ptr<Connection> ConnectionPool::getConnection()
{
    // // 第一步先检测 当前连接数量是否达到最大的连接数量
    // if (m_db_cur_conn_cnt >= m_db_max_conn_cnt) {
    // // 如果已经到达了，看看是否需要超时等待
    // if (timeout_ms <= 0) // 死等，直到有连接可以用 或者 连接池要退出
    // {
    //     log_info("wait ms:%d\n", timeout_ms);
    //     m_cond_var.wait(lock, [this] {
    //     // log_info("wait:%d, size:%d\n", wait_cout++, m_free_list.size());
    //     // 当前连接数量小于最大连接数量 或者请求释放连接池时退出
    //     return (!m_free_list.empty()) | m_abort_request;
    //     });
    // }
    //else

        std::unique_lock<mutex> lock(_queueMutex);
        //当没有连接可以用时
        while (_connectionQue.empty())
        {
            // sleep
            if (std::cv_status::timeout == cv.wait_for(lock, chrono::milliseconds(_connectionTimeout)))
            {
                if (_connectionQue.empty())
                {
                    //LOG("获取空闲连接超时了...获取连接失败!");
                    printf("获取空闲连接超时了...获取连接失败!");
                    return nullptr;
                }
            }
        }

	/*
	shared_ptr智能指针析构时，会把connection资源直接delete掉，相当于
	调用connection的析构函数，connection就被close掉了。
	这里需要自定义shared_ptr的释放资源的方式，把connection直接归还到queue当中
	*/
	shared_ptr<Connection> sp(_connectionQue.front(),
		[&](Connection* pcon) {
			// 这里是在服务器应用线程中调用的，所以一定要考虑队列的线程安全操作
			unique_lock<mutex> lock(_queueMutex);
			pcon->refreshAliveTime(); // 刷新一下开始空闲的起始时间
			_connectionQue.push(pcon);
		});

	_connectionQue.pop();
	cv.notify_all();  // 消费完连接以后，通知生产者线程检查一下，如果队列为空了，赶紧生产连接

	return sp;
  }

// 扫描超过maxIdleTime时间的空闲连接，进行对于的连接回收
void ConnectionPool::scannerConnectionTask()
{
	for (;;)
	{
		// 通过sleep模拟定时效果
		this_thread::sleep_for(chrono::seconds(_maxIdleTime));

		// 扫描整个队列，释放多余的连接
		unique_lock<mutex> lock(_queueMutex);
		while (_connectionCnt > _initSize)
		{
			Connection* p = _connectionQue.front();
			if (p->getAliveeTime() >= (_maxIdleTime * 1000))
			{
				_connectionQue.pop();
				_connectionCnt--;
				delete p; // 调用~Connection()释放连接
			}
			else
			{
				break; // 队头的连接没有超过_maxIdleTime，其它连接肯定没有
			}
		}
	}
}

