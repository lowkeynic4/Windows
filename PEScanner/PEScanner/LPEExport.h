#pragma once
#include "lpefile.h"

class LPEExport :public LPEFile
{
public:
	LPEExport(void);
	virtual ~LPEExport(void);
	void operator=(LPEFile& lPeFile);
	PIMAGE_EXPORT_DIRECTORY GetExportTab();   //��õ�����
	int GetFuncCount();                               //��õ��������ĺ�������
	int GetNameCount();                             //��ú������Ƶĸ���
	DWORD GetFirstFunc();                         //��õ�һ��������ַ
	DWORD GetFunc(int nIndex);                //���ָ���ĺ�����ַ
	DWORD GetFuncValue(int nIndex);       //��ú���������
	DWORD GetFirstName();                       //��õ�һ�����ֵĵ�ַ
	DWORD GetName(int nIndex);              //���ָ���������Ƶĵ�ַ
	TCHAR* GetNameValue(int nIndex);         //���ָ���ĺ�������
	DWORD GetFirstNameOrd();                //��õ�һ����������
	DWORD GetNameOrd(int nIndex);       //���ָ���ĺ���������ַ
	WORD GetNameOrdValue(int nIndex);  //��ú�������������
	
};
