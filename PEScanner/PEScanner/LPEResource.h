#pragma once
#include "lpefile.h"

class LPEResource :public LPEFile
{
public:
	LPEResource(void);
	virtual ~LPEResource(void);
	void operator=(LPEFile& lPeFile);
	PIMAGE_RESOURCE_DIRECTORY GetResDir();                       //�����ԴĿ¼  
	PIMAGE_RESOURCE_DIRECTORY_ENTRY GetResDirEntry();               //�����ԴĿ¼�����λ��
	int GetResDirEntryCount();              //�����ԴĿ¼�����
};
