//////////////////////////////////////////////////////////////////////////
/*
˵����
�������н���PE��Ϣ��������е��˻ῴ����Ϥ���ǽ���� :)���������޸ĺͲ���,�ڴ˱�ʾ��л~
��������Ϊ���˶�����ɣ��������ģ��LoadPE��ѧϰ֮������ţ��Ц��
���ߣ���Ĭ
���䣺haimohk@163.com
*/
//////////////////////////////////////////////////////////////////////////


#include <tchar.h>
#include <Windows.h>
#include <commctrl.h>
#include <WindowsX.h>
#include <Richedit.h>
#include <Tlhelp32.h>
#include <strsafe.h>    //for  StringCchPrintf...
#include <assert.h>
#include "resource.h"
#include "LPEFile.h"
#include "LPESection.h"
#include "LPEImport.h"
#include "LPEExport.h"
#include "LPEResource.h"
#include "LPERelocate.h"

#pragma comment(lib, "comctl32")

#define  MYWM_MSG (WM_USER+100)

typedef struct _ResData{
	DWORD lpRes;
	DWORD lpResEntry;
}ResData,*pResData;

enum tmPOS{
	HOUR =0,
	MINUTE,
	SECOND	
};


tmPOS  tmPos;       //��¼EDIT �����ݱ�ѡ�е�λ��

HINSTANCE g_hInstance;
HWND g_hWnd;
HIMAGELIST  g_hImageList;            //ͼ���б�
LONG lpOldEditProc;
LONG lpOldSpinProc;
LONG lpEditProc;

TCHAR szPeFilePath[MAX_PATH]={0};
TCHAR szFilter[]=_T("PE Files(*.exe;*.dll;*.sys)\0*.exe;*.dll;*.sys\0All Files(*.*)\0*.*\0\0");
TCHAR szDefExt[]=_T("exe");
TCHAR szOpenCaption[]=_T("��ѡ��Ҫ�򿪵��ļ�");

void MaskTimeConvert(SYSTEMTIME& stTime,DWORD& dwMask,BOOL Mask2Time = TRUE);
LRESULT CALLBACK NewEditProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK NewSpinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK NewHexEditProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
void FreeData(HWND hTree);
HTREEITEM TreeAddChildItem(HWND hTree,HTREEITEM hParent,DWORD lpRes,
	PIMAGE_RESOURCE_DIRECTORY_ENTRY lpResEntry,DWORD dwLevel);
void FormatBytes(TCHAR szBuf[10],TCHAR szBytes[35]);
void CheckHexInput(HWND hEdit);
BOOL EnableDebugPrivilege(BOOL bEnable);
BOOL EnumProcess(HWND hwnd);
BOOL EnumModules(HWND hwnd);
BOOL Dlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
BOOL Dlg_OnCommand(HWND hwnd, WPARAM wParam,  LPARAM lParam) ;
BOOL Dlg_OnNotify(HWND hwnd, WPARAM wParam,  LPARAM lParam);
BOOL Dlg_OnPeImportNotify(HWND hwnd, WPARAM wParam,  LPARAM lParam,LPEImport &PeImport);
BOOL Dlg_OnPeReloctNotify(HWND hwnd, WPARAM wParam,  LPARAM lParam,LPERelocate &PeRelocate);
BOOL Dlg_OnInitPeSectionDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
BOOL Dlg_OnInitPeDirDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
BOOL Dlg_OnInitPeImportDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
BOOL Dlg_OnInitPeExportDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
BOOL Dlg_OnInitPeResDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
BOOL Dlg_OnInitPeRelocDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
BOOL Dlg_OnInitPeStructureDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
BOOL CALLBACK Dlg_Proc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Dlg_AboutProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Dlg_PeProc(HWND hwndDlg, UINT uMsg,  WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Dlg_PeStructure(HWND hwndDlg, UINT uMsg,  WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Dlg_PeSectionProc(HWND hwndDlg, UINT uMsg,  WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Dlg_PeDirProc(HWND hwndDlg, UINT uMsg,  WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Dlg_PeCalcProc(HWND hwndDlg, UINT uMsg,  WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Dlg_PeDatetimeProc(HWND hwndDlg, UINT uMsg,  WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Dlg_PeImportProc(HWND hwndDlg, UINT uMsg,  WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Dlg_PeExportProc(HWND hwndDlg, UINT uMsg,  WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Dlg_PeResourceProc(HWND hwndDlg, UINT uMsg,  WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Dlg_PeRelocateProc(HWND hwndDlg, UINT uMsg,  WPARAM wParam, LPARAM lParam);

//16����Edit���ڹ���
LRESULT CALLBACK NewHexEditProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	TCHAR ch;
	switch (uMsg)
	{
	case WM_CHAR:
		ch =(TCHAR)wParam;
		if (!( ((ch >=_T('0')) && (ch <=_T('9')))  || ((ch>=_T('a')) && (ch<=_T('f')))  ||  ((ch>=_T('A')) && (ch<=_T('F'))) ))
			return TRUE;
		break;
	default:
		break;
	}
	return CallWindowProc((WNDPROC)lpEditProc,hwnd,uMsg,wParam,lParam);
}

//ʱ��ת��
void MaskTimeConvert(SYSTEMTIME& stTime,DWORD& dwMask,BOOL Mask2Time)
{
	// GMTʱ��1970��1��1��
	SYSTEMTIME sysTime1970;
	ZeroMemory(&sysTime1970, sizeof(SYSTEMTIME));
	sysTime1970.wYear		= 1970;
	sysTime1970.wMonth		= 1;
	sysTime1970.wDay		= 1;


	LARGE_INTEGER li;
	li.QuadPart = 0;
	// Ĭ��Ϊ���뵽���ڵ�ת��
	if (Mask2Time)
	{
		// תΪ�ļ�ʱ��(100����Ϊ��λ)
		FILETIME fTime1970;
		SystemTimeToFileTime(&sysTime1970, &fTime1970);
		// ������ת��Ϊ100����ĵ�λ
		li.LowPart = dwMask;
		li.QuadPart *= 1000 * 1000 * 10;
		li.LowPart += fTime1970.dwLowDateTime;
		li.HighPart += fTime1970.dwHighDateTime;
		// ������ת���ļ�ʱ��
		FILETIME fTime;
		fTime.dwLowDateTime = li.LowPart;
		fTime.dwHighDateTime = li.HighPart;
		// ���ļ�ʱ��ת��ΪGMTʱ��
		FileTimeToSystemTime(&fTime, &stTime);

		return ;
	}
	else//Time2Mask
	{
		// תΪ�ļ�ʱ��(100����Ϊ��λ)
		FILETIME fTime1970;
		SystemTimeToFileTime(&sysTime1970, &fTime1970);
		// ������ʱ��ת��Ϊ�ļ�ʱ��
		FILETIME fTimeNow;
		SystemTimeToFileTime(&stTime, &fTimeNow);
		// ʱ���ֵ����
		fTimeNow.dwLowDateTime -= fTime1970.dwLowDateTime;
		fTimeNow.dwHighDateTime -= fTime1970.dwHighDateTime;
		// ���ļ�ʱ��ת��Ϊ����
		li.LowPart = fTimeNow.dwLowDateTime;
		li.HighPart = fTimeNow.dwHighDateTime;
		li.QuadPart /= (1000 * 1000 * 10);
		dwMask = li.LowPart;
		return ;
	}
}

//��ʽ���ֽ�
void FormatBytes(UCHAR szBuf[10],TCHAR szBytes[35])
{
	TCHAR szByte[4]={0};
	for (int i=0; i<10; i++)
	{
		StringCchPrintf(szByte,sizeof(szByte),_T("%02x%c"),szBuf[i],(char)32);
		int nIdx=lstrlen(szBytes);
		for (int j=0; j<3; j++)
		{
			szBytes[nIdx+j]=szByte[j];
		}
	}
}

//Edit 16��������
void CheckHexInput(HWND hEdit)
{
	TCHAR szText[10]={0};
	BOOL bChanged=FALSE;
	int n=GetWindowTextLength(hEdit);
	if (n==0)
	{
		return;
	}
	GetWindowText(hEdit,szText,sizeof(szText));
	for (int i=0; i<n; i++)
	{
		if (!(((szText[i] >=_T('0')) && (szText[i] <=_T('9'))) || ((szText[i]>=_T('A')) && (szText[i]<=_T('F')))))
		{
			int j;
			bChanged =TRUE;
			for (j=i; j<=n-1;j++)
			{
				szText[j]=szText[j+1];
			}
			szText[j+1]=_T('\0');
		}
	}
	if (bChanged)
	{
		SetWindowText(hEdit,szText);
		SendMessage(hEdit,EM_SETSEL,n-1,n-1);
	}

}

//��������Ȩ��
BOOL EnableDebugPrivilege(BOOL bEnable)
{
	BOOL status=FALSE;
	HANDLE hToken;
	//�򿪵�ǰ���̵ķ�������
	if(OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hToken))
	{
		LUID uID;
		LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&uID);
		//������Ȩ����
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount =1;
		tp.Privileges[0].Luid=uID;
		tp.Privileges[0].Attributes=bEnable?SE_PRIVILEGE_ENABLED:0;
		AdjustTokenPrivileges(hToken,FALSE,&tp,sizeof(tp),NULL,NULL);
		status =(GetLastError() ==ERROR_SUCCESS);
		CloseHandle(hToken);
	}
	return status;
}

//ö�ٽ���
BOOL EnumProcess(HWND hwnd)
{
	HWND hProLV =GetDlgItem(hwnd,IDC_LIST_PROCESS);
	LVITEM lvi;
	DWORD dwIdx=0;
	TCHAR szPath[MAX_PATH];         //����·��
	TCHAR szPID[10];                        //PID
	TCHAR szBaseAddr[10];               //�����ַ
	TCHAR szBaseSize[10];                //�����С
	HANDLE hProSnapshot =NULL;  //���̿��վ��
	HANDLE hModSnapshot=NULL; //ģ����վ��
	PROCESSENTRY32 pe32={0};
	MODULEENTRY32 me32 = {0};

	SHFILEINFO fi;
	int nImgIdx=0;
	HICON hIcon1=LoadIcon(g_hInstance,MAKEINTRESOURCE(IDI_ICON_SYS));
	g_hImageList=ImageList_Create(16,16,TRUE|ILC_COLOR32,30,20);
	assert(g_hImageList!=NULL);
	ImageList_AddIcon(g_hImageList,hIcon1);
	//���
	ListView_SetImageList(hProLV,g_hImageList,LVSIL_SMALL);
	ListView_DeleteAllItems(hProLV);

	//�������̿���
	hProSnapshot =CreateToolhelp32Snapshot(TH32CS_SNAPALL,0); 
	if(hProSnapshot==INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	pe32.dwSize =sizeof(pe32);
	ZeroMemory(&lvi,sizeof(lvi));
	lvi.mask		= LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
	lvi.state		= 0;
	lvi.stateMask	= 0;
	//ö�ٽ���
	BOOL fOk =Process32First(hProSnapshot,&pe32);
	for (; fOk; fOk =Process32Next(hProSnapshot,&pe32),dwIdx++)
	{
		StringCchPrintf(szPID,sizeof(szPID),_T("%08x"),pe32.th32ProcessID);

		hModSnapshot =CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,pe32.th32ProcessID);
		if (hModSnapshot !=INVALID_HANDLE_VALUE)
		{
			ZeroMemory(&me32,sizeof(me32));
			me32.dwSize = sizeof(me32);
			if(Module32First(hModSnapshot, &me32) && pe32.th32ProcessID !=0)
			{
				StringCchPrintf(szBaseAddr,sizeof(szBaseAddr),_T("%08x"),me32.modBaseAddr);
				StringCchPrintf(szBaseSize,sizeof(szBaseSize),_T("%08x"),me32.modBaseSize);
				StringCchPrintf(szPath,sizeof(szPath),_T("%s"),me32.szExePath);
				if(SHGetFileInfo(szPath,0,&fi,sizeof(fi),SHGFI_ICON|SHGFI_SYSICONINDEX|SHGFI_SMALLICON))
				{
					nImgIdx =ImageList_AddIcon(g_hImageList,fi.hIcon);
				}else{
					nImgIdx =0;
				}
			}else{
				StringCchPrintf(szBaseAddr,sizeof(szBaseAddr),_T("%08x"),0);
				StringCchPrintf(szBaseSize,sizeof(szBaseSize),_T("%08x"),0);
				StringCchPrintf(szPath,sizeof(szPath),_T("%s"),pe32.szExeFile);
				nImgIdx =0;
			}
		}else{
			StringCchPrintf(szBaseAddr,sizeof(szBaseAddr),_T("%08x"),0);
			StringCchPrintf(szBaseSize,sizeof(szBaseSize),_T("%08x"),0);
			StringCchPrintf(szPath,sizeof(szPath),_T("%s"),pe32.szExeFile);
			nImgIdx =0;
		}
		lvi.pszText =szPath;
		lvi.cchTextMax=MAX_PATH;
		lvi.iImage =nImgIdx;
		lvi.iItem =dwIdx;
		ListView_InsertItem(hProLV,&lvi);
		ListView_SetItemText(hProLV,dwIdx,1,szPID);
		ListView_SetItemText(hProLV,dwIdx,2,szBaseAddr);
		ListView_SetItemText(hProLV,dwIdx,3,szBaseSize);
	}
	return TRUE;
}


//���Ի����ʼ��
BOOL Dlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{

	LVCOLUMN lvc;
	int i;

	//����ListView
	TCHAR *lpszProColNames[]={_T("·��"),_T("PID"),_T("�����ַ"),_T("�����С")};
	DWORD dwProColWidths[]={240,80,80,80};

	//ģ��ListView
	TCHAR *lpszModColNames[]={_T("·��"),_T("�����ַ"),_T("�����С")};
	DWORD dwModColWidths[]={240,80,80};

	EnableDebugPrivilege(TRUE);

	HWND hProLV=GetDlgItem(hwnd,IDC_LIST_PROCESS);
	HWND hModLV=GetDlgItem(hwnd,IDC_LIST_MODULE);

	//��ý��̵�ʵ����� ��(HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE)
	//���ó����ͼ�ꡢСͼ��
	SendMessage(hwnd,WM_SETICON,ICON_BIG,(LPARAM)
		LoadIcon(g_hInstance,MAKEINTRESOURCE(IDI_ICON_MAIN)));
	SendMessage(hwnd,WM_SETICON,ICON_SMALL,(LPARAM)
		LoadIcon(g_hInstance,MAKEINTRESOURCE(IDI_ICON_MAIN)));

	//����ListView���ڷ�� ---Report������ѡ��
	long dwLong=GetWindowLong(hProLV,GWL_STYLE);
	dwLong |=LVS_REPORT | LVS_SINGLESEL;
	SetWindowLong(hProLV,GWL_STYLE,dwLong);
	SetWindowLong(hModLV,GWL_STYLE,dwLong);

	//����ListView���͡�����ѡ�з�񡢱�ƽ
	ListView_SetExtendedListViewStyle(hProLV,/*LVS_EX_GRIDLINES |*/ LVS_EX_FULLROWSELECT | LVS_EX_FLATSB);
	ListView_SetExtendedListViewStyle(hModLV,/*LVS_EX_GRIDLINES |*/ LVS_EX_FULLROWSELECT | LVS_EX_FLATSB);

	ZeroMemory(&lvc,sizeof(lvc));
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt =LVCFMT_LEFT;

	//����ListView������
	for (i=0;i<4;i++)
	{
		lvc.cx =dwProColWidths[i];
		lvc.pszText =lpszProColNames[i];
		lvc.iSubItem	= i;
		if (ListView_InsertColumn(hProLV, i, &lvc) == -1)
		{
			return FALSE;
		}
	}	

	//ģ��ListView������
	for(i=0;i<3;i++)
	{
		lvc.cx =dwModColWidths[i];
		lvc.pszText =lpszModColNames[i];
		lvc.iSubItem	= i;
		if (ListView_InsertColumn(hModLV, i, &lvc) == -1)
		{
			return FALSE;
		}
	}
	EnumProcess(hwnd);
	return TRUE;
}

//About�Ի��򴰿ڹ���
BOOL CALLBACK Dlg_AboutProc(
	HWND hwndDlg, 
	UINT uMsg, 
	WPARAM wParam, 
	LPARAM lParam
	)
{
	switch(uMsg)
	{
	case WM_CLOSE:
		EndDialog(hwndDlg,0);
		return TRUE;
	default:
		break;
	}
	return FALSE;
}

//Pe Section �Ի����ʼ��
BOOL Dlg_OnInitPeSectionDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	LPEFile *pPeFile=(LPEFile*)lParam;

	LVCOLUMN lvc;
	LVITEM lvi;
	TCHAR szTemp[10]={0};

	//ListView
	TCHAR *lpszColNames[]={_T("����"),_T("VOffset"),_T("VSize"),_T("ROffset"),_T("RSize"),_T("��־")};
	DWORD dwColWidths[]={75,75,75,75,75,75};

	//����ListView���ڷ�� ---Report������ѡ��
	HWND hWndLV=GetDlgItem(hwnd,IDC_LIST_SECTION);
	long dwLong=GetWindowLong(hWndLV,GWL_STYLE);
	dwLong |=LVS_REPORT | LVS_SINGLESEL;
	SetWindowLong(hWndLV,GWL_STYLE,dwLong);

	//����ListView���͡�����ѡ�з�񡢱�ƽ
	ListView_SetExtendedListViewStyle(hWndLV,/*LVS_EX_GRIDLINES |*/ LVS_EX_FULLROWSELECT | LVS_EX_FLATSB);

	ZeroMemory(&lvc,sizeof(lvc));
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt =LVCFMT_LEFT;

	for (int i=0;i<6;i++)
	{
		lvc.cx =dwColWidths[i];
		lvc.pszText =lpszColNames[i];
		lvc.iSubItem	= i;
		if (ListView_InsertColumn(hWndLV, i, &lvc) == -1)
		{
			return FALSE;
		}
	}	

	ZeroMemory(&lvi,sizeof(lvi));
	lvi.mask	= LVIF_TEXT | LVIF_STATE;
	lvi.state		= 0;
	lvi.stateMask	= 0;

	DWORD dwSectionCount=pPeFile->GetSectionCount();

	for (DWORD i=0; i<dwSectionCount; i++)
	{
		PIMAGE_SECTION_HEADER pTempSectionHeader=pPeFile->GetSection(i);
		if (pTempSectionHeader!=NULL)
		{
			StringCchPrintf(szTemp,sizeof(szTemp),_T("%s"),pTempSectionHeader->Name);
			lvi.iItem =i;
			lvi.pszText =szTemp;
			lvi.cchTextMax=sizeof(szTemp);
			ListView_InsertItem(hWndLV,&lvi);
			StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempSectionHeader->VirtualAddress);
			ListView_SetItemText(hWndLV,i,1,szTemp);
			StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempSectionHeader->Misc.VirtualSize);
			ListView_SetItemText(hWndLV,i,2,szTemp);
			StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempSectionHeader->PointerToRawData);
			ListView_SetItemText(hWndLV,i,3,szTemp);
			StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempSectionHeader->SizeOfRawData);
			ListView_SetItemText(hWndLV,i,4,szTemp);
			StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempSectionHeader->Characteristics);
			ListView_SetItemText(hWndLV,i,5,szTemp);
		}
	}
	return TRUE;
}

