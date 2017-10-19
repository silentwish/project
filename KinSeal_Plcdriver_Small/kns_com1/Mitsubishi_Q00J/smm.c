#include "stdio.h"
#include "def.h"
#include "smm.h"

 struct Com_struct_D *ptcom;


void Enter_PlcDriver(void)
{
	ptcom=(struct Com_struct_D *)adcom;
	
	if (ptcom->send_staradd[99]>=256)
	{
		ptcom->send_staradd[99]=1;
	}	
	
	switch (ptcom->R_W_Flag)
	{
	case PLC_READ_DATA:				//���������Ƕ�����
	case PLC_READ_DIGITALALARM:			//���������Ƕ�����,����	
	case PLC_READ_TREND:			//���������Ƕ�����,����ͼ
	case PLC_READ_ANALOGALARM:			//���������Ƕ�����,��ȱ���	
	case PLC_READ_CONTROL:			//���������Ƕ�PLC���Ƶ�����	
		switch(ptcom->registerr)
		{
		case 'X'://X
		case 'Y'://Y
		case 'M'://M
		case 'L'://L
		case 'H'://V
		case 'K'://B
		case 'T'://T
		case 'C'://C	
			ptcom->send_staradd[98]=0;		
			Read_Bool();   //���������Ƕ�λ����       
			break;
		case 'D'://D
		case 'R'://W
		case 'N'://SW
		case 't'://T*
		case 'c'://C*
			ptcom->send_staradd[98]=0;	
			Read_Analog();  //���������Ƕ�ģ������ 
			break;		
		}
		break;
	case PLC_WRITE_DATA:				
		switch(ptcom->registerr)
		{
		case 'Y'://Y
		case 'M'://M
		case 'L'://L
		case 'H'://V
		case 'K'://B
		case 'T'://T
		case 'C'://C	
			ptcom->send_staradd[98]=0;			
			Set_Reset();      //����������ǿ����λ�͸�λ
			break;
		case 'D'://D
		case 'R'://W
		case 'N'://SW
		case 't'://T*
		case 'c'://C*
			ptcom->send_staradd[98]=0;	
			Write_Analog();	  //����������дģ������	
			break;			
		}
		break;	
	case PLC_WRITE_TIME:				//����������дʱ�䵽PLC
		switch(ptcom->registerr)
		{
		case 'D'://D
		case 'R'://W
		case 'N'://SW
		case 't'://T*
		case 'c'://C*
			ptcom->send_staradd[98]=0;	
			Write_Time();		
			break;			
		}
		break;	
	case PLC_READ_TIME:				//���������Ƕ�ȡʱ�䵽PLC
		switch(ptcom->registerr)
		{
		case 'D'://D
		case 'R'://W
		case 'N'://SW
		case 't'://T*
		case 'c'://C*
			ptcom->send_staradd[98]=0;		
			Read_Time();		
			break;		
		}
		break;
	case PLC_WRITE_RECIPE:				//����������д�䷽��PLC
		switch(*(U8 *)(PE+3))//�䷽�Ĵ�������
		{
		case 'D'://D
		case 'R'://W
		case 'N'://SW
		case 't'://T*
		case 'c'://C*
			ptcom->send_staradd[98]=0;		
			Write_Recipe();		
			break;			
		}
		break;
	case PLC_READ_RECIPE:				//���������Ǵ�PLC��ȡ�䷽
		switch(*(U8 *)(PE+3))//�䷽�Ĵ�������
		{
		case 'D'://D
		case 'R'://W
		case 'N'://SW
		case 't'://T*
		case 'c'://C*
			ptcom->send_staradd[98]=0;					
			Read_Recipe();		
			break;			
		}
		break;							
	case PLC_CHECK_DATA:				//�������������ݴ���
		watchcom();
		break;
	case 7:				//�������������ݴ���
		ptcom->send_staradd[98]=1;
		handshake();
		break;				
	}	 
}
 

