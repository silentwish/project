/***************************************************************************
�����Ӷ�ȡ��д�붼�ǰ��ֽ�������
***************************************************************************/
#include "stdio.h"
#include "def.h"
#include "smm.h"

	
struct Com_struct_D *ptcom;


void Enter_PlcDriver(void)
{
	ptcom=(struct Com_struct_D *)adcom;

	*(U8 *)(AD1+500)=ptcom->R_W_Flag;
	*(U8 *)(AD1+501)=ptcom->registerr;

	switch (ptcom->R_W_Flag)
	{
	case PLC_READ_DATA:				//���������Ƕ�����
	case PLC_READ_DIGITALALARM:			//���������Ƕ�����,����	
	case PLC_READ_TREND:			//���������Ƕ�����,����ͼ
	case PLC_READ_ANALOGALARM:			//���������Ƕ�����,��ȱ���	
	case PLC_READ_CONTROL:			//���������Ƕ�PLC���Ƶ�����	
		switch(ptcom->registerr)
		{
		case 'X':
		case 'Y':
		case 'M':
		case 'H':
			ptcom->send_staradd[99] = 0;					
			Read_Bool();
			break;
		case 'D':
		case 'R':		
		case 't':
		case 'c':
			ptcom->send_staradd[99] = 0;	
			Read_Analog();
			break;			
		}
		break;
	case PLC_WRITE_DATA:				//����������д����
		switch(ptcom->registerr)
		{
		case 'X':
		case 'M':
		case 'Y':
		case 'T':
		case 'C':
		case 'H':
			ptcom->send_staradd[99] = 0;				
			Set_Reset();
			break;
		case 'D':
		case 'R':		
		case 't':
		case 'c':
			ptcom->send_staradd[99] = 0;
			Write_Analog();		
			break;			
		}
		break;	
	case PLC_WRITE_TIME:				//����������дʱ�䵽PLC
		switch(ptcom->registerr)
		{
		case 'D':
			ptcom->send_staradd[99] = 0;
			Write_Time();		
			break;			
		}
		break;	
	case PLC_READ_TIME:				//���������Ƕ�ȡʱ�䵽PLC
		switch(ptcom->registerr)
		{
		case 'D':	
			ptcom->send_staradd[99] = 0;	
			Read_Time();		
			break;			
		}
		break;
	case PLC_WRITE_RECIPE:				//����������д�䷽��PLC
		switch(*(U8 *)(PE+3))//�䷽�Ĵ�������
		{
		case 'D':
			ptcom->send_staradd[99] = 0;		
			Write_Recipe();		
			break;			
		}
		break;
	case PLC_READ_RECIPE:				//���������Ǵ�PLC��ȡ�䷽
		switch(*(U8 *)(PE+3))//�䷽�Ĵ�������
		{
		case 'D':
			ptcom->send_staradd[99] = 0;		
			Read_Recipe();		
			break;			
		}
		break;	
	case 7:				//����������Ϊ������
		ptcom->send_staradd[99] = 1;
		handshake();	//��������
		
		ptcom->send_staradd[97] = 0;
		ptcom->send_staradd[96] = 0;
		ptcom->send_staradd[95] = 0;
		ptcom->send_staradd[94] = 0;
		ptcom->send_staradd[93] = 0;
		ptcom->send_staradd[92] = 0;
		ptcom->send_staradd[91] = 0;
		break;						
	case PLC_CHECK_DATA:				//�������������ݴ���
		watchcom();
		break;	
	default:
		handshake();
		break;			
	}	
	if (ptcom->Simens_Count>255)
	{
		ptcom->Simens_Count=1;
	}			 
}

void handshake()		//���֣�ͨ�Ž���ǰǰ����
{
	//���ִ���
//--��1��-----------------------------------------------------------------
//OUT    02
//IN     10
//-----------------------------------------------------------------------		
	*(U8 *)(AD1+0)=0x02;
//--��2��-----------------------------------------------------------------
//OUT    01 03 02 17  00 9f 01 3c  00 90 01 14  00 00 14 02  00 0f 05 01  00 03 00 10  03 3d
//IN     10 02
//-----------------------------------------------------------------------	
	*(U8 *)(AD1+1)=0x01;
	*(U8 *)(AD1+2)=0x03;
	*(U8 *)(AD1+3)=0x02;
	*(U8 *)(AD1+4)=0x17;
	
	*(U8 *)(AD1+5)=0x00;
	*(U8 *)(AD1+6)=0x9f;
	*(U8 *)(AD1+7)=0x01;
	*(U8 *)(AD1+8)=0x3c;
	
	*(U8 *)(AD1+9)=0x00;
	*(U8 *)(AD1+10)=0x90;
	*(U8 *)(AD1+11)=0x01;
	*(U8 *)(AD1+12)=0x14;
	
	*(U8 *)(AD1+13)=0x00;
	*(U8 *)(AD1+14)=0x00;
	*(U8 *)(AD1+15)=0x14;
	*(U8 *)(AD1+16)=0x02;
	
	*(U8 *)(AD1+17)=0x00;
	*(U8 *)(AD1+18)=0x0f;
	*(U8 *)(AD1+19)=0x05;
	*(U8 *)(AD1+20)=0x01;
	
	*(U8 *)(AD1+21)=0x00;
	*(U8 *)(AD1+22)=0x03;
	*(U8 *)(AD1+23)=0x00;
	*(U8 *)(AD1+24)=0x10;
	
	*(U8 *)(AD1+25)=0x03;
	*(U8 *)(AD1+26)=0x3d;	
//--��3��-----------------------------------------------------------------
//OUT    10
//IN     01 03 20 56  30 30 2e 36  30 10 03 4f
//-----------------------------------------------------------------------	
	*(U8 *)(AD1+27)=0x10;
//--��4��-----------------------------------------------------------------
//OUT    10 02
//IN     10
//-----------------------------------------------------------------------	
	*(U8 *)(AD1+28)=0x10;
	*(U8 *)(AD1+29)=0x02;
//--��5��-----------------------------------------------------------------
//OUT    00 0d 00 03  e0 04 00 80  00 02 01 06  01 00 00 01  02 02 01 00  10 03 7d
//IN     10 02
//-----------------------------------------------------------------------	
	*(U8 *)(AD1+30)=0x00;
	*(U8 *)(AD1+31)=0x0d;
	*(U8 *)(AD1+32)=0x00;
	*(U8 *)(AD1+33)=0x03;
	
	*(U8 *)(AD1+34)=0xe0;
	*(U8 *)(AD1+35)=0x04;
	*(U8 *)(AD1+36)=0x00;
	*(U8 *)(AD1+37)=0x80;
	
	*(U8 *)(AD1+38)=0x00;
	*(U8 *)(AD1+39)=0x02;
	*(U8 *)(AD1+40)=0x01;
	*(U8 *)(AD1+41)=0x06;
	
	*(U8 *)(AD1+42)=0x01;
	*(U8 *)(AD1+43)=0x00;
	*(U8 *)(AD1+44)=0x00;
	*(U8 *)(AD1+45)=0x01;
	
	*(U8 *)(AD1+46)=0x02;
	*(U8 *)(AD1+47)=0x02;
	*(U8 *)(AD1+48)=0x01;
	*(U8 *)(AD1+49)=0x00;
	
	*(U8 *)(AD1+50)=0x10;
	*(U8 *)(AD1+51)=0x03;
	*(U8 *)(AD1+52)=0x7d;
//--��6��-----------------------------------------------------------------
//OUT    10
//IN     00 0c 03 03  d0 04 00 80  01 06 00 02  00 01 02 02  01 00 01 00  10 03 4f
//-----------------------------------------------------------------------
	*(U8 *)(AD1+53)=0x10;
//--��7��-----------------------------------------------------------------
//OUT    10 02
//IN     10
//-----------------------------------------------------------------------	
	*(U8 *)(AD1+54)=0x10;
	*(U8 *)(AD1+55)=0x02;
//--��8��-----------------------------------------------------------------
//OUT    00 0c 03 03  05 01 10 03  1b
//IN     10 02
//-----------------------------------------------------------------------	
	*(U8 *)(AD1+56)=0x00;
	*(U8 *)(AD1+57)=0x0c;
	*(U8 *)(AD1+58)=0x03;
	*(U8 *)(AD1+59)=0x03;
	
	*(U8 *)(AD1+60)=0x05;
	*(U8 *)(AD1+61)=0x01;
	*(U8 *)(AD1+62)=0x10;
	*(U8 *)(AD1+63)=0x03;
		
	*(U8 *)(AD1+64)=0x1b;
//--��9��-----------------------------------------------------------------
//OUT    10
//IN     00 0c 03 03  05 01 10 03  1b
//-----------------------------------------------------------------------			
	*(U8 *)(AD1+65)=0x10;
//--��10��-----------------------------------------------------------------
//OUT    10 02
//IN     10
//-----------------------------------------------------------------------	
	*(U8 *)(AD1+66)=0x10;
	*(U8 *)(AD1+67)=0x02;
//--��11��-----------------------------------------------------------------
//OUT    00 0c 03 03  f1 00 32 01  00 00 ff ff  00 08 00 00  f0 00 00 03  00 03 01 00  10 03 24
//IN     10 02
//-----------------------------------------------------------------------	
	*(U8 *)(AD1+68)=0x00;
	*(U8 *)(AD1+69)=0x0c;
	*(U8 *)(AD1+70)=0x03;
	*(U8 *)(AD1+71)=0x03;
	
	*(U8 *)(AD1+72)=0xf1;
	*(U8 *)(AD1+73)=0x00;
	*(U8 *)(AD1+74)=0x32;
	*(U8 *)(AD1+75)=0x01;
	
	*(U8 *)(AD1+76)=0x00;
	*(U8 *)(AD1+77)=0x00;
	*(U8 *)(AD1+78)=0xff;
	*(U8 *)(AD1+79)=0xff;
	
	*(U8 *)(AD1+80)=0x00;
	*(U8 *)(AD1+81)=0x08;
	*(U8 *)(AD1+82)=0x00;
	*(U8 *)(AD1+83)=0x00;
	
	*(U8 *)(AD1+84)=0xf0;
	*(U8 *)(AD1+85)=0x00;
	*(U8 *)(AD1+86)=0x00;
	*(U8 *)(AD1+87)=0x03;
	
	*(U8 *)(AD1+88)=0x00;
	*(U8 *)(AD1+89)=0x03;
	*(U8 *)(AD1+90)=0x01;	
	*(U8 *)(AD1+91)=0x00;
	
	*(U8 *)(AD1+92)=0x10;
	*(U8 *)(AD1+93)=0x03;
	*(U8 *)(AD1+94)=0x24;
//--��12��-----------------------------------------------------------------
//OUT    10
//IN     00 0c 03 03  b0 01 00 10  03 ae
//-----------------------------------------------------------------------	
	*(U8 *)(AD1+95)=0x10;
//--��13��-----------------------------------------------------------------
//OUT    10
//IN     02
//-----------------------------------------------------------------------		
	*(U8 *)(AD1+96)=0x10;	
//--��14��-----------------------------------------------------------------
//OUT    10
//IN     00 0c 03 03  f1 00 32 03  00 00 ff ff  00 08 00 00  00 00 f0 00  00 02 00 02  00 f0 10 03  d7
//-----------------------------------------------------------------------
	*(U8 *)(AD1+97)=0x10;
//--��15��-----------------------------------------------------------------
//OUT    10 02
//IN     10
//-----------------------------------------------------------------------	
	*(U8 *)(AD1+98)=0x10;	
	*(U8 *)(AD1+99)=0x02;
//--��16��-----------------------------------------------------------------
//OUT    00 0c 03 03  b0 01 00 10  03 ae
//IN     10
//-----------------------------------------------------------------------			
	*(U8 *)(AD1+100)=0x00;
	*(U8 *)(AD1+101)=0x0c;
	*(U8 *)(AD1+102)=0x03;
	*(U8 *)(AD1+103)=0x03;
		
	*(U8 *)(AD1+104)=0xb0;
	*(U8 *)(AD1+105)=0x01;
	*(U8 *)(AD1+106)=0x00;
	*(U8 *)(AD1+107)=0x10;
	
	*(U8 *)(AD1+108)=0x03;
	*(U8 *)(AD1+109)=0xae;	
	
//1
	ptcom->send_length[0]=1;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ			
	ptcom->return_length[0]=1;				//�������ݳ���
	ptcom->return_start[0]=0;				//����������Ч��ʼ
	ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
//2	
	ptcom->send_length[1]=26;				//���ͳ���
	ptcom->send_staradd[1]=1;				//�������ݴ洢��ַ			
	ptcom->return_length[1]=2;				//�������ݳ���
	ptcom->return_start[1]=0;				//����������Ч��ʼ
	ptcom->return_length_available[1]=0;	//������Ч���ݳ���	
//3		
	ptcom->send_length[2]=1;				//���ͳ���
	ptcom->send_staradd[2]=27;				//�������ݴ洢��ַ			
	ptcom->return_length[2]=12;				//�������ݳ���
	ptcom->return_start[2]=0;				//����������Ч��ʼ
	ptcom->return_length_available[2]=0;	//������Ч���ݳ���	
//4	
	ptcom->send_length[3]=2;				//���ͳ���
	ptcom->send_staradd[3]=28;				//�������ݴ洢��ַ			
	ptcom->return_length[3]=1;				//�������ݳ���
	ptcom->return_start[3]=0;				//����������Ч��ʼ
	ptcom->return_length_available[3]=0;	//������Ч���ݳ���					
//5
	ptcom->send_length[4]=23;				//���ͳ���
	ptcom->send_staradd[4]=30;				//�������ݴ洢��ַ			
	ptcom->return_length[4]=2;				//�������ݳ���
	ptcom->return_start[4]=0;				//����������Ч��ʼ
	ptcom->return_length_available[4]=0;	//������Ч���ݳ���	
//6	
	ptcom->send_length[5]=1;				//���ͳ���
	ptcom->send_staradd[5]=53;				//�������ݴ洢��ַ			
	ptcom->return_length[5]=23;				//�������ݳ���
	ptcom->return_start[5]=0;				//����������Ч��ʼ
	ptcom->return_length_available[5]=0;	//������Ч���ݳ���	
//7		
	ptcom->send_length[6]=2;				//���ͳ���
	ptcom->send_staradd[6]=54;				//�������ݴ洢��ַ			
	ptcom->return_length[6]=1;				//�������ݳ���
	ptcom->return_start[6]=0;				//����������Ч��ʼ
	ptcom->return_length_available[6]=0;	//������Ч���ݳ���	
//8	
	ptcom->send_length[7]=9;				//���ͳ���
	ptcom->send_staradd[7]=56;				//�������ݴ洢��ַ			
	ptcom->return_length[7]=2;				//�������ݳ���
	ptcom->return_start[7]=0;				//����������Ч��ʼ
	ptcom->return_length_available[7]=0;	//������Ч���ݳ���					
//9
	ptcom->send_length[8]=1;				//���ͳ���
	ptcom->send_staradd[8]=65;				//�������ݴ洢��ַ			
	ptcom->return_length[8]=9;				//�������ݳ���
	ptcom->return_start[8]=0;				//����������Ч��ʼ
	ptcom->return_length_available[8]=0;	//������Ч���ݳ���	
//10	
	ptcom->send_length[9]=2;				//���ͳ���
	ptcom->send_staradd[9]=66;				//�������ݴ洢��ַ			
	ptcom->return_length[9]=1;				//�������ݳ���
	ptcom->return_start[9]=0;				//����������Ч��ʼ
	ptcom->return_length_available[9]=0;	//������Ч���ݳ���	
//11		
	ptcom->send_length[10]=27;				//���ͳ���
	ptcom->send_staradd[10]=68;				//�������ݴ洢��ַ			
	ptcom->return_length[10]=2;				//�������ݳ���
	ptcom->return_start[10]=0;				//����������Ч��ʼ
	ptcom->return_length_available[10]=0;	//������Ч���ݳ���	
//12	
	ptcom->send_length[11]=1;				//���ͳ���
	ptcom->send_staradd[11]=95;				//�������ݴ洢��ַ			
	ptcom->return_length[11]=10;			//�������ݳ���
	ptcom->return_start[11]=0;				//����������Ч��ʼ
	ptcom->return_length_available[11]=0;	//������Ч���ݳ���					
//13
	ptcom->send_length[12]=1;				//���ͳ���
	ptcom->send_staradd[12]=96;				//�������ݴ洢��ַ			
	ptcom->return_length[12]=1;				//�������ݳ���
	ptcom->return_start[12]=0;				//����������Ч��ʼ
	ptcom->return_length_available[12]=0;	//������Ч���ݳ���	
//14	
	ptcom->send_length[13]=1;				//���ͳ���
	ptcom->send_staradd[13]=97;				//�������ݴ洢��ַ			
	ptcom->return_length[13]=29;			//�������ݳ���
	ptcom->return_start[13]=0;				//����������Ч��ʼ
	ptcom->return_length_available[13]=0;	//������Ч���ݳ���	
//15		
	ptcom->send_length[14]=2;				//���ͳ���
	ptcom->send_staradd[14]=98;				//�������ݴ洢��ַ			
	ptcom->return_length[14]=1;				//�������ݳ���
	ptcom->return_start[14]=0;				//����������Ч��ʼ
	ptcom->return_length_available[14]=0;	//������Ч���ݳ���	
//16	
	ptcom->send_length[15]=10;				//���ͳ���
	ptcom->send_staradd[15]=100;			//�������ݴ洢��ַ			
	ptcom->return_length[15]=1;				//�������ݳ���
	ptcom->return_start[15]=0;				//����������Ч��ʼ
	ptcom->return_length_available[15]=0;	//������Ч���ݳ���			
			
	ptcom->send_times=16;					//���ʹ���
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	ptcom->Simens_Count=1; 
}

