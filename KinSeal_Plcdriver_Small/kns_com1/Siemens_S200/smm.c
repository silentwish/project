
#include "stdio.h"
#include "def.h"
#include "smm.h"

		
struct Com_struct_D *ptcom;

 
U8 id_val;


/*************************************************************************************************************************

*************************************************************************************************************************/
void Enter_PlcDriver(void)
{
	ptcom=(struct Com_struct_D *)adcom;	
	//sysprintf("s7-200 enter plc driver=%c  ptcom->R_W_Flag=%d \n",ptcom->registerr,ptcom->R_W_Flag);
	switch (ptcom->R_W_Flag)
	{
		case PLC_READ_DATA:					//进入驱动是读数据0
		case PLC_READ_DIGITALALARM:			//进入驱动是读数据,报警10	
		case PLC_READ_TREND:				//进入驱动是读数据,趋势图11
		case PLC_READ_ANALOGALARM:			//进入驱动是读数据,类比报警	12
		case PLC_READ_CONTROL:				//进入驱动是读PLC控制的数据13	
		switch(ptcom->registerr)
		{
			case 'X':	//I
			case 'Y':	//Q
			case 'M':	//M
			case 'H':	//VB_Bit
			case 'L':	//S
			case 'K':	//SM
			case 'T':	//
			case 'C':	//
				Read_Bool();
				break;
			case 'x':	//IW
			case 'y':	//QW
			case 'm':	//MW
			case 'l':	//SW
			case 'v':	//VW
			case 'A':	//AIW
			case 'D':	//VB
			case 'R':	//VD
			case 'N':	//SMW
			case 't':	//TV
			case 'c':	//CV
			Read_Analog();
			break;			
		}
		break;
	case PLC_WRITE_DATA:				//进入驱动是写数据1
		switch(ptcom->registerr)
		{
		case 'Y':
		case 'M':	
		case 'H':	
		case 'L':			
		case 'K':				
		case 'T':			
		case 'C':										
			Set_Reset();
			break;
		case 'y':
		case 'm':		
		case 'l':
		case 'v':
		case 'D':
		case 'R':	
		case 't':
		case 'c':					
			Write_Analog();		
			break;			
		}
		break;	
	case PLC_WRITE_TIME:				//进入驱动是写时间到PLC 3
		switch(ptcom->registerr)
		{
		case 'D':
		case 'R':
		case 'N':	
		case 't':
		case 'c':						
			Write_Time();		
			break;			
		}
		break;	
	case PLC_READ_TIME:				//进入驱动是读取时间到PLC 4
		switch(ptcom->registerr)
		{
		case 'D':
		case 'R':
		case 'N':	
		case 't':
		case 'c':						
			Read_Time();		
			break;			
		}
		break;
	case PLC_WRITE_RECIPE:				//进入驱动是写配方到PLC 5
		switch(*(U8 *)(PE+3))//配方寄存器名称
		{	
		case 'y':
		case 'm':		
		case 'l':
		case 'v':
		case 'D':
		case 'R':	
		case 't':
		case 'c':	
			Write_Recipe();		
			break;			
		}
		break;
	case PLC_READ_RECIPE:				//进入驱动是从PLC读取配方 6
		switch(*(U8 *)(PE+3))//配方寄存器名称
		{
		case 'y':
		case 'm':		
		case 'l':
		case 'v':
		case 'D':
		case 'R':	
		case 't':
		case 'c':					
			Read_Recipe();		
			break;			
		}
		break;							
	case PLC_CHECK_DATA:				//进入驱动是数据处理2
		watchcom();
		break;				
	}	 	 
}

/**************************************************************************************************************************************************
Description: 置位复位函数
***************************************************************************************************************************************************/
void Set_Reset()
{
	U16 aakj;
	int b,stationAdd;
	int a1,a2,a3;
	int YM_Check;
	int value;	//置位或复位
	int i;
		
	switch (ptcom->registerr)
	{
	case 'Y':			//Y
		YM_Check=0x82;
		break;
	case 'M':
		YM_Check=0x83;	//M
		break;	
	case 'H':			//v_bIT
		YM_Check=0x84;
		break;			
	case 'L':			//S
		YM_Check=0x04;
		break;		
	case 'K':			//SM
		YM_Check=0x05;
		break;					
	}
	b=ptcom->address;			// 置位地址
	value=ptcom->writeValue;	//写入的值
	stationAdd=ptcom->plc_address;	//站地址

	a1=(b>>16)&0xff;
	a2=(b>>8)&0xff;
	a3=b&0xff;	
		
	*(U8 *)(AD1+0)=0x68;			//SD--开始符（ 68H）
	*(U8 *)(AD1+1)=0x20;			//Byte 2: LE、 Ler--长度（ 即去掉 Bit0-Bit3,校验码，结束符后的数据长度）。
	*(U8 *)(AD1+2)=0x20;
	
	*(U8 *)(AD1+3)=0x68;			//SD—开始符（ 68H）
	*(U8 *)(AD1+4)=stationAdd;		//站地址  
	*(U8 *)(AD1+5)=0x00;			//SA--源地址
	*(U8 *)(AD1+6)=0x6c;			//FC--功能码 （ 6CH）
	*(U8 *)(AD1+7)=0x32;
	*(U8 *)(AD1+8)=0x01;
	*(U8 *)(AD1+9)=0x00;
	*(U8 *)(AD1+10)=0x00;
	*(U8 *)(AD1+11)=0x01;
	*(U8 *)(AD1+12)=0x01;
	*(U8 *)(AD1+13)=0x00;
	
	*(U8 *)(AD1+14)=0x0e;			//数据块占位字节
	*(U8 *)(AD1+15)=0x00;
	*(U8 *)(AD1+16)=0x05;
	*(U8 *)(AD1+17)=0x05;
	*(U8 *)(AD1+18)=0x01;
	*(U8 *)(AD1+19)=0x12;
	*(U8 *)(AD1+20)=0x0a;
	*(U8 *)(AD1+21)=0x10;

	*(U8 *)(AD1+22)=0x01;			//读取数据的长度
	*(U8 *)(AD1+23)=0x00;
	*(U8 *)(AD1+24)=0x01;			//以字节为单位，连续读取的字节数。
	*(U8 *)(AD1+25)=0x00;
	if (ptcom->registerr=='H')
	{
		*(U8 *)(AD1+26)=0x01;		//存储器类型 01：
	}
	else
	{
		*(U8 *)(AD1+26)=0x00;
	}
/*Byte 27 ： 存储器类型
Byte26 Byte27
Q 0x00 0x82
M 0x00 0x83
V_BIT 0x01 0x84
S 0x00 0x04
SM 0x00 0x05*/
	*(U8 *)(AD1+27)=YM_Check;		//Y,M,V_B,S,SM
	
	*(U8 *)(AD1+28)=a1;				//Byte 28,29,30： 存储器偏移量指针（存储器地址*8）
	*(U8 *)(AD1+29)=a2;				//寄存器地址
	*(U8 *)(AD1+30)=a3;
	
	*(U8 *)(AD1+31)=0x00;
	*(U8 *)(AD1+32)=0x03;
	*(U8 *)(AD1+33)=0x00;
	*(U8 *)(AD1+34)=0x01;
	*(U8 *)(AD1+35)=value;			//置位或复位
	aakj=CalcHe((U8 *)AD1,36);
	a2=aakj&0xff;
	*(U8 *)(AD1+36)=a2;				//和校检
	*(U8 *)(AD1+37)=0x16;			//结束符
	/*第二包数据*/
	*(U8 *)(AD1+38)=0x10;
	*(U8 *)(AD1+39)=stationAdd;
	*(U8 *)(AD1+40)=0x00;
	*(U8 *)(AD1+41)=0x5c;
	aakj=Second_Send_CalcHe((U8 *)(AD1+39),3);
	a2=aakj&0xff;
	*(U8 *)(AD1+42)=a2;				//和校检
	*(U8 *)(AD1+43)=0x16;


//西门子200通信都要发送2次才能完成一次通信
	ptcom->send_length[0]=38;				//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址			
	ptcom->return_length[0]=1;				//返回数据长度
	ptcom->return_start[0]=0;				//返回数据有效开始
	ptcom->return_length_available[0]=0;	//返回有效数据长度	

	
	ptcom->send_length[1]=6;				//发送长度
	ptcom->send_staradd[1]=38;				//发送数据存储地址			
	ptcom->return_length[1]=24;				//返回数据长度
	ptcom->return_start[1]=0;				//返回数据有效开始
	ptcom->return_length_available[1]=0;	//返回有效数据长度	
		
	
	ptcom->send_times=2;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数					
}