void handshake()
{	
	int ps = 0;

//--��1��-----------------------------------------------------------------
//OUT    00 10 02 ff  ff fc 05 10  03
//IN     00 10 02 ff  ff fc 0f 50  02 10 03 00  10 02 01 ee  0f 4f 00 b0  cc 65 83 f9  ea 10 03
//���н���PLC���ش�����ee  0f 4f 00 b0  cc 65 83 f9  eaΪPLC���ص������ֵ
//-----------------------------------------------------------------------
    *(U8 *)(AD1 + 0 + ps) = 0x00;
    *(U8 *)(AD1 + 1 + ps) = 0x10;
    *(U8 *)(AD1 + 2 + ps) = 0x02;
    *(U8 *)(AD1 + 3 + ps) = 0xff;

    *(U8 *)(AD1 + 4 + ps) = 0xff;
    *(U8 *)(AD1 + 5 + ps) = 0xfc;
    *(U8 *)(AD1 + 6 + ps) = 0x05;
    *(U8 *)(AD1 + 7 + ps) = 0x10;

    *(U8 *)(AD1 + 8 + ps) = 0x03;

//--��2��-----------------------------------------------------------------
//OUT    10 02 00 00  fc 00 00 00  11 11 00 ff  ff 03 00 00  00 00 24 00  1c 00 0a 08  00 00 00 00  00 00 00 00
//       01 13 01 00  00 00 01 00  b3 ce 23 7c  94 cb c7 5d  97 ee 4f 42  54 df d2 2a  10 03 36 46
//IN     10 02 00 00  fc 00 00 00  11 11 00 00  e4 03 00 ff  ff 03 26 00  9c 00 0c 08  00 00 00 00  00 00 00 00
//       00 00 01 13  01 00 00 00  01 00 3a e1  9c 7b be de  7e 21 b7 ae  c6 4b ba e2  69 44 10 03  31 45
//���з��ʹ�����b3 ce 23 7c  94 cb c7 5d  97 ee 4f 42  54 df d2 2aΪ�����㷨���ɴ��룬�ڸú�������д0�����յ�PLC�����������и�ֵ�޸�
//���ش�����3a e1  9c 7b be de  7e 21 b7 ae  c6 4b ba e2  69 44ΪPLC�˶��㷨׼ȷ�ԣ����Բ������
//-----------------------------------------------------------------------
    *(U8 *)(AD1 + 9 + ps) = 0x10;
    *(U8 *)(AD1 + 10 + ps) = 0x02;
    *(U8 *)(AD1 + 11 + ps) = 0x00;
    *(U8 *)(AD1 + 12 + ps) = 0x00;

    *(U8 *)(AD1 + 13 + ps) = 0xfc;
    *(U8 *)(AD1 + 14 + ps) = 0x00;
    *(U8 *)(AD1 + 15 + ps) = 0x00;
    *(U8 *)(AD1 + 16 + ps) = 0x00;

    *(U8 *)(AD1 + 17 + ps) = 0x11;
    *(U8 *)(AD1 + 18 + ps) = 0x11;
    *(U8 *)(AD1 + 19 + ps) = 0x00;
    *(U8 *)(AD1 + 20 + ps) = 0xff;

    *(U8 *)(AD1 + 21 + ps) = 0xff;
    *(U8 *)(AD1 + 22 + ps) = 0x03;
    *(U8 *)(AD1 + 23 + ps) = 0x00;
    *(U8 *)(AD1 + 24 + ps) = 0x00;

    *(U8 *)(AD1 + 25 + ps) = 0x00;
    *(U8 *)(AD1 + 26 + ps) = 0x00;
    *(U8 *)(AD1 + 27 + ps) = 0x24;
    *(U8 *)(AD1 + 28 + ps) = 0x00;

    *(U8 *)(AD1 + 29 + ps) = 0x1c;
    *(U8 *)(AD1 + 30 + ps) = 0x00;
    *(U8 *)(AD1 + 31 + ps) = 0x0a;
    *(U8 *)(AD1 + 32 + ps) = 0x08;

    *(U8 *)(AD1 + 33 + ps) = 0x00;
    *(U8 *)(AD1 + 34 + ps) = 0x00;
    *(U8 *)(AD1 + 35 + ps) = 0x00;
    *(U8 *)(AD1 + 36 + ps) = 0x00;

    *(U8 *)(AD1 + 37 + ps) = 0x00;
    *(U8 *)(AD1 + 38 + ps) = 0x00;
    *(U8 *)(AD1 + 39 + ps) = 0x00;
    *(U8 *)(AD1 + 40 + ps) = 0x00;

    *(U8 *)(AD1 + 41 + ps) = 0x01;
    *(U8 *)(AD1 + 42 + ps) = 0x13;
    *(U8 *)(AD1 + 43 + ps) = 0x01;
    *(U8 *)(AD1 + 44 + ps) = 0x00;

    *(U8 *)(AD1 + 45 + ps) = 0x00;
    *(U8 *)(AD1 + 46 + ps) = 0x00;
    *(U8 *)(AD1 + 47 + ps) = 0x01;
    *(U8 *)(AD1 + 48 + ps) = 0x00;
//---------------------------------------------------------------------------------
    *(U8 *)(AD1 + 49 + ps) = 0x00;//�����㷨�����������д0���ڵ�һ�ֽ��յ�PLC���������и�ֵ
    *(U8 *)(AD1 + 50 + ps) = 0x00;
    *(U8 *)(AD1 + 51 + ps) = 0x00;
    *(U8 *)(AD1 + 52 + ps) = 0x00;

    *(U8 *)(AD1 + 53 + ps) = 0x00;
    *(U8 *)(AD1 + 54 + ps) = 0x00;
    *(U8 *)(AD1 + 55 + ps) = 0x00;
    *(U8 *)(AD1 + 56 + ps) = 0x00;

    *(U8 *)(AD1 + 57 + ps) = 0x00;
    *(U8 *)(AD1 + 58 + ps) = 0x00;
    *(U8 *)(AD1 + 59 + ps) = 0x00;
    *(U8 *)(AD1 + 60 + ps) = 0x00;

    *(U8 *)(AD1 + 61 + ps) = 0x00;
    *(U8 *)(AD1 + 62 + ps) = 0x00;
    *(U8 *)(AD1 + 63 + ps) = 0x00;
    *(U8 *)(AD1 + 64 + ps) = 0x00;
//---------------------------------------------------------------------------------
    *(U8 *)(AD1 + 65 + ps) = 0x10;
    *(U8 *)(AD1 + 66 + ps) = 0x03;

    *(U8 *)(AD1 + 67 + ps) = 0x00;//��У�飬���޸������ֵ֮���ٽ���У��
    *(U8 *)(AD1 + 68 + ps) = 0x00;

//--��3��-----------------------------------------------------------------
//OUT    10 02 00 00  fc 00 00 00  11 11 00 ff  ff 03 00 00  00 00 24 00  1c 08 0a 08  00 00 00 00  00 00 00 00
//       01 13 01 00  00 00 02 00  b7 c4 28 79  95 cb c7 5d  97 ee 4f 42  54 df d2 34  10 03 38 30
//IN     10 02 00 00  fc 00 00 00  11 11 00 00  e4 03 00 ff  ff 03 16 00  9c 08 0c 08  00 00 00 00  00 00 00 00
//       00 00 01 13  02 00 00 00  02 00 10 03  45 43
//���з��ʹ�����b7 c4 28 79  95 cb c7 5d  97 ee 4f 42  54 df d2 34Ϊ�����㷨���ɴ��룬�ڸú�������д0�����յ�PLC�����������и�ֵ�޸�
//-----------------------------------------------------------------------
    *(U8 *)(AD1 + 69 + ps) = 0x10;
    *(U8 *)(AD1 + 70 + ps) = 0x02;
    *(U8 *)(AD1 + 71 + ps) = 0x00;
    *(U8 *)(AD1 + 72 + ps) = 0x00;

    *(U8 *)(AD1 + 73 + ps) = 0xfc;
    *(U8 *)(AD1 + 74 + ps) = 0x00;
    *(U8 *)(AD1 + 75 + ps) = 0x00;
    *(U8 *)(AD1 + 76 + ps) = 0x00;

    *(U8 *)(AD1 + 77 + ps) = 0x11;
    *(U8 *)(AD1 + 78 + ps) = 0x11;
    *(U8 *)(AD1 + 79 + ps) = 0x00;
    *(U8 *)(AD1 + 80 + ps) = 0xff;

    *(U8 *)(AD1 + 81 + ps) = 0xff;
    *(U8 *)(AD1 + 82 + ps) = 0x03;
    *(U8 *)(AD1 + 83 + ps) = 0x00; 
    *(U8 *)(AD1 + 84 + ps) = 0x00;

    *(U8 *)(AD1 + 85 + ps) = 0x00;
    *(U8 *)(AD1 + 86 + ps) = 0x00;
    *(U8 *)(AD1 + 87 + ps) = 0x24;
    *(U8 *)(AD1 + 88 + ps) = 0x00;

    *(U8 *)(AD1 + 89 + ps) = 0x1c;
    *(U8 *)(AD1 + 90 + ps) = 0x08;
    *(U8 *)(AD1 + 91 + ps) = 0x0a;
    *(U8 *)(AD1 + 92 + ps) = 0x08;

    *(U8 *)(AD1 + 93 + ps) = 0x00;
    *(U8 *)(AD1 + 94 + ps) = 0x00;
    *(U8 *)(AD1 + 95 + ps) = 0x00;
    *(U8 *)(AD1 + 96 + ps) = 0x00;

    *(U8 *)(AD1 + 97 + ps) = 0x00;
    *(U8 *)(AD1 + 98 + ps) = 0x00;
    *(U8 *)(AD1 + 99 + ps) = 0x00;
    *(U8 *)(AD1 + 100 + ps) = 0x00;

    *(U8 *)(AD1 + 101 + ps) = 0x01;
    *(U8 *)(AD1 + 102 + ps) = 0x13;
    *(U8 *)(AD1 + 103 + ps) = 0x02;
    *(U8 *)(AD1 + 104 + ps) = 0x00;

    *(U8 *)(AD1 + 105 + ps) = 0x00;
    *(U8 *)(AD1 + 106 + ps) = 0x00;
    *(U8 *)(AD1 + 107 + ps) = 0x02;
    *(U8 *)(AD1 + 108 + ps) = 0x00;
//---------------------------------------------------------------------------------
    *(U8 *)(AD1 + 109 + ps) = 0x00;//�����㷨�����������д0���ڵ�һ�ֽ��յ�PLC���������и�ֵ
    *(U8 *)(AD1 + 110 + ps) = 0x00;
    *(U8 *)(AD1 + 111 + ps) = 0x00;
    *(U8 *)(AD1 + 112 + ps) = 0x00;

    *(U8 *)(AD1 + 113 + ps) = 0x00;
    *(U8 *)(AD1 + 114 + ps) = 0x00;
    *(U8 *)(AD1 + 115 + ps) = 0x00;
    *(U8 *)(AD1 + 116 + ps) = 0x00;

    *(U8 *)(AD1 + 117 + ps) = 0x00;
    *(U8 *)(AD1 + 118 + ps) = 0x00;
    *(U8 *)(AD1 + 119 + ps) = 0x00;
    *(U8 *)(AD1 + 120 + ps) = 0x00;

    *(U8 *)(AD1 + 121 + ps) = 0x00;
    *(U8 *)(AD1 + 122 + ps) = 0x00;
    *(U8 *)(AD1 + 123 + ps) = 0x00;
    *(U8 *)(AD1 + 124 + ps) = 0x00;
//---------------------------------------------------------------------------------
    *(U8 *)(AD1 + 125 + ps) = 0x10;
    *(U8 *)(AD1 + 126 + ps) = 0x03;

    *(U8 *)(AD1 + 127 + ps) = 0x00;//��У�飬���޸������ֵ֮���ٽ���У��
    *(U8 *)(AD1 + 128 + ps) = 0x00;
 
//////////1	
	ptcom->send_length[0]=9;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ			
	ptcom->return_length[0]=27;				//�������ݳ���
	ptcom->return_start[0]=0;				//����������Ч��ʼ
	ptcom->return_length_available[0]=0;	//������Ч���ݳ���
//////////2	
	ptcom->send_length[1]=60;				//���ͳ���
	ptcom->send_staradd[1]=9;				//�������ݴ洢��ַ			
	ptcom->return_length[1]=62;				//�������ݳ���
	ptcom->return_start[1]=0;				//����������Ч��ʼ
	ptcom->return_length_available[1]=0;	//������Ч���ݳ���
//////////3	
	ptcom->send_length[2]=60;				//���ͳ���
	ptcom->send_staradd[2]=69;				//�������ݴ洢��ַ			
	ptcom->return_length[2]=46;				//�������ݳ���
	ptcom->return_start[2]=0;				//����������Ч��ʼ
	ptcom->return_length_available[2]=0;	//������Ч���ݳ���
	
	ptcom->send_times=3;					//���ʹ���	
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	
	ptcom->send_staradd[99]=0x03;
	ptcom->send_staradd[97] = 0;
}