/*************************************************************************
 * Funciton: ������д��
 * Parameters: ��
 * Return: ��
**************************************************************************/
void Set_Reset()						
{
	U16 usCalXOR;
	int iSendAdd;
	int iPlcStation;
	int iWriteValue;
	int iBlockNo;
	int iWordNo;
	int iBitNo;
	int iRegCode;
	int pos = 0;
	int pos1 = 0;
	int iTemp1,iTemp2,iTemp3;
	
	iPlcStation = ptcom->plc_address;
	iSendAdd = ptcom->address;
	iWriteValue = ptcom->writeValue;

	switch (ptcom->registerr)
	{
	case 'X'://I
		iBlockNo = 0;
		iWordNo = iSendAdd/8;
		iBitNo = iSendAdd%8;
       // iBlockNo = 1;
      //  iWordNo = 1;
        iRegCode = 0x81;
		break;
	case 'Y'://Q
		iBlockNo = 0;
		iWordNo = iSendAdd/8;
		iBitNo = iSendAdd%8;
		
		iRegCode = 0x82;
		break;
	case 'M'://M
		iBlockNo = 0;
		iWordNo = iSendAdd/8;
		iBitNo = iSendAdd%8;
		
		iRegCode = 0x83;
		break;
	case 'H'://DBBIT
		iBlockNo = (iSendAdd/8)/10000;
        iWordNo = (iSendAdd/8)%10000;
        iBitNo = iSendAdd%8;
        
        iRegCode = 0x84;
		break;
	}
	
//--��1��-----------------------------------------------------------------
	*(U8 *)(AD1 + 0) = 0x02;

//--��2��-----------------------------------------------------------------
	*(U8 *)(AD1 + 1) = 0x00;
	*(U8 *)(AD1 + 2) = 0x0c;
	*(U8 *)(AD1 + 3) = 0x03;
	*(U8 *)(AD1 + 4) = 0x03;
		
	*(U8 *)(AD1 + 5) = 0xf1;
	if (ptcom->Simens_Count == 0x10)
	{
		*(U8 *)(AD1 + 6) = 0x10;
		*(U8 *)(AD1 + 7) = 0x10;
		pos++;
	}
	else
	{
		*(U8 *)(AD1 + 6) = ptcom->Simens_Count & 0xff;
	}
	*(U8 *)(AD1 + 7 + pos) = 0x32;
	*(U8 *)(AD1 + 8 + pos) = 0x01;

	*(U8 *)(AD1 + 9 + pos) = 0x00;
	*(U8 *)(AD1 + 10 + pos) = 0x00;
	*(U8 *)(AD1 + 11 + pos) = 0x44;
	*(U8 *)(AD1 + 12 + pos) = 0x01;

	*(U8 *)(AD1 + 13 + pos) = 0x00;
	*(U8 *)(AD1 + 14 + pos) = 0x0e;
	*(U8 *)(AD1 + 15 + pos) = 0x00;
	*(U8 *)(AD1 + 16 + pos) = 0x05;

	*(U8 *)(AD1 + 17 + pos) = 0x05;
	*(U8 *)(AD1 + 18 + pos) = 0x01;
	*(U8 *)(AD1 + 19 + pos) = 0x12;
	*(U8 *)(AD1 + 20 + pos) = 0x0a;

	*(U8 *)(AD1 + 21 + pos) = 0x10;
	*(U8 *)(AD1 + 22 + pos) = 0x10;
	*(U8 *)(AD1 + 23 + pos) = 0x01;
	*(U8 *)(AD1 + 24 + pos) = 0x00;

	*(U8 *)(AD1 + 25 + pos) = 0x01;

    iTemp1 = (iBlockNo >> 8) & 0xff;
    iTemp2 = (iBlockNo >> 0) & 0xff;
    if (iTemp1 == 0x10)
    {
        *(U8 *)(AD1 + 26 + pos) = 0x10;
        *(U8 *)(AD1 + 27 + pos) = 0x10;
        pos++;
    }
    else
    {
        *(U8 *)(AD1 + 26 + pos) = iTemp1;
    }

    if (iTemp2 == 0x10)
    {
        *(U8 *)(AD1 + 27 + pos) = 0x10;
        *(U8 *)(AD1 + 28 + pos) = 0x10;
        pos++;
    }
    else
    {
        *(U8 *)(AD1 + 27 + pos) = iTemp2;
    }

	*(U8 *)(AD1 + 28 + pos) = iRegCode & 0xff;

    iTemp1 = ((iWordNo*8 + iBitNo) >> 16) & 0xff;
    iTemp2 = ((iWordNo*8 + iBitNo) >> 8) & 0xff;
    iTemp3 = ((iWordNo*8 + iBitNo) >> 0) & 0xff;
    if (iTemp1 == 0x10)
    {
        *(U8 *)(AD1 + 29 + pos) = 0x10;
        *(U8 *)(AD1 + 30 + pos) = 0x10;
        pos++;
    }
    else
    {
        *(U8 *)(AD1 + 29 + pos) = iTemp1;
    }

    if (iTemp2 == 0x10)
    {
        *(U8 *)(AD1 + 30 + pos) = 0x10;
        *(U8 *)(AD1 + 31 + pos) = 0x10;
        pos++;
    }
    else
    {
        *(U8 *)(AD1 + 30 + pos) = iTemp2;
    }

    if (iTemp3 == 0x10)
    {
        *(U8 *)(AD1 + 31 + pos) = 0x10;
        *(U8 *)(AD1 + 32 + pos) = 0x10;
        pos++;
    }
    else
    {
        *(U8 *)(AD1 + 31 + pos) = iTemp3;
    }
    
    *(U8 *)(AD1 + 32 + pos) = 0x00;
    *(U8 *)(AD1 + 33 + pos) = 0x03;
    *(U8 *)(AD1 + 34 + pos) = 0x00;
    *(U8 *)(AD1 + 35 + pos) = 0x01;
    
    *(U8 *)(AD1 + 36 + pos) = iWriteValue & 0xff;
    
    *(U8 *)(AD1 + 37 + pos) = 0x10;
    *(U8 *)(AD1 + 38 + pos) = 0x03;   

	usCalXOR = CalXor((U8 *)(AD1 + 1),38 + pos);		
	*(U8 *)(AD1 + 39 + pos) = usCalXOR & 0xff;

//--��3��-----------------------------------------------------------------
	*(U8 *)(AD1 + 40 + pos) = 0x10;

//--��4��-----------------------------------------------------------------
	*(U8 *)(AD1 + 41 + pos) = 0x10;

//--��5��-----------------------------------------------------------------
	*(U8 *)(AD1 + 42 + pos) = 0x10;

//--��6��-----------------------------------------------------------------
	*(U8 *)(AD1 + 43 + pos) = 0x10;
	*(U8 *)(AD1 + 44 + pos) = 0x02;

//--��7��-----------------------------------------------------------------
    *(U8 *)(AD1 + 45 + pos) = 0x00;
    *(U8 *)(AD1 + 46 + pos) = 0x0c;
    *(U8 *)(AD1 + 47 + pos) = 0x03;
    *(U8 *)(AD1 + 48 + pos) = 0x03;

    *(U8 *)(AD1 + 49 + pos) = 0xb0;
    *(U8 *)(AD1 + 50 + pos) = 0x01;
    if (ptcom->Simens_Count == 0x10)
    {
        *(U8 *)(AD1 + 51 + pos) = 0x10;
        *(U8 *)(AD1 + 52 + pos) = 0x10;
        pos1++;
    }
    else
    {
        *(U8 *)(AD1 + 51 + pos) = ptcom->Simens_Count & 0xff;
    }

    *(U8 *)(AD1 + 52 + pos + pos1) = 0x10;
    *(U8 *)(AD1 + 53 + pos + pos1) = 0x03;

	usCalXOR = CalXor((U8 *)(AD1 + 45 + pos),9 + pos1);		
	*(U8 *)(AD1 + 54 + pos + pos1) = usCalXOR & 0xff;
	
//1
	ptcom->send_length[0]=1;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ			
	ptcom->return_length[0]=1;				//�������ݳ���
	ptcom->return_start[0]=0;				//����������Ч��ʼ
	ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
//2	
	ptcom->send_length[1]=39 + pos;			//���ͳ���
	ptcom->send_staradd[1]=1;				//�������ݴ洢��ַ			
	ptcom->return_length[1]=2;				//�������ݳ���
	ptcom->return_start[1]=0;				//����������Ч��ʼ
	ptcom->return_length_available[1]=0;	//������Ч���ݳ���	
//3		
	ptcom->send_length[2]=1;				//���ͳ���
	ptcom->send_staradd[2]=40 + pos;		//�������ݴ洢��ַ			
	ptcom->return_length[2]=10;				//�������ݳ���
	ptcom->return_start[2]=0;				//����������Ч��ʼ
	ptcom->return_length_available[2]=0;	//������Ч���ݳ���	
//4	
	ptcom->send_length[3]=1;				//���ͳ���
	ptcom->send_staradd[3]=41 + pos;		//�������ݴ洢��ַ			
	ptcom->return_length[3]=1;				//�������ݳ���
	ptcom->return_start[3]=0;				//����������Ч��ʼ
	ptcom->return_length_available[3]=0;	//������Ч���ݳ���					
//5
	ptcom->send_length[4]=1;				//���ͳ���
	ptcom->send_staradd[4]=42 + pos;		//�������ݴ洢��ַ
	if (ptcom->Simens_Count == 0x10)
	{
		ptcom->return_length[4]=25;
	}	
	else
	{
		ptcom->return_length[4]=24;			//�������ݳ���
	}		
	ptcom->return_start[4]=0;				//����������Ч��ʼ
	ptcom->return_length_available[4]=0;	//������Ч���ݳ���	
//6	
	ptcom->send_length[5]=2;				//���ͳ���
	ptcom->send_staradd[5]=43 + pos;		//�������ݴ洢��ַ			
	ptcom->return_length[5]=1;				//�������ݳ���
	ptcom->return_start[5]=0;				//����������Ч��ʼ
	ptcom->return_length_available[5]=0;	//������Ч���ݳ���	
//7		
	ptcom->send_length[6]=10 + pos1;		//���ͳ���
	ptcom->send_staradd[6]=45 + pos;		//�������ݴ洢��ַ			
	ptcom->return_length[6]=1;				//�������ݳ���
	ptcom->return_start[6]=0;				//����������Ч��ʼ
	ptcom->return_length_available[6]=0;	//������Ч���ݳ���	
	
	ptcom->send_times=7;					//���ʹ���
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	ptcom->Simens_Count++;									
}

