
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
#define MAX_PATH          260			//�ļ�������������󳤶�
#define MAX_BUFSIZE		  63*512		//��ȡ���̵Ļ�������С(һ���ŵ������� * ������С)
typedef struct _FIND_RESULT{
	BOOL bFolder;						// TRUE Ϊ�ļ���, FALSE Ϊ�ļ�
	union{
		char szFileName[MAX_PATH + 2];		
		struct{
			BOOL bEmpty;				// TRUE ��ʾĿ¼Ϊ��
			char szFilePath[MAX_PATH + 2];
		};		
	};
}FIND_RESULT, *PFIND_RESULT, *LPFIND_RESULT;

class Clear_File
{
public:
	Clear_File();
	ULONGLONG m_ullTotalFileSize; // Ҫɾ�����ܵ��ļ���С
	ULONGLONG m_ullFinishFileSize; // �Ѿ�ɾ�����ļ����ܴ�С

	LPBYTE m_pFillData;	// ������ݻ�����
	DWORD m_nBlockSize;		// �ֿ��С

	void EraseFileObject(FIND_RESULT stFindResult);
	BOOL EraseFileData(LPCSTR lpFileName);
	BOOL EraseFileObjectName(LPCSTR lpFileName, LPSTR lpNewFileName);


};

#endif // !defined(AFX_BRUSH_H__F8C29635_E8C9_46AF_99A3_2A8743545380__INCLUDED_)
