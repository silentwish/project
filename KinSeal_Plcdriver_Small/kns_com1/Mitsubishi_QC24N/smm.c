#include "stdio.h"
#include "def.h"
#include "smm.h"

 struct Com_struct_D *ptcom;

void Enter_PlcDriver(void)
{
	ptcom=(struct Com_struct_D *)adcom;	
	

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
		case 'M':
		case 'Y':
		case 'L':
		case 'K':
		case 'H':			
			Read_Bool();   //���������Ƕ�λ����       
			break;
		case 'D':
		case 'R':
		case 'N':
		case 't':
		case 'c':
			Read_Analog();  //���������Ƕ�ģ������ 
			break;
		default:
			handshake();
			break;			
		}
		break;
	case PLC_WRITE_DATA:				
		switch(ptcom->registerr)
		{
		case 'M':
		case 'Y':
		case 'L':
		case 'K':
		case 'H':			
			Set_Reset();      //����������ǿ����λ�͸�λ
			break;
		case 'D':
		case 'R':
		case 'N':
		case 't':
		case 'c':
			Write_Analog();	  //����������дģ������	
			break;
		default:
			handshake();
			break;			
		}
		break;	
	case PLC_WRITE_TIME:				//����������дʱ�䵽PLC
		switch(ptcom->registerr)
		{
		case 'D':	
			Write_Time();		
			break;
		default:
			handshake();
			break;			
		}
		break;	
	case PLC_READ_TIME:				//���������Ƕ�ȡʱ�䵽PLC
		switch(ptcom->registerr)
		{
		case 'D':		
			Read_Time();		
			break;
		default:
			handshake();
			break;			
		}
		break;
	case PLC_WRITE_RECIPE:				//����������д�䷽��PLC
		switch(*(U8 *)(PE+3))//�䷽�Ĵ�������
		{
		case 'D':		
			Write_Recipe();		
			break;	
		default:
			handshake();
			break;		
		}
		break;
	case PLC_READ_RECIPE:				//���������Ǵ�PLC��ȡ�䷽
		switch(*(U8 *)(PE+3))//�䷽�Ĵ�������
		{
		case 'D':	
		case 'R':	
		case 'N':					
			Read_Recipe();		
			break;	
		default:
			Read_Recipe();
			break;		
		}
		break;							
	case PLC_CHECK_DATA:				//�������������ݴ���
		watchcom();
		break;
	case 7:				//�������������ݴ���
		handshake();
		break;
	default:
		handshake();
		break;				
	}	 
}