/*************************************************************************
 * Funciton: ��������ȡ
 * Parameters: ��
 * Return: ��
**************************************************************************/
void Read_Bool()				
{
	U16 usCalXOR;
	int iSendAdd;
	int iPlcStation;
	int iLength;
	int iBlockNo = 0;
	int iWordNo;
	int iRegCode;
	int pos = 0;
	int pos1 = 0;
	int pos2 = 0;
	int iTemp1,iTemp2,iTemp3;
	
	iPlcStation = ptcom->plc_address;
	iSendAdd = ptcom->address;
	iLength = ptcom->register_length;
	
	
	switch (ptcom->registerr)
	{
	case 'M'://m
		iBlockNo = 0;
		iWordNo = iSendAdd;
		iRegCode = 0x83;
		break;
	case 'Y'://Q
		iBlockNo = 0;
		iWordNo = iSendAdd;
		iRegCode = 0x82;
		break;
	case 'H'://DB_BIT
		iBlockNo = (iSendAdd/8)/10000;
		iWordNo = (iSendAdd/8)%10000;
		iRegCode = 0x84;
		break;
	case 'X'://I
		iBlockNo = 0;
		iWordNo = iSendAdd;
       // iBlockNo = 1;
      //  iWordNo = 1;
        iRegCode = 0x81;
		break;
	}
	
	
//--��1��-----------------------------------------------------------------
	*(U8 *)(AD1 + 0) = 0x02;

//--��2��-----------------------------------------------------------------
	*(U8 *)(AD1 + 1) = 0x00;
	*(U8 *)(AD1 + 2) = 0x0c;
	*(U8 *)(AD1 + 3) = 0x03;
	*(U8 *)(AD1 + 4) = 0x03;
		
	*(U8 *)(AD1 + 5) = 0xf1;
	if (ptcom->Simens_Count == 0x10)
	{
		*(U8 *)(AD1 + 6) = 0x10;
		*(U8 *)(AD1 + 7) = 0x10;
		pos++;
	}
	else
	{
		*(U8 *)(AD1 + 6) = ptcom->Simens_Count & 0xff;
	}
	*(U8 *)(AD1 + 7 + pos) = 0x32;
	*(U8 *)(AD1 + 8 + pos) = 0x01;

	*(U8 *)(AD1 + 9 + pos) = 0x00;
	*(U8 *)(AD1 + 10 + pos) = 0x00;
	*(U8 *)(AD1 + 11 + pos) = 0x33;
	*(U8 *)(AD1 + 12 + pos) = 0x01;

	*(U8 *)(AD1 + 13 + pos) = 0x00;
	*(U8 *)(AD1 + 14 + pos) = 0x0e;
	*(U8 *)(AD1 + 15 + pos) = 0x00;
	*(U8 *)(AD1 + 16 + pos) = 0x00;

	*(U8 *)(AD1 + 17 + pos) = 0x04;
	*(U8 *)(AD1 + 18 + pos) = 0x01;
	*(U8 *)(AD1 + 19 + pos) = 0x12;
	*(U8 *)(AD1 + 20 + pos) = 0x0a;

	*(U8 *)(AD1 + 21 + pos) = 0x10;
	*(U8 *)(AD1 + 22 + pos) = 0x10;
	*(U8 *)(AD1 + 23 + pos) = 0x02;
	*(U8 *)(AD1 + 24 + pos) = 0x00;
	
	if (iLength*2 == 0x10)
	{
		*(U8 *)(AD1 + 25 + pos) = 0x10;
		*(U8 *)(AD1 + 26 + pos) = 0x10;
	}
	else
	{
		*(U8 *)(AD1 + 25 + pos) = iLength*2 & 0xff;
	}	

    iTemp1 = (iBlockNo >> 8) & 0xff;
    iTemp2 = (iBlockNo >> 0) & 0xff;
    if (iTemp1 == 0x10)
    {
        *(U8 *)(AD1 + 26 + pos) = 0x10;
        *(U8 *)(AD1 + 27 + pos) = 0x10;
        pos++;
    }
    else
    {
        *(U8 *)(AD1 + 26 + pos) = iTemp1;
    }

    if (iTemp2 == 0x10)
    {
        *(U8 *)(AD1 + 27 + pos) = 0x10;
        *(U8 *)(AD1 + 28 + pos) = 0x10;
        pos++;
    }
    else
    {
        *(U8 *)(AD1 + 27 + pos) = iTemp2;
    }

	*(U8 *)(AD1 + 28 + pos) = iRegCode & 0xff;

    iTemp1 = ((iWordNo * 8) >> 16) & 0xff;
    iTemp2 = ((iWordNo * 8) >> 8) & 0xff;
    iTemp3 = ((iWordNo * 8) >> 0) & 0xff;
    
    if(ptcom->registerr == 'H')
    {
    	iTemp1 = ((iWordNo * 8) >> 16) & 0xff;
   		 iTemp2 = ((iWordNo * 8) >> 8) & 0xff;
    	iTemp3 = ((iWordNo * 8) >> 0) & 0xff;
    }
    else
    {
    	iTemp1 = ((iWordNo ) >> 16) & 0xff;
    	iTemp2 = ((iWordNo ) >> 8) & 0xff;
   		 iTemp3 = ((iWordNo) >> 0) & 0xff;
    }
    
    if (iTemp1 == 0x10)
    {
        *(U8 *)(AD1 + 29 + pos) = 0x10;
        *(U8 *)(AD1 + 30 + pos) = 0x10;
        pos++;
    }
    else
    {
        *(U8 *)(AD1 + 29 + pos) = iTemp1;
    }

    if (iTemp2 == 0x10)
    {
        *(U8 *)(AD1 + 30 + pos) = 0x10;
        *(U8 *)(AD1 + 31 + pos) = 0x10;
        pos++;
    }
    else
    {
        *(U8 *)(AD1 + 30 + pos) = iTemp2;
    }

    if (iTemp3 == 0x10)
    {
        *(U8 *)(AD1 + 31 + pos) = 0x10;
        *(U8 *)(AD1 + 32 + pos) = 0x10;
        pos++;
    }
    else
    {
        *(U8 *)(AD1 + 31 + pos) = iTemp3;
    }   
    
    *(U8 *)(AD1 + 32 + pos) = 0x10;
    *(U8 *)(AD1 + 33 + pos) = 0x03;   

	usCalXOR = CalXor((U8 *)(AD1 + 1),33 + pos);		
	*(U8 *)(AD1 + 34 + pos) = usCalXOR & 0xff;

//--��3��-----------------------------------------------------------------
	*(U8 *)(AD1 + 35 + pos) = 0x10;

//--��4��-----------------------------------------------------------------
	*(U8 *)(AD1 + 36 + pos) = 0x10;

//--��5��-----------------------------------------------------------------
	*(U8 *)(AD1 + 37 + pos) = 0x10;

//--��6��-----------------------------------------------------------------
	*(U8 *)(AD1 + 38 + pos) = 0x10;
	*(U8 *)(AD1 + 39 + pos) = 0x02;

//--��7��-----------------------------------------------------------------
    *(U8 *)(AD1 + 40 + pos) = 0x00;
    *(U8 *)(AD1 + 41 + pos) = 0x0c;
    *(U8 *)(AD1 + 42 + pos) = 0x03;
    *(U8 *)(AD1 + 43 + pos) = 0x03;

    *(U8 *)(AD1 + 44 + pos) = 0xb0;
    *(U8 *)(AD1 + 45 + pos) = 0x01;
    if (ptcom->Simens_Count == 0x10)
    {
        *(U8 *)(AD1 + 46 + pos) = 0x10;
        *(U8 *)(AD1 + 47 + pos) = 0x10;
        pos1++;
    }
    else
    {
        *(U8 *)(AD1 + 46 + pos) = ptcom->Simens_Count & 0xff;
    }

    *(U8 *)(AD1 + 47 + pos + pos1) = 0x10;
    *(U8 *)(AD1 + 48 + pos + pos1) = 0x03;

	usCalXOR = CalXor((U8 *)(AD1 + 40 + pos),9 + pos1);		
	*(U8 *)(AD1 + 49 + pos + pos1) = usCalXOR & 0xff;
	
//1
	ptcom->send_length[0]=1;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ			
	ptcom->return_length[0]=1;				//�������ݳ���
	ptcom->return_start[0]=0;				//����������Ч��ʼ
	ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
//2	
	ptcom->send_length[1]=34 + pos;			//���ͳ���
	ptcom->send_staradd[1]=1;				//�������ݴ洢��ַ			
	ptcom->return_length[1]=2;				//�������ݳ���
	ptcom->return_start[1]=0;				//����������Ч��ʼ
	ptcom->return_length_available[1]=0;	//������Ч���ݳ���	
//3		
	ptcom->send_length[2]=1;				//���ͳ���
	ptcom->send_staradd[2]=35 + pos;		//�������ݴ洢��ַ
	if (ptcom->Simens_Count == 0x10)
	{
		ptcom->return_length[2]=11;
	}	
	else
	{
		ptcom->return_length[2]=10;			//�������ݳ���
	}			
	ptcom->return_start[2]=0;				//����������Ч��ʼ
	ptcom->return_length_available[2]=0;	//������Ч���ݳ���	
//4	
	ptcom->send_length[3]=1;				//���ͳ���
	ptcom->send_staradd[3]=36 + pos;		//�������ݴ洢��ַ			
	ptcom->return_length[3]=1;				//�������ݳ���
	ptcom->return_start[3]=0;				//����������Ч��ʼ
	ptcom->return_length_available[3]=0;	//������Ч���ݳ���					
//5
	ptcom->send_length[4]=1;				//���ͳ���
	ptcom->send_staradd[4]=37 + pos;		//�������ݴ洢��ַ
	if (ptcom->Simens_Count == 0x10)
    {
        pos2++;
    }
    if (iLength*2 + 0x04 == 0x10)
    {
        pos2++;
    }
    if (iLength*2*8 == 0x10)
    {
        pos2++;
    }
	ptcom->return_length[4]=27 + iLength*2;	//�������ݳ���				
	ptcom->return_start[4]=24 + pos2;		//����������Ч��ʼ
	ptcom->return_length_available[4]=iLength*2;//������Ч���ݳ���	
//6	
	ptcom->send_length[5]=2;				//���ͳ���
	ptcom->send_staradd[5]=38 + pos;		//�������ݴ洢��ַ			
	ptcom->return_length[5]=1;				//�������ݳ���
	ptcom->return_start[5]=0;				//����������Ч��ʼ
	ptcom->return_length_available[5]=0;	//������Ч���ݳ���	
//7		
	ptcom->send_length[6]=10 + pos1;		//���ͳ���
	ptcom->send_staradd[6]=40 + pos;		//�������ݴ洢��ַ			
	ptcom->return_length[6]=1;				//�������ݳ���
	ptcom->return_start[6]=0;				//����������Ч��ʼ
	ptcom->return_length_available[6]=0;	//������Ч���ݳ���	
	
	ptcom->send_times=7;					//���ʹ���
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	ptcom->Simens_Count++;	
	*(U8 *)(COMad+300)=ptcom->send_times;
	
	ptcom->send_add[4]=ptcom->address;		//����������Ч��ʼ
	ptcom->send_data_length[4]=iLength*2;	//������Ч���ݳ���			
}

/*************************************************************************
 * Funciton: ģ������ȡ
 * Parameters: ��
 * Return: ��
**************************************************************************/
void Read_Analog()				
{
	U16 usCalXOR;
	int iSendAdd;
	int iPlcStation;
	int iLength;
	int iBlockNo;
	int iWordNo;
	int iRegCode;
	int pos = 0;
	int pos1 = 0;
	int pos2 = 0;
	int iTemp1,iTemp2,iTemp3;
	
	iPlcStation = ptcom->plc_address;
	iSendAdd = ptcom->address;
	iLength = ptcom->register_length;

	switch (ptcom->registerr)
	{
	case 'R'://MW
		iBlockNo = 0;
		iWordNo = iSendAdd*2;
		
		iRegCode = 0x83;
		break;
	case 'D'://DB
		iBlockNo = (iSendAdd*2)/10000;
        iWordNo = (iSendAdd*2)%10000;
        
        iRegCode = 0x84;
		break;
	}
	
//--��1��-----------------------------------------------------------------
	*(U8 *)(AD1 + 0) = 0x02;

//--��2��-----------------------------------------------------------------
	*(U8 *)(AD1 + 1) = 0x00;
	*(U8 *)(AD1 + 2) = 0x0c;
	*(U8 *)(AD1 + 3) = 0x03;
	*(U8 *)(AD1 + 4) = 0x03;
		
	*(U8 *)(AD1 + 5) = 0xf1;
	if (ptcom->Simens_Count == 0x10)
	{
		*(U8 *)(AD1 + 6) = 0x10;
		*(U8 *)(AD1 + 7) = 0x10;
		pos++;
	}
	else
	{
		*(U8 *)(AD1 + 6) = ptcom->Simens_Count & 0xff;
	}
	*(U8 *)(AD1 + 7 + pos) = 0x32;
	*(U8 *)(AD1 + 8 + pos) = 0x01;

	*(U8 *)(AD1 + 9 + pos) = 0x00;
	*(U8 *)(AD1 + 10 + pos) = 0x00;
	*(U8 *)(AD1 + 11 + pos) = 0x33;
	*(U8 *)(AD1 + 12 + pos) = 0x01;

	*(U8 *)(AD1 + 13 + pos) = 0x00;
	*(U8 *)(AD1 + 14 + pos) = 0x0e;
	*(U8 *)(AD1 + 15 + pos) = 0x00;
	*(U8 *)(AD1 + 16 + pos) = 0x00;

	*(U8 *)(AD1 + 17 + pos) = 0x04;
	*(U8 *)(AD1 + 18 + pos) = 0x01;
	*(U8 *)(AD1 + 19 + pos) = 0x12;
	*(U8 *)(AD1 + 20 + pos) = 0x0a;

	*(U8 *)(AD1 + 21 + pos) = 0x10;
	*(U8 *)(AD1 + 22 + pos) = 0x10;
	*(U8 *)(AD1 + 23 + pos) = 0x02;
	*(U8 *)(AD1 + 24 + pos) = 0x00;
	
	if (iLength*2 == 0x10)
	{
		*(U8 *)(AD1 + 25 + pos) = 0x10;
		*(U8 *)(AD1 + 26 + pos) = 0x10;
	}
	else
	{
		*(U8 *)(AD1 + 25 + pos) = iLength*2 & 0xff;
	}	

    iTemp1 = (iBlockNo >> 8) & 0xff;
    iTemp2 = (iBlockNo >> 0) & 0xff;
    if (iTemp1 == 0x10)
    {
        *(U8 *)(AD1 + 26 + pos) = 0x10;
        *(U8 *)(AD1 + 27 + pos) = 0x10;
        pos++;
    }
    else
    {
        *(U8 *)(AD1 + 26 + pos) = iTemp1;
    }

    if (iTemp2 == 0x10)
    {
        *(U8 *)(AD1 + 27 + pos) = 0x10;
        *(U8 *)(AD1 + 28 + pos) = 0x10;
        pos++;
    }
    else
    {
        *(U8 *)(AD1 + 27 + pos) = iTemp2;
    }

	*(U8 *)(AD1 + 28 + pos) = iRegCode & 0xff;

    iTemp1 = ((iWordNo*8) >> 16) & 0xff;
    iTemp2 = ((iWordNo*8) >> 8) & 0xff;
    iTemp3 = ((iWordNo*8) >> 0) & 0xff;
    if (iTemp1 == 0x10)
    {
        *(U8 *)(AD1 + 29 + pos) = 0x10;
        *(U8 *)(AD1 + 30 + pos) = 0x10;
        pos++;
    }
    else
    {
        *(U8 *)(AD1 + 29 + pos) = iTemp1;
    }

    if (iTemp2 == 0x10)
    {
        *(U8 *)(AD1 + 30 + pos) = 0x10;
        *(U8 *)(AD1 + 31 + pos) = 0x10;
        pos++;
    }
    else
    {
        *(U8 *)(AD1 + 30 + pos) = iTemp2;
    }

    if (iTemp3 == 0x10)
    {
        *(U8 *)(AD1 + 31 + pos) = 0x10;
        *(U8 *)(AD1 + 32 + pos) = 0x10;
        pos++;
    }
    else
    {
        *(U8 *)(AD1 + 31 + pos) = iTemp3;
    }   
    
    *(U8 *)(AD1 + 32 + pos) = 0x10;
    *(U8 *)(AD1 + 33 + pos) = 0x03;   

	usCalXOR = CalXor((U8 *)(AD1 + 1),33 + pos);		
	*(U8 *)(AD1 + 34 + pos) = usCalXOR & 0xff;

//--��3��-----------------------------------------------------------------
	*(U8 *)(AD1 + 35 + pos) = 0x10;

//--��4��-----------------------------------------------------------------
	*(U8 *)(AD1 + 36 + pos) = 0x10;

//--��5��-----------------------------------------------------------------
	*(U8 *)(AD1 + 37 + pos) = 0x10;

//--��6��-----------------------------------------------------------------
	*(U8 *)(AD1 + 38 + pos) = 0x10;
	*(U8 *)(AD1 + 39 + pos) = 0x02;

//--��7��-----------------------------------------------------------------
    *(U8 *)(AD1 + 40 + pos) = 0x00;
    *(U8 *)(AD1 + 41 + pos) = 0x0c;
    *(U8 *)(AD1 + 42 + pos) = 0x03;
    *(U8 *)(AD1 + 43 + pos) = 0x03;

    *(U8 *)(AD1 + 44 + pos) = 0xb0;
    *(U8 *)(AD1 + 45 + pos) = 0x01;
    if (ptcom->Simens_Count == 0x10)
    {
        *(U8 *)(AD1 + 46 + pos) = 0x10;
        *(U8 *)(AD1 + 47 + pos) = 0x10;
        pos1++;
    }
    else
    {
        *(U8 *)(AD1 + 46 + pos) = ptcom->Simens_Count & 0xff;
    }

    *(U8 *)(AD1 + 47 + pos + pos1) = 0x10;
    *(U8 *)(AD1 + 48 + pos + pos1) = 0x03;

	usCalXOR = CalXor((U8 *)(AD1 + 40 + pos),9 + pos1);		
	*(U8 *)(AD1 + 49 + pos + pos1) = usCalXOR & 0xff;
	
//1
	ptcom->send_length[0]=1;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ			
	ptcom->return_length[0]=1;				//�������ݳ���
	ptcom->return_start[0]=0;				//����������Ч��ʼ
	ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
//2	
	ptcom->send_length[1]=34 + pos;			//���ͳ���
	ptcom->send_staradd[1]=1;				//�������ݴ洢��ַ			
	ptcom->return_length[1]=2;				//�������ݳ���
	ptcom->return_start[1]=0;				//����������Ч��ʼ
	ptcom->return_length_available[1]=0;	//������Ч���ݳ���	
//3		
	ptcom->send_length[2]=1;				//���ͳ���
	ptcom->send_staradd[2]=35 + pos;		//�������ݴ洢��ַ
	if (ptcom->Simens_Count == 0x10)
	{
		ptcom->return_length[2]=11;
	}	
	else
	{
		ptcom->return_length[2]=10;			//�������ݳ���
	}			
	ptcom->return_start[2]=0;				//����������Ч��ʼ
	ptcom->return_length_available[2]=0;	//������Ч���ݳ���	
//4	
	ptcom->send_length[3]=1;				//���ͳ���
	ptcom->send_staradd[3]=36 + pos;		//�������ݴ洢��ַ			
	ptcom->return_length[3]=1;				//�������ݳ���
	ptcom->return_start[3]=0;				//����������Ч��ʼ
	ptcom->return_length_available[3]=0;	//������Ч���ݳ���					
//5
	ptcom->send_length[4]=1;				//���ͳ���
	ptcom->send_staradd[4]=37 + pos;		//�������ݴ洢��ַ
	if (ptcom->Simens_Count == 0x10)
    {
        pos2++;
    }
    if (iLength*2 + 0x04 == 0x10)
    {
        pos2++;
    }
    if (iLength*2*8 == 0x10)
    {
        pos2++;
    }
	ptcom->return_length[4]=27 + iLength*2;	//�������ݳ���				
	ptcom->return_start[4]=24 + pos2;		//����������Ч��ʼ
	ptcom->return_length_available[4]=iLength*2;//������Ч���ݳ���	
//6	
	ptcom->send_length[5]=2;				//���ͳ���
	ptcom->send_staradd[5]=38 + pos;		//�������ݴ洢��ַ			
	ptcom->return_length[5]=1;				//�������ݳ���
	ptcom->return_start[5]=0;				//����������Ч��ʼ
	ptcom->return_length_available[5]=0;	//������Ч���ݳ���	
//7		
	ptcom->send_length[6]=10 + pos1;		//���ͳ���
	ptcom->send_staradd[6]=40 + pos;		//�������ݴ洢��ַ			
	ptcom->return_length[6]=1;				//�������ݳ���
	ptcom->return_start[6]=0;				//����������Ч��ʼ
	ptcom->return_length_available[6]=0;	//������Ч���ݳ���	
	
	ptcom->send_times=7;					//���ʹ���
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	ptcom->Simens_Count++;	
	//*(U8 *)(COMad+300)=ptcom->send_times;
}

