#include "LPEResource.h"

LPEResource::LPEResource(void)
{

}

LPEResource::~LPEResource(void)
{

}

void LPEResource::operator=(LPEFile& lPeFile)
{
	SetInfo(lPeFile.GetInfo());
}

PIMAGE_RESOURCE_DIRECTORY LPEResource::GetResDir()
{
		return (PIMAGE_RESOURCE_DIRECTORY)GetDirectoryEntryToData(IMAGE_DIRECTORY_ENTRY_RESOURCE);
}

PIMAGE_RESOURCE_DIRECTORY_ENTRY LPEResource::GetResDirEntry()
{
	PIMAGE_RESOURCE_DIRECTORY pTempResDir=GetResDir();
	if (pTempResDir !=NULL)
	{
		return (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)pTempResDir+sizeof(IMAGE_RESOURCE_DIRECTORY));
	}
	return NULL;
}
int LPEResource::GetResDirEntryCount()
{
	PIMAGE_RESOURCE_DIRECTORY pTempResDir=GetResDir();
	if (pTempResDir!=NULL)
	{
		return (pTempResDir->NumberOfIdEntries+pTempResDir->NumberOfNamedEntries);
	}
	return 0;
}
