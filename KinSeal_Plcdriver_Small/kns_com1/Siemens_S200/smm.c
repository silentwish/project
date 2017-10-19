
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
		case PLC_READ_DATA:					//���������Ƕ�����0
		case PLC_READ_DIGITALALARM:			//���������Ƕ�����,����10	
		case PLC_READ_TREND:				//���������Ƕ�����,����ͼ11
		case PLC_READ_ANALOGALARM:			//���������Ƕ�����,��ȱ���	12
		case PLC_READ_CONTROL:				//���������Ƕ�PLC���Ƶ�����13	
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
	case PLC_WRITE_DATA:				//����������д����1
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
	case PLC_WRITE_TIME:				//����������дʱ�䵽PLC 3
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
	case PLC_READ_TIME:				//���������Ƕ�ȡʱ�䵽PLC 4
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
	case PLC_WRITE_RECIPE:				//����������д�䷽��PLC 5
		switch(*(U8 *)(PE+3))//�䷽�Ĵ�������
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
	case PLC_READ_RECIPE:				//���������Ǵ�PLC��ȡ�䷽ 6
		switch(*(U8 *)(PE+3))//�䷽�Ĵ�������
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
	case PLC_CHECK_DATA:				//�������������ݴ���2
		watchcom();
		break;				
	}	 	 
}

/**************************************************************************************************************************************************
Description: ��λ��λ����
***************************************************************************************************************************************************/
void Set_Reset()
{
	U16 aakj;
	int b,stationAdd;
	int a1,a2,a3;
	int YM_Check;
	int value;	//��λ��λ
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
	b=ptcom->address;			// ��λ��ַ
	value=ptcom->writeValue;	//д���ֵ
	stationAdd=ptcom->plc_address;	//վ��ַ

	a1=(b>>16)&0xff;
	a2=(b>>8)&0xff;
	a3=b&0xff;	
		
	*(U8 *)(AD1+0)=0x68;			//SD--��ʼ���� 68H��
	*(U8 *)(AD1+1)=0x20;			//Byte 2: LE�� Ler--���ȣ� ��ȥ�� Bit0-Bit3,У���룬������������ݳ��ȣ���
	*(U8 *)(AD1+2)=0x20;
	
	*(U8 *)(AD1+3)=0x68;			//SD����ʼ���� 68H��
	*(U8 *)(AD1+4)=stationAdd;		//վ��ַ  
	*(U8 *)(AD1+5)=0x00;			//SA--Դ��ַ
	*(U8 *)(AD1+6)=0x6c;			//FC--������ �� 6CH��
	*(U8 *)(AD1+7)=0x32;
	*(U8 *)(AD1+8)=0x01;
	*(U8 *)(AD1+9)=0x00;
	*(U8 *)(AD1+10)=0x00;
	*(U8 *)(AD1+11)=0x01;
	*(U8 *)(AD1+12)=0x01;
	*(U8 *)(AD1+13)=0x00;
	
	*(U8 *)(AD1+14)=0x0e;			//���ݿ�ռλ�ֽ�
	*(U8 *)(AD1+15)=0x00;
	*(U8 *)(AD1+16)=0x05;
	*(U8 *)(AD1+17)=0x05;
	*(U8 *)(AD1+18)=0x01;
	*(U8 *)(AD1+19)=0x12;
	*(U8 *)(AD1+20)=0x0a;
	*(U8 *)(AD1+21)=0x10;

	*(U8 *)(AD1+22)=0x01;			//��ȡ���ݵĳ���
	*(U8 *)(AD1+23)=0x00;
	*(U8 *)(AD1+24)=0x01;			//���ֽ�Ϊ��λ��������ȡ���ֽ�����
	*(U8 *)(AD1+25)=0x00;
	if (ptcom->registerr=='H')
	{
		*(U8 *)(AD1+26)=0x01;		//�洢������ 01��
	}
	else
	{
		*(U8 *)(AD1+26)=0x00;
	}
/*Byte 27 �� �洢������
Byte26 Byte27
Q 0x00 0x82
M 0x00 0x83
V_BIT 0x01 0x84
S 0x00 0x04
SM 0x00 0x05*/
	*(U8 *)(AD1+27)=YM_Check;		//Y,M,V_B,S,SM
	
	*(U8 *)(AD1+28)=a1;				//Byte 28,29,30�� �洢��ƫ����ָ�루�洢����ַ*8��
	*(U8 *)(AD1+29)=a2;				//�Ĵ�����ַ
	*(U8 *)(AD1+30)=a3;
	
	*(U8 *)(AD1+31)=0x00;
	*(U8 *)(AD1+32)=0x03;
	*(U8 *)(AD1+33)=0x00;
	*(U8 *)(AD1+34)=0x01;
	*(U8 *)(AD1+35)=value;			//��λ��λ
	aakj=CalcHe((U8 *)AD1,36);
	a2=aakj&0xff;
	*(U8 *)(AD1+36)=a2;				//��У��
	*(U8 *)(AD1+37)=0x16;			//������
	/*�ڶ�������*/
	*(U8 *)(AD1+38)=0x10;
	*(U8 *)(AD1+39)=stationAdd;
	*(U8 *)(AD1+40)=0x00;
	*(U8 *)(AD1+41)=0x5c;
	aakj=Second_Send_CalcHe((U8 *)(AD1+39),3);
	a2=aakj&0xff;
	*(U8 *)(AD1+42)=a2;				//��У��
	*(U8 *)(AD1+43)=0x16;


//������200ͨ�Ŷ�Ҫ����2�β������һ��ͨ��
	ptcom->send_length[0]=38;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ			
	ptcom->return_length[0]=1;				//�������ݳ���
	ptcom->return_start[0]=0;				//����������Ч��ʼ
	ptcom->return_length_available[0]=0;	//������Ч���ݳ���	

	
	ptcom->send_length[1]=6;				//���ͳ���
	ptcom->send_staradd[1]=38;				//�������ݴ洢��ַ			
	ptcom->return_length[1]=24;				//�������ݳ���
	ptcom->return_start[1]=0;				//����������Ч��ʼ
	ptcom->return_length_available[1]=0;	//������Ч���ݳ���	
		
	
	ptcom->send_times=2;					//���ʹ���
	ptcom->Current_Times=0;					//��ǰ���ʹ���					
}