/*************************************************************************
 * Funciton: �䷽��ȡ
 * Parameters: ��
 * Return: ��
**************************************************************************/
void Read_Recipe()							
{
/*	U16 usCalXOR;
	int iSendAdd;
	int iPlcStation;
	int iLength;
	int iBlockNo;
	int iWordNo;
	int iRegCode;
	int pos = 0;
	int pos1 = 0;
	int pos2 = 0;
	int iTemp1,iTemp2,iTemp3;
	int iSendTimes;
	int iLastTimeLength;
	int i;
	int iSendLength;
	int PS = 0;
	
	
	iPlcStation = ptcom->plc_address;
	iSendAdd = ptcom->address;
	iLength = ptcom->register_length;

	if(iLength%32 == 0)
	{
		iSendTimes = iLength/32;
		iLastTimeLength = 32;	
	}
	if(iLength%32!=0)
	{
		iSendTimes = iLength/32+1;
		iLastTimeLength = iLength%32;
	}
	
	for (i=0; i<iSendTimes; i++)
	{
		switch (ptcom->registerr)
		{
		case 'R'://MW
			iBlockNo = 0;
			iWordNo = (iSendAdd + i*32)*2;
			
			iRegCode = 0x83;
			break;
		case 'D'://DB
			iBlockNo = ((iSendAdd + i*32)*2)/10000;
	        iWordNo = ((iSendAdd + i*32)*2)%1000;
	        
	        iRegCode = 0x84;
			break;
		}
		
//--��1��-----------------------------------------------------------------
		*(U8 *)(AD1 + 0 + PS) = 0x02;

//--��2��-----------------------------------------------------------------
		*(U8 *)(AD1 + 1 + PS) = 0x00;
		*(U8 *)(AD1 + 2 + PS) = 0x0c;
		*(U8 *)(AD1 + 3 + PS) = 0x03;
		*(U8 *)(AD1 + 4 + PS) = 0x03;
			
		*(U8 *)(AD1 + 5 + PS) = 0xf1;
		if (ptcom->Simens_Count == 0x10)
		{
			*(U8 *)(AD1 + 6 + PS) = 0x10;
			*(U8 *)(AD1 + 7 + PS) = 0x10;
			pos++;
		}
		else
		{
			*(U8 *)(AD1 + 6 + PS) = ptcom->Simens_Count & 0xff;
		}
		*(U8 *)(AD1 + 7 + pos + PS) = 0x32;
		*(U8 *)(AD1 + 8 + pos + PS) = 0x01;

		*(U8 *)(AD1 + 9 + pos + PS) = 0x00;
		*(U8 *)(AD1 + 10 + pos + PS) = 0x00;
		*(U8 *)(AD1 + 11 + pos + PS) = 0x33;
		*(U8 *)(AD1 + 12 + pos + PS) = 0x01;

		*(U8 *)(AD1 + 13 + pos + PS) = 0x00;
		*(U8 *)(AD1 + 14 + pos + PS) = 0x0e;
		*(U8 *)(AD1 + 15 + pos + PS) = 0x00;
		*(U8 *)(AD1 + 16 + pos + PS) = 0x00;

		*(U8 *)(AD1 + 17 + pos + PS) = 0x04;
		*(U8 *)(AD1 + 18 + pos + PS) = 0x01;
		*(U8 *)(AD1 + 19 + pos + PS) = 0x12;
		*(U8 *)(AD1 + 20 + pos + PS) = 0x0a;

		*(U8 *)(AD1 + 21 + pos + PS) = 0x10;
		*(U8 *)(AD1 + 22 + pos + PS) = 0x10;
		*(U8 *)(AD1 + 23 + pos + PS) = 0x02;
		*(U8 *)(AD1 + 24 + pos + PS) = 0x00;
		
		if (i != (iSendTimes-1))
		{
			iSendLength = 32*2;
		}
		else
		{
			iSendLength = iLastTimeLength*2;
		}
		
		if (iSendLength*2 == 0x10)
		{
			*(U8 *)(AD1 + 25 + pos + PS) = 0x10;
			*(U8 *)(AD1 + 26 + pos + PS) = 0x10;
		}
		else
		{
			*(U8 *)(AD1 + 25 + pos + PS) = iLength*2 & 0xff;
		}	

	    iTemp1 = (iBlockNo >> 8) & 0xff;
	    iTemp2 = (iBlockNo >> 0) & 0xff;
	    if (iTemp1 == 0x10)
	    {
	        *(U8 *)(AD1 + 26 + pos + PS) = 0x10;
	        *(U8 *)(AD1 + 27 + pos + PS) = 0x10;
	        pos++;
	    }
	    else
	    {
	        *(U8 *)(AD1 + 26 + pos + PS) = iTemp1;
	    }

	    if (iTemp2 == 0x10)
	    {
	        *(U8 *)(AD1 + 27 + pos + PS) = 0x10;
	        *(U8 *)(AD1 + 28 + pos + PS) = 0x10;
	        pos++;
	    }
	    else
	    {
	        *(U8 *)(AD1 + 27 + pos + PS) = iTemp2;
	    }

		*(U8 *)(AD1 + 28 + pos + PS) = iRegCode & 0xff;

	    iTemp1 = ((iWordNo*8) >> 16) & 0xff;
	    iTemp2 = ((iWordNo*8) >> 8) & 0xff;
	    iTemp3 = ((iWordNo*8) >> 0) & 0xff;
	    if (iTemp1 == 0x10)
	    {
	        *(U8 *)(AD1 + 29 + pos + PS) = 0x10;
	        *(U8 *)(AD1 + 30 + pos + PS) = 0x10;
	        pos++;
	    }
	    else
	    {
	        *(U8 *)(AD1 + 29 + pos + PS) = iTemp1;
	    }

	    if (iTemp2 == 0x10)
	    {
	        *(U8 *)(AD1 + 30 + pos + PS) = 0x10;
	        *(U8 *)(AD1 + 31 + pos + PS) = 0x10;
	        pos++;
	    }
	    else
	    {
	        *(U8 *)(AD1 + 30 + pos + PS) = iTemp2;
	    }

	    if (iTemp3 == 0x10)
	    {
	        *(U8 *)(AD1 + 31 + pos + PS) = 0x10;
	        *(U8 *)(AD1 + 32 + pos + PS) = 0x10;
	        pos++;
	    }
	    else
	    {
	        *(U8 *)(AD1 + 31 + pos + PS) = iTemp3;
	    }   
	    
	    *(U8 *)(AD1 + 32 + pos + PS) = 0x10;
	    *(U8 *)(AD1 + 33 + pos + PS) = 0x03;   

		usCalXOR = CalXor((U8 *)(AD1 + 1 + PS),33 + pos);		
		*(U8 *)(AD1 + 34 + pos + PS) = usCalXOR & 0xff;

//--��3��-----------------------------------------------------------------
		*(U8 *)(AD1 + 35 + pos + PS) = 0x10;

//--��4��-----------------------------------------------------------------
		*(U8 *)(AD1 + 36 + pos + PS) = 0x10;

//--��5��-----------------------------------------------------------------
		*(U8 *)(AD1 + 37 + pos + PS) = 0x10;

//--��6��-----------------------------------------------------------------
		*(U8 *)(AD1 + 38 + pos + PS) = 0x10;
		*(U8 *)(AD1 + 39 + pos + PS) = 0x02;

//--��7��-----------------------------------------------------------------
	    *(U8 *)(AD1 + 40 + pos + PS) = 0x00;
	    *(U8 *)(AD1 + 41 + pos + PS) = 0x0c;
	    *(U8 *)(AD1 + 42 + pos + PS) = 0x03;
	    *(U8 *)(AD1 + 43 + pos + PS) = 0x03;

	    *(U8 *)(AD1 + 44 + pos + PS) = 0xb0;
	    *(U8 *)(AD1 + 45 + pos + PS) = 0x01;
	    if (ptcom->Simens_Count == 0x10)
	    {
	        *(U8 *)(AD1 + 46 + pos + PS) = 0x10;
	        *(U8 *)(AD1 + 47 + pos + PS) = 0x10;
	        pos1++;
	    }
	    else
	    {
	        *(U8 *)(AD1 + 46 + pos + PS) = ptcom->Simens_Count & 0xff;
	    }

	    *(U8 *)(AD1 + 47 + pos + pos1 + PS) = 0x10;
	    *(U8 *)(AD1 + 48 + pos + pos1 + PS) = 0x03;

		usCalXOR = CalXor((U8 *)(AD1 + 40 + pos + PS),9 + pos1);		
		*(U8 *)(AD1 + 49 + pos + pos1 + PS) = usCalXOR & 0xff;		
		
	//1
		ptcom->send_length[7*i + 0] = 1;				//���ͳ���
		ptcom->send_staradd[7*i + 0] = 0 + PS;			//�������ݴ洢��ַ			
		ptcom->return_length[7*i + 0] = 1;				//�������ݳ���
		ptcom->return_start[7*i + 0] =0;				//����������Ч��ʼ
		ptcom->return_length_available[7*i + 0] = 0;	//������Ч���ݳ���	
	//2	
		ptcom->send_length[7*i + 1] = 34 + pos;			//���ͳ���
		ptcom->send_staradd[7*i + 1] = 1 + PS;			//�������ݴ洢��ַ			
		ptcom->return_length[7*i + 1] = 2;				//�������ݳ���
		ptcom->return_start[7*i + 1] = 0;				//����������Ч��ʼ
		ptcom->return_length_available[7*i + 1] = 0;	//������Ч���ݳ���	
	//3		
		ptcom->send_length[7*i + 2] = 1;				//���ͳ���
		ptcom->send_staradd[7*i + 2] = 35 + pos + PS;	//�������ݴ洢��ַ
		if (ptcom->Simens_Count == 0x10)
		{
			ptcom->return_length[7*i + 2] = 11;
		}	
		else
		{
			ptcom->return_length[7*i + 2] = 10;			//�������ݳ���
		}			
		ptcom->return_start[7*i + 2] = 0;				//����������Ч��ʼ
		ptcom->return_length_available[7*i + 2] = 0;	//������Ч���ݳ���	
	//4	
		ptcom->send_length[7*i + 3] = 1;				//���ͳ���
		ptcom->send_staradd[7*i + 3] = 36 + pos + PS;	//�������ݴ洢��ַ			
		ptcom->return_length[7*i + 3] = 1;				//�������ݳ���
		ptcom->return_start[7*i + 3] = 0;				//����������Ч��ʼ
		ptcom->return_length_available[7*i + 3] = 0;	//������Ч���ݳ���					
	//5
		ptcom->send_length[7*i + 4] = 1;				//���ͳ���
		ptcom->send_staradd[7*i + 4] = 37 + pos + PS;	//�������ݴ洢��ַ
		if (ptcom->Simens_Count == 0x10)
	    {
	        pos2++;
	    }
	    if (iLength*2 + 0x04 == 0x10)
	    {
	        pos2++;
	    }
	    if (iLength*2*8 == 0x10)
	    {
	        pos2++;
	    }
		ptcom->return_length[7*i + 4] = 27 + iLength*2;	//�������ݳ���				
		ptcom->return_start[7*i + 4] = 24 + pos2;		//����������Ч��ʼ
		ptcom->return_length_available[7*i + 4] = iLength*2;//������Ч���ݳ���	
	//6	
		ptcom->send_length[7*i + 5] = 2;				//���ͳ���
		ptcom->send_staradd[7*i + 5] = 38 + pos + PS;	//�������ݴ洢��ַ			
		ptcom->return_length[7*i + 5] = 1;				//�������ݳ���
		ptcom->return_start[7*i + 5] = 0;				//����������Ч��ʼ
		ptcom->return_length_available[7*i + 5] = 0;	//������Ч���ݳ���	
	//7		
		ptcom->send_length[7*i + 6] = 10 + pos1;		//���ͳ���
		ptcom->send_staradd[7*i + 6] = 40 + pos + PS;	//�������ݴ洢��ַ			
		ptcom->return_length[7*i + 6] = 1;				//�������ݳ���
		ptcom->return_start[7*i + 6] = 0;				//����������Ч��ʼ
		ptcom->return_length_available[7*i + 6] = 0;	//������Ч���ݳ���	
		
		PS = PS + 50 + pos + pos1;
		pos = 0;
		pos1 = 0;
		pos2 = 0;
		
		ptcom->Simens_Count++;
		if (ptcom->Simens_Count>255)
		{
			ptcom->Simens_Count=1;
		}		
	}
	ptcom->send_times=7*iSendTimes;						//���ʹ���
	ptcom->Current_Times=0;								//��ǰ���ʹ���	*/
	
	
	U16 aakj;
	int b,stationAdd;
	int a1,a2,a3,a4,a5;
	int YM_Check;
	int length;
	int i;
	int block;
	int datalength;
	int SendTimes;
	int LastTimeWord;
	int PS = 0;
	int k=0;
	int k1=0;
	int k2=0;

	datalength = ptcom->register_length;

	if(datalength>5000)
		datalength=5000;
	
	//ÿ������ܷ���32��D
	if(datalength%100==0)
	{
		SendTimes=datalength/100;
		LastTimeWord=100;//���һ�η��͵ĳ���	
	}
	if(datalength%100!=0)
	{
		SendTimes=datalength/100+1;//���͵Ĵ���
		LastTimeWord=datalength%100;//���һ�η��͵ĳ���	
	}
	
	
	for (i=0;i<SendTimes;i++)
	{			
		switch (ptcom->registerr)
		{	
		case 'R':	//MW
			YM_Check=0x83;
			break;
		case 'D':		//DB
			YM_Check=0x84;
			break;				
		}

		stationAdd=ptcom->plc_address;	//վ��ַ	
		b = ptcom->address + 100*i;		// ��λ��ַ
		a2 = b*2;							//�����ӵ�ַ���ֽ�Ϊ��λ,���������Ҫ����2��
		
		if (ptcom->registerr=='D')		//DB
		{
			block=a2/10000;				//����1000�����ǿ��ַ
			b=a2-block*10000;				//���ڵĵ�ַ
			a2=b;
			a4=(block>>8)&0xff;	//���ַ
			a5=block&0xff;
		}		
	
			b=a2*8;				//�����ӹ涨��ַҪ����8��
			a1=(b>>16)&0xff;
			a2=(b>>8)&0xff;
			a3=b&0xff;
		
//��ʼ��������
//1--------------------------------------------------------------------------------	
		*(U8 *)(AD1+0+PS) = 0x02;
//2--------------------------------------------------------------------------------				
		*(U8 *)(AD1+1+PS) = 0x00;
		*(U8 *)(AD1+2+PS) = 0x0c;
		*(U8 *)(AD1+3+PS) = 0x03;
		*(U8 *)(AD1+4+PS) = 0x03;	
		*(U8 *)(AD1+5+PS) = 0xf1;
		if (ptcom->Simens_Count == 0x10)
		{
			*(U8 *)(AD1+6+PS) = 0x10;
			*(U8 *)(AD1+7+PS) = 0x10;
			k++;
		}
		else
		{
			*(U8 *)(AD1+6+PS) = ptcom->Simens_Count;		//������֮��ķ��ʹ���
		}

		*(U8 *)(AD1+7+k+PS) = 0x32;
		*(U8 *)(AD1+8+k+PS) = 0x01;		
		*(U8 *)(AD1+9+k+PS) = 0x00;
		*(U8 *)(AD1+10+k+PS) = 0x00;	
		*(U8 *)(AD1+11+k+PS) = 0x33;
		*(U8 *)(AD1+12+k+PS) = 0x01;
		*(U8 *)(AD1+13+k+PS) = 0x00;		
		*(U8 *)(AD1+14+k+PS) = 0x0e;
		*(U8 *)(AD1+15+k+PS) = 0x00;
		*(U8 *)(AD1+16+k+PS) = 0x00;
		*(U8 *)(AD1+17+k+PS) = 0x04;
		*(U8 *)(AD1+18+k+PS) = 0x01;
		*(U8 *)(AD1+19+k+PS) = 0x12;
		*(U8 *)(AD1+20+k+PS) = 0x0a;		
		*(U8 *)(AD1+21+k+PS) = 0x10;
		*(U8 *)(AD1+22+k+PS) = 0x10;
		*(U8 *)(AD1+23+k+PS) = 0x02;			//���ֽڶ�ȡ
			
		if (i != (SendTimes-1))											//�������һ��ʱ
		{
			length = 100*2;
		}
		else
		{
			length = LastTimeWord*2;
		}
		
		if ((length>>8) & 0xff == 0x10)
		{
			*(U8 *)(AD1+24+k+PS) = 0x10;
			*(U8 *)(AD1+25+k+PS) = 0x10;
			k++;
		}
		else
		{
			*(U8 *)(AD1+24+k+PS) = (length>>8) & 0xff;
		}
		
		if (length & 0xff == 0x10)
		{
			*(U8 *)(AD1+25+k+PS) = 0x10;			//��ȡ����
			*(U8 *)(AD1+26+k+PS) = 0x10;
			k++;
		}
		else
		{
			*(U8 *)(AD1+25+k+PS) = length &0xff;
		}
			
		if (ptcom->registerr == 'D')		//DB,���ַ
		{
			if (a4 == 0x10)
			{
				*(U8 *)(AD1+26+k+PS) = 0x10;
				*(U8 *)(AD1+27+k+PS) = 0x10;
				k++;
			}
			else
			{
				*(U8 *)(AD1+26+k+PS) = a4 & 0xff;
			}
			
			if (a5 == 0x10)
			{
				*(U8 *)(AD1+27+k+PS) = 0x10;
				*(U8 *)(AD1+28+k+PS) = 0x10;
				k++;
			}
			else
			{
				*(U8 *)(AD1+27+k+PS) = a5 & 0xff;
			}
		}
		else
		{
			*(U8 *)(AD1+26+k+PS) = 0x00;
			*(U8 *)(AD1+27+k+PS) = 0x00;
		}

		*(U8 *)(AD1+28+k+PS) = YM_Check;
		
		if (a1 == 0x10)
		{
			*(U8 *)(AD1+29+k+PS) = 0x10;
			*(U8 *)(AD1+30+k+PS) = 0x10;
			k++;
		}
		else
		{
			*(U8 *)(AD1+29+k+PS) = a1 & 0xff;
		}
		
		if (a2 == 0x10)
		{
			*(U8 *)(AD1+30+k+PS) = 0x10;
			*(U8 *)(AD1+31+k+PS) = 0x10;
			k++;
		}
		else
		{
			*(U8 *)(AD1+30+k+PS) = a2 & 0xff;
		}
		
		if (a3 == 0x10)
		{
			*(U8 *)(AD1+31+k+PS) = 0x10;
			*(U8 *)(AD1+32+k+PS) = 0x10;
			k++;
		}
		else
		{
			*(U8 *)(AD1+31+k+PS) = a3 & 0xff;
		}
		
		*(U8 *)(AD1+32+k+PS) = 0x10;
		*(U8 *)(AD1+33+k+PS) = 0x03;
		
		k1 = k;
		
		aakj = CalXor((U8 *)(AD1+1),33+k+PS);//���У��
		*(U8 *)(AD1+34+k+PS) = aakj & 0xff;
//3--------------------------------------------------------------------------------			
		*(U8 *)(AD1+35+k+PS) = 0x10;
//4--------------------------------------------------------------------------------			
		*(U8 *)(AD1+36+k+PS) = 0x10;
//5--------------------------------------------------------------------------------			
		*(U8 *)(AD1+37+k+PS) = 0x10;
//6--------------------------------------------------------------------------------			
		*(U8 *)(AD1+38+k+PS) = 0x10;
		*(U8 *)(AD1+39+k+PS) = 0x02;
//7--------------------------------------------------------------------------------			
		*(U8 *)(AD1+40+k+PS) = 0x00;
		*(U8 *)(AD1+41+k+PS) = 0x0c;
		*(U8 *)(AD1+42+k+PS) = 0x03;
		*(U8 *)(AD1+43+k+PS) = 0x03;
		*(U8 *)(AD1+44+k+PS) = 0xb0;
		*(U8 *)(AD1+45+k+PS) = 0x01;
		
		if (ptcom->Simens_Count == 0x10)
		{
			*(U8 *)(AD1+46+k+PS) = 0x10;
			*(U8 *)(AD1+47+k+PS) = 0x10;
			k++;
			k2++;
		}
		else
		{
			*(U8 *)(AD1+46+k+PS) = ptcom->Simens_Count & 0xff;
		}
		
		*(U8 *)(AD1+47+k+PS) = 0x10;
		*(U8 *)(AD1+48+k+PS) = 0x03;
		
		aakj = CalXor((U8 *)(AD1+40+k+PS),9+k2);
		*(U8 *)(AD1+49+k+PS) = aakj & 0xff;	
//1-------------------------------------------------------------------		
		ptcom->send_length[i*7] = 1;				//���ͳ���
		ptcom->send_staradd[i*7] = 0+PS;			//�������ݴ洢��ַ	
		ptcom->return_length[i*7] = 1;				//�������ݳ���
		ptcom->return_start[i*7] = 0;				//����������Ч��ʼ
		ptcom->return_length_available[i*7] = 0;	//������Ч���ݳ���
				
//2-------------------------------------------------------------------
		ptcom->send_length[i*7+1] = 34+k1;			//���ͳ���
		ptcom->send_staradd[i*7+1] = 1+PS;			//�������ݴ洢��ַ			
		ptcom->return_length[i*7+1] = 2;			//�������ݳ���
		ptcom->return_start[i*7+1] = 0;				//����������Ч��ʼ
		ptcom->return_length_available[i*7+1]=0;	//������Ч���ݳ���
			
//3-------------------------------------------------------------------	
		ptcom->send_length[i*7+2] = 1;				//���ͳ���
		ptcom->send_staradd[i*7+2] = 35+k1+PS;		//�������ݴ洢��ַ			
		ptcom->return_length[i*7+2]=10;				//�������ݳ���
		if (ptcom->Simens_Count == 0x10)			//���ص������У�ֻҪ�ǳ���10�����ݣ����÷�2�Σ����Է�������Ҫƫ��
		{
			ptcom->return_length[i*7+2] = ptcom->return_length[i*7+2] + 1;		
		}		
		ptcom->return_start[i*7+2] = 0;				//����������Ч��ʼ
		ptcom->return_length_available[i*7+2] = 0;	//������Ч���ݳ���
			
//4-------------------------------------------------------------------			
		ptcom->send_length[i*7+3] = 1;				//���ͳ���
		ptcom->send_staradd[i*7+3] = 36+k1+PS;		//�������ݴ洢��ַ			
		ptcom->return_length[i*7+3] = 1;			//�������ݳ���
		ptcom->return_start[i*7+3] = 0;				//����������Ч��ʼ
		ptcom->return_length_available[i*7+3] = 0;	//������Ч���ݳ���			
				
//5-------------------------------------------------------------------
		ptcom->send_length[i*7+4] = 1;				//���ͳ���
		ptcom->send_staradd[i*7+4] = 37+k1+PS;		//�������ݴ洢��ַ
		ptcom->send_add[i*7+4] = ptcom->address + i*100;//�����������ַ������	
		ptcom->send_data_length[i*7+4] = length/2;	//�������һ�ζ���32��D			
		ptcom->return_length[i*7+4] = 24+length+3;	//�������ݳ��ȣ�24���̶�+����+10+03+У��,���ص����ݴ洢�ڴ�
		ptcom->return_start[i*7+4] = 24;			//����������Ч��ʼ
		if (ptcom->Simens_Count == 0x10)			//���ص������У�ֻҪ�ǳ���10�����ݣ����÷�2�Σ����Է�������Ҫƫ��
		{
			ptcom->return_length[i*7+4] = ptcom->return_length[i*7+4] + 1;
			ptcom->return_start[i*7+4] = ptcom->return_start[i*7+4] + 1;//����������Ч��ʼ			
		}	
		if (length==0x0c)							//���·����������ֽ�+4=0x10
		{
			ptcom->return_length[i*7+4] = ptcom->return_length[i*7+4] + 1;
			ptcom->return_start[i*7+4] = ptcom->return_start[i*7+4] + 1;//����������Ч��ʼ			
		}	
		if (length==0x02)							//���·����������ֽ�*8=0x10
		{
			ptcom->return_length[i*7+4] = ptcom->return_length[i*7+4] + 1;
			ptcom->return_start[i*7+4] = ptcom->return_start[i*7+4] + 1;//����������Ч��ʼ			
		}				
		ptcom->return_length_available[i*7+4] = length;//������Ч���ݳ���
			
				
//6-------------------------------------------------------------------
		ptcom->send_length[i*7+5] = 2;				//���ͳ���
		ptcom->send_staradd[i*7+5] = 38+k1+PS;		//�������ݴ洢��ַ			
		ptcom->return_length[i*7+5] = 1;			//�������ݳ���
		ptcom->return_start[i*7+5] = 0;				//����������Ч��ʼ
		ptcom->return_length_available[i*7+5] = 0;	//������Ч���ݳ���
			
//7-------------------------------------------------------------------		
		ptcom->send_length[i*7+6] = 10+k2;			//���ͳ���
		ptcom->send_staradd[i*7+6] = 40+k1+k2+PS;	//�������ݴ洢��ַ			
		ptcom->return_length[i*7+6] = 1;			//�������ݳ���	
		ptcom->return_start[i*7+6] = 0;				//����������Ч��ʼ
		ptcom->return_length_available[i*7+6] = 0;	//������Ч���ݳ���	
		
//----------------------------------------------------------------------------------------------------------------------
		PS = 50+k+PS;
		k=0;
		k1=0;
		k2=0;
		
		ptcom->Simens_Count++;	
		if (ptcom->Simens_Count>255)
		{
			ptcom->Simens_Count=1;
		}	
	}	
	ptcom->send_times = SendTimes*7;				//���ʹ���
	ptcom->Current_Times = 0;						//��ǰ���ʹ���	
			
}