//Pe Section �Ի��򴰿ڹ���
BOOL CALLBACK Dlg_PeSectionProc(HWND hwndDlg, 
	UINT uMsg, 
	WPARAM wParam, 
	LPARAM lParam)
{

	switch(uMsg)
	{
	case WM_INITDIALOG:
		return Dlg_OnInitPeSectionDialog(hwndDlg,NULL,lParam);
	case WM_CLOSE:
		EndDialog(hwndDlg,0);
		return TRUE;
	default:
		break;
	}
	return FALSE;
}

//��ʼ������Ŀ¼�Ի���
BOOL Dlg_OnInitPeDirDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	LPEFile *pPeFile=(LPEFile*)lParam;
	TCHAR szTemp[10]={0};

	PIMAGE_DATA_DIRECTORY pTempDir=pPeFile->GetDataDirectory(IMAGE_DIRECTORY_ENTRY_EXPORT);  //�����
	if (pTempDir !=NULL)
	{
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->VirtualAddress);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_EXPORT_RVA),szTemp);
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->Size);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_EXPORT_SIZE),szTemp);
	}

	pTempDir=pPeFile->GetDataDirectory(IMAGE_DIRECTORY_ENTRY_IMPORT);   //�����
	if (pTempDir !=NULL)
	{
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->VirtualAddress);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_IMPORT_RVA),szTemp);
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->Size);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_IMPORT_SIZE),szTemp);
	}

	pTempDir=pPeFile->GetDataDirectory(IMAGE_DIRECTORY_ENTRY_RESOURCE);  //��Դ
	if (pTempDir !=NULL)
	{
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->VirtualAddress);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_RESOURCE_RVA),szTemp);
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->Size);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_RESOURCE_SIZE),szTemp);
	}
	pTempDir=pPeFile->GetDataDirectory(IMAGE_DIRECTORY_ENTRY_EXCEPTION);  //����
	if (pTempDir !=NULL)
	{
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->VirtualAddress);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_EXCEPTION_RVA),szTemp);
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->Size);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_EXCEPTION_SIZE),szTemp);
	}
	pTempDir=pPeFile->GetDataDirectory(IMAGE_DIRECTORY_ENTRY_SECURITY);  //��ȫ
	if (pTempDir !=NULL)
	{
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->VirtualAddress);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_SECURITY_RVA),szTemp);
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->Size);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_SECURITY_SIZE),szTemp);
	}
	pTempDir=pPeFile->GetDataDirectory(IMAGE_DIRECTORY_ENTRY_BASERELOC);  //�ض�λ
	if (pTempDir !=NULL)
	{
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->VirtualAddress);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_RELOCATE_RVA),szTemp);
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->Size);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_RELOCATE_SIZE),szTemp);
	}
	pTempDir=pPeFile->GetDataDirectory(IMAGE_DIRECTORY_ENTRY_DEBUG);  //����
	if (pTempDir !=NULL)
	{
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->VirtualAddress);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_DEBUG_RVA),szTemp);
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->Size);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_DEBUG_SIZE),szTemp);
	}
	pTempDir=pPeFile->GetDataDirectory(IMAGE_DIRECTORY_ENTRY_ARCHITECTURE);  //��Ȩ
	if (pTempDir !=NULL)
	{
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->VirtualAddress);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_COPYRIGHT_RVA),szTemp);
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->Size);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_COPYRIGHT_SIZE),szTemp);
	}
	pTempDir=pPeFile->GetDataDirectory(IMAGE_DIRECTORY_ENTRY_GLOBALPTR);  //ȫ��ָ��
	if (pTempDir !=NULL)
	{
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->VirtualAddress);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_GLOBALPTR_RVA),szTemp);
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->Size);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_GLOBALPTR_SIZE),szTemp);
	}
	pTempDir=pPeFile->GetDataDirectory(IMAGE_DIRECTORY_ENTRY_TLS);  //TLS
	if (pTempDir !=NULL)
	{
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->VirtualAddress);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_TLS_RVA),szTemp);
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->Size);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_TLS_SIZE),szTemp);
	}
	pTempDir=pPeFile->GetDataDirectory(IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG);  //��������
	if (pTempDir !=NULL)
	{
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->VirtualAddress);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_LOAD_CONFIG_RVA),szTemp);
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->Size);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_LOAD_CONFIG_SIZE),szTemp);
	}
	pTempDir=pPeFile->GetDataDirectory(IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT);  //���뷶Χ
	if (pTempDir !=NULL)
	{
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->VirtualAddress);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_BOUND_IMPORT_RVA),szTemp);
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->Size);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_BOUND_IMPORT_SIZE),szTemp);
	}
	pTempDir=pPeFile->GetDataDirectory(IMAGE_DIRECTORY_ENTRY_IAT);  //IAT
	if (pTempDir !=NULL)
	{
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->VirtualAddress);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_IAT_RVA),szTemp);
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->Size);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_IAT_SIZE),szTemp);
	}
	pTempDir=pPeFile->GetDataDirectory(IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT);  //�ӳ�����
	if (pTempDir !=NULL)
	{
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->VirtualAddress);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_DELAY_IMPORT_RVA),szTemp);
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->Size);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_DELAY_IMPORT_SIZE),szTemp);
	}
	pTempDir=pPeFile->GetDataDirectory(IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR);  //COM
	if (pTempDir !=NULL)
	{
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->VirtualAddress);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_COM_RVA),szTemp);
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->Size);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_COM_SIZE),szTemp);
	}
	pTempDir=pPeFile->GetDataDirectory(15);  //����
	if (pTempDir !=NULL)
	{
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->VirtualAddress);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_RESERVE_RVA),szTemp);
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDir->Size);
		SetWindowText(GetDlgItem(hwnd,IDC_EDIT_RESERVE_SIZE),szTemp);
	}

	return TRUE;
}