void Read_Bool()					//读取数字量的信息
{
	U16 aakj;
	int b,stationAdd;
	int a1,a2,a3;
	int YM_Check;
	int length;
	int i;
	

	length=ptcom->register_length;	//读取长度
	stationAdd=ptcom->plc_address;	//站地址	
	b=ptcom->address;				// 置位地址
	if (length==1)
	{
		length=2;
		ptcom->register_length=2;	//西门子不能只读一个字节，最少一个字
	}
	#if 0
	*(U16*)((U16*)LW+ 600) = ptcom->address;
	*(U16*)((U16*)LW+ 601) = length;
	*(U16*)((U16*)LW+ 599) = b;	
	#endif
/*
到这里的时候开始地址变化了，从每个字节的0位开始，去掉位地址
即M10.7,下载时下载87(转化过)，经过优化后到这里变成80
*/			
			
	switch (ptcom->registerr)
	{
	case 'X':
		YM_Check=0x81;
		a1=b;
		a2=a1/8;
		b=a2*8;				//西门子规定地址要扩大8倍
		a1=(b>>16)&0xff;
		a2=(b>>8)&0xff;
		a3=b&0xff;				
		break;
	case 'Y':
		YM_Check=0x82;
		a1=b;
		a2=a1/8;	
		b=a2*8;				//西门子规定地址要扩大8倍
		a1=(b>>16)&0xff;
		a2=(b>>8)&0xff;
		a3=b&0xff;					
		break;		
	case 'M':
		YM_Check=0x83;
		a1=b;
		a2=a1/8;	
		b=a2*8;				//西门子规定地址要扩大8倍
		a1=(b>>16)&0xff;
		a2=(b>>8)&0xff;
		a3=b&0xff;					
		break;		
	case 'H':				//V_BIT
		YM_Check=0x84;
		a1=b;
		a2=a1/8;
		b=a2*8;				//西门子规定地址要扩大8倍
		a1=(b>>16)&0xff;
		a2=(b>>8)&0xff;
		a3=b&0xff;						
		break;	
	case 'L':				//S
		YM_Check=0x04;
		a1=b;
		a2=a1/8;	
		b=a2*8;				//西门子规定地址要扩大8倍
		a1=(b>>16)&0xff;
		a2=(b>>8)&0xff;
		a3=b&0xff;					
		break;		
	case 'K':				//SM
		YM_Check=0x05;
		a1=b;
		a2=a1/8;
		b=a2*8;				//西门子规定地址要扩大8倍
		a1=(b>>16)&0xff;
		a2=(b>>8)&0xff;
		a3=b&0xff;						
		break;	
	case 'T':				//T
		YM_Check=0x1F;
		a1=(b>>16)&0xff;
		a2=(b>>8)&0xff;
		a3=b&0xff;				
		break;		
	case 'C':				//C
		YM_Check=0x1E;
		a1=(b>>16)&0xff;
		a2=(b>>8)&0xff;
		a3=b&0xff;		
		break;					
	}

	*(U8 *)(AD1+0)=0x68;			//SD,开始符（ 68H）
	*(U8 *)(AD1+1)=0x1b;			//Byte 1, Byte 2: LE、 Ler--长度（ 即去掉 Bit0-Bit3,校验码， Byte 32 后的数据长度）。
	*(U8 *)(AD1+2)=0x1b;
	*(U8 *)(AD1+3)=0x68;			//Byte 3: SD—开始符（ 68H）

	*(U8 *)(AD1+4)=stationAdd;		//站地址
	*(U8 *)(AD1+5)=0x00;			//Byte 5： SA--源地址
	*(U8 *)(AD1+6)=0x6c;			//Byte 6： FC--功能码 （ 6CH）
	*(U8 *)(AD1+7)=0x32;

	*(U8 *)(AD1+8)=0x01;
	*(U8 *)(AD1+9)=0x00;
	*(U8 *)(AD1+10)=0x00;
	*(U8 *)(AD1+11)=0x00;
	
	*(U8 *)(AD1+12)=0x00;
	*(U8 *)(AD1+13)=0x00;
	*(U8 *)(AD1+14)=0x0e;			//Byte14: 数据块占位字节
	*(U8 *)(AD1+15)=0x00;

	*(U8 *)(AD1+16)=0x00;
	*(U8 *)(AD1+17)=0x04;
	*(U8 *)(AD1+18)=0x01;
	*(U8 *)(AD1+19)=0x12;
	
	*(U8 *)(AD1+20)=0x0a;
	*(U8 *)(AD1+21)=0x10;
	
	switch (ptcom->registerr)
	{
	case 'T':		//T
		*(U8 *)(AD1+22)=0x1F;		//Byte 22: 读取数据的长度	
		*(U8 *)(AD1+23)=0x00;
		*(U8 *)(AD1+24)=(length*8) & 0xff;			//读取长度,读取每个T,C时，读取的长度指多少个T
		*(U8 *)(AD1+25)=0x00;		
		break;		
	case 'C':		//C
		*(U8 *)(AD1+22)=0x1E;			
		*(U8 *)(AD1+23)=0x00;	
		*(U8 *)(AD1+24)=(length*8) & 0xff;			//读取长度,读取每个T,C时，读取的长度指多少个T
		*(U8 *)(AD1+25)=0x00;		
		break;	
	default:	
		*(U8 *)(AD1+22)=0x02;			//以字节读取数据
		*(U8 *)(AD1+23)=0x00;	
		*(U8 *)(AD1+24)=length;			//读取长度
		*(U8 *)(AD1+25)=0x00;		
		break;			
	}	
		
	if (ptcom->registerr=='H')
	{
		*(U8 *)(AD1+26)=0x01;
	}
	else
	{
		*(U8 *)(AD1+26)=0x00;
	}
/*
Byte 27 ： 存储器类型
I—0x81
Q—0x82
M—0x83
S—0x04
SM—0x05
*/
	*(U8 *)(AD1+27)=YM_Check;		//X,Y,M,V_BIT
	
	*(U8 *)(AD1+28)=a1;				//开始地址
	*(U8 *)(AD1+29)=a2;
	*(U8 *)(AD1+30)=a3;	
	
	aakj=CalcHe((U8 *)AD1,31);
	a2=aakj&0xff;
	*(U8 *)(AD1+31)=a2;				//校验和，从(DA+SA+DSAP+SSAP+DU) Mod 256 。
	*(U8 *)(AD1+32)=0x16;			//结束符
	/*第二包数据：*/
	*(U8 *)(AD1+33)=0x10;
	*(U8 *)(AD1+34)=stationAdd;
	*(U8 *)(AD1+35)=0x00;
	*(U8 *)(AD1+36)=0x5c;
	aakj=Second_Send_CalcHe((U8 *)(AD1+34),3);
	a2=aakj&0xff;
	*(U8 *)(AD1+37)=a2;				//和校检
	*(U8 *)(AD1+38)=0x16;

//西门子200通信都要发送2次才能完成一次通信
	ptcom->send_length[0]=33;				//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址			
	ptcom->return_length[0]=1;				//返回数据长度
	ptcom->return_start[0]=0;				//返回数据有效开始
	ptcom->return_length_available[0]=0;	//返回有效数据长度	

	
	ptcom->send_length[1]=6;				//发送长度
	ptcom->send_staradd[1]=33;				//发送数据存储地址	
	if (ptcom->registerr=='T')
	{		
		ptcom->return_length[1]=25+2+length*8*5;	//返回数据长度,25个固定，2位是校检和结束符,每个字节读取8个T,C，每个T5个字节返回
	}
	else if(ptcom->registerr=='C')
	{
		ptcom->return_length[1]=25+2+length*8*3;	//返回数据长度,25个固定，2位是校检和结束符,每个字节读取8个T,C，每个T5个字节返回
	}
	else
	{
		ptcom->return_length[1]=25+2+length;	//返回数据长度,25个固定，2位是校检和结束符
	}
	ptcom->return_start[1]=25;				//返回数据有效开始
	ptcom->return_length_available[1]=length;	//返回有效数据长度	
		
	
	ptcom->send_times=2;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数					
	
}



