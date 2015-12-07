#include "LPEFile.h"
#include <Dbghelp.h>

#pragma comment(lib,"DbgHelp.lib")

LPEFile::LPEFile()
{
	m_bLoadMod =LFILE_FROM_NOMAL;
	m_pstMapFile =NULL;
}

LPEFile::~LPEFile()
{
	if (Free())
	{
		delete m_pstMapFile;
		m_pstMapFile=NULL;
	}
}
//重载=，对象间赋值
void LPEFile::operator=(LPEFile& lPeFile)
{
	SetInfo(lPeFile.GetInfo());
}

//加载PE文件
BOOL LPEFile::LoadPEFile(LPCTSTR lpFileName)
{
	HANDLE hFile;
	HANDLE hMapping;
	LPVOID ImageBase;
	m_pstMapFile =new MAP_FILE_STRUCT;
	m_bLoadMod =LFILE_FROM_FILE;

	hFile =CreateFile(lpFileName,GENERIC_READ|GENERIC_WRITE,
		FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile ==NULL)
	{
		return FALSE;
	}
	m_pstMapFile->dwFileSize=GetFileSize(hFile,NULL);
	hMapping =CreateFileMapping(hFile,NULL,PAGE_READWRITE,0,m_pstMapFile->dwFileSize,NULL);
	if (hMapping==NULL)
	{
		CloseHandle(hFile);
		return FALSE;
	}
	ImageBase =MapViewOfFile(hMapping,FILE_MAP_ALL_ACCESS,0,0,m_pstMapFile->dwFileSize);
	if (ImageBase==NULL)
	{
		CloseHandle(hFile);
		CloseHandle(hMapping);
		return FALSE;
	}
	m_pstMapFile->hFile =hFile;
	m_pstMapFile->hMapping=hMapping;
	m_pstMapFile->ImageBase=ImageBase;
	return TRUE;
}

//判断是不是有效PE文件
BOOL LPEFile::IsPeFile()
{
	PIMAGE_DOS_HEADER pDosHeader=GetDosHeader();
	if (pDosHeader->e_magic ==IMAGE_DOS_SIGNATURE)
	{
		PIMAGE_NT_HEADERS pNtHeader=GetNtHeader();
		if (pNtHeader->Signature ==IMAGE_NT_SIGNATURE)
		{
			return TRUE;
		}
	}
	return FALSE;
}

//释放内存文件映射
BOOL LPEFile::Free()
{
	if (m_bLoadMod ==LFILE_FROM_FILE)
	{
		if (m_pstMapFile->hMapping !=NULL)
		{
			if(!UnmapViewOfFile(m_pstMapFile->ImageBase))
				return FALSE;
			CloseHandle(m_pstMapFile->hMapping);
			m_pstMapFile->hMapping=NULL;
			if (m_pstMapFile->hFile !=NULL)
			{
				CloseHandle(m_pstMapFile->hFile);
				m_pstMapFile->hFile =NULL;
			}
			return TRUE;
		}
	}
	return FALSE;
}

//获得镜像基址
BYTE* LPEFile::GetImageBase()
{
	if (m_pstMapFile !=NULL)
	{
		return (BYTE*)m_pstMapFile->ImageBase;
	}
	return NULL;
}

//获得镜像大小
DWORD LPEFile::GetImageSize()
{
	if (m_pstMapFile !=NULL)
	{
		return m_pstMapFile->dwFileSize;
	}
	return 0;
}

//获得DOS头
PIMAGE_DOS_HEADER LPEFile::GetDosHeader()
{
	return (PIMAGE_DOS_HEADER)GetImageBase();
}

//获得NT头
PIMAGE_NT_HEADERS LPEFile::GetNtHeader()
{
	PIMAGE_DOS_HEADER pDosHeader=GetDosHeader();
	if (pDosHeader!=NULL)
	{
		return (PIMAGE_NT_HEADERS32)((DWORD)GetImageBase()+pDosHeader->e_lfanew);
	}
	return NULL;
}

