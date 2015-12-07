#include "StdAfx.h"
#include "classAccess.h"


CAccess::CAccess(void)
{
	this->m_hdbc=NULL;
	this->m_henv=NULL;
	this->m_hstmt=NULL;
}


CAccess::~CAccess(void)
{
	
}



BOOL CAccess::ConnectDateBase(TCHAR* szPath) 
{
	RETCODE		retcode;
	m_hstmt = SQL_NULL_HSTMT;
	SQLTCHAR 		ConnStrIn[255]={0};
	SQLTCHAR 		ConnStrOut[255]={0};
	SQLSMALLINT	cbConnStrOut = 0;
	SQLINTEGER	cb1=SQL_NTS;

	retcode = SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&m_henv);
	retcode = SQLSetEnvAttr(m_henv,SQL_ATTR_ODBC_VERSION,(SQLPOINTER)SQL_OV_ODBC3,SQL_IS_INTEGER);

	wsprintf( (TCHAR *)ConnStrIn,_T("Driver={MICROSOFT ACCESS DRIVER (*.mdb)};DBQ=%s"),szPath);
	retcode = SQLAllocHandle(SQL_HANDLE_DBC,m_henv,&m_hdbc);
	retcode = SQLDriverConnect( m_hdbc,				// Connection handle
		NULL,					// Window handle
		ConnStrIn,				// Input connect string
		SQL_NTS,				// Null-terminated string
		ConnStrOut,				// Address of output buffer
		500,				// Size of output buffer
		&cbConnStrOut,			// Address of output length
		SQL_DRIVER_NOPROMPT );
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
		retcode = SQLFreeHandle(SQL_HANDLE_DBC,m_hdbc);
		retcode = SQLFreeHandle(SQL_HANDLE_ENV,m_henv);
		m_henv = SQL_NULL_HENV;
		m_hdbc = SQL_NULL_HDBC;
		m_hstmt = SQL_NULL_HSTMT;
		//::MessageBox(NULL,"¡¨Ω” ˝æ›ø‚ ß∞‹£°","Ã· æ",MB_OK);
		return FALSE;
	}
	return TRUE ;

}

BOOL CAccess::DisConnectDB()
{
	if(this->m_hdbc!=NULL)
	{
		SQLFreeHandle(SQL_HANDLE_DBC,m_hdbc);
		m_hdbc=NULL;
	}
	if(this->m_henv!=NULL)
	{
		SQLFreeHandle(SQL_HANDLE_ENV,m_henv);
		m_henv=NULL;
	}
	if(this->m_hstmt!=NULL)
	{
		SQLFreeHandle(SQL_HANDLE_STMT,m_hstmt);
		m_hstmt=NULL;
	}
	return TRUE;
}


BOOL CAccess::ExecuteSQL(TCHAR * sql)
{

	RETCODE retcode;
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &m_hstmt);
	if (SQL_ERROR == retcode)
	{
		//MessageBox("∑÷≈‰”Ôæ‰æ‰±˙ ß∞‹£°");
		return false ;
	}

	retcode = SQLSetStmtAttr(m_hstmt, SQL_ATTR_CURSOR_TYPE, (SQLPOINTER)SQL_CURSOR_DYNAMIC, SQL_IS_INTEGER);
	if (SQL_ERROR == retcode)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, m_hstmt);
		m_hstmt = SQL_NULL_HSTMT;
		//MessageBox("…Ë÷√”Ôæ‰æ‰±˙ ß∞‹£°");
		return false ;
	}
	retcode = SQLSetStmtAttr(m_hstmt, SQL_ATTR_CONCURRENCY, (SQLPOINTER)SQL_CONCUR_ROWVER, SQL_IS_INTEGER);
	if (SQL_ERROR == retcode)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, m_hstmt);
		m_hstmt = SQL_NULL_HSTMT;
		//MessageBox("…Ë÷√”Ôæ‰æ‰±˙ ß∞‹£°");
		return false ;
	}

	retcode = SQLExecDirect(m_hstmt, (SQLTCHAR *)sql, 256);
	if (SQL_ERROR == retcode)
	{

		SQLFreeHandle(SQL_HANDLE_STMT, m_hstmt);
		m_hstmt = SQL_NULL_HSTMT;
		return false ;
	}

	return true;
}