void Read_Bool()					//��ȡ����������Ϣ
{
	U16 aakj;
	int b,stationAdd;
	int a1,a2,a3;
	int YM_Check;
	int length;
	int i;
	

	length=ptcom->register_length;	//��ȡ����
	stationAdd=ptcom->plc_address;	//վ��ַ	
	b=ptcom->address;				// ��λ��ַ
	if (length==1)
	{
		length=2;
		ptcom->register_length=2;	//�����Ӳ���ֻ��һ���ֽڣ�����һ����
	}
	#if 0
	*(U16*)((U16*)LW+ 600) = ptcom->address;
	*(U16*)((U16*)LW+ 601) = length;
	*(U16*)((U16*)LW+ 599) = b;	
	#endif
/*
�������ʱ��ʼ��ַ�仯�ˣ���ÿ���ֽڵ�0λ��ʼ��ȥ��λ��ַ
��M10.7,����ʱ����87(ת����)�������Ż���������80
*/			
			
	switch (ptcom->registerr)
	{
	case 'X':
		YM_Check=0x81;
		a1=b;
		a2=a1/8;
		b=a2*8;				//�����ӹ涨��ַҪ����8��
		a1=(b>>16)&0xff;
		a2=(b>>8)&0xff;
		a3=b&0xff;				
		break;
	case 'Y':
		YM_Check=0x82;
		a1=b;
		a2=a1/8;	
		b=a2*8;				//�����ӹ涨��ַҪ����8��
		a1=(b>>16)&0xff;
		a2=(b>>8)&0xff;
		a3=b&0xff;					
		break;		
	case 'M':
		YM_Check=0x83;
		a1=b;
		a2=a1/8;	
		b=a2*8;				//�����ӹ涨��ַҪ����8��
		a1=(b>>16)&0xff;
		a2=(b>>8)&0xff;
		a3=b&0xff;					
		break;		
	case 'H':				//V_BIT
		YM_Check=0x84;
		a1=b;
		a2=a1/8;
		b=a2*8;				//�����ӹ涨��ַҪ����8��
		a1=(b>>16)&0xff;
		a2=(b>>8)&0xff;
		a3=b&0xff;						
		break;	
	case 'L':				//S
		YM_Check=0x04;
		a1=b;
		a2=a1/8;	
		b=a2*8;				//�����ӹ涨��ַҪ����8��
		a1=(b>>16)&0xff;
		a2=(b>>8)&0xff;
		a3=b&0xff;					
		break;		
	case 'K':				//SM
		YM_Check=0x05;
		a1=b;
		a2=a1/8;
		b=a2*8;				//�����ӹ涨��ַҪ����8��
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

	*(U8 *)(AD1+0)=0x68;			//SD,��ʼ���� 68H��
	*(U8 *)(AD1+1)=0x1b;			//Byte 1, Byte 2: LE�� Ler--���ȣ� ��ȥ�� Bit0-Bit3,У���룬 Byte 32 ������ݳ��ȣ���
	*(U8 *)(AD1+2)=0x1b;
	*(U8 *)(AD1+3)=0x68;			//Byte 3: SD����ʼ���� 68H��

	*(U8 *)(AD1+4)=stationAdd;		//վ��ַ
	*(U8 *)(AD1+5)=0x00;			//Byte 5�� SA--Դ��ַ
	*(U8 *)(AD1+6)=0x6c;			//Byte 6�� FC--������ �� 6CH��
	*(U8 *)(AD1+7)=0x32;

	*(U8 *)(AD1+8)=0x01;
	*(U8 *)(AD1+9)=0x00;
	*(U8 *)(AD1+10)=0x00;
	*(U8 *)(AD1+11)=0x00;
	
	*(U8 *)(AD1+12)=0x00;
	*(U8 *)(AD1+13)=0x00;
	*(U8 *)(AD1+14)=0x0e;			//Byte14: ���ݿ�ռλ�ֽ�
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
		*(U8 *)(AD1+22)=0x1F;		//Byte 22: ��ȡ���ݵĳ���	
		*(U8 *)(AD1+23)=0x00;
		*(U8 *)(AD1+24)=(length*8) & 0xff;			//��ȡ����,��ȡÿ��T,Cʱ����ȡ�ĳ���ָ���ٸ�T
		*(U8 *)(AD1+25)=0x00;		
		break;		
	case 'C':		//C
		*(U8 *)(AD1+22)=0x1E;			
		*(U8 *)(AD1+23)=0x00;	
		*(U8 *)(AD1+24)=(length*8) & 0xff;			//��ȡ����,��ȡÿ��T,Cʱ����ȡ�ĳ���ָ���ٸ�T
		*(U8 *)(AD1+25)=0x00;		
		break;	
	default:	
		*(U8 *)(AD1+22)=0x02;			//���ֽڶ�ȡ����
		*(U8 *)(AD1+23)=0x00;	
		*(U8 *)(AD1+24)=length;			//��ȡ����
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
Byte 27 �� �洢������
I��0x81
Q��0x82
M��0x83
S��0x04
SM��0x05
*/
	*(U8 *)(AD1+27)=YM_Check;		//X,Y,M,V_BIT
	
	*(U8 *)(AD1+28)=a1;				//��ʼ��ַ
	*(U8 *)(AD1+29)=a2;
	*(U8 *)(AD1+30)=a3;	
	
	aakj=CalcHe((U8 *)AD1,31);
	a2=aakj&0xff;
	*(U8 *)(AD1+31)=a2;				//У��ͣ���(DA+SA+DSAP+SSAP+DU) Mod 256 ��
	*(U8 *)(AD1+32)=0x16;			//������
	/*�ڶ������ݣ�*/
	*(U8 *)(AD1+33)=0x10;
	*(U8 *)(AD1+34)=stationAdd;
	*(U8 *)(AD1+35)=0x00;
	*(U8 *)(AD1+36)=0x5c;
	aakj=Second_Send_CalcHe((U8 *)(AD1+34),3);
	a2=aakj&0xff;
	*(U8 *)(AD1+37)=a2;				//��У��
	*(U8 *)(AD1+38)=0x16;

//������200ͨ�Ŷ�Ҫ����2�β������һ��ͨ��
	ptcom->send_length[0]=33;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ			
	ptcom->return_length[0]=1;				//�������ݳ���
	ptcom->return_start[0]=0;				//����������Ч��ʼ
	ptcom->return_length_available[0]=0;	//������Ч���ݳ���	

	
	ptcom->send_length[1]=6;				//���ͳ���
	ptcom->send_staradd[1]=33;				//�������ݴ洢��ַ	
	if (ptcom->registerr=='T')
	{		
		ptcom->return_length[1]=25+2+length*8*5;	//�������ݳ���,25���̶���2λ��У��ͽ�����,ÿ���ֽڶ�ȡ8��T,C��ÿ��T5���ֽڷ���
	}
	else if(ptcom->registerr=='C')
	{
		ptcom->return_length[1]=25+2+length*8*3;	//�������ݳ���,25���̶���2λ��У��ͽ�����,ÿ���ֽڶ�ȡ8��T,C��ÿ��T5���ֽڷ���
	}
	else
	{
		ptcom->return_length[1]=25+2+length;	//�������ݳ���,25���̶���2λ��У��ͽ�����
	}
	ptcom->return_start[1]=25;				//����������Ч��ʼ
	ptcom->return_length_available[1]=length;	//������Ч���ݳ���	
		
	
	ptcom->send_times=2;					//���ʹ���
	ptcom->Current_Times=0;					//��ǰ���ʹ���					
	
}



void Read_Analog()				//��ģ����
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
	length=ptcom->register_length;	//��ȡ����,//��Ϊ��λ
	length=length*2;				//�������ֽ���
	if (length==1)
	{
		length=2;
		ptcom->register_length=2;	//�����Ӳ���ֻ��һ���ֽڣ�����һ����
	}
	stationAdd=ptcom->plc_address;	//վ��ַ	

	if (ptcom->registerr=='R')
	{
		b=ptcom->address * 2;		// ����ʱ���ֵ�ַ���أ�������Ҫ�Ŵ�2��
		length=length*2;
	}
	else if (ptcom->registerr=='D' || ptcom->registerr=='N' ||ptcom->registerr=='v'|| ptcom->registerr=='x'
		|| ptcom->registerr=='y'|| ptcom->registerr=='m'|| ptcom->registerr=='l')
	{
		b=ptcom->address * 2;		// ����ʱ���ֵ�ַ���أ�������Ҫ�Ŵ�2��
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

	*(U8 *)(AD1+4)=stationAdd;		//վ��ַ
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
		*(U8 *)(AD1+22)=0x02;			//���ֽڶ�ȡ����
		*(U8 *)(AD1+23)=0x00;
		*(U8 *)(AD1+24)=length;			//��ȡ����
		*(U8 *)(AD1+25)=0x00;			
		break;
	case 't':		//t
		*(U8 *)(AD1+22)=0x1F;			
		*(U8 *)(AD1+23)=0x00;
		*(U8 *)(AD1+24)=length;		//��ȡ����,��ȡÿ��T,Cʱ����ȡ�ĳ���ָ���ٸ�T
		*(U8 *)(AD1+25)=0x00;			
		break;		
	case 'c':		//c
		*(U8 *)(AD1+22)=0x1E;			
		*(U8 *)(AD1+23)=0x00;
		*(U8 *)(AD1+24)=length;		//��ȡ����,��ȡÿ��T,Cʱ����ȡ�ĳ���ָ���ٸ�T
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
	
	*(U8 *)(AD1+28)=a1;				//��ʼ��ַ
	*(U8 *)(AD1+29)=a2;
	*(U8 *)(AD1+30)=a3;	
	
	aakj=CalcHe((U8 *)AD1,31);
	a2=aakj&0xff;
	*(U8 *)(AD1+31)=a2;          //У��
	*(U8 *)(AD1+32)=0x16;		

	*(U8 *)(AD1+33)=0x10;
	*(U8 *)(AD1+34)=stationAdd;
	*(U8 *)(AD1+35)=0x00;
	*(U8 *)(AD1+36)=0x5c;
	aakj=Second_Send_CalcHe((U8 *)(AD1+34),3);
	a2=aakj&0xff;
	*(U8 *)(AD1+37)=a2;				//��У��
	*(U8 *)(AD1+38)=0x16;
	
//������200ͨ�Ŷ�Ҫ����2�β������һ��ͨ��
	ptcom->send_length[0]=33;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ			
	ptcom->return_length[0]=1;				//�������ݳ���
	ptcom->return_start[0]=0;				//����������Ч��ʼ
	ptcom->return_length_available[0]=0;	//������Ч���ݳ���	

	
	ptcom->send_length[1]=6;				//���ͳ���
	ptcom->send_staradd[1]=33;				//�������ݴ洢��ַ	
	if (ptcom->registerr=='t' || ptcom->registerr=='c')
	{	
		if(	ptcom->registerr=='t')
		{
		    ptcom->return_length[1]=25+2+length*5;	//�������ݳ���,25���̶���2λ��У��ͽ�����,ÿ���ֽڶ�ȡ8��T,C��ÿ��T5���ֽڷ���
		}
		else
		{
			ptcom->return_length[1]=25+2+length*3;	//�������ݳ���,25���̶���2λ��У��ͽ�����,ÿ���ֽڶ�ȡ8��T,C��ÿ��T5���ֽڷ���
		}
	}
	else
	{
		ptcom->return_length[1]=25+2+length;	//�������ݳ���,25���̶���2λ��У��ͽ�����
	}			
	ptcom->return_start[1]=25;				//����������Ч��ʼ
	ptcom->return_length_available[1]=length;	//������Ч���ݳ���	
	
	if(ptcom->registerr=='R')
	{
		ptcom->return_length_available[1]=length * 4;
	}
	ptcom->send_times=2;					//���ʹ���
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	
}


void Read_Recipe()								//��ȡ�䷽
{
	U16 aakj;
	int b;
	int a1,a2,a3,a4,a5;
	int i;
	int datalength;
	int staradd;
	int ps;
	int SendTimes;
	int LastTimeWord,length;							//���һ�η��ͳ���
	int stationAdd;
	
	datalength=ptcom->register_length;		//�����ܳ���
	staradd=ptcom->address*2;				//��ʼ��ַ��// ����ʱ���ֵ�ַ���أ�������Ҫ�Ŵ�2��
	if(datalength>5000)
		datalength=5000;
	
	//ÿ������ܷ���32��D
	if(datalength%32==0)
	{
		SendTimes=datalength/32;
		LastTimeWord=32;//���һ�η��͵ĳ���	
	}
	if(datalength%32!=0)
	{
		SendTimes=datalength/32+1;//���͵Ĵ���
		LastTimeWord=datalength%32;//���һ�η��͵ĳ���	
	}

	ps=39;
	stationAdd=ptcom->plc_address;			//վ��ַ	
	for (i=0;i<SendTimes;i++)
	{
		b=staradd+i*32*2;					// ��ʼ��ַ,V��С��λ���ֽ�
		if (i==SendTimes-1)				//���һ��
		{
			length=LastTimeWord;	//д��ĳ���
		}
		else
		{
			length=32;					//32����
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

		*(U8 *)(AD1+4+ps*i)=stationAdd;		//վ��ַ
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
		
		*(U8 *)(AD1+22+ps*i)=0x02;			//���ֽڶ�ȡ����
		*(U8 *)(AD1+23+ps*i)=0x00;	
			
		*(U8 *)(AD1+24+ps*i)=length*2;		//��ȡ����
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
		
		*(U8 *)(AD1+28+ps*i)=a3;				//��ʼ��ַ
		*(U8 *)(AD1+29+ps*i)=a4;
		*(U8 *)(AD1+30+ps*i)=a5;	
		
		aakj=CalcHe((U8 *)AD1+ps*i,31);
		a2=aakj&0xff;
		*(U8 *)(AD1+31+ps*i)=a2;
		*(U8 *)(AD1+32+ps*i)=0x16;	
		
		ptcom->send_length[i*2]=33;			//���ͳ���
		ptcom->send_staradd[i*2]=i*ps;		//�������ݴ洢��ַ	
		ptcom->send_add[i*2]=b/2;				//�����������ַ������	
		ptcom->send_data_length[i*2]=length;	//�������һ�ζ���32��D		
		
		ptcom->return_length[i*2]=1;			//�������ݳ���
		ptcom->return_start[i*2]=0;				//����������Ч��ʼ
		ptcom->return_length_available[i*2]=0;	//������Ч���ݳ���			
		
		*(U8 *)(AD1+33+ps*i)=0x10;
		*(U8 *)(AD1+34+ps*i)=stationAdd;
		*(U8 *)(AD1+35+ps*i)=0x00;
		*(U8 *)(AD1+36+ps*i)=0x5c;
		aakj=Second_Send_CalcHe((U8 *)(AD1+34+ps*i),3);
		a2=aakj&0xff;
		*(U8 *)(AD1+37+ps*i)=a2;				//��У��
		*(U8 *)(AD1+38+ps*i)=0x16;
		
		ptcom->send_length[i*2+1]=6;		//���ͳ���
		ptcom->send_staradd[i*2+1]=i*ps+33;			//�������ݴ洢��ַ
		ptcom->send_add[i*2+1]=b/2;				//�����������ַ������	
		ptcom->send_data_length[i*2+1]=length;	//�������һ�ζ���32��D	
					
		ptcom->return_length[i*2+1]=25+2+length*2;				//�������ݳ���
		ptcom->return_start[i*2+1]=25;				//����������Ч��ʼ
		ptcom->return_length_available[i*2+1]=length*2;	//������Ч���ݳ���			
		
	}
	ptcom->send_times=SendTimes*2;					//���ʹ���
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	
		
}

void writet(void)								//д��ʱ�� ������
{
	U16 aakj;
	int b,stationAdd;
	int a1,a2,a3,a4,a5;
	int YM_Check;
	int LE,LER,length;
	int k1,k2,k3,k4,k5;
	int i;

	b=ptcom->address;
	
	length=ptcom->register_length;	//д��ĳ���
	stationAdd=ptcom->plc_address;	//վ��ַ
	
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
		LE=0x21+length*5-2;				//length����Ϊ��λ
		LER=0x21+length*5-2;
	
		break;		
	case 'c':		//c
		YM_Check=0x1E;
		
		k1=4+length*3;
		k2=length;
		k3=5<<(2+length);
		k4=((length*3*8) >> 0) & 0xff;
		k5=((length*3*8) >> 8) & 0xff;
		LE=0x21+length*3-2;				//length����Ϊ��λ
		LER=0x21+length*3-2;
		break;		
	}
	
	
	
			
	*(U8 *)(AD1+0)=0x68;
	*(U8 *)(AD1+1)=LE;
	*(U8 *)(AD1+2)=LER;
	*(U8 *)(AD1+3)=0x68;

	*(U8 *)(AD1+4)=stationAdd;		//վ��ַ
	*(U8 *)(AD1+5)=0x00;
	if(ptcom->registerr == 't')
	{
		*(U8 *)(AD1+6)=0x7c;			//����д��
	}
	else
	{
		*(U8 *)(AD1+6)=0x6c;			//����д��
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
	*(U8 *)(AD1+28)=a3;				//�Ĵ�����ַ
	*(U8 *)(AD1+29)=a4;				
	*(U8 *)(AD1+30)=a5;
	*(U8 *)(AD1+31)=0x00;
	*(U8 *)(AD1+32)=0x04;
	
	*(U8 *)(AD1+33)=k5;
	*(U8 *)(AD1+34)=k4;
	
	*(U8 *)(AD1+35)=0x00;			//״̬
	
	*(U8 *)(AD1+36)=0x00;			//����δ֪
	*(U8 *)(AD1+37)=0x00;	
	for (i=0;i<length*2;i++)
	{
		if (i%2==0)
		{
			*(U8 *)(AD1+38+i)=ptcom->U8_Data[i+1];			//�ߵ��ֽڽ���
		}
		else
		{
			*(U8 *)(AD1+38+i)=ptcom->U8_Data[i-1];		
		}	
	}

	aakj=CalcHe((U8 *)AD1,38+length*2);
	a2=aakj&0xff;
	*(U8 *)(AD1+38+length*2)=a2;				//��У��
	*(U8 *)(AD1+39+length*2)=0x16;
	
	
	*(U8 *)(AD1+40+length*2)=0x10;
	*(U8 *)(AD1+41+length*2)=stationAdd;
	*(U8 *)(AD1+42+length*2)=0x00;
	*(U8 *)(AD1+43+length*2)=0x5c;
	aakj=Second_Send_CalcHe((U8 *)(AD1+41+length*2),3);
	a2=aakj&0xff;
	*(U8 *)(AD1+44+length*2)=a2;				//��У��
	*(U8 *)(AD1+45+length*2)=0x16;	
	

//������200ͨ�Ŷ�Ҫ����2�β������һ��ͨ��
	ptcom->send_length[0]=39+length*2+1;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ			
	ptcom->return_length[0]=1;				//�������ݳ���
	ptcom->return_start[0]=0;				//����������Ч��ʼ
	ptcom->return_length_available[0]=0;	//������Ч���ݳ���	

	
	ptcom->send_length[1]=6;				//���ͳ���
	ptcom->send_staradd[1]=39+length*2+1;		//�������ݴ洢��ַ			
	ptcom->return_length[1]=24;				//�������ݳ���
	ptcom->return_start[1]=0;				//����������Ч��ʼ
	ptcom->return_length_available[1]=0;	//������Ч���ݳ���	
		
	
	ptcom->send_times=2;					//���ʹ���
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
}

void writec(void)								//д��ʱ�� ������
{
	U16 aakj;
	int b,stationAdd;
	int a1,a2,a3,a4,a5;
	int YM_Check;
	int LE,LER,length;
	int k1,k2,k3,k4,k5;
	int i;

	b=ptcom->address;
	
	length=ptcom->register_length;	//д��ĳ���
	stationAdd=ptcom->plc_address;	//վ��ַ
	
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
		LE=0x21+length*5-2;				//length����Ϊ��λ
		LER=0x21+length*5-2;
	
		break;		
	case 'c':		//c
		YM_Check=0x1E;
		
		k1=4+length*3;
		k2=length;
		k3=5<<(2+length);
		k4=((length*3*8) >> 0) & 0xff;
		k5=((length*3*8) >> 8) & 0xff;
		LE=0x21+length*3-2;				//length����Ϊ��λ
		LER=0x21+length*3-2;
		break;		
	}
	
	
	
			
	*(U8 *)(AD1+0)=0x68;
	*(U8 *)(AD1+1)=LE;
	*(U8 *)(AD1+2)=LER;
	*(U8 *)(AD1+3)=0x68;

	*(U8 *)(AD1+4)=stationAdd;		//վ��ַ
	*(U8 *)(AD1+5)=0x00;
	if(ptcom->registerr == 't')
	{
		*(U8 *)(AD1+6)=0x7c;			//����д��
	}
	else
	{
		*(U8 *)(AD1+6)=0x6c;			//����д��
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
	*(U8 *)(AD1+28)=a3;				//�Ĵ�����ַ
	*(U8 *)(AD1+29)=a4;				
	*(U8 *)(AD1+30)=a5;
	*(U8 *)(AD1+31)=0x00;
	*(U8 *)(AD1+32)=0x04;
	
	*(U8 *)(AD1+33)=k5;
	*(U8 *)(AD1+34)=k4;
	
	*(U8 *)(AD1+35)=0x00;			//״̬
	
	//*(U8 *)(AD1+36)=0x00;			//����δ֪
	//*(U8 *)(AD1+37)=0x00;	
	for (i=0;i<length*2;i++)
	{
		if (i%2==0)
		{
			*(U8 *)(AD1+36+i)=ptcom->U8_Data[i+1];			//�ߵ��ֽڽ���
		}
		else
		{
			*(U8 *)(AD1+36+i)=ptcom->U8_Data[i-1];		
		}	
	}

	aakj=CalcHe((U8 *)AD1,36+length*2);
	a2=aakj&0xff;
	*(U8 *)(AD1+36+length*2)=a2;				//��У��
	*(U8 *)(AD1+37+length*2)=0x16;
	
	
	*(U8 *)(AD1+38+length*2)=0x10;
	*(U8 *)(AD1+39+length*2)=stationAdd;
	*(U8 *)(AD1+40+length*2)=0x00;
	*(U8 *)(AD1+41+length*2)=0x5c;
	aakj=Second_Send_CalcHe((U8 *)(AD1+39+length*2),3);
	a2=aakj&0xff;
	*(U8 *)(AD1+42+length*2)=a2;				//��У��
	*(U8 *)(AD1+43+length*2)=0x16;	
	

//������200ͨ�Ŷ�Ҫ����2�β������һ��ͨ��
	ptcom->send_length[0]=38+length*2;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ			
	ptcom->return_length[0]=1;				//�������ݳ���
	ptcom->return_start[0]=0;				//����������Ч��ʼ
	ptcom->return_length_available[0]=0;	//������Ч���ݳ���	

	
	ptcom->send_length[1]=6;				//���ͳ���
	ptcom->send_staradd[1]=37+length*2+1;		//�������ݴ洢��ַ			
	ptcom->return_length[1]=24;				//�������ݳ���
	ptcom->return_start[1]=0;				//����������Ч��ʼ
	ptcom->return_length_available[1]=0;	//������Ч���ݳ���	
		
	
	ptcom->send_times=2;					//���ʹ���
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
}

void Write_Analog()								//дģ����
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
	
	length=ptcom->register_length;	//д��ĳ���
	stationAdd=ptcom->plc_address;	//վ��ַ
	if(ptcom->registerr == 'R')
	{
		b=ptcom->address * 2;				// ����ʱ���ֵ�ַ���أ�������Ҫ�Ŵ�2��
	}
	else
	{
		b=ptcom->address*2;				// ����ʱ���ֵ�ַ���أ�������Ҫ�Ŵ�2��
	}
	//sysprintf("ptcom->address=%d\n",ptcom->address);
	
	a1=b;
	a1=a1*8;
	a3=a1/0x10000;
	a4=(a1-a3*0x10000)/0x100;
	a5=a1-a3*0x10000-a4*0x100;	

	LE=0x21+length*2-2;				//length����Ϊ��λ
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

	*(U8 *)(AD1+4)=stationAdd;		//վ��ַ
	*(U8 *)(AD1+5)=0x00;
	*(U8 *)(AD1+6)=0x6c;			//����д��
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
		
	*(U8 *)(AD1+22)=0x02;			//���ֽڶ�ȡ����
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
	*(U8 *)(AD1+28)=a3;				//�Ĵ�����ַ
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
				*(U8 *)(AD1+35+i)=ptcom->U8_Data[i+1];			//�ߵ��ֽڽ���
			}
			else
			{
				*(U8 *)(AD1+35+i)=ptcom->U8_Data[i-1];		
			}	
		}	
	}

	aakj=CalcHe((U8 *)AD1,35+length*2);
	a2=aakj&0xff;
	*(U8 *)(AD1+35+length*2)=a2;				//��У��
	*(U8 *)(AD1+36+length*2)=0x16;
	
	*(U8 *)(AD1+37+length*2)=0x10;
	*(U8 *)(AD1+38+length*2)=stationAdd;
	*(U8 *)(AD1+39+length*2)=0x00;
	*(U8 *)(AD1+40+length*2)=0x5c;
	aakj=Second_Send_CalcHe((U8 *)(AD1+38+length*2),3);
	a2=aakj&0xff;
	*(U8 *)(AD1+41+length*2)=a2;				//��У��
	*(U8 *)(AD1+42+length*2)=0x16;

	//sysprintf("*(U8 *)(AD1+4)=%d\n",*(U8 *)(AD1+4));
	//sysprintf("*(U8 *)(AD1+27)=0x%x\n",*(U8 *)(AD1+27));
	//sysprintf("*(U8 *)(AD1+24)=%d\n",*(U8 *)(AD1+24));
	
	//sysprintf("*(U8 *)(AD1+28)=%d\n",*(U8 *)(AD1+28));
	//sysprintf("*(U8 *)(AD1+29)=%d\n",*(U8 *)(AD1+29));
	//sysprintf("*(U8 *)(AD1+30)=%d\n",*(U8 *)(AD1+30));
	
