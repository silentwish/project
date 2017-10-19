#include "stdio.h"
#include "def.h"
#include "smm.h"
	
struct Com_struct_D *ptcom;

/*************************************************************************************************************************

*************************************************************************************************************************/
void Enter_PlcDriver(void)
{
	ptcom=(struct Com_struct_D *)adcom;
	
	switch (ptcom->R_W_Flag)
	{
		case PLC_READ_DATA:															//进入驱动是读数据
		case PLC_READ_DIGITALALARM:														//进入驱动是读数据,报警	
		case PLC_READ_TREND:														//进入驱动是读数据,趋势图
		case PLC_READ_ANALOGALARM:														//进入驱动是读数据,类比报警	
		case PLC_READ_CONTROL:														//进入驱动是读PLC控制的数据	
			switch(ptcom->registerr)
			{
				case 'X':												//%MX1.
				case 'Y':												//%MX3.
				case 'M':												//%MX10.
					Read_Bool();
					break;
				case 'D':												//%MW1.
				case 'R':												//%MW3.
				case 'N':												//%MW10.	
					Read_Analog();
					break;
				default:
					break;			
			}
			break;
		case PLC_WRITE_DATA:															//进入驱动是写数据
			switch(ptcom->registerr)
			{
				case 'X':												//%MX1.
				case 'Y':												//%MX3.
				case 'M':												//%MX10.
					setreset();
					break;
				case 'D':												//%MW1.
				case 'R':												//%MW3.
				case 'N':												//%MW10.	
					Write_Analog();		
					break;
				default:
					break;				
			}
			break;	
		case PLC_WRITE_TIME:															//进入驱动是写时间到PLC
			switch(ptcom->registerr)
			{
				case 'D':												//%MW1.
				case 'R':												//%MW3.
				case 'N':												//%MW10.				
					Write_Time();		
					break;	
				default:
					break;			
			}
			break;	
		case PLC_READ_TIME:															//进入驱动是读取时间到PLC
			switch(ptcom->registerr)
			{
				case 'D':												//%MW1.
				case 'R':												//%MW3.
				case 'N':												//%MW10.	
					Read_Time();		
					break;
				default:
					break;				
			}
			break;
		case PLC_WRITE_RECIPE:															//进入驱动是写配方到PLC
			switch(*(U8 *)(PE+3))										//配方寄存器名称
			{
				case 'D':												//%MW1.
				case 'R':												//%MW3.
				case 'N':												//%MW10.	
					Write_Recipe();		
					break;
				default:
					break;				
			}
			break;
		case PLC_READ_RECIPE:															//进入驱动是从PLC读取配方
			switch(*(U8 *)(PE+3))										//配方寄存器名称
			{
				case 'D':												//%MW1.
				case 'R':												//%MW3.
				case 'N':												//%MW10.	
					Read_Recipe();		
					break;
				default:
					break;							
			}
			break;							
		case PLC_CHECK_DATA:															//进入驱动是数据处理
			watchcom();
			break;
		default:
			break;					
	}		 
}

