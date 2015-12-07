// IpcScanDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IpcScan.h"
#include "IpcScanDlg.h"
#include <IO.H>
#pragma comment(lib,"mpr")
#pragma comment(lib,"ws2_32.lib")
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//�����̲߳����ṹ
struct ThreadParam
{
	int iPort;      //�����Ķ˿ں�
	TCHAR ip[16];    //ip��ַ���ĸ��ֶ�

};
int iThreadNum;
CCriticalSection CriticalSection; //�߳�ͬ������
CCriticalSection CriticalSectionChild;
CCriticalSection CriticalOut;
CIpcScanDlg* pCIpcScanDlg;
HANDLE handleSemaphore;
int end = 0;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
	
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CIpcScanDlg dialog




CIpcScanDlg::CIpcScanDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CIpcScanDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);


	m_thread = 0;
	pCIpcScanDlg = this;
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2,2), &wsadata);
	iState = 0;

}

void CIpcScanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER, m_slider);
	//  DDX_Control(pDX, IDC_EDIT_THREAD, m_thread);
	DDX_Control(pDX, IDC_IPADDR_START, m_addrStart);
	DDX_Control(pDX, IDC_IPADDR_END, m_addrStop);
	DDX_Control(pDX, IDC_LIST_PROCESS, m_process);
	DDX_Control(pDX, IDC_LIST_RESULT, m_result);
	DDX_Control(pDX, IDC_BUTTON_START, m_start);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_stop);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_save);
	DDX_Control(pDX, IDC_BUTTON_CLEAR, m_clear);
	DDX_Text(pDX, IDC_EDIT_THREAD, m_thread);
	DDV_MinMaxInt(pDX, m_thread, 1, 100);
}

BEGIN_MESSAGE_MAP(CIpcScanDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_START, &CIpcScanDlg::OnBnClickedButtonStart)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CIpcScanDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CIpcScanDlg::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CIpcScanDlg::OnBnClickedButtonSave)
END_MESSAGE_MAP()


// CIpcScanDlg message handlers

