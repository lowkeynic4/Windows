// service.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
void Init();
BOOL IsInstalled();
BOOL Install();
BOOL Uninstall();
void LogEvent(LPCTSTR pszFormat, ...);
void WINAPI ServiceMain();
void WINAPI ServiceStrl(DWORD dwOpcode);
 
TCHAR szServiceName[] = _T("tenshine");
BOOL bInstall;
SERVICE_STATUS_HANDLE hServiceStatus;
SERVICE_STATUS status;
DWORD dwThreadID;
 
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    Init();
 
    dwThreadID = ::GetCurrentThreadId();
 
    SERVICE_TABLE_ENTRY st[] =
    {
        { szServiceName, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
        { NULL, NULL }
    };
 
    if (stricmp(lpCmdLine, "/install") == 0)
        {
           Install();
        }
    else if (stricmp(lpCmdLine, "/uninstall") == 0)
        {
            Uninstall();
        }
    else
        {
         if (!::StartServiceCtrlDispatcher(st))
        {
            LogEvent(_T("Register Service Main Function Error!"));
        }
      }
 
    return 0;
}
//*********************************************************
//Functiopn:            Init
//Description:          初始化
//Calls:                main       
//          <author>niying <time>2006-8-10      <version>     <desc>
//*********************************************************
void Init()
{
    hServiceStatus = NULL;
    status.dwServiceType = SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS;
    status.dwCurrentState = SERVICE_START_PENDING;
    status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    status.dwWin32ExitCode = 0;
    status.dwServiceSpecificExitCode = 0;
    status.dwCheckPoint = 0;
    status.dwWaitHint = 0;
}
 
//*********************************************************
//Functiopn:            ServiceMain
//Description:          服务主函数，这在里进行控制对服务控制的注册
//          <author>niying <time>2006-8-10      <version>     <desc>
//*********************************************************
void WINAPI ServiceMain()
{
    // Register the control request handler
    status.dwCurrentState = SERVICE_START_PENDING;
    status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
 
    //注册服务控制
    hServiceStatus = RegisterServiceCtrlHandler(szServiceName, ServiceStrl);
    if (hServiceStatus == NULL)
    {
        LogEvent(_T("Handler not installed"));
        return;
    }
    SetServiceStatus(hServiceStatus, &status);
 
    status.dwWin32ExitCode = S_OK;
    status.dwCheckPoint = 0;
    status.dwWaitHint = 0;
    status.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(hServiceStatus, &status);
 
    //模拟服务的运行。应用时将主要任务放于此即可
        //可在此写上服务需要执行的代码，一般为死循环
      while(1)
    {
           //循环干什么
     }   
    status.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(hServiceStatus, &status);
    OutputDebugString(_T("Service stopped"));
}
 
//*********************************************************
//Functiopn:            ServiceStrl
//Description:          服务控制主函数，这里实现对服务的控制，
//                      当在服务管理器上停止或其它操作时，将会运行此处代码
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:                dwOpcode：控制服务的状态
//Output:
//Return:
//Others:
//History:
//          <author>niying <time>2006-8-10      <version>     <desc>
//*********************************************************
void WINAPI ServiceStrl(DWORD dwOpcode)
{
    switch (dwOpcode)
    {
    case SERVICE_CONTROL_STOP:
        status.dwCurrentState = SERVICE_STOP_PENDING;
        SetServiceStatus(hServiceStatus, &status);
        PostThreadMessage(dwThreadID, WM_CLOSE, 0, 0);
        break;
    case SERVICE_CONTROL_PAUSE:
        break;
    case SERVICE_CONTROL_CONTINUE:
        break;
    case SERVICE_CONTROL_INTERROGATE:
        break;
    case SERVICE_CONTROL_SHUTDOWN:
        break;
    default:
        LogEvent(_T("Bad service request"));
        OutputDebugString(_T("Bad service request"));
    }
}
//*********************************************************
//Functiopn:            IsInstalled
//Description:          判断服务是否已经被安装
//          <author>niying <time>2006-8-10      <version>     <desc>
//*********************************************************
BOOL IsInstalled()
{
    BOOL bResult = FALSE;
 
    //打开服务控制管理器
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
 
    if (hSCM != NULL)
    {
        //打开服务
        SC_HANDLE hService = ::OpenService(hSCM, szServiceName, SERVICE_QUERY_CONFIG);
        if (hService != NULL)
        {
            bResult = TRUE;
            ::CloseServiceHandle(hService);
        }  
        ::CloseServiceHandle(hSCM);
    }
    return bResult;
}
 
//*********************************************************
//Functiopn:            Install
//Description:          安装服务函数
//          <author>niying <time>2006-8-10      <version>     <desc>
//*********************************************************
BOOL Install()
{
    if (IsInstalled())
        return TRUE;
 
    //打开服务控制管理器
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCM == NULL)
    {
        MessageBox(NULL, _T("Couldn't open service manager"), szServiceName, MB_OK);
        return FALSE;
    }
 
    // Get the executable file path
    TCHAR szFilePath[MAX_PATH];
    ::GetModuleFileName(NULL, szFilePath, MAX_PATH);
 
    //创建服务
    SC_HANDLE hService = ::CreateService(hSCM, szServiceName, szServiceName,
        SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS ,SERVICE_AUTO_START , SERVICE_ERROR_NORMAL,
        szFilePath, NULL, NULL, _T(""), NULL, NULL);
 
    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
        MessageBox(NULL, _T("Couldn't create service"), szServiceName, MB_OK);
        return FALSE;
    }
 
    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);
    return TRUE;
}
 
//*********************************************************
//Functiopn:            Uninstall
//Description:          删除服务函数
//          <author>niying <time>2006-8-10      <version>     <desc>
//*********************************************************
BOOL Uninstall()
{
    if (!IsInstalled())
        return TRUE;
 
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
 
    if (hSCM == NULL)
    {
        MessageBox(NULL, _T("Couldn't open service manager"), szServiceName, MB_OK);
        return FALSE;
    }
 
    SC_HANDLE hService = ::OpenService(hSCM, szServiceName, SERVICE_STOP | DELETE);
 
    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
        MessageBox(NULL, _T("Couldn't open service"), szServiceName, MB_OK);
        return FALSE;
    }
    SERVICE_STATUS status;
    ::ControlService(hService, SERVICE_CONTROL_STOP, &status);
 
    //删除服务
    BOOL bDelete = ::DeleteService(hService);
    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);
 
    if (bDelete)
        return TRUE;
 
    LogEvent(_T("Service could not be deleted"));
    return FALSE;
}
 
//*********************************************************
//Functiopn:            LogEvent
//Description:          记录服务事件
//          <author>niying <time>2006-8-10      <version>     <desc>
//*********************************************************
void LogEvent(LPCTSTR pFormat, ...)
{
    TCHAR    chMsg[256];
    HANDLE  hEventSource;
    LPTSTR  lpszStrings[1];
    va_list pArg;
 
    va_start(pArg, pFormat);
    _vstprintf(chMsg, pFormat, pArg);
    va_end(pArg);
 
    lpszStrings[0] = chMsg;
     
    hEventSource = RegisterEventSource(NULL, szServiceName);
    if (hEventSource != NULL)
    {
        ReportEvent(hEventSource, EVENTLOG_INFORMATION_TYPE, 0, 0, NULL, 1, 0, (LPCTSTR*) &lpszStrings[0], NULL);
        DeregisterEventSource(hEventSource);
    }
}