/*************************************************************************************************************************
//读数字量
*************************************************************************************************************************/
void Read_Bool()
{
	U16 usBCC;
	int nAddr = 0;
	int nPlcSta = 0;
	int nRLen = 0;
	int nSendAddr = 0;
	
	nAddr	= ptcom->address;												//开始地址
	nPlcSta	= ptcom->plc_address;											//PLC站地址
	nRLen	= ptcom->register_length;										//本次读取长度
	
	//Transmission header
	*(U8 *)(AD1 + 0) = 0x5a;												//START code
	
	*(U8 *)(AD1 + 1) = 0x17;												//Data counter L H
	*(U8 *)(AD1 + 2) = 0x00;
	
	//Loader command
	*(U8 *)(AD1 + 3) = 0xff;												//Processing status
	*(U8 *)(AD1 + 4) = 0x7a;												//Connection mode

	*(U8 *)(AD1 + 5) = (nPlcSta>>0) & 0xff;									//Connection ID L H
	*(U8 *)(AD1 + 6) = (nPlcSta>>8) & 0xff;	
	
	*(U8 *)(AD1 + 7) = 0x11;												//Fixed
	*(U8 *)(AD1 + 8) = 0x00;
	*(U8 *)(AD1 + 9) = 0x00;
	*(U8 *)(AD1 + 10) = 0x00;
	*(U8 *)(AD1 + 11) = 0x00;
	*(U8 *)(AD1 + 12) = 0x00;
	
	*(U8 *)(AD1 + 13) = 0x00;												//Read command
	*(U8 *)(AD1 + 14) = 0x00;												//Read mode
	
	*(U8 *)(AD1 + 15) = 0x00;												//Fixed
	*(U8 *)(AD1 + 16) = 0x01;
	
	*(U8 *)(AD1 + 17) = 0x06;												//Number of bytes in data L H	
	*(U8 *)(AD1 + 18) = 0x00;
	
	//Data
	switch(ptcom->registerr)												//Memory type
	{
		case 'X':															//%MW1.
			*(U8 *)(AD1 + 19) = 0x02;
			break;
		case 'Y':															//%MW3.
			*(U8 *)(AD1 + 19) = 0x04;
			break;
		case 'M':															//%MW10.				
			*(U8 *)(AD1 + 19) = 0x08;
			break;	
		default:
			break;			
	}	
	
	nSendAddr = nAddr/16;
	ptcom->address = nSendAddr*16;
	
	if (nRLen%2 == 0)
	{
		nRLen = nRLen/2;
	}
	else
	{
		nRLen = nRLen/2 + 1;
	}
	
	*(U8 *)(AD1 + 20) = (nSendAddr>>0) & 0xff;								//Memory address L M H
	*(U8 *)(AD1 + 21) = (nSendAddr>>8) & 0xff;	
	*(U8 *)(AD1 + 22) = (nSendAddr>>16) & 0xff;	
	
	*(U8 *)(AD1 + 23) = (nRLen>>0) & 0xff;									//Number of words of read data L H
	*(U8 *)(AD1 + 24) = (nRLen>>8) & 0xff;	
	
	usBCC = CalcBCC((U8 *)(AD1 + 1), 24); 
	*(U8 *)(AD1 + 25) = usBCC & 0xff;										//Check BCC
	
	ptcom->send_length[0] = 26;												//发送长度
	ptcom->send_staradd[0] = 0;												//发送数据存储地址	
		
	ptcom->return_length[0] = 26 + nRLen*2;									//返回数据长度
	ptcom->return_start[0] = 25;											//返回数据有效开始
	ptcom->return_length_available[0] = nRLen*2;							//返回有效数据长度
	
	ptcom->send_times = 1;													//发送次数
	ptcom->Current_Times = 0;												//当前发送次数
	
	ptcom->send_staradd[99] = 2;	
}

