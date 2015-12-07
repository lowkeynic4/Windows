#pragma once
#include <windows.h>
#include <sqlext.h>
#include <sqltypes.h>
#pragma comment(lib, "odbc32.lib")

class CAccess
{
private:

public:
	CAccess(void);
	~CAccess(void);
	BOOL ExecuteSQL(TCHAR * sql);
	BOOL ConnectDateBase(TCHAR* szPath);
	BOOL DisConnectDB();
	SQLHENV m_henv;
	SQLHDBC m_hdbc;
	SQLHSTMT m_hstmt ;
};

typedef struct empInfo
{
	char name[10];
	long salary;
}EMPINFO,*PEMPINFO; 