/**************************************************************************************************
Set_Reset��λ��λ����
	���жϿؼ�Ϊλ�ؼ�ʱ����ѡ������λ��λ����ʱ��
	�����ؼ������������˺����ռ������͵����ݡ�
**************************************************************************************************/
void Set_Reset()
{
	U16 check;          
	int check_H;		
	int check_L;		
	int startadd;		
	int startadd_1;	    
	int startadd_2;		
	int plcadd;         
	int k=0;

	startadd=ptcom->address;			// ��ʼ��λ��ַ
	plcadd=ptcom->plc_address;	    	//PLCվ��ַ
		
	*(U8 *)(AD1+0)=0x10;				//�����Ƿ��ʹ���Σ�ǰ42���̶�����      
	*(U8 *)(AD1+1)=0x02;	
	*(U8 *)(AD1+2)=0x00;
	*(U8 *)(AD1+3)=0x00;
	
	*(U8 *)(AD1+4)=0xfc;	        
	*(U8 *)(AD1+5)=0x00;
	*(U8 *)(AD1+6)=0x00;
	*(U8 *)(AD1+7)=0x00;
	
	*(U8 *)(AD1+8)=0x11;       
	*(U8 *)(AD1+9)=0x11;	
	*(U8 *)(AD1+10)=0x00;
	*(U8 *)(AD1+11)=0xff;
		
	*(U8 *)(AD1+12)=0xff;
	*(U8 *)(AD1+13)=0x03;	
	*(U8 *)(AD1+14)=0x00;        
	*(U8 *)(AD1+15)=0x00;
	
	*(U8 *)(AD1+16)=0x00;
	*(U8 *)(AD1+17)=0x00;	
	*(U8 *)(AD1+18)=0x22;
	*(U8 *)(AD1+19)=0x00;
	       
	*(U8 *)(AD1+20)=0x1c;
	*(U8 *)(AD1+21)=0x08;		
	*(U8 *)(AD1+22)=0x0a;
	*(U8 *)(AD1+23)=0x08;
	        
	*(U8 *)(AD1+24)=0x00;
	*(U8 *)(AD1+25)=0x00;	
	*(U8 *)(AD1+26)=0x00;
	*(U8 *)(AD1+27)=0x00;
	        
	*(U8 *)(AD1+28)=0x00;
	*(U8 *)(AD1+29)=0x00;	
	*(U8 *)(AD1+30)=0x00;
	*(U8 *)(AD1+31)=0x00;
	        
	*(U8 *)(AD1+32)=0x14;
	*(U8 *)(AD1+33)=0x02;
	if (ptcom->send_staradd[99] == 0x10)
	{
		*(U8 *)(AD1+34)=0x10;
		*(U8 *)(AD1+35)=0x10;
		k++;
	}
	else
	{
		*(U8 *)(AD1+34)=0x03;   
	}	
	*(U8 *)(AD1+35+k)=0x00;
	        
	*(U8 *)(AD1+36+k)=0x00;
	*(U8 *)(AD1+37+k)=0x00;
	*(U8 *)(AD1+38+k)=0x00;
	*(U8 *)(AD1+39+k)=0x01;
	
	*(U8 *)(AD1+40+k)=0x00;
	*(U8 *)(AD1+41+k)=0x00;		

	switch (ptcom->registerr)//���ݲ�ͬ�ļĴ������ƣ����Ͳ�ͬI/O����
    {
    case 'Y'://Y
        *(U8 *)(AD1+42+k) = 0x9d;
        break;
    case 'M'://M
        *(U8 *)(AD1+42+k) = 0x90;
        break;
    case 'L'://L
        *(U8 *)(AD1+42+k) = 0x92;
        break;
    case 'H'://V
        *(U8 *)(AD1+42+k) = 0x94;
        break;
    case 'K'://B
        *(U8 *)(AD1+42+k) = 0xa0;
        break;
    case 'T'://T
        *(U8 *)(AD1+42+k) = 0xc0;
        break;
    case 'C'://C
        *(U8 *)(AD1+42+k) = 0xc3;
        break;
    }
	
	*(U8 *)(AD1+43+k)=0x00;
	
	startadd_1=(startadd>>8)&0xff;
	startadd_2=(startadd)&0xff;

	if (startadd_2==0x10)
	{
		*(U8 *)(AD1+44+k)=0x10;
		*(U8 *)(AD1+45+k)=0x10;
		k++;		
	}
	else 
	{
		*(U8 *)(AD1+44+k)=startadd_2;
	}
	
	if (startadd_1==0x10)
	{
		*(U8 *)(AD1+45+k)=0x10;
		*(U8 *)(AD1+46+k)=0x10;
		k++;		
	}
	else 
	{
		*(U8 *)(AD1+45+k)=startadd_1;
	}
	
	*(U8 *)(AD1+46+k)=0x00;
	*(U8 *)(AD1+47+k)=0x00;
			
	*(U8 *)(AD1+48+k)=0x00;
	*(U8 *)(AD1+49+k)=0x00;	
	*(U8 *)(AD1+50+k)=0x00;
	*(U8 *)(AD1+51+k)=0x00;
	
	if (ptcom->writeValue==1)	    			//��λ
	{
		*(U8 *)(AD1+52+k)=0x01;		
	}
	if (ptcom->writeValue==0)	    			//��λ
	{
		*(U8 *)(AD1+52+k)=0x00;
	}
	
	*(U8 *)(AD1+53+k)=0x00;						//�̶�����

	*(U8 *)(AD1+54+k)=0x10;						//������10 03
	*(U8 *)(AD1+55+k)=0x03;	
	
	check=CalcHe((U8 *)(AD1+2),52+k)-k*0x10; 	//��ȥ��ʼ�ַ�02��������к�У��
	check_H=(check&0xf0)>>4;					//ȡУ���8λ�ĸ�λ�͵�λ
	check_H=check_H&0xf;
	check_L=check&0xf;	
	*(U8 *)(AD1+56+k)=asicc(check_H);			//����У�飬��asicc�뷢�ͣ��ȸߺ��        
	*(U8 *)(AD1+57+k)=asicc(check_L);
	 	
	ptcom->send_length[0]=58+k;					//���ͳ���
	ptcom->send_staradd[0]=0;					//�������ݴ洢��ַ		
	ptcom->return_length[0]=44;					//�������ݳ���
	ptcom->return_start[0]=0;					//����������Ч��ʼ
	ptcom->return_length_available[0]=0;		//������Ч���ݳ���	
			
	ptcom->send_times=1;						//���ʹ���
	ptcom->Current_Times=0;						//��ǰ���ʹ���
	ptcom->send_staradd[97] = 0;				//���ز������־
	
	ptcom->send_staradd[99]++;			
}

/**************************************************************************************************
Read_Bool��λ����
	���жϿؼ�Ϊλ�ؼ�ʱ����ѡ���м��ӹ���ʱ��
	���������˺����ռ������͵����ݡ�
**************************************************************************************************/
void Read_Bool()	
{
	U16 check;          
	int check_H;		
	int check_L;		
	int startadd;		
	int startadd_1;	    
	int startadd_2;		
	int plcadd;         
	int length;			
	int k=0;
	int nSendlen_L;
	int nSendlen_H;

	plcadd=ptcom->plc_address;					//PLCվ��ַ	
	length=ptcom->register_length;				//��ȡ�ĳ���
	startadd=ptcom->address;					//��ʼ��ַ
	
	*(U8 *)(AD1+0)=0x10;						//�����Ƿ��ʹ���Σ�ǰ38���̶�����      
	*(U8 *)(AD1+1)=0x02;
	
	*(U8 *)(AD1+2)=0x00;
	*(U8 *)(AD1+3)=0x00;
	*(U8 *)(AD1+4)=0xfc;
	        
	*(U8 *)(AD1+5)=0x00;
	*(U8 *)(AD1+6)=0x00;
	*(U8 *)(AD1+7)=0x00;
	*(U8 *)(AD1+8)=0x11;       
	*(U8 *)(AD1+9)=0x11;
	
	*(U8 *)(AD1+10)=0x00;
	*(U8 *)(AD1+11)=0xff;	
	*(U8 *)(AD1+12)=0xff;
	*(U8 *)(AD1+13)=0x03;
	
	*(U8 *)(AD1+14)=0x00;        
	*(U8 *)(AD1+15)=0x00;
	*(U8 *)(AD1+16)=0x00;
	*(U8 *)(AD1+17)=0x00;
	
	*(U8 *)(AD1+18)=0x1a;
	*(U8 *)(AD1+19)=0x00;        
	*(U8 *)(AD1+20)=0x1c;
	*(U8 *)(AD1+21)=0x08;	
	
	*(U8 *)(AD1+22)=0x0a;
	*(U8 *)(AD1+23)=0x08;        
	*(U8 *)(AD1+24)=0x00;
	*(U8 *)(AD1+25)=0x00;
	
	*(U8 *)(AD1+26)=0x00;
	*(U8 *)(AD1+27)=0x00;        
	*(U8 *)(AD1+28)=0x00;
	*(U8 *)(AD1+29)=0x00;
	
	*(U8 *)(AD1+30)=0x00;
	*(U8 *)(AD1+31)=0x00;        
	*(U8 *)(AD1+32)=0x04;
	*(U8 *)(AD1+33)=0x01;
	
	if (ptcom->send_staradd[99] == 0x10)
	{
		*(U8 *)(AD1+34)=0x10;
		*(U8 *)(AD1+35)=0x10;
		k++;
	}
	else
	{
		*(U8 *)(AD1+34)=0x03; 
	}
	*(U8 *)(AD1+35+k)=0x00;        
	*(U8 *)(AD1+36+k)=0x00;
	*(U8 *)(AD1+37+k)=0x00;		
		
	switch (ptcom->registerr)//���ݲ�ͬ�ļĴ������ƣ����Ͳ�ͬI/O����
    {
    case 'X'://X
        *(U8 *)(AD1+38+k) = 0x9c;
        break;
    case 'Y'://Y
        *(U8 *)(AD1+38+k) = 0x9d;
        break;
    case 'M'://M
        *(U8 *)(AD1+38+k) = 0x90;
        break;
    case 'L'://L
        *(U8 *)(AD1+38+k) = 0x92;
        break;
    case 'H'://V
        *(U8 *)(AD1+38+k) = 0x94;
        break;
    case 'K'://B
        *(U8 *)(AD1+38+k) = 0xa0;
        break;
    case 'T'://T
        *(U8 *)(AD1+38+k) = 0xc0;
        break;
    case 'C'://C
        *(U8 *)(AD1+38+k) = 0xc3;
        break;
    }	
	
	*(U8 *)(AD1+39+k)=0x00;
	
	startadd=startadd/16*16;
	startadd_1=(startadd>>8)&0xff;
	startadd_2=	startadd & 0xff;
	
	if (startadd_2==0x10)
	{
		*(U8 *)(AD1+40+k)=0x10;
		*(U8 *)(AD1+41+k)=0x10;
		k++;		
	}
	else 
	{
		*(U8 *)(AD1+40+k)=startadd_2;
	}
	
	if (startadd_1==0x10)
	{
		*(U8 *)(AD1+41+k)=0x10;
		*(U8 *)(AD1+42+k)=0x10;
		k++;		
	}
	else 
	{
		*(U8 *)(AD1+41+k)=startadd_1;
	}	
	
	*(U8 *)(AD1+42+k)=0x00;
	*(U8 *)(AD1+43+k)=0x00;		
	
	length = (length/2) + 1 ;
    nSendlen_L = (length*16) & 0xff;
    nSendlen_H = ((length*16) >> 8) & 0xff;
    if (nSendlen_L == 0x10)
    {
		*(U8 *)(AD1+44+k)=0x10;
		*(U8 *)(AD1+45+k)=0x10;
        k++;
    }
    else *(U8 *)(AD1+44+k) = nSendlen_L;

    if (nSendlen_H == 0x10)
    {
		*(U8 *)(AD1+45+k)=0x10;
		*(U8 *)(AD1+46+k)=0x10;
        k++;
    }
    else *(U8 *)(AD1+45+k) = nSendlen_H;

	*(U8 *)(AD1+46+k)=0x10;						//������10 03
	*(U8 *)(AD1+47+k)=0x03;
	
	check=CalcHe((U8 *)(AD1+2),44+k)-k*0x10; 	//��ȥ��ʼ�ַ�02��������к�У��
	check_H=(check&0xf0)>>4;					//ȡУ���8λ�ĸ�λ�͵�λ
	check_H=check_H&0xf;
	check_L=check&0xf;	
	*(U8 *)(AD1+48+k)=asicc(check_H);			//����У�飬��asicc�뷢�ͣ��ȸߺ��        
	*(U8 *)(AD1+49+k)=asicc(check_L);

	
	ptcom->send_length[0]=50+k;					//���ͳ���
	ptcom->send_staradd[0]=0;					//�������ݴ洢��ַ		
	ptcom->return_length[0]=44 + length * 2;	//�������ݳ��ȣ���������0x10ʱ�����ص����ݳ���Ҫ+1
	ptcom->return_start[0]=40;					//����������Ч��ʼ
	ptcom->return_length_available[0]=length*2;	//������Ч���ݳ���
		
	ptcom->register_length=length*2;			//��ȡ�ĳ���,�Ѿ��ı�
			
	ptcom->send_times=1;						//���ʹ���
	ptcom->Current_Times=0;						//��ǰ���ʹ���	
	ptcom->send_add[0]=startadd;				//�����������ַ������
	ptcom->address = ptcom->send_add[0];
	ptcom->send_staradd[97] = 1;				//λ���ر�־	
	
	ptcom->send_staradd[99]++;	
}