/*************************************************************************************************************************
//写数字量
*************************************************************************************************************************/
void setreset()
{
	U16 usBCC;
	int nAddr = 0;
	int nPlcSta = 0;
	int nSendAddr = 0;
	
	nAddr	= ptcom->address;												//开始地址
	nPlcSta	= ptcom->plc_address;											//PLC站地址

//先读----------------------------------------------------------------------------------------------------------------------	
	//Transmission header
	*(U8 *)(AD1 + 0) = 0x5a;												//START code
	
	*(U8 *)(AD1 + 1) = 0x17;												//Data counter L H
	*(U8 *)(AD1 + 2) = 0x00;
	
	//Loader command
	*(U8 *)(AD1 + 3) = 0xff;												//Processing status
	*(U8 *)(AD1 + 4) = 0x7a;												//Connection mode

	*(U8 *)(AD1 + 5) = (nPlcSta>>0) & 0xff;									//Connection ID L H
	*(U8 *)(AD1 + 6) = (nPlcSta>>8) & 0xff;	
	
	*(U8 *)(AD1 + 7) = 0x11;												//Fixed
	*(U8 *)(AD1 + 8) = 0x00;
	*(U8 *)(AD1 + 9) = 0x00;
	*(U8 *)(AD1 + 10) = 0x00;
	*(U8 *)(AD1 + 11) = 0x00;
	*(U8 *)(AD1 + 12) = 0x00;
	
	*(U8 *)(AD1 + 13) = 0x00;												//Read command
	*(U8 *)(AD1 + 14) = 0x00;												//Read mode
	
	*(U8 *)(AD1 + 15) = 0x00;												//Fixed
	*(U8 *)(AD1 + 16) = 0x01;
	
	*(U8 *)(AD1 + 17) = 0x06;												//Number of bytes in data L H	
	*(U8 *)(AD1 + 18) = 0x00;
	
	//Data
	switch(ptcom->registerr)												//Memory type
	{
		case 'X':															//%MW1.
			*(U8 *)(AD1 + 19) = 0x02;
			break;
		case 'Y':															//%MW3.
			*(U8 *)(AD1 + 19) = 0x04;
			break;
		case 'M':															//%MW10.				
			*(U8 *)(AD1 + 19) = 0x08;
			break;	
		default:
			break;			
	}	
	
	nSendAddr = nAddr/16;
	//ptcom->address = nSendAddr*16;
	
	*(U8 *)(AD1 + 20) = (nSendAddr>>0) & 0xff;								//Memory address L M H
	*(U8 *)(AD1 + 21) = (nSendAddr>>8) & 0xff;	
	*(U8 *)(AD1 + 22) = (nSendAddr>>16) & 0xff;	
	
	*(U8 *)(AD1 + 23) = 0x01;												//Number of words of read data L H
	*(U8 *)(AD1 + 24) = 0x00;	
	
	usBCC = CalcBCC((U8 *)(AD1 + 1), 24); 
	*(U8 *)(AD1 + 25) = usBCC & 0xff;										//Check BCC
	
	ptcom->send_length[0] = 26;												//发送长度
	ptcom->send_staradd[0] = 0;												//发送数据存储地址	
		
	ptcom->return_length[0] = 28;											//返回数据长度
	ptcom->return_start[0] = 25;											//返回数据有效开始
	ptcom->return_length_available[0] = 2;									//返回有效数据长度	
	
	ptcom->send_add[0] = nSendAddr*16;										//读的是这个地址的数据	

//后写-------------------------------------------------------------------------------------------------------------------------	
	//Transmission header
	*(U8 *)(AD1 + 26) = 0x5a;												//START code
	
	*(U8 *)(AD1 + 27) = 0x19;												//Data counter L H
	*(U8 *)(AD1 + 28) = 0x00;
	
	//Loader command
	*(U8 *)(AD1 + 29) = 0xff;												//Processing status
	*(U8 *)(AD1 + 30) = 0x7a;												//Connection mode

	*(U8 *)(AD1 + 31) = (nPlcSta>>0) & 0xff;								//Connection ID L H
	*(U8 *)(AD1 + 32) = (nPlcSta>>8) & 0xff;	
	
	*(U8 *)(AD1 + 33) = 0x11;												//Fixed
	*(U8 *)(AD1 + 34) = 0x00;
	*(U8 *)(AD1 + 35) = 0x00;
	*(U8 *)(AD1 + 36) = 0x00;
	*(U8 *)(AD1 + 37) = 0x00;
	*(U8 *)(AD1 + 38) = 0x00;
	
	*(U8 *)(AD1 + 39) = 0x01;												//Write command
	*(U8 *)(AD1 + 40) = 0x00;												//Write mode
	
	*(U8 *)(AD1 + 41) = 0x00;												//Fixed
	*(U8 *)(AD1 + 42) = 0x01;
	
	*(U8 *)(AD1 + 43) = 0x08;												//Number of bytes in data L H	
	*(U8 *)(AD1 + 44) = 0x00;
	
	//Data
	switch(ptcom->registerr)												//Memory type
	{
		case 'X':															//%MW1.
			*(U8 *)(AD1 + 45) = 0x02;
			break;
		case 'Y':															//%MW3.
			*(U8 *)(AD1 + 45) = 0x04;
			break;
		case 'M':															//%MW10.				
			*(U8 *)(AD1 + 45) = 0x08;
			break;	
		default:
			break;			
	}	

	*(U8 *)(AD1 + 46) = *(U8 *)(AD1 + 20);									//Memory address L M H
	*(U8 *)(AD1 + 47) = *(U8 *)(AD1 + 21);	
	*(U8 *)(AD1 + 48) = *(U8 *)(AD1 + 22);	
	
	//*(U8 *)(AD1 + 46) = 0x70;									//Memory address L M H
	//*(U8 *)(AD1 + 47) = 0x17;	
	//*(U8 *)(AD1 + 48) = 0x00;	
	
	*(U8 *)(AD1 + 49) = 0x01;												//Number of words of read data L H
	*(U8 *)(AD1 + 50) = 0x00;	
	
	*(U8 *)(AD1 + 51) = 0x00;												//Write data
	*(U8 *)(AD1 + 52) = 0x00;
	
	usBCC = CalcBCC((U8 *)(AD1 + 27), 26); 
	*(U8 *)(AD1 + 53) = usBCC & 0xff;										//Check BCC
	
	ptcom->send_length[1] = 28;												//发送长度
	ptcom->send_staradd[1] = 26;											//发送数据存储地址	
		
	ptcom->return_length[1] = 26;											//返回数据长度
	ptcom->return_start[1] = 0;												//返回数据有效开始
	ptcom->return_length_available[1] = 0;									//返回有效数据长度
	
	ptcom->send_times = 2;													//发送次数
	ptcom->Current_Times = 0;												//当前发送次数
	
	ptcom->send_staradd[99] = 1;
	
	if (ptcom->writeValue == 1)												//置位
	{
		ptcom->send_staradd[98] = 1;
	}	
	if (ptcom->writeValue == 0)												//复位
	{
		ptcom->send_staradd[98] = 2;
	}	
}

