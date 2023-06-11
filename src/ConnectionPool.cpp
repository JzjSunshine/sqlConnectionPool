#include <fstream>
#include <thread>
#include <jsoncpp/json/json.h>
#include "../include/ConnectionPool.h"

// C++ 11 线程安全
ConnectionPool *ConnectionPool::getConnectionPool()
{
    static ConnectionPool pool;
    return &pool;
}

// 解析json 文件
bool ConnectionPool::parseJsonFile()
{

    // m_ip = "192,168.0.102";
    // m_port = 33061;
    // m_user = "root";
    // m_passwd = "rsroot";
    // m_dbName = "qgydb";
    // m_minSize = 100;
    // m_maxSize = 300;
    // m_maxIdleTime = 5000;
    // m_timeout = 1000;

    ifstream ifs("../dbconfig.json");
    Json::Reader rd;
    Json::Value root;
    rd.parse(ifs, root);
    if (root.isObject()) // 判断读入的数据是否为 json 对象
    {
        m_ip = root["ip"].asString();
        m_port = root["port"].asInt();
        m_user = root["userName"].asString();
        m_passwd = root["password"].asString();
        m_dbName = root["dbName"].asString();
        m_minSize = root["minSize"].asInt();
        m_maxSize = -root["maxSize"].asInt();
        m_maxIdleTime = root["maxIdleTime"].asInt();
        m_timeout = root["timeout"].asInt();

        return true;
    }

    return false;
}

void ConnectionPool::producerConnection() // 生产数据库连接
{
    while (true)
    {
        unique_lock<mutex> locker(m_mutexQ);// 操作练级的时加锁
        while (m_connectionQ.size() >= m_minSize && m_connectionQ.size() <= m_maxSize)
        {
            // 生产者线程被阻塞 主要针对多个生产数据库的线程 在本例中，生产数据库的线程只有一个
            m_cond.wait(locker);
        }
        addConnection();
        // 连接被创建之后，唤醒消费者，调用条件变量
        m_cond.notify_all();// 生产者线程只有一个，会唤醒所有消费者

    }
}
void ConnectionPool::recycleConnection() // 销毁数据库连接
{
    while (true)
    {
        // 让线程休眠一定的时间 不要让线程一直执行
        this_thread::sleep_for(chrono::milliseconds(500));
        lock_guard<mutex> locker(m_mutexQ);// 给操作对象加锁
        while (m_connectionQ.size() > m_minSize)
        {
            // 取出队首的线程
            MysqlConn *conn = m_connectionQ.front();
            // 数据库连接的空闲时长：一直未被使用；使用了又被放回
            if (conn->getAliveTime() >= m_maxIdleTime)
            {
                m_connectionQ.pop();
                delete conn;
            }
            else
            {
                break;
            }
        }
    }// 这里 locker 对象会被析构，释放 m_mutex 锁
}

// 该函数内部不需要加锁，加锁时在调用该函数时加锁
void ConnectionPool::addConnection()
{
    MysqlConn *conn = new MysqlConn;
    conn->connect(m_user, m_passwd, m_dbName, m_ip, m_port);
    conn->refreshAliveTime(); // 记录连接时间
    m_connectionQ.push(conn);
}

// MysqlConn *ConnectionPool::getConnection()
shared_ptr<MysqlConn> ConnectionPool::getConnection()// 智能指针
{
    // 封装互斥锁
    unique_lock<mutex> locker(m_mutexQ); // unique_lock
    while (m_connectionQ.empty())
    {
        // 让调用 wait_for 的线程阻塞
        // 唤醒条件：1. notify_one 等唤醒；2. 等待时长结束，返回一个状态，名称为 timeout
        if (cv_status::timeout == m_cond.wait_for(locker, chrono::milliseconds(m_timeout)))
        {
            // 进入到这里队列依旧为空
            if(m_connectionQ.empty()){ // 保险起见再判断
                // return nullptr;/
                continue;
            }
        }
    }
    /**
     * 拿走连接之后该如何还回连接
     * 1. 定义函数，设置参数回收连接
     * 2. 使用智能指针
     *     共享的智能指针,对应析构时会把管理的指针析构，但这并不是我们想要的，我们需要的时地址回收
     *     故需要指定共享指针删除器的处理动作
     *      - 指定有名函数
     *      - 指定匿名函数
    */
    // 从数据库池中取出一个可用的连接
    shared_ptr<MysqlConn> connptr(m_connectionQ.front(),[this](MysqlConn* conn){
        lock_guard<mutex> locker(this->m_mutexQ);// 自动上锁和释放锁，释放锁的时机：locker对象析构时
        // 连接放回之后更新时间点
        conn->refreshAliveTime();
        this->m_connectionQ.push(conn);// 任务队列时共享队列
    });
    m_connectionQ.pop();
    // 消费连接之后，需要唤醒生产者
    m_cond.notify_all();// 生产者和消费者共用一个条件变量，唤醒时，生产者和消费者都会被唤醒
    return connptr;
}

ConnectionPool::ConnectionPool()
{
    // 加载配置文件
    if (!parseJsonFile())
    {
        return;
    }

    // 初始化数据库连接
    for (int i = 0; i < m_minSize; ++i)
    {
        addConnection();
    }
    // 实时检测连接池中空闲线程是否空闲太多（则释放）
    // 不够则生产
    // 创建和销毁，交给两个线程完成
    thread producer(&ConnectionPool::producerConnection, this); // 生产线程：生产数据库中的连接
    thread recycler(&ConnectionPool::recycleConnection, this);  // 释放线程：释放数据库池中的连接

    // 不能使得主线程阻塞，要使得这两个线程分离
    producer.detach();
    recycler.detach();
}

// 释放堆内存 队列释放之后，队列中的指针是不能被释放的
ConnectionPool::~ConnectionPool()
{
    // 判断任务队列是否为空
    while(!m_connectionQ.empty())
    {
        MysqlConn* conn = m_connectionQ.front();//依次取出并释放
        m_connectionQ.pop();
        delete conn;
    }
}