/**************************************************************************************************
Read_Analog���ֺ���
	���жϿؼ�Ϊ�ְ�ť�ؼ�ʱ����ѡ���м��ӹ���ʱ��
	���������˺����ռ������͵����ݡ�
**************************************************************************************************/
void Read_Analog()		
{
	U16 check;          
	int check_H;		
	int check_L;		
	int startadd;		
	int startadd_1;	   
	int startadd_2;		
	int plcadd;       
	int length;			
	int k=0;

	plcadd=ptcom->plc_address;					//PLCվ��ַ	
	length=ptcom->register_length;				//��ȡ�ĳ���
	startadd=ptcom->address;					//��ʼ��ַ  	                                        
	
	*(U8 *)(AD1+0)=0x10;						//�����Ƿ��ʹ���Σ�ǰ38���̶�����      
	*(U8 *)(AD1+1)=0x02;	
	*(U8 *)(AD1+2)=0x00;
	*(U8 *)(AD1+3)=0x00;
	
	*(U8 *)(AD1+4)=0xfc;	        
	*(U8 *)(AD1+5)=0x00;
	*(U8 *)(AD1+6)=0x00;
	*(U8 *)(AD1+7)=0x00;
		
	*(U8 *)(AD1+8)=0x11;       
	*(U8 *)(AD1+9)=0x11;	
	*(U8 *)(AD1+10)=0x00;
	*(U8 *)(AD1+11)=0xff;
		
	*(U8 *)(AD1+12)=0xff;
	*(U8 *)(AD1+13)=0x03;	
	*(U8 *)(AD1+14)=0x00;        
	*(U8 *)(AD1+15)=0x00;
	
	*(U8 *)(AD1+16)=0x00;
	*(U8 *)(AD1+17)=0x00;	
	*(U8 *)(AD1+18)=0x1a;
	*(U8 *)(AD1+19)=0x00;
	        
	*(U8 *)(AD1+20)=0x1c;
	*(U8 *)(AD1+21)=0x08;		
	*(U8 *)(AD1+22)=0x0a;
	*(U8 *)(AD1+23)=0x08;
	        
	*(U8 *)(AD1+24)=0x00;
	*(U8 *)(AD1+25)=0x00;	
	*(U8 *)(AD1+26)=0x00;
	*(U8 *)(AD1+27)=0x00;
	        
	*(U8 *)(AD1+28)=0x00;
	*(U8 *)(AD1+29)=0x00;	
	*(U8 *)(AD1+30)=0x00;
	*(U8 *)(AD1+31)=0x00;
	        
	*(U8 *)(AD1+32)=0x04;
	*(U8 *)(AD1+33)=0x01;	
	if (ptcom->send_staradd[99] == 0x10)
	{
		*(U8 *)(AD1+34)=0x10;
		*(U8 *)(AD1+35)=0x10;
		k++;
	}
	else
	{
		*(U8 *)(AD1+34)=0x03; 
	}
	*(U8 *)(AD1+35+k)=0x00;        
	*(U8 *)(AD1+36+k)=0x00;
	*(U8 *)(AD1+37+k)=0x00;
		
	switch (ptcom->registerr)//���ݲ�ͬ�ļĴ������ƣ����Ͳ�ͬI/O����
    {
    case 'D'://D
        *(U8 *)(AD1+38+k) = 0xa8;
        break;
    case 'R'://W
        *(U8 *)(AD1+38+k) = 0xb4;
        break;
    case 'N'://SW
        *(U8 *)(AD1+38+k) = 0xb5;
        break;
    case 't'://T*
        *(U8 *)(AD1+38+k) = 0xc2;
        break;
    case 'c'://C*
        *(U8 *)(AD1+38+k) = 0xc5;
        break;
    }	
	
	*(U8 *)(AD1+39+k)=0x00;
	
	startadd_1=(startadd>>8)&0xff;
	startadd_2=	(startadd)&0xff;
	
	if (startadd_2==0x10)
	{
		*(U8 *)(AD1+40+k)=0x10;
		*(U8 *)(AD1+41+k)=0x10;
		k++;		
	}
	else 
	{
		*(U8 *)(AD1+40+k)=startadd_2;
	}
	
	if (startadd_1==0x10)
	{
		*(U8 *)(AD1+41+k)=0x10;
		*(U8 *)(AD1+42+k)=0x10;
		k++;		
	}
	else 
	{
		*(U8 *)(AD1+41+k)=startadd_1;
	}	
	
	*(U8 *)(AD1+42+k)=0x00;
	*(U8 *)(AD1+43+k)=0x00;	

	if (length==0x10)
	{
		*(U8 *)(AD1+44+k)=0x10;
		*(U8 *)(AD1+45+k)=0x10;
		k++;			
	}
	else
	{
		*(U8 *)(AD1+44+k)=length&0xff;	
	}
	
	*(U8 *)(AD1+45+k)=0x00;							//�̶�����

	*(U8 *)(AD1+46+k)=0x10;							//������10 03
	*(U8 *)(AD1+47+k)=0x03;
	
	check=CalcHe((U8 *)(AD1+2),44+k)-k*0x10; 		//��ȥ��ʼ�ַ�02��������к�У��
	check_H=(check&0xf0)>>4;						//ȡУ���8λ�ĸ�λ�͵�λ
	check_H=check_H&0xf;
	check_L=check&0xf;	
	*(U8 *)(AD1+48+k)=asicc(check_H);				//����У�飬��asicc�뷢�ͣ��ȸߺ��        
	*(U8 *)(AD1+49+k)=asicc(check_L);

	
	ptcom->send_length[0]=50+k;						//���ͳ���
	ptcom->send_staradd[0]=0;						//�������ݴ洢��ַ		
	ptcom->return_length[0]=46;						//�������ݳ���
	if (ptcom->send_staradd[99] == 0x10)
	{
		ptcom->return_start[0]=41;					//����������Ч��ʼ
	}
	else
	{
		ptcom->return_start[0]=40;					//����������Ч��ʼ 
	}	
	ptcom->return_length_available[0]=length*2;		//������Ч���ݳ���	
		
	ptcom->send_times=1;							//���ʹ���
	ptcom->Current_Times=0;							//��ǰ���ʹ���	
	ptcom->send_add[0]=ptcom->address;				//�����������ַ������	

	ptcom->send_staradd[97] = 2;							//�ַ��ر�־
	ptcom->send_staradd[99]++;
}

