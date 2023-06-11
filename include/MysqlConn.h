#ifndef MYSQLCONN_H
#define MYSQLCONN_H

#include <iostream>
#include <string>
#include <chrono>
#include <mysql/mysql.h>
#include <jsoncpp/json/json.h>
#include <ctime>

using namespace std;

class MysqlConn{
public:
    // 初始化数据库连接
    MysqlConn();
    // 释放数据库连接
    ~MysqlConn();
    // 连接数据库
    bool connect(string user, string passswd, string dbName, string ip, unsigned short port);
    //更新数据库: insert, update, delete
    bool update(string sql);
    //查询数据库
    bool query(string sql);
    //遍历查询得到的结果集
    bool next();
    //得到结果集中的字段值
    string value(int index);// 有很多的字段，使用索引返回字段
    //事务操作 将事务设置成手动提交
    bool transaction();
    //提交事务
    bool commit();
    //事务回滚
    bool rollback();

    // 刷新数据库起始空闲时间点
    void refreshAliveTime();
    // 计算连接存货的总时长
    long long getAliveTime();

private:
    void freeResult();
    MYSQL * m_conn = nullptr;
    MYSQL_RES* m_result = nullptr;// 释放时机，地址的数据读完了就释放
    MYSQL_ROW m_row = nullptr;// 保存 next 遍历结果

    // 绝对时钟:是单调的时钟，只会增长，不受系统时间的影响，适用于记录程序耗时
    // 系统时钟：系统时间，可以被修改或者同步，可能不准确或者不连续，适用于获取当前日期和时间
    chrono::steady_clock::time_point m_alivetime;

};

#endif