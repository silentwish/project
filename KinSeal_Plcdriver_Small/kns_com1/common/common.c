#include "stdio.h"
#include "def.h"
#include "smm.h"


//������������
#ifdef COM0	
U32 AD1=0x003e0000;   
U32 adcom=0x003e6000; 
U32 COMad=0x003e2000; 
#endif

#ifdef COM1	
U32 AD1=0x003e1000;   
U32 adcom=0x003e7000; 
U32 COMad=0x003e4000; 
#endif

U32 PE=0x003e8c00; 

U32 LB_Address=0x003e8010; 			//���ָ��LB�ṹ��ĵ�ַ
U32 LW_Address=0x003e8020; 			//���ָ��LW�ṹ��ĵ�ַ
U32 RWI_Address=0x003e8030; 			//���ָ��RWI�ṹ��ĵ�ַ

U32 LB;
U32 LW;
U32 RWI;

