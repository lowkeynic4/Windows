#pragma once
#include "lpefile.h"

class LPEImport :
	public LPEFile
{
public:
	LPEImport(void);
	virtual ~LPEImport(void);
	void operator=(LPEFile& lPeFile);
	PIMAGE_IMPORT_DESCRIPTOR GetFirstImportDesc(void);       //获得第一个dll导入表
	PIMAGE_IMPORT_DESCRIPTOR GetImportDesc(int Index);      //获得指定的dll导入表
	int GetImportDesCount();                                                       //导入表中存在的dll个数
	DWORD GetImportThunk(PIMAGE_IMPORT_DESCRIPTOR pImportDesc);    //获得导入表中Thunk
	PIMAGE_IMPORT_BY_NAME GetImportFucByName(DWORD RavThunk);    //从导入表的Thunk得到导入函数
};