//������200ͨ�Ŷ�Ҫ����2�β������һ��ͨ��
	ptcom->send_length[0]=36+length*2+1;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ			
	ptcom->return_length[0]=1;				//�������ݳ���
	ptcom->return_start[0]=0;				//����������Ч��ʼ
	ptcom->return_length_available[0]=0;	//������Ч���ݳ���	

	
	ptcom->send_length[1]=6;				//���ͳ���
	ptcom->send_staradd[1]=36+length*2+1;		//�������ݴ洢��ַ			
	ptcom->return_length[1]=24;				//�������ݳ���
	ptcom->return_start[1]=0;				//����������Ч��ʼ
	ptcom->return_length_available[1]=0;	//������Ч���ݳ���	
		
	
	ptcom->send_times=2;					//���ʹ���
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
}

void Write_Time()
{
	Write_Analog();
}

void Read_Time()									//��PLC��ȡʱ��
{
	Read_Analog();
}

void Write_Recipe()								//д�䷽��PLC
{
	int datalength;
	int staradd;
	int SendTimes;
	int LastTimeWord;							//���һ�η��ͳ���
	int i,j;
	int ps;
	int b;
	int a1,a2,a3,a4,a5;
	int k1,k2,k3,k4,k5;	
	U16 aakj;
	int LE,LER;
	int stationAdd,length;
	
	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));//���ݳ���
	staradd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//���ݳ���
	staradd=staradd*2;			// ����ʱ���ֵ�ַ���أ�������Ҫ�Ŵ�2��
	//ÿ������ܷ���32��D
	if(datalength%32==0)
	{
		SendTimes=datalength/32;
		LastTimeWord=32;//���һ�η��͵ĳ���	
	}
	if(datalength%32!=0)
	{
		SendTimes=datalength/32+1;//���͵Ĵ���
		LastTimeWord=datalength%32;//���һ�η��͵ĳ���	
	}	
	
	ps=107;
	stationAdd=*(U8 *)(PE+4);	//վ��ַ	
	for (i=0;i<SendTimes;i++)
	{
		b=staradd+i*32*2;					// ��ʼ��ַ,V��С��λ���ֽ�
		if (i==SendTimes-1)				//���һ��
		{
			length=LastTimeWord;	//д��ĳ���
		}
		else
		{
			length=32;					//32����
		}
		a1=b;
		a1=a1*8;
		
		a3=a1/0x10000;
		a4=(a1-a3*0x10000)/0x100;
		a5=a1-a3*0x10000-a4*0x100;	

		LE=0x21+length*2-2;				//length����Ϊ��λ
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

		*(U8 *)(AD1+4+ps*i)=stationAdd;		//վ��ַ
		*(U8 *)(AD1+5+ps*i)=0x00;
		*(U8 *)(AD1+6+ps*i)=0x6c;			//����д��
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
		*(U8 *)(AD1+24+ps*i)=k2;			//���ȣ���
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
		
		*(U8 *)(AD1+28+ps*i)=a3;				//�Ĵ�����ַ
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
				*(U8 *)(AD1+35+j+ps*i)=*(U8 *)(PE+9+i*64+j+3);			//�ߵ��ֽڽ���
				*(U8 *)(AD1+36+j+ps*i)=*(U8 *)(PE+9+i*64+j+2);			//�ߵ��ֽڽ���
				*(U8 *)(AD1+37+j+ps*i)=*(U8 *)(PE+9+i*64+j+1);			//�ߵ��ֽڽ���
				*(U8 *)(AD1+38+j+ps*i)=*(U8 *)(PE+9+i*64+j+0);			//�ߵ��ֽڽ���
			}	
		}
		else
		{
			for (j=0;j<length*2;j++)
			{
				if (j%2==0)
				{
					*(U8 *)(AD1+35+j+ps*i)=*(U8 *)(PE+9+i*64+j+1);			//�ߵ��ֽڽ���
				}
				else
				{
					*(U8 *)(AD1+35+j+ps*i)=*(U8 *)(PE+9+i*64+j-1);	
				}	
			}
		}

		aakj=CalcHe((U8 *)AD1+ps*i,35+length*2);
		a2=aakj&0xff;
		*(U8 *)(AD1+35+length*2+ps*i)=a2;				//��У��
		*(U8 *)(AD1+36+length*2+ps*i)=0x16;
		
		ptcom->send_length[i*2]=35+length*2+2;		//���ͳ���
		ptcom->send_staradd[i*2]=i*ps;			//�������ݴ洢��ַ	
		ptcom->return_length[i*2]=1;				//�������ݳ���
		ptcom->return_start[i*2]=0;				//����������Ч��ʼ
		ptcom->return_length_available[i*2]=0;	//������Ч���ݳ���			
		
		*(U8 *)(AD1+37+length*2+ps*i)=0x10;
		*(U8 *)(AD1+38+length*2+ps*i)=stationAdd;
		*(U8 *)(AD1+39+length*2+ps*i)=0x00;
		*(U8 *)(AD1+40+length*2+ps*i)=0x5c;
		aakj=Second_Send_CalcHe((U8 *)(AD1+38+length*2+ps*i),3);
		a2=aakj&0xff;
		*(U8 *)(AD1+41+length*2+ps*i)=a2;				//��У��
		*(U8 *)(AD1+42+length*2+ps*i)=0x16;
		
		ptcom->send_length[i*2+1]=6;		//���ͳ���
		ptcom->send_staradd[i*2+1]=i*ps+35+length*2+2;			//�������ݴ洢��ַ	
		ptcom->return_length[i*2+1]=24;				//�������ݳ���
		ptcom->return_start[i*2+1]=0;				//����������Ч��ʼ
		ptcom->return_length_available[i*2+1]=0;	//������Ч���ݳ���			
		
	}
	ptcom->send_times=SendTimes*2;					//���ʹ���
	ptcom->Current_Times=0;					//��ǰ���ʹ���		
}


