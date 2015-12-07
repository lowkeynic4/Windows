#pragma once
#include "lpefile.h"

class LPEExport :public LPEFile
{
public:
	LPEExport(void);
	virtual ~LPEExport(void);
	void operator=(LPEFile& lPeFile);
	PIMAGE_EXPORT_DIRECTORY GetExportTab();   //获得导出表
	int GetFuncCount();                               //获得导出表导出的函数个数
	int GetNameCount();                             //获得函数名称的个数
	DWORD GetFirstFunc();                         //获得第一个函数地址
	DWORD GetFunc(int nIndex);                //获得指定的函数地址
	DWORD GetFuncValue(int nIndex);       //获得函数的内容
	DWORD GetFirstName();                       //获得第一个名字的地址
	DWORD GetName(int nIndex);              //获得指定函数名称的地址
	TCHAR* GetNameValue(int nIndex);         //获得指定的函数名称
	DWORD GetFirstNameOrd();                //获得第一个函数索引
	DWORD GetNameOrd(int nIndex);       //获得指定的函数索引地址
	WORD GetNameOrdValue(int nIndex);  //获得函数索引的内容
	
};