/**************************************************************************************************
Write_Analogд�ֺ���
	���жϿؼ�Ϊ�ְ�ť�ؼ�ʱ�������������򿪼��̣�
	������ֵ�󣬳��������˺����ռ������͵����ݡ�
**************************************************************************************************/
void Write_Analog()		
{
	U16 check;          
	int check_H;		
	int check_L;		
	int startadd;		
	int startadd_1;	   
	int startadd_2;		
	int plcadd;      	
	int length;			
	int i;				
	int data_H,data_L;  
	int k;

	plcadd=ptcom->plc_address;						//PLCվ��ַ	
	length=ptcom->register_length;					//��ȡ�ĳ���
	startadd=ptcom->address;						//��ʼ��ַ

	*(U8 *)(AD1+0)=0x10;							//�����Ƿ��ʹ���Σ�ǰ38���̶�����      
	*(U8 *)(AD1+1)=0x02;	
	*(U8 *)(AD1+2)=0x00;
	*(U8 *)(AD1+3)=0x00;
	
	*(U8 *)(AD1+4)=0xfc;	        
	*(U8 *)(AD1+5)=0x00;
	*(U8 *)(AD1+6)=0x00;
	*(U8 *)(AD1+7)=0x00;
		
	*(U8 *)(AD1+8)=0x11;       
	*(U8 *)(AD1+9)=0x11;	
	*(U8 *)(AD1+10)=0x00;
	*(U8 *)(AD1+11)=0xff;
		
	*(U8 *)(AD1+12)=0xff;
	*(U8 *)(AD1+13)=0x03;	
	*(U8 *)(AD1+14)=0x00;        
	*(U8 *)(AD1+15)=0x00;
	
	*(U8 *)(AD1+16)=0x00;
	*(U8 *)(AD1+17)=0x00;	
	*(U8 *)(AD1+18)=0x1a + (length * 2);			//��24��ʼ�����������ݳ���				
	*(U8 *)(AD1+19)=0x00;
	        
	*(U8 *)(AD1+20)=0x1c;
	*(U8 *)(AD1+21)=0x08;		
	*(U8 *)(AD1+22)=0x0a;
	*(U8 *)(AD1+23)=0x08;
	        
	*(U8 *)(AD1+24)=0x00;
	*(U8 *)(AD1+25)=0x00;	
	*(U8 *)(AD1+26)=0x00;
	*(U8 *)(AD1+27)=0x00;
	        
	*(U8 *)(AD1+28)=0x00;
	*(U8 *)(AD1+29)=0x00;	
	*(U8 *)(AD1+30)=0x00;
	*(U8 *)(AD1+31)=0x00;
	        
	*(U8 *)(AD1+32)=0x14;
	*(U8 *)(AD1+33)=0x01;	
	if (ptcom->send_staradd[99] == 0x10)
	{
		*(U8 *)(AD1+34)=0x10;
		*(U8 *)(AD1+35)=0x10;
		k++;
	}
	else
	{
		*(U8 *)(AD1+34)=0x03; 
	}
	*(U8 *)(AD1+35+k)=0x00; 
	        
	*(U8 *)(AD1+36+k)=0x00;
	*(U8 *)(AD1+37+k)=0x00;

	switch (ptcom->registerr)//���ݲ�ͬ�ļĴ������ƣ����Ͳ�ͬI/O����
    {
    case 'D'://D
        *(U8 *)(AD1+38+k) = 0xa8;
        break;
    case 'R'://W
        *(U8 *)(AD1+38+k) = 0xb4;
        break;
    case 'N'://SW
        *(U8 *)(AD1+38+k) = 0xb5;
        break;
    case 't'://T*
        *(U8 *)(AD1+38+k) = 0xc2;
        break;
    case 'c'://C*
        *(U8 *)(AD1+38+k) = 0xc5;
        break;
    }		
	
	*(U8 *)(AD1+39+k)=0x00;
		
	startadd_1=(startadd>>8)&0xff;
	startadd_2=	(startadd)&0xff;	
	
	if (startadd_2==0x10)
	{
		*(U8 *)(AD1+40+k)=0x10;
		*(U8 *)(AD1+41+k)=0x10;
		k++;		
	}
	else 
	{
		*(U8 *)(AD1+40)=startadd_2;
	}
	
	if (startadd_1==0x10)
	{
		*(U8 *)(AD1+41+k)=0x10;
		*(U8 *)(AD1+42+k)=0x10;
		k++;		
	}
	else 
	{
		*(U8 *)(AD1+41+k)=startadd_1;
	}	
	
	*(U8 *)(AD1+42+k)=0x00;
	*(U8 *)(AD1+43+k)=0x00;
	
	if (length==0x10)
	{
		*(U8 *)(AD1+44+k)=0x10;
		*(U8 *)(AD1+45+k)=0x10;
		k++;			
	}
	else
	{
		*(U8 *)(AD1+44+k)=length&0xff;	
	}

	*(U8 *)(AD1+45+k)=0x00;							//�̶�����

	for (i=0;i<length;i++)							//ѭ��д������ֵ
	{				
		data_L=ptcom->U8_Data[i*2];                 //��������ֵ��D[]������ȡ���ݣ���Ӧ��data_HΪ��λ��data_LΪ��λ
		data_H=ptcom->U8_Data[i*2+1];			

		if (data_L==0x10)
		{
			*(U8 *)(AD1+46+k+i*2)=0x10;
			*(U8 *)(AD1+47+k+i*2)=0x10;
			k++;
		}
		else
		{
			*(U8 *)(AD1+46+k+i*2)=data_L;
		}
		
		if (data_H==0x10)
		{
			*(U8 *)(AD1+47+k+i*2)=0x10;
			*(U8 *)(AD1+48+k+i*2)=0x10;
			k++;
		}
		else
		{
			*(U8 *)(AD1+47+k+i*2)=data_H;
		}
	}

	*(U8 *)(AD1+46+k+length*2)=0x10;				//������10 03
	*(U8 *)(AD1+47+k+length*2)=0x03;
	
	*(U8 *)(AD1+18) = *(U8 *)(AD1+18) + k;			//���ǵ���0x10���ڣ����Զ෢�͵ĳ���������Ҫ����
		
	check=CalcHe((U8 *)(AD1+2),44+k+length*2)-k*0x10;//��ȥ��ʼ�ַ�02��������к�У��
	check_H=(check&0xf0)>>4;						//ȡУ���8λ�ĸ�λ�͵�λ
	check_H=check_H&0xf;
	check_L=check&0xf;	
	*(U8 *)(AD1+48+k+length*2)=asicc(check_H);		//����У�飬��asicc�뷢�ͣ��ȸߺ��        
	*(U8 *)(AD1+49+k+length*2)=asicc(check_L);	
	
	ptcom->send_length[0]=50+k+length*2;			//���ͳ���
	ptcom->send_staradd[0]=0;						//�������ݴ洢��ַ		
	ptcom->return_length[0]=44;						//�������ݳ���
	ptcom->return_start[0]=0;						//����������Ч��ʼ
	ptcom->return_length_available[0]=0;			//������Ч���ݳ���	
	
	ptcom->send_times=1;							//���ʹ���
	ptcom->Current_Times=0;							//��ǰ���ʹ���
	ptcom->send_staradd[97] = 0;							//���ز������־
	
	ptcom->send_staradd[99]++;		
}

/**************************************************************************************************
Read_Recipe��ȡ�䷽����
	����̬�������䷽���Ҵ�����PLC�ж�ȡ�䷽��ťʱ��
	���������˺����ռ������͵����ݡ�
**************************************************************************************************/
void Read_Recipe()		
{
	U16 check;          
	int check_H;		
	int check_L;		
	int startadd;		
	int startadd_1;	    
	int startadd_2;		
	int plcadd;         	
	int length;			
	int ps=0;		
	int SendTimes;      
	int LastTimeWord;	
	int datalength;		
	int i,k;				
	int p_startadd;		
	
	plcadd=ptcom->plc_address;						//PLCվ��ַ	
	datalength=ptcom->register_length;				//��ȡ�ĳ���
	startadd=ptcom->address;			    		//��ʼ��ַ
	
	if(datalength>5000)                     		//���Ƴ���
		datalength=5000;

	if(datalength%64==0)                    		//�涨ÿ������ܷ�64��D�����ݸպ���64�ı���ʱ
	{
		SendTimes=datalength/64;            		//���ʹ���
		LastTimeWord=64;                    		//���һ�η��͵ĳ���Ϊ64	
	}
	if(datalength%64!=0)                    		//���ݲ���64�ı���ʱ 
	{
		SendTimes=datalength/64+1;          		//���͵Ĵ���
		LastTimeWord=datalength%64;         		//���һ�η��͵ĳ���Ϊ��64������	
	}

	*(U8 *)(AD1+300) = SendTimes + 67;

	ps = 0;
	for (i=0;i<SendTimes;i++)
	{	
		p_startadd=startadd+i*64;					//ÿ�ο�ʼ�ĵ�ַ����64����
		
		*(U8 *)(AD1+0+ps)=0x10;						//�����Ƿ��ʹ���Σ�ǰ38���̶�����      
		*(U8 *)(AD1+1+ps)=0x02;	
		*(U8 *)(AD1+2+ps)=0x00;
		*(U8 *)(AD1+3+ps)=0x00;
		
		*(U8 *)(AD1+4+ps)=0xfc;	        
		*(U8 *)(AD1+5+ps)=0x00;
		*(U8 *)(AD1+6+ps)=0x00;
		*(U8 *)(AD1+7+ps)=0x00;
			
		*(U8 *)(AD1+8+ps)=0x11;       
		*(U8 *)(AD1+9+ps)=0x11;	
		*(U8 *)(AD1+10+ps)=0x00;
		*(U8 *)(AD1+11+ps)=0xff;
			
		*(U8 *)(AD1+12+ps)=0xff;
		*(U8 *)(AD1+13+ps)=0x03;	
		*(U8 *)(AD1+14+ps)=0x00;        
		*(U8 *)(AD1+15+ps)=0x00;
		
		*(U8 *)(AD1+16+ps)=0x00;
		*(U8 *)(AD1+17+ps)=0x00;	
		*(U8 *)(AD1+18+ps)=0x1a;
		*(U8 *)(AD1+19+ps)=0x00;
		        
		*(U8 *)(AD1+20+ps)=0x1c;
		*(U8 *)(AD1+21+ps)=0x08;		
		*(U8 *)(AD1+22+ps)=0x0a;
		*(U8 *)(AD1+23+ps)=0x08;
		        
		*(U8 *)(AD1+24+ps)=0x00;
		*(U8 *)(AD1+25+ps)=0x00;	
		*(U8 *)(AD1+26+ps)=0x00;
		*(U8 *)(AD1+27+ps)=0x00;
		        
		*(U8 *)(AD1+28+ps)=0x00;
		*(U8 *)(AD1+29+ps)=0x00;	
		*(U8 *)(AD1+30+ps)=0x00;
		*(U8 *)(AD1+31+ps)=0x00;
		        
		*(U8 *)(AD1+32+ps)=0x04;
		*(U8 *)(AD1+33+ps)=0x01;	
		if (ptcom->send_staradd[99] == 0x10)
		{
			*(U8 *)(AD1+34+ps)=0x10;
			*(U8 *)(AD1+35+ps)=0x10;
			k++;
		}
		else
		{
			*(U8 *)(AD1+34+ps)=0x03; 
		}
		*(U8 *)(AD1+35+k+ps)=0x00; 
		        
		*(U8 *)(AD1+36+k+ps)=0x00;
		*(U8 *)(AD1+37+k+ps)=0x00;
		
		switch (ptcom->registerr)//���ݲ�ͬ�ļĴ������ƣ����Ͳ�ͬI/O����
	    {
	    case 'D'://D
	        *(U8 *)(AD1+38+k+ps) = 0xa8;
	        break;
	    case 'R'://W
	        *(U8 *)(AD1+38+k+ps) = 0xb4;
	        break;
	    case 'N'://SW
	        *(U8 *)(AD1+38+k+ps) = 0xb5;
	        break;
	    case 't'://T*
	        *(U8 *)(AD1+38+k+ps) = 0xc2;
	        break;
	    case 'c'://C*
	        *(U8 *)(AD1+38+k+ps) = 0xc5;
	        break;
	    }		
		
		*(U8 *)(AD1+39+k+ps)=0x00;		
			
		startadd_1=(p_startadd>>8)&0xff;
		startadd_2=	(p_startadd)&0xff;
		
		if (startadd_2==0x10)
		{
			*(U8 *)(AD1+40+k+ps)=0x10;
			*(U8 *)(AD1+41+k+ps)=0x10;
			k++;		
		}
		else 
		{
			*(U8 *)(AD1+40+k+ps)=startadd_2;
		}
		
		if (startadd_1==0x10)
		{
			*(U8 *)(AD1+41+k+ps)=0x10;
			*(U8 *)(AD1+42+k+ps)=0x10;
			k++;		
		}
		else 
		{
			*(U8 *)(AD1+41+k+ps)=startadd_1;
		}	
		
		*(U8 *)(AD1+42+k+ps)=0x00;
		*(U8 *)(AD1+43+k+ps)=0x00;			
		
		if (i!=((SendTimes/2)-1))   				//�������һ�η���ʱ
		{
			length=64;              				//ÿ�η�����32����
		}
		if (i==((SendTimes/2)-1))   				//������η���ʱ��
		{
			length=LastTimeWord;    				//���ĳ�������ʣ�µ���
		}		
		
		if (length==0x10)
		{
			*(U8 *)(AD1+44+k+ps)=0x10;
			*(U8 *)(AD1+45+k+ps)=0x10;
			k++;			
		}
		else
		{
			*(U8 *)(AD1+44+k+ps)=length&0xff;	
		}
		
		*(U8 *)(AD1+45+k+ps)=0x00;					//�̶�����

		*(U8 *)(AD1+46+k+ps)=0x10;					//������10 03
		*(U8 *)(AD1+47+k+ps)=0x03;
		
		check=CalcHe((U8 *)(AD1+2+ps),44+k)-k*0x10; //��ȥ��ʼ�ַ�02��������к�У��
		check_H=(check&0xf0)>>4;					//ȡУ���8λ�ĸ�λ�͵�λ
		check_H=check_H&0xf;
		check_L=check&0xf;	
		*(U8 *)(AD1+48+k+ps)=asicc(check_H);		//����У�飬��asicc�뷢�ͣ��ȸߺ��        
		*(U8 *)(AD1+49+k+ps)=asicc(check_L);		
			
		ptcom->send_length[0+i]=50+k;				//���ͳ���
		ptcom->send_staradd[0+i]=ps;				//�������ݴ洢��ַ		
		ptcom->return_length[0+i]=46;				//�������ݳ���
		ptcom->return_start[0+i]=40;				//����������Ч��ʼ
		ptcom->return_length_available[0+i]=length*2;//������Ч���ݳ���	
		ptcom->send_add[0+i]=ptcom->address+i*64;	//�����������ַ������	
		
		ps=ps+50+k;									//�ۼƷ��͵ĳ���
		
		ptcom->send_staradd[99]++;	
		if (ptcom->send_staradd[99] >= 256)
		{
			ptcom->send_staradd[99] = 1;
		}	
	}			
	ptcom->send_times=SendTimes;					//���ʹ���
	ptcom->Current_Times=0;							//��ǰ���ʹ���		

	ptcom->send_staradd[97] = 2;							//�ַ��ر�־	
}