//PE Import�Ի����ʼ��
BOOL Dlg_OnInitPeImportDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	LPEImport PeImport;
	PeImport =*(LPEFile*)lParam;

	LVCOLUMN lvc;
	LVITEM lvi;
	TCHAR szTemp[40]={0};

	//DLL ListView
	TCHAR *lpszDllColNames[]={_T("DLL����"),_T("OriginalFirstThunk"),_T("����ʱ���־"),_T("ForwarderChain"),_T("����"),_T("FirstThunk")};
	DWORD dwDllColWidths[]={100,100,100,100,92,100};

	//Function ListView
	TCHAR *lpszFuncColNames[]={_T("ThunkRVA"),_T("Thunk ƫ��"),_T("Thunk ֵ"),_T("��ʾ"),_T("API����")};
	DWORD dwFuncColWidths[]={100,100,100,100,100};

	//����ListView���ڷ�� ---Report������ѡ��
	HWND hDllLV=GetDlgItem(hwnd,IDC_LIST_DLL);
	HWND hFuncLV=GetDlgItem(hwnd,IDC_LIST_IMPORT_FUNC);

	long dwLong=GetWindowLong(hDllLV,GWL_STYLE);
	dwLong |=LVS_REPORT | LVS_SINGLESEL;
	SetWindowLong(hDllLV,GWL_STYLE,dwLong);
	SetWindowLong(hFuncLV,GWL_STYLE,dwLong);

	//����ListView���͡�����ѡ�з�񡢱�ƽ
	ListView_SetExtendedListViewStyle(hDllLV,/*LVS_EX_GRIDLINES |*/ LVS_EX_FULLROWSELECT | LVS_EX_FLATSB);
	ListView_SetExtendedListViewStyle(hFuncLV,/*LVS_EX_GRIDLINES |*/ LVS_EX_FULLROWSELECT | LVS_EX_FLATSB);

	ZeroMemory(&lvc,sizeof(lvc));
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt =LVCFMT_LEFT;

	for (int i=0;i<6;i++)
	{
		lvc.cx =dwDllColWidths[i];
		lvc.pszText =lpszDllColNames[i];
		lvc.iSubItem	= i;
		if (ListView_InsertColumn(hDllLV, i, &lvc) == -1)
		{
			return FALSE;
		}
	}	

	for (int i=0;i<5;i++)
	{
		lvc.cx =dwFuncColWidths[i];
		lvc.pszText =lpszFuncColNames[i];
		lvc.iSubItem	= i;
		if (ListView_InsertColumn(hFuncLV, i, &lvc) == -1)
		{
			return FALSE;
		}
	}	

	ZeroMemory(&lvi,sizeof(lvi));
	lvi.mask	= LVIF_TEXT | LVIF_STATE;
	lvi.state		= 0;
	lvi.stateMask	= 0;

	ListView_DeleteAllItems(hDllLV);
	PIMAGE_IMPORT_DESCRIPTOR pTempDesc;
	int nCount=PeImport.GetImportDesCount();

	for (int i=0;i<nCount;i++)
	{
		pTempDesc =PeImport.GetImportDesc(i);
		if (pTempDesc !=NULL)
		{
			StringCchPrintf(szTemp,sizeof(szTemp),_T("%s"),PeImport.RvaToPtr(pTempDesc->Name));
			lvi.iItem=i;
			lvi.cchTextMax=sizeof(szTemp);
			lvi.pszText =szTemp;
			ListView_InsertItem(hDllLV,&lvi);
			StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDesc->OriginalFirstThunk);
			ListView_SetItemText(hDllLV,i,1,szTemp);
			StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDesc->TimeDateStamp);
			ListView_SetItemText(hDllLV,i,2,szTemp);
			StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDesc->ForwarderChain);
			ListView_SetItemText(hDllLV,i,3,szTemp);
			StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDesc->Name);
			ListView_SetItemText(hDllLV,i,4,szTemp);
			StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempDesc->FirstThunk);
			ListView_SetItemText(hDllLV,i,5,szTemp);

		}
	}	
	return TRUE;
}

//����pe Import Notify��Ϣ
BOOL Dlg_OnPeImportNotify(HWND hwnd, WPARAM wParam,  LPARAM lParam,LPEImport &PeImport)
{
	LVITEM lvi;
	TCHAR szTemp[40]={0};
	DWORD nIdx=0;
	WORD Id=LOWORD(wParam);
	DWORD nCode=((LPNMHDR)lParam)->code;
	LPESection PeSection;
	PeSection =(LPEFile)PeImport;

	switch(Id)
	{
	case IDC_LIST_DLL:
		if( nCode== NM_CLICK)
		{
			HWND hDllLV=GetDlgItem(hwnd,IDC_LIST_DLL);
			HWND hFuncLV=GetDlgItem(hwnd,IDC_LIST_IMPORT_FUNC);
			nIdx=ListView_GetSelectionMark(hDllLV);
			if (nIdx !=-1)
			{
				PIMAGE_IMPORT_DESCRIPTOR pTempDesc =PeImport.GetImportDesc(nIdx);
				if (pTempDesc !=NULL)
				{
					DWORD dwThunk=PeImport.GetImportThunk(pTempDesc);
					DWORD *pdwRva=(DWORD*)dwThunk;
					DWORD *pdwThunk=(DWORD*)PeImport.RvaToPtr(dwThunk);

					ZeroMemory(&lvi,sizeof(lvi));
					lvi.mask	= LVIF_TEXT | LVIF_STATE;
					lvi.state		= 0;
					lvi.stateMask	= 0;

					int i=0;
					while (*pdwThunk)
					{
						StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pdwRva);     //ThunkRva
						lvi.iItem=i;
						lvi.cchTextMax=sizeof(szTemp);
						lvi.pszText =szTemp;
						ListView_InsertItem(hFuncLV,&lvi);

						DWORD dwThunkOffset=PeSection.RvaToOffset((DWORD)pdwRva);   //ThunkOffset
						StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),dwThunkOffset);
						ListView_SetItemText(hFuncLV,i,1,szTemp);


						StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),*pdwThunk);   //Thunkֵ
						ListView_SetItemText(hFuncLV,i,2,szTemp);

						if (HIWORD(*pdwThunk)==0x8000)
						{
							StringCchPrintf(szTemp,sizeof(szTemp),_T("%s"),_T("--"));
							ListView_SetItemText(hFuncLV,i,3,szTemp);
							ListView_SetItemText(hFuncLV,i,4,szTemp);
						}else{
							PIMAGE_IMPORT_BY_NAME pByName=(PIMAGE_IMPORT_BY_NAME)PeImport.GetImportFucByName((DWORD)*pdwThunk);
							if (pByName !=NULL)
							{
								StringCchPrintf(szTemp,sizeof(szTemp),_T("%04x"),pByName->Hint);    //Hint
								ListView_SetItemText(hFuncLV,i,3,szTemp);
								StringCchPrintf(szTemp,sizeof(szTemp),_T("%s"),pByName->Name);    //Name
								ListView_SetItemText(hFuncLV,i,4,szTemp);
							}else{
								StringCchPrintf(szTemp,sizeof(szTemp),_T("%s"),_T("--"));
								ListView_SetItemText(hFuncLV,i,3,szTemp);
								ListView_SetItemText(hFuncLV,i,4,szTemp);
							}
						}
						++pdwRva;
						++pdwThunk;
						++i;
					}

				}
			}
		}
		return TRUE;
	default:
		break;
	}
	return FALSE;
}

//��ʾpe�ļ� ��������Ϣ
BOOL CALLBACK Dlg_PeImportProc(HWND hwndDlg, UINT uMsg,  WPARAM wParam, LPARAM lParam)
{
	static LPEImport PeImport;
	switch(uMsg)
	{
	case WM_INITDIALOG:
		PeImport =*(LPEFile*)lParam;
		if (PeImport.GetDataDirectory(IMAGE_DIRECTORY_ENTRY_IMPORT)->Size==0)
		{
			MessageBox(hwndDlg,_T("��ǰ�������"),_T("Error"),MB_OK|MB_ICONERROR);
			EndDialog(hwndDlg,0);
			return TRUE;
		}
		return Dlg_OnInitPeImportDialog(hwndDlg,NULL,lParam);
	case WM_NOTIFY:
		return Dlg_OnPeImportNotify(hwndDlg,wParam,lParam,PeImport);
	case WM_COMMAND:
		break;
	case WM_CLOSE:
		EndDialog(hwndDlg,0);
		return TRUE;
	default:
		break;
	}
	return FALSE;
}

//PE Export�Ի����ʼ��
BOOL Dlg_OnInitPeExportDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	LPEExport PeExport;
	LPESection PeSection;
	PeExport =*(LPEFile*)lParam;
	PeSection=*(LPEFile*)lParam;
	LVCOLUMN lvc;
	LVITEM lvi;
	TCHAR szTemp[40]={0};

	//ListView
	TCHAR *lpszColNames[]={_T("���"),_T("RVA"),_T("ƫ��"),_T("������")};
	DWORD dwColWidths[]={100,100,100,200};

	PIMAGE_EXPORT_DIRECTORY pExportDir=PeExport.GetExportTab();
	DWORD dwOffset=PeSection.RvaToOffset(PeExport.GetDataDirectory(IMAGE_DIRECTORY_ENTRY_EXPORT)->VirtualAddress);

	StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),dwOffset);                                            //�����ƫ��
	SetWindowText(GetDlgItem(hwnd,IDC_EDIT_EXP_OFFSET),szTemp);

	StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pExportDir->Characteristics);                //����ֵ
	SetWindowText(GetDlgItem(hwnd,IDC_EDIT_EXP_CHARCTOR),szTemp);

	StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pExportDir->Base);                               //��ַ
	SetWindowText(GetDlgItem(hwnd,IDC_EDIT_EXP_BASE),szTemp);

	StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pExportDir->Name);                              //����
	SetWindowText(GetDlgItem(hwnd,IDC_EDIT_EXP_NAME),szTemp);

	StringCchPrintf(szTemp,sizeof(szTemp),_T("%s"),PeExport.RvaToPtr(pExportDir->Name));    //�����ִ�
	SetWindowText(GetDlgItem(hwnd,IDC_EDIT_EXP_STRING),szTemp);

	StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pExportDir->NumberOfFunctions);        //��������
	SetWindowText(GetDlgItem(hwnd,IDC_EDIT_FUNC_NUM),szTemp);

	StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pExportDir->NumberOfNames);            //����������
	SetWindowText(GetDlgItem(hwnd,IDC_EDIT_FUNC_NAME_NUM),szTemp);

	StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pExportDir->AddressOfFunctions);         //������ַ
	SetWindowText(GetDlgItem(hwnd,IDC_EDIT_FUNC_ADDR),szTemp);

	StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pExportDir->AddressOfNames);             //�������Ƶ�ַ
	SetWindowText(GetDlgItem(hwnd,IDC_EDIT_FUNC_NAME_ADDR),szTemp);

	StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pExportDir->AddressOfNameOrdinals);  //��������ŵ�ַ
	SetWindowText(GetDlgItem(hwnd,IDC_EDIT_FUNC_NAME_ORDER_ADDR),szTemp);

	//����ListView���ڷ�� ---Report������ѡ��
	HWND hWndLV=GetDlgItem(hwnd,IDC_LIST_EXPORT_FUNC);
	long dwLong=GetWindowLong(hWndLV,GWL_STYLE);
	dwLong |=LVS_REPORT | LVS_SINGLESEL;
	SetWindowLong(hWndLV,GWL_STYLE,dwLong);

	//����ListView���͡�����ѡ�з�񡢱�ƽ
	ListView_SetExtendedListViewStyle(hWndLV,/*LVS_EX_GRIDLINES |*/ LVS_EX_FULLROWSELECT | LVS_EX_FLATSB);

	ZeroMemory(&lvc,sizeof(lvc));
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt =LVCFMT_LEFT;

	for (int i=0;i<4;i++)
	{
		lvc.cx =dwColWidths[i];
		lvc.pszText =lpszColNames[i];
		lvc.iSubItem	= i;
		if (ListView_InsertColumn(hWndLV, i, &lvc) == -1)
		{
			return FALSE;
		}
	}	

	ZeroMemory(&lvi,sizeof(lvi));
	lvi.mask	= LVIF_TEXT | LVIF_STATE;
	lvi.state		= 0;
	lvi.stateMask	= 0;

	int nCnt=PeExport.GetNameCount();
	for (int i=0; i<nCnt; i++)
	{
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%04x"),i+PeExport.GetExportTab()->Base);        //���
		lvi.iItem=i;
		lvi.cchTextMax=sizeof(szTemp);
		lvi.pszText =szTemp;
		ListView_InsertItem(hWndLV,&lvi);

		int nFuncIdx=PeExport.GetNameOrdValue(i);
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),PeExport.GetFuncValue(nFuncIdx));      //RVA
		ListView_SetItemText(hWndLV,i,1,szTemp);

		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),PeSection.RvaToOffset(PeExport.GetFuncValue(nFuncIdx)));  //ƫ��
		ListView_SetItemText(hWndLV,i,2,szTemp);

		StringCchPrintf(szTemp,sizeof(szTemp),_T("%s"),PeExport.GetNameValue(i));
		ListView_SetItemText(hWndLV,i,3,szTemp);

	}

	return TRUE;
}

