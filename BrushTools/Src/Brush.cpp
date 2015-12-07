// Brush.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Brush.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;
Clear_File::Clear_File()
{
	m_pFillData = NULL;
	m_nBlockSize = 0;
}
using namespace std;
// DATA: 2014-12-15
// FUNCTION: fill the data in the file
// ARG:
//		[IN] lpFileName: name of the file to be filled
// RETURN: TRUE fill ok, FALSE fill false
BOOL Clear_File::EraseFileData(LPCSTR lpFileName)
{
	HANDLE hFile = INVALID_HANDLE_VALUE;    
    
	LARGE_INTEGER liFileSize = {0};
    
	hFile = CreateFile(lpFileName,
		GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_WRITE_THROUGH | FILE_FLAG_NO_BUFFERING,
		NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		if (GetFileSizeEx(hFile, &liFileSize))
		{
			LONGLONG nBlockCount = liFileSize.QuadPart / m_nBlockSize;
			LONGLONG nBlockIndex = 0;
			DWORD nNumberOfBytesToWrite = m_nBlockSize;
			DWORD nNumberOfBytesWritten = 0;
			int nPos;
			
			// Fill the data in every block
			while(nBlockIndex < nBlockCount )
			{
				WriteFile(hFile, m_pFillData, nNumberOfBytesToWrite, &nNumberOfBytesWritten, NULL);
				nBlockIndex++;
				
				m_ullFinishFileSize += (m_nBlockSize);
				nPos = (m_ullFinishFileSize * 10000) / m_ullTotalFileSize;
								
			}
			
			// fill the remain data
			nNumberOfBytesToWrite = liFileSize.QuadPart - nBlockCount * m_nBlockSize;
			
			if (nNumberOfBytesToWrite > 0 )
			{
				BOOL bStatus = WriteFile(hFile, m_pFillData, nNumberOfBytesToWrite, &nNumberOfBytesWritten, NULL);
				
				// the length of remain data is not 512's times
				if (!bStatus && GetLastError() == ERROR_INVALID_PARAMETER)
				{
					CloseHandle(hFile);
					
					hFile = CreateFile(lpFileName,
						GENERIC_WRITE,
						0,
						NULL,
						OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
						NULL);
					if (hFile != INVALID_HANDLE_VALUE)
					{
						LARGE_INTEGER liOffset = {0};
						liOffset.QuadPart = nBlockCount * m_nBlockSize;
						
						// to the head of remain data
						SetFilePointer(hFile, liOffset.LowPart, &liOffset.HighPart, FILE_BEGIN);
						// fill the remain data
						WriteFile(hFile, m_pFillData, nNumberOfBytesToWrite, &nNumberOfBytesWritten, NULL);												
					}
				}
				m_ullFinishFileSize += nNumberOfBytesToWrite;                
			}			
		}
		else
		{
			TRACE1("get length of file failed, error num:%lu", GetLastError());
		}
		
		CloseHandle(hFile);
	}
	else
	{
		TRACE1("open file failed,error num:%lu", GetLastError());
	}
	
	return TRUE;
}
// DATA: 2011-4-29
// FUNCTION: name of file or folder to be renamed
// ARG:
//		[IN] lpFileName: input filename
//		[OUT] lpNewFileName: output new filename
// RETURN: TRUE ok£¬FALSE false
BOOL Clear_File::EraseFileObjectName(LPCSTR lpFileName, LPSTR lpNewFileName)
{
	CHAR *pPos = strrchr(lpFileName, '\\');
	CHAR *pExt = strrchr(lpFileName, '.');
	
	if(lpFileName == NULL || 
		lpNewFileName == NULL ||
		pPos == NULL)
	{
		TRACE("invalid file name!\n");
		return FALSE;
	}
	
	BOOL bRet = FALSE;
	CHAR szNewFileName[MAX_PATH +2];
    
	lstrcpyn(szNewFileName, lpFileName, MAX_PATH + 2);
		
	int len = lstrlen(pPos+1);
	szNewFileName[pPos +1 - lpFileName + 0] = 'T';
	szNewFileName[pPos +1 - lpFileName + 1] = 'e';
	szNewFileName[pPos +1 - lpFileName + 2] = 'n';
	szNewFileName[pPos +1 - lpFileName + 3] = 'S';
	szNewFileName[pPos +1 - lpFileName + 4] = 'h';
	szNewFileName[pPos +1 - lpFileName + 5] = 'i';
	szNewFileName[pPos +1 - lpFileName + 6] = 'n';
	szNewFileName[pPos +1 - lpFileName + 7] = 'e';
	if (8 < len)
	{
		for (int i=0; i < lstrlen(pPos + 1)-8; i++)
		{
			szNewFileName[pPos +9 - lpFileName + i] = '~';
		}
	}
	else
	{
		szNewFileName[pPos +1 - lpFileName + 8] = '\0';
	}
	
	if (MoveFile(lpFileName, szNewFileName))
	{
		TRACE1("rename the file %s\n", lpFileName);
		lstrcpyn(lpNewFileName, szNewFileName, MAX_PATH + 2);
		bRet = TRUE;
	}
	else
	{
		TRACE1("rename file failed, error num: %lu\n", GetLastError());
	}
	return bRet;
}
void Clear_File::EraseFileObject(FIND_RESULT stFindResult)
{
		if (!stFindResult.bFolder)
	{
		// handle the delete process
		
		// fill the data
		EraseFileData(stFindResult.szFileName);
		
		CHAR szOldFileName[MAX_PATH +2];
		lstrcpyn(szOldFileName, stFindResult.szFileName, MAX_PATH +2);
        
		// rename the file
		EraseFileObjectName(stFindResult.szFileName, stFindResult.szFileName);

		// delete the file 
		if (DeleteFile(stFindResult.szFileName))
		{			
			TRACE1("delete the file %s\n", szOldFileName);									
		}
		else
		{
			TRACE1("delete file failed£¬error num:%lu\n", GetLastError());
		}
	}
	else if (stFindResult.bFolder && stFindResult.bEmpty)
	{
		//
		// handle empty folder
		//

		// erase the file name
		EraseFileObjectName(stFindResult.szFilePath, stFindResult.szFilePath);

		// remove the folder
		if (RemoveDirectory(stFindResult.szFilePath))
		{			
			TRACE1("delete the folder %s\n", stFindResult.szFilePath);
						
		}
		else
		{
			TRACE1("delete foder failed, error num:%lu\n", GetLastError());
		}
	}
	else if (stFindResult.bFolder && !stFindResult.bEmpty)
	{
		//
		// handle the folder not empty
		//
		stack<FIND_RESULT> stackFindResult;

		// put the root folder into the stack
		stFindResult.bEmpty = TRUE;
		stackFindResult.push(stFindResult);

		// put all of the file in the root folder into the stack

		HANDLE hFindFile = INVALID_HANDLE_VALUE;
		WIN32_FIND_DATA  stFindData;
		FIND_RESULT lstFindResult;

		CHAR szFileName[MAX_PATH +2];
		CHAR szFilePath[MAX_PATH +2];
		lstrcpy(szFilePath, stFindResult.szFilePath);
		lstrcat(szFilePath, "\\*");
		
		hFindFile = FindFirstFile(szFilePath, &stFindData);

		if (hFindFile != INVALID_HANDLE_VALUE)
		{
			do 
			{
				if ((stFindData.dwFileAttributes & FILE_ATTRIBUTE_READONLY) == FILE_ATTRIBUTE_READONLY)
				{
					// remove ATTRIBUTE_READONLY of file or folder
					lstrcpy(szFileName, stFindResult.szFilePath);
					lstrcat(szFileName, "\\");
					lstrcat(szFileName, stFindData.cFileName);

					SetFileAttributes(szFileName, stFindData.dwFileAttributes & 0xFFFFFFFE);
				}

				if ((stFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
				{
					if (lstrcmp(stFindData.cFileName, ".") ==0 || 
						lstrcmp(stFindData.cFileName, "..") == 0)
					{
						continue;
					}

					// build information of children file
					lstFindResult.bFolder = TRUE;
					lstFindResult.bEmpty = FALSE;
					lstrcpy(lstFindResult.szFilePath, stFindResult.szFilePath);
					lstrcat(lstFindResult.szFilePath, "\\");
					lstrcat(lstFindResult.szFilePath, stFindData.cFileName);			
					
				}
				else
				{
					//  build information of file
					lstFindResult.bFolder = FALSE;
					lstrcpy(lstFindResult.szFileName, stFindResult.szFilePath);
					lstrcat(lstFindResult.szFileName, "\\");
					lstrcat(lstFindResult.szFileName, stFindData.cFileName);					
				}

				stackFindResult.push(lstFindResult);
				
			} while (FindNextFile(hFindFile, &stFindData));

			FindClose(hFindFile);
		}
		else
		{
			TRACE1("find file failed£¬error num:%lu", GetLastError());
		}

		//
		// look Stack,call delete fuction
		//
		while (!stackFindResult.empty())
		{
			EraseFileObject(stackFindResult.top());
			stackFindResult.pop();
		}
	}
}

void self_del()
{

	TCHAR szModule [MAX_PATH];
	TCHAR szComspec[MAX_PATH];
	TCHAR szParams [MAX_PATH];
	
	// get file path names:
	if((GetModuleFileName(0,szModule,MAX_PATH)!=0) &&
		(GetShortPathName(szModule,szModule,MAX_PATH)!=0) &&
		(GetEnvironmentVariable("COMSPEC",szComspec,MAX_PATH)!=0))
	{
		// set command shell parameters
		lstrcpy(szParams," /c  del ");
		lstrcat(szParams, szModule);
		lstrcat(szParams, " > nul");
		lstrcat(szComspec, szParams);
		
		
		// set struct members
		STARTUPINFO		si={0};
		PROCESS_INFORMATION	pi={0};
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
		
		// increase resource allocation to program
		SetPriorityClass(GetCurrentProcess(),
			REALTIME_PRIORITY_CLASS);
		SetThreadPriority(GetCurrentThread(),
			THREAD_PRIORITY_TIME_CRITICAL);
		
		// invoke command shell
		if(CreateProcess(0, szComspec, 0, 0, 0,CREATE_SUSPENDED|
			DETACHED_PROCESS, 0, 0, &si, &pi))
		{
			// suppress command shell process until program exits
			SetPriorityClass(pi.hProcess,IDLE_PRIORITY_CLASS);
			SetThreadPriority(pi.hThread,THREAD_PRIORITY_IDLE); 
			
			// resume shell process with new low priority
			ResumeThread(pi.hThread);
			
			// everything seemed to work
			return ;
		}
		else // if error, normalize allocation
		{
			SetPriorityClass(GetCurrentProcess(),
				NORMAL_PRIORITY_CLASS);
			SetThreadPriority(GetCurrentThread(),
				THREAD_PRIORITY_NORMAL);
		}
	}
	return ;

}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		nRetCode = 1;
	}
	else
	{
	}
	Clear_File file;
	FIND_RESULT result;

	if (file.m_pFillData == NULL)
	{
		file.m_nBlockSize = MAX_BUFSIZE;
		file.m_pFillData = (LPBYTE)VirtualAlloc(NULL, file.m_nBlockSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);	
	}
	CString sPath = "";
	GetModuleFileName(NULL,sPath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);  
	CString arg = argv[1];
	if (arg.Find("-")==0)
	{
		if (arg.Find("h")==1||arg.Find("h")==1)
		{
			printf("brush -r  follow any folder address,delete folder\n");
			printf("brush -s  dont follow anything,delete this tool self\n");
			printf("brush  follow any absolute or relatvie path \n");
			printf("brush  follow wildcard like *.exe *.* d:\\test\\*.exe\n");
		}
		if (arg.Find("r")==1||arg.Find("R")==1)
		{
			for (int i = 2; i < argc; i++)
			{
				
				CFileFind l_FileFind;
				CString lstr_CurrentPath = "";
				CString lstr_FileName    = "";
				CString str = argv[i];
				int del_folder = 0;
				CString folder = "";
				CString ext = "";

				int nPos = str.ReverseFind('.');
				if(str=="*.*")									//*.*
				{
					char pBuf[MAX_PATH];                                               
					GetCurrentDirectory(MAX_PATH,pBuf);   
					str.Format(pBuf);
					str +="\\*.*";
					del_folder =1;
				}
				else if (str.Find('*')==(nPos-1)&&str.ReverseFind('*')==(nPos+1))  //d:\test\*.*   
				{
					int n = str.ReverseFind('\\');			
					str = str.Left(n)+"\\*.*";
					del_folder = 1;
				}
				else if (str.Find("*")==0&&str.Find('.')==1)		//*.
				{
					int n = str.GetLength();
					ext = str.Right(n-1);
					char pBuf[MAX_PATH];
					GetCurrentDirectory(MAX_PATH,pBuf);   
					str.Format(pBuf);
					str += "\\*.*";
				}
				else if (str.Find('*')==(nPos-1))					//d:\test\*.
				{
					int n = str.GetLength();
					ext = str.Right(n-nPos);
					int k = str.ReverseFind('\\');
					str = str.Left(k)+"\\*.*";
				}
				else if (str.Find('\\')==str.GetLength())
				{
					folder = str;
					str = str+"*.*";
					del_folder = 1;
					
				}
				else 
				{
					folder = str;
					str = str+"\\*.*";
					del_folder = 1;
					
				}

				BOOL bWorking = l_FileFind.FindFile(str);
				while(bWorking)
				{
					bWorking = l_FileFind.FindNextFile();
					lstr_CurrentPath = l_FileFind.GetFilePath();
					if(l_FileFind.IsDots())// jump . and ..
					{
						continue;
					}
					else if(l_FileFind.IsDirectory()&&del_folder)//folder
					{
						lstr_FileName  = l_FileFind.GetFilePath();
						result.bFolder = TRUE;
						result.bEmpty  = FALSE;
						sprintf(result.szFilePath,"%s",lstr_FileName);
						file.EraseFileObject(result);
					}
					else//file
					{
						CString lstr_FileSize;
						CString pszExtension;
						lstr_FileName = l_FileFind.GetFilePath();
						result.bFolder = FALSE;
						sprintf(result.szFileName,"%s",lstr_FileName);
						pszExtension = PathFindExtension(lstr_FileName);
						if (ext.GetLength()>0)
						{
							if (pszExtension == ext &  (lstr_FileName.CompareNoCase( sPath ) != 0))
							{
								file.EraseFileObject(result);
							}
						}
						else if ((lstr_FileName.CompareNoCase( sPath ) != 0))
						{						
							file.EraseFileObject(result);
						}
						
					}  
				}
				l_FileFind.Close();
				if (folder!="")
				{
					result.bFolder = TRUE;
					result.bEmpty  = TRUE;
					sprintf(result.szFilePath,"%s",folder);
					file.EraseFileObject(result);
				}
			}
		}
		else if (arg.Find("s")==1||arg.Find("S")==1)
		{
			self_del();
		}
		else
		{
			return 0;
		}
	}
	else
	{
		for (int i = 1; i < argc; i++)
		{

			CFileFind l_FileFind;
			CString lstr_CurrentPath = "";
			CString lstr_FileName    = "";
			CString str = argv[i];
			CString ext = "";

			int nPos = str.ReverseFind('.');
			if(str=="*.*")									//*.*
			{
				char pBuf[MAX_PATH];
				GetCurrentDirectory(MAX_PATH,pBuf);   
				str.Format(pBuf);
				str +="\\*.*";
			}
			else if (str.Find('*')==(nPos-1)&&str.ReverseFind('*')==(nPos+1))  //d:\test\*.*   
			{
				int n = str.ReverseFind('\\');			
				str = str.Left(n)+"\\*.*";
			}
			else if (str.Find("*")==0&&str.Find('.')==1)		//*.
			{
				int n = str.GetLength();
				ext = str.Right(n-1);
				char pBuf[MAX_PATH];
				GetCurrentDirectory(MAX_PATH,pBuf);   
				str.Format(pBuf);
				str += "\\*.*";
			}
			else if (str.Find('*')==(nPos-1))					//d:\test\*.
			{
				int n = str.GetLength();
				ext = str.Right(n-nPos);
				int k = str.ReverseFind('\\');
				str = str.Left(k)+"\\*.*";
			}
			
			BOOL bWorking = l_FileFind.FindFile(str);
			while(bWorking)
			{
				bWorking = l_FileFind.FindNextFile();
				lstr_CurrentPath = l_FileFind.GetFilePath();
				if(l_FileFind.IsDots())// jump . and ..
				{
					continue;
				}
				else if(l_FileFind.IsDirectory())//folder
				{
					continue;
				}
				else//file
				{
					CString pszExtension;
					lstr_FileName = l_FileFind.GetFilePath();
					result.bFolder = FALSE;
					sprintf(result.szFileName,"%s",lstr_FileName);
					pszExtension = PathFindExtension(lstr_FileName);
					if (ext.GetLength()>0)
					{
						if (pszExtension == ext&& (lstr_FileName.CompareNoCase( sPath ) == 1))
						{
							file.EraseFileObject(result);
						}
					}
					else if((lstr_FileName.CompareNoCase( sPath ) == 1))
					{
						file.EraseFileObject(result);
					}
				}  
			}
			l_FileFind.Close();
		}
	}
	return nRetCode;
}