/*************************************************************************
 * Funciton: ģ����д��
 * Parameters: ��
 * Return: ��
**************************************************************************/
void Write_Analog()								
{
/*	U16 usCalXOR;
	int iSendAdd;
	int iPlcStation;
	int iLength;
	int iBlockNo;
	int iWordNo;
	int iRegCode;
	int pos = 0;
	int pos1 = 0;
	int iTemp1,iTemp2,iTemp3;
	int i;
	
	iPlcStation = ptcom->plc_address;
	iSendAdd = ptcom->address;
	iLength = ptcom->register_length;

	switch (ptcom->registerr)
	{
	case 'R'://MW
		iBlockNo = 0;
		iWordNo = iSendAdd*2;
		
		iRegCode = 0x83;
		break;
	case 'D'://DB
		iBlockNo = (iSendAdd*2)/10000;
        iWordNo = (iSendAdd*2)%1000;
        
        iRegCode = 0x84;
		break;
	}
	
//--��1��-----------------------------------------------------------------
	*(U8 *)(AD1 + 0) = 0x02;

//--��2��-----------------------------------------------------------------
	*(U8 *)(AD1 + 1) = 0x00;
	*(U8 *)(AD1 + 2) = 0x0c;
	*(U8 *)(AD1 + 3) = 0x03;
	*(U8 *)(AD1 + 4) = 0x03;
		
	*(U8 *)(AD1 + 5) = 0xf1;
	if (ptcom->Simens_Count == 0x10)
	{
		*(U8 *)(AD1 + 6) = 0x10;
		*(U8 *)(AD1 + 7) = 0x10;
		pos++;
	}
	else
	{
		*(U8 *)(AD1 + 6) = ptcom->Simens_Count & 0xff;
	}
	*(U8 *)(AD1 + 7 + pos) = 0x32;
	*(U8 *)(AD1 + 8 + pos) = 0x01;

	*(U8 *)(AD1 + 9 + pos) = 0x00;
	*(U8 *)(AD1 + 10 + pos) = 0x00;
	*(U8 *)(AD1 + 11 + pos) = 0x44;
	*(U8 *)(AD1 + 12 + pos) = 0x01;

	*(U8 *)(AD1 + 13 + pos) = 0x00;
	*(U8 *)(AD1 + 14 + pos) = 0x0e;
	*(U8 *)(AD1 + 15 + pos) = 0x00;
    iTemp1 = 0x04 + iLength*2;
    if (iTemp1 == 0x10)
    {
        *(U8 *)(AD1 + 16 + pos) = 0x10;
        *(U8 *)(AD1 + 17 + pos)  = 0x10;
        pos++;
    }
    else
    {
        *(U8 *)(AD1 + 16 + pos)  = iTemp1;
    }

	*(U8 *)(AD1 + 17 + pos) = 0x05;
	*(U8 *)(AD1 + 18 + pos) = 0x01;
	*(U8 *)(AD1 + 19 + pos) = 0x12;
	*(U8 *)(AD1 + 20 + pos) = 0x0a;

	*(U8 *)(AD1 + 21 + pos) = 0x10;
	*(U8 *)(AD1 + 22 + pos) = 0x10;
	*(U8 *)(AD1 + 23 + pos) = 0x02;
    iTemp1 = ((iLength*2) >> 8) & 0xff;
    iTemp2 = ((iLength*2) >> 0) & 0xff;
    if (iTemp1 == 0x10)
    {
        *(U8 *)(AD1 + 24 + pos) = 0x10;
        *(U8 *)(AD1 + 25 + pos) = 0x10;
        pos++;
    }
    else
    {
        *(U8 *)(AD1 + 24 + pos) = iTemp1;
    }

    if (iTemp2 == 0x10)
    {
        *(U8 *)(AD1 + 25 + pos) = 0x10;
        *(U8 *)(AD1 + 26 + pos) = 0x10;
        pos++;
    }
    else
    {
        *(U8 *)(AD1 + 25 + pos) = iTemp2;
    }

    iTemp1 = (iBlockNo >> 8) & 0xff;
    iTemp2 = (iBlockNo >> 0) & 0xff;
    if (iTemp1 == 0x10)
    {
        *(U8 *)(AD1 + 26 + pos) = 0x10;
        *(U8 *)(AD1 + 27 + pos) = 0x10;
        pos++;
    }
    else
    {
        *(U8 *)(AD1 + 26 + pos) = iTemp1;
    }

    if (iTemp2 == 0x10)
    {
        *(U8 *)(AD1 + 27 + pos) = 0x10;
        *(U8 *)(AD1 + 28 + pos) = 0x10;
        pos++;
    }
    else
    {
        *(U8 *)(AD1 + 27 + pos) = iTemp2;
    }

	*(U8 *)(AD1 + 28 + pos) = iRegCode & 0xff;

    iTemp1 = ((iWordNo*8) >> 16) & 0xff;
    iTemp2 = ((iWordNo*8) >> 8) & 0xff;
    iTemp3 = ((iWordNo*8) >> 0) & 0xff;
    if (iTemp1 == 0x10)
    {
        *(U8 *)(AD1 + 29 + pos) = 0x10;
        *(U8 *)(AD1 + 30 + pos) = 0x10;
        pos++;
    }
    else
    {
        *(U8 *)(AD1 + 29 + pos) = iTemp1;
    }

    if (iTemp2 == 0x10)
    {
        *(U8 *)(AD1 + 30 + pos) = 0x10;
        *(U8 *)(AD1 + 31 + pos) = 0x10;
        pos++;
    }
    else
    {
        *(U8 *)(AD1 + 30 + pos) = iTemp2;
    }

    if (iTemp3 == 0x10)
    {
        *(U8 *)(AD1 + 31 + pos) = 0x10;
        *(U8 *)(AD1 + 32 + pos) = 0x10;
        pos++;
    }
    else
    {
        *(U8 *)(AD1 + 31 + pos) = iTemp3;
    }   
    
    *(U8 *)(AD1 + 32 + pos) = 0x30;
    *(U8 *)(AD1 + 33 + pos) = 0x04;
    iTemp1 = ((iLength*2*8) >> 8) & 0xff;
    iTemp2 = ((iLength*2*8) >> 0) & 0xff;
    if (iTemp1 == 0x10)
    {
        *(U8 *)(AD1 + 34 + pos) = 0x10;
        *(U8 *)(AD1 + 35 + pos) = 0x10;
        pos++;
    }
    else
    {
        *(U8 *)(AD1 + 34 + pos) = iTemp1;
    }
    
    if (iTemp2 == 0x10)
    {
        *(U8 *)(AD1 + 35 + pos) = 0x10;
        *(U8 *)(AD1 + 36 + pos) = 0x10;
        pos++;
    }
    else
    {
        *(U8 *)(AD1 + 35 + pos) = iTemp2;
    }  
    
    for (i=0; i<iLength*2; i++)
    {
    	if ((i+1) == iLength*2 && i%2 == 0)
    	{
    		iTemp1 = ptcom->U8_Data[i];	
    	}
    	else if (i%2 == 0)
    	{
    		iTemp1 = ptcom->U8_Data[i+1];
    	}
    	else
    	{
    		iTemp1 = ptcom->U8_Data[i-1];
    	}
    	
	    if (iTemp1 == 0x10)
	    {
	        *(U8 *)(AD1 + 36 + pos + i) = 0x10;
	        *(U8 *)(AD1 + 37 + pos + i) = 0x10;
	        pos++;
	    }
	    else
	    {
	        *(U8 *)(AD1 + 36 + pos + i) = iTemp1;
	    }
    }
       
	*(U8 *)(AD1 + 36 + pos + iLength*2) = 0x10;
	*(U8 *)(AD1 + 37 + pos + iLength*2) = 0x03;

	usCalXOR = CalXor((U8 *)(AD1 + 1),37 + pos + iLength*2);		
	*(U8 *)(AD1 + 38 + pos + iLength*2) = usCalXOR & 0xff;

//--��3��-----------------------------------------------------------------
	*(U8 *)(AD1 + 39 + pos + iLength*2) = 0x10;

//--��4��-----------------------------------------------------------------
	*(U8 *)(AD1 + 40 + pos + iLength*2) = 0x10;

//--��5��-----------------------------------------------------------------
	*(U8 *)(AD1 + 41 + pos + iLength*2) = 0x10;

//--��6��-----------------------------------------------------------------
	*(U8 *)(AD1 + 42 + pos + iLength*2) = 0x10;
	*(U8 *)(AD1 + 43 + pos + iLength*2) = 0x02;

//--��7��-----------------------------------------------------------------
    *(U8 *)(AD1 + 44 + pos + iLength*2) = 0x00;
    *(U8 *)(AD1 + 45 + pos + iLength*2) = 0x0c;
    *(U8 *)(AD1 + 46 + pos + iLength*2) = 0x03;
    *(U8 *)(AD1 + 47 + pos + iLength*2) = 0x03;

    *(U8 *)(AD1 + 48 + pos + iLength*2) = 0xb0;
    *(U8 *)(AD1 + 49 + pos + iLength*2) = 0x01;
    if (ptcom->Simens_Count == 0x10)
    {
        *(U8 *)(AD1 + 50 + pos + iLength*2) = 0x10;
        *(U8 *)(AD1 + 51 + pos + iLength*2) = 0x10;
        pos1++;
    }
    else
    {
        *(U8 *)(AD1 + 50 + pos) = ptcom->Simens_Count & 0xff;
    }

    *(U8 *)(AD1 + 51 + pos + pos1 + iLength*2) = 0x10;
    *(U8 *)(AD1 + 52 + pos + pos1 + iLength*2) = 0x03;

	usCalXOR = CalXor((U8 *)(AD1 + 44 + pos + iLength*2),9 + pos1);		
	*(U8 *)(AD1 + 53 + pos + pos1 + iLength*2) = usCalXOR & 0xff;
	
//1
	ptcom->send_length[0]=1;							//���ͳ���
	ptcom->send_staradd[0]=0;							//�������ݴ洢��ַ			
	ptcom->return_length[0]=1;							//�������ݳ���
	ptcom->return_start[0]=0;							//����������Ч��ʼ
	ptcom->return_length_available[0]=0;				//������Ч���ݳ���	
//2	
	ptcom->send_length[1]=38 + pos + iLength*2;			//���ͳ���
	ptcom->send_staradd[1]=1;							//�������ݴ洢��ַ			
	ptcom->return_length[1]=2;							//�������ݳ���
	ptcom->return_start[1]=0;							//����������Ч��ʼ
	ptcom->return_length_available[1]=0;				//������Ч���ݳ���	
//3		
	ptcom->send_length[2]=1;							//���ͳ���
	ptcom->send_staradd[2]=39 + pos + iLength*2;		//�������ݴ洢��ַ
	if (ptcom->Simens_Count == 0x10)
	{
		ptcom->return_length[2]=11;
	}	
	else
	{
		ptcom->return_length[2]=10;						//�������ݳ���
	}			
	ptcom->return_start[2]=0;							//����������Ч��ʼ
	ptcom->return_length_available[2]=0;				//������Ч���ݳ���	
//4	
	ptcom->send_length[3]=1;							//���ͳ���
	ptcom->send_staradd[3]=40 + pos + iLength*2;		//�������ݴ洢��ַ			
	ptcom->return_length[3]=1;							//�������ݳ���
	ptcom->return_start[3]=0;							//����������Ч��ʼ
	ptcom->return_length_available[3]=0;				//������Ч���ݳ���					
//5
	ptcom->send_length[4]=1;							//���ͳ���
	ptcom->send_staradd[4]=41 + pos + iLength*2;		//�������ݴ洢��ַ
	if (ptcom->Simens_Count == 0x10)
    {
        ptcom->return_length[4]=25;						//�������ݳ���
    }
    else
	{
		ptcom->return_length[4]=24;						//�������ݳ���
	}			
	ptcom->return_start[4]=0;							//����������Ч��ʼ
	ptcom->return_length_available[4]=0;				//������Ч���ݳ���	
//6	
	ptcom->send_length[5]=2;							//���ͳ���
	ptcom->send_staradd[5]=42 + pos + iLength*2;		//�������ݴ洢��ַ			
	ptcom->return_length[5]=1;							//�������ݳ���
	ptcom->return_start[5]=0;							//����������Ч��ʼ
	ptcom->return_length_available[5]=0;				//������Ч���ݳ���	
//7		
	ptcom->send_length[6]=10 + pos1;					//���ͳ���
	ptcom->send_staradd[6]=44 + pos + iLength*2;		//�������ݴ洢��ַ			
	ptcom->return_length[6]=1;							//�������ݳ���
	ptcom->return_start[6]=0;							//����������Ч��ʼ
	ptcom->return_length_available[6]=0;				//������Ч���ݳ���		
	
	ptcom->send_times=7;								//���ʹ���
	ptcom->Current_Times=0;								//��ǰ���ʹ���	
	ptcom->Simens_Count++;	

	*(U8 *)(COMad+300)=ptcom->send_times;*/
	
		U16 aakj;
	int b,stationAdd;
	int a1,a2,a3,a4,a5;
	int YM_Check;
	int length;
	int i;
	int k1,k2,k3,k4,k5,k6,k7,k8,k9,k10,k11;
	int block;

	
	k1=0;
	k2=0;
	k3=0;
	k4=k5=k6=k7=k8=k9=k10=k11=0;
			
	switch (ptcom->registerr)
	{	
	case 'R':	//MW
		YM_Check=0x83;
		break;
	case 'D':		//DB
		YM_Check=0x84;
		break;				
	}
	
	length=ptcom->register_length*2;//��ȡ����,�����ӵ�һ��DB�������ֽ�Ϊ��λ
	stationAdd=ptcom->plc_address;	//վ��ַ	
	b=ptcom->address;			// ��λ��ַ
	a2=b*2;						//�����ӵ�ַ���ֽ�Ϊ��λ,���������Ҫ����2��
		
	a4=0;
	a5=0;
	if (ptcom->registerr=='D')		//DB
	{
		block=a2/10000;				//����1000�����ǿ��ַ
		b=a2-block*10000;				//���ڵĵ�ַ
		a2=b;	
		a4=(block>>8)&0xff;	//���ַ
		a5=block&0xff;	
	}
	
			
	b=a2*8;				//�����ӹ涨��ַҪ����8��
	a1=(b>>16)&0xff;
	a2=(b>>8)&0xff;
	a3=b&0xff;	
	
//��ʼ��������	
		*(U8 *)(AD1+0)=0x02;//1
		
		*(U8 *)(AD1+1)=0x00;//2
		*(U8 *)(AD1+2)=0x0c;
		*(U8 *)(AD1+3)=0x03;
		*(U8 *)(AD1+4)=0x03;	
		*(U8 *)(AD1+5)=0xf1;
		*(U8 *)(AD1+6)=ptcom->Simens_Count;		//������֮��ķ��ʹ���
		if (ptcom->Simens_Count==0x10)
		{
			*(U8 *)(AD1+7)=ptcom->Simens_Count;		//������֮��ķ��ʹ���		
			k1++;
		}
		*(U8 *)(AD1+7+k1)=0x32;
		*(U8 *)(AD1+8+k1)=0x01;		
		*(U8 *)(AD1+9+k1)=0x00;
		*(U8 *)(AD1+10+k1)=0x00;	
		*(U8 *)(AD1+11+k1)=0x44;
		*(U8 *)(AD1+12+k1)=0x01;
		*(U8 *)(AD1+13+k1)=0x00;		
		*(U8 *)(AD1+14+k1)=0x0e;
		*(U8 *)(AD1+15+k1)=0x00;
		*(U8 *)(AD1+16+k1)=length+4;		//���ͳ���+4
		if ((length+4)==0x10)
		{
			*(U8 *)(AD1+17+k1+k2)=length+4;			
			k2++;	
		}		
		
		*(U8 *)(AD1+17+k1+k2)=0x05;
		*(U8 *)(AD1+18+k1+k2)=0x01;
		*(U8 *)(AD1+19+k1+k2)=0x12;
		*(U8 *)(AD1+20+k1+k2)=0x0a;		
		*(U8 *)(AD1+21+k1+k2)=0x10;
		*(U8 *)(AD1+22+k1+k2)=0x10;
		*(U8 *)(AD1+23+k1+k2)=0x02;			//���ֽ�д��
		*(U8 *)(AD1+24+k1+k2)=0x00;
		*(U8 *)(AD1+25+k1+k2)=length;			//��ȡ����
		if (length==0x10)
		{
			*(U8 *)(AD1+26+k1+k2)=length;			
			k3++;	
		}		
		*(U8 *)(AD1+26+k1+k2+k3)=a4;
		if (a4==0x10)
		{
			*(U8 *)(AD1+27+k1+k2+k3)=a4;			
			k4++;	
		}		
		*(U8 *)(AD1+27+k1+k2+k3+k4)=a5;
		if (a5==0x10)
		{
			*(U8 *)(AD1+28+k1+k2+k3+k4)=a5;			
			k5++;	
		}	
		*(U8 *)(AD1+28+k1+k2+k3+k4+k5)=YM_Check;		//Q����M		
		*(U8 *)(AD1+29+k1+k2+k3+k4+k5)=a1;				//��ַ
		if (a1==0x10)
		{
			*(U8 *)(AD1+30+k1+k2+k3+k4+k5)=a1;			
			k6++;	
		}		
		*(U8 *)(AD1+30+k1+k2+k3+k4+k5+k6)=a2;
		if (a2==0x10)
		{
			*(U8 *)(AD1+31+k1+k2+k3+k4+k5+k6)=a2;			
			k7++;	
		}		
		*(U8 *)(AD1+31+k1+k2+k3+k4+k5+k6+k7)=a3;	
		if (a3==0x10)
		{
			*(U8 *)(AD1+32+k1+k2+k3+k3+k4+k5+k6+k7)=a3;			
			k8++;	
		}		
		*(U8 *)(AD1+32+k1+k2+k3+k4+k5+k6+k7+k8)=0x30;				//��ַ
		*(U8 *)(AD1+33+k1+k2+k3+k4+k5+k6+k7+k8)=0x04;
		*(U8 *)(AD1+34+k1+k2+k3+k4+k5+k6+k7+k8)=0x00;	
		
		*(U8 *)(AD1+35+k1+k2+k3+k4+k5+k6+k7+k8)=length*8;			//���ͳ���*8		
		if ((length*8)==0x10)
		{
			*(U8 *)(AD1+36+k1+k2+k3+k3+k4+k5+k6+k7+k8)=length*8;			
			k9++;	
		}		
		
		for (i=0;i<length;i++)
		{
			if ((i+1)==length && i%2==0)
			{
				*(U8 *)(AD1+36+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+i)=ptcom->U8_Data[i];		
			}
			else if (i%2==0)
			{
				*(U8 *)(AD1+36+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+i)=ptcom->U8_Data[i+1];			//�ߵ��ֽڽ���
			}
			else
			{
				*(U8 *)(AD1+36+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+i)=ptcom->U8_Data[i-1];		
			}	
			if (*(U8 *)(AD1+36+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+i)==0x10)
			{
				*(U8 *)(AD1+37+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+i)=0x10;
				k10++;
			}
		}
					
		*(U8 *)(AD1+36+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+length)=0x10;
		*(U8 *)(AD1+37+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+length)=0x03;
		aakj=CalXor((U8 *)AD1+1,37+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+length);	//���У��			
		*(U8 *)(AD1+38+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+length)=aakj&0xff;
		
		*(U8 *)(AD1+39+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+length)=0x10;//3
			
		*(U8 *)(AD1+40+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+length)=0x10;//4
											
		*(U8 *)(AD1+41+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+length)=0x10;//5
		
		*(U8 *)(AD1+42+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+length)=0x10;//6	
		*(U8 *)(AD1+43+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+length)=0x02;	
		
		*(U8 *)(AD1+44+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+length)=0x00;//7
		*(U8 *)(AD1+45+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+length)=0x0c;
		*(U8 *)(AD1+46+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+length)=0x03;
		*(U8 *)(AD1+47+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+length)=0x03;
		*(U8 *)(AD1+48+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+length)=0xb0;
		*(U8 *)(AD1+49+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+length)=0x01;	
		*(U8 *)(AD1+50+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+length)=ptcom->Simens_Count;	//������֮��ķ��ʹ���	
		if (ptcom->Simens_Count==0x10)
		{
			*(U8 *)(AD1+51+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+length)=ptcom->Simens_Count;			
			k11++;	
		}			
		*(U8 *)(AD1+51+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+k11+length)=0x10;
		*(U8 *)(AD1+52+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+k11+length)=0x03;
		aakj=CalXor((U8 *)AD1+44+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+length,9+k11);	//���У��		
		*(U8 *)(AD1+53+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+k11+length)=aakj&0xff;
		

	//1
		ptcom->send_length[0]=1;				//���ͳ���
		ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ			
		ptcom->return_length[0]=1;				//�������ݳ���
		ptcom->return_start[0]=0;				//����������Ч��ʼ
		ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
	//2
		
		ptcom->send_length[1]=38+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+length;				//���ͳ���
		ptcom->send_staradd[1]=1;				//�������ݴ洢��ַ			
		ptcom->return_length[1]=2;				//�������ݳ���
		ptcom->return_start[1]=0;				//����������Ч��ʼ
		ptcom->return_length_available[1]=0;	//������Ч���ݳ���	
	//3		
		ptcom->send_length[2]=1;				//���ͳ���
		ptcom->send_staradd[2]=39+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+length;		//�������ݴ洢��ַ			
		ptcom->return_length[2]=10;				//�������ݳ���
		if (ptcom->Simens_Count==0x10)			//���ص������У�ֻҪ�ǳ���10�����ݣ����÷�2�Σ����Է�������Ҫƫ��
		{
			ptcom->return_length[2]=ptcom->return_length[2]+1;		
		}		
		ptcom->return_start[2]=0;				//����������Ч��ʼ
		ptcom->return_length_available[2]=0;	//������Ч���ݳ���	
	//4
		
		ptcom->send_length[3]=1;					//���ͳ���
		ptcom->send_staradd[3]=40+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+length;				//�������ݴ洢��ַ			
		ptcom->return_length[3]=1;				//�������ݳ���
		ptcom->return_start[3]=0;				//����������Ч��ʼ
		ptcom->return_length_available[3]=0;	//������Ч���ݳ���			
			
	//5
		ptcom->send_length[4]=1;				//���ͳ���
		ptcom->send_staradd[4]=41+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+length;			//�������ݴ洢��ַ			
		ptcom->return_length[4]=24;				//�������ݳ���
		ptcom->return_start[4]=0;				//����������Ч��ʼ
		if (ptcom->Simens_Count==0x10)			//���ص������У�ֻҪ�ǳ���10�����ݣ����÷�2�Σ����Է�������Ҫƫ��
		{
			ptcom->return_length[4]=ptcom->return_length[4]+1;
			ptcom->return_start[4]=ptcom->return_start[4]+1;				//����������Ч��ʼ			
		}				
		ptcom->return_length_available[4]=0;	//������Ч���ݳ���	
	//6
		
		ptcom->send_length[5]=2;				//���ͳ���
		ptcom->send_staradd[5]=42+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+length;				//�������ݴ洢��ַ			
		ptcom->return_length[5]=1;				//�������ݳ���
		ptcom->return_start[5]=0;				//����������Ч��ʼ
		ptcom->return_length_available[5]=0;	//������Ч���ݳ���	
	//7		
		ptcom->send_length[6]=10+k11;				//���ͳ���
		ptcom->send_staradd[6]=44+k1+k2+k3+k3+k4+k5+k6+k7+k8+k9+k10+length;				//�������ݴ洢��ַ			
		ptcom->return_length[6]=1;				//�������ݳ���	
		ptcom->return_start[6]=0;				//����������Ч��ʼ
		ptcom->return_length_available[6]=0;	//������Ч���ݳ���	
	
	
	ptcom->send_times=7;					//���ʹ���
	ptcom->Current_Times=0;					//��ǰ���ʹ���
	
	*(U8 *)(COMad+300)=ptcom->send_times;
	
	ptcom->Simens_Count++;		
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
	int LastTimeWord;
	int stationAdd;
	U16 aakj;
	int b;
	int a1,a2,a3,a4,a5;
	int YM_Check;
	int length;
	int i,j;
	int k1 = 0;
	int k2 = 0;
	int block;
	int PS = 0;
	int k = 0;

	datalength = ((*(U8 *)(PE+0))>>8)+(*(U8 *)(PE+1));											//���ݳ���
	staradd = ((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));	//���ݳ���
	stationAdd = *(U8 *)(PE+4);																	//վ��ַ

	if(datalength%100 == 0)
	{
		SendTimes = datalength/100;
		LastTimeWord = 100;																		//���һ�η��͵ĳ���	
	}
	if(datalength%100 != 0)
	{
		SendTimes = datalength/100 + 1;															//���͵Ĵ���
		LastTimeWord = datalength%100;															//���һ�η��͵ĳ���	
	}	

	for (i=0; i<SendTimes;i++)
	{
		switch (*(U8 *)(PE+3))
		{	
		case 'R':																				//MW
			YM_Check=0x83;
			break;
		case 'D':																				//DB
			YM_Check=0x84;
			break;				
		}
		
		if (i != (SendTimes-1))																	//�������һ��ʱ
		{
			length = 100;
		}
		else
		{
			length = LastTimeWord;
		}
		
		length = length*2;																		//��ȡ����,�����ӵ�һ��DB�������ֽ�Ϊ��λ	
		b = staradd + 100*i;																		// ��λ��ַ
		a2=b*2;																					//�����ӵ�ַ���ֽ�Ϊ��λ,���������Ҫ����2��
			
		a4=0;
		a5=0;
		if (*(U8 *)(PE+3) == 'D')																//DB
		{
			block=a2/10000;																		//����1000�����ǿ��ַ
			b=a2-block*10000;																	//���ڵĵ�ַ
			a2=b;	
			a4=(block>>8)&0xff;																	//���ַ
			a5=block&0xff;	
		}
				
		b=a2*8;																					//�����ӹ涨��ַҪ����8��
		a1=(b>>16)&0xff;
		a2=(b>>8)&0xff;
		a3=b&0xff;	
//1------------------------------------------------------------------------------------------------------------------
		*(U8 *)(AD1+0+PS) = 0x02;
		
//2------------------------------------------------------------------------------------------------------------------
		*(U8 *)(AD1+1+PS) = 0x00;
		*(U8 *)(AD1+2+PS) = 0x0c;
		*(U8 *)(AD1+3+PS) = 0x03;
		*(U8 *)(AD1+4+PS) = 0x03;	
		*(U8 *)(AD1+5+PS) = 0xf1;
		if (ptcom->Simens_Count == 0x10)
		{
			*(U8 *)(AD1+6+PS) = 0x10;
			*(U8 *)(AD1+7+PS) = 0x10;
			k++;
		}
		else
		{
			*(U8 *)(AD1+6+PS) = ptcom->Simens_Count;											//������֮��ķ��ʹ���
		}
		*(U8 *)(AD1+7+k+PS) = 0x32;
		*(U8 *)(AD1+8+k+PS) = 0x01;		
		*(U8 *)(AD1+9+k+PS) = 0x00;
		*(U8 *)(AD1+10+k+PS) = 0x00;	
		*(U8 *)(AD1+11+k+PS) = 0x44;
		*(U8 *)(AD1+12+k+PS) = 0x01;
		*(U8 *)(AD1+13+k+PS) = 0x00;		
		*(U8 *)(AD1+14+k+PS) = 0x0e;
		*(U8 *)(AD1+15+k+PS) = 0x00;
		if ((length+4) == 0x10)
		{
			*(U8 *)(AD1+16+k+PS) = 0x10;
			*(U8 *)(AD1+17+k+PS) = 0x10;
			k++;
		}
		else
		{
			*(U8 *)(AD1+16+k+PS) = length+4;													//���ͳ���+4
		}
		*(U8 *)(AD1+17+k+PS) = 0x05;
		*(U8 *)(AD1+18+k+PS) = 0x01;
		*(U8 *)(AD1+19+k+PS) = 0x12;
		*(U8 *)(AD1+20+k+PS) = 0x0a;		
		*(U8 *)(AD1+21+k+PS) = 0x10;
		*(U8 *)(AD1+22+k+PS) = 0x10;
		*(U8 *)(AD1+23+k+PS) = 0x02;			
		*(U8 *)(AD1+24+k+PS) = 0x00;
		if (length == 0x10)
		{
			*(U8 *)(AD1+25+k+PS) = 0x10;
			*(U8 *)(AD1+26+k+PS) = 0x10;
			k++;
		}
		else
		{
			*(U8 *)(AD1+25+k+PS) = length;														//���ͳ���
		}
		
		if (a4 == 0x10)
		{
			*(U8 *)(AD1+26+k+PS) = 0x10;
			*(U8 *)(AD1+27+k+PS) = 0x10;
			k++;
		}
		else
		{
			*(U8 *)(AD1+26+k+PS) = a4;														
		}	
		
		if (a5 == 0x10)
		{
			*(U8 *)(AD1+27+k+PS) = 0x10;
			*(U8 *)(AD1+28+k+PS) = 0x10;
			k++;
		}
		else
		{
			*(U8 *)(AD1+27+k+PS) = a5;														
		}
		
		*(U8 *)(AD1+28+k+PS) = YM_Check;														//Q����M
		
		if (a1 == 0x10)
		{
			*(U8 *)(AD1+29+k+PS) = 0x10;
			*(U8 *)(AD1+30+k+PS) = 0x10;
			k++;
		}
		else
		{
			*(U8 *)(AD1+29+k+PS) = a1;														
		}
		
		if (a2 == 0x10)
		{
			*(U8 *)(AD1+30+k+PS) = 0x10;
			*(U8 *)(AD1+31+k+PS) = 0x10;
			k++;
		}
		else
		{
			*(U8 *)(AD1+30+k+PS) = a2;														
		}
		
		if (a3 == 0x10)
		{
			*(U8 *)(AD1+31+k+PS) = 0x10;
			*(U8 *)(AD1+32+k+PS) = 0x10;
			k++;
		}
		else
		{
			*(U8 *)(AD1+31+k+PS) = a3;														
		}
		
		*(U8 *)(AD1+32+k+PS) = 0x30;
		*(U8 *)(AD1+33+k+PS) = 0x04;
		
		if (((length*8)>>8) & 0xff == 0x10)
		{
			*(U8 *)(AD1+34+k+PS) = 0x10;
			*(U8 *)(AD1+35+k+PS) = 0x10;
			k++;
		}
		else
		{
			*(U8 *)(AD1+34+k+PS) = (length*8)>>8 & 0xff;
		}
		
		if ((length*8) & 0xff == 0x10)
		{
			*(U8 *)(AD1+35+k+PS) = 0x10;
			*(U8 *)(AD1+36+k+PS) = 0x10;
			k++;
		}
		else
		{
			*(U8 *)(AD1+35+k+PS) = length*8 & 0xff;														
		}
		
		for (j=0;j<length;j++)
		{
			if ((j+1) == length && j%2 == 0)
			{
				if (*(U8 *)(PE+9+i*200+j) == 0x10)
				{
					*(U8 *)(AD1+36+k+PS+j) = 0x10;
					*(U8 *)(AD1+37+k+PS+j) = 0x10;
					k++;
				}
				else
				{
					*(U8 *)(AD1+36+k+PS+j) = *(U8 *)(PE+9+i*200+j);
				}
			}
			else if (j%2 == 0)
			{
				if (*(U8 *)(PE+9+i*200+j+1) == 0x10)
				{
					*(U8 *)(AD1+36+k+PS+j) = 0x10;
					*(U8 *)(AD1+37+k+PS+j) = 0x10;
					k++;
				}
				else
				{
					*(U8 *)(AD1+36+k+PS+j) = *(U8 *)(PE+9+i*200+j+1);
				}			
			}
			else
			{
				if (*(U8 *)(PE+9+i*200+j-1) == 0x10)
				{
					*(U8 *)(AD1+36+k+PS+j) = 0x10;
					*(U8 *)(AD1+37+k+PS+j) = 0x10;
					k++;
				}
				else
				{
					*(U8 *)(AD1+36+k+PS+j) = *(U8 *)(PE+9+i*200+j-1);
				}				
			}
		}
		
		//ptcom->send_staradd[99] =  *(U8 *)(PE+9);
		//ptcom->send_staradd[98] =  *(U8 *)(PE+10);
		k1 = k;
		
		*(U8 *)(AD1+36+k+PS+length) = 0x10;	
		*(U8 *)(AD1+37+k+PS+length) = 0x03;	
		aakj = CalXor((U8 *)(AD1+1+PS),37+k+length);
		*(U8 *)(AD1+38+k+PS+length) = aakj & 0xff;												//���У��
		
//3------------------------------------------------------------------------------------------------------------------
		*(U8 *)(AD1+39+k+PS+length) = 0x10;
		
//4------------------------------------------------------------------------------------------------------------------
		*(U8 *)(AD1+40+k+PS+length) = 0x10;
		
//5------------------------------------------------------------------------------------------------------------------
		*(U8 *)(AD1+41+k+PS+length) = 0x10;
		
//6------------------------------------------------------------------------------------------------------------------
		*(U8 *)(AD1+42+k+PS+length) = 0x10;
		*(U8 *)(AD1+43+k+PS+length) = 0x02;
		
//7------------------------------------------------------------------------------------------------------------------
		*(U8 *)(AD1+44+k+PS+length) = 0x00;
		*(U8 *)(AD1+45+k+PS+length) = 0x0c;
		*(U8 *)(AD1+46+k+PS+length) = 0x03;
		*(U8 *)(AD1+47+k+PS+length) = 0x03;
		*(U8 *)(AD1+48+k+PS+length) = 0xb0;
		*(U8 *)(AD1+49+k+PS+length) = 0x01;
		
		if (ptcom->Simens_Count == 0x10)
		{
			*(U8 *)(AD1+50+k+PS+length) = 0x10;
			*(U8 *)(AD1+51+k+PS+length) = 0x10;
			k++;
			k2 = 1;
		}
		else
		{
			*(U8 *)(AD1+50+k+PS+length) = ptcom->Simens_Count;
		}
		
		*(U8 *)(AD1+51+k+PS+length) = 0x10;
		*(U8 *)(AD1+52+k+PS+length) = 0x03;
		
		aakj = CalXor((U8 *)(AD1+44+k+PS+length),9+k2);
		*(U8 *)(AD1+53+k+PS+length) = aakj & 0xff;
		
//1-------------------------------------------------------------------		
		ptcom->send_length[i*7] = 1;				//���ͳ���
		ptcom->send_staradd[i*7] = 0+PS;			//�������ݴ洢��ַ	
		ptcom->return_length[i*7] = 1;				//�������ݳ���
		ptcom->return_start[i*7] = 0;				//����������Ч��ʼ
		ptcom->return_length_available[i*7] = 0;	//������Ч���ݳ���
				
//2-------------------------------------------------------------------
		ptcom->send_length[i*7+1] = 38+k1+length;	//���ͳ���
		ptcom->send_staradd[i*7+1] = 1+PS;			//�������ݴ洢��ַ			
		ptcom->return_length[i*7+1] = 2;			//�������ݳ���
		ptcom->return_start[i*7+1] = 0;				//����������Ч��ʼ
		ptcom->return_length_available[i*7+1]=0;	//������Ч���ݳ���
			
//3-------------------------------------------------------------------	
		ptcom->send_length[i*7+2] = 1;				//���ͳ���
		ptcom->send_staradd[i*7+2] = 39+k1+PS+length;//�������ݴ洢��ַ			
		ptcom->return_length[i*7+2]=10;				//�������ݳ���
		if (ptcom->Simens_Count == 0x10)			//���ص������У�ֻҪ�ǳ���10�����ݣ����÷�2�Σ����Է�������Ҫƫ��
		{
			ptcom->return_length[i*7+2] = ptcom->return_length[i*7+2] + 1;		
		}		
		ptcom->return_start[i*7+2] = 0;				//����������Ч��ʼ
		ptcom->return_length_available[i*7+2] = 0;	//������Ч���ݳ���
			
//4-------------------------------------------------------------------			
		ptcom->send_length[i*7+3] = 1;				//���ͳ���
		ptcom->send_staradd[i*7+3] = 40+k1+PS+length;//�������ݴ洢��ַ			
		ptcom->return_length[i*7+3] = 1;			//�������ݳ���
		ptcom->return_start[i*7+3] = 0;				//����������Ч��ʼ
		ptcom->return_length_available[i*7+3] = 0;	//������Ч���ݳ���			
				
//5-------------------------------------------------------------------
		ptcom->send_length[i*7+4] = 1;				//���ͳ���
		ptcom->send_staradd[i*7+4] = 41+k1+PS+length;//�������ݴ洢��ַ			
		ptcom->return_length[i*7+4] = 24;			//�������ݳ��ȣ�24���̶�+����+10+03+У��,���ص����ݴ洢�ڴ�
		ptcom->return_start[i*7+4] = 0;				//����������Ч��ʼ
		if (ptcom->Simens_Count == 0x10)			//���ص������У�ֻҪ�ǳ���10�����ݣ����÷�2�Σ����Է�������Ҫƫ��
		{
			ptcom->return_length[i*7+4] = ptcom->return_length[i*7+4] + 1;
			ptcom->return_start[i*7+4] = ptcom->return_start[i*7+4] + 1;//����������Ч��ʼ			
		}				
		ptcom->return_length_available[i*7+4] = 0;//������Ч���ݳ���	
				
//6-------------------------------------------------------------------
		ptcom->send_length[i*7+5] = 2;				//���ͳ���
		ptcom->send_staradd[i*7+5] = 42+k1+PS+length;//�������ݴ洢��ַ			
		ptcom->return_length[i*7+5] = 1;			//�������ݳ���
		ptcom->return_start[i*7+5] = 0;				//����������Ч��ʼ
		ptcom->return_length_available[i*7+5] = 0;	//������Ч���ݳ���
			
//7-------------------------------------------------------------------		
		ptcom->send_length[i*7+6] = 10+k2;			//���ͳ���
		ptcom->send_staradd[i*7+6] = 44+k1+k2+PS+length;//�������ݴ洢��ַ			
		ptcom->return_length[i*7+6] = 1;			//�������ݳ���	
		ptcom->return_start[i*7+6] = 0;				//����������Ч��ʼ
		ptcom->return_length_available[i*7+6] = 0;	//������Ч���ݳ���	
		
//----------------------------------------------------------------------------------------------------------------------
		PS = 54+k+PS+length;
		k=0;
		k1=0;
		k2=0;
		
		ptcom->Simens_Count++;					
	}
	ptcom->send_times = SendTimes*7;				//���ʹ���
	ptcom->Current_Times = 0;						//��ǰ���ʹ���
	
	//*(U8 *)(COMad+300)=ptcom->send_times;	
}

