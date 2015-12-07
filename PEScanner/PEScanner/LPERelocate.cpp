#include "LPERelocate.h"

LPERelocate::LPERelocate(void)
{
}

LPERelocate::~LPERelocate(void)
{
}

void LPERelocate::operator=(LPEFile& lPeFile)
{
	SetInfo(lPeFile.GetInfo());
}

PIMAGE_BASE_RELOCATION LPERelocate::GetFirstRelocBlock()
{
	return (PIMAGE_BASE_RELOCATION)GetDirectoryEntryToData(IMAGE_DIRECTORY_ENTRY_BASERELOC);
}

PIMAGE_BASE_RELOCATION LPERelocate::GetRelocBlock(int Idx)
{
	if (Idx <0 || Idx>GetRelocBlocksCount())
	{
		return NULL;
	}
	PIMAGE_BASE_RELOCATION pTempReloc =GetFirstRelocBlock();
	for (int i=0; i<Idx; i++)
	{
		pTempReloc =(PIMAGE_BASE_RELOCATION)((DWORD)pTempReloc+pTempReloc->SizeOfBlock);
	}
	return pTempReloc;
}

int LPERelocate::GetRelocBlocksCount()
{
	int nCount=0;
	PIMAGE_BASE_RELOCATION pTempReloc=GetFirstRelocBlock();
	while (pTempReloc->SizeOfBlock)
	{
		nCount ++;
		pTempReloc =(PIMAGE_BASE_RELOCATION)((DWORD)pTempReloc+pTempReloc->SizeOfBlock);
	}
	return nCount;
}

