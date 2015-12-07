#include "LPESection.h"

LPESection::LPESection(void)
{

}

LPESection::~LPESection(void)
{

}

void LPESection::operator=(LPEFile& lPeFile)
{
	SetInfo(lPeFile.GetInfo());
}

//�ļ�ƫ��offsetת��ΪRVA   
DWORD LPESection::OffsetToRva(DWORD dwOffset)
{
	int nSectionIdx=OffsetToSectionIndex(dwOffset);
	DWORD dwVRk=GetVRk(nSectionIdx);
	return dwVRk+dwOffset;
}
//RVAת��Ϊ�ļ�ƫ��offset
DWORD LPESection::RvaToOffset(DWORD dwRva)
{
	int nSectionIdx=RvaToSectionIndex(dwRva);
	DWORD dwVRk =GetVRk(nSectionIdx);
	return dwRva-dwVRk;
}

 //���ݽڵ������õ�VRk
DWORD LPESection::GetVRk(int nSeciotnIndex)
{
	DWORD dwVRk=0;
	PIMAGE_SECTION_HEADER pTempSectionHeader=GetSection(nSeciotnIndex);
	if (pTempSectionHeader !=NULL)
	{
		dwVRk=pTempSectionHeader->VirtualAddress - pTempSectionHeader->PointerToRawData;
	}
	
	return dwVRk;
}

//�����ļ�ƫ�Ƶõ���Ӧ�Ľ�
int LPESection::OffsetToSectionIndex(DWORD dwOffset)
{
	int iSectionIdx=-1;
	int iSectionCount=GetSectionCount();
	for (int i=0; i<iSectionCount; i++)
	{
		PIMAGE_SECTION_HEADER pTempSectionHeader=GetSection(i);
		if (pTempSectionHeader->PointerToRawData <=dwOffset)
		{
			if (dwOffset <=pTempSectionHeader->PointerToRawData + pTempSectionHeader->SizeOfRawData)
			{
				iSectionIdx =i;
				break;
			}
		}
	}
	return iSectionIdx;
}

//����RVA�õ���Ӧ�Ľ�
int LPESection::RvaToSectionIndex(DWORD dwRva)
{
	int iSectionIdx =-1;
	int iSectionCount =GetSectionCount();
	for(int i=0; i<iSectionCount; i++)
	{
		PIMAGE_SECTION_HEADER pTempSectionHeader=GetSection(i);
		if (pTempSectionHeader->VirtualAddress <=dwRva)
		{
			if (dwRva <=pTempSectionHeader->Misc.VirtualSize + pTempSectionHeader->VirtualAddress)
			{
				iSectionIdx =i;
				break;
			}
		}
	}
	return iSectionIdx;
}

BOOL LPESection::RvaToSectionName(DWORD dwRva,TCHAR szName[10])
{
	if (dwRva <0 || dwRva >=GetImageSize())
	{
		return FALSE;
	}

	DWORD dwHeadSize=424;
	//dwHeadSize+=4+sizeof(IMAGE_FILE_HEADER);
	PIMAGE_FILE_HEADER pTempFileHeader=GetNtFileHeader();
	//dwHeadSize +=pTempFileHeader->SizeOfOptionalHeader;
	DWORD	dwSectionCnt=pTempFileHeader->NumberOfSections;
	dwHeadSize +=dwSectionCnt * sizeof(IMAGE_SECTION_HEADER);
	
	DWORD dwTmp=dwHeadSize%512;

	if(dwTmp!=0)
	{
		dwHeadSize +=512-dwTmp;
	}

	if (dwRva <dwHeadSize && dwRva>=0)
	{
		wsprintf(szName,"%s","����");
		return TRUE;
	}

	int iSectionCount =GetSectionCount();
	for(int i=0; i<iSectionCount; i++)
	{
		PIMAGE_SECTION_HEADER pTempSectionHeader=GetSection(i);
		if (pTempSectionHeader->VirtualAddress <=dwRva)
		{
			if (dwRva <=pTempSectionHeader->Misc.VirtualSize + pTempSectionHeader->VirtualAddress)
			{
				wsprintf(szName,"%s",pTempSectionHeader->Name);
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL LPESection::OffsetToReadBytes(DWORD dwOffset,UCHAR szBuf[10])
{
	MAP_FILE_STRUCT* pstMapFile;
	pstMapFile =GetInfo();
	SetFilePointer(pstMapFile->hFile,dwOffset,0,FILE_BEGIN);
	DWORD dwReaded;
	if(ReadFile(pstMapFile->hFile,(LPVOID)szBuf,10,&dwReaded,NULL))
	{
		if (dwReaded ==10)
		{
			return TRUE;
		}
	}
	return FALSE;

}

BOOL  LPESection::VaToReadBytes(DWORD dwVa,UCHAR szBuf[10])
{
	DWORD dwRva =dwVa -GetNtOptionalHeader()->ImageBase;
	DWORD dwOffset =RvaToOffset(dwRva);
	return OffsetToReadBytes(dwOffset,szBuf);
}