//PE Export�Ի��򴰿ڹ���
BOOL CALLBACK Dlg_PeExportProc(HWND hwndDlg, UINT uMsg,  WPARAM wParam, LPARAM lParam)
{
	static LPEExport PeExport;
	switch(uMsg)
	{
	case WM_INITDIALOG:
		PeExport =*(LPEFile*)lParam;
		if (PeExport.GetDataDirectory(IMAGE_DIRECTORY_ENTRY_EXPORT)->Size==0)
		{
			MessageBox(hwndDlg,_T("��ǰ�������"),_T("Error"),MB_OK|MB_ICONERROR);
			EndDialog(hwndDlg,0);
			return TRUE;
		}
		return Dlg_OnInitPeExportDialog(hwndDlg,NULL,lParam);
	case WM_COMMAND:
		if (LOWORD(wParam)==IDC_BTN_EXPORT_OK)
		{
			EndDialog(hwndDlg,0);
		}else if (LOWORD(wParam)==IDC_BTN_EXPORT_SAVE)
		{
			MessageBox(hwndDlg,_T("û��ʵ��(*^__^*)"),_T("��ʾ"),MB_OK|MB_ICONINFORMATION);
		}
		return TRUE;
	case WM_CLOSE:
		EndDialog(hwndDlg,0);
		return TRUE;
	default:
		break;
	}
	return FALSE;
}

//�ͷŶ�̬���ɵ��ڴ�
void FreeData(HWND hTree,HTREEITEM hParent)
{
	TVITEM tvItem;
	//TCHAR szTemp[40];
	pResData pdata=NULL;
	while(hParent)
	{
		ZeroMemory(&tvItem,sizeof(tvItem));
		tvItem.mask =LVIF_PARAM|TVIF_CHILDREN/*|TVIF_TEXT*/;
		tvItem.hItem=hParent;
		//tvItem.cchTextMax=sizeof(szTemp);
		//tvItem.pszText =szTemp;
		TreeView_GetItem(hTree,&tvItem);
		if (tvItem.cChildren ==1)
		{
			HTREEITEM hChild=TreeView_GetChild(hTree,hParent);
			FreeData(hTree,hChild);
		}
		pdata =(pResData)tvItem.lParam;
		delete pdata;
		hParent =TreeView_GetNextSibling(hTree,hParent);
	}

}

//TreeView ��ӽڵ�
HTREEITEM TreeAddChildItem(HWND hTree,HTREEITEM hParent,DWORD lpRes,
	PIMAGE_RESOURCE_DIRECTORY_ENTRY lpResEntry,DWORD dwLevel)
{

	HTREEITEM hItem=NULL;
	TCHAR szTemp[40]={0};
	TV_INSERTSTRUCT stTvInsert;
	TV_ITEM tvItem;

	ZeroMemory(&tvItem,sizeof(tvItem));
	tvItem.mask =TVIF_TEXT | TVIF_STATE |LVIF_PARAM;
	tvItem.cchTextMax =sizeof(szTemp);
	tvItem.state =0;
	tvItem.stateMask=0;

	pResData pdata =new ResData;
	pdata->lpRes=lpRes;
	pdata->lpResEntry=(DWORD)lpResEntry;

	tvItem.lParam =(DWORD)pdata;

	ZeroMemory(&stTvInsert,sizeof(stTvInsert));
	stTvInsert.hParent =hParent;
	stTvInsert.hInsertAfter =TVI_LAST;

	//������ַ���
	if(lpResEntry->NameIsString)  
	{
		PIMAGE_RESOURCE_DIR_STRING_U pString=(PIMAGE_RESOURCE_DIR_STRING_U)(lpRes+lpResEntry->NameOffset);
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%s"),pString->NameString);
		tvItem.pszText =szTemp;
		stTvInsert.item =tvItem;

		hItem =TreeView_InsertItem(hTree,&stTvInsert);
	}else{  //ID
		if (dwLevel==1)
		{ //��һ��
			switch(lpResEntry->Id)
			{
			case 1: _tcscpy_s(szTemp, _T("���")); break;
			case 2: _tcscpy_s(szTemp, _T("λͼ")); break;
			case 3: _tcscpy_s(szTemp, _T("ͼ��")); break;
			case 4: _tcscpy_s(szTemp, _T("�˵�")); break;
			case 5: _tcscpy_s(szTemp, _T("�Ի���")); break;
			case 6: _tcscpy_s(szTemp, _T("�ַ���")); break;
			case 7: _tcscpy_s(szTemp, _T("FontDir")); break;
			case 8: _tcscpy_s(szTemp, _T("����")); break;
			case 9: _tcscpy_s(szTemp, _T("��ݼ�")); break;
			case 10: _tcscpy_s(szTemp, _T("RCDATA")); break;
			case 11: _tcscpy_s(szTemp, _T("��Ϣ��")); break;
			case 12: _tcscpy_s(szTemp, _T("�����")); break;
			case 14: _tcscpy_s(szTemp, _T("ͼ��")); break;		
			case 16: _tcscpy_s(szTemp, _T("�汾")); break;
			case 17: _tcscpy_s(szTemp, _T("DlgInclude")); break;
			case 19: _tcscpy_s(szTemp, _T("PlugPlay")); break;
			case 20: _tcscpy_s(szTemp, _T("VXD")); break;
			case 21: _tcscpy_s(szTemp, _T("ANICursor")); break;
			case 22: _tcscpy_s(szTemp, _T("ANIIcon")); break;
			case 23: _tcscpy_s(szTemp, _T("HTML")); break;
			case 24: _tcscpy_s(szTemp, _T("XML")); break;
			default: StringCchPrintf(szTemp,sizeof(szTemp),_T("%d"),lpResEntry->Id);
			}

			tvItem.pszText =szTemp;
			stTvInsert.item =tvItem;
			hItem =TreeView_InsertItem(hTree,&stTvInsert);
		}else if(dwLevel ==2){
			StringCchPrintf(szTemp,sizeof(szTemp),_T("%d"),lpResEntry->Id);
			tvItem.pszText =szTemp;
			stTvInsert.item =tvItem;
			hItem =TreeView_InsertItem(hTree,&stTvInsert);
		}
	}

	//��һ���Ƿ�ΪĿ¼
	if (lpResEntry->DataIsDirectory)
	{
		PIMAGE_RESOURCE_DIRECTORY pTempDir=(PIMAGE_RESOURCE_DIRECTORY)(lpRes +lpResEntry->OffsetToDirectory);
		PIMAGE_RESOURCE_DIRECTORY_ENTRY pResEntry=(PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)pTempDir+sizeof(IMAGE_RESOURCE_DIRECTORY));
		int nCount=pTempDir->NumberOfIdEntries +pTempDir->NumberOfNamedEntries;
		for (int i=0; i<nCount; i++)
		{
			TreeAddChildItem(hTree,hItem,lpRes,pResEntry+i,dwLevel+1);
		}
	}

	return hItem;
}

//��ʼ��pe ��Դ��Ϣ�Ի���
BOOL Dlg_OnInitPeResDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	LPEResource PeResource;
	PeResource =*(LPEFile*)lParam;
	TCHAR szTemp[30]={0};

	PIMAGE_RESOURCE_DIRECTORY pResDir =PeResource.GetResDir();
	PIMAGE_RESOURCE_DIRECTORY_ENTRY pResDirEntry=PeResource.GetResDirEntry();
	assert(pResDir !=NULL);
	assert(pResDirEntry !=NULL);

	StringCchPrintf(szTemp,sizeof(szTemp),_T("%04x"),pResDir->NumberOfNamedEntries);
	SetWindowText(GetDlgItem(hwnd,IDC_EDIT_ROOT_NAME),szTemp);

	StringCchPrintf(szTemp,sizeof(szTemp),_T("%04x"),pResDir->NumberOfIdEntries);
	SetWindowText(GetDlgItem(hwnd,IDC_EDIT_ROOT_ID),szTemp);

	HWND hTree=GetDlgItem(hwnd,IDC_TREE_RESOURCE);
	HTREEITEM hRoot =TreeView_GetRoot(hTree);

	int nCount =PeResource.GetResDirEntryCount();
	for(int i=0; i<nCount; i++)
	{
		TreeAddChildItem(hTree,hRoot,(DWORD)pResDir,pResDirEntry+i,1);
	}

	HTREEITEM hChild=TreeView_GetChild(hTree,hRoot);
	TreeView_SelectItem(hTree,hChild);  //ѡ�е�һ���ڵ�

	return TRUE;
}

//PE ��Դ��Ϣ�Ի��򴰿ڹ���
BOOL CALLBACK Dlg_PeResourceProc(HWND hwndDlg, UINT uMsg,  WPARAM wParam, LPARAM lParam)
{
	static LPEResource PeResource;
	switch(uMsg)
	{
	case WM_INITDIALOG:
		PeResource =*(LPEFile*)lParam;
		if (PeResource.GetDataDirectory(IMAGE_DIRECTORY_ENTRY_RESOURCE)->Size==0)   //�ж��Ƿ������ԴĿ¼
		{
			MessageBox(hwndDlg,_T("��ǰ����ԴĿ¼"),_T("Error"),MB_OK|MB_ICONERROR);
			EndDialog(hwndDlg,0);
			return TRUE;
		}
		return Dlg_OnInitPeResDialog(hwndDlg,NULL,lParam);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam)==IDC_BTN_RESOURCE_SAVE)
		{
			MessageBox(hwndDlg,_T("û��ʵ��(*^__^*)"),_T("��ʾ"),MB_OK|MB_ICONINFORMATION);
		}else if (LOWORD(wParam)==IDC_BTN_HEX)
		{
			MessageBox(hwndDlg,_T("û��ʵ��(*^__^*)"),_T("��ʾ"),MB_OK|MB_ICONINFORMATION);
		}
		return TRUE;
	case WM_NOTIFY:               //����TreeView �ӿؼ��¼�
		{
			if (LOWORD(wParam)==IDC_TREE_RESOURCE)
			{
				NM_TREEVIEW* phdr = (NM_TREEVIEW*)lParam;
				if (phdr->hdr.code ==TVN_SELCHANGED)  //ѡ��ı�
				{
					TVITEM tvItem;
					TCHAR szTemp[40]={0};
					HWND hTree=GetDlgItem(hwndDlg,IDC_TREE_RESOURCE);
					HTREEITEM hSelItem=TreeView_GetSelection(hTree);
					ZeroMemory(&tvItem,sizeof(tvItem));
					tvItem.mask =LVIF_PARAM|TVIF_CHILDREN;
					tvItem.hItem=hSelItem;
					TreeView_GetItem(hTree,&tvItem);

					pResData pData=(pResData)tvItem.lParam;

					if (tvItem.cChildren==1)       //��������ӽڵ�
					{
						PIMAGE_RESOURCE_DIRECTORY_ENTRY pEntry=(PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pData->lpResEntry);
						PIMAGE_RESOURCE_DIRECTORY pDir=(PIMAGE_RESOURCE_DIRECTORY)(pData->lpRes+pEntry->OffsetToDirectory);
						StringCchPrintf(szTemp,sizeof(szTemp),_T("%04x"),pDir->NumberOfNamedEntries);
						SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_SELECTED_NAME),szTemp);

						StringCchPrintf(szTemp,sizeof(szTemp),_T("%04x"),pDir->NumberOfIdEntries);
						SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_SELECTED_ID),szTemp);
						SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_REC_RVA),_T(""));
						SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_REC_OFFSET),_T(""));
						SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_REC_SIZE),_T(""));
					}else if (tvItem.cChildren==0)
					{
						LPESection PeSection;
						PeSection =(LPEFile)PeResource;
						PIMAGE_RESOURCE_DIRECTORY_ENTRY pEntry=(PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pData->lpResEntry);
						PIMAGE_RESOURCE_DIRECTORY pDir=(PIMAGE_RESOURCE_DIRECTORY)(pData->lpRes+pEntry->OffsetToDirectory);
						PIMAGE_RESOURCE_DIRECTORY_ENTRY pNextEntry =(PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)pDir+sizeof(IMAGE_RESOURCE_DIRECTORY));

						PIMAGE_RESOURCE_DATA_ENTRY pDataEntry=(PIMAGE_RESOURCE_DATA_ENTRY)(pData->lpRes+pNextEntry->OffsetToData);
						StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pDataEntry->OffsetToData);
						SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_REC_RVA),szTemp);            //RVA
						StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),PeSection.RvaToOffset(pDataEntry->OffsetToData));
						SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_REC_OFFSET),szTemp);      //Offset
						StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pDataEntry->Size);          
						SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_REC_SIZE),szTemp);             //Size

					}
				}
			}
		}
		return TRUE;
	case WM_CLOSE:
		{
			HWND hTree =GetDlgItem(hwndDlg,IDC_TREE_RESOURCE);
			HTREEITEM hRoot=TreeView_GetRoot(hTree);
			FreeData(hTree,hRoot);     //�ͷŶ��ڴ�
			EndDialog(hwndDlg,0);
			return TRUE;
		}
	default:
		break;
	}
	return FALSE;
}

