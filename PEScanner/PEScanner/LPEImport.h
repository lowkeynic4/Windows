#pragma once
#include "lpefile.h"

class LPEImport :
	public LPEFile
{
public:
	LPEImport(void);
	virtual ~LPEImport(void);
	void operator=(LPEFile& lPeFile);
	PIMAGE_IMPORT_DESCRIPTOR GetFirstImportDesc(void);       //��õ�һ��dll�����
	PIMAGE_IMPORT_DESCRIPTOR GetImportDesc(int Index);      //���ָ����dll�����
	int GetImportDesCount();                                                       //������д��ڵ�dll����
	DWORD GetImportThunk(PIMAGE_IMPORT_DESCRIPTOR pImportDesc);    //��õ������Thunk
	PIMAGE_IMPORT_BY_NAME GetImportFucByName(DWORD RavThunk);    //�ӵ�����Thunk�õ����뺯��
};
