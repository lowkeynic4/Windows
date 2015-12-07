// fordarkray.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "stdio.h"
#include "classAccess.h"

int _tmain(int argc, _TCHAR* argv[])
{
	EMPINFO info;
	CAccess access;
	access.ConnectDateBase(_T("test.mdb"));
	//查
	if(!access.ExecuteSQL(_T("select name,salary from emp")))
	{
		printf("select error\n");
		return FALSE;
	}
	else 
	{
		SQLINTEGER cbID;
		RETCODE retcode;
		retcode = SQLBindCol(access.m_hstmt, 1, SQL_C_CHAR, info.name, 10, &cbID);
		retcode = SQLBindCol(access.m_hstmt, 2, SQL_C_LONG ,&info.salary,4, &cbID);

		while(SQLFetch(access.m_hstmt) != SQL_NO_DATA)
		{
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				printf("name:%s\n",info.name);	
				printf("salary:%ld\n",info.salary);
			}
			else
			{
				break;
			}
		}
	}
	//增
	if(access.ExecuteSQL(_T("insert into emp(name,salary) values ('dark',100)")))
		printf("insert succ\n");
	else 
		printf("insert err\n");
	//改
	if(access.ExecuteSQL(_T("update emp set name='world' where name='dark'")))
		printf("update succ\n");
	else 
		printf("update err\n");

	//删
	if(access.ExecuteSQL(_T("delete from emp where name='world'")))
		printf("del succ\n");
	else 
		printf("del err\n");

	//查
	if(!access.ExecuteSQL(_T("select name,salary from emp")))
	{
		printf("select success\n");
		return FALSE;
	}
	else 
	{
		SQLINTEGER cbID;
		RETCODE retcode;
		retcode = SQLBindCol(access.m_hstmt, 1, SQL_C_CHAR, info.name, 10, &cbID);
		retcode = SQLBindCol(access.m_hstmt, 2, SQL_C_LONG ,&info.salary,4, &cbID);

		while(SQLFetch(access.m_hstmt) != SQL_NO_DATA)
		{
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				printf("name:%s\n",info.name);	
				printf("salary:%ld\n",info.salary);
			}
			else
			{
				break;
			}
		}
	}


	access.DisConnectDB();
	getchar();
	return 0;
}

