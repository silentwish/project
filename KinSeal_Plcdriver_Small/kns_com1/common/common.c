#include "stdio.h"
#include "def.h"
#include "smm.h"


//公共变量定义
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

U32 LB_Address=0x003e8010; 			//获得指向LB结构体的地址
U32 LW_Address=0x003e8020; 			//获得指向LW结构体的地址
U32 RWI_Address=0x003e8030; 			//获得指向RWI结构体的地址

U32 LB;
U32 LW;
U32 RWI;