//获得文件头
PIMAGE_FILE_HEADER LPEFile::GetNtFileHeader()
{
	PIMAGE_NT_HEADERS pNtHeader=GetNtHeader();
	if (pNtHeader!=NULL)
	{
		return (PIMAGE_FILE_HEADER)&pNtHeader->FileHeader;
	}
	return NULL;
}

//获得可选头
PIMAGE_OPTIONAL_HEADER LPEFile::GetNtOptionalHeader()
{
	PIMAGE_NT_HEADERS pNtHeader=GetNtHeader();
	if (pNtHeader!=NULL)
	{
		return (PIMAGE_OPTIONAL_HEADER32)&pNtHeader->OptionalHeader;
	}
	return NULL;
}

//获得数据目录
PIMAGE_DATA_DIRECTORY LPEFile::GetDataDirectory()
{
	PIMAGE_OPTIONAL_HEADER pOptionalHeader=GetNtOptionalHeader();
	if (pOptionalHeader !=NULL)
	{
		return (PIMAGE_DATA_DIRECTORY)pOptionalHeader->DataDirectory;
	}
	return NULL;
}

//获得指定的数据目录项
PIMAGE_DATA_DIRECTORY LPEFile::GetDataDirectory(int nIdx)
{
	if (nIdx>-1 && nIdx<16)
	{
		PIMAGE_DATA_DIRECTORY pDataDir=GetDataDirectory();
		if (pDataDir !=NULL)
		{
			return &pDataDir[nIdx];
		}
	}
	return NULL;
}

//获得第一个节头
PIMAGE_SECTION_HEADER LPEFile::GetFirstSectionHeader()
{
	PIMAGE_NT_HEADERS pNtHeader=GetNtHeader();
	if (pNtHeader!=NULL)
	{
		return IMAGE_FIRST_SECTION(pNtHeader);  //根据NT头得到首个SECTION的宏
	}
	return NULL;
}

//获得指定的节
PIMAGE_SECTION_HEADER LPEFile::GetSection(int nIdx)
{
	int nCount=GetSectionCount();
	if (nIdx>=nCount||nIdx<0)
	{
		return NULL;
	}
	PIMAGE_SECTION_HEADER pFirstSectionHeader=GetFirstSectionHeader();
	for (int i=0;i<nCount;i++,pFirstSectionHeader++)
	{
		if (i==nIdx)
		{
			return pFirstSectionHeader;
		}
	}
	return NULL;
}

//获得节的数量
int LPEFile::GetSectionCount()
{
	PIMAGE_FILE_HEADER pNtFileHeader=GetNtFileHeader();
	if (pNtFileHeader!=NULL)
	{
		return pNtFileHeader->NumberOfSections;
	}
	return 0;
}

//RVA到VA的转换
LPVOID LPEFile::RvaToPtr(DWORD dwRVA)
{
	PIMAGE_NT_HEADERS  pNtHeader=GetNtHeader();
	if (pNtHeader !=NULL)
	{
		return ImageRvaToVa(pNtHeader,GetImageBase(),dwRVA,NULL);
	}
	return NULL;
}

//从数据目录得到指向相应数据的指针
LPVOID LPEFile::GetDirectoryEntryToData(USHORT DirectoryEntry)
{
	DWORD dwDataStartRva;
	LPVOID pDirData=NULL;

	dwDataStartRva=GetDataDirectory(DirectoryEntry)->VirtualAddress;
	pDirData =RvaToPtr(dwDataStartRva);
	
	return pDirData;
}

 //用打开的文件载入一个PE
BOOL LPEFile::SetInfo(MAP_FILE_STRUCT *pMapFile)
{
	if (pMapFile==NULL)
	{
		return FALSE;
	}
	m_pstMapFile =pMapFile;
	return TRUE;
}

//得到打开文件的信息
MAP_FILE_STRUCT* LPEFile::GetInfo(void)
{
	return m_pstMapFile;
}