//��ʼ��Pe �ض�λ��Ϣ�Ի���
BOOL Dlg_OnInitPeRelocDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	LPERelocate PeRelocate;
	LPESection PeSection;
	PeRelocate =*(LPEFile*)lParam;
	PeSection =*(LPEFile*)lParam;

	LVCOLUMN lvc;
	LVITEM lvi;
	TCHAR szTemp[40]={0};

	//���� ListView
	TCHAR *lpszSecColNames[]={_T("����"),_T("����"),_T("RVA"),_T("��Ŀ")};
	DWORD dwSecColWidths[]={50,120,100,100};

	//����Ŀ ListView
	TCHAR *lpszBlockColNames[]={_T("����"),_T("RVA"),_T("ƫ��"),_T("����"),_T("FAR��ַ"),_T("���ݽ���")};
	DWORD dwBlockColWidths[]={50,80,80,120,80,200};

	//����ListView���ڷ�� ---Report������ѡ��
	HWND hSecLV=GetDlgItem(hwnd,IDC_LIST_RELO_SECTION);
	HWND hBlockLV=GetDlgItem(hwnd,IDC_LIST_RELO_PROJECT);

	long dwLong=GetWindowLong(hSecLV,GWL_STYLE);
	dwLong |=LVS_REPORT | LVS_SINGLESEL;
	SetWindowLong(hSecLV,GWL_STYLE,dwLong);
	SetWindowLong(hBlockLV,GWL_STYLE,dwLong);

	//����ListView���͡�����ѡ�з�񡢱�ƽ
	ListView_SetExtendedListViewStyle(hSecLV,/*LVS_EX_GRIDLINES |*/ LVS_EX_FULLROWSELECT | LVS_EX_FLATSB);
	ListView_SetExtendedListViewStyle(hBlockLV,/*LVS_EX_GRIDLINES |*/ LVS_EX_FULLROWSELECT | LVS_EX_FLATSB);

	ZeroMemory(&lvc,sizeof(lvc));
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt =LVCFMT_LEFT;

	for (int i=0;i<4;i++)
	{
		lvc.cx =dwSecColWidths[i];
		lvc.pszText =lpszSecColNames[i];
		lvc.iSubItem	= i;
		if (ListView_InsertColumn(hSecLV, i, &lvc) == -1)
		{
			return FALSE;
		}
	}	

	for (int i=0;i<6;i++)
	{
		lvc.cx =dwBlockColWidths[i];
		lvc.pszText =lpszBlockColNames[i];
		lvc.iSubItem	= i;
		if (ListView_InsertColumn(hBlockLV, i, &lvc) == -1)
		{
			return FALSE;
		}
	}	

	ZeroMemory(&lvi,sizeof(lvi));
	lvi.mask	= LVIF_TEXT | LVIF_STATE;
	lvi.state		= 0;
	lvi.stateMask	= 0;

	TCHAR szSecName[10]={0};
	PIMAGE_BASE_RELOCATION pTempReloc=NULL;
	//��ʼ������ListView
	int nCnt=PeRelocate.GetRelocBlocksCount();
	for (int i=0; i<nCnt; i++)
	{
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%d"),i+1);     //����
		lvi.cchTextMax=sizeof(szTemp);
		lvi.pszText =szTemp;
		lvi.iItem =i;
		ListView_InsertItem(hSecLV,&lvi);
		pTempReloc =PeRelocate.GetRelocBlock(i);
		PeSection.RvaToSectionName(pTempReloc->VirtualAddress,szSecName);
		StringCchPrintf(szTemp,sizeof(szTemp),_T("(\"%s\")"),szSecName);  //����
		ListView_SetItemText(hSecLV,i,1,szTemp);

		StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempReloc->VirtualAddress);  //RVA
		ListView_SetItemText(hSecLV,i,2,szTemp);

		int nEntries=(pTempReloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) >>1;
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%xh / %dd"),nEntries,nEntries);
		ListView_SetItemText(hSecLV,i,3,szTemp);

	}

	return TRUE;
}

//����PE �ض�λ��Ϣ���ڹ���Notify֪ͨ
BOOL Dlg_OnPeReloctNotify(HWND hwnd, WPARAM wParam,  LPARAM lParam,LPERelocate &PeRelocate)
{
	LVITEM lvi;
	TCHAR szTemp[40]={0},szBytes[35]={0};
	UCHAR szBuf[10]={0};
	DWORD nIdx=0;
	WORD Id=LOWORD(wParam);
	DWORD nCode=((LPNMHDR)lParam)->code;
	LPESection PeSection;
	PeSection =(LPEFile)PeRelocate;
	if (Id ==IDC_LIST_RELO_SECTION)
	{
		if (nCode ==NM_CLICK)
		{

			PIMAGE_BASE_RELOCATION pTempReloc=NULL;
			HWND hSecLV=GetDlgItem(hwnd,IDC_LIST_RELO_SECTION);
			HWND hBlockLV=GetDlgItem(hwnd,IDC_LIST_RELO_PROJECT);
			ListView_DeleteAllItems(hBlockLV);
			nIdx=ListView_GetSelectionMark(hSecLV);
			if (nIdx !=-1)
			{
				ZeroMemory(&lvi,sizeof(lvi));
				lvi.mask	= LVIF_TEXT | LVIF_STATE;
				lvi.state		= 0;
				lvi.stateMask	= 0;

				pTempReloc=PeRelocate.GetRelocBlock(nIdx);
				if (pTempReloc !=NULL)
				{
					//�õ��ض�λ���е�����
					int nEntries =(pTempReloc->SizeOfBlock -sizeof(IMAGE_BASE_RELOCATION)) >>1;
					//�õ�ָ���һ���ָ��
					USHORT *offsetPtr=(USHORT*)(pTempReloc+1);
					for(int i=0; i<nEntries; i++)
					{
						StringCchPrintf(szTemp,sizeof(szTemp),_T("%d"),i+1);    //����
						lvi.iItem =i;
						lvi.cchTextMax =sizeof(szTemp);
						lvi.pszText =szTemp;
						ListView_InsertItem(hBlockLV,&lvi);
						StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),pTempReloc->VirtualAddress+(*offsetPtr & 0x0FFF));   //RVA
						ListView_SetItemText(hBlockLV,i,1,szTemp);

						StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"),            //ƫ��
							PeSection.RvaToOffset(pTempReloc->VirtualAddress+(*offsetPtr & 0x0FFF)));
						ListView_SetItemText(hBlockLV,i,2,szTemp);

						int TypeOffset=(*offsetPtr & 0xF000)>>12;         //����
						if(TypeOffset ==IMAGE_REL_BASED_ABSOLUTE)
						{
							StringCchPrintf(szTemp,sizeof(szTemp),_T("ABSOLUTE  (%d)"),TypeOffset);
						}else if (TypeOffset ==IMAGE_REL_BASED_HIGHLOW)
						{
							StringCchPrintf(szTemp,sizeof(szTemp),_T("HIGHLOW  (%d)"),TypeOffset);
						}else{
							StringCchPrintf(szTemp,sizeof(szTemp),_T("(%d)"),TypeOffset);
						}
						ListView_SetItemText(hBlockLV,i,3,szTemp);
						DWORD *dwVA=(DWORD*)PeRelocate.RvaToPtr(pTempReloc->VirtualAddress+(*offsetPtr & 0x0FFF));
						StringCchPrintf(szTemp,sizeof(szTemp),_T("%08x"), *dwVA);   //FAR��ַ
						ListView_SetItemText(hBlockLV,i,4,szTemp);

						ZeroMemory(szBuf,sizeof(szBuf));
						ZeroMemory(szBytes,sizeof(szBytes));
						PeSection.VaToReadBytes(*dwVA,szBuf);
						FormatBytes(szBuf,szBytes);
						StringCchPrintf(szTemp,sizeof(szTemp),_T("%s  ..."),szBytes);     //���ݽ���
						ListView_SetItemText(hBlockLV,i,5,szTemp);
						offsetPtr++;
					}//end for
				}
			}
		}
	}
	return TRUE;
}


//PE �ض�λ��Ϣ���ڹ���
BOOL CALLBACK Dlg_PeRelocateProc(HWND hwndDlg, UINT uMsg,  WPARAM wParam, LPARAM lParam)
{
	static LPERelocate PeRelocate;
	switch(uMsg)
	{
	case WM_INITDIALOG:
		PeRelocate =*(LPEFile*)lParam;
		if (PeRelocate.GetDataDirectory(IMAGE_DIRECTORY_ENTRY_BASERELOC)->Size ==0)
		{
			MessageBox(hwndDlg,_T("û���ҵ��ض�λ"),_T("Error"),MB_OK|MB_ICONERROR);
			EndDialog(hwndDlg,0);
			return TRUE;
		}
		return Dlg_OnInitPeRelocDialog(hwndDlg,NULL,lParam);
	case WM_NOTIFY:
		return Dlg_OnPeReloctNotify(hwndDlg,wParam,lParam,PeRelocate);
	case WM_CLOSE:
		EndDialog(hwndDlg,0);
		return TRUE;
	default:
		break;
	}
	return FALSE;
}

//pe ����Ŀ¼���ڹ���
BOOL CALLBACK Dlg_PeDirProc(HWND hwndDlg, 
	UINT uMsg, 
	WPARAM wParam, 
	LPARAM lParam)
{
	static LPEFile *pPeFile;

	switch(uMsg)
	{
	case WM_INITDIALOG:
		pPeFile =(LPEFile*)lParam;
		return Dlg_OnInitPeDirDialog(hwndDlg,NULL,lParam);
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_BTN_DIR_OK:
			EndDialog(hwndDlg,0);
			break;
		case IDC_BTN_SAVE:
			MessageBox(hwndDlg,_T("û��ʵ��(*^__^*)"),_T("��ʾ"),MB_OK|MB_ICONINFORMATION);
			break;
		case IDC_BTN_EXPORT:
			DialogBoxParam(g_hInstance,MAKEINTRESOURCE(IDD_DLG_EXPORT),hwndDlg,Dlg_PeExportProc,(LPARAM)pPeFile);
			break;
		case IDC_BTN_IMPORT:
			DialogBoxParam(g_hInstance,MAKEINTRESOURCE(IDD_DLG_IMPORT),hwndDlg,Dlg_PeImportProc,(LPARAM)pPeFile);
			break;
		case IDC_BTN_RESOURCE:
			DialogBoxParam(g_hInstance,MAKEINTRESOURCE(IDD_DLG_RESOURCE),hwndDlg,Dlg_PeResourceProc,(LPARAM)pPeFile);
			break;
		case IDC_BTN_RELOCATE:
			DialogBoxParam(g_hInstance,MAKEINTRESOURCE(IDD_DLG_RELOCATE),hwndDlg,Dlg_PeRelocateProc,(LPARAM)pPeFile);
			break;
		case IDC_BTN_DEBUG:
			//DialogBoxParam(g_hInstance,MAKEINTRESOURCE(IDD_DLG_DEBUG),hwndDlg,Dlg_PeDebugProc,(LPARAM)pPeFile);
			break;
		case IDC_BTN_COPYRIGHT:
			//DialogBoxParam(g_hInstance,MAKEINTRESOURCE(IDD_DLG_COPYRIGTH),hwndDlg,Dlg_PeCopyrightProc,(LPARAM)pPeFile);
			break;
		case IDC_BTN_TLS:
			//DialogBoxParam(g_hInstance,MAKEINTRESOURCE(IDD_DLG_TLS),hwndDlg,Dlg_PeTlsProc,(LPARAM)pPeFile);
			break;
		case IDC_BTN_IMPORTBOUND:
			//DialogBoxParam(g_hInstance,MAKEINTRESOURCE(IDD_DLG_IMPORTBOUND),hwndDlg,Dlg_PeImportBoundProc,(LPARAM)pPeFile);
			break;
		case IDC_BTN_COM:
			//DialogBoxParam(g_hInstance,MAKEINTRESOURCE(IDD_DLG_COM),hwndDlg,Dlg_PeComProc,(LPARAM)pPeFile);
			break;
		default:
			break;
		}
		return TRUE;
	case WM_CLOSE:
		pPeFile =NULL;
		EndDialog(hwndDlg,0);
		return TRUE;
	default:
		break;
	}
	return FALSE;
}

