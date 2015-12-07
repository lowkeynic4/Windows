#include "stdafx.h"
#include "TipsMySql.h"

//���캯�� ��ʼ����������������
MySQLInterface::MySQLInterface(): errorNum(0), errorInfo("ok")
{
	mysql_library_init( 0, NULL, NULL );
	mysql_init(&mysqlInstance);
	mysql_options(&mysqlInstance,MYSQL_SET_CHARSET_NAME,"gbk");
}

MySQLInterface::~MySQLInterface()
{

}

//����MySQL
bool MySQLInterface::connectMySQL(char* server, char* username, char* password, char* database,int port)
{
	if(mysql_real_connect(&mysqlInstance,server,username,password,database,port,0,0) != NULL)
		return true;
	else
		errorIntoMySQL();
	return false;
}
//ѡ�����ݿ�
bool MySQLInterface::selectDB(char* dbname)
{
	if(!mysql_select_db(&mysqlInstance,dbname))
		return true;
	else
		errorIntoMySQL();
	return false;
}
//�ж����ݿ��Ƿ���ڣ��������򴴽����ݿ⣬����
bool MySQLInterface::createDatabase(std::string& dbname)
{
	std::string queryStr = "create database if not exists ";
	queryStr += dbname;
	if (0 == mysql_query(&mysqlInstance,queryStr.c_str()))
	{
		queryStr = "use ";
		queryStr += dbname;
		if (0 == mysql_query(&mysqlInstance,queryStr.c_str()))
		{
			return true;
		}

	}
	errorIntoMySQL();
	return false;
}
//�ж����ݿ����Ƿ������Ӧ���������򴴽���
bool MySQLInterface::createdbTable(const std::string& query)
{
	if (0 == mysql_query(&mysqlInstance,query.c_str()))
	{
		return true;
	}
	errorIntoMySQL();
	return false;
}
//ɾ��ָ����
bool MySQLInterface::dropdbTable(const std::string& tableName)
{
	char str[128]={0};
	sprintf(str,"drop table %s",tableName.c_str());
	if (0 == mysql_query(&mysqlInstance,str))
	{
		return true;
	}
	errorIntoMySQL();
	return false;
}
//ɾ���������ݣ�������ṹ
bool MySQLInterface::truncateTable(const std::string& tableName)
{
	char str[128]={0};
	sprintf(str,"truncate table %s",tableName.c_str());
	if (0 == mysql_query(&mysqlInstance,str))
	{
		return true;
	}
	errorIntoMySQL();
	return false;
}
//д������
bool MySQLInterface::writeDataToDB(string queryStr)
{
	OutputDebugString(L"writeDataToDB\n");
	if(0==mysql_query(&mysqlInstance,queryStr.c_str()))
		return true;
	else
		errorIntoMySQL();
	return false;   
}
//��������
bool MySQLInterface::updateDataToDB(string queryStr)
{ 
	OutputDebugString(L"updateDataToDB\n");
	if(0==mysql_query(&mysqlInstance,queryStr.c_str()))
		return true;
	else
		errorIntoMySQL();
	return false;  
}
//��ȡ����
bool MySQLInterface::getDatafromDB(string queryStr, vector<vector<string> >& data)
{
	if(0!=mysql_query(&mysqlInstance,queryStr.c_str()))
	{
		errorIntoMySQL();
		return false;
	}

	result=mysql_store_result(&mysqlInstance);

	int row=mysql_num_rows(result);
	int field=mysql_num_fields(result);

	MYSQL_ROW line=NULL;
	line=mysql_fetch_row(result);

	int j=0;
	std::string temp;
	while(NULL!=line)
	{   
		std::vector<std::string> linedata;
		for(int i=0; i<field;i++)
		{
			if(line[i])
			{
				temp = line[i];
				linedata.push_back(temp);
			}
			else
			{
				temp = "";
				linedata.push_back(temp);
			}
		}
		line=mysql_fetch_row(result);
		data.push_back(linedata);
	}
	return true;
}

//������Ϣ
void MySQLInterface::errorIntoMySQL()
{
	errorNum=mysql_errno(&mysqlInstance);
	errorInfo=mysql_error(&mysqlInstance);
}

//�Ͽ�����
void MySQLInterface::closeMySQL()
{
	mysql_close(&mysqlInstance);
}