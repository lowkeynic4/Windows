// token.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <Windows.h>
#include <TlHelp32.h>
BOOL GetTokenByName(HANDLE &hToken,LPSTR lpName)
{
	OutputDebugString("3");
	if(!lpName)
	{
		return FALSE;
	}
	HANDLE hProcessSnap = NULL; 
	BOOL bRet = FALSE; 
	PROCESSENTRY32 pe32 = {0}; 

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
	if (hProcessSnap == INVALID_HANDLE_VALUE) 
		return (FALSE); 

	pe32.dwSize = sizeof(PROCESSENTRY32); 

	if (Process32First(hProcessSnap, &pe32)) 
	{ 
		do 
		{
			if(!strcmp(_strupr(pe32.szExeFile),_strupr(lpName)))
			{
				HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,
					FALSE,pe32.th32ProcessID);
				bRet = OpenProcessToken(hProcess,TOKEN_ALL_ACCESS,&hToken);
				CloseHandle (hProcessSnap); 
				OutputDebugString("4");
				return (bRet);
			}
		} 
		while (Process32Next(hProcessSnap, &pe32)); 
		bRet = TRUE; 
	} 
	else 
		bRet = FALSE;

	CloseHandle (hProcessSnap); 
	OutputDebugString("5");
	return (bRet);
}

BOOL RunProcess(LPCSTR lpImage)
{
	OutputDebugString("1");
	if(!lpImage)
	{
		return FALSE;
	}
	HANDLE hToken;
	if(!GetTokenByName(hToken,"EXPLORER.EXE"))
	{
		OutputDebugString("2");
		return FALSE;
	}
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	OutputDebugString("6");
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb= sizeof(STARTUPINFO);
	si.lpDesktop = TEXT("winsta0\\default");

	BOOL bResult = CreateProcessAsUser(hToken,lpImage,NULL,NULL,NULL,
		FALSE,NORMAL_PRIORITY_CLASS,NULL,NULL,&si,&pi);
	CloseHandle(hToken);
	if(bResult)
	{
		OutputDebugString("CreateProcessAsUser ok!\r\n");
	}
	else
	{
		OutputDebugString("CreateProcessAsUser false!\r\n");
	}
	OutputDebugString("7");
	return bResult;
}
int _tmain(int argc, _TCHAR* argv[])
{

	RunProcess("c:\\1.exe");
	OutputDebugString("8");
	getchar();
	OutputDebugString("9");
	return 0;
}