void Read_Analog()				//读模拟量
{
	U16 aakj;
	int b,stationAdd;
	int a1,a2,a3;
	int YM_Check;
	int length;
	int i;
	
	switch (ptcom->registerr)
	{
	case 'x':		//IW
		YM_Check=0x81;
		break;		
	case 'y':		//QW
		YM_Check=0x82;
		break;
	case 'm':		//MW
		YM_Check=0x83;
		break;		
	case 'l':		//SW
		YM_Check=0x04;
		break;		
	case 'N':		//SMW
		YM_Check=0x05;
		break;			
	case 'A':		//AIW
		YM_Check=0x06;
		break;				
	case 'D':		//VB
	case 'R':		//VD
	case 'v':		//VW
		YM_Check=0x84;
		break;	
	case 't':		//TV
		YM_Check=0x1F;
		break;		
	case 'c':		//CV
		YM_Check=0x1E;
		break;				
	}
	length=ptcom->register_length;	//读取长度,//字为单位
	length=length*2;				//读的总字节数
	if (length==1)
	{
		length=2;
		ptcom->register_length=2;	//西门子不能只读一个字节，最少一个字
	}
	stationAdd=ptcom->plc_address;	//站地址	

	if (ptcom->registerr=='R')
	{
		b=ptcom->address * 2;		// 下载时按字地址下载，在这里要放大2倍
		length=length*2;
	}
	else if (ptcom->registerr=='D' || ptcom->registerr=='N' ||ptcom->registerr=='v'|| ptcom->registerr=='x'
		|| ptcom->registerr=='y'|| ptcom->registerr=='m'|| ptcom->registerr=='l')
	{
		b=ptcom->address * 2;		// 下载时按字地址下载，在这里要放大2倍
	}
	else
	{
		b=ptcom->address;
	}
	switch (ptcom->registerr)
	{
	case 'x':		//IW
	case 'y':		//QW
	case 'm':		//MW
	case 'l':		//SW
	case 'A':		//AIW
	
	case 'v':		//VW
	case 'D':		//VB
	case 'R':		//VD
	case 'N':		//SMW
		a1=b;
		b=a1*8;
		a1=(b>>16)&0xff;
		a2=(b>>8)&0xff;
		a3=b&0xff;
		break;	
	case 't':		//t
	case 'c':		//c
		a1=(b>>16)&0xff;
		a2=(b>>8)&0xff;
		a3=b&0xff;
		break;				
	}	
	*(U8 *)(AD1+0)=0x68;
	*(U8 *)(AD1+1)=0x1b;
	*(U8 *)(AD1+2)=0x1b;
	*(U8 *)(AD1+3)=0x68;

	*(U8 *)(AD1+4)=stationAdd;		//站地址
	*(U8 *)(AD1+5)=0x00;
	*(U8 *)(AD1+6)=0x6c;
	*(U8 *)(AD1+7)=0x32;

	*(U8 *)(AD1+8)=0x01;
	*(U8 *)(AD1+9)=0x00;
	*(U8 *)(AD1+10)=0x00;
	*(U8 *)(AD1+11)=0x00;
	
	*(U8 *)(AD1+12)=0x00;
	*(U8 *)(AD1+13)=0x00;
	*(U8 *)(AD1+14)=0x0e;
	*(U8 *)(AD1+15)=0x00;

	*(U8 *)(AD1+16)=0x00;
	*(U8 *)(AD1+17)=0x04;
	*(U8 *)(AD1+18)=0x01;
	*(U8 *)(AD1+19)=0x12;
	
	*(U8 *)(AD1+20)=0x0a;
	*(U8 *)(AD1+21)=0x10;
	
	switch (ptcom->registerr)
	{
	case 'x':		//IW
	case 'y':		//QW
	case 'm':		//MW
	case 'l':		//SW
	case 'v':		//VW
	case 'A':		//AIW
	
	case 'D':		//VB
	case 'R':		//VD
	case 'N':		//SMW	
		*(U8 *)(AD1+22)=0x02;			//以字节读取数据
		*(U8 *)(AD1+23)=0x00;
		*(U8 *)(AD1+24)=length;			//读取长度
		*(U8 *)(AD1+25)=0x00;			
		break;
	case 't':		//t
		*(U8 *)(AD1+22)=0x1F;			
		*(U8 *)(AD1+23)=0x00;
		*(U8 *)(AD1+24)=length;		//读取长度,读取每个T,C时，读取的长度指多少个T
		*(U8 *)(AD1+25)=0x00;			
		break;		
	case 'c':		//c
		*(U8 *)(AD1+22)=0x1E;			
		*(U8 *)(AD1+23)=0x00;
		*(U8 *)(AD1+24)=length;		//读取长度,读取每个T,C时，读取的长度指多少个T
		*(U8 *)(AD1+25)=0x00;				
		break;		
	}	

	if (ptcom->registerr=='D' || ptcom->registerr=='R'|| ptcom->registerr=='v')
	{
		*(U8 *)(AD1+26)=0x01;
	}
	else
	{
		*(U8 *)(AD1+26)=0x00;
	}
	*(U8 *)(AD1+27)=YM_Check;		//X,Y,M
	
	*(U8 *)(AD1+28)=a1;				//开始地址
	*(U8 *)(AD1+29)=a2;
	*(U8 *)(AD1+30)=a3;	
	
	aakj=CalcHe((U8 *)AD1,31);
	a2=aakj&0xff;
	*(U8 *)(AD1+31)=a2;          //校验
	*(U8 *)(AD1+32)=0x16;		

	*(U8 *)(AD1+33)=0x10;
	*(U8 *)(AD1+34)=stationAdd;
	*(U8 *)(AD1+35)=0x00;
	*(U8 *)(AD1+36)=0x5c;
	aakj=Second_Send_CalcHe((U8 *)(AD1+34),3);
	a2=aakj&0xff;
	*(U8 *)(AD1+37)=a2;				//和校检
	*(U8 *)(AD1+38)=0x16;
	
//西门子200通信都要发送2次才能完成一次通信
	ptcom->send_length[0]=33;				//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址			
	ptcom->return_length[0]=1;				//返回数据长度
	ptcom->return_start[0]=0;				//返回数据有效开始
	ptcom->return_length_available[0]=0;	//返回有效数据长度	

	
	ptcom->send_length[1]=6;				//发送长度
	ptcom->send_staradd[1]=33;				//发送数据存储地址	
	if (ptcom->registerr=='t' || ptcom->registerr=='c')
	{	
		if(	ptcom->registerr=='t')
		{
		    ptcom->return_length[1]=25+2+length*5;	//返回数据长度,25个固定，2位是校检和结束符,每个字节读取8个T,C，每个T5个字节返回
		}
		else
		{
			ptcom->return_length[1]=25+2+length*3;	//返回数据长度,25个固定，2位是校检和结束符,每个字节读取8个T,C，每个T5个字节返回
		}
	}
	else
	{
		ptcom->return_length[1]=25+2+length;	//返回数据长度,25个固定，2位是校检和结束符
	}			
	ptcom->return_start[1]=25;				//返回数据有效开始
	ptcom->return_length_available[1]=length;	//返回有效数据长度	
	
	if(ptcom->registerr=='R')
	{
		ptcom->return_length_available[1]=length * 4;
	}
	ptcom->send_times=2;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数	
	
}