/**************************************************************************************************
Write_Recipeд�䷽����
	����̬�������䷽���Ҵ���д�䷽��PLC��ťʱ��
	���������˺����ռ������͵����ݡ�
**************************************************************************************************/
void Write_Recipe()		
{
	U16 check;         
	int check_H;		
	int check_L;		
	int startadd;		
	int startadd_1;	    
	int startadd_2;		
	int p_startadd;		
	int plcadd;         	
	int length;			
	int data_H,data_L;  
	int datalength;		
	int SendTimes;      
	int LastTimeWord;	
	int ps=0;		
	int i;				
	int j,k;
	int regname;

	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));											//���ݳ���
	startadd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));	//���ݿ�ʼ��ַ
	plcadd=*(U8 *)(PE+4);	
	
	switch (*(U8 *)(PE+3))//���ݲ�ͬ�ļĴ������ƣ����Ͳ�ͬI/O����
    {
    case 'D'://D
        regname = 0xa8;
        break;
    case 'R'://W
        regname = 0xb4;
        break;
    case 'N'://SW
        regname = 0xb5;
        break;
    case 't'://T*
        regname = 0xc2;
        break;
    case 'c'://C*
        regname = 0xc5;
        break;
    }	

	if(datalength>5000)                     	//���Ƴ���
		datalength=5000;

	if(datalength%64==0)                   	 	//�涨ÿ������ܷ�64��D�����ݸպ���64�ı���ʱ
	{
		SendTimes=datalength/64;            	//���ʹ���
		LastTimeWord=64;                    	//���һ�η��͵ĳ���Ϊ64	
	}
	if(datalength%64!=0)                    	//���ݲ���64�ı���ʱ 
	{
		SendTimes=datalength/64+1;          	//���͵Ĵ���
		LastTimeWord=datalength%64;         	//���һ�η��͵ĳ���Ϊ��64������	
	}
	
	ps=0;										//ǰ�淢�͵ĳ���
	
	for (i=0;i<SendTimes;i++)
	{
		k = 0;									//��λk
		*(U8 *)(AD1+0+ps)=0x10;					//�����Ƿ��ʹ���Σ�ǰ38���̶�����      
		*(U8 *)(AD1+1+ps)=0x02;	
		*(U8 *)(AD1+2+ps)=0x00;
		*(U8 *)(AD1+3+ps)=0x00;
		
		*(U8 *)(AD1+4+ps)=0xfc;	        
		*(U8 *)(AD1+5+ps)=0x00;
		*(U8 *)(AD1+6+ps)=0x00;
		*(U8 *)(AD1+7+ps)=0x00;
			
		*(U8 *)(AD1+8+ps)=0x11;       
		*(U8 *)(AD1+9+ps)=0x11;	
		*(U8 *)(AD1+10+ps)=0x00;
		*(U8 *)(AD1+11+ps)=0xff;
			
		*(U8 *)(AD1+12+ps)=0xff;
		*(U8 *)(AD1+13+ps)=0x03;	
		*(U8 *)(AD1+14+ps)=0x00;        
		*(U8 *)(AD1+15+ps)=0x00;
		
		*(U8 *)(AD1+16+ps)=0x00;
		*(U8 *)(AD1+17+ps)=0x00;	
		*(U8 *)(AD1+18+ps)=0x1a;				//Ԥ���趨���淢�ͳ���Ϊ��ʼֵ
		*(U8 *)(AD1+19+ps)=0x00;
		        
		*(U8 *)(AD1+20+ps)=0x1c;
		*(U8 *)(AD1+21+ps)=0x08;		
		*(U8 *)(AD1+22+ps)=0x0a;
		*(U8 *)(AD1+23+ps)=0x08;
		        
		*(U8 *)(AD1+24+ps)=0x00;
		*(U8 *)(AD1+25+ps)=0x00;	
		*(U8 *)(AD1+26+ps)=0x00;
		*(U8 *)(AD1+27+ps)=0x00;
		        
		*(U8 *)(AD1+28+ps)=0x00;
		*(U8 *)(AD1+29+ps)=0x00;	
		*(U8 *)(AD1+30+ps)=0x00;
		*(U8 *)(AD1+31+ps)=0x00;
		        
		*(U8 *)(AD1+32+ps)=0x14;
		*(U8 *)(AD1+33+ps)=0x01;	
		if (ptcom->send_staradd[99] == 0x10)
		{
			*(U8 *)(AD1+34+ps)=0x10;
			*(U8 *)(AD1+35+ps)=0x10;
			k++;
		}
		else
		{
			*(U8 *)(AD1+34+ps)=0x03; 
		}
		*(U8 *)(AD1+35+k+ps)=0x00; 
		        
		*(U8 *)(AD1+36+k+ps)=0x00;
		*(U8 *)(AD1+37+k+ps)=0x00;
		

		*(U8 *)(AD1+38+k+ps)=regname;
		
		*(U8 *)(AD1+39+k+ps)=0x00;				//fix
				
		p_startadd=startadd+i*64;
		
		startadd_1=(p_startadd>>8)&0xff;
		startadd_2=	(p_startadd)&0xff;	
		
		if (startadd_2==0x10)
		{
			*(U8 *)(AD1+40+k+ps)=0x10;
			*(U8 *)(AD1+41+k+ps)=0x10;
			k++;		
		}
		else 
		{
			*(U8 *)(AD1+40+k+ps)=startadd_2;
		}
		
		if (startadd_1==0x10)
		{
			*(U8 *)(AD1+41+k+ps)=0x10;
			*(U8 *)(AD1+42+k+ps)=0x10;
			k++;		
		}
		else 
		{
			*(U8 *)(AD1+41+k+ps)=startadd_1;
		}	
		
		*(U8 *)(AD1+42+k+ps)=0x00;
		*(U8 *)(AD1+43+k+ps)=0x00;
		
		if (i==SendTimes-1)						//���һ��
		{
			length=LastTimeWord;
		}
		else
		{
			length=64;
		}			
		
		if (length==0x10)
		{
			*(U8 *)(AD1+44+k+ps)=0x10;
			*(U8 *)(AD1+45+k+ps)=0x10;
			k++;			
		}
		else
		{
			*(U8 *)(AD1+44+k+ps)=length&0xff;	
		}

		*(U8 *)(AD1+45+k+ps)=0x00;				//�̶�����	
		
		for (j=0;j<length;j++)					//ѭ��д������ֵ
		{				
			data_L=*(U8 *)(PE+9+j*2+64*i*2);    //��������ֵ��D[]������ȡ���ݣ���Ӧ��data_HΪ��λ��data_LΪ��λ
			data_H=*(U8 *)(PE+10+j*2+64*i*2);			

			if (data_L==0x10)
			{
				*(U8 *)(AD1+46+k+j*2+ps)=0x10;
				*(U8 *)(AD1+47+k+j*2+ps)=0x10;
				k++;
			}
			else
			{
				*(U8 *)(AD1+46+k+j*2+ps)=data_L;
			}
			
			if (data_H==0x10)
			{
				*(U8 *)(AD1+47+k+j*2+ps)=0x10;
				*(U8 *)(AD1+48+k+j*2+ps)=0x10;
				k++;
			}
			else
			{
				*(U8 *)(AD1+47+k+j*2+ps)=data_H;
			}
		}

		*(U8 *)(AD1+46+k+length*2+ps)=0x10;		//������10 03
		*(U8 *)(AD1+47+k+length*2+ps)=0x03;
		
		
		*(U8 *)(AD1+18+ps) = 0x1a + (length * 2) + k;		
		
		check=CalcHe((U8 *)(AD1+2+ps),44+k+length*2)-k*0x10;//��ȥ��ʼ�ַ�02��������к�У��	

		check_H = (check >> 4) & 0xf;
		check_L = (check >> 0) & 0xf;		
	
		*(U8 *)(AD1+48+k+length*2+ps)=asicc(check_H);		//����У�飬��asicc�뷢�ͣ��ȸߺ��        
		*(U8 *)(AD1+49+k+length*2+ps)=asicc(check_L);		
		
		ptcom->send_length[i]=50+k+length*2;				//���ͳ���
		ptcom->send_staradd[i]=ps;							//�������ݴ洢��ַ		
		ptcom->return_length[i]=44;							//�������ݳ���
		ptcom->return_start[i]=0;							//����������Ч��ʼ
		ptcom->return_length_available[i]=0;				//������Ч���ݳ���	
		
		ps=ps+50+k+length*2;								//�ۼƷ��ͳ���
		
		ptcom->send_staradd[99]++;	
		if (ptcom->send_staradd[99] >= 256)
		{
			ptcom->send_staradd[99] = 1;
		}
	}	
	ptcom->send_times=SendTimes;							//���ʹ���
	ptcom->Current_Times=0;									//��ǰ���ʹ���	
	ptcom->send_staradd[97] = 0;									//���ز������־			
}

