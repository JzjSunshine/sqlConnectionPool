#ifndef CONNECTIONPOOL
#define CONNECTIONPOOL

#include <queue>
#include <string>
#include <condition_variable>
#include <mutex>
#include <jsoncpp/json/json.h>
#include <thread>
#include <unistd.h>
#include <fstream>

#include "MysqlConn.h"

using namespace std;

class ConnectionPool{
public:
    static ConnectionPool* getConnectionPool();
    //删除拷贝构造和拷贝赋值运算符重载函数 防止创建新的对象
    ConnectionPool(const ConnectionPool& obj) = delete;// 拷贝构造
    ConnectionPool& operator=(const ConnectionPool& obj) = delete;
    // 将可用连接给用户
    shared_ptr<MysqlConn> getConnection();

    ~ConnectionPool();//队列中的指针指向的地址是不会被释放的
private:
    ConnectionPool();
    void producerConnection(); //生产数据库连接
    void recycleConnection();  // 销毁数据库连接
    void addConnection();      // 增加连接
    bool parseJsonFile();      // 解析json 文件 ++++
    queue<MysqlConn*>m_connectionQ;

    string m_ip;             // 数据库服务器ip地址
    string m_user;           // 数据库服务器用户名
    string m_dbName;         // 数据库服务器的数据库名
    string m_passwd;         // 数据库服务器密码
    unsigned short m_port;   //数据库绑定端口号

    int m_minSize;           // 连接池维护的最小连接数
    int m_maxSize;           // 连接池维护的最大连接数
    int m_maxIdleTime;       // 连接池中连接的最大空闲时长
    int m_timeout;           // 连接池获取连接的超时时长

    mutex m_mutexQ;          // 保护连接的互斥锁
    condition_variable m_cond; // 对线程的阻塞（条件变量）;既阻塞生产者，又阻塞消费者
     
};

#endif