void Read_Recipe()								//读取配方
{
	U16 aakj;
	int b;
	int a1,a2,a3,a4,a5;
	int i;
	int datalength;
	int staradd;
	int ps;
	int SendTimes;
	int LastTimeWord,length;							//最后一次发送长度
	int stationAdd;
	
	datalength=ptcom->register_length;		//发送总长度
	staradd=ptcom->address*2;				//开始地址，// 下载时按字地址下载，在这里要放大2倍
	if(datalength>5000)
		datalength=5000;
	
	//每次最多能发送32个D
	if(datalength%32==0)
	{
		SendTimes=datalength/32;
		LastTimeWord=32;//最后一次发送的长度	
	}
	if(datalength%32!=0)
	{
		SendTimes=datalength/32+1;//发送的次数
		LastTimeWord=datalength%32;//最后一次发送的长度	
	}

	ps=39;
	stationAdd=ptcom->plc_address;			//站地址	
	for (i=0;i<SendTimes;i++)
	{
		b=staradd+i*32*2;					// 开始地址,V最小单位是字节
		if (i==SendTimes-1)				//最后一次
		{
			length=LastTimeWord;	//写入的长度
		}
		else
		{
			length=32;					//32个字
		}
		a1=b;
		a1=a1*8;
		a3=a1/0x10000;
		a4=(a1-a3*0x10000)/0x100;
		a5=a1-a3*0x10000-a4*0x100;	

		*(U8 *)(AD1+0+ps*i)=0x68;
		*(U8 *)(AD1+1+ps*i)=0x1b;
		*(U8 *)(AD1+2+ps*i)=0x1b;
		*(U8 *)(AD1+3+ps*i)=0x68;

		*(U8 *)(AD1+4+ps*i)=stationAdd;		//站地址
		*(U8 *)(AD1+5+ps*i)=0x00;
		*(U8 *)(AD1+6+ps*i)=0x6c;
		*(U8 *)(AD1+7+ps*i)=0x32;

		*(U8 *)(AD1+8+ps*i)=0x01;
		*(U8 *)(AD1+9+ps*i)=0x00;
		*(U8 *)(AD1+10+ps*i)=0x00;
		*(U8 *)(AD1+11+ps*i)=0x00;
		
		*(U8 *)(AD1+12+ps*i)=0x00;
		*(U8 *)(AD1+13+ps*i)=0x00;
		*(U8 *)(AD1+14+ps*i)=0x0e;
		*(U8 *)(AD1+15+ps*i)=0x00;

		*(U8 *)(AD1+16+ps*i)=0x00;
		*(U8 *)(AD1+17+ps*i)=0x04;
		*(U8 *)(AD1+18+ps*i)=0x01;
		*(U8 *)(AD1+19+ps*i)=0x12;
		
		*(U8 *)(AD1+20+ps*i)=0x0a;
		*(U8 *)(AD1+21+ps*i)=0x10;
		
		*(U8 *)(AD1+22+ps*i)=0x02;			//以字节读取数据
		*(U8 *)(AD1+23+ps*i)=0x00;	
			
		*(U8 *)(AD1+24+ps*i)=length*2;		//读取长度
		*(U8 *)(AD1+25+ps*i)=0x00;

		switch (ptcom->registerr)
		{
		case 'D'://vb
		case 'v'://vw
		case 'R'://vd
			*(U8 *)(AD1+26+ps*i)=0x01;
			*(U8 *)(AD1+27+ps*i)=0x84;			
			break;	
		case 'm':		//MW	
			*(U8 *)(AD1+26+ps*i)=0x00;
			*(U8 *)(AD1+27+ps*i)=0x83;	
			break;				
		}

		*(U8 *)(AD1+26+ps*i)=0x01;
		*(U8 *)(AD1+27+ps*i)=0x84;			//V
		
		*(U8 *)(AD1+28+ps*i)=a3;				//开始地址
		*(U8 *)(AD1+29+ps*i)=a4;
		*(U8 *)(AD1+30+ps*i)=a5;	
		
		aakj=CalcHe((U8 *)AD1+ps*i,31);
		a2=aakj&0xff;
		*(U8 *)(AD1+31+ps*i)=a2;
		*(U8 *)(AD1+32+ps*i)=0x16;	
		
		ptcom->send_length[i*2]=33;			//发送长度
		ptcom->send_staradd[i*2]=i*ps;		//发送数据存储地址	
		ptcom->send_add[i*2]=b/2;				//读的是这个地址的数据	
		ptcom->send_data_length[i*2]=length;	//不是最后一次都是32个D		
		
		ptcom->return_length[i*2]=1;			//返回数据长度
		ptcom->return_start[i*2]=0;				//返回数据有效开始
		ptcom->return_length_available[i*2]=0;	//返回有效数据长度			
		
		*(U8 *)(AD1+33+ps*i)=0x10;
		*(U8 *)(AD1+34+ps*i)=stationAdd;
		*(U8 *)(AD1+35+ps*i)=0x00;
		*(U8 *)(AD1+36+ps*i)=0x5c;
		aakj=Second_Send_CalcHe((U8 *)(AD1+34+ps*i),3);
		a2=aakj&0xff;
		*(U8 *)(AD1+37+ps*i)=a2;				//和校检
		*(U8 *)(AD1+38+ps*i)=0x16;
		
		ptcom->send_length[i*2+1]=6;		//发送长度
		ptcom->send_staradd[i*2+1]=i*ps+33;			//发送数据存储地址
		ptcom->send_add[i*2+1]=b/2;				//读的是这个地址的数据	
		ptcom->send_data_length[i*2+1]=length;	//不是最后一次都是32个D	
					
		ptcom->return_length[i*2+1]=25+2+length*2;				//返回数据长度
		ptcom->return_start[i*2+1]=25;				//返回数据有效开始
		ptcom->return_length_available[i*2+1]=length*2;	//返回有效数据长度			
		
	}
	ptcom->send_times=SendTimes*2;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数	
	
		
}

