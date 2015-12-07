#pragma once
#ifndef	DLL_IMPORT
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif
#include <WinSock.h>
#include "mysql.h"
#include <iostream>
#include <string>
#include <vector>
#include <string>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libmysql.lib") 
using namespace std;
class DLL_API MySQLInterface
{
public:  
	MySQLInterface();
	virtual ~MySQLInterface();

	bool connectMySQL(char* server, char* username, char* password, char* database,int port);
	bool createDatabase(std::string& dbname);
	bool createdbTable(const std::string& query);

	bool selectDB(char* server);
	bool writeDataToDB(string queryStr);
	bool updateDataToDB(string queryStr);
	bool getDatafromDB(string queryStr, std::vector<std::vector<std::string> >& data);

	bool dropdbTable(const std::string& tableName);
	bool truncateTable(const std::string& tableName);

	void closeMySQL();
	void errorIntoMySQL();
public:
	int errorNum;                      // 错误代号
	const char* errorInfo;             // 错误提示
	
	
private:
	MYSQL mysqlInstance; // MySQL对象，必备的一个数据结构
	MYSQL_RES *result; // 用于存放结果 建议用char* 数组将此结果转存
};