//λ�ü��㴰�ڹ���
BOOL CALLBACK Dlg_PeCalcProc(HWND hwndDlg, UINT uMsg,  WPARAM wParam, LPARAM lParam)
{
	static LPESection PeSection;
	static int nFlag=1;

	switch(uMsg)
	{
	case WM_INITDIALOG:
		PeSection = *(LPEFile*)lParam;
		SendMessage(GetDlgItem(hwndDlg,IDC_EDIT_RVA),EM_LIMITTEXT,(WPARAM)8,(LPARAM)0);
		SendMessage(GetDlgItem(hwndDlg,IDC_EDIT_VA),EM_LIMITTEXT,(WPARAM)8,(LPARAM)0);
		SendMessage(GetDlgItem(hwndDlg,IDC_EDIT_OFFSET),EM_LIMITTEXT,(WPARAM)8,(LPARAM)0);
		SetFocus(GetDlgItem(hwndDlg,IDC_EDIT_RVA));
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_BTN_RVA:
			SendDlgItemMessage(hwndDlg,IDC_EDIT_VA,EM_SETREADONLY,TRUE,0);
			SendDlgItemMessage(hwndDlg,IDC_EDIT_RVA,EM_SETREADONLY,FALSE,0);
			SendDlgItemMessage(hwndDlg,IDC_EDIT_OFFSET,EM_SETREADONLY,TRUE,0);
			SendDlgItemMessage(hwndDlg,IDC_EDIT_VA,WM_SETTEXT,0,0);
			SendDlgItemMessage(hwndDlg,IDC_EDIT_RVA,WM_SETTEXT,0,0);
			SendDlgItemMessage(hwndDlg,IDC_EDIT_OFFSET,WM_SETTEXT,0,0);
			SendDlgItemMessage(hwndDlg,IDC_EDIT_SECTION,WM_SETTEXT,0,0);
			SendDlgItemMessage(hwndDlg,IDC_EDIT_BYTE,WM_SETTEXT,0,0);
			SetFocus(GetDlgItem(hwndDlg,IDC_EDIT_RVA));
			nFlag=1;
			break;
		case IDC_BTN_VA:
			SendDlgItemMessage(hwndDlg,IDC_EDIT_VA,EM_SETREADONLY,FALSE,0);
			SendDlgItemMessage(hwndDlg,IDC_EDIT_RVA,EM_SETREADONLY,TRUE,0);
			SendDlgItemMessage(hwndDlg,IDC_EDIT_OFFSET,EM_SETREADONLY,TRUE,0);
			SendDlgItemMessage(hwndDlg,IDC_EDIT_VA,WM_SETTEXT,0,0);
			SendDlgItemMessage(hwndDlg,IDC_EDIT_RVA,WM_SETTEXT,0,0);
			SendDlgItemMessage(hwndDlg,IDC_EDIT_OFFSET,WM_SETTEXT,0,0);
			SendDlgItemMessage(hwndDlg,IDC_EDIT_SECTION,WM_SETTEXT,0,0);
			SendDlgItemMessage(hwndDlg,IDC_EDIT_BYTE,WM_SETTEXT,0,0);
			SetFocus(GetDlgItem(hwndDlg,IDC_EDIT_VA));
			nFlag=0;
			break;
		case IDC_BTN_OFFSET:
			SendDlgItemMessage(hwndDlg,IDC_EDIT_VA,EM_SETREADONLY,TRUE,0);
			SendDlgItemMessage(hwndDlg,IDC_EDIT_RVA,EM_SETREADONLY,TRUE,0);
			SendDlgItemMessage(hwndDlg,IDC_EDIT_OFFSET,EM_SETREADONLY,FALSE,0);
			SendDlgItemMessage(hwndDlg,IDC_EDIT_VA,WM_SETTEXT,0,0);
			SendDlgItemMessage(hwndDlg,IDC_EDIT_RVA,WM_SETTEXT,0,0);
			SendDlgItemMessage(hwndDlg,IDC_EDIT_OFFSET,WM_SETTEXT,0,0);
			SendDlgItemMessage(hwndDlg,IDC_EDIT_SECTION,WM_SETTEXT,0,0);
			SendDlgItemMessage(hwndDlg,IDC_EDIT_BYTE,WM_SETTEXT,0,0);
			SetFocus(GetDlgItem(hwndDlg,IDC_EDIT_OFFSET));
			nFlag=2;
			break;
		case IDC_EDIT_RVA:                  //�˴�Ҳ�������滻Edit���ڹ��̵ķ���������16��������
			if (HIWORD(wParam)==EN_UPDATE)
			{
				HWND hEdit =GetDlgItem(hwndDlg,IDC_EDIT_RVA);
				CheckHexInput(hEdit);
			}	
			break;
		case IDC_EDIT_VA:
			if (HIWORD(wParam)==EN_UPDATE)
			{
				HWND hEdit =GetDlgItem(hwndDlg,IDC_EDIT_VA);
				CheckHexInput(hEdit);
			}	
			break;
		case IDC_EDIT_OFFSET:
			if (HIWORD(wParam)==EN_UPDATE)
			{
				HWND hEdit =GetDlgItem(hwndDlg,IDC_EDIT_OFFSET);
				CheckHexInput(hEdit);
			}	
			break;
		case IDC_BTN_EXECUTE:
			{
				DWORD dwVA,dwRVA,dwOffset;
				TCHAR szVA[10]={0},szRVA[10]={0},szOffset[10]={0},szName[10]={0},szBytes[35]={0};
				UCHAR szBuf[10]={0};
				if (nFlag ==0)
				{
					GetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_VA),szVA,sizeof(szVA));
					dwVA=strtoul(szVA,NULL,16);
					dwRVA =dwVA -PeSection.GetNtOptionalHeader()->ImageBase;
					if (!PeSection.RvaToSectionName(dwRVA,szName))
					{
						MessageBox(hwndDlg,_T("������Χ"),_T("Error"),MB_OK | MB_ICONERROR);
						return TRUE;
					}
					dwOffset =PeSection.RvaToOffset(dwRVA);

				}else if (nFlag==1)
				{
					GetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_RVA),szRVA,sizeof(szRVA));
					dwRVA=strtoul(szRVA,NULL,16);
					if (!PeSection.RvaToSectionName(dwRVA,szName))
					{
						MessageBox(hwndDlg,_T("������Χ"),_T("Error"),MB_OK | MB_ICONERROR);
						return TRUE;
					}
					dwVA =dwRVA +PeSection.GetNtOptionalHeader()->ImageBase;
					dwOffset =PeSection.RvaToOffset(dwRVA);
				}else if(nFlag ==2){
					GetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_OFFSET),szOffset,sizeof(szOffset));
					dwOffset=strtoul(szOffset,NULL,16);
					dwRVA =PeSection.OffsetToRva(dwOffset);
					if (!PeSection.RvaToSectionName(dwRVA,szName))
					{
						MessageBox(hwndDlg,_T("������Χ"),_T("Error"),MB_OK | MB_ICONERROR);
						return TRUE;
					}
					dwVA =dwRVA +PeSection.GetNtOptionalHeader()->ImageBase;
				}
				StringCchPrintf(szVA,sizeof(szVA),_T("%08x"),dwVA);
				StringCchPrintf(szRVA,sizeof(szRVA),_T("%08x"),dwRVA);
				StringCchPrintf(szOffset,sizeof(szOffset),_T("%08x"),dwOffset);
				SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_VA),szVA);
				SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_RVA),szRVA);
				SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_OFFSET),szOffset);
				SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_SECTION),szName);
				if (PeSection.OffsetToReadBytes(dwOffset,szBuf))
				{
					FormatBytes(szBuf,szBytes);
					SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_BYTE),szBytes);
				}
			}
			return TRUE;
		default:
			break;
		}
		return TRUE;
	case WM_CLOSE:
		EndDialog(hwndDlg,0);
		return TRUE;
	default:
		break;
	}
	return FALSE;
}

//Edit �´��ڹ���
LRESULT CALLBACK NewEditProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch(uMsg)
	{
	case WM_SETCURSOR:
		SetCursor(LoadCursor(NULL,IDC_ARROW));
		return TRUE;
	case WM_LBUTTONDOWN:
		{
			// 			HWND  hTop= FindWindow(NULL,_T("[����ʱ���־ת���� -GMT]"));
			HWND  hTop=GetParent(hwnd);
			POINT pos;
			pos.x =LOWORD(lParam);
			pos.y =HIWORD(lParam);
			SetFocus(hwnd);        //��ý���
			HideCaret(hwnd);      //���ع��
			if (pos.x<31)
			{
				SendDlgItemMessage(hTop,IDC_EDIT_TIME,EM_SETSEL,0,2);
				SendDlgItemMessage(hTop,IDC_SPIN_TIME,UDM_SETPOS,0,(LPARAM)MAKELONG(0,0));
				SendDlgItemMessage(hTop,IDC_SPIN_TIME,UDM_SETRANGE,0,(LPARAM)MAKELONG(23,0));
				tmPos =HOUR;
			}else if (pos.x>31 && pos.x<47)
			{
				SendDlgItemMessage(hTop,IDC_EDIT_TIME,EM_SETSEL,3,5);
				SendDlgItemMessage(hTop,IDC_SPIN_TIME,UDM_SETPOS,0,(LPARAM)MAKELONG(0,0));
				SendDlgItemMessage(hTop,IDC_SPIN_TIME,UDM_SETRANGE,0,(LPARAM)MAKELONG(59,0));
				tmPos =MINUTE;
			}else if (pos.x>47)
			{
				SendDlgItemMessage(hTop,IDC_EDIT_TIME,EM_SETSEL,6,-1);
				SendDlgItemMessage(hTop,IDC_SPIN_TIME,UDM_SETPOS,0,(LPARAM)MAKELONG(0,0));
				SendDlgItemMessage(hTop,IDC_SPIN_TIME,UDM_SETRANGE,0,(LPARAM)MAKELONG(59,0));
				tmPos =SECOND;
			}
		}
		return TRUE;
	case WM_LBUTTONDBLCLK:    //�������˫���¼�
		return TRUE;
	case WM_CHAR:                     //���μ��������¼�,����Ͳ�����Edit�ļ���������
		return TRUE;
	default:
		break;
	}
	return CallWindowProc((WNDPROC)lpOldEditProc,hwnd,uMsg,wParam,lParam);
}

//Spin �´��ڹ���
LRESULT CALLBACK NewSpinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch(uMsg)
	{
	case WM_LBUTTONUP:
		{
			CallWindowProc((WNDPROC)lpOldSpinProc,hwnd,uMsg,wParam,lParam);
			HWND hTop=GetParent(hwnd);
			SendMessage(hTop,MYWM_MSG,0,0);
			return TRUE;
		}	
	default:
		break;
	}
	return CallWindowProc((WNDPROC)lpOldSpinProc,hwnd,uMsg,wParam,lParam);
}

