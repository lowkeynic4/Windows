#include "LPEExport.h"

LPEExport::LPEExport(void)
{
}

LPEExport::~LPEExport(void)
{
}

void LPEExport::operator=(LPEFile& lPeFile)
{
	SetInfo(lPeFile.GetInfo());
}

PIMAGE_EXPORT_DIRECTORY LPEExport::GetExportTab()
{
	PIMAGE_EXPORT_DIRECTORY pExportDesc =NULL;
	pExportDesc=(PIMAGE_EXPORT_DIRECTORY)GetDirectoryEntryToData(IMAGE_DIRECTORY_ENTRY_EXPORT);
	return pExportDesc;
}

int LPEExport::GetFuncCount()
{
	PIMAGE_EXPORT_DIRECTORY pExportDesc=GetExportTab();
	if (pExportDesc)
	{
		return pExportDesc->NumberOfFunctions;
	}
	return -1;
}

int LPEExport::GetNameCount()
{
	PIMAGE_EXPORT_DIRECTORY pExportDesc=GetExportTab();
	if (pExportDesc)
	{
		return pExportDesc->NumberOfNames;
	}
	return -1;
}

DWORD LPEExport::GetFirstFunc()
{
	PIMAGE_EXPORT_DIRECTORY pExportDesc=GetExportTab();
	if (pExportDesc)
	{
		return pExportDesc->AddressOfFunctions;
	}
	return 0;
}

DWORD LPEExport::GetFunc(int nIndex)
{
	if (nIndex<GetFuncCount())
	{
		DWORD dwFirstFunc=GetFirstFunc();
		return dwFirstFunc+nIndex*4;
	}
	return 0;
}

DWORD LPEExport::GetFuncValue(int nIndex)
{
	DWORD dwFuncRva=GetFunc(nIndex);
	if (dwFuncRva)
	{
		return *(DWORD*)RvaToPtr(dwFuncRva);
	}
	return -1;
}

DWORD LPEExport::GetFirstName()
{
	PIMAGE_EXPORT_DIRECTORY pExportDesc=GetExportTab();
	if (pExportDesc)
	{
		return pExportDesc->AddressOfNames;
	}
	return 0;
}

DWORD LPEExport::GetName(int nIndex)
{
	if (nIndex<GetNameCount())
	{
		DWORD dwFirstName=GetFirstName();
		return dwFirstName+nIndex*4;
	}
	return 0;
}

TCHAR* LPEExport::GetNameValue(int nIndex)
{
	DWORD dwNameRva=GetName(nIndex);
	if (dwNameRva)
	{
		DWORD dwNameValueRva=*(DWORD*)RvaToPtr(dwNameRva);
		return (TCHAR*)RvaToPtr(dwNameValueRva);
	}
	return NULL;
}

DWORD LPEExport::GetFirstNameOrd()
{
	PIMAGE_EXPORT_DIRECTORY pExportDesc=GetExportTab();
	if (pExportDesc)
	{
		return pExportDesc->AddressOfNameOrdinals;
	}
	return 0;
}


DWORD LPEExport::GetNameOrd(int nIndex)
{
	if (nIndex<GetNameCount())
	{
		DWORD dwFirstNameOrd=GetFirstNameOrd();
		return dwFirstNameOrd+nIndex*2;
	}
	return 0;
}

WORD LPEExport::GetNameOrdValue(int nIndex)
{
	DWORD dwNameOrdRva=GetNameOrd(nIndex);
	if (dwNameOrdRva)
	{
		return *(WORD*)RvaToPtr(dwNameOrdRva);
	}
	return -1;
}
