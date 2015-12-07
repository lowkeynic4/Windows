#pragma once
#include "LPEFile.h"

class LPESection :public LPEFile
{
public:
	LPESection(void);
	virtual ~LPESection(void);
	void operator=(LPEFile& lPeFile);
	DWORD OffsetToRva(DWORD dwOffset);         //�ļ�ƫ��offsetת��ΪRVA   
	DWORD RvaToOffset(DWORD dwRva);             //RVAת��Ϊ�ļ�ƫ��offset
	DWORD GetVRk(int nSeciotnIndex);                 //���ݽڵ������õ�VRk
	int OffsetToSectionIndex(DWORD dwOffset);   //�����ļ�ƫ�Ƶõ���Ӧ�Ľ�
	int RvaToSectionIndex(DWORD dwRva);           //����RVA�õ���Ӧ�Ľ�
	BOOL RvaToSectionName(DWORD dwRva,TCHAR szName[10]);  //����RVA�õ���Ӧ�Ľ���
	BOOL OffsetToReadBytes(DWORD dwOffset,UCHAR szBuf[10]);  //����Offset��ȡ�ļ����ֽ�����
	BOOL VaToReadBytes(DWORD dwVa,UCHAR szBuf[10]);  //����VA��ȡ�ļ����ֽ�����
};