//����ʱ��ת�����ڹ���
BOOL CALLBACK Dlg_PeDatetimeProc(HWND hwndDlg, UINT uMsg,  WPARAM wParam, LPARAM lParam)
{
	static SYSTEMTIME stTime;
	static DWORD dwSecTime;
	TCHAR szTime[20]={0}, szSeconds[20]={0};
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			//Spin ��Edit����
			HWND hSpin=GetDlgItem(hwndDlg,IDC_SPIN_TIME);
			HWND  hTimeEdit=GetDlgItem(hwndDlg,IDC_EDIT_TIME);
			SendMessage(hSpin,UDM_SETBUDDY,(WPARAM) hTimeEdit,0);
			SendMessage(hSpin,UDM_SETBASE,(WPARAM)10,0);
			SendMessage(hSpin,UDM_SETRANGE,0,(LPARAM)MAKELONG(23,0));
			tmPos =HOUR;  //Ĭ��Ϊhour

			//����Edit���ݳ���
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DATETIME_FLAG,EM_LIMITTEXT,(WPARAM)8,0);

			SendDlgItemMessage(hwndDlg,IDC_RADIO_ENCODE,BM_SETCHECK,BST_CHECKED,0);
			//CheckDlgButton(hwndDlg,IDC_RADIO_ENCODE,BST_CHECKED);
			SendDlgItemMessage(hwndDlg,IDC_EDIT_TIME,WM_SETTEXT,0,(LPARAM)_T("00:00:00"));
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DATETIME_FLAG,WM_SETTEXT,0,(LPARAM)_T("00000000"));
			EnableWindow(GetDlgItem(hwndDlg,IDC_EDIT_TIME),FALSE);
			EnableWindow(GetDlgItem(hwndDlg,IDC_DATETIME_PICKER),FALSE);

			//�滻Edit���ڹ���
			lpOldEditProc =SetWindowLong(hTimeEdit,GWL_WNDPROC,(LONG)NewEditProc);
			lpOldSpinProc=SetWindowLong(hSpin,GWL_WNDPROC,(LONG)NewSpinProc);
			lpEditProc =SetWindowLong(GetDlgItem(hwndDlg,IDC_EDIT_DATETIME_FLAG),
				GWL_WNDPROC,(LONG)NewHexEditProc);

		}
		return TRUE;
	case WM_NOTIFY:
		if (wParam==IDC_SPIN_TIME)
		{
			NM_UPDOWN *pNMUD=(NM_UPDOWN *)lParam;
			if (pNMUD->hdr.code==UDN_DELTAPOS)
			{
				TCHAR szTemp[5]={0},szTime[20]={0};
				GetDlgItemText(hwndDlg,IDC_EDIT_TIME,szTime,sizeof(szTime));

				if (tmPos ==HOUR)
				{
					szTemp[0]=szTime[0];
					szTemp[1]=szTime[1];
					int iTime=_ttoi(szTemp);
					if (pNMUD->iDelta >0)
					{
						iTime +=1;
					}else if (pNMUD->iDelta <0)
					{
						iTime -=1;
					}

					if (iTime>23)
					{
						iTime =0;
					}
					if (iTime <0)
					{
						iTime =23;
					}
					StringCchPrintf(szTemp,sizeof(szTemp),_T("%02d"),iTime);
					szTime[0]=szTemp[0];
					szTime[1]=szTemp[1];
				}else if (tmPos ==MINUTE)
				{
					szTemp[0]=szTime[3];
					szTemp[1]=szTime[4];
					int iTime=_ttoi(szTemp);
					if (pNMUD->iDelta >0)
					{
						iTime +=1;
					}else if (pNMUD->iDelta <0)
					{
						iTime -=1;
					}
					if (iTime>59)
					{
						iTime =0;
					}
					if (iTime <0)
					{
						iTime =59;
					}
					StringCchPrintf(szTemp,sizeof(szTemp),_T("%02d"),iTime);
					szTime[3]=szTemp[0];
					szTime[4]=szTemp[1];
				}else if (tmPos ==SECOND)
				{
					szTemp[0]=szTime[6];
					szTemp[1]=szTime[7];
					int iTime=_ttoi(szTemp);
					if (pNMUD->iDelta >0)
					{
						iTime +=1;
					}else if (pNMUD->iDelta <0)
					{
						iTime -=1;
					}
					if (iTime>59)
					{
						iTime =0;
					}
					if (iTime <0)
					{
						iTime =59;
					}
					StringCchPrintf(szTemp,sizeof(szTemp),_T("%02d"),iTime);
					szTime[6]=szTemp[0];
					szTime[7]=szTemp[1];
				}
				SendDlgItemMessage(hwndDlg,IDC_EDIT_TIME,WM_SETTEXT,0,(LPARAM)szTime);
			}	
			return TRUE;
		}
		break;
	case WM_COMMAND:
		{
			if (LOWORD(wParam)==IDC_RADIO_ENCODE)
			{
				EnableWindow(GetDlgItem(hwndDlg,IDC_EDIT_DATETIME_FLAG),TRUE);
				EnableWindow(GetDlgItem(hwndDlg,IDC_BTN_COPY_CLOSE),TRUE);
				EnableWindow(GetDlgItem(hwndDlg,IDC_EDIT_TIME),FALSE);
				EnableWindow(GetDlgItem(hwndDlg,IDC_DATETIME_PICKER),FALSE);
				//��ȡ����
				DateTime_GetSystemtime(
					GetDlgItem(hwndDlg, IDC_DATETIME_PICKER),
					&stTime);
				GetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_TIME),szTime,sizeof(szTime));
				stTime.wHour =(szTime[0]-'0')*10 + (szTime[1]-'0');
				stTime.wMinute =(szTime[3]-'0')*10 + (szTime[4]-'0');
				stTime.wSecond =(szTime[6]-'0')*10 + (szTime[7]-'0');
				stTime.wMilliseconds =0;

				MaskTimeConvert(stTime,dwSecTime,FALSE);

				StringCchPrintf(szSeconds,sizeof(szSeconds)/sizeof(szSeconds[0]),_T("%08x"),dwSecTime);
				SendDlgItemMessage(hwndDlg,IDC_EDIT_DATETIME_FLAG,WM_SETTEXT,0,(LPARAM)szSeconds);

			}else if(LOWORD(wParam)==IDC_RADIO_DECODE)
			{
				EnableWindow(GetDlgItem(hwndDlg,IDC_EDIT_DATETIME_FLAG),FALSE);
				EnableWindow(GetDlgItem(hwndDlg,IDC_BTN_COPY_CLOSE),FALSE);
				EnableWindow(GetDlgItem(hwndDlg,IDC_EDIT_TIME),TRUE);
				EnableWindow(GetDlgItem(hwndDlg,IDC_DATETIME_PICKER),TRUE);

				GetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_DATETIME_FLAG),szSeconds,sizeof(szSeconds)/sizeof(szSeconds[0]));
				dwSecTime =strtoul(szSeconds,NULL,16);
				MaskTimeConvert(stTime,dwSecTime,TRUE);
				DateTime_SetSystemtime(
					GetDlgItem(hwndDlg, IDC_DATETIME_PICKER),GDT_VALID,
					&stTime);
				StringCchPrintf(szTime,sizeof(szTime)/sizeof(szTime[0]),_T("%02d:%02d:%02d"),stTime.wHour,stTime.wMinute,stTime.wSecond);
				SendDlgItemMessage(hwndDlg,IDC_EDIT_TIME,WM_SETTEXT,0,(LPARAM)szTime);
			}else if(LOWORD(wParam)==IDC_BTN_COPY_CLOSE){
				SendDlgItemMessage(hwndDlg,IDC_EDIT_DATETIME_FLAG,WM_GETTEXT,(WPARAM)sizeof(szSeconds),(LPARAM)szSeconds);
				// ���Ƶ�������
				if (OpenClipboard(hwndDlg) && EmptyClipboard())
				{
					//��ȡ����
					HGLOBAL hMem;
					hMem = GlobalAlloc(GMEM_MOVEABLE, lstrlen(szSeconds)+1);
					TCHAR* pBuff = (TCHAR*)GlobalLock(hMem);
					StringCchCopy(pBuff,lstrlen(szSeconds)+1,szSeconds);
					GlobalUnlock(hMem);
					SetClipboardData(CF_TEXT, hMem);
					CloseClipboard();
				}
				// �رմ���
				SendMessage(hwndDlg, WM_CLOSE, 0, 0);
			}
			return TRUE;
		}
	case MYWM_MSG:
		{
			HWND hEdit =GetDlgItem(hwndDlg,IDC_EDIT_TIME);
			HideCaret(hEdit);
			if (tmPos ==HOUR)
			{
				SendDlgItemMessage(hwndDlg,IDC_EDIT_TIME,EM_SETSEL,0,2);
			}else if (tmPos ==MINUTE)
			{
				SendDlgItemMessage(hwndDlg,IDC_EDIT_TIME,EM_SETSEL,3,5);
			}else if (tmPos ==SECOND)
			{
				SendDlgItemMessage(hwndDlg,IDC_EDIT_TIME,EM_SETSEL,6,-1);
			}
			return TRUE;
		}
	case WM_CLOSE:
		EndDialog(hwndDlg,0);
		return TRUE;
	default:
		break;
	}
	return FALSE;
}

//��ʼ��Pe�ṹ��Ϣ����
BOOL Dlg_OnInitPeStructureDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	TCHAR szTemp[40];
	TCHAR szFormat[1024]={0};
	LPEFile *pPeFile =(LPEFile*)lParam;
	HWND hEdit =GetDlgItem(hwnd,IDC_EDIT_STRUCT_LIST);
	//DOS Header
	PIMAGE_DOS_HEADER pDosHeader =pPeFile->GetDosHeader();
	StringCchPrintf(szFormat,sizeof(szFormat),_T("->Dos Header\r\n"));
	StringCchPrintf(szTemp,sizeof(szTemp),_T("     e_magic:\t0x%04x\r\n"),pDosHeader->e_magic);
	StringCchCat(szFormat,sizeof(szFormat),szTemp);
	StringCchPrintf(szTemp,sizeof(szTemp),_T("     e_cblp:\t0x%04x\r\n"),pDosHeader->e_cblp);
	StringCchCat(szFormat,sizeof(szFormat),szTemp);
	StringCchPrintf(szTemp,sizeof(szTemp),_T("     e_cp:\t\t0x%04x\r\n"),pDosHeader->e_cp);
	StringCchCat(szFormat,sizeof(szFormat),szTemp);
	StringCchPrintf(szTemp,sizeof(szTemp),_T("     e_crlc:\t\t0x%04x\r\n"),pDosHeader->e_crlc);
	StringCchCat(szFormat,sizeof(szFormat),szTemp);
	StringCchPrintf(szTemp,sizeof(szTemp),_T("     e_cparhdr:\t0x%04x\r\n"),pDosHeader->e_cparhdr);
	StringCchCat(szFormat,sizeof(szFormat),szTemp);
	StringCchPrintf(szTemp,sizeof(szTemp),_T("     e_minalloc:\t0x%04x\r\n"),pDosHeader->e_minalloc);
	StringCchCat(szFormat,sizeof(szFormat),szTemp);
	StringCchPrintf(szTemp,sizeof(szTemp),_T("     e_maxalloc:\t0x%04x\r\n"),pDosHeader->e_maxalloc);
	StringCchCat(szFormat,sizeof(szFormat),szTemp);
	StringCchPrintf(szTemp,sizeof(szTemp),_T("     e_ss:\t\t0x%04x\r\n"),pDosHeader->e_ss);
	StringCchCat(szFormat,sizeof(szFormat),szTemp);
	StringCchPrintf(szTemp,sizeof(szTemp),_T("     e_sp:\t\t0x%04x\r\n"),pDosHeader->e_sp);
	StringCchCat(szFormat,sizeof(szFormat),szTemp);
	StringCchPrintf(szTemp,sizeof(szTemp),_T("     e_csum:\t0x%04x\r\n"),pDosHeader->e_csum);
	StringCchCat(szFormat,sizeof(szFormat),szTemp);
	StringCchPrintf(szTemp,sizeof(szTemp),_T("     e_ip:\t\t0x%04x\r\n"),pDosHeader->e_ip);
	StringCchCat(szFormat,sizeof(szFormat),szTemp);
	StringCchPrintf(szTemp,sizeof(szTemp),_T("     e_cs:\t\t0x%04x\r\n"),pDosHeader->e_cs);
	StringCchCat(szFormat,sizeof(szFormat),szTemp);
	StringCchPrintf(szTemp,sizeof(szTemp),_T("     e_lfarlc:\t0x%04x\r\n"),pDosHeader->e_lfarlc);
	StringCchCat(szFormat,sizeof(szFormat),szTemp);
	StringCchPrintf(szTemp,sizeof(szTemp),_T("     e_ovno:\t0x%04x\r\n"),pDosHeader->e_ovno);
	StringCchCat(szFormat,sizeof(szFormat),szTemp);
	StringCchPrintf(szTemp,sizeof(szTemp),_T("     e_res[4]:\t0x%04x"),pDosHeader->e_res[0]);
	StringCchCat(szFormat,sizeof(szFormat),szTemp);
	for (int i=1;i<4;i++)
	{
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%04x"),pDosHeader->e_res[i]);
		StringCchCat(szFormat,sizeof(szFormat),szTemp);
	}
	StringCchCat(szFormat,sizeof(szFormat),_T("\r\n"));
	StringCchPrintf(szTemp,sizeof(szTemp),_T("     e_oemid:\t0x%04x\r\n"),pDosHeader->e_oemid);
	StringCchCat(szFormat,sizeof(szFormat),szTemp);
	StringCchPrintf(szTemp,sizeof(szTemp),_T("     e_oeminfo:\t0x%04x\r\n"),pDosHeader->e_oeminfo);
	StringCchCat(szFormat,sizeof(szFormat),szTemp);
	StringCchPrintf(szTemp,sizeof(szTemp),_T("     e_res2[10]:\t0x%04x"),pDosHeader->e_res2[0]);
	StringCchCat(szFormat,sizeof(szFormat),szTemp);
	for (int i=1;i<10;i++)
	{
		StringCchPrintf(szTemp,sizeof(szTemp),_T("%04x"),pDosHeader->e_res2[i]);
		StringCchCat(szFormat,sizeof(szFormat),szTemp);
	}
	StringCchCat(szFormat,sizeof(szFormat),_T("\r\n"));
	StringCchPrintf(szTemp,sizeof(szTemp),_T("     e_lfanew:\t0x%08x\r\n"),pDosHeader->e_lfanew);
	StringCchCat(szFormat,sizeof(szFormat),szTemp);

	//File Header
	PIMAGE_FILE_HEADER pFileHeader=pPeFile->GetNtFileHeader();
	StringCchCat(szFormat,sizeof(szFormat),_T("\r\n->File Header\r\n"));
	StringCchPrintf(szTemp,sizeof(szTemp),_T("     Machine:\t\t0x%04x\r\n"),pFileHeader->Machine);
	StringCchCat(szFormat,sizeof(szFormat),szTemp);
	StringCchPrintf(szTemp,sizeof(szTemp),_T("     NumberOfSection:\t0x%04x\r\n"),pFileHeader->NumberOfSections);
	StringCchCat(szFormat,sizeof(szFormat),szTemp);
	StringCchPrintf(szTemp,sizeof(szTemp),_T("     TimeDateStamp:\t\t0x%08x\r\n"),pFileHeader->TimeDateStamp);
	StringCchCat(szFormat,sizeof(szFormat),szTemp);
	StringCchPrintf(szTemp,sizeof(szTemp),_T("     PointerToSymbolTable:\t0x%08x\r\n"),pFileHeader->PointerToSymbolTable);
	StringCchCat(szFormat,sizeof(szFormat),szTemp);
	StringCchPrintf(szTemp,sizeof(szTemp),_T("     SizeOfOptionalHeader:\t0x%04x\r\n"),pFileHeader->SizeOfOptionalHeader);
	StringCchCat(szFormat,sizeof(szFormat),szTemp);
	StringCchPrintf(szTemp,sizeof(szTemp),_T("     Characteristics:\t\t0x%04x\r\n"),pFileHeader->Characteristics);
	StringCchCat(szFormat,sizeof(szFormat),szTemp);

	//Optional Header
	StringCchCat(szFormat,sizeof(szFormat),_T("\r\n->Optional Header\r\n"));
	StringCchCat(szFormat,sizeof(szFormat),_T("~~o(>_<)o ~~����~��д��\r\n"));

	Edit_SetText(hEdit,szFormat);
	return TRUE;
}

