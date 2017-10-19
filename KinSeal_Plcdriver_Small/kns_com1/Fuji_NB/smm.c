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
		case PLC_READ_DATA:															//���������Ƕ�����
		case PLC_READ_DIGITALALARM:														//���������Ƕ�����,����	
		case PLC_READ_TREND:														//���������Ƕ�����,����ͼ
		case PLC_READ_ANALOGALARM:														//���������Ƕ�����,��ȱ���	
		case PLC_READ_CONTROL:														//���������Ƕ�PLC���Ƶ�����	
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
		case PLC_WRITE_DATA:															//����������д����
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
		case PLC_WRITE_TIME:															//����������дʱ�䵽PLC
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
		case PLC_READ_TIME:															//���������Ƕ�ȡʱ�䵽PLC
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
		case PLC_WRITE_RECIPE:															//����������д�䷽��PLC
			switch(*(U8 *)(PE+3))										//�䷽�Ĵ�������
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
		case PLC_READ_RECIPE:															//���������Ǵ�PLC��ȡ�䷽
			switch(*(U8 *)(PE+3))										//�䷽�Ĵ�������
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
		case PLC_CHECK_DATA:															//�������������ݴ���
			watchcom();
			break;
		default:
			break;					
	}		 
}

/*************************************************************************************************************************
//��������
*************************************************************************************************************************/
void Read_Bool()
{
	U16 usBCC;
	int nAddr = 0;
	int nPlcSta = 0;
	int nRLen = 0;
	int nSendAddr = 0;
	
	nAddr	= ptcom->address;												//��ʼ��ַ
	nPlcSta	= ptcom->plc_address;											//PLCվ��ַ
	nRLen	= ptcom->register_length;										//���ζ�ȡ����
	
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
	
	ptcom->send_length[0] = 26;												//���ͳ���
	ptcom->send_staradd[0] = 0;												//�������ݴ洢��ַ	
		
	ptcom->return_length[0] = 26 + nRLen*2;									//�������ݳ���
	ptcom->return_start[0] = 25;											//����������Ч��ʼ
	ptcom->return_length_available[0] = nRLen*2;							//������Ч���ݳ���
	
	ptcom->send_times = 1;													//���ʹ���
	ptcom->Current_Times = 0;												//��ǰ���ʹ���
	
	ptcom->send_staradd[99] = 2;	
}

/*************************************************************************************************************************
//д������
*************************************************************************************************************************/
void setreset()
{
	U16 usBCC;
	int nAddr = 0;
	int nPlcSta = 0;
	int nSendAddr = 0;
	
	nAddr	= ptcom->address;												//��ʼ��ַ
	nPlcSta	= ptcom->plc_address;											//PLCվ��ַ

//�ȶ�----------------------------------------------------------------------------------------------------------------------	
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
	
	ptcom->send_length[0] = 26;												//���ͳ���
	ptcom->send_staradd[0] = 0;												//�������ݴ洢��ַ	
		
	ptcom->return_length[0] = 28;											//�������ݳ���
	ptcom->return_start[0] = 25;											//����������Ч��ʼ
	ptcom->return_length_available[0] = 2;									//������Ч���ݳ���	
	
	ptcom->send_add[0] = nSendAddr*16;										//�����������ַ������	

//��д-------------------------------------------------------------------------------------------------------------------------	
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
	
	ptcom->send_length[1] = 28;												//���ͳ���
	ptcom->send_staradd[1] = 26;											//�������ݴ洢��ַ	
		
	ptcom->return_length[1] = 26;											//�������ݳ���
	ptcom->return_start[1] = 0;												//����������Ч��ʼ
	ptcom->return_length_available[1] = 0;									//������Ч���ݳ���
	
	ptcom->send_times = 2;													//���ʹ���
	ptcom->Current_Times = 0;												//��ǰ���ʹ���
	
	ptcom->send_staradd[99] = 1;
	
	if (ptcom->writeValue == 1)												//��λ
	{
		ptcom->send_staradd[98] = 1;
	}	
	if (ptcom->writeValue == 0)												//��λ
	{
		ptcom->send_staradd[98] = 2;
	}	
}