void writet(void)								//写定时器 计数器
{
	U16 aakj;
	int b,stationAdd;
	int a1,a2,a3,a4,a5;
	int YM_Check;
	int LE,LER,length;
	int k1,k2,k3,k4,k5;
	int i;

	b=ptcom->address;
	
	length=ptcom->register_length;	//写入的长度
	stationAdd=ptcom->plc_address;	//站地址
	
	a1=b;
	a3=a1/0x10000;
	a4=(a1-a3*0x10000)/0x100;
	a5=a1-a3*0x10000-a4*0x100;				
	
	
	
	switch (ptcom->registerr)
	{	
	case 't':		//t
		YM_Check=0x1F;
		
		k1=4+length*5;
		k2=length;
		k3=5<<(2+length);
		k4=k3&0xff;
		k5=(k3>>8)&0xff;
		LE=0x21+length*5-2;				//length以字为单位
		LER=0x21+length*5-2;
	
		break;		
	case 'c':		//c
		YM_Check=0x1E;
		
		k1=4+length*3;
		k2=length;
		k3=5<<(2+length);
		k4=((length*3*8) >> 0) & 0xff;
		k5=((length*3*8) >> 8) & 0xff;
		LE=0x21+length*3-2;				//length以字为单位
		LER=0x21+length*3-2;
		break;		
	}
	
	
	
			
	*(U8 *)(AD1+0)=0x68;
	*(U8 *)(AD1+1)=LE;
	*(U8 *)(AD1+2)=LER;
	*(U8 *)(AD1+3)=0x68;

	*(U8 *)(AD1+4)=stationAdd;		//站地址
	*(U8 *)(AD1+5)=0x00;
	if(ptcom->registerr == 't')
	{
		*(U8 *)(AD1+6)=0x7c;			//代表写入
	}
	else
	{
		*(U8 *)(AD1+6)=0x6c;			//代表写入
	}
	*(U8 *)(AD1+7)=0x32;

	*(U8 *)(AD1+8)=0x01;
	*(U8 *)(AD1+9)=0x00;
	*(U8 *)(AD1+10)=0x00;
	*(U8 *)(AD1+11)=0x00;


	*(U8 *)(AD1+12)=0x00;
	*(U8 *)(AD1+13)=0x00;
	*(U8 *)(AD1+14)=0x0e;
	*(U8 *)(AD1+15)=0x00;

	*(U8 *)(AD1+16)=k1;
	*(U8 *)(AD1+17)=0x05;
	*(U8 *)(AD1+18)=0x01;
	*(U8 *)(AD1+19)=0x12;

	*(U8 *)(AD1+20)=0x0a;
	*(U8 *)(AD1+21)=0x10;


	switch (ptcom->registerr)
	{
	case 't':		//t
		*(U8 *)(AD1+22)=0x1F;			
		*(U8 *)(AD1+23)=0x00;	
		break;		
	case 'c':		//c
		*(U8 *)(AD1+22)=0x1E;			
		*(U8 *)(AD1+23)=0x00;	
		break;		
	}

	*(U8 *)(AD1+24)=k2;
	*(U8 *)(AD1+25)=0x00;
	*(U8 *)(AD1+26)=0x00;

	*(U8 *)(AD1+27)=YM_Check;		//Y,M
	*(U8 *)(AD1+28)=a3;				//寄存器地址
	*(U8 *)(AD1+29)=a4;				
	*(U8 *)(AD1+30)=a5;
	*(U8 *)(AD1+31)=0x00;
	*(U8 *)(AD1+32)=0x04;
	
	*(U8 *)(AD1+33)=k5;
	*(U8 *)(AD1+34)=k4;
	
	*(U8 *)(AD1+35)=0x00;			//状态
	
	*(U8 *)(AD1+36)=0x00;			//其他未知
	*(U8 *)(AD1+37)=0x00;	
	for (i=0;i<length*2;i++)
	{
		if (i%2==0)
		{
			*(U8 *)(AD1+38+i)=ptcom->U8_Data[i+1];			//高低字节交换
		}
		else
		{
			*(U8 *)(AD1+38+i)=ptcom->U8_Data[i-1];		
		}	
	}

	aakj=CalcHe((U8 *)AD1,38+length*2);
	a2=aakj&0xff;
	*(U8 *)(AD1+38+length*2)=a2;				//和校检
	*(U8 *)(AD1+39+length*2)=0x16;
	
	
	*(U8 *)(AD1+40+length*2)=0x10;
	*(U8 *)(AD1+41+length*2)=stationAdd;
	*(U8 *)(AD1+42+length*2)=0x00;
	*(U8 *)(AD1+43+length*2)=0x5c;
	aakj=Second_Send_CalcHe((U8 *)(AD1+41+length*2),3);
	a2=aakj&0xff;
	*(U8 *)(AD1+44+length*2)=a2;				//和校检
	*(U8 *)(AD1+45+length*2)=0x16;	
	

//西门子200通信都要发送2次才能完成一次通信
	ptcom->send_length[0]=39+length*2+1;				//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址			
	ptcom->return_length[0]=1;				//返回数据长度
	ptcom->return_start[0]=0;				//返回数据有效开始
	ptcom->return_length_available[0]=0;	//返回有效数据长度	

	
	ptcom->send_length[1]=6;				//发送长度
	ptcom->send_staradd[1]=39+length*2+1;		//发送数据存储地址			
	ptcom->return_length[1]=24;				//返回数据长度
	ptcom->return_start[1]=0;				//返回数据有效开始
	ptcom->return_length_available[1]=0;	//返回有效数据长度	
		
	
	ptcom->send_times=2;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数	
}

