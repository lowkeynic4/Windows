#pragma once
#include "lpefile.h"

class LPERelocate :public LPEFile
{
public:
	LPERelocate(void);
	virtual ~LPERelocate(void);
	void operator=(LPEFile& lPeFile);
	int GetRelocBlocksCount();                   //得到重定位块的数量
	PIMAGE_BASE_RELOCATION GetFirstRelocBlock();       //得到第一个重定位块
	PIMAGE_BASE_RELOCATION GetRelocBlock(int Idx);    //得到指定索引的重定位块
	
	
};
