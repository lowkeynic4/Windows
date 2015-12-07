
// zlibCompressDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "zlibCompress.h"
#include "zlibCompressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CzlibCompressDlg 对话框




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


// CzlibCompressDlg 消息处理程序

BOOL CzlibCompressDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CzlibCompressDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CzlibCompressDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
DWORD GetFileSize(TCHAR tcFilePath[])
{
	HANDLE hFile = CreateFile(tcFilePath, GENERIC_READ , 0, NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile==NULL||hFile==INVALID_HANDLE_VALUE)
	{
		MessageBox(0,"打开文件失败",tcFilePath,0);
	}

	DWORD  dwFileSize= 0;

	dwFileSize=GetFileSize(hFile,  NULL);

	CloseHandle(hFile);

	return dwFileSize;
}
UINT ReadFile(TCHAR file_name[],char * buff)//把文件读到buff内存
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
		m_strFilePath = dlg.GetPathName();  //全路径加文件名
		//m_strFileName = dlg.GetFileName();  //带扩展名的文件名
		m_strFileName = dlg.GetFileTitle();  //不带扩展名的文件名
		lpszFileName = (LPCSTR)m_strFilePath; 

		filePath = (LPSTR)(LPCTSTR)m_strFilePath;

		DWORD dwFileSize = GetFileSize(filePath);
		char * buff=(char*)malloc(dwFileSize);//为文件分内 (LPSTR)(LPCTSTR)是将cstring转为TCHAR
		ReadFile(filePath,buff);//将文件读到内存

		//以下是压缩
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
		//保存已经压缩的数据
		HANDLE handle =CreateFile(csOutName,GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_ALWAYS,0,0);
		DWORD size = 0;
		WriteFile(handle,pDataCompress,lenthCompress,&size,0);
		CloseHandle(handle);
		MessageBox("保存在选择文件的目录，文件名后加zlib");
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
		m_strFilePath = dlg.GetPathName();  //全路径加文件名
		//m_strFileName = dlg.GetFileName();  //带扩展名的文件名
		m_strFileName = dlg.GetFileTitle();  //不带扩展名的文件名
		lpszFileName = (LPCSTR)m_strFilePath; 

		filePath = (LPSTR)(LPCTSTR)m_strFilePath;

		DWORD dwFileSize = GetFileSize(filePath);
		char * buff=(char*)malloc(dwFileSize);//为文件分内 (LPSTR)(LPCTSTR)是将cstring转为TCHAR
		ReadFile(filePath,buff);//将文件读到内存

		DWORD dw_uncompress=1024*1024*100;

		BYTE* pCompress=new BYTE [dw_uncompress];
		int err = ::uncompress(pCompress,&dw_uncompress,(const Bytef *)buff,dwFileSize);//zlib进行解压
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
		HANDLE hFileWrite=CreateFile(csOutName,GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_ALWAYS,0,0);//把读到的文件写到子文件
		if(hFileWrite==INVALID_HANDLE_VALUE) 	
		{
			CloseHandle(hFileWrite);
		}
		DWORD size = 0;
		WriteFile(hFileWrite,pCompress,dw_uncompress,&size,0);//这里写解压后的数据
		CloseHandle(hFileWrite);

		MessageBox("保存在选择文件的目录，文件名后zlib去掉");
	}  
	else  
	{  
		return;
	}

	
}

void CzlibCompressDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	OnCancel();
}