void writec(void)								//写定时器 计数器
{
	U16 aakj;
	int b,stationAdd;
	int a1,a2,a3,a4,a5;
	int YM_Check;
	int LE,LER,length;
	int k1,k2,k3,k4,k5;
	int i;

	b=ptcom->address;
	
	length=ptcom->register_length;	//写入的长度
	stationAdd=ptcom->plc_address;	//站地址
	
	a1=b;
	a3=a1/0x10000;
	a4=(a1-a3*0x10000)/0x100;
	a5=a1-a3*0x10000-a4*0x100;				
	
	
	
	switch (ptcom->registerr)
	{	
	case 't':		//t
		YM_Check=0x1F;
		
		k1=4+length*5;
		k2=length;
		k3=5<<(2+length);
		k4=k3&0xff;
		k5=(k3>>8)&0xff;
		LE=0x21+length*5-2;				//length以字为单位
		LER=0x21+length*5-2;
	
		break;		
	case 'c':		//c
		YM_Check=0x1E;
		
		k1=4+length*3;
		k2=length;
		k3=5<<(2+length);
		k4=((length*3*8) >> 0) & 0xff;
		k5=((length*3*8) >> 8) & 0xff;
		LE=0x21+length*3-2;				//length以字为单位
		LER=0x21+length*3-2;
		break;		
	}
	
	
	
			
	*(U8 *)(AD1+0)=0x68;
	*(U8 *)(AD1+1)=LE;
	*(U8 *)(AD1+2)=LER;
	*(U8 *)(AD1+3)=0x68;

	*(U8 *)(AD1+4)=stationAdd;		//站地址
	*(U8 *)(AD1+5)=0x00;
	if(ptcom->registerr == 't')
	{
		*(U8 *)(AD1+6)=0x7c;			//代表写入
	}
	else
	{
		*(U8 *)(AD1+6)=0x6c;			//代表写入
	}
	*(U8 *)(AD1+7)=0x32;

	*(U8 *)(AD1+8)=0x01;
	*(U8 *)(AD1+9)=0x00;
	*(U8 *)(AD1+10)=0x00;
	*(U8 *)(AD1+11)=0x00;


	*(U8 *)(AD1+12)=0x00;
	*(U8 *)(AD1+13)=0x00;
	*(U8 *)(AD1+14)=0x0e;
	*(U8 *)(AD1+15)=0x00;

	*(U8 *)(AD1+16)=k1;
	*(U8 *)(AD1+17)=0x05;
	*(U8 *)(AD1+18)=0x01;
	*(U8 *)(AD1+19)=0x12;

	*(U8 *)(AD1+20)=0x0a;
	*(U8 *)(AD1+21)=0x10;


	switch (ptcom->registerr)
	{
	case 't':		//t
		*(U8 *)(AD1+22)=0x1F;			
		*(U8 *)(AD1+23)=0x00;	
		break;		
	case 'c':		//c
		*(U8 *)(AD1+22)=0x1E;			
		*(U8 *)(AD1+23)=0x00;	
		break;		
	}

	*(U8 *)(AD1+24)=k2;
	*(U8 *)(AD1+25)=0x00;
	*(U8 *)(AD1+26)=0x00;

	*(U8 *)(AD1+27)=YM_Check;		//Y,M
	*(U8 *)(AD1+28)=a3;				//寄存器地址
	*(U8 *)(AD1+29)=a4;				
	*(U8 *)(AD1+30)=a5;
	*(U8 *)(AD1+31)=0x00;
	*(U8 *)(AD1+32)=0x04;
	
	*(U8 *)(AD1+33)=k5;
	*(U8 *)(AD1+34)=k4;
	
	*(U8 *)(AD1+35)=0x00;			//状态
	
	//*(U8 *)(AD1+36)=0x00;			//其他未知
	//*(U8 *)(AD1+37)=0x00;	
	for (i=0;i<length*2;i++)
	{
		if (i%2==0)
		{
			*(U8 *)(AD1+36+i)=ptcom->U8_Data[i+1];			//高低字节交换
		}
		else
		{
			*(U8 *)(AD1+36+i)=ptcom->U8_Data[i-1];		
		}	
	}

	aakj=CalcHe((U8 *)AD1,36+length*2);
	a2=aakj&0xff;
	*(U8 *)(AD1+36+length*2)=a2;				//和校检
	*(U8 *)(AD1+37+length*2)=0x16;
	
	
	*(U8 *)(AD1+38+length*2)=0x10;
	*(U8 *)(AD1+39+length*2)=stationAdd;
	*(U8 *)(AD1+40+length*2)=0x00;
	*(U8 *)(AD1+41+length*2)=0x5c;
	aakj=Second_Send_CalcHe((U8 *)(AD1+39+length*2),3);
	a2=aakj&0xff;
	*(U8 *)(AD1+42+length*2)=a2;				//和校检
	*(U8 *)(AD1+43+length*2)=0x16;	
	

//西门子200通信都要发送2次才能完成一次通信
	ptcom->send_length[0]=38+length*2;				//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址			
	ptcom->return_length[0]=1;				//返回数据长度
	ptcom->return_start[0]=0;				//返回数据有效开始
	ptcom->return_length_available[0]=0;	//返回有效数据长度	

	
	ptcom->send_length[1]=6;				//发送长度
	ptcom->send_staradd[1]=37+length*2+1;		//发送数据存储地址			
	ptcom->return_length[1]=24;				//返回数据长度
	ptcom->return_start[1]=0;				//返回数据有效开始
	ptcom->return_length_available[1]=0;	//返回有效数据长度	
		
	
	ptcom->send_times=2;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数	
}

void Write_Analog()								//写模拟量
{
	U16 aakj;
	int b,stationAdd;
	int a1,a2,a3,a4,a5;
	int YM_Check;
	int LE,LER,length;
	int k1,k2,k3,k4,k5;
	int i;
		
	if (ptcom->registerr=='t' )
	{
		writet();
		return;
	}
	else if(ptcom->registerr=='c')
	{
		writec();
		return;
	}	
			
	switch (ptcom->registerr)
	{
	case 'y':		//QW
		YM_Check=0x82;
		break;
	case 'm':		//MW
		YM_Check=0x83;
		break;		
	case 'l':		//SW
		YM_Check=0x04;
		break;		
	case 'N':		//SMW
		YM_Check=0x05;
		break;			
	case 'A':		//AIW
		YM_Check=0x06;
		break;				
	case 'D':		//VB
	case 'R':		//VD
	case 'v':		//VW
		YM_Check=0x84;
		break;	
	case 't':		//TV
		YM_Check=0x1F;
		break;		
	case 'c':		//CV
		YM_Check=0x1E;
		break;			
	}
	
	length=ptcom->register_length;	//写入的长度
	stationAdd=ptcom->plc_address;	//站地址
	if(ptcom->registerr == 'R')
	{
		b=ptcom->address * 2;				// 下载时按字地址下载，在这里要放大2倍
	}
	else
	{
		b=ptcom->address*2;				// 下载时按字地址下载，在这里要放大2倍
	}
	//sysprintf("ptcom->address=%d\n",ptcom->address);
	
	a1=b;
	a1=a1*8;
	a3=a1/0x10000;
	a4=(a1-a3*0x10000)/0x100;
	a5=a1-a3*0x10000-a4*0x100;	

	LE=0x21+length*2-2;				//length以字为单位
	LER=0x21+length*2-2;
	k1=4+length*2;
	k2=length;
	k3=k2<<4;
	k4=k3&0xff;
	k5=(k3>>8)&0xff;
			
	*(U8 *)(AD1+0)=0x68;
	*(U8 *)(AD1+1)=LE;
	*(U8 *)(AD1+2)=LER;
	*(U8 *)(AD1+3)=0x68;

	*(U8 *)(AD1+4)=stationAdd;		//站地址
	*(U8 *)(AD1+5)=0x00;
	*(U8 *)(AD1+6)=0x6c;			//代表写入
	*(U8 *)(AD1+7)=0x32;

	*(U8 *)(AD1+8)=0x01;
	*(U8 *)(AD1+9)=0x00;
	*(U8 *)(AD1+10)=0x00;
	*(U8 *)(AD1+11)=0xa1;

	*(U8 *)(AD1+12)=0xa1;
	*(U8 *)(AD1+13)=0x00;
	*(U8 *)(AD1+14)=0x0e;
	*(U8 *)(AD1+15)=0x00;

	*(U8 *)(AD1+16)=k1;
	*(U8 *)(AD1+17)=0x05;
	*(U8 *)(AD1+18)=0x01;
	*(U8 *)(AD1+19)=0x12;

	*(U8 *)(AD1+20)=0x0a;
	*(U8 *)(AD1+21)=0x10;
		
	*(U8 *)(AD1+22)=0x02;			//以字节读取数据
	*(U8 *)(AD1+23)=0x00;	

	*(U8 *)(AD1+24)=k2 * 2;
	*(U8 *)(AD1+25)=0x00;
	if (ptcom->registerr=='D' || ptcom->registerr=='R'|| ptcom->registerr=='v')
	{
		*(U8 *)(AD1+26)=0x01;
	}
	else
	{
		*(U8 *)(AD1+26)=0x00;
	}	
	*(U8 *)(AD1+27)=YM_Check;		//Y,M
	*(U8 *)(AD1+28)=a3;				//寄存器地址
	*(U8 *)(AD1+29)=a4;				
	*(U8 *)(AD1+30)=a5;
	*(U8 *)(AD1+31)=0x00;
	*(U8 *)(AD1+32)=0x04;
	
	*(U8 *)(AD1+33)=k5;
	*(U8 *)(AD1+34)=k4;
	
	if (ptcom->registerr=='R')//vd
	{
		for (i=0;i<length*2;i=i+4)
		{
			*(U8 *)(AD1+35+i)=ptcom->U8_Data[i+1];	
			*(U8 *)(AD1+36+i)=ptcom->U8_Data[i+0];	
			*(U8 *)(AD1+37+i)=ptcom->U8_Data[i+3];	
			*(U8 *)(AD1+38+i)=ptcom->U8_Data[i+2];	
		}	
	}
	else
	{
		for (i=0;i<length*2;i++)
		{
			if (i%2==0)
			{
				*(U8 *)(AD1+35+i)=ptcom->U8_Data[i+1];			//高低字节交换
			}
			else
			{
				*(U8 *)(AD1+35+i)=ptcom->U8_Data[i-1];		
			}	
		}	
	}

	aakj=CalcHe((U8 *)AD1,35+length*2);
	a2=aakj&0xff;
	*(U8 *)(AD1+35+length*2)=a2;				//和校检
	*(U8 *)(AD1+36+length*2)=0x16;
	
	*(U8 *)(AD1+37+length*2)=0x10;
	*(U8 *)(AD1+38+length*2)=stationAdd;
	*(U8 *)(AD1+39+length*2)=0x00;
	*(U8 *)(AD1+40+length*2)=0x5c;
	aakj=Second_Send_CalcHe((U8 *)(AD1+38+length*2),3);
	a2=aakj&0xff;
	*(U8 *)(AD1+41+length*2)=a2;				//和校检
	*(U8 *)(AD1+42+length*2)=0x16;

	//sysprintf("*(U8 *)(AD1+4)=%d\n",*(U8 *)(AD1+4));
	//sysprintf("*(U8 *)(AD1+27)=0x%x\n",*(U8 *)(AD1+27));
	//sysprintf("*(U8 *)(AD1+24)=%d\n",*(U8 *)(AD1+24));
	
	//sysprintf("*(U8 *)(AD1+28)=%d\n",*(U8 *)(AD1+28));
	//sysprintf("*(U8 *)(AD1+29)=%d\n",*(U8 *)(AD1+29));
	//sysprintf("*(U8 *)(AD1+30)=%d\n",*(U8 *)(AD1+30));
	
//西门子200通信都要发送2次才能完成一次通信
	ptcom->send_length[0]=36+length*2+1;				//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址			
	ptcom->return_length[0]=1;				//返回数据长度
	ptcom->return_start[0]=0;				//返回数据有效开始
	ptcom->return_length_available[0]=0;	//返回有效数据长度	

	
	ptcom->send_length[1]=6;				//发送长度
	ptcom->send_staradd[1]=36+length*2+1;		//发送数据存储地址			
	ptcom->return_length[1]=24;				//返回数据长度
	ptcom->return_start[1]=0;				//返回数据有效开始
	ptcom->return_length_available[1]=0;	//返回有效数据长度	
		
	
	ptcom->send_times=2;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数	
}