/**************************************************************************************************
Write_Timeдʱ�亯��
	������ʱ����������ʱ��ѡ�
	���������˺����ռ������͵����ݡ�
**************************************************************************************************/
void Write_Time()			
{
	Write_Analog();											
}

/**************************************************************************************************
Read_Time��ʱ�亯��
	������ʱ����������ʱ��ѡ�
	���������˺����ռ������͵����ݡ�
**************************************************************************************************/
void Read_Time()				
{
	Read_Analog();			
}

/**************************************************************************************************
compxy���ݴ�����
	���ڲ�ͬ��PLC���ص���ֵ����ͬ��HMI����ʶ��
	����˺��������б�Ҫ��ת����ת����HMI�������ֵ��
**************************************************************************************************/
void compxy(void)				              	
{
	int i,k,j;
	unsigned char a1;
	k=0;

	if (ptcom->send_staradd[97] == 1)//λ���ش���
	{
		j = ptcom->return_length_available[ptcom->Current_Times-1];		//��Ч���ݳ���
		k = 0;		
		for(i=0;i<j;i++)		//16���Ʒ���
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i);	
			*(U8 *)(COMad+k)=a1;	
			k++;	
			if (a1 == 0x10)
			{
				i++;		//��һ����ȡ
				j++;		//��Ч��չ����
			}								
		}		
	}
	if (ptcom->send_staradd[97] == 2)//�ַ��ش���
	{
		ptcom->send_staradd[95] = ptcom->return_length_available[ptcom->Current_Times-1];
		ptcom->send_staradd[94] = ptcom->return_start[ptcom->Current_Times-1];	
	
	
		j = ptcom->return_length_available[ptcom->Current_Times-1];		//��Ч���ݳ���
		k = 0;
		for(i=0;i<j;i++)		//16���Ʒ���
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i);
			*(U8 *)(COMad+k)=a1;	
			k++;	
			if (a1 == 0x10)
			{
				i++;		//��һ����ȡ
				j++;		//��Ч��չ����
			}					
		}
		ptcom->send_staradd[93] = *(U8 *)(COMad+0);	
		ptcom->send_staradd[92] = *(U8 *)(COMad+1);	
	}	
}

/**************************************************************************************************
watchcom�������У�캯��
	���ڲ�ͬ��PLCʹ�õ�У�����ͬ��
	����˺��������б�Ҫ������У�졣����֤�����Ƿ�����
**************************************************************************************************/
void watchcom(void)		
{
	unsigned int aakj=0;
	aakj=remark();
	if(aakj==1 && ptcom->send_staradd[98] == 0)			
	{
		ptcom->IfResultCorrect=1;
		compxy();		
	}
	else
	{
		ptcom->IfResultCorrect=0;
	}
}

/**************************************************************************************************
asicc Asc��ת������
	��ЩPLC�ڽ������ݵĽ���ʱ��ʹ�õ���Asc�룬����˺�����ת����Asc��
	���͵Ĳ���a����ת����Asc�����ֵ
**************************************************************************************************/
int asicc(int result)			
{
	int temp;
	if(result<10)
		temp=result+0x30;
	if(result>9)
		temp=result-10+0x41;
	return temp;	
}

