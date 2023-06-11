#include <string>
#include <iostream>

#include "../include/MysqlConn.h"


using namespace std;

MysqlConn::MysqlConn()
{
    m_conn = mysql_init(nullptr);
    // 设置编码
    mysql_set_character_set(m_conn, "utf8");
}
MysqlConn::~MysqlConn()
{
    if (m_conn != nullptr)
    {
        mysql_close(m_conn);
    }
    freeResult();
}
bool MysqlConn::connect(string user, string passswd, string dbName, string ip, unsigned short port = 3306)
{
    MYSQL *ptr = mysql_real_connect(m_conn, ip.c_str(), user.c_str(), passswd.c_str(), dbName.c_str(), port, NULL, 0);

    return ptr != nullptr;
}

bool MysqlConn::update(string sql)
{
    if(mysql_query(m_conn, sql.c_str())){
        return false;
    }
    return true;
}
// 查询数据库
bool MysqlConn::query(string sql)
{
    // 查询之前清空上一次数据
    freeResult();
    if(mysql_query(m_conn, sql.c_str())){
        return false;
    }
    m_result = mysql_store_result(m_conn);

    return true;
}
// 遍历查询得到的结果集
bool MysqlConn::next()
{
    if(m_result != nullptr)
    {
        m_row = mysql_fetch_row(m_result);
        if(m_row != nullptr){
            return true;
        }
    }
    return false;
}
// 得到结果集中的字段值 // 有很多的字段，使用索引返回字段
string MysqlConn::value(int index)
{
    // 当前记录中字段的数量
    int fieldNum = mysql_num_fields(m_result);
    if(index >= fieldNum || index < 0){
        return string();
    }
    char *val = m_row[index];
    unsigned long len = mysql_fetch_lengths(m_result)[index];
    // 创建字符串对象的构造函数，它接受一个字符指针和一个长度参数，
    // 然后从字符指针开始复制长度个字符到字符串对象中
    return string(val,len);
    
} 
// 事务操作 将事务设置成手动提交
bool MysqlConn::transaction()
{
    return mysql_autocommit(m_conn, false);
}
// 提交事务
bool MysqlConn::commit()
{
    return mysql_commit(m_conn);
}
// 事务回滚
bool MysqlConn::rollback()
{
    return mysql_rollback(m_conn);
}

void MysqlConn::freeResult()
{
    if(m_result){
        mysql_free_result(m_result);
        m_result = nullptr;
    }
}

// 刷新数据库起始空闲时间点
void MysqlConn::refreshAliveTime()
{
    m_alivetime = chrono::steady_clock::now();
}
// 计算连接存货的总时长
long long MysqlConn::getAliveTime()
{
    chrono::nanoseconds res = chrono::steady_clock::now() - m_alivetime;
    //时间转换
    chrono::microseconds millsec = chrono::duration_cast<chrono::milliseconds>(res);
    // 有多少个毫秒
    return millsec.count();
}