void Write_Time()
{
	Write_Analog();
}

void Read_Time()									//从PLC读取时间
{
	Read_Analog();
}

void Write_Recipe()								//写配方到PLC
{
	int datalength;
	int staradd;
	int SendTimes;
	int LastTimeWord;							//最后一次发送长度
	int i,j;
	int ps;
	int b;
	int a1,a2,a3,a4,a5;
	int k1,k2,k3,k4,k5;	
	U16 aakj;
	int LE,LER;
	int stationAdd,length;
	
	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));//数据长度
	staradd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//数据长度
	staradd=staradd*2;			// 下载时按字地址下载，在这里要放大2倍
	//每次最多能发送32个D
	if(datalength%32==0)
	{
		SendTimes=datalength/32;
		LastTimeWord=32;//最后一次发送的长度	
	}
	if(datalength%32!=0)
	{
		SendTimes=datalength/32+1;//发送的次数
		LastTimeWord=datalength%32;//最后一次发送的长度	
	}	
	
	ps=107;
	stationAdd=*(U8 *)(PE+4);	//站地址	
	for (i=0;i<SendTimes;i++)
	{
		b=staradd+i*32*2;					// 开始地址,V最小单位是字节
		if (i==SendTimes-1)				//最后一次
		{
			length=LastTimeWord;	//写入的长度
		}
		else
		{
			length=32;					//32个字
		}
		a1=b;
		a1=a1*8;
		
		a3=a1/0x10000;
		a4=(a1-a3*0x10000)/0x100;
		a5=a1-a3*0x10000-a4*0x100;	

		LE=0x21+length*2-2;				//length以字为单位
		LER=0x21+length*2-2;
		k1=4+length*2;
		k2=length;
		k3=k2<<4;
		k4=k3&0xff;
		k5=(k3>>8)&0xff;

		*(U8 *)(AD1+0+ps*i)=0x68;
		*(U8 *)(AD1+1+ps*i)=LE;
		*(U8 *)(AD1+2+ps*i)=LER;
		*(U8 *)(AD1+3+ps*i)=0x68;

		*(U8 *)(AD1+4+ps*i)=stationAdd;		//站地址
		*(U8 *)(AD1+5+ps*i)=0x00;
		*(U8 *)(AD1+6+ps*i)=0x6c;			//代表写入
		*(U8 *)(AD1+7+ps*i)=0x32;

		*(U8 *)(AD1+8+ps*i)=0x01;
		*(U8 *)(AD1+9+ps*i)=0x00;
		*(U8 *)(AD1+10+ps*i)=0x00;
		*(U8 *)(AD1+11+ps*i)=0x00;


		*(U8 *)(AD1+12+ps*i)=0x00;
		*(U8 *)(AD1+13+ps*i)=0x00;
		*(U8 *)(AD1+14+ps*i)=0x0e;
		*(U8 *)(AD1+15+ps*i)=0x00;

		*(U8 *)(AD1+16+ps*i)=k1;
		*(U8 *)(AD1+17+ps*i)=0x05;
		*(U8 *)(AD1+18+ps*i)=0x01;
		*(U8 *)(AD1+19+ps*i)=0x12;

		*(U8 *)(AD1+20+ps*i)=0x0a;
		*(U8 *)(AD1+21+ps*i)=0x10;

		*(U8 *)(AD1+22+ps*i)=0x04;
		*(U8 *)(AD1+23+ps*i)=0x00;
		*(U8 *)(AD1+24+ps*i)=k2;			//长度，字
		*(U8 *)(AD1+25+ps*i)=0x00;
		
		switch (ptcom->registerr)
		{
		case 'D'://vb
		case 'v'://vw
		case 'R'://vd
			*(U8 *)(AD1+26+ps*i)=0x01;
			*(U8 *)(AD1+27+ps*i)=0x84;				//V
			break;	
		case 'm':		//MW
			*(U8 *)(AD1+26+ps*i)=0x00;
			*(U8 *)(AD1+27+ps*i)=0x83;				
			break;			
		}
		
		*(U8 *)(AD1+28+ps*i)=a3;				//寄存器地址
		*(U8 *)(AD1+29+ps*i)=a4;				
		*(U8 *)(AD1+30+ps*i)=a5;
		*(U8 *)(AD1+31+ps*i)=0x00;
		*(U8 *)(AD1+32+ps*i)=0x04;
		
		*(U8 *)(AD1+33+ps*i)=k5;
		*(U8 *)(AD1+34+ps*i)=k4;
		
		if (ptcom->registerr == 'R')
		{
			for (j=0;j<length*2;j=j+4)
			{
				*(U8 *)(AD1+35+j+ps*i)=*(U8 *)(PE+9+i*64+j+3);			//高低字节交换
				*(U8 *)(AD1+36+j+ps*i)=*(U8 *)(PE+9+i*64+j+2);			//高低字节交换
				*(U8 *)(AD1+37+j+ps*i)=*(U8 *)(PE+9+i*64+j+1);			//高低字节交换
				*(U8 *)(AD1+38+j+ps*i)=*(U8 *)(PE+9+i*64+j+0);			//高低字节交换
			}	
		}
		else
		{
			for (j=0;j<length*2;j++)
			{
				if (j%2==0)
				{
					*(U8 *)(AD1+35+j+ps*i)=*(U8 *)(PE+9+i*64+j+1);			//高低字节交换
				}
				else
				{
					*(U8 *)(AD1+35+j+ps*i)=*(U8 *)(PE+9+i*64+j-1);	
				}	
			}
		}

		aakj=CalcHe((U8 *)AD1+ps*i,35+length*2);
		a2=aakj&0xff;
		*(U8 *)(AD1+35+length*2+ps*i)=a2;				//和校检
		*(U8 *)(AD1+36+length*2+ps*i)=0x16;
		
		ptcom->send_length[i*2]=35+length*2+2;		//发送长度
		ptcom->send_staradd[i*2]=i*ps;			//发送数据存储地址	
		ptcom->return_length[i*2]=1;				//返回数据长度
		ptcom->return_start[i*2]=0;				//返回数据有效开始
		ptcom->return_length_available[i*2]=0;	//返回有效数据长度			
		
		*(U8 *)(AD1+37+length*2+ps*i)=0x10;
		*(U8 *)(AD1+38+length*2+ps*i)=stationAdd;
		*(U8 *)(AD1+39+length*2+ps*i)=0x00;
		*(U8 *)(AD1+40+length*2+ps*i)=0x5c;
		aakj=Second_Send_CalcHe((U8 *)(AD1+38+length*2+ps*i),3);
		a2=aakj&0xff;
		*(U8 *)(AD1+41+length*2+ps*i)=a2;				//和校检
		*(U8 *)(AD1+42+length*2+ps*i)=0x16;
		
		ptcom->send_length[i*2+1]=6;		//发送长度
		ptcom->send_staradd[i*2+1]=i*ps+35+length*2+2;			//发送数据存储地址	
		ptcom->return_length[i*2+1]=24;				//返回数据长度
		ptcom->return_start[i*2+1]=0;				//返回数据有效开始
		ptcom->return_length_available[i*2+1]=0;	//返回有效数据长度			
		
	}
	ptcom->send_times=SendTimes*2;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数		
}


