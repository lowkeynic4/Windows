
// zlibCompressDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "zlibCompress.h"
#include "zlibCompressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CzlibCompressDlg �Ի���




CzlibCompressDlg::CzlibCompressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CzlibCompressDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CzlibCompressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CzlibCompressDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CzlibCompressDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CzlibCompressDlg::OnBnClickedCancel)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CzlibCompressDlg ��Ϣ�������

BOOL CzlibCompressDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CzlibCompressDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CzlibCompressDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CzlibCompressDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
DWORD GetFileSize(TCHAR tcFilePath[])
{
	HANDLE hFile = CreateFile(tcFilePath, GENERIC_READ , 0, NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile==NULL||hFile==INVALID_HANDLE_VALUE)
	{
		MessageBox(0,"���ļ�ʧ��",tcFilePath,0);
	}

	DWORD  dwFileSize= 0;

	dwFileSize=GetFileSize(hFile,  NULL);

	CloseHandle(hFile);

	return dwFileSize;
}
UINT ReadFile(TCHAR file_name[],char * buff)//���ļ�����buff�ڴ�
{
	HANDLE hFile = CreateFile(file_name, GENERIC_READ , 0, NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile==NULL||hFile==INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	DWORD  dwFileSize= 0;

	dwFileSize=GetFileSize(hFile,  NULL);

	CloseHandle(hFile);

	FILE* fp = _tfopen(file_name, _T("rb"));

	if (fp)
	{
		dwFileSize=fread(buff, 1, dwFileSize, fp);
	}

	fclose(fp);

	return dwFileSize;
}

#include "zlib.h"
#pragma comment(lib,"zlib.lib")

void CzlibCompressDlg::OnBnClickedOk()
{
	CString m_strFilePath,m_strFileName,lpszFileName;
	char szFilter[] = {"All Files(*.*)|*.*||"};  
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter,NULL);  
	TCHAR *filePath,*fileName;
	if(dlg.DoModal() == IDOK)  
	{  
		m_strFilePath = dlg.GetPathName();  //ȫ·�����ļ���
		//m_strFileName = dlg.GetFileName();  //����չ�����ļ���
		m_strFileName = dlg.GetFileTitle();  //������չ�����ļ���
		lpszFileName = (LPCSTR)m_strFilePath; 

		filePath = (LPSTR)(LPCTSTR)m_strFilePath;

		DWORD dwFileSize = GetFileSize(filePath);
		char * buff=(char*)malloc(dwFileSize);//Ϊ�ļ����� (LPSTR)(LPCTSTR)�ǽ�cstringתΪTCHAR
		ReadFile(filePath,buff);//���ļ������ڴ�

		//������ѹ��
		DWORD lenthUncompress = dwFileSize;
		DWORD lenthCompress = (lenthUncompress+12)*1.1;
		BYTE* pDataCompress = new BYTE [lenthCompress];
		int err;
		err=::compress(pDataCompress, &lenthCompress, (const Bytef *)buff, lenthUncompress);

		if (err != Z_OK) 
		{
			char str_error[10];
			sprintf(str_error,"%d",err);
			MessageBox(str_error);
		}

		CString csOutName ="";

		csOutName.Format("%szlib",m_strFilePath);
		//�����Ѿ�ѹ��������
		HANDLE handle =CreateFile(csOutName,GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_ALWAYS,0,0);
		DWORD size = 0;
		WriteFile(handle,pDataCompress,lenthCompress,&size,0);
		CloseHandle(handle);
		MessageBox("������ѡ���ļ���Ŀ¼���ļ������zlib");
	}  
	else  
	{  
		return;
	}

}

void CzlibCompressDlg::OnBnClickedCancel()
{
	CString m_strFilePath,m_strFileName,lpszFileName;
	char szFilter[] = {"All Files(*.*)|*.*||"};  
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter,NULL);  
	TCHAR *filePath,*fileName;
	if(dlg.DoModal() == IDOK)  
	{  
		m_strFilePath = dlg.GetPathName();  //ȫ·�����ļ���
		//m_strFileName = dlg.GetFileName();  //����չ�����ļ���
		m_strFileName = dlg.GetFileTitle();  //������չ�����ļ���
		lpszFileName = (LPCSTR)m_strFilePath; 

		filePath = (LPSTR)(LPCTSTR)m_strFilePath;

		DWORD dwFileSize = GetFileSize(filePath);
		char * buff=(char*)malloc(dwFileSize);//Ϊ�ļ����� (LPSTR)(LPCTSTR)�ǽ�cstringתΪTCHAR
		ReadFile(filePath,buff);//���ļ������ڴ�

		DWORD dw_uncompress=1024*1024*100;

		BYTE* pCompress=new BYTE [dw_uncompress];
		int err = ::uncompress(pCompress,&dw_uncompress,(const Bytef *)buff,dwFileSize);//zlib���н�ѹ
		if (err != Z_OK) 
		{
			return ;
		}

		CString csOutName ="";
		int n=m_strFilePath.ReverseFind('z');
		if (n != -1)
		{
			csOutName = m_strFilePath.Left(n);
		}
		HANDLE hFileWrite=CreateFile(csOutName,GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_ALWAYS,0,0);//�Ѷ������ļ�д�����ļ�
		if(hFileWrite==INVALID_HANDLE_VALUE) 	
		{
			CloseHandle(hFileWrite);
		}
		DWORD size = 0;
		WriteFile(hFileWrite,pCompress,dw_uncompress,&size,0);//����д��ѹ�������
		CloseHandle(hFileWrite);

		MessageBox("������ѡ���ļ���Ŀ¼���ļ�����zlibȥ��");
	}  
	else  
	{  
		return;
	}

	
}

void CzlibCompressDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	
	OnCancel();
}