/*************************************************************************************************************************
//��ģ����
*************************************************************************************************************************/
void Read_Analog()				
{
	U16 usBCC;
	int nAddr = 0;
	int nPlcSta = 0;
	int nRLen = 0;
	
	nAddr	= ptcom->address;												//��ʼ��ַ
	nPlcSta	= ptcom->plc_address;											//PLCվ��ַ
	nRLen	= ptcom->register_length;										//���ζ�ȡ����
	
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
	
	ptcom->send_length[0] = 26;												//���ͳ���
	ptcom->send_staradd[0] = 0;												//�������ݴ洢��ַ	
		
	ptcom->return_length[0] = 26 + nRLen*2;									//�������ݳ���
	ptcom->return_start[0] = 25;											//����������Ч��ʼ
	ptcom->return_length_available[0] = nRLen*2;							//������Ч���ݳ���
	
	ptcom->send_times = 1;													//���ʹ���
	ptcom->Current_Times = 0;												//��ǰ���ʹ���
	
	ptcom->send_staradd[99] = 0;
}
/*************************************************************************************************************************
//дģ����
*************************************************************************************************************************/
void Write_Analog()				
{
	U16 usBCC;
	int nAddr = 0;
	int nPlcSta = 0;
	int nWLen = 0;
	int i = 0;
	
	nAddr	= ptcom->address;												//��ʼ��ַ
	nPlcSta	= ptcom->plc_address;											//PLCվ��ַ
	nWLen	= ptcom->register_length;										//���ζ�ȡ����
	
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
	
	ptcom->send_length[0] = 26 + nWLen*2;									//���ͳ���
	ptcom->send_staradd[0] = 0;												//�������ݴ洢��ַ	
		
	ptcom->return_length[0] = 26;											//�������ݳ���
	ptcom->return_start[0] = 0;												//����������Ч��ʼ
	ptcom->return_length_available[0] = 0;									//������Ч���ݳ���
	
	ptcom->send_times = 1;													//���ʹ���
	ptcom->Current_Times = 0;												//��ǰ���ʹ���
	
	ptcom->send_staradd[99] = 0;
}
/*************************************************************************************************************************
//��ȡ�䷽
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
	
	nAddr	= ptcom->address;												//��ʼ��ַ
	nPlcSta	= ptcom->plc_address;											//PLCվ��ַ
	nRLen	= ptcom->register_length;										//���ζ�ȡ����

	if(Datalength > 5000)													//�����䷽����
	{
		Datalength = 5000;
	}                								
		
	if(Datalength%64 == 0)													//ÿ������ܷ���64��D
	{
		SendTimes = Datalength/64;      									//���͵Ĵ���
		LastTimeWord = 64;                									//�̶�����64	
	}
	if(Datalength%64 != 0)
	{
		SendTimes = Datalength/64 + 1;      								//���͵Ĵ���
		LastTimeWord = Datalength%64;     									//���һ�η��͵ĳ���	
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
		
		ptcom->send_length[i] = 26;											//���ͳ���
		ptcom->send_staradd[i] = ps;										//�������ݴ洢��ַ	
			
		ptcom->return_length[i] = 26 + nRLen*2;								//�������ݳ���
		ptcom->return_start[i] = 25;										//����������Ч��ʼ
		ptcom->return_length_available[i] = nRLen*2;						//������Ч���ݳ���
		
		ps = 26;
		
		ptcom->send_staradd[99] = 0;
	}
	ptcom->send_times = SendTimes;											//���ʹ���
	ptcom->Current_Times = 0;												//��ǰ���ʹ���	
}
/*************************************************************************************************************************
//д�䷽��PLC
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
	
	nAddr	= ((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//��ʼ��ַ
	nPlcSta	= *(U8 *)(PE+4);												//PLCվ��ַ
	Datalength = ((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));						//���γ���	             								
		
	if(Datalength%64 == 0)													//ÿ������ܷ���64��D
	{
		SendTimes = Datalength/64;      									//���͵Ĵ���
		LastTimeWord = 64;                									//�̶�����64	
	}
	if(Datalength%64 != 0)
	{
		SendTimes = Datalength/64 + 1;      								//���͵Ĵ���
		LastTimeWord = Datalength%64;     									//���һ�η��͵ĳ���	
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
		
		ptcom->send_length[i] = 26 + nWLen*2;								//���ͳ���
		ptcom->send_staradd[i] = ps*i;										//�������ݴ洢��ַ	
			
		ptcom->return_length[i] = 26;										//�������ݳ���
		ptcom->return_start[i] = 0;											//����������Ч��ʼ
		ptcom->return_length_available[i] = 0;								//������Ч���ݳ���	
		
		ptcom->send_staradd[99] = 0;
	}
	ptcom->send_times = SendTimes;											//���ʹ���	
	ptcom->Current_Times=0;													//��ǰ���ʹ���
}
/*************************************************************************************************************************
//дʱ�䵽PLC
*************************************************************************************************************************/
void Write_Time()
{
	Write_Analog();							
}
/*************************************************************************************************************************
//��PLC��ȡʱ��
*************************************************************************************************************************/
void Read_Time()									
{
	Read_Analog();
}
/*************************************************************************************************************************
//����ɱ�׼�洢��ʽ
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
	
		if (ptcom->send_staradd[98] == 1)//��λ
		{			
			temp = 1 << nOffset;
			nValue = nValue | temp;
		}
		else if (ptcom->send_staradd[98] == 2)//��λ
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
//�������У��
*************************************************************************************************************************/
void watchcom(void)		
{
	unsigned int aakj=0;
	aakj=remark();
	if(aakj==1)															//У������ȷ
	{
		ptcom->IfResultCorrect=1;
		compxy();														//�������ݴ������
	}
	else
	{
		ptcom->IfResultCorrect=0;
	}
}
/*************************************************************************************************************************
//�����������ݼ���У�����Ƿ���ȷ
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
//BCCУ�����ȡ�ͺ�ȡ����1
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
