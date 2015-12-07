#pragma once
#include "lpefile.h"

class LPEResource :public LPEFile
{
public:
	LPEResource(void);
	virtual ~LPEResource(void);
	void operator=(LPEFile& lPeFile);
	PIMAGE_RESOURCE_DIRECTORY GetResDir();                       //获得资源目录  
	PIMAGE_RESOURCE_DIRECTORY_ENTRY GetResDirEntry();               //获得资源目录项入口位置
	int GetResDirEntryCount();              //获得资源目录项个数
};