void compxy(void)				//处理成标准存储格式
{
	int i;
	unsigned char a1,a2,a3,a4;
	int sendlength;
	int k;
	int result=0;
	int j=0;
	int p;
	
	if (ptcom->registerr=='T')
	{			
		sendlength=ptcom->register_length*8;
		for (i=0;i<sendlength;i++)
		{
			if (*(U8 *)(COMad+25+i*5)==0x02)	//返回ON
			{
				k=1;
			}
			else
			{
				k=0;
			}
			result=result+(k<<(i%8));
			if ((i+1)%8==0)
			{
				*(U8 *)(COMad+j)=result;
				result=0;
				j++;
			}			
		}
	}
	else if(ptcom->registerr=='C')
	{
		sendlength=ptcom->register_length*8;
		for (i=0;i<sendlength;i++)
		{
			if (*(U8 *)(COMad+25+i*3)==0x10)	//返回ON
			{
				k=1;
			}
			else
			{
				k=0;
			}
			result=result+(k<<(i%8));
			if ((i+1)%8==0)
			{
				*(U8 *)(COMad+j)=result;
				result=0;
				j++;
			}			
		}
	}
	else if (ptcom->registerr=='t' || ptcom->registerr=='c')
	{
		sendlength=ptcom->register_length;
		if(ptcom->registerr=='t')
		{
			for (i=0;i<sendlength;i++)
			{
				*(U8 *)(COMad+j)=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*5+3);
				*(U8 *)(COMad+j+1)=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*5+4);			
				j=j+2;		
			}
		}
		else
		{
			for (i=0;i<sendlength;i++)
			{
				*(U8 *)(COMad+j)=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*3+1);
				*(U8 *)(COMad+j+1)=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*3+2);			
				j=j+2;		
			}
		}
	}
	else if (ptcom->registerr=='R')
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/4;i++)		//16进制返回
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+0);	
			*(U8 *)(COMad+i*4)=a1;							//重新存,从第0个开始存,VB100,VB101.....M0，M1...
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+1);	
			*(U8 *)(COMad+i*4+1)=a2;
			a3=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+2);	
			*(U8 *)(COMad+i*4+2)=a3;
			a4=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+3);	
			*(U8 *)(COMad+i*4+3)=a4;
		}
	}
	else
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/2;i++)		//16进制返回
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2+0);	
			*(U8 *)(COMad+i*2)=a1;							//重新存,从第0个开始存,VB100,VB101.....M0，M1...
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2+1);	
			*(U8 *)(COMad+i*2+1)=a2;
			//sysprintf("*(U8 *)(COMad+%d)=%d\n",i*2,*(U8 *)(COMad+i*2));
			//sysprintf("*(U8 *)(COMad+%d)=%d\n",i*2+1,*(U8 *)(COMad+i*2+1));
		}
		
	}
}


void watchcom(void)		//检查数据校检
{
	unsigned int aakj=0;
	aakj=remark();
	if(aakj==1)			//校检玛正确
	{
		ptcom->IfResultCorrect=1;
		compxy();		//进入数据处理程序
	}
	else
	{
		ptcom->IfResultCorrect=0;
	}
}





int asicc(int a)			//转为Asc码
{
	int bl;
	if(a<10)
		bl=a+0x30;
	if(a>9)
		bl=a-10+0x41;
	return bl;	
}

int bsicc(int a)			//Asc转为数字
{
	int bl=0;
	if(a>=0x30)
	{
		if(a<0x40)
			bl=a-0x30;
		if(a>0x40)
			bl=a-0x41+10;
	}
	return bl;
}

int remark()				//返回来的数据计算校检码是否正确
{
	unsigned int aakj2;
	unsigned int akj1;
	unsigned int akj2,i;
//	for(i=0;i<=ptcom->return_length[ptcom->Current_Times-1];i++)
//		sysprintf("*(U8 *)(AD1+%d)=0x%x\n",i,*(U8 *)(COMad+i));

	if (*(U8 *)(COMad+0)==0xe5)
	{
		return 0;
	}
	aakj2=*(U8 *)(COMad+ptcom->return_length[ptcom->Current_Times-1]-2)&0xff;
	akj1=CalcHe((U8 *)COMad,ptcom->return_length[ptcom->Current_Times-1]-2);
	akj2=akj1&0xff;
//	sysprintf("aakj2=0x%x akj2 0x%x,ptcom->Current_Times %d,ptcom->return_length[1] %d\n",aakj2,akj2,ptcom->Current_Times,ptcom->return_length[1]);
	if(akj2==aakj2)
		return 1;
	else
		return 0;
}


U16 CalcHe(unsigned char *chData,U16 uNo)		//计算和校检
{
	int i;
	int ab=0;
	for(i=4;i<uNo;i++)
	{
		ab=ab+chData[i];
	}
	return (ab);
}

U16 Second_Send_CalcHe(unsigned char *chData,U16 uNo)		//计算和校检
{
	int i;
	int ab=0;
	for(i=0;i<uNo;i++)
	{
		ab=ab+chData[i];
	}
	return (ab);
}

