
#if !defined(AFX_BRUSH_H__F8C29635_E8C9_46AF_99A3_2A8743545380__INCLUDED_)
#define AFX_BRUSH_H__F8C29635_E8C9_46AF_99A3_2A8743545380__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include <WINDOWS.H>
#include <Shlwapi.h>
#include <stack>
#pragma comment(lib,"Shlwapi.lib")
#define MAX_PATH          260			//文件名存放数组的最大长度
#define MAX_BUFSIZE		  63*512		//读取磁盘的缓冲区大小(一个磁道扇区数 * 扇区大小)
typedef struct _FIND_RESULT{
	BOOL bFolder;						// TRUE 为文件夹, FALSE 为文件
	union{
		char szFileName[MAX_PATH + 2];		
		struct{
			BOOL bEmpty;				// TRUE 表示目录为空
			char szFilePath[MAX_PATH + 2];
		};		
	};
}FIND_RESULT, *PFIND_RESULT, *LPFIND_RESULT;

class Clear_File
{
public:
	Clear_File();
	ULONGLONG m_ullTotalFileSize; // 要删除的总的文件大小
	ULONGLONG m_ullFinishFileSize; // 已经删除的文件的总大小

	LPBYTE m_pFillData;	// 填充数据缓存区
	DWORD m_nBlockSize;		// 分块大小

	void EraseFileObject(FIND_RESULT stFindResult);
	BOOL EraseFileData(LPCSTR lpFileName);
	BOOL EraseFileObjectName(LPCSTR lpFileName, LPSTR lpNewFileName);


};

#endif // !defined(AFX_BRUSH_H__F8C29635_E8C9_46AF_99A3_2A8743545380__INCLUDED_)
