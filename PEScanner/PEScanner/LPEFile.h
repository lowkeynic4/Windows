#pragma once
#include <Windows.h>

//自定义结构体
typedef struct _MAP_FILE_STRUCT 
{
	HANDLE	hFile;               //打开的文件句柄
	HANDLE	hMapping;      //文件映射到内存的句柄
	LPVOID		ImageBase;     //文件数据
	DWORD	dwFileSize;      //文件大小
}MAP_FILE_STRUCT,*PMAP_FILE_STRUCT;


//文件打开处理方式方式
enum{
	LFILE_FROM_NOMAL =0,
	LFILE_FROM_FILE,
	LFILE_FROM_MEMORY
};

class LPEFile{
public:
		LPEFile();
		virtual ~LPEFile();
		void operator=(LPEFile& lPeFile);                     //重载=，对象间赋值
		BOOL LoadPEFile(LPCTSTR lpFileName);           //装载PE文件
		BOOL IsPeFile();                                               //判断是不是有效PE文件
		BOOL Free();                                                    //释放PE文件
		BYTE* GetImageBase();                                    //获得镜像基址 
		DWORD GetImageSize();                                 //获得镜像大小
		LPVOID RvaToPtr(DWORD dwRVA);                 //RVA到VA的转换
		LPVOID GetDirectoryEntryToData(USHORT DirectoryEntry);      //获得数据目录的起始地址
		PIMAGE_DOS_HEADER GetDosHeader();                                  //得到Dos头
		PIMAGE_NT_HEADERS GetNtHeader();                                    //得到NT头
		PIMAGE_FILE_HEADER GetNtFileHeader();                               //得到文件头
		PIMAGE_OPTIONAL_HEADER GetNtOptionalHeader();             //得到可选头
		PIMAGE_DATA_DIRECTORY GetDataDirectory();                       //得到数据目录
		PIMAGE_DATA_DIRECTORY GetDataDirectory(int nIdx);           //得到指定的数据目录
		PIMAGE_SECTION_HEADER GetFirstSectionHeader();               //得到第一个节
		PIMAGE_SECTION_HEADER GetSection(int nIdx);                     //得到指定的节
		int GetSectionCount();                                         //得到节的数目
		BOOL SetInfo(MAP_FILE_STRUCT *pMapFile);      //用打开的文件载入一个PE
		MAP_FILE_STRUCT* GetInfo(void);                       //得到打开文件的信息

private:
	//文件的打开状态
	BYTE m_bLoadMod;    
	//用于存储打开的文件句柄，文件数据，文件大小
	MAP_FILE_STRUCT *m_pstMapFile;

};