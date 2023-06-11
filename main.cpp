#include <iostream>
#include <memory>
#include <chrono>
#include "../include/MysqlConn.h"
#include "../include/ConnectionPool.h"

using namespace std;

// 1. 单线程：使用/不使用连接池
// 单线程不使用数据库连接池
void op1(int begin, int end)
{
    for(int i=begin;i<end;i++){
        MysqlConn conn; // 将该语句写入这里，相当于是创建多个连接，若写在外面则是创建了一个连接
        conn.connect("root","rsroot", "qgydb", "192.168.0.102",33061);
        char sql[1024] = {0};
        sprintf(sql,"insert into person values(%d,25,'man','tom')",i);
        bool flag = conn.update(sql);
        // cout << "插入结果: " << flag << endl;
    }
}

// 单线程使用数据库连接池
void op2(ConnectionPool* pool, int begin,int end)
{
    for(int i=begin;i<end;i++){
        shared_ptr<MysqlConn> conn = pool->getConnection();
        char sql[1024] = {0};
        sprintf(sql,"insert into person values(%d,25,'man','tom')",i);
        bool flag = conn->update(sql);
        // cout << "插入结果: " << flag << endl;
    }
}
// 2. 多线程：使用/不使用连接池


void test1()
{
#if 1
    // 不使用连接池，单线程 用时：117932963436 纳秒, 117932毫秒
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    op1(0,5000);
    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    auto length = end - begin;
    cout << "不使用连接池，单线程 用时：" << length.count() << " 纳秒, "
                                    << length.count() / 1000000 << "毫秒" << endl;
#else  
    // 连接池，单线程 用时：12400522151 纳秒, 12400毫秒
    ConnectionPool *pool = ConnectionPool::getConnectionPool();
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    op2(pool, 0,5000);
    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    auto length = end - begin;
    cout << "连接池，单线程 用时：" << length.count() << " 纳秒, "
                                    << length.count() / 1000000 << "毫秒" << endl;
#endif
}


void test2()
{
#if 0
    // 不使用连接池，多线程 用时：23280875138 纳秒, 23280毫秒
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    thread t1(op1, 0,1000);
    thread t2(op1, 1000,2000);
    thread t3(op1, 2000,3000);
    thread t4(op1, 3000,4000);
    thread t5(op1, 4000,5000);

    t1.join();// 阻塞主线程
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    auto length = end - begin;
    cout << "不使用连接池，多线程 用时：" << length.count() << " 纳秒, "
                                    << length.count() / 1000000 << "毫秒" << endl;
#else 
    // 使用连接池，多线程 用时：3497524167 纳秒, 3497毫秒
    ConnectionPool *pool = ConnectionPool::getConnectionPool();
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    thread t1(op2, pool, 0,1000);
    thread t2(op2, pool, 1000,2000);
    thread t3(op2, pool, 2000,3000);
    thread t4(op2, pool, 3000,4000);
    thread t5(op2, pool, 4000,5000);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    auto length = end - begin;
    cout << "使用连接池，多线程 用时：" << length.count() << " 纳秒, "
                                    << length.count() / 1000000 << "毫秒" << endl;
#endif

}
int query()
{
    /**
     * 测试数据库连接的正确性
    */
    MysqlConn conn;
    conn.connect("root","rsroot", "qgydb", "192.168.0.102",33061);
    string sql = "insert into person values(7,25,'man','tom')";
    bool flag = conn.update(sql);
    cout << "flag: " << flag << endl;

    sql = "select *from person";
    conn.query(sql);
    while(conn.next())
    {
        cout << conn.value(0) << ", "
            << conn.value(1) << ", "
            << conn.value(2) << ", "
            << conn.value(3) << ", " << endl;
    }
    return 0;
}

int main()
{
    // query();
    // test1();
    test2();
    return 0;
}