/**************************************************************************************************
asicc ����ת������
	��ЩPLC�ڽ������ݵĽ���ʱ��ʹ�õ���Asc�룬
	�������ݵĴ���ʱ����Ҫ����Ϊ���ָ�ʽ������˺�����ת��
	���͵Ĳ���a����ת����Asc�����ֵ
**************************************************************************************************/
int bsicc(int result)			
{
	int temp=0;
	if(result>=0x30)
	{
		if(result<0x40)
			temp=result-0x30;
		if(result>0x40)
			temp=result-0x41+10;
	}
	return temp;
}
#if 0
#define REG_UART_BAUD 0xB8008024
U32 _sys_uUARTClockRate = 12000000;
#define outpw(port,value)     (*((U32 volatile *) (port))=value)

 void sysSetBaudRate(U32 uBaudRate)
{
	U32 _mBaudValue;

	/* First, compute the baudrate divisor. */
	// mode 3
	_mBaudValue = (_sys_uUARTClockRate / uBaudRate)-2;
	outpw(REG_UART_BAUD,  ((0x30<<24)| _mBaudValue));	
	
}
#endif
/**************************************************************************************************
remark ���У�麯��
	�˺���Ϊ���У���Ƿ���ȷ������1���յ�����������������0���յ������ݲ�����
**************************************************************************************************/
int remark()				
{
	int i,iTemp;
	U16 check;          
	int check_H;		
	int check_L;
    unsigned char Ramdon[10];
    unsigned int XorBuf[10];//�����
    unsigned int ProductBuf[4];//�˻����
    
	int pclk = 56000000;
	int baud0 = 115200;
	int ps = 0;

	if (ptcom->send_staradd[98]==0)
	{
		if (ptcom->IQ<40)		
		{
			return 0;
		}
		return 1;	
	}
	else if (ptcom->send_staradd[98]==1)
	{
		switch (ptcom->Current_Times)
		{
		case 1:
			
			if (ptcom->IQ >= 27)
			{
				baud0 = 115200;
				sysSetBaudRate(baud0);
//robert lee				rUBRDIV0=( (int)(pclk/16./baud0+0.5) -1 );   //Baud rate divisior register 0�������ʷ�����
	            for (i=0; i<10; i++)
	            {
	                Ramdon[i] = *(U8 *)(COMad + 15 + i);
	            }
	            
	            iTemp = *(U8 *)(COMad + 14) & 0xff;
	            	
	            if (iTemp == 0x01)
	            {
	                XorBuf[0] = (Ramdon[7] ^ 0x51) & 0xff;
	                XorBuf[1] = (Ramdon[3] ^ 0x53) & 0xff;
	                XorBuf[2] = (Ramdon[0] ^ 0x4d) & 0xff;
	                XorBuf[3] = (Ramdon[6] ^ 0x2d) & 0xff;
	                XorBuf[4] = (Ramdon[5] ^ 0x43) & 0xff;
	                XorBuf[5] = (Ramdon[2] ^ 0x4c) & 0xff;
	                XorBuf[6] = (Ramdon[4] ^ 0x45) & 0xff;
	                XorBuf[7] = (Ramdon[1] ^ 0x45) & 0xff;
	                XorBuf[8] = Ramdon[8] & 0xff;
	                XorBuf[9] = Ramdon[9] & 0xff;

	                ProductBuf[0] = ((XorBuf[7] << 8) + XorBuf[6]) * ((XorBuf[3] << 8) + XorBuf[2]);
	                ProductBuf[1] = ((XorBuf[7] << 8) + XorBuf[6]) * ((XorBuf[1] << 8) + XorBuf[0]);
	                ProductBuf[2] = ((XorBuf[7] << 8) + XorBuf[6]) * ((XorBuf[5] << 8) + XorBuf[4]);
	                ProductBuf[3] = ((XorBuf[7] << 8) + XorBuf[6]) * ((XorBuf[7] << 8) + XorBuf[6]);
	            }
	            else
	            {
	                XorBuf[0] = (Ramdon[0] ^ 0x4d) & 0xff;
	                XorBuf[1] = (Ramdon[1] ^ 0x45) & 0xff;
	                XorBuf[2] = (Ramdon[2] ^ 0x4c) & 0xff;
	                XorBuf[3] = (Ramdon[3] ^ 0x53) & 0xff;
	                XorBuf[4] = (Ramdon[4] ^ 0x45) & 0xff;
	                XorBuf[5] = (Ramdon[5] ^ 0x43) & 0xff;
	                XorBuf[6] = (Ramdon[6] ^ 0x2d) & 0xff;
	                XorBuf[7] = (Ramdon[7] ^ 0x51) & 0xff;
	                XorBuf[8] = Ramdon[8] & 0xff;
	                XorBuf[9] = Ramdon[9] & 0xff;

	                for (i=0; i<8; i++)
	                {
	                    if (XorBuf[i] >= 0x41 && XorBuf[i] <= 0x5a)
	                    {
	                        XorBuf[i] = XorBuf[i] + 0x20;
	                    }
	                    else if (XorBuf[i] >= 0x61 && XorBuf[i] <= 0x78)
	                    {
	                        XorBuf[i] = XorBuf[i] - 0x20;
	                    }
	                }
	                ProductBuf[0] = ((XorBuf[7] << 8) + XorBuf[6]) * ((XorBuf[3] << 8) + XorBuf[2]);
	                ProductBuf[1] = ((XorBuf[7] << 8) + XorBuf[6]) * ((XorBuf[1] << 8) + XorBuf[0]);
	                ProductBuf[2] = ((XorBuf[7] << 8) + XorBuf[6]) * ((XorBuf[5] << 8) + XorBuf[4]);
	                ProductBuf[3] = ((XorBuf[7] << 8) + XorBuf[6]) * ((XorBuf[7] << 8) + XorBuf[6]);
	            }	
	            
	            iTemp = (ProductBuf[0] & 0xff) ^ 0x4c;
	             *(U8 *)(AD1 + 49 + ps) = iTemp & 0xff;

	            iTemp = ((ProductBuf[0] >> 8) & 0xff) ^ 0x6f;
	            *(U8 *)(AD1 + 50 + ps) = iTemp & 0xff;

	            iTemp = ((ProductBuf[0] >> 16) & 0xff) ^ 0x67;
	            *(U8 *)(AD1 + 51 + ps) = iTemp & 0xff;

	            iTemp = ((ProductBuf[0] >> 24) & 0xff) ^ 0x69;
	            *(U8 *)(AD1 + 52 + ps) = iTemp & 0xff;

	            iTemp = (ProductBuf[1] & 0xff) ^ 0x6e;
	            *(U8 *)(AD1 + 53 + ps) = iTemp & 0xff;

	            iTemp = ((ProductBuf[1] >> 8) & 0xff) ^ 0x20;
	            *(U8 *)(AD1 + 54 + ps) = iTemp & 0xff;

	            iTemp = ((ProductBuf[1] >> 16) & 0xff) ^ 0x4d;
	            *(U8 *)(AD1 + 55 + ps) = iTemp & 0xff;

	            iTemp = ((ProductBuf[1] >> 24) & 0xff) ^ 0x45;
	            *(U8 *)(AD1 + 56 + ps) = iTemp & 0xff;

	            iTemp = (ProductBuf[2] & 0xff) ^ 0x4c;
	            *(U8 *)(AD1 + 57 + ps) = iTemp & 0xff;

	            iTemp = ((ProductBuf[2] >> 8) & 0xff) ^ 0x53;
	            *(U8 *)(AD1 + 58 + ps) = iTemp & 0xff;

	            iTemp = ((ProductBuf[2] >> 16) & 0xff) ^ 0x45;
	            *(U8 *)(AD1 + 59 + ps) = iTemp & 0xff;

	            iTemp = ((ProductBuf[2] >> 24) & 0xff) ^ 0x43;
	            *(U8 *)(AD1 + 60 + ps) = iTemp & 0xff;

	            iTemp = (ProductBuf[3] & 0xff) ^ 0x2d;
	            *(U8 *)(AD1 + 61 + ps) = iTemp & 0xff;

	            iTemp = ((ProductBuf[3] >> 8) & 0xff) ^ 0x51;
	            *(U8 *)(AD1 + 62 + ps) = iTemp & 0xff;

	            iTemp = ((ProductBuf[3] >> 16) & 0xff) ^ 0x20;
	            *(U8 *)(AD1 + 63 + ps) = iTemp & 0xff;

	            iTemp = ((ProductBuf[3] >> 24) & 0xff) ^ 0x3f;
	            *(U8 *)(AD1 + 64 + ps) = iTemp & 0xff;

				check = CalcHe((U8 *)(AD1 + 11 + ps),54); 						//��ȥ��ʼ�ַ�02��������к�У��
				check_H = (check&0xf0)>>4;									//ȡУ���8λ�ĸ�λ�͵�λ
				check_H = check_H&0xf;
				check_L = check&0xf;	
				*(U8 *)(AD1 + 67 + ps) = asicc(check_H);							//����У�飬��asicc�뷢�ͣ��ȸߺ��        
				*(U8 *)(AD1 + 68 + ps) = asicc(check_L);
				
	//------------------------------------------------------------------------------------------------------------			
	            iTemp = (ProductBuf[0] & 0xff) ^ 0x48;
	            *(U8 *)(AD1 + 109 + ps) = iTemp & 0xff;

	            iTemp = ((ProductBuf[0] >> 8) & 0xff) ^ 0x65;
	            *(U8 *)(AD1 + 110 + ps) = iTemp & 0xff;

	            iTemp = ((ProductBuf[0] >> 16) & 0xff) ^ 0x6c;
	            *(U8 *)(AD1 + 111 + ps) = iTemp & 0xff;

	            iTemp = ((ProductBuf[0] >> 24) & 0xff) ^ 0x6c;
	            *(U8 *)(AD1 + 112 + ps) = iTemp & 0xff;

	            iTemp = (ProductBuf[1] & 0xff) ^ 0x6f;
	            *(U8 *)(AD1 + 113 + ps) = iTemp & 0xff;

	            iTemp = ((ProductBuf[1] >> 8) & 0xff) ^ 0x20;
	            *(U8 *)(AD1 + 114 + ps) = iTemp & 0xff;

	            iTemp = ((ProductBuf[1] >> 16) & 0xff) ^ 0x4d;
	            *(U8 *)(AD1 + 115 + ps) = iTemp & 0xff;

	            iTemp = ((ProductBuf[1] >> 24) & 0xff) ^ 0x45;
	            *(U8 *)(AD1 + 116 + ps) = iTemp & 0xff;

	            iTemp = (ProductBuf[2] & 0xff) ^ 0x4c;
	            *(U8 *)(AD1 + 117 + ps) = iTemp & 0xff;

	            iTemp = ((ProductBuf[2] >> 8) & 0xff) ^ 0x53;
	            *(U8 *)(AD1 + 118 + ps) = iTemp & 0xff;

	            iTemp = ((ProductBuf[2] >> 16) & 0xff) ^ 0x45;
	            *(U8 *)(AD1 + 119 + ps) = iTemp & 0xff;

	            iTemp = ((ProductBuf[2] >> 24) & 0xff) ^ 0x43;
	            *(U8 *)(AD1 + 120 + ps) = iTemp & 0xff;

	            iTemp = (ProductBuf[3] & 0xff) ^ 0x2d;
	            *(U8 *)(AD1 + 121 + ps) = iTemp & 0xff;

	            iTemp = ((ProductBuf[3] >> 8) & 0xff) ^ 0x51;
	            *(U8 *)(AD1 + 122 + ps) = iTemp & 0xff;

	            iTemp = ((ProductBuf[3] >> 16) & 0xff) ^ 0x20;
	            *(U8 *)(AD1 + 123 + ps) = iTemp & 0xff;

	            iTemp = ((ProductBuf[3] >> 24) & 0xff) ^ 0x21;
	            *(U8 *)(AD1 + 124 + ps) = iTemp & 0xff;

				check = CalcHe((U8 *)(AD1 + 71 + ps),54); 							//��ȥ��ʼ�ַ�02��������к�У��
				check_H = (check&0xf0)>>4;										//ȡУ���8λ�ĸ�λ�͵�λ
				check_H = check_H&0xf;
				check_L = check&0xf;	
				*(U8 *)(AD1 + 127 + ps) = asicc(check_H);							//����У�飬��asicc�뷢�ͣ��ȸߺ��        
				*(U8 *)(AD1 + 128 + ps) = asicc(check_L);
				return 2;
			}
			else
			{
				//baud0 = 9600;
				//rUBRDIV0=( (int)(pclk/16./baud0+0.5) -1 );   //Baud rate divisior register 0�������ʷ�����
				return 0;
			}        	
			break;
		case 2:
			if (ptcom->IQ == 62)
			{
			
				return 2;
			}
			else
			{
				//baud0 = 9600;
				//rUBRDIV0=( (int)(pclk/16./baud0+0.5) -1 );   //Baud rate divisior register 0�������ʷ�����
				return 0;
			}
			break;
		case 3:
			if (ptcom->IQ == 46)
			{			
				return 2;
			}
			else
			{
				//baud0 = 9600;
				//rUBRDIV0=( (int)(pclk/16./baud0+0.5) -1 );   //Baud rate divisior register 0�������ʷ�����
				return 0;
			}		
			break;
		}
	}
	return 0;	
}

/**************************************************************************************************
CalcHe ��У�麯��
	�˺����Ƕ�һ�����н������
**************************************************************************************************/
U16 CalcHe(unsigned char *chData,U16 uNo)		//�����У��
{
	int i;
	int sun=0;
	for(i=0;i<uNo;i++)
	{
		sun=sun+chData[i];
	}
	return (sun);
}