void compxy(void)				//����ɱ�׼�洢��ʽ
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
			if (*(U8 *)(COMad+25+i*5)==0x02)	//����ON
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
			if (*(U8 *)(COMad+25+i*3)==0x10)	//����ON
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
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/4;i++)		//16���Ʒ���
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+0);	
			*(U8 *)(COMad+i*4)=a1;							//���´�,�ӵ�0����ʼ��,VB100,VB101.....M0��M1...
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
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/2;i++)		//16���Ʒ���
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2+0);	
			*(U8 *)(COMad+i*2)=a1;							//���´�,�ӵ�0����ʼ��,VB100,VB101.....M0��M1...
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2+1);	
			*(U8 *)(COMad+i*2+1)=a2;
			//sysprintf("*(U8 *)(COMad+%d)=%d\n",i*2,*(U8 *)(COMad+i*2));
			//sysprintf("*(U8 *)(COMad+%d)=%d\n",i*2+1,*(U8 *)(COMad+i*2+1));
		}
		
	}
}


void watchcom(void)		//�������У��
{
	unsigned int aakj=0;
	aakj=remark();
	if(aakj==1)			//У������ȷ
	{
		ptcom->IfResultCorrect=1;
		compxy();		//�������ݴ������
	}
	else
	{
		ptcom->IfResultCorrect=0;
	}
}





int asicc(int a)			//תΪAsc��
{
	int bl;
	if(a<10)
		bl=a+0x30;
	if(a>9)
		bl=a-10+0x41;
	return bl;	
}

int bsicc(int a)			//AscתΪ����
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

int remark()				//�����������ݼ���У�����Ƿ���ȷ
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


U16 CalcHe(unsigned char *chData,U16 uNo)		//�����У��
{
	int i;
	int ab=0;
	for(i=4;i<uNo;i++)
	{
		ab=ab+chData[i];
	}
	return (ab);
}

U16 Second_Send_CalcHe(unsigned char *chData,U16 uNo)		//�����У��
{
	int i;
	int ab=0;
	for(i=0;i<uNo;i++)
	{
		ab=ab+chData[i];
	}
	return (ab);
}