/*************************************************************************
 * Funciton: �洢�ּĴ�����ֵ��unsigned char��������
 * Parameters: ��
 * Return: ��
**************************************************************************/
void compxy(void)				//����ɱ�׼�洢��ʽ
{
	int i,pos,iTemp;
	
	pos = 0;
	
	for(i=0; i<ptcom->return_length_available[ptcom->Current_Times-1]; i++)
	{
		iTemp = (*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1] + i + pos))&0xff;	
		if (iTemp == 0x10)
		{
			*(U8 *)(COMad+i) = 0x10;
			pos++;
		}
		else
		{
			*(U8 *)(COMad+i) = iTemp;
		}
	}
}

/*************************************************************************
 * Funciton: ��������������ռ��������ݽӿں���
 * Parameters: ��
 * Return: ��
**************************************************************************/
void watchcom(void)
{
	unsigned int iCheck = 0;
	iCheck = remark();

	if(iCheck == 1 && ptcom->IfRead == 1)
	{
		ptcom->IfResultCorrect=1;
		compxy();
	}
	else if (iCheck == 2)
	{
		ptcom->IfResultCorrect=0;
	}
	else
	{
		ptcom->IfResultCorrect=0;
	}
}

/*************************************************************************
 * Funciton: ��PLC���ص����ݽ�����Ӧ��У�飬�Ѽ�����ݵ���ȷ��
 * Parameters: ��
 * Return: 0:����
           1:��ȷ
           2:����Ҫ�����ݽ��д洢����
*************************************************************************/
int remark()
{
	int iCalXOR = 0;
    int iRcvXOR = 0;
    int iTemp1,iTemp2;
	
	if (ptcom->send_staradd[99] == 1)//���ַ���
	{
		switch (ptcom->Current_Times)
		{
		case 1:
			iTemp1 = *(U8 *)(COMad + 0)&0xff;
			if (iTemp1 == 0x10)
			{
				return 2;
			}
			break;
		case 2:
			iTemp1 = *(U8 *)(COMad + 0)&0xff;
			iTemp2 = *(U8 *)(COMad + 1)&0xff;
			if (iTemp1 == 0x10 && iTemp2 == 0x02)
			{
				return 2;
			}
			break;
		case 3:
			iTemp1 = *(U8 *)(COMad + 9)&0xff;
			iTemp2 = *(U8 *)(COMad + 10)&0xff;
			if (iTemp1 == 0x10 && iTemp2 == 0x03)
			{
				iRcvXOR = *(U8 *)(COMad + 11)&0xff;
				iCalXOR = CalXor((U8 *)COMad,11);
				if (iRcvXOR == iCalXOR)
				{
					return 2;
				}
			}
			break;
		case 4:
			iTemp1 = *(U8 *)(COMad + 0)&0xff;
			if (iTemp1 == 0x10)
			{
				return 2;
			}
			break;
		case 5:
			iTemp1 = *(U8 *)(COMad + 0)&0xff;
			iTemp2 = *(U8 *)(COMad + 1)&0xff;
			if (iTemp1 == 0x10 && iTemp2 == 0x02)
			{
				return 2;
			}
			break;
		case 6:
			iTemp1 = *(U8 *)(COMad + 20)&0xff;
			iTemp2 = *(U8 *)(COMad + 21)&0xff;
			if (iTemp1 == 0x10 && iTemp2 == 0x03)
			{
				iRcvXOR = *(U8 *)(COMad + 22)&0xff;
				iCalXOR = CalXor((U8 *)COMad,22);
				if (iRcvXOR == iCalXOR)
				{
					return 2;
				}
			}
			break;
		case 7:
			iTemp1 = *(U8 *)(COMad + 0)&0xff;
			if (iTemp1 == 0x10)
			{
				return 2;
			}
			break;
		case 8:
			iTemp1 = *(U8 *)(COMad + 0)&0xff;
			iTemp2 = *(U8 *)(COMad + 1)&0xff;
			if (iTemp1 == 0x10 && iTemp2 == 0x02)
			{
				return 2;
			}
			break;
		case 9:
			iTemp1 = *(U8 *)(COMad + 6)&0xff;
			iTemp2 = *(U8 *)(COMad + 7)&0xff;
			if (iTemp1 == 0x10 && iTemp2 == 0x03)
			{
				iRcvXOR = *(U8 *)(COMad + 8)&0xff;
				iCalXOR = CalXor((U8 *)COMad,8);
				if (iRcvXOR == iCalXOR)
				{
					return 2;
				}
			}
			break;
		case 10:
			iTemp1 = *(U8 *)(COMad + 0)&0xff;
			if (iTemp1 == 0x10)
			{
				return 2;
			}
			break;
		case 11:
			iTemp1 = *(U8 *)(COMad + 0)&0xff;
			iTemp2 = *(U8 *)(COMad + 1)&0xff;
			if (iTemp1 == 0x10 && iTemp2 == 0x02)
			{
				return 2;
			}
			break;
		case 12:
			iTemp1 = *(U8 *)(COMad + 7)&0xff;
			iTemp2 = *(U8 *)(COMad + 8)&0xff;
			if (iTemp1 == 0x10 && iTemp2 == 0x03)
			{
				iRcvXOR = *(U8 *)(COMad + 9)&0xff;
				iCalXOR = CalXor((U8 *)COMad,9);
				if (iRcvXOR == iCalXOR)
				{
					return 2;
				}
			}
			break;
		case 13:
			iTemp1 = *(U8 *)(COMad + 0)&0xff;
			if (iTemp1 == 0x02)
			{
				return 2;
			}
			break;
		case 14:
			iTemp1 = *(U8 *)(COMad + 26)&0xff;
			iTemp2 = *(U8 *)(COMad + 27)&0xff;
			if (iTemp1 == 0x10 && iTemp2 == 0x03)
			{
				iRcvXOR = *(U8 *)(COMad + 28)&0xff;
				iCalXOR = CalXor((U8 *)COMad,28);
				if (iRcvXOR == iCalXOR)
				{
					return 2;
				}
			}
			break;
		case 15:
			iTemp1 = *(U8 *)(COMad + 0)&0xff;
			if (iTemp1 == 0x10)
			{
				return 2;
			}
			break;
		case 16:
			iTemp1 = *(U8 *)(COMad + 0)&0xff;
			if (iTemp1 == 0x10)
			{
				return 2;
			}
			break;
		}
		return 0;
	}
	else//������д���ݷ���
	{
		switch (ptcom->Current_Times%7)
		{
		case 1:
			iTemp1 = *(U8 *)(COMad + 0)&0xff;
			if (iTemp1 == 0x10)
			{
				ptcom->send_staradd[97]++;
				return 2;
			}
			break;
		case 2:
			iTemp1 = *(U8 *)(COMad + 0)&0xff;
			iTemp2 = *(U8 *)(COMad + 1)&0xff;
			if (iTemp1 == 0x10 && iTemp2 == 0x02)
			{
			ptcom->send_staradd[96]++;
				return 2;
			}
			break;
		case 3:
			iTemp1 = *(U8 *)(COMad + ptcom->IQ - 3)&0xff;
			iTemp2 = *(U8 *)(COMad + ptcom->IQ - 2)&0xff;
			if (iTemp1 == 0x10 && iTemp2 == 0x03)
			{
				iRcvXOR = *(U8 *)(COMad + ptcom->IQ - 1)&0xff;
				iCalXOR = CalXor((U8 *)COMad, ptcom->IQ - 1);
				if (iRcvXOR == iCalXOR)
				{
				ptcom->send_staradd[95]++;
					return 2;
				}
			}
			break;
		case 4:
			iTemp1 = *(U8 *)(COMad + 0)&0xff;
			if (iTemp1 == 0x02)
			{
			ptcom->send_staradd[94]++;
				return 2;
			}
			break;
		case 5:		
			iTemp1 = *(U8 *)(COMad + ptcom->IQ - 3)&0xff;
			iTemp2 = *(U8 *)(COMad + ptcom->IQ - 2)&0xff;
			if (iTemp1 == 0x10 && iTemp2 == 0x03)
			{
				iRcvXOR = *(U8 *)(COMad + ptcom->IQ - 1)&0xff;
				iCalXOR = CalXor((U8 *)COMad, ptcom->IQ - 1);
				if (iRcvXOR == iCalXOR)
				{		
				ptcom->send_staradd[93]++;			
					return 1;
				}
			}
			break;
		case 6:
			iTemp1 = *(U8 *)(COMad + 0)&0xff;
			if (iTemp1 == 0x10)
			{
			ptcom->send_staradd[92]++;
				return 2;
			}
			break;
		case 0:
			iTemp1 = *(U8 *)(COMad + 0)&0xff;
			if (iTemp1 == 0x10)
			{
			ptcom->send_staradd[91]++;
				return 2;
			}
			break;
		}
		return 0;	
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



unsigned short CalXor(unsigned char *chData,unsigned short uNo)
{
	U8 Xor;
	U16 i;
	Xor=chData[0];
	for(i=1;i<uNo;i++)
	{
	  	Xor=Xor^chData[i];
	}
	return (Xor);
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
