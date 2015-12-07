#pragma once
#include <Windows.h>

//�Զ���ṹ��
typedef struct _MAP_FILE_STRUCT 
{
	HANDLE	hFile;               //�򿪵��ļ����
	HANDLE	hMapping;      //�ļ�ӳ�䵽�ڴ�ľ��
	LPVOID		ImageBase;     //�ļ�����
	DWORD	dwFileSize;      //�ļ���С
}MAP_FILE_STRUCT,*PMAP_FILE_STRUCT;


//�ļ��򿪴���ʽ��ʽ
enum{
	LFILE_FROM_NOMAL =0,
	LFILE_FROM_FILE,
	LFILE_FROM_MEMORY
};

class LPEFile{
public:
		LPEFile();
		virtual ~LPEFile();
		void operator=(LPEFile& lPeFile);                     //����=������丳ֵ
		BOOL LoadPEFile(LPCTSTR lpFileName);           //װ��PE�ļ�
		BOOL IsPeFile();                                               //�ж��ǲ�����ЧPE�ļ�
		BOOL Free();                                                    //�ͷ�PE�ļ�
		BYTE* GetImageBase();                                    //��þ����ַ 
		DWORD GetImageSize();                                 //��þ����С
		LPVOID RvaToPtr(DWORD dwRVA);                 //RVA��VA��ת��
		LPVOID GetDirectoryEntryToData(USHORT DirectoryEntry);      //�������Ŀ¼����ʼ��ַ
		PIMAGE_DOS_HEADER GetDosHeader();                                  //�õ�Dosͷ
		PIMAGE_NT_HEADERS GetNtHeader();                                    //�õ�NTͷ
		PIMAGE_FILE_HEADER GetNtFileHeader();                               //�õ��ļ�ͷ
		PIMAGE_OPTIONAL_HEADER GetNtOptionalHeader();             //�õ���ѡͷ
		PIMAGE_DATA_DIRECTORY GetDataDirectory();                       //�õ�����Ŀ¼
		PIMAGE_DATA_DIRECTORY GetDataDirectory(int nIdx);           //�õ�ָ��������Ŀ¼
		PIMAGE_SECTION_HEADER GetFirstSectionHeader();               //�õ���һ����
		PIMAGE_SECTION_HEADER GetSection(int nIdx);                     //�õ�ָ���Ľ�
		int GetSectionCount();                                         //�õ��ڵ���Ŀ
		BOOL SetInfo(MAP_FILE_STRUCT *pMapFile);      //�ô򿪵��ļ�����һ��PE
		MAP_FILE_STRUCT* GetInfo(void);                       //�õ����ļ�����Ϣ

private:
	//�ļ��Ĵ�״̬
	BYTE m_bLoadMod;    
	//���ڴ洢�򿪵��ļ�������ļ����ݣ��ļ���С
	MAP_FILE_STRUCT *m_pstMapFile;

};