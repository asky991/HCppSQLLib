/*
 * DBPool.h
 *
 *  Created on: 2014年7月22日
 *      Author: ziteng
 *  Modify By ZhangYuanhao
 *  2015-01-12
 *  enable config the max connection of every instance
 */

#ifndef DBPOOL_H_
#define DBPOOL_H_

//#include "../base/util.h"
//#include "ThreadPool.h"
//#include <mysql.h>
#include <map>
#include <list>
#include <string>
#include <iostream>
#include <mutex>
using namespace std;

#ifdef WIN32
typedef	unsigned int SOCKET;
#include "mysql.h"
#else
#include "mysql.h"
#endif

#define MAX_ESCAPE_STRING_LEN	10240


#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#ifdef _WIN32
#pragma comment(lib,"lib/x86/pthreadVC2.lib")  
#elif _WIN64
#pragma comment(lib,"lib/x64/pthreadVC2.lib") 
#endif // WIN32


class CThreadNotify
{
public:
	CThreadNotify()
	{
		pthread_mutexattr_init(&m_mutexattr);
		pthread_mutexattr_settype(&m_mutexattr, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(&m_mutex, &m_mutexattr);

		pthread_cond_init(&m_cond, NULL);
	}
	~CThreadNotify()
	{
		pthread_mutexattr_destroy(&m_mutexattr);
		pthread_mutex_destroy(&m_mutex);

		pthread_cond_destroy(&m_cond);
	}
	void Lock() { pthread_mutex_lock(&m_mutex); }
	void Unlock() { pthread_mutex_unlock(&m_mutex); }
	void Wait() { pthread_cond_wait(&m_cond, &m_mutex); }
	void Signal() { pthread_cond_signal(&m_cond); }
private:
	pthread_mutex_t 	m_mutex;
	pthread_mutexattr_t	m_mutexattr;

	pthread_cond_t 		m_cond;
};

class CResultSet {
public:
	CResultSet(MYSQL_RES* res);
	virtual ~CResultSet();

	bool Next();
	int GetInt(const char* key);
	char* GetString(const char* key);
private:
	int _GetIndex(const char* key);

	MYSQL_RES* 			m_res;
	MYSQL_ROW			m_row;
	map<string, int>	m_key_map;
};

/*
 * TODO:
 * 用MySQL的prepare statement接口来防止SQL注入
 * 暂时只用于插入IMMessage表，因为只有那里有SQL注入的风险，
 * 以后可以把全部接口用这个来实现替换
 */
class CPrepareStatement {
public:
	CPrepareStatement();
	virtual ~CPrepareStatement();

	bool Init(MYSQL* mysql, string& sql);

	void SetParam(uint32_t index, int& value);
	void SetParam(uint32_t index, uint32_t& value);
    void SetParam(uint32_t index, string& value);
    void SetParam(uint32_t index, const string& value);

	bool ExecuteUpdate();
	uint32_t GetInsertId();
private:
	MYSQL_STMT*	m_stmt;
	MYSQL_BIND*	m_param_bind;
	uint32_t	m_param_cnt;
};

class CDBPool;

class CDBConn {
public:
	CDBConn(CDBPool* pDBPool);
	virtual ~CDBConn();
	int Init();

	CResultSet* ExecuteQuery(const char* sql_query);
	bool ExecuteUpdate(const char* sql_query);
	char* EscapeString(const char* content, uint32_t content_len);

	uint32_t GetInsertId();

	const char* GetPoolName();
	MYSQL* GetMysql() { return m_mysql; }
private:
	CDBPool* 	m_pDBPool;	// to get MySQL server information
	MYSQL* 		m_mysql;
	char		m_escape_string[MAX_ESCAPE_STRING_LEN + 1];
};

class CDBPool {
public:
	CDBPool(const char* pool_name, const char* db_server_ip, uint16_t db_server_port,
			const char* username, const char* password, const char* db_name, int max_conn_cnt);
	virtual ~CDBPool();

	int Init();
	CDBConn* GetDBConn();
	void RelDBConn(CDBConn* pConn);

	const char* GetPoolName() { return m_pool_name.c_str(); }
	const char* GetDBServerIP() { return m_db_server_ip.c_str(); }
	uint16_t GetDBServerPort() { return m_db_server_port; }
	const char* GetUsername() { return m_username.c_str(); }
	const char* GetPasswrod() { return m_password.c_str(); }
	const char* GetDBName() { return m_db_name.c_str(); }
private:
	string 		m_pool_name;
	string 		m_db_server_ip;
	uint16_t	m_db_server_port;
	string 		m_username;
	string 		m_password;
	string 		m_db_name;
	int			m_db_cur_conn_cnt;
	int 		m_db_max_conn_cnt;
	list<CDBConn*>	m_free_list;
	CThreadNotify	m_free_notify;
};

// manage db pool (master for write and slave for read)
class CDBManager {
public:
	virtual ~CDBManager();

	static CDBManager* getInstance();

	int Init();

	CDBConn* GetDBConn(const char* dbpool_name);
	void RelDBConn(CDBConn* pConn);
private:
	CDBManager();

private:
	static CDBManager*		s_db_manager;
	map<string, CDBPool*>	m_dbpool_map;
};

#endif /* DBPOOL_H_ */