void handshake()
{

}
/*************************************************
Set_Reset��λ��λ����
	���жϿؼ�Ϊλ�ؼ�ʱ����ѡ������λ��λ����ʱ��
	�����ؼ������������˺����ռ������͵����ݡ�
*************************************************/
void Set_Reset()
{
	U16 check;          //У��
	int check_H;		//У���λ
	int check_L;		//У���λ
	int startadd;		//��ʼ��ַ	
	int startadd_1;	    //��ʼ��ַ12��15λ
	int startadd_2;		//��ʼ��ַ8��11λ
	int plcadd;         //PLC��ַ
	int k=0;

	startadd=ptcom->address;		// ��ʼ��λ��ַ
	plcadd=ptcom->plc_address;	    //PLCվ��ַ
		
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
	*(U8 *)(AD1+34)=0x03;   //�����
	*(U8 *)(AD1+35)=0x00;
	        
	*(U8 *)(AD1+36)=0x00;
	*(U8 *)(AD1+37)=0x00;
	*(U8 *)(AD1+38)=0x00;
	*(U8 *)(AD1+39)=0x01;
	
	*(U8 *)(AD1+40)=0x00;
	*(U8 *)(AD1+41)=0x00;		
	
	switch (ptcom->registerr)       				//���ݲ�ͬ�ļĴ������ƣ����Ͳ�ͬI/O����
	{
		case 'Y':
			*(U8 *)(AD1+42)=0x9d;			
			break;
		case 'M':
			*(U8 *)(AD1+42)=0x90;
			break;			
		case 'L':
			*(U8 *)(AD1+42)=0x92;
			break;			
		case 'H':
			*(U8 *)(AD1+42)=0x94;
			break;			
		case 'K':
			*(U8 *)(AD1+42)=0xA0;
			break;			
		default:
			break;			
	}
	
	*(U8 *)(AD1+43)=0x00;
	
	startadd_1=(startadd>>8)&0xff;
	startadd_2=(startadd)&0xff;

	if (startadd_2==0x10)
	{
		*(U8 *)(AD1+44)=0x10;
		*(U8 *)(AD1+45)=0x10;
		k++;		
	}
	else 
	{
		*(U8 *)(AD1+44)=startadd_2;
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
	
	if (ptcom->writeValue==1)	    //��λ
	{
		*(U8 *)(AD1+52+k)=0x01;		
	}
	if (ptcom->writeValue==0)	    //��λ
	{
		*(U8 *)(AD1+52+k)=0x00;
	}
	
	*(U8 *)(AD1+53+k)=0x00;					//�̶�����

	*(U8 *)(AD1+54+k)=0x10;					//������10 03
	*(U8 *)(AD1+55+k)=0x03;	
	
	check=CalcHe((U8 *)(AD1+2),52+k)-k*0x10; 	//��ȥ��ʼ�ַ�02��������к�У��
	check_H=(check&0xf0)>>4;				//ȡУ���8λ�ĸ�λ�͵�λ
	check_H=check_H&0xf;
	check_L=check&0xf;	
	*(U8 *)(AD1+56+k)=asicc(check_H);	//����У�飬��asicc�뷢�ͣ��ȸߺ��        
	*(U8 *)(AD1+57+k)=asicc(check_L);
	 	
	ptcom->send_length[0]=58+k;			//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ		
	ptcom->return_length[0]=44;				//�������ݳ���
	ptcom->return_start[0]=0;				//����������Ч��ʼ
	ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
			
	ptcom->send_times=1;					//���ʹ���
	ptcom->Current_Times=0;					//��ǰ���ʹ���
	ptcom->Simens_Count=0;						//���ز������־			
}

/*************************************************
Read_Bool��λ����
	���жϿؼ�Ϊλ�ؼ�ʱ����ѡ���м��ӹ���ʱ��
	���������˺����ռ������͵����ݡ�
*************************************************/
void Read_Bool()	
{
	U16 check;          //У��
	int check_H;		//У���λ
	int check_L;		//У���λ
	int startadd;		//��ʼ��ַ	
	int startadd_1;	    //��ʼ��ַ12��15λ
	int startadd_2;		//��ʼ��ַ8��11λ
	int nSendlen_L;
	int nSendlen_H;
	int plcadd;         //PLC��ַ	
	int length;			//��ȡ�ĳ���
	int k=0;

	plcadd=ptcom->plc_address;			//PLCվ��ַ	
	length=ptcom->register_length;		//��ȡ�ĳ���
	startadd=ptcom->address;			//��ʼ��ַ
	
	*(U8 *)(AD1+0)=0x10;				//�����Ƿ��ʹ���Σ�ǰ38���̶�����      
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
	
	*(U8 *)(AD1+34)=0x1d;   //�����
	*(U8 *)(AD1+35)=0x00;        
	*(U8 *)(AD1+36)=0x00;
	*(U8 *)(AD1+37)=0x00;		
	
	switch (ptcom->registerr)       				//���ݲ�ͬ�ļĴ������ƣ����Ͳ�ͬI/O����
	{
		case 'X':
			*(U8 *)(AD1+38)=0x9c;
			break;	
		case 'Y':
			*(U8 *)(AD1+38)=0x9d;			
			break;
		case 'M':
			*(U8 *)(AD1+38)=0x90;
			break;			
		case 'L'://L
			*(U8 *)(AD1+38)=0x92;
			break;			
		case 'H'://V
			*(U8 *)(AD1+38)=0x94;
			break;			
		case 'K'://B
			*(U8 *)(AD1+38)=0xa0;
			break;			
		default:
			break;			
	}
	
	*(U8 *)(AD1+39)=0x00;
	
	startadd=startadd/16*16;//��16ȡ����ÿ�ζ�16��λ������λ���ĵ�ַ��8�ı���
	startadd_1=(startadd>>8)&0xff;
	startadd_2=	startadd & 0xff;
	
	if (startadd_2==0x10)
	{
		*(U8 *)(AD1+40)=0x10;
		*(U8 *)(AD1+41)=0x10;
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

	*(U8 *)(AD1+46+k)=0x10;					//������10 03
	*(U8 *)(AD1+47+k)=0x03;
	
	check=CalcHe((U8 *)(AD1+2),44+k)-k*0x10; 		//��ȥ��ʼ�ַ�02��������к�У��
	check_H=(check&0xf0)>>4;				//ȡУ���8λ�ĸ�λ�͵�λ
	check_H=check_H&0xf;
	check_L=check&0xf;	
	*(U8 *)(AD1+48+k)=asicc(check_H);		//����У�飬��asicc�뷢�ͣ��ȸߺ��        
	*(U8 *)(AD1+49+k)=asicc(check_L);

	
	ptcom->send_length[0]=50+k;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ		
	ptcom->return_length[0]=44 + length * 2;//�������ݳ��ȣ���������0x10ʱ�����ص����ݳ���Ҫ+1
	ptcom->return_start[0]=40;				//����������Ч��ʼ
	ptcom->return_length_available[0]=length*2;	//������Ч���ݳ���
		
	ptcom->register_length=length*2;		//��ȡ�ĳ���,�Ѿ��ı�
			
	ptcom->send_times=1;					//���ʹ���
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	ptcom->send_add[0]=startadd;		//�����������ַ������
	ptcom->address = ptcom->send_add[0];
	ptcom->Simens_Count=1;						//λ���ر�־		
}

/*************************************************
Read_Analog���ֺ���
	���жϿؼ�Ϊ�ְ�ť�ؼ�ʱ����ѡ���м��ӹ���ʱ��
	���������˺����ռ������͵����ݡ�
*************************************************/

void Read_Analog()		//��ģ����
{
	U16 check;          //У��
	int check_H;		//У���λ
	int check_L;		//У���λ
	int startadd;		//��ʼ��ַ	
	int startadd_1;	    //��ʼ��ַ12��15λ
	int startadd_2;		//��ʼ��ַ8��11λ
	int plcadd;         //PLC��ַ	
	int length;			//��ȡ�ĳ���
	int k=0;


	plcadd=ptcom->plc_address;			//PLCվ��ַ	
	length=ptcom->register_length;		//��ȡ�ĳ���
	startadd=ptcom->address;			//��ʼ��ַ                                          
	
	*(U8 *)(AD1+0)=0x10;				//�����Ƿ��ʹ���Σ�ǰ38���̶�����      
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
//	*(U8 *)(AD1+34)=ptcom->send_staradd[99]&0xff;   //�����
	*(U8 *)(AD1+34)=0x2C;   //�����	
	*(U8 *)(AD1+35)=0x00;
	        
	*(U8 *)(AD1+36)=0x00;
	*(U8 *)(AD1+37)=0x00;
	
	switch (ptcom->registerr)       				//���ݲ�ͬ�ļĴ������ƣ����Ͳ�ͬI/O����
	{
		case 'D':
			*(U8 *)(AD1+38)=0xa8;
			break;	
		case 'R'://W
			*(U8 *)(AD1+38)=0xB4;			
			break;
		case 'N'://SW
			*(U8 *)(AD1+38)=0xb5;
			break;			
		case 't':
			*(U8 *)(AD1+38)=0xc2;
			break;			
		case 'c':
			*(U8 *)(AD1+38)=0xc5;
			break;	
		default:
			break;	
	}	
	
	*(U8 *)(AD1+39)=0x00;
	
	startadd_1=(startadd>>8)&0xff;
	startadd_2=	(startadd)&0xff;
	
	if (startadd_2==0x10)
	{
		*(U8 *)(AD1+40)=0x10;
		*(U8 *)(AD1+41)=0x10;
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
	
	*(U8 *)(AD1+45+k)=0x00;					//�̶�����

	*(U8 *)(AD1+46+k)=0x10;					//������10 03
	*(U8 *)(AD1+47+k)=0x03;
	
	check=CalcHe((U8 *)(AD1+2),44+k)-k*0x10; 		//��ȥ��ʼ�ַ�02��������к�У��
	check_H=(check&0xf0)>>4;						//ȡУ���8λ�ĸ�λ�͵�λ
	check_H=check_H&0xf;
	check_L=check&0xf;	
	*(U8 *)(AD1+48+k)=asicc(check_H);			//����У�飬��asicc�뷢�ͣ��ȸߺ��        
	*(U8 *)(AD1+49+k)=asicc(check_L);

	
	ptcom->send_length[0]=50+k;				//���ͳ���
	ptcom->send_staradd[0]=0;						//�������ݴ洢��ַ		
	ptcom->return_length[0]=46;						//�������ݳ���
	ptcom->return_start[0]=40;						//����������Ч��ʼ
	ptcom->return_length_available[0]=length*2;			//������Ч���ݳ���	
		
	ptcom->send_times=1;							//���ʹ���
	ptcom->Current_Times=0;							//��ǰ���ʹ���	
	ptcom->send_add[0]=ptcom->address;				//�����������ַ������	

	ptcom->Simens_Count=2;						//�ַ��ر�־
	ptcom->send_staradd[99]++;
}

/*************************************************
Write_Analogд�ֺ���
	���жϿؼ�Ϊ�ְ�ť�ؼ�ʱ�������������򿪼��̣�
	������ֵ�󣬳��������˺����ռ������͵����ݡ�
*************************************************/

void Write_Analog()		//дģ����
{
	U16 check;          //У��
	int check_H;		//У���λ
	int check_L;		//У���λ
	int startadd;		//��ʼ��ַ	
	int startadd_1;	    //��ʼ��ַ12��15λ
	int startadd_2;		//��ʼ��ַ8��11λ
	int plcadd;         //PLC��ַ	
	int length;			//��ȡ�ĳ���
	int i;				//ѭ��ȡ����
	int data_H,data_L;  //�������ָ�8λ�͵�8λ
	int k=0;

	plcadd=ptcom->plc_address;			//PLCվ��ַ	
	length=ptcom->register_length;		//��ȡ�ĳ���
	startadd=ptcom->address;			//��ʼ��ַ

	*(U8 *)(AD1+0)=0x10;				//�����Ƿ��ʹ���Σ�ǰ38���̶�����      
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
	*(U8 *)(AD1+18)=0x1a + (length * 2);	//��24��ʼ�����������ݳ���				
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
	*(U8 *)(AD1+34)=0x06;   //�����
	*(U8 *)(AD1+35)=0x00;
	        
	*(U8 *)(AD1+36)=0x00;
	*(U8 *)(AD1+37)=0x00;
	
	switch (ptcom->registerr)       				//���ݲ�ͬ�ļĴ������ƣ����Ͳ�ͬI/O����
	{
		case 'D':
			*(U8 *)(AD1+38)=0xa8;
			break;	
		case 'R'://w
			*(U8 *)(AD1+38)=0xb4;			
			break;
		case 'N'://sw
			*(U8 *)(AD1+38)=0xb5;
			break;			
		case 't':
			*(U8 *)(AD1+38)=0xc2;
			break;			
		case 'c':
			*(U8 *)(AD1+38)=0xc5;
			break;	
		default:
			break;	
	}		
	
	*(U8 *)(AD1+39)=0x00;
		
	startadd_1=(startadd>>8)&0xff;
	startadd_2=	(startadd)&0xff;	
	
	if (startadd_2==0x10)
	{
		*(U8 *)(AD1+40)=0x10;
		*(U8 *)(AD1+41)=0x10;
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

	*(U8 *)(AD1+45+k)=0x00;					//�̶�����

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

	*(U8 *)(AD1+46+k+length*2)=0x10;					//������10 03
	*(U8 *)(AD1+47+k+length*2)=0x03;
	
	*(U8 *)(AD1+18) = *(U8 *)(AD1+18);				//���ǵ���0x10���ڣ����Զ෢�͵ĳ���������Ҫ����
		
	check=CalcHe((U8 *)(AD1+2),44+k+length*2)-k*0x10;   //��ȥ��ʼ�ַ�02��������к�У��
	check_H=(check&0xf0)>>4;							//ȡУ���8λ�ĸ�λ�͵�λ
	check_H=check_H&0xf;
	check_L=check&0xf;	
	*(U8 *)(AD1+48+k+length*2)=asicc(check_H);			//����У�飬��asicc�뷢�ͣ��ȸߺ��        
	*(U8 *)(AD1+49+k+length*2)=asicc(check_L);	
	
	ptcom->send_length[0]=50+k+length*2;				//���ͳ���
	ptcom->send_staradd[0]=0;						//�������ݴ洢��ַ		
	ptcom->return_length[0]=44;						//�������ݳ���
	ptcom->return_start[0]=0;						//����������Ч��ʼ
	ptcom->return_length_available[0]=0;			//������Ч���ݳ���	
	
	ptcom->send_times=1;							//���ʹ���
	ptcom->Current_Times=0;							//��ǰ���ʹ���
	ptcom->Simens_Count=0;						//���ز������־		
}
/*************************************************
Read_Recipe��ȡ�䷽����
	����̬�������䷽���Ҵ�����PLC�ж�ȡ�䷽��ťʱ��
	���������˺����ռ������͵����ݡ�
*************************************************/
void Read_Recipe()		//��ȡ�䷽
{
	U16 check;          //У��
	int check_H;		//У���λ
	int check_L;		//У���λ
	int startadd;		//��ʼ��ַ	
	int startadd_1;	    //��ʼ��ַ12��15λ
	int startadd_2;		//��ʼ��ַ8��11λ
	int plcadd;         //PLC��ַ	
	int length;			//��ȡ�ĳ���
	int ps=0;		//ÿ�η��͵ĳ���
	int SendTimes;      //���ʹ���
	int LastTimeWord;	//���һ�η��ͳ���
	int datalength;		//���Ƴ���
	int i = 0,k = 0;				//ѭ����
	int p_startadd;		//ÿ�ο�ʼ��ַ
	



	plcadd=ptcom->plc_address;				//PLCվ��ַ	
	datalength=ptcom->register_length;			//��ȡ�ĳ���
	startadd=ptcom->address;			    //��ʼ��ַ
	
	if(datalength>5000)                     //���Ƴ���
		datalength=5000;

	if(datalength%64==0)                    //�涨ÿ������ܷ�64��D�����ݸպ���64�ı���ʱ
	{
		SendTimes=datalength/64;            //���ʹ���
		LastTimeWord=64;                    //���һ�η��͵ĳ���Ϊ64	
	}
	if(datalength%64!=0)                    //���ݲ���64�ı���ʱ 
	{
		SendTimes=datalength/64+1;          //���͵Ĵ���
		LastTimeWord=datalength%64;         //���һ�η��͵ĳ���Ϊ��64������	
	}

*(U8 *)(AD1+300) = SendTimes + 67;


	ps = 0;
	for (i=0;i<SendTimes;i++)
	{	
		p_startadd=startadd+i*64;			//ÿ�ο�ʼ�ĵ�ַ����64����
		
		*(U8 *)(AD1+0+ps)=0x10;				//�����Ƿ��ʹ���Σ�ǰ38���̶�����      
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
		*(U8 *)(AD1+34+ps)=0x29;   //�����
		*(U8 *)(AD1+35+ps)=0x00;
		        
		*(U8 *)(AD1+36+ps)=0x00;
		*(U8 *)(AD1+37+ps)=0x00;
		
		switch (ptcom->registerr)       				//���ݲ�ͬ�ļĴ������ƣ����Ͳ�ͬI/O����
		{
			case 'D':
				*(U8 *)(AD1+38+ps)=0xa8;
				break;	
			case 'R':
				*(U8 *)(AD1+38+ps)=0xcc;			
				break;
			case 'N':
				*(U8 *)(AD1+38+ps)=0xb4;
				break;			
			case 't':
				*(U8 *)(AD1+38+ps)=0xc2;
				break;			
			case 'c':
				*(U8 *)(AD1+38+ps)=0xc5;
				break;	
			default:
				break;	
		}	
		
		*(U8 *)(AD1+39+ps)=0x00;		
			
		startadd_1=(p_startadd>>8)&0xff;
		startadd_2=	(p_startadd)&0xff;
		
		if (startadd_2==0x10)
		{
			*(U8 *)(AD1+40+ps)=0x10;
			*(U8 *)(AD1+41+ps)=0x10;
			k++;		
		}
		else 
		{
			*(U8 *)(AD1+40+ps)=startadd_2;
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
		
		if (i!=((SendTimes/2)-1))   //�������һ�η���ʱ
		{
			length=64;              //ÿ�η�����32����
		}
		if (i==((SendTimes/2)-1))   //������η���ʱ��
		{
			length=LastTimeWord;    //���ĳ�������ʣ�µ���
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
		
		check=CalcHe((U8 *)(AD1+2+ps),44+k)-k*0x10; 		//��ȥ��ʼ�ַ�02��������к�У��
		check_H=(check&0xf0)>>4;						//ȡУ���8λ�ĸ�λ�͵�λ
		check_H=check_H&0xf;
		check_L=check&0xf;	
		*(U8 *)(AD1+48+k+ps)=asicc(check_H);			//����У�飬��asicc�뷢�ͣ��ȸߺ��        
		*(U8 *)(AD1+49+k+ps)=asicc(check_L);		
			
		ptcom->send_length[0+i]=50+k;						//���ͳ���
		ptcom->send_staradd[0+i]=ps;						//�������ݴ洢��ַ		
		ptcom->return_length[0+i]=46;						//�������ݳ���
		ptcom->return_start[0+i]=40;						//����������Ч��ʼ
		ptcom->return_length_available[0+i]=length*2;			//������Ч���ݳ���	
		ptcom->send_add[0+i]=ptcom->address+i*64;			//�����������ַ������	
		
		ps=ps+50+k;						//�ۼƷ��͵ĳ���	
	}			
	ptcom->send_times=SendTimes;							//���ʹ���
	ptcom->Current_Times=0;									//��ǰ���ʹ���		

	ptcom->Simens_Count=2;						//�ַ��ر�־	
}

/*************************************************
Write_Recipeд�䷽����
	����̬�������䷽���Ҵ���д�䷽��PLC��ťʱ��
	���������˺����ռ������͵����ݡ�
*************************************************/

void Write_Recipe()		//д�䷽��PLC
{
	U16 check;          //У��
	int check_H;		//У���λ
	int check_L;		//У���λ
	int startadd;		//��ʼ��ַ	
	int startadd_1;	    //��ʼ��ַ12��15λ
	int startadd_2;		//��ʼ��ַ8��11λ
	int p_startadd;		//ÿ�ο�ʼ��ַ
	int plcadd;         //PLC��ַ	
	int length;			//��ȡ�ĳ���
	int data_H,data_L;  //�������ָ�8λ�͵�8λ
	int datalength;		//���Ƴ���
	int SendTimes;      //���ʹ���
	int LastTimeWord;	//���һ�η��ͳ���	
	int ps=0;		//ÿ�η��͵ĳ���
	int i = 0;				//ѭ����
	int j = 0,k = 0;
	int regname;

	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));											//���ݳ���
	startadd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));	//���ݿ�ʼ��ַ
	plcadd=*(U8 *)(PE+4);
	
	switch (*(U8 *)(PE+3))       				//���ݲ�ͬ�ļĴ������ƣ����Ͳ�ͬI/O����
	{
		case 'D':
			regname=0xa8;
			break;	
		case 'R':
			regname=0xcc;			
			break;
		case 'N':
			regname=0xb4;
			break;				
		default:
			regname=0xa8;
			break;	
	}	

	if(datalength>5000)                     //���Ƴ���
		datalength=5000;

	if(datalength%64==0)                    //�涨ÿ������ܷ�64��D�����ݸպ���64�ı���ʱ
	{
		SendTimes=datalength/64;            //���ʹ���
		LastTimeWord=64;                    //���һ�η��͵ĳ���Ϊ64	
	}
	if(datalength%64!=0)                    //���ݲ���64�ı���ʱ 
	{
		SendTimes=datalength/64+1;          //���͵Ĵ���
		LastTimeWord=datalength%64;         //���һ�η��͵ĳ���Ϊ��64������	
	}
	
	ps=0;								//ǰ�淢�͵ĳ���
	
	for (i=0;i<SendTimes;i++)
	{
		k = 0;			//��λk
		*(U8 *)(AD1+0+ps)=0x10;				//�����Ƿ��ʹ���Σ�ǰ38���̶�����      
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
		*(U8 *)(AD1+34+ps)=0x04;   //�����
		*(U8 *)(AD1+35+ps)=0x00;
		        
		*(U8 *)(AD1+36+ps)=0x00;
		*(U8 *)(AD1+37+ps)=0x00;
		

		*(U8 *)(AD1+38+ps)=regname;
		
		*(U8 *)(AD1+39+ps)=0x00;	//fix
				
		p_startadd=startadd+i*64;
		
		startadd_1=(p_startadd>>8)&0xff;
		startadd_2=	(p_startadd)&0xff;	
		
		if (startadd_2==0x10)
		{
			*(U8 *)(AD1+40+ps)=0x10;
			*(U8 *)(AD1+41+ps)=0x10;
			k++;		
		}
		else 
		{
			*(U8 *)(AD1+40+ps)=startadd_2;
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
		
		if (i==SendTimes-1)	//���һ��
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

		*(U8 *)(AD1+45+k+ps)=0x00;					//�̶�����	
		
		for (j=0;j<length;j++)							//ѭ��д������ֵ
		{				
			data_L=*(U8 *)(PE+9+j*2+64*i*2);                 //��������ֵ��D[]������ȡ���ݣ���Ӧ��data_HΪ��λ��data_LΪ��λ
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

		*(U8 *)(AD1+46+k+length*2+ps)=0x10;					//������10 03
		*(U8 *)(AD1+47+k+length*2+ps)=0x03;
		
		
		*(U8 *)(AD1+18+ps) = 0x1a + (length * 2);		
		
		check=CalcHe((U8 *)(AD1+2+ps),44+k+length*2)-k*0x10;   //��ȥ��ʼ�ַ�02��������к�У��	

		check_H = (check >> 4) & 0xf;
		check_L = (check >> 0) & 0xf;		
	
		*(U8 *)(AD1+48+k+length*2+ps)=asicc(check_H);			//����У�飬��asicc�뷢�ͣ��ȸߺ��        
		*(U8 *)(AD1+49+k+length*2+ps)=asicc(check_L);		
		
		ptcom->send_length[i]=50+k+length*2;				//���ͳ���
		ptcom->send_staradd[i]=ps;						//�������ݴ洢��ַ		
		ptcom->return_length[i]=44;						//�������ݳ���
		ptcom->return_start[i]=0;						//����������Ч��ʼ
		ptcom->return_length_available[i]=0;			//������Ч���ݳ���	
		
		ps=ps+50+k+length*2;//�ۼƷ��ͳ���
	}	
	ptcom->send_times=SendTimes;							//���ʹ���
	ptcom->Current_Times=0;									//��ǰ���ʹ���	
	ptcom->Simens_Count=0;						//���ز������־			
}

/*************************************************
Write_Timeдʱ�亯��
	������ʱ����������ʱ��ѡ�
	���������˺����ռ������͵����ݡ�
*************************************************/
void Write_Time()			//дʱ�䵽PLC
{
	Write_Analog();			//��ת��дģ��������									
}

/*************************************************
Read_Time��ʱ�亯��
	������ʱ����������ʱ��ѡ�
	���������˺����ռ������͵����ݡ�
*************************************************/
void Read_Time()				//��PLC��ȡʱ��
{
	Read_Analog();			//��ת����ģ��������
}

/*************************************************
compxy���ݴ�����
	���ڲ�ͬ��PLC���ص���ֵ����ͬ��HMI����ʶ��
	����˺��������б�Ҫ��ת����ת����HMI�������ֵ��
*************************************************/
void compxy(void)				              	//���ݴ�����
{
	int i,k,j;
	unsigned char a1;
	k=0;

	if (ptcom->Simens_Count==1)//λ���ش���
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
	if (ptcom->Simens_Count==2)//�ַ��ش���
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
}

/*************************************************
watchcom�������У�캯��
	���ڲ�ͬ��PLCʹ�õ�У�����ͬ��
	����˺��������б�Ҫ������У�졣����֤�����Ƿ�����
*************************************************/
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

/*************************************************
asicc Asc��ת������
	��ЩPLC�ڽ������ݵĽ���ʱ��ʹ�õ���Asc�룬����˺�����ת����Asc��
	���͵Ĳ���a����ת����Asc�����ֵ
*************************************************/
int asicc(int result)			//תΪAsc��
{
	int temp;
	if(result<10)
		temp=result+0x30;
	if(result>9)
		temp=result-10+0x41;
	return temp;	
}

/*************************************************
asicc ����ת������
	��ЩPLC�ڽ������ݵĽ���ʱ��ʹ�õ���Asc�룬
	�������ݵĴ���ʱ����Ҫ����Ϊ���ָ�ʽ������˺�����ת��
	���͵Ĳ���a����ת����Asc�����ֵ
*************************************************/
int bsicc(int result)			//AscתΪ����
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

/*************************************************
remark ���У�麯��
	�˺���Ϊ���У���Ƿ���ȷ������1���յ�����������������0���յ������ݲ�����
*************************************************/
int remark()				//�����������ݼ���У�����Ƿ���ȷ
{
	if (ptcom->IQ<40)		//���س��Ȳ���
	{
		return 0;
	}
	return 1;	
}

/*************************************************
CalcHe ��У�麯��
	�˺����Ƕ�һ�����н������
*************************************************/
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
