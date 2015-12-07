#pragma once
#include "LPEFile.h"

class LPESection :public LPEFile
{
public:
	LPESection(void);
	virtual ~LPESection(void);
	void operator=(LPEFile& lPeFile);
	DWORD OffsetToRva(DWORD dwOffset);         //文件偏移offset转换为RVA   
	DWORD RvaToOffset(DWORD dwRva);             //RVA转换为文件偏移offset
	DWORD GetVRk(int nSeciotnIndex);                 //根据节的索引得到VRk
	int OffsetToSectionIndex(DWORD dwOffset);   //根据文件偏移得到相应的节
	int RvaToSectionIndex(DWORD dwRva);           //根据RVA得到相应的节
	BOOL RvaToSectionName(DWORD dwRva,TCHAR szName[10]);  //根据RVA得到相应的节名
	BOOL OffsetToReadBytes(DWORD dwOffset,UCHAR szBuf[10]);  //根据Offset读取文件中字节数据
	BOOL VaToReadBytes(DWORD dwVa,UCHAR szBuf[10]);  //根据VA读取文件中字节数据
};