//Pe�ṹ��Ϣ���ڹ���
BOOL CALLBACK Dlg_PeStructure(HWND hwndDlg, UINT uMsg,  WPARAM wParam, LPARAM lParam)
{

	switch(uMsg)
	{
	case WM_INITDIALOG:
		return Dlg_OnInitPeStructureDialog(hwndDlg,NULL,lParam);
	case WM_COMMAND:
		if (LOWORD(wParam)==IDC_BTN_OK)
		{
			EndDialog(hwndDlg,0);
			return TRUE;
		}
	default:
		break;
	}
	return FALSE;
}

//Pe��Ϣ�Ի��򴰿ڹ���
BOOL CALLBACK Dlg_PeProc(HWND hwndDlg, 
	UINT uMsg, 
	WPARAM wParam, 
	LPARAM lParam)
{
	static LPEFile *pPeFile=new LPEFile;
	PIMAGE_NT_HEADERS pNtHeader=NULL;
	TCHAR szInfoFormat[10]={0};

	switch(uMsg)
	{
	case WM_INITDIALOG:
		if(!pPeFile->LoadPEFile(szPeFilePath))
		{
			MessageBox(hwndDlg,_T("�޷����ļ�!"),_T("ERROR"),MB_OK|MB_ICONERROR);
			EndDialog(hwndDlg,0);
			return TRUE;
		}
		if (!pPeFile->IsPeFile())
		{
			MessageBox(hwndDlg,_T("��Ч��PE�ļ�!"),_T("ERROR"),MB_OK|MB_ICONERROR);
			EndDialog(hwndDlg,0);
			return TRUE;
		}
		pNtHeader=pPeFile->GetNtHeader();
		StringCchPrintf(szInfoFormat,sizeof(szInfoFormat),_T("%08x"),pNtHeader->OptionalHeader.AddressOfEntryPoint);  //��ڵ�
		SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_ENTRY),szInfoFormat);
		StringCchPrintf(szInfoFormat,sizeof(szInfoFormat),_T("%08x"),pNtHeader->OptionalHeader.ImageBase);                //�����ַ
		SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_IMAGE_BASE),szInfoFormat);
		StringCchPrintf(szInfoFormat,sizeof(szInfoFormat),_T("%08x"),pNtHeader->OptionalHeader.SizeOfImage);             //�����С
		SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_IMAGE_SIZE),szInfoFormat);
		StringCchPrintf(szInfoFormat,sizeof(szInfoFormat),_T("%08x"),pNtHeader->OptionalHeader.BaseOfCode);             //�����ַ
		SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_CODE_BASE),szInfoFormat);
		StringCchPrintf(szInfoFormat,sizeof(szInfoFormat),_T("%08x"),pNtHeader->OptionalHeader.BaseOfData);             //���ݻ�ַ
		SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_DATA_BASE),szInfoFormat);
		StringCchPrintf(szInfoFormat,sizeof(szInfoFormat),_T("%08x"),pNtHeader->OptionalHeader.SectionAlignment);    //�����
		SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_SECTION_ALIGNMENT),szInfoFormat);
		StringCchPrintf(szInfoFormat,sizeof(szInfoFormat),_T("%08x"),pNtHeader->OptionalHeader.FileAlignment);          //�ļ�����
		SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_FILE_ALIGNMENT),szInfoFormat);
		StringCchPrintf(szInfoFormat,sizeof(szInfoFormat),_T("%04x"),pNtHeader->OptionalHeader.Magic);                     //��־��
		SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_FLAG),szInfoFormat);
		StringCchPrintf(szInfoFormat,sizeof(szInfoFormat),_T("%04x"),pNtHeader->OptionalHeader.Subsystem);              //��ϵͳ
		SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_SUBSYSTEM),szInfoFormat);
		StringCchPrintf(szInfoFormat,sizeof(szInfoFormat),_T("%04x"),pNtHeader->FileHeader.NumberOfSections);          //������Ŀ
		SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_SECTION_NUM),szInfoFormat);
		StringCchPrintf(szInfoFormat,sizeof(szInfoFormat),_T("%08x"),pNtHeader->FileHeader.TimeDateStamp);               //����ʱ���־
		SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_DATATIME),szInfoFormat);
		StringCchPrintf(szInfoFormat,sizeof(szInfoFormat),_T("%08x"),pNtHeader->OptionalHeader.SizeOfHeaders);         //���״�С
		SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_HEADER_SIZE),szInfoFormat);
		StringCchPrintf(szInfoFormat,sizeof(szInfoFormat),_T("%04x"),pNtHeader->FileHeader.Characteristics);                  //����ֵ
		SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_CHARACTER),szInfoFormat);
		StringCchPrintf(szInfoFormat,sizeof(szInfoFormat),_T("%08x"),pNtHeader->OptionalHeader.CheckSum);                //У���
		SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_CHECKSUM),szInfoFormat);
		StringCchPrintf(szInfoFormat,sizeof(szInfoFormat),_T("%04x"),pNtHeader->FileHeader.SizeOfOptionalHeader);        //��ѡͷ����С
		SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_OPTIONAL_SIZE),szInfoFormat);
		StringCchPrintf(szInfoFormat,sizeof(szInfoFormat),_T("%08x"),pNtHeader->OptionalHeader.NumberOfRvaAndSizes);  //RVA������С
		SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT_RVA_SIZE),szInfoFormat);

		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_BTN_OK:
			EndDialog(hwndDlg,0);
			break;
		case IDC_BTN_SAVE:
			MessageBox(hwndDlg,_T("û��ʵ��(*^__^*)"),_T("��ʾ"),MB_OK|MB_ICONINFORMATION);
			break;
		case IDC_BTN_SECTION:
			DialogBoxParam(g_hInstance,MAKEINTRESOURCE(IDD_DLG_SECTION),hwndDlg,Dlg_PeSectionProc,(LPARAM)pPeFile);
			break;
		case IDC_BTN_DIRECTORY:
			DialogBoxParam(g_hInstance,MAKEINTRESOURCE(IDD_DLG_DIRECTORY),hwndDlg,Dlg_PeDirProc,(LPARAM)pPeFile);
			break;
		case IDC_BTN_POS_CALC:
			DialogBoxParam(g_hInstance,MAKEINTRESOURCE(IDD_DLG_CALC),hwndDlg,Dlg_PeCalcProc,(LPARAM)pPeFile);
			break;
		case IDC_BTN_TIME_FLAG:
			DialogBoxParam(g_hInstance,MAKEINTRESOURCE(IDD_DLG_DATETIME),hwndDlg,Dlg_PeDatetimeProc,(LPARAM)pPeFile);
			break;
		case IDC_BTN_STRUCTURE:
			DialogBoxParam(g_hInstance,MAKEINTRESOURCE(IDD_DLG_STRUCT),hwndDlg,Dlg_PeStructure,(LPARAM)pPeFile);
			break;
		default:
			break;
		}		
		return TRUE;
	case WM_CLOSE:
		EndDialog(hwndDlg,0);
		return TRUE;
	case WM_DESTROY:
		pPeFile->Free();
		return TRUE;
	default:
		break;
	}
	return FALSE;
}


//�������Ի����Command
BOOL Dlg_OnCommand(HWND hwnd, WPARAM wParam,  LPARAM lParam) 
{

	OPENFILENAME ofn={0};


	//LOWORD(wParam)  Ϊ�ӿؼ���ID
	//HIWORD(wParam)  Ϊ�ӿؼ����¼�
	//lParam  �����Ϣ�ǲ˵���Ϣ�����ӿؼ���Ϣ����Ϊ�����ӿؼ�ID�Ͳ˵���ID��ͬ
	switch(LOWORD(wParam))
	{
	case IDC_BTN_PE:
		ZeroMemory(&ofn,sizeof(ofn));
		ofn.lStructSize =sizeof(ofn);
		ofn.hwndOwner =hwnd;
		ofn.lpstrFilter =szFilter;
		ofn.lpstrFile =szPeFilePath;
		ofn.nMaxFile =MAX_PATH;
		ofn.Flags =OFN_PATHMUSTEXIST;
		ofn.lpstrDefExt =szDefExt;
		ofn.lpstrTitle =szOpenCaption;
		if (GetOpenFileName(&ofn))
		{
			DialogBox(g_hInstance,MAKEINTRESOURCE(IDD_DLG_PE),hwnd,Dlg_PeProc);
		}
		return TRUE;
	case IDC_BTN_ABOUT:
		DialogBox(g_hInstance,MAKEINTRESOURCE(IDD_DLG_ABOUT),hwnd,Dlg_AboutProc);
		return TRUE;
	case IDCANCEL:
		EndDialog(hwnd,0);
		return TRUE;
	}
	return FALSE;
}

//ö�ٽ���ģ�麯��
BOOL EnumModules(HWND hwnd)
{
	DWORD dwPId;
	TCHAR szPID[10]={0};                  //����ID
	TCHAR szPath[MAX_PATH];         //ģ��·��
	TCHAR szBaseAddr[10];               //�����ַ
	TCHAR szBaseSize[10];                //�����С
	HANDLE hModuleSnapshot =NULL;
	MODULEENTRY32 me32 = {0};
	BOOL fOk=FALSE;
	LVITEM lvi;
	SHFILEINFO fi;
	int nImgIdx=0;
	DWORD dwIdx=0;

	HWND hProLV =GetDlgItem(hwnd,IDC_LIST_PROCESS);
	HWND hModLV =GetDlgItem(hwnd,IDC_LIST_MODULE);
	ListView_SetImageList(hModLV,g_hImageList,LVSIL_SMALL);
	ListView_DeleteAllItems(hModLV);

	int nItem=ListView_GetSelectionMark(hProLV);
	if (nItem ==-1)
	{
		return FALSE;
	}

	ListView_GetItemText(hProLV,nItem,1,szPID,sizeof(szPID));
	int idx=strspn(szPID,_T("0"));
	if (idx <0 || idx >7)
	{
		return FALSE;
	}

	StringCchCopy(szPID,sizeof(szPID),szPID+idx);
	dwPId =strtoul(szPID,NULL,16);      //�ַ���ת��Ϊ16������ -->PID

	hModuleSnapshot =CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,dwPId);
	if (hModuleSnapshot ==INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	ZeroMemory(&lvi,sizeof(lvi));
	lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
	lvi.state		= 0;
	lvi.stateMask	= 0;

	me32.dwSize = sizeof(MODULEENTRY32);
	fOk =Module32First(hModuleSnapshot, &me32);
	for (; fOk; fOk =Module32Next(hModuleSnapshot,&me32),dwIdx++)
	{
		StringCchPrintf(szBaseAddr,sizeof(szBaseAddr),_T("%08x"),me32.modBaseAddr);
		StringCchPrintf(szBaseSize,sizeof(szBaseSize),_T("%08x"),me32.modBaseSize);
		StringCchPrintf(szPath,sizeof(szPath),_T("%s"),me32.szExePath);
		if(SHGetFileInfo(szPath,0,&fi,sizeof(fi),SHGFI_ICON|SHGFI_SYSICONINDEX|SHGFI_SMALLICON))
		{
			nImgIdx =ImageList_AddIcon(g_hImageList,fi.hIcon);
		}else
		{
			nImgIdx =0;
		}

		lvi.pszText =szPath;
		lvi.cchTextMax=MAX_PATH;
		lvi.iImage =nImgIdx;
		lvi.iItem =dwIdx;
		ListView_InsertItem(hModLV,&lvi);
		ListView_SetItemText(hModLV,dwIdx,1,szBaseAddr);
		ListView_SetItemText(hModLV,dwIdx,2,szBaseSize);

	}

	return TRUE;
}

//�������Ի���Notify��Ϣ
BOOL Dlg_OnNotify(HWND hwnd, WPARAM wParam,  LPARAM lParam)
{
	WORD Id=LOWORD(wParam);
	DWORD nCode=((LPNMHDR)lParam)->code;
	switch(Id)
	{
	case IDC_LIST_PROCESS:
		if( nCode== NM_CLICK)
			return EnumModules(hwnd);
	default:
		break;
	}
	return FALSE;
}

//���Ի��򴰿ڹ���
BOOL CALLBACK Dlg_Proc(
	HWND hwndDlg, 
	UINT uMsg, 
	WPARAM wParam, 
	LPARAM lParam
	)
{
	switch(uMsg)
	{
	case  WM_INITDIALOG:
		g_hWnd =hwndDlg;
		return	Dlg_OnInitDialog(hwndDlg,NULL,lParam);
	case WM_COMMAND:
		return Dlg_OnCommand(hwndDlg,wParam,lParam);
	case WM_NOTIFY:
		return Dlg_OnNotify(hwndDlg,wParam,lParam);
	case WM_CLOSE:
		EndDialog(hwndDlg,0);
		return TRUE;
	default:
		break;
	}

	return FALSE;
}


int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd)
{
	//InitCommonControls();
	g_hInstance =hInstance;
	INITCOMMONCONTROLSEX icce;     

	icce.dwSize = sizeof(icce);
	icce.dwICC = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&icce);        

	DialogBox(hInstance,MAKEINTRESOURCE(IDD_DLG_MAIN),NULL,Dlg_Proc);
	return 0;
}