#include "LPEImport.h"

LPEImport::LPEImport(void)
{
}

LPEImport::~LPEImport(void)
{
}

void LPEImport::operator=(LPEFile& lPeFile)
{
	SetInfo(lPeFile.GetInfo());
}

PIMAGE_IMPORT_DESCRIPTOR LPEImport::GetFirstImportDesc(void)
{
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc=NULL;
	pImportDesc =(PIMAGE_IMPORT_DESCRIPTOR)GetDirectoryEntryToData(IMAGE_DIRECTORY_ENTRY_IMPORT);
	
	return pImportDesc;

}
PIMAGE_IMPORT_DESCRIPTOR LPEImport::GetImportDesc(int Index)
{
	int i=GetImportDesCount();
	if (Index>=i)
	{
		return NULL;
	}
	PIMAGE_IMPORT_DESCRIPTOR pTempImport=GetFirstImportDesc();
	if (pTempImport!=NULL)
	{
		return &pTempImport[Index];
	}
	return NULL;
}

int LPEImport::GetImportDesCount()
{
	int i=0;
	PIMAGE_IMPORT_DESCRIPTOR pTempImport=GetFirstImportDesc();
	while (pTempImport->FirstThunk)
	{
		i++;
		pTempImport++;
	}
	return i;
}

DWORD LPEImport::GetImportThunk(PIMAGE_IMPORT_DESCRIPTOR pImportDesc)
{
	return pImportDesc->OriginalFirstThunk?pImportDesc->OriginalFirstThunk:pImportDesc->FirstThunk;
}
PIMAGE_IMPORT_BY_NAME LPEImport::GetImportFucByName(DWORD RavThunk)
{
	PIMAGE_IMPORT_BY_NAME pApiName=NULL;
	pApiName=(PIMAGE_IMPORT_BY_NAME)RvaToPtr(RavThunk);
	if (pApiName!=NULL)
	{
		return pApiName;
	}
	return NULL;
}