/*************************************************************************************************************************
//读模拟量
*************************************************************************************************************************/
void Read_Analog()				
{
	U16 usBCC;
	int nAddr = 0;
	int nPlcSta = 0;
	int nRLen = 0;
	
	nAddr	= ptcom->address;												//开始地址
	nPlcSta	= ptcom->plc_address;											//PLC站地址
	nRLen	= ptcom->register_length;										//本次读取长度
	
	//Transmission header
	*(U8 *)(AD1 + 0) = 0x5a;												//START code
	
	*(U8 *)(AD1 + 1) = 0x17;												//Data counter L H
	*(U8 *)(AD1 + 2) = 0x00;
	
	//Loader command
	*(U8 *)(AD1 + 3) = 0xff;												//Processing status
	*(U8 *)(AD1 + 4) = 0x7a;												//Connection mode

	*(U8 *)(AD1 + 5) = (nPlcSta>>0) & 0xff;									//Connection ID L H
	*(U8 *)(AD1 + 6) = (nPlcSta>>8) & 0xff;	
	
	*(U8 *)(AD1 + 7) = 0x11;												//Fixed
	*(U8 *)(AD1 + 8) = 0x00;
	*(U8 *)(AD1 + 9) = 0x00;
	*(U8 *)(AD1 + 10) = 0x00;
	*(U8 *)(AD1 + 11) = 0x00;
	*(U8 *)(AD1 + 12) = 0x00;
	
	*(U8 *)(AD1 + 13) = 0x00;												//Read command
	*(U8 *)(AD1 + 14) = 0x00;												//Read mode
	
	*(U8 *)(AD1 + 15) = 0x00;												//Fixed
	*(U8 *)(AD1 + 16) = 0x01;
	
	*(U8 *)(AD1 + 17) = 0x06;												//Number of bytes in data L H	
	*(U8 *)(AD1 + 18) = 0x00;
	
	//Data
	switch(ptcom->registerr)												//Memory type
	{
		case 'D':															//%MW1.
			*(U8 *)(AD1 + 19) = 0x02;
			break;
		case 'R':															//%MW3.
			*(U8 *)(AD1 + 19) = 0x04;
			break;
		case 'N':															//%MW10.				
			*(U8 *)(AD1 + 19) = 0x08;
			break;	
		default:
			break;			
	}	
	
	*(U8 *)(AD1 + 20) = (nAddr>>0) & 0xff;									//Memory address L M H
	*(U8 *)(AD1 + 21) = (nAddr>>8) & 0xff;	
	*(U8 *)(AD1 + 22) = (nAddr>>16) & 0xff;	
	
	*(U8 *)(AD1 + 23) = (nRLen>>0) & 0xff;									//Number of words of read data L H
	*(U8 *)(AD1 + 24) = (nRLen>>8) & 0xff;	
	
	usBCC = CalcBCC((U8 *)(AD1 + 1), 24); 
	*(U8 *)(AD1 + 25) = usBCC & 0xff;										//Check BCC
	
	ptcom->send_length[0] = 26;												//发送长度
	ptcom->send_staradd[0] = 0;												//发送数据存储地址	
		
	ptcom->return_length[0] = 26 + nRLen*2;									//返回数据长度
	ptcom->return_start[0] = 25;											//返回数据有效开始
	ptcom->return_length_available[0] = nRLen*2;							//返回有效数据长度
	
	ptcom->send_times = 1;													//发送次数
	ptcom->Current_Times = 0;												//当前发送次数
	
	ptcom->send_staradd[99] = 0;
}
/*************************************************************************************************************************
//写模拟量
*************************************************************************************************************************/
void Write_Analog()				
{
	U16 usBCC;
	int nAddr = 0;
	int nPlcSta = 0;
	int nWLen = 0;
	int i = 0;
	
	nAddr	= ptcom->address;												//开始地址
	nPlcSta	= ptcom->plc_address;											//PLC站地址
	nWLen	= ptcom->register_length;										//本次读取长度
	
	//Transmission header
	*(U8 *)(AD1 + 0) = 0x5a;												//START code
	
	*(U8 *)(AD1 + 1) = ((23 + nWLen*2) >> 0) & 0xff;						//Data counter L H
	*(U8 *)(AD1 + 2) = ((23 + nWLen*2) >> 8) & 0xff;
	
	//Loader command
	*(U8 *)(AD1 + 3) = 0xff;												//Processing status
	*(U8 *)(AD1 + 4) = 0x7a;												//Connection mode

	*(U8 *)(AD1 + 5) = (nPlcSta>>0) & 0xff;									//Connection ID L H
	*(U8 *)(AD1 + 6) = (nPlcSta>>8) & 0xff;	
	
	*(U8 *)(AD1 + 7) = 0x11;												//Fixed
	*(U8 *)(AD1 + 8) = 0x00;
	*(U8 *)(AD1 + 9) = 0x00;
	*(U8 *)(AD1 + 10) = 0x00;
	*(U8 *)(AD1 + 11) = 0x00;
	*(U8 *)(AD1 + 12) = 0x00;
	
	*(U8 *)(AD1 + 13) = 0x01;												//Write command
	*(U8 *)(AD1 + 14) = 0x00;												//Write mode
	
	*(U8 *)(AD1 + 15) = 0x00;												//Fixed
	*(U8 *)(AD1 + 16) = 0x01;
	
	*(U8 *)(AD1 + 17) = 0x06 + nWLen*2;										//Number of bytes in data L H	
	*(U8 *)(AD1 + 18) = 0x00;
	
	//Data
	switch(ptcom->registerr)												//Memory type
	{
		case 'D':															//%MW1.
			*(U8 *)(AD1 + 19) = 0x02;
			break;
		case 'R':															//%MW3.
			*(U8 *)(AD1 + 19) = 0x04;
			break;
		case 'N':															//%MW10.				
			*(U8 *)(AD1 + 19) = 0x08;
			break;	
		default:
			break;			
	}	
	
	*(U8 *)(AD1 + 20) = (nAddr>>0) & 0xff;									//Memory address L M H
	*(U8 *)(AD1 + 21) = (nAddr>>8) & 0xff;	
	*(U8 *)(AD1 + 22) = (nAddr>>16) & 0xff;	
	
	*(U8 *)(AD1 + 23) = (nWLen>>0) & 0xff;									//Number of words of read data L H
	*(U8 *)(AD1 + 24) = (nWLen>>8) & 0xff;	
	
	for (i=0; i<nWLen; i++)													//Write data
	{
		*(U8 *)(AD1 + 25 + i*2) = ptcom->U8_Data[i*2];
		*(U8 *)(AD1 + 26 + i*2) = ptcom->U8_Data[i*2 + 1];
	}
	
	usBCC = CalcBCC((U8 *)(AD1 + 1), 24 + nWLen*2); 
	*(U8 *)(AD1 + 25 + nWLen*2) = usBCC & 0xff;								//Check BCC
	
	ptcom->send_length[0] = 26 + nWLen*2;									//发送长度
	ptcom->send_staradd[0] = 0;												//发送数据存储地址	
		
	ptcom->return_length[0] = 26;											//返回数据长度
	ptcom->return_start[0] = 0;												//返回数据有效开始
	ptcom->return_length_available[0] = 0;									//返回有效数据长度
	
	ptcom->send_times = 1;													//发送次数
	ptcom->Current_Times = 0;												//当前发送次数
	
	ptcom->send_staradd[99] = 0;
}
/*************************************************************************************************************************
//读取配方
*************************************************************************************************************************/
void Read_Recipe()								
{
	U16 usBCC;
	int nAddr = 0;
	int nPlcSta = 0;
	int nRLen = 0;
	int i = 0;
	int Datalength = 0;
	int SendTimes = 0;
	int LastTimeWord = 0;
	int ps = 0;
	
	nAddr	= ptcom->address;												//开始地址
	nPlcSta	= ptcom->plc_address;											//PLC站地址
	nRLen	= ptcom->register_length;										//本次读取长度

	if(Datalength > 5000)													//限制配方长度
	{
		Datalength = 5000;
	}                								
		
	if(Datalength%64 == 0)													//每次最多能发送64个D
	{
		SendTimes = Datalength/64;      									//发送的次数
		LastTimeWord = 64;                									//固定长度64	
	}
	if(Datalength%64 != 0)
	{
		SendTimes = Datalength/64 + 1;      								//发送的次数
		LastTimeWord = Datalength%64;     									//最后一次发送的长度	
	}	

	for (i=0; i<SendTimes; i++)
	{
		//Transmission header
		*(U8 *)(AD1 + 0 + ps) = 0x5a;										//START code
		
		*(U8 *)(AD1 + 1 + ps) = 0x17;										//Data counter L H
		*(U8 *)(AD1 + 2 + ps) = 0x00;
		
		//Loader command
		*(U8 *)(AD1 + 3 + ps) = 0xff;										//Processing status
		*(U8 *)(AD1 + 4 + ps) = 0x7a;										//Connection mode

		*(U8 *)(AD1 + 5 + ps) = (nPlcSta>>0) & 0xff;						//Connection ID L H
		*(U8 *)(AD1 + 6 + ps) = (nPlcSta>>8) & 0xff;	
		
		*(U8 *)(AD1 + 7 + ps) = 0x11;										//Fixed
		*(U8 *)(AD1 + 8 + ps) = 0x00;
		*(U8 *)(AD1 + 9 + ps) = 0x00;
		*(U8 *)(AD1 + 10 + ps) = 0x00;
		*(U8 *)(AD1 + 11 + ps) = 0x00;
		*(U8 *)(AD1 + 12 + ps) = 0x00;
		
		*(U8 *)(AD1 + 13 + ps) = 0x00;										//Read command
		*(U8 *)(AD1 + 14 + ps) = 0x00;										//Read mode
		
		*(U8 *)(AD1 + 15 + ps) = 0x00;										//Fixed
		*(U8 *)(AD1 + 16 + ps) = 0x01;
		
		*(U8 *)(AD1 + 17 + ps) = 0x06;										//Number of bytes in data L H	
		*(U8 *)(AD1 + 18 + ps) = 0x00;
		
		//Data
		switch(ptcom->registerr)											//Memory type
		{
			case 'D':														//%MW1.
				*(U8 *)(AD1 + 19 + ps) = 0x02;
				break;
			case 'R':														//%MW3.
				*(U8 *)(AD1 + 19 + ps) = 0x04;
				break;
			case 'N':														//%MW10.				
				*(U8 *)(AD1 + 19 + ps) = 0x08;
				break;	
			default:
				break;			
		}	
		
		*(U8 *)(AD1 + 20 + ps) = ((nAddr + 64*i)>>0) & 0xff;				//Memory address L M H
		*(U8 *)(AD1 + 21 + ps) = ((nAddr + 64*i)>>8) & 0xff;	
		*(U8 *)(AD1 + 22 + ps) = ((nAddr + 64*i)>>16) & 0xff;	
		
		if (i != (SendTimes - 1))
		{
			*(U8 *)(AD1 + 23 + ps) = 0x40;									//Number of words of read data L H
			*(U8 *)(AD1 + 24 + ps) = 0x00;	      
		}
		if (i == (SendTimes - 1))	
		{
			*(U8 *)(AD1 + 23 + ps) = (LastTimeWord >> 0) & 0xff;			//Number of words of read data L H
			*(U8 *)(AD1 + 24 + ps) = (LastTimeWord >> 8) & 0xff;   
		}		
		
		usBCC = CalcBCC((U8 *)(AD1 + 1 + ps), 24); 
		*(U8 *)(AD1 + 25 + ps) = usBCC & 0xff;								//Check BCC
		
		ptcom->send_length[i] = 26;											//发送长度
		ptcom->send_staradd[i] = ps;										//发送数据存储地址	
			
		ptcom->return_length[i] = 26 + nRLen*2;								//返回数据长度
		ptcom->return_start[i] = 25;										//返回数据有效开始
		ptcom->return_length_available[i] = nRLen*2;						//返回有效数据长度
		
		ps = 26;
		
		ptcom->send_staradd[99] = 0;
	}
	ptcom->send_times = SendTimes;											//发送次数
	ptcom->Current_Times = 0;												//当前发送次数	
}
/*************************************************************************************************************************
//写配方到PLC
*************************************************************************************************************************/
void Write_Recipe()								
{
	U16 usBCC;
	int nAddr = 0;
	int nPlcSta = 0;
	int nWLen = 0;
	int i = 0;
	int j = 0;
	int Datalength = 0;
	int SendTimes = 0;
	int LastTimeWord = 0;
	int ps = 0;
	
	nAddr	= ((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//开始地址
	nPlcSta	= *(U8 *)(PE+4);												//PLC站地址
	Datalength = ((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));						//本次长度	             								
		
	if(Datalength%64 == 0)													//每次最多能发送64个D
	{
		SendTimes = Datalength/64;      									//发送的次数
		LastTimeWord = 64;                									//固定长度64	
	}
	if(Datalength%64 != 0)
	{
		SendTimes = Datalength/64 + 1;      								//发送的次数
		LastTimeWord = Datalength%64;     									//最后一次发送的长度	
	}		

	ps = 192;

	for (i=0; i<SendTimes; i++)
	{
		if (i != (SendTimes - 1))
		{
			nWLen = 64;    
		}
		if (i == (SendTimes - 1))	
		{
			nWLen = LastTimeWord;
		}
		//Transmission header
		*(U8 *)(AD1 + 0 + ps*i) = 0x5a;										//START code
		
		*(U8 *)(AD1 + 1 + ps*i) = ((23 + nWLen*2) >> 0) & 0xff;				//Data counter L H
		*(U8 *)(AD1 + 2 + ps*i) = ((23 + nWLen*2) >> 8) & 0xff;
		
		//Loader command
		*(U8 *)(AD1 + 3 + ps*i) = 0xff;										//Processing status
		*(U8 *)(AD1 + 4 + ps*i) = 0x7a;										//Connection mode

		*(U8 *)(AD1 + 5 + ps*i) = (nPlcSta>>0) & 0xff;						//Connection ID L H
		*(U8 *)(AD1 + 6 + ps*i) = (nPlcSta>>8) & 0xff;	
		
		*(U8 *)(AD1 + 7 + ps*i) = 0x11;										//Fixed
		*(U8 *)(AD1 + 8 + ps*i) = 0x00;
		*(U8 *)(AD1 + 9 + ps*i) = 0x00;
		*(U8 *)(AD1 + 10 + ps*i) = 0x00;
		*(U8 *)(AD1 + 11 + ps*i) = 0x00;
		*(U8 *)(AD1 + 12 + ps) = 0x00;
		
		*(U8 *)(AD1 + 13 + ps*i) = 0x01;									//Write command
		*(U8 *)(AD1 + 14 + ps*i) = 0x00;									//Write mode
		
		*(U8 *)(AD1 + 15 + ps*i) = 0x00;									//Fixed
		*(U8 *)(AD1 + 16 + ps*i) = 0x01;
		
		*(U8 *)(AD1 + 17 + ps*i) = 0x06 + nWLen*2;							//Number of bytes in data L H	
		*(U8 *)(AD1 + 18 + ps*i) = 0x00;
			
		//Data
		switch(ptcom->registerr)											//Memory type
		{
			case 'D':														//%MW1.
				*(U8 *)(AD1 + 19 + ps*i) = 0x02;
				break;
			case 'R':														//%MW3.
				*(U8 *)(AD1 + 19 + ps*i) = 0x04;
				break;
			case 'N':														//%MW10.				
				*(U8 *)(AD1 + 19 + ps*i) = 0x08;
				break;	
			default:
				break;			
		}	
		
		*(U8 *)(AD1 + 20 + ps*i) = ((nAddr + 64*i)>>0) & 0xff;				//Memory address L M H
		*(U8 *)(AD1 + 21 + ps*i) = ((nAddr + 64*i)>>8) & 0xff;	
		*(U8 *)(AD1 + 22 + ps*i) = ((nAddr + 64*i)>>16) & 0xff;		
	
		*(U8 *)(AD1 + 23 + ps*i) = (nWLen>>0) & 0xff;						//Number of words of read data L H
		*(U8 *)(AD1 + 24 + ps*i) = (nWLen>>8) & 0xff;	   
		
		for (j=0; j<nWLen; j++)												//Write data
		{
			*(U8 *)(AD1 + 25 + ps*i + j*2) = *(U8 *)(PE + 9 + i*128 + j*2);
			*(U8 *)(AD1 + 26 + ps*i + j*2) = *(U8 *)(PE + 9 + i*128 + j*2 + 1);
		}
		
		usBCC = CalcBCC((U8 *)(AD1 + 1 + ps*i), 24 + nWLen*2); 
		*(U8 *)(AD1 + 25 + nWLen*2 + ps*i) = usBCC & 0xff;					//Check BCC
		
		ptcom->send_length[i] = 26 + nWLen*2;								//发送长度
		ptcom->send_staradd[i] = ps*i;										//发送数据存储地址	
			
		ptcom->return_length[i] = 26;										//返回数据长度
		ptcom->return_start[i] = 0;											//返回数据有效开始
		ptcom->return_length_available[i] = 0;								//返回有效数据长度	
		
		ptcom->send_staradd[99] = 0;
	}
	ptcom->send_times = SendTimes;											//发送次数	
	ptcom->Current_Times=0;													//当前发送次数
}
/*************************************************************************************************************************
//写时间到PLC
*************************************************************************************************************************/
void Write_Time()
{
	Write_Analog();							
}
/*************************************************************************************************************************
//从PLC读取时间
*************************************************************************************************************************/
void Read_Time()									
{
	Read_Analog();
}
/*************************************************************************************************************************
//处理成标准存储格式
*************************************************************************************************************************/
void compxy(void)				
{
	int i;
	int nValue_L = 0;
	int nValue_H = 0;
	int nValue = 0;
	int nOffset = 0;
	int temp = 0;
	U16 usBCC = 0;
	
	if (ptcom->send_staradd[99] == 0)
	{
		for(i=0; i<ptcom->return_length_available[ptcom->Current_Times - 1] / 2; i++)
		{
			nValue_L = *(U8 *)(COMad + ptcom->return_start[ptcom->Current_Times - 1] + i*2);
			nValue_H = *(U8 *)(COMad + ptcom->return_start[ptcom->Current_Times - 1] + i*2 + 1);
			
			*(U8 *)(COMad + i*2) = nValue_H;
			*(U8 *)(COMad + i*2 + 1) = nValue_L;
		}
	}
	else if (ptcom->send_staradd[99] == 2)
	{
		for(i=0; i<ptcom->return_length_available[ptcom->Current_Times - 1] / 2; i++)
		{
			nValue_L = *(U8 *)(COMad + ptcom->return_start[ptcom->Current_Times - 1] + i*2);
			nValue_H = *(U8 *)(COMad + ptcom->return_start[ptcom->Current_Times - 1] + i*2 + 1);
			
			*(U8 *)(COMad + i*2) = nValue_L;
			*(U8 *)(COMad + i*2 + 1) = nValue_H;
		}
	}
	else
	{
		nValue_L = *(U8 *)(COMad + 25);
		nValue_H = *(U8 *)(COMad + 26);	
		
		nValue = (nValue_H << 8) + nValue_L;
		
		nOffset = ptcom->address%16;	
	
		if (ptcom->send_staradd[98] == 1)//置位
		{			
			temp = 1 << nOffset;
			nValue = nValue | temp;
		}
		else if (ptcom->send_staradd[98] == 2)//复位
		{			
			temp = 1 << nOffset;
			temp = ~temp;			
			nValue = nValue & temp;
		}
		
		*(U8 *)(AD1 + 51) = (nValue >> 0) & 0xff;								//Write data
		*(U8 *)(AD1 + 52) = (nValue >> 8) & 0xff;
		
		usBCC = CalcBCC((U8 *)(AD1 + 27), 26); 
		*(U8 *)(AD1 + 53) = usBCC & 0xff;										//Check BCC
		
		ptcom->send_staradd[98] = 0;
	}
}
/*************************************************************************************************************************
//检查数据校检
*************************************************************************************************************************/
void watchcom(void)		
{
	unsigned int aakj=0;
	aakj=remark();
	if(aakj==1)															//校检玛正确
	{
		ptcom->IfResultCorrect=1;
		compxy();														//进入数据处理程序
	}
	else
	{
		ptcom->IfResultCorrect=0;
	}
}
/*************************************************************************************************************************
//返回来的数据计算校检码是否正确
*************************************************************************************************************************/
int remark()				
{
	U16 usRcvBCC;
	U16 usCalBCC;
	
	usRcvBCC = (*(U8 *)(COMad + ptcom->return_length[ptcom->Current_Times - 1] - 1)) & 0xff;

	usCalBCC = CalcBCC((U8 *)(COMad + 1), ptcom->return_length[ptcom->Current_Times-1] - 2) & 0xff;
;
	if(usRcvBCC == usCalBCC)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
/*************************************************************************************************************************
//BCC校验程序，取和后取反加1
*************************************************************************************************************************/
U16 CalcBCC(unsigned char *chData,unsigned short uNo)
{
	unsigned short BCC=0;
	U16 i;
	for(i=0;i<uNo;i++)
	{
		BCC=BCC+chData[i];
	}
	BCC=(~BCC+1)&0xff;
	return (BCC);
}
