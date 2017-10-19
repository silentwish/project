#include "stdio.h"
#include "def.h"
#include "smm.h"


//公共变量定义
U32 AD1=0x004a0000;   
U32 adcom=0x004a3000; 
U32 COMad=0x004a1000; 


U32 PE=0x004c5c00; 

U32 LB_Address=0x004c5010; 			//获得指向LB结构体的地址
U32 LW_Address=0x004c5020; 			//获得指向LW结构体的地址
U32 RWI_Address=0x004c5030; 			//获得指向RWI结构体的地址

U32 LB;
U32 LW;
U32 RWI;