BOOL CIpcScanDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	
	m_slider.SetRange(5,100);//���û����ؼ������ݷ�Χ
	m_slider.SetPos(5);//���û����ؼ��ĳ�ʼֵ
	m_slider.SetTicFreq(5);	//���ÿ̶�Ƶ��
	m_thread = 5;
	UpdateData(FALSE);
	

	m_result.InsertColumn( 0, "Host", LVCFMT_CENTER, 100);//������
	m_result.InsertColumn( 1, "Username", LVCFMT_CENTER, 100 );
	m_result.InsertColumn( 2, "Password", LVCFMT_CENTER, 100);


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CIpcScanDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CIpcScanDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CIpcScanDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//ɨ����̺߳���
UINT ThreadFun( LPVOID pParam )
{
	OutputDebugString("�����߳�");
	TCHAR ip[20];   //���ڴ��ip��ַ
	int iPort = ((ThreadParam *)pParam)->iPort; //��ɨ��˿ں�
	sprintf(ip,"%s",((ThreadParam *)pParam)->ip);  //���ĸ��ֶκϲ���ip��ַ
	SOCKET WinSocket;
	struct sockaddr_in local_sin;
	NETRESOURCE nr; 
	FILE *userfile,*passfile;
	TCHAR userString[32],passString[32];
	//�����ַ�Ͷ˿�
	local_sin.sin_family = AF_INET;
	local_sin.sin_port = htons(iPort);
	local_sin.sin_addr.s_addr = inet_addr( ip );
	TCHAR szStr[256] = {0};
	//����socket�׽���
	if ((WinSocket = socket (AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) 
	{
		return 0;
	}
	//���Խ������ӣ��ɹ�����ʾ�˿ڿ���
	else
	{
		if (end == 1)
		{
			return 0;
		}
		CriticalOut.Lock();
		memset(szStr,0,256);
		sprintf(szStr,"����ping[%s]\n",ip);
		int iCount = pCIpcScanDlg->m_process.GetCount();
		pCIpcScanDlg->m_process.InsertString(iCount,szStr);
		pCIpcScanDlg->m_process.SetCurSel(pCIpcScanDlg->m_process.GetCount()-1);
		CriticalOut.Unlock();

		if ((connect(WinSocket,(struct sockaddr *)&local_sin,sizeof(local_sin))) == 0)
		{
			DWORD res;
			TCHAR server[100]={0};
			if((userfile=fopen("user.txt","r"))==NULL)
			{
				MessageBox(NULL,"user.txt�ļ�������!","��ʾ",MB_OK);
				exit(0);
			}
			while(fgets(userString,sizeof(userString),userfile)!=NULL)
			{
				if((passfile=fopen("pass.txt","r"))==NULL)
				{
					
					MessageBox(NULL,"pass.txt�ļ�������!","��ʾ",MB_OK);
					exit(0);
				}
				while(fgets(passString,sizeof(passString),passfile)!=NULL)
				{
					if (end == 1)
					{
						return 0;
					}
					strtok(ip,"\n");
					strtok(userString,"\x0a");
					strtok(passString,"\x0a");
					sprintf(server,"\\\\%s\\ipc$",ip);
					strtok(server,"\x0a");
					CriticalOut.Lock();
					sprintf(szStr,"[%s]���ڳ���%s:%s",ip,userString,passString);
					int iCount = pCIpcScanDlg->m_process.GetCount();
					pCIpcScanDlg->m_process.InsertString(iCount,szStr);
					pCIpcScanDlg->m_process.SetCurSel(pCIpcScanDlg->m_process.GetCount()-1);
					CriticalOut.Unlock();
					nr.dwType=RESOURCETYPE_ANY; 
					nr.lpLocalName=NULL;
					nr.lpRemoteName=server; 
					nr.lpProvider=NULL; 	
					res=WNetAddConnection2(&nr,passString,userString,0);//����IPC����
					res=WNetCancelConnection2(server,0,TRUE);//�Ͽ�IPC����
					if(res==ERROR_SUCCESS)
					{
						CriticalSectionChild.Lock();
						int nRow=pCIpcScanDlg->m_result.InsertItem(0, ip);  
						pCIpcScanDlg->m_result.SetItemText(nRow,1,userString);  
						pCIpcScanDlg->m_result.SetItemText(nRow,2,passString);  
						CriticalSectionChild.Unlock();

						return 0;
					}
				}
				fclose(passfile);
			}
			fclose(userfile);
			closesocket( WinSocket );
		}
		else 
		{
			if (end == 1)
			{
				return 0;
			}
			CriticalOut.Lock();
			memset(szStr,0,256);
			sprintf(szStr,"[%s]�޷�pingͨ\n",ip);
			int iCount = pCIpcScanDlg->m_process.GetCount();
			pCIpcScanDlg->m_process.InsertString(iCount,szStr);
			pCIpcScanDlg->m_process.SetCurSel(pCIpcScanDlg->m_process.GetCount()-1);
			CriticalOut.Unlock();
			CriticalSection.Lock();
			iThreadNum--;
			CriticalSection.Unlock();
			closesocket( WinSocket );

			return 0;
		}
	}

}
DWORD WINAPI ThreadFunChild(LPVOID pParam)
{
	iThreadNum = 0;
	BYTE IPByte[4]; 
	u_long uNum1,uNum2;
	char startIp[16]= {0};
	pCIpcScanDlg->m_addrStart.GetAddress(IPByte[0], IPByte[1], IPByte[2], IPByte[3]); 
	char temp1[10], temp2[10], temp3[10], temp4[10]; 
	itoa(IPByte[0], temp1, 10); 
	itoa(IPByte[1], temp2, 10); 
	itoa(IPByte[2], temp3, 10); 
	itoa(IPByte[3], temp4, 10);
	sprintf(startIp,"%s.%s.%s.%s",temp1,temp2,temp3,temp4);
	uNum1 = inet_addr(startIp);
	u_long l1=ntohl(uNum1);

	char stopIp[16] = {0};
	pCIpcScanDlg->m_addrStop.GetAddress(IPByte[0], IPByte[1], IPByte[2], IPByte[3]); 
	itoa(IPByte[0], temp1, 10); 
	itoa(IPByte[1], temp2, 10); 
	itoa(IPByte[2], temp3, 10); 
	itoa(IPByte[3], temp4, 10);
	sprintf(stopIp,"%s.%s.%s.%s",temp1,temp2,temp3,temp4);
	uNum2 = inet_addr(stopIp);
	u_long l2=ntohl(uNum2);

	if (l1>l2)
	{
		::MessageBox(NULL,"IP����ȷ","��ʾ",MB_OK);
		return 0;
	}
	
	for (u_long i = l1; i <= l2; i++)
	{
		
		if (end == 1)
		{
			return 0;
		}
		u_long l=ntohl(i);
		struct in_addr addr;
		addr.S_un.S_addr=l;
		TCHAR *s = inet_ntoa(addr);
		ThreadParam *param = new ThreadParam;
		param->iPort = 139;
		sprintf(param->ip,s);
	//	param->ip = s;
		//�жϵ�ǰ�߳����Ƿ񳬹�����߳����������ѭ��
		while( iThreadNum >= pCIpcScanDlg->m_thread ){}

		//�̼߳�����һ
		CriticalSection.Lock();
		iThreadNum++;
		CriticalSection.Unlock();

		//��ʼ�߳�
		AfxBeginThread(ThreadFun, (LPVOID)param);
	}
	return 0;
}
void CIpcScanDlg::OnBnClickedButtonStart()
{
	end = 0;
	UpdateData(TRUE);


	if (m_addrStart.IsBlank())
	{
		MessageBox("��ʼIP����Ϊ�գ�");
		return;
	}
	if (m_addrStop.IsBlank())
	{
		MessageBox("����IP����Ϊ�գ�");
		return;
	}

//	AfxBeginThread(ThreadFunChild,NULL);
	HANDLE hThread = CreateThread(NULL,0,ThreadFunChild,NULL,0,0);
	DWORD dwRet = 0;  
	MSG msg;  
	while (TRUE)  
	{  
		//wait for m_hThread to be over��and wait for  
		//QS_ALLINPUT��Any message is in the queue��  
		dwRet = MsgWaitForMultipleObjects (1, &hThread,   FALSE, INFINITE, QS_ALLINPUT);  
		switch(dwRet)  
		{  
		case WAIT_OBJECT_0:   
			break; //break the loop  

		case WAIT_OBJECT_0 + 1:  
			//get the message from Queue  
			//and dispatch it to specific window  
			PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);  
			DispatchMessage(&msg);   
			continue;  
		default:  
			break; // unexpected failure  
		}  
		break;  
	} 



}


void CIpcScanDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CSliderCtrl *pSlidCtrl = (CSliderCtrl *)GetDlgItem(IDC_SLIDER);
	int pos = pSlidCtrl->GetPos();
	m_thread = pos;
	UpdateData(FALSE);
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CIpcScanDlg::OnBnClickedButtonStop()
{
	end = 1;
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	CriticalSection.Lock();
	iThreadNum = 0;
	CriticalSection.Unlock();


	
}



void CIpcScanDlg::OnBnClickedButtonClear()
{
	m_process.ResetContent();
	m_result.DeleteAllItems();
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CIpcScanDlg::OnBnClickedButtonSave()
{

	CString strFilePath;
	CString strFilter;
	CTime nowtime;
	CString strExt = ".txt";  
	nowtime =CTime::GetCurrentTime();
	char fileName[256]={0};
	sprintf(fileName,"%s%4d%.2d%.2d %.2d%.2d%.2d", "ipc������ɨ����",    
		nowtime.GetYear(), nowtime.GetMonth(), nowtime.GetDay(),     
		nowtime.GetHour(), nowtime.GetMinute(), nowtime.GetSecond());    ///��Ϊ�ļ������м�û�б����ţ�ʱ����һ��

	strFilter.Format("Text Files (*txt)|*txt|All Files (*.*)|*.*||");
	CFileDialog dlg(FALSE, NULL, fileName, NULL, strFilter);
	if (dlg.DoModal() == IDOK)
	{
		strFilePath = dlg.GetPathName();
		if (strFilePath.Find(strExt) == -1)
		{
			strFilePath += strExt;
		}
		if ( access(strFilePath, 0) == 0 )
		{
			CString strQuery;
			strQuery.Format("%s �Ѿ����ڣ�Ҫ�滻����", strFilePath);
			if ( IDNO == ::MessageBox(m_hWnd, strQuery, "�ļ�����ѯ��", MB_ICONQUESTION | MB_YESNO) )
			{
				return;
			}
		}
	}
	CString str1,str2,str3;
	FILE *file;
	file = fopen(strFilePath,"a+");
	char str[256] = {0};
	int count = m_result.GetItemCount();
	for (int i = 0; i < count; i++)
	{
		str1 = m_result.GetItemText(i,0);
		str2 = m_result.GetItemText(i,1);
		str3 = m_result.GetItemText(i,2);
		sprintf(str,"%s    %s:%s\n",str1,str2,str3);
		fputs(str,file);
	}
	fclose(file);

	
	
	

	




}