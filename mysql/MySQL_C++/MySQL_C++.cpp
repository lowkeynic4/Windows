#include "stdafx.h"
#include <vector>
#include "TipsMySql.h"
//#define DEBUG
using namespace std;
int _tmain(int argc, _TCHAR* argv[])
{
	char dbuser[30]="root";  //用户名	
	char dbpasswd[30]=""; //密码
	char dbip[30]="localhost"; //服务器ip
	char dbname[50]="test";   //数据库名
	char tablename[50]="manager"; //表名
	char executStr[1024]={0};	//sql语句
	MySQLInterface mySql;
#ifdef DEBUG 
	/////1.第一种操作，数据库和表都已经建好了，直接操作数据库。
	///////////////////连接数据库/////////////////////////////////
	bool a = mySql.connectMySQL(dbip,dbuser,dbpasswd,dbname,3306);
	if (a)
	{
		printf("connect ok!\n");
	}
	else printf("error\n");
	///////////////////选择数据库/////////////////////////////////
	if(mySql.selectDB(dbname))
	{
		printf("select ok!\n");

	}
	else  printf("select error!\n"); 
	///////////////////写数据//////////////////////////////////////
	memset(executStr,0,1024);
	sprintf(executStr,"%s",
		"INSERT INTO manager(`manager_of`, `name`,`sex`,`age`) VALUES ('windows', 'tenshine','男','12')");
	if (mySql.writeDataToDB(executStr))
	{
		printf("insert ok!\n");
	}
	else printf("insert error\n");
// 	///////////////////更新数据/////////////////////////////////
	memset(executStr,0,1024);
	sprintf(executStr,"%s","update manager set name='帅哥' where name='tenshine'");
	if (mySql.updateDataToDB(executStr))
	{
		printf("update ok!\n");
	}
	else printf("update error\n");
	///////////////////取数据//////////////////////////////////
	memset(executStr,0,1024);
	sprintf(executStr,"select id,name from %s",tablename);
	vector<vector<string> > data;
	mySql.getDatafromDB(executStr,data);
	//data.size()共有多少条记录
	for(int i = 0 ; i < data.size(); i++)
	{
		printf("%s\n",data.at(i).at(1).c_str());

	}
	mySql.closeMySQL();
	system("pause");
#else

	
	///2.第二种操作，先建数据库，再建表，插入和其他操作：包括删除表，清空表
	/// 新建数据库之后就不需要再选择一次数据库了，因为在新建完之后会直接选择它

	///////////////////连接数据库/////////////////////////////////
	bool a = mySql.connectMySQL(dbip,dbuser,dbpasswd,dbname,3306);
	if (a)
	{
		printf("connect ok!\n");
	}
	else printf("error\n");
	//////////////////新建数据库///////////////////////////////////
	string databaseName="hello";
	if(mySql.createDatabase(databaseName))
		printf("createDatabase ok!\n");
	else
		printf("createDatabase error!\n");
	///////////////////新建表////////////////////////////////////
	sprintf(executStr,"%s","create table user(id int(4) primary key, name varchar(50)not null)");
	if(mySql.createdbTable(executStr))
		printf("create table ok!\n");
	else 
		printf("create table error!\n");
	///////////////////插入数据///////////////////////////////////
	sprintf(executStr,"%s",
		"insert into user(`id`, `name`) VALUES (1, 'tenshine')");
	if (mySql.writeDataToDB(executStr))
	{
		printf("insert ok!\n");
	}
	else 
		printf("insert error\n");
	////////////////////更新数据///////////////////////////////////
	sprintf(executStr,"%s","update user set name='帅哥' where name='tenshine'");
	if (mySql.updateDataToDB(executStr))
	{
		printf("update ok!\n");
	}
	else printf("update error\n");
	///////////////////取数据//////////////////////////////////
	sprintf(executStr,"select id,name from %s","user");
	vector<vector<string> > data;
	mySql.getDatafromDB(executStr,data);
	//data.size()共有多少条记录
	printf("共有%d条数据\n",data.size());
	for(int i = 0 ; i < data.size(); i++)
	{
		printf("%s:",data.at(i).at(0).c_str());
		printf("%s\n",data.at(i).at(1).c_str());

	}
	///////////////////清空表内数据，保存表结构///////////////////
	if(mySql.truncateTable("user"))
		printf("truncate test ok!\n");
	else 
		printf("truncate test error!\n");
	///////////////////删除表////////////////////////////////////
	if(mySql.dropdbTable("user"))
		printf("drop user ok!\n");
	else 
		printf("drop user error!\n");
	/////////////////////关闭///////////////////////////////////////
	mySql.closeMySQL();
	system("pause");
#endif
	return 0;
}

