#pragma once
#include "lpefile.h"

class LPERelocate :public LPEFile
{
public:
	LPERelocate(void);
	virtual ~LPERelocate(void);
	void operator=(LPEFile& lPeFile);
	int GetRelocBlocksCount();                   //�õ��ض�λ�������
	PIMAGE_BASE_RELOCATION GetFirstRelocBlock();       //�õ���һ���ض�λ��
	PIMAGE_BASE_RELOCATION GetRelocBlock(int Idx);    //�õ�ָ���������ض�λ��
	
	
};
