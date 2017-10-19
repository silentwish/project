/***********************************************************************************************************************
  �޸����ݣ�̨��λ�ֶ�ȡ����ܶ�15��word��λҲ��Ҫ����д��Խ1536�ֽ��ݲ�������
*************************************************************************************************************************/
#include "stdio.h"
#include "def.h"
#include "smm.h"
struct Com_struct_D *ptcom;


unsigned char KK[100];

/************************************************************************************************************************
�������뺯�������ݹ��ܽ��벻ͬ�Ĵ����Ӻ���
*************************************************************************************************************************/
void Enter_PlcDriver(void)
{
	ptcom=(struct Com_struct_D *)adcom;	 
	//sysprintf("dvp enter plc driver=%c\n",ptcom->registerr);
	switch (ptcom->R_W_Flag)
	{
		case PLC_READ_DATA:								//���������Ƕ�����
		case PLC_READ_DIGITALALARM:							//���������Ƕ�����,����	
		case PLC_READ_TREND:							//���������Ƕ�����,����ͼ
		case PLC_READ_ANALOGALARM:							//���������Ƕ�����,��ȱ���	
		case PLC_READ_CONTROL:							//���������Ƕ�PLC���Ƶ�����	
			switch(ptcom->registerr)
			{
				case 'X':
				case 'Y':
				case 'M':
				case 'T':
				case 'C':
				case 'H':
					ptcom->Simens_Count = 0;						
					Read_Bool();   			//���������Ƕ�λ����       
					break;
				case 'D':
				case 't':
				case 'c':
					ptcom->Simens_Count = 0;	
					Read_Analog();  			//���������Ƕ�ģ������ 
					break;			
			}
			break;
		case PLC_WRITE_DATA:				
			switch(ptcom->registerr)
			{
				case 'M':
				case 'Y':
				case 'T':
				case 'C':
				case 'H':
					ptcom->Simens_Count = 0;				
					Set_Reset();      		//����������ǿ����λ�͸�λ
					break;
				case 'D':
				case 't':
				case 'c':
					ptcom->Simens_Count = 0;
					Write_Analog();	  		//����������дģ������	
					break;			
			}
			break;	
		case PLC_WRITE_TIME:								//����������дʱ�䵽PLC
			switch(ptcom->registerr)
			{
				case 'D':
					ptcom->Simens_Count = 0;
					Write_Time();		
					break;			
			}
			break;	
		case PLC_READ_TIME:								//���������Ƕ�ȡʱ�䵽PLC
			switch(ptcom->registerr)
			{
				case 'D':
					ptcom->Simens_Count = 0;		
					Read_Time();		
					break;			
			}
			break;
		case PLC_WRITE_RECIPE:								//����������д�䷽��PLC
			switch(*(U8 *)(PE+3))			//�䷽�Ĵ�������
			{
				case 'D':
					ptcom->Simens_Count = 0;		
					Write_Recipe();		
					break;			
			}
			break;
		case PLC_READ_RECIPE:								//���������Ǵ�PLC��ȡ�䷽
			switch(*(U8 *)(PE+3))			//�䷽�Ĵ�������
			{
				case 'D':
					ptcom->Simens_Count = 0;		
					Read_Recipe();		
					break;			
			}
			break;							
		case PLC_CHECK_DATA:								//�������������ݴ���
			watchcom();
			break;				
	}	 
}
/************************************************************************************************************************
��λ��λ����
*************************************************************************************************************************/
void Set_Reset()
{
	U16 aakj;
	int b,b1,b2;
	int a1,a2,a3,a4;
	int add;
	int t;	
	int plcadd;
	int i;

	b=ptcom->address;						// ��ʼ��λ��ַ
	plcadd=ptcom->plc_address;	            //PLCվ��ַ
	switch (ptcom->registerr)				//���ݼĴ������ͻ��ƫ�Ƶ�ַ
	{
	case 'Y':
		add=0x500;
		break;
	case 'M':
		if (b<=1535)
		{
			add=0x800;
		}
		else
		{
			b=b-1536;
			add=0xB000;
		}
		break;	
	case 'T':
		add=0x600;
		break;
	case 'C':
		add=0xE00;
		break;		
	case 'H':
		add=0x000;
		break;				
	}
	b=b+add;								//��ʼ��ַƫ��
	b1=(b&0xff00)>>8;           			//�Կ�ʼƫ�Ƶ�ַȡ�ߵ�λ��b1��λ��b2��λ��16����
	b2=b&0xff;
	a1=b&0xf000;                			//�Կ�ʼƫ�Ƶ�ַ��asicc�������δӸߵ���
	a1=a1>>12;
	a1=a1&0xf;
	a2=b&0xf00;
	a2=a2>>8;
	a2=a2&0xf;
	a3=b&0xf0;
	a3=a3>>4;
	a3=a3&0xf;
	a4=b&0xf;
	*(U8 *)(AD1+0)=0x3a;        					//̨�￪ʼ��Ԫ":",��3a
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);//plcվ��ַ01��asicc����0x30��0x31
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);
	*(U8 *)(AD1+3)=0x30;        			//������05��ǿ����λ�͸�λ����Ҫת��asicc��
	*(U8 *)(AD1+4)=0x35;
	*(U8 *)(AD1+5)=asicc(a1);  	 			//��ʼ��ַ�����δӸߵ��ͣ�Ҫת��asicc��
	*(U8 *)(AD1+6)=asicc(a2);
	*(U8 *)(AD1+7)=asicc(a3);
	*(U8 *)(AD1+8)=asicc(a4);
	if (ptcom->writeValue==1)				//��λ0xff00
	{
		*(U8 *)(AD1+9)=0x46;
		*(U8 *)(AD1+10)=0x46;
		*(U8 *)(AD1+11)=0x30;
		*(U8 *)(AD1+12)=0x30;
		t=0xff;
	}
	if (ptcom->writeValue==0)				//��λ0x0000
	{
		*(U8 *)(AD1+9)=0x30;
		*(U8 *)(AD1+10)=0x30;
		*(U8 *)(AD1+11)=0x30;
		*(U8 *)(AD1+12)=0x30;
		t=0x00;
	}
	KK[1]=plcadd&0xff;                 			//LRCУ�飬16����У�飬asicc����ʾ
	KK[2]=0x05;
	KK[3]=b1;
	KK[4]=b2;
	KK[5]=t;
	KK[6]=0x00;	
	aakj=CalLRC(KK,7);          			//LRCУ�飬��λ��ǰ����λ�ں�         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+13)=asicc(a1);  			//asicc����ʾ
	*(U8 *)(AD1+14)=asicc(a2);
	*(U8 *)(AD1+15)=0x0d;       			//������Ԫ0d,0a
	*(U8 *)(AD1+16)=0x0a;

	//sysprintf("Set_Reset\n");
	//for(i=0;i<=16;i++)
	//{
	//	sysprintf("AD%d=%x\n",i,*(U8 *)(AD1+i));
	//}
	
	ptcom->send_length[0]=17;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
	ptcom->send_times=1;					//���ʹ���
		
	ptcom->return_length[0]=17;				//�������ݳ���
	ptcom->return_start[0]=0;				//����������Ч��ʼ
	ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	
	ptcom->send_staradd[90]=0;		
}
/************************************************************************************************************************
������������
*************************************************************************************************************************/
void Read_Bool()								
{
	switch (ptcom->registerr)//���ݼĴ������ͻ���
	{
	case 'X':
	case 'Y':
	case 'T':
	case 'C':
	case 'H':
		Read_Bool_continous();//��ַ��������
		break;	
	case 'M':
		if ( ((ptcom->address <= 1535 && (ptcom->address + ptcom->register_length*8) <= 1535)) || ptcom->address > 1535 )
		{
			Read_Bool_continous();//��ַ��������
		}
		else
		{
			Read_Bool_discontinous();//��ַ������
		}
		break;					
	}
	ptcom->send_staradd[90]=1;				
}
/************************************************************************************************************************
���̵�������ΪXYTC��������Ϊ��ַ�������ģ�������������
*************************************************************************************************************************/
void Read_Bool_continous()
{
	int nFuncCode = 0x01;
	int nSendTimes = 0;
	int nLastTimeLen = 0;
	int ps = 0;
	int i=0;
	int nPlcStation = 0;
	int nPassAddress = 0;
	int nSendAddress = 0;
	int nSenLen = 0;
	U16 nCalCheck = 0;
	
	nPlcStation = ptcom->plc_address;	            	
	nSendAddress = ptcom->address;
	
	switch (ptcom->registerr)				
	{
		case 'X':
			nPassAddress = nSendAddress + 0x0400;
			nFuncCode = 0x02;
			break;
		case 'Y':
			nPassAddress = nSendAddress + 0x0500;
			break;		
		case 'T':
			nPassAddress = nSendAddress + 0x0600;
			break;
		case 'C':
			nPassAddress = nSendAddress + 0x0e00;
			break;	
		case 'H':
			nPassAddress = nSendAddress + 0x0000;
			break;	
		case 'M':
			if (nSendAddress <= 1535)
			{
				nPassAddress = nSendAddress + 0x0800;
			}
			else
			{
				nPassAddress = nSendAddress - 1536 + 0xb000;
			}
			break;				
	}		
	
	/*̨��һ������ܶ�ȡ15��word����30��char*/
	if (ptcom->register_length % 30 == 0)
	{
		nSendTimes = ptcom->register_length / 30;
		nLastTimeLen = 30;
	}
	else
	{
		nSendTimes = (ptcom->register_length / 30) + 1;
		nLastTimeLen = ptcom->register_length % 30;
	}
	
	for (i = 0; i < nSendTimes; i++)
	{
		ps = i*17;
		nSendAddress = nPassAddress + i*30*8;
		
		if (i != nSendTimes - 1)
		{
			nSenLen = 30 * 8;
		}
		else
		{
			nSenLen = nLastTimeLen * 8;
		}
	
		
		*(U8 *)(AD1 + 0 + ps) = 0x3a;
		
		*(U8 *)(AD1 + 1 + ps) = asicc((nPlcStation >> 4) & 0x0f);
		*(U8 *)(AD1 + 2 + ps) = asicc(nPlcStation & 0x0f);
		
		*(U8 *)(AD1 + 3 + ps) = asicc((nFuncCode >> 4) & 0x0f);
		*(U8 *)(AD1 + 4 + ps) = asicc(nFuncCode & 0x0f);
		
		*(U8 *)(AD1 + 5 + ps) = asicc((nSendAddress >> 12) & 0x0f);
		*(U8 *)(AD1 + 6 + ps) = asicc((nSendAddress >> 8) & 0x0f);
		*(U8 *)(AD1 + 7 + ps) = asicc((nSendAddress >> 4) & 0x0f);
		*(U8 *)(AD1 + 8 + ps) = asicc(nSendAddress & 0x0f);
		
		*(U8 *)(AD1 + 9 + ps) = asicc((nSenLen >> 12) & 0x0f);
		*(U8 *)(AD1 + 10 + ps) = asicc((nSenLen >> 8) & 0x0f);
		*(U8 *)(AD1 + 11 + ps) = asicc((nSenLen >> 4) & 0x0f);
		*(U8 *)(AD1 + 12 + ps) = asicc(nSenLen & 0x0f);
		
		KK[1] = nPlcStation & 0xff; 
		KK[2] = nFuncCode & 0xff;
		KK[3] = (nSendAddress >> 8) & 0xff;
		KK[4] = nSendAddress & 0xff;
		KK[5] = (nSenLen >> 8) & 0xff;
		KK[6] = nSenLen & 0xff;
		
		nCalCheck = CalLRC( KK, 7 );          			
		*(U8 *)(AD1 + 13 + ps) = asicc((nCalCheck >> 4) & 0x0f);  			
		*(U8 *)(AD1 + 14 + ps) = asicc(nCalCheck & 0x0f);
		*(U8 *)(AD1 + 15 + ps) = 0x0d;       		
		*(U8 *)(AD1 + 16 + ps) = 0x0a;	
		
		ptcom->send_length[i] = 17;
		ptcom->send_staradd[i] = ps;											
		ptcom->return_length[i]= 11 + (nSenLen / 8) * 2;                                       
		ptcom->return_start[i] = 7;				
		ptcom->return_length_available[i] = (nSenLen / 8) * 2;
		
		ptcom->send_add[i] = ptcom->address + i*30*8;		
		ptcom->send_data_length[i] = nSenLen / 8;
			
		//ptcom->address = ptcom->address + i*30*8;
		//ptcom->register_length = nSenLen / 8;	
	}
	ptcom->Simens_Count = 100;
	ptcom->Current_Times = 0;
	ptcom->send_times = nSendTimes;		
}
/************************************************************************************************************************
���̵�������ΪM��������Ϊ��ַ�ǲ��������ģ�����ֶζ����߽��ַΪ1535����Խ�߽�������ζ�
*************************************************************************************************************************/
void Read_Bool_discontinous()
{
	U16 aakj;
	int b,t;
	int a1,a2,a3,a4;
	int add;
	int b1,b2,b3,b4;
	int t1,t2,t3,t4;
	int plcadd;
	int length;
	int add_1;
	int	length_1,length_2;
	 
	plcadd=ptcom->plc_address;	            //PLCվ��ַ	
	b=ptcom->address;						//���������Ѿ�ת�����öεĿ�ʼ��ַ		
	length=ptcom->register_length;
	
	if (b<1535 && b+length*8>1536)			//����Խ�߽�ʱ�������ζ�
	{
		add=0x800;							//��ӦPLC�￪ʼ��ַ���ֽ�ǰ
		add_1=b;
		length_1=(1536-b)/8;
		
		add_1=add_1+add;					//����ƫ�Ƶ�ַ
		
		b1=(add_1&0xff00)>>8;           	//�Կ�ʼƫ�Ƶ�ַȡ�ߵ�λ��b1��λ��b2��λ��16����
		b2=add_1&0xff;
		a1=add_1&0xf000;                	//�Կ�ʼƫ�Ƶ�ַ��asicc�������δӸߵ��� 
		a1=a1>>12;
		a1=a1&0xf;
		a2=add_1&0xf00;
		a2=a2>>8;
		a2=a2&0xf;
		a3=add_1&0xf0;
		a3=a3>>4;
		a3=a3&0xf;
		a4=add_1&0xf;
		*(U8 *)(AD1+0)=0x3a;        		//̨�￪ʼ��Ԫ":",��3a
		*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);//plcվ��ַ01��asicc����0x30��0x31
		*(U8 *)(AD1+2)=asicc(plcadd&0xf);
		*(U8 *)(AD1+3)=0x30;        		//������02��Ҫת��asicc��
		*(U8 *)(AD1+4)=0x31;
		*(U8 *)(AD1+5)=asicc(a1);   		//��ʼ��ַ�����δӸߵ��ͣ�Ҫת��asicc��
		*(U8 *)(AD1+6)=asicc(a2);
		*(U8 *)(AD1+7)=asicc(a3);
		*(U8 *)(AD1+8)=asicc(a4);
		t=length_1*8; 						//һ��Ԫ������8λ����
		b3=(t&0xff00)>>8;           		//��Ԫ������ȡ�ߵ�λ��b3��λ��b4��λ��16����
		b4=t&0xff;
		t1=t&0xf000;                		//��Ԫ��������asicc�������δӸߵ���
		t1=t1>>12;
		t1=t1&0xf;
		t2=t&0xf00;
		t2=t2>>8;
		t2=t2&0xf;
		t3=t&0xf0;
		t3=t3>>4;
		t3=t3&0xf;
		t4=t&0xf;
		*(U8 *)(AD1+9)=asicc(t1);			//����Ԫ����������asicc�뷢�ͣ����δӸߵ���
		*(U8 *)(AD1+10)=asicc(t2);
		*(U8 *)(AD1+11)=asicc(t3);
		*(U8 *)(AD1+12)=asicc(t4);
		KK[1]=plcadd&0xff;                 		//LRCУ�飬16����У�飬asicc����ʾ
		KK[2]=0x01;
		KK[3]=b1;
		KK[4]=b2;
		KK[5]=b3;
		KK[6]=b4;
		
		aakj=CalLRC(KK,7);          		//LRCУ�飬��λ��ǰ����λ�ں�
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13)=asicc(a1);  		//asicc����ʾ
		*(U8 *)(AD1+14)=asicc(a2);
		*(U8 *)(AD1+15)=0x0d;       		//������Ԫ0d,0a
		*(U8 *)(AD1+16)=0x0a;	
		
		ptcom->send_length[0]=17;			//���ͳ���
		ptcom->send_staradd[0]=0;			//�������ݴ洢��ַ				
		ptcom->return_length[0]=11+length_1*2;//�������ݳ��ȣ���9���̶���3a��PLC��ַ2�����ȣ�������2�����ȣ�
		                                    //λ����ʾ2�����ȣ�У��2�����ȣ�0d��0a
		ptcom->return_start[0]=7;			//����������Ч��ʼ
		ptcom->return_length_available[0]=length_1*2;//������Ч���ݳ���	
		ptcom->send_add[0]=ptcom->address;	//�����������ַ������	
		
		ptcom->register_length=length_1;
//--------------------------------------------------------------------------------------------------------		
		add=0xB000;							//��ӦPLC�￪ʼ��ַ���ֽ��
		length_2=length-length_1;
		
		b1=(add&0xff00)>>8;           		//�Կ�ʼƫ�Ƶ�ַȡ�ߵ�λ��b1��λ��b2��λ��16����
		b2=add&0xff;
		a1=add&0xf000;                		//�Կ�ʼƫ�Ƶ�ַ��asicc�������δӸߵ��� 
		a1=a1>>12;
		a1=a1&0xf;
		a2=add&0xf00;
		a2=a2>>8;
		a2=a2&0xf;
		a3=add&0xf0;
		a3=a3>>4;
		a3=a3&0xf;
		a4=add&0xf;
		*(U8 *)(AD1+17)=0x3a;        		//̨�￪ʼ��Ԫ":",��3a
		*(U8 *)(AD1+18)=asicc(((plcadd&0xf0)>>4)&0xf);//plcվ��ַ01��asicc����0x30��0x31
		*(U8 *)(AD1+19)=asicc(plcadd&0xf);
		*(U8 *)(AD1+20)=0x30;        		//������02��Ҫת��asicc��
		*(U8 *)(AD1+21)=0x31;
		*(U8 *)(AD1+22)=asicc(a1);   		//��ʼ��ַ�����δӸߵ��ͣ�Ҫת��asicc��
		*(U8 *)(AD1+23)=asicc(a2);
		*(U8 *)(AD1+24)=asicc(a3);
		*(U8 *)(AD1+25)=asicc(a4);
		
		t=length_2*8; 						//һ��Ԫ������8λ����
		b3=(t&0xff00)>>8;           		//��Ԫ������ȡ�ߵ�λ��b3��λ��b4��λ��16����
		b4=t&0xff;
		t1=t&0xf000;                		//��Ԫ��������asicc�������δӸߵ���
		t1=t1>>12;
		t1=t1&0xf;
		t2=t&0xf00;
		t2=t2>>8;
		t2=t2&0xf;
		t3=t&0xf0;
		t3=t3>>4;
		t3=t3&0xf;
		t4=t&0xf;
		*(U8 *)(AD1+26)=asicc(t1);			//����Ԫ����������asicc�뷢�ͣ����δӸߵ���
		*(U8 *)(AD1+27)=asicc(t2);
		*(U8 *)(AD1+28)=asicc(t3);
		*(U8 *)(AD1+29)=asicc(t4);
		KK[1]=plcadd&0xff;                 		//LRCУ�飬16����У�飬asicc����ʾ
		KK[2]=0x01;
		KK[3]=b1;
		KK[4]=b2;
		KK[5]=b3;
		KK[6]=b4;
		
		aakj=CalLRC(KK,7);          		//LRCУ�飬��λ��ǰ����λ�ں�
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+30)=asicc(a1);  		//asicc����ʾ
		*(U8 *)(AD1+31)=asicc(a2);
		*(U8 *)(AD1+32)=0x0d;       		//������Ԫ0d,0a
		*(U8 *)(AD1+33)=0x0a;	
		
		ptcom->send_length[1]=17;			//���ͳ���
		ptcom->send_staradd[1]=17;			//�������ݴ洢��ַ				
		ptcom->return_length[1]=11+length_2*2;//�������ݳ��ȣ���9���̶���3a��PLC��ַ2�����ȣ�������2�����ȣ�
		                                    //λ����ʾ2�����ȣ�У��2�����ȣ�0d��0a
		ptcom->return_start[1]=7;			//����������Ч��ʼ
		ptcom->return_length_available[1]=length_2*2;//������Ч���ݳ���	
		ptcom->send_add[1]=1536;			//�����������ַ������	
		
		ptcom->send_staradd[98]=1536;
		ptcom->send_staradd[99]=length_2;
		
		ptcom->Current_Times=0;				//��ǰ���ʹ���
		ptcom->send_times=2;				//���ʹ���					
	}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	else									//δ��Խ�߽��ַ������������
	{
		if (b<1535 && b+length*8<=1536)
		{
			add=0x0800;
		}
		if (b>=1536)
		{
			b=b-1536;
			add=0xB000;
		}
		
		b=b+add;							//����ƫ�Ƶ�ַ
		b1=(b&0xff00)>>8;           		//�Կ�ʼƫ�Ƶ�ַȡ�ߵ�λ��b1��λ��b2��λ��16����
		b2=b&0xff;
		a1=b&0xf000;               			//�Կ�ʼƫ�Ƶ�ַ��asicc�������δӸߵ��� 
		a1=a1>>12;
		a1=a1&0xf;
		a2=b&0xf00;
		a2=a2>>8;
		a2=a2&0xf;
		a3=b&0xf0;
		a3=a3>>4;
		a3=a3&0xf;
		a4=b&0xf;
		*(U8 *)(AD1+0)=0x3a;        		//̨�￪ʼ��Ԫ":",��3a
		*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);//plcվ��ַ01��asicc����0x30��0x31
		*(U8 *)(AD1+2)=asicc(plcadd&0xf);
		*(U8 *)(AD1+3)=0x30;        		//������02��Ҫת��asicc��
		*(U8 *)(AD1+4)=0x31;
		*(U8 *)(AD1+5)=asicc(a1);   		//��ʼ��ַ�����δӸߵ��ͣ�Ҫת��asicc��
		*(U8 *)(AD1+6)=asicc(a2);
		*(U8 *)(AD1+7)=asicc(a3);
		*(U8 *)(AD1+8)=asicc(a4);
		t=ptcom->register_length*8; 		//һ��Ԫ������8λ����
		b3=(t&0xff00)>>8;           		//��Ԫ������ȡ�ߵ�λ��b3��λ��b4��λ��16����
		b4=t&0xff;
		t1=t&0xf000;                		//��Ԫ��������asicc�������δӸߵ���
		t1=t1>>12;
		t1=t1&0xf;
		t2=t&0xf00;
		t2=t2>>8;
		t2=t2&0xf;
		t3=t&0xf0;
		t3=t3>>4;
		t3=t3&0xf;
		t4=t&0xf;
		*(U8 *)(AD1+9)=asicc(t1);			//����Ԫ����������asicc�뷢�ͣ����δӸߵ���
		*(U8 *)(AD1+10)=asicc(t2);
		*(U8 *)(AD1+11)=asicc(t3);
		*(U8 *)(AD1+12)=asicc(t4);
		KK[1]=plcadd&0xff;                 		//LRCУ�飬16����У�飬asicc����ʾ
		KK[2]=0x01;
		KK[3]=b1;
		KK[4]=b2;
		KK[5]=b3;
		KK[6]=b4;
		
		aakj=CalLRC(KK,7);          		//LRCУ�飬��λ��ǰ����λ�ں�
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13)=asicc(a1);  		//asicc����ʾ
		*(U8 *)(AD1+14)=asicc(a2);
		*(U8 *)(AD1+15)=0x0d;       		//������Ԫ0d,0a
		*(U8 *)(AD1+16)=0x0a;	
		
		ptcom->send_length[0]=17;			//���ͳ���
		ptcom->send_staradd[0]=0;			//�������ݴ洢��ַ	
		ptcom->send_times=1;				//���ʹ���
			
		ptcom->return_length[0]=11+ptcom->register_length*2;//�������ݳ��ȣ���9���̶���3a��PLC��ַ2�����ȣ�������2�����ȣ�
		                                    //λ����ʾ2�����ȣ�У��2�����ȣ�0d��0a
		ptcom->return_start[0]=7;			//����������Ч��ʼ
		ptcom->return_length_available[0]=ptcom->register_length*2;	//������Ч���ݳ���	
		ptcom->Current_Times=0;				//��ǰ���ʹ���	
		ptcom->send_add[0]=ptcom->address;	//�����������ַ������					
	}	
}
/************************************************************************************************************************
��ģ����������
*************************************************************************************************************************/
void Read_Analog()							//��ģ����
{
	switch (ptcom->registerr)				//���ݼĴ������ͻ���
	{
		case 'D':		
			Read_Analog_D();					//D�Ĵ�����ַ������������зֶβ���
			ptcom->send_staradd[90]=1;
			break;
		case 't':	
		case 'c':
			Read_Analog_tc();				//tc�Ĵ�����ַ�������ģ�������
			break;						
	}		
}
/************************************************************************************************************************
������ΪD�ļĴ����������ַ���������߽��ַΪ4095����ֶν��ж�
*************************************************************************************************************************/
void Read_Analog_D()
{
	U16 aakj;
	int b,t;
	int a1,a2,a3,a4;
	int add;
	int b1,b2,b3,b4;
	int t1,t2,t3,t4;
	int plcadd;
	int length,length_1,length_2;
	int add_1;
	
	plcadd=ptcom->plc_address;	            //PLCվ��ַ	
	b=ptcom->address;						//��ʼ��ַ	
	length=ptcom->register_length;
	
	if (b<=4095 && (b+length)>4096)			//����Խ�߽�ʱ�������ζ�
	{
		add=0x1000;
		add_1=b;
		length_1=4096-b;
		
		add_1=add_1+add;					//����ƫ�Ƶ�ַ
		
		b1=(add_1&0xff00)>>8;           	//�Կ�ʼƫ�Ƶ�ַȡ�ߵ�λ��b1��λ��b2��λ��16����
		b2=add_1&0xff;	
		a1=add_1&0xf000;                	//�Կ�ʼƫ�Ƶ�ַ��asicc�������δӸߵ��� 
		a1=a1>>12;
		a1=a1&0xf;
		a2=add_1&0xf00;
		a2=a2>>8;
		a2=a2&0xf;
		a3=add_1&0xf0;
		a3=a3>>4;
		a3=a3&0xf;
		a4=add_1&0xf;
		*(U8 *)(AD1+0)=0x3a;        		//̨�￪ʼ��Ԫ":",��3a
		*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);//plcվ��ַ01��asicc����0x30��0x31
		*(U8 *)(AD1+2)=asicc(plcadd&0xf);
		*(U8 *)(AD1+3)=0x30;        		//������03��Ҫת��asicc��
		*(U8 *)(AD1+4)=0x33;	
		*(U8 *)(AD1+5)=asicc(a1);			//��ʼ��ַ�����δӸߵ��ͣ�Ҫת��asicc��
		*(U8 *)(AD1+6)=asicc(a2);
		*(U8 *)(AD1+7)=asicc(a3);
		*(U8 *)(AD1+8)=asicc(a4);
		t=length_1;   						// register_length�ǼĴ������ݳ���
		b3=(t&0xff00)>>8;           		//��Ԫ������ȡ�ߵ�λ��b3��λ��b4��λ��16����
		b4=t&0xff;
		t1=t&0xf000;                		//��Ԫ��������asicc�������δӸߵ���
		t1=t1>>12;
		t1=t1&0xf;
		t2=t&0xf00;
		t2=t2>>8;
		t2=t2&0xf;
		t3=t&0xf0;
		t3=t3>>4;
		t3=t3&0xf;
		t4=t&0xf;
		*(U8 *)(AD1+9)=asicc(t1);			//����Ԫ��������asicc����ʾ
		*(U8 *)(AD1+10)=asicc(t2);
		*(U8 *)(AD1+11)=asicc(t3);
		*(U8 *)(AD1+12)=asicc(t4);
		KK[1]=plcadd&0xff;                 		//LRCУ�飬16����У�飬asicc����ʾ
		KK[2]=0x03;
		KK[3]=b1;
		KK[4]=b2;
		KK[5]=b3;
		KK[6]=b4;
		
		aakj=CalLRC(KK,7);	       			//�����У�죬LRCУ�飬��λ��ǰ����λ�ں�
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13)=asicc(a1); 			//asicc����ʾ
		*(U8 *)(AD1+14)=asicc(a2);
		*(U8 *)(AD1+15)=0x0d;      			//������Ԫ0d,0a
		*(U8 *)(AD1+16)=0x0a;
		
		ptcom->send_length[0]=17;			//���ͳ���
		ptcom->send_staradd[0]=0;			//�������ݴ洢��ַ				
		ptcom->return_length[0]=11+length_1*4;//�������ݳ��ȣ���9���̶���3a��PLC��ַ2�����ȣ�������2�����ȣ�
		                                    //λ����ʾ2�����ȣ�У��2�����ȣ�0d��0a
		ptcom->return_start[0]=7;			//����������Ч��ʼ
		ptcom->return_length_available[0]=length_1*4;//������Ч���ݳ���	
		ptcom->send_add[0]=ptcom->address;	//�����������ַ������	
		
		ptcom->register_length=length_1;
//-----------------------------------------------------------------------------------------------------------------		
		add=0x9000;		
		length_2=length-length_1;
		
		b1=(add&0xff00)>>8;           		//�Կ�ʼƫ�Ƶ�ַȡ�ߵ�λ��b1��λ��b2��λ��16����
		b2=add&0xff;	
		a1=add&0xf000;                		//�Կ�ʼƫ�Ƶ�ַ��asicc�������δӸߵ��� 
		a1=a1>>12;
		a1=a1&0xf;
		a2=add&0xf00;
		a2=a2>>8;
		a2=a2&0xf;
		a3=add&0xf0;
		a3=a3>>4;
		a3=a3&0xf;
		a4=add&0xf;
		*(U8 *)(AD1+17)=0x3a;        		//̨�￪ʼ��Ԫ":",��3a
		*(U8 *)(AD1+18)=asicc(((plcadd&0xf0)>>4)&0xf);//plcվ��ַ01��asicc����0x30��0x31
		*(U8 *)(AD1+19)=asicc(plcadd&0xf);
		*(U8 *)(AD1+20)=0x30;        		//������03��Ҫת��asicc��
		*(U8 *)(AD1+21)=0x33;	
		*(U8 *)(AD1+22)=asicc(a1);			//��ʼ��ַ�����δӸߵ��ͣ�Ҫת��asicc��
		*(U8 *)(AD1+23)=asicc(a2);
		*(U8 *)(AD1+24)=asicc(a3);
		*(U8 *)(AD1+25)=asicc(a4);
		t=length_2;   						// register_length�ǼĴ������ݳ���
		b3=(t&0xff00)>>8;           		//��Ԫ������ȡ�ߵ�λ��b3��λ��b4��λ��16����
		b4=t&0xff;
		t1=t&0xf000;                		//��Ԫ��������asicc�������δӸߵ���
		t1=t1>>12;
		t1=t1&0xf;
		t2=t&0xf00;
		t2=t2>>8;
		t2=t2&0xf;
		t3=t&0xf0;
		t3=t3>>4;
		t3=t3&0xf;
		t4=t&0xf;
		*(U8 *)(AD1+26)=asicc(t1);			//����Ԫ��������asicc����ʾ
		*(U8 *)(AD1+27)=asicc(t2);
		*(U8 *)(AD1+28)=asicc(t3);
		*(U8 *)(AD1+29)=asicc(t4);
		KK[1]=plcadd&0xff;                 		//LRCУ�飬16����У�飬asicc����ʾ
		KK[2]=0x03;
		KK[3]=b1;
		KK[4]=b2;
		KK[5]=b3;
		KK[6]=b4;
		
		aakj=CalLRC(KK,7);	       			//�����У�죬LRCУ�飬��λ��ǰ����λ�ں�
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+30)=asicc(a1); 			//asicc����ʾ
		*(U8 *)(AD1+31)=asicc(a2);
		*(U8 *)(AD1+32)=0x0d;      			//������Ԫ0d,0a
		*(U8 *)(AD1+33)=0x0a;
		
		ptcom->send_length[1]=17;			//���ͳ���
		ptcom->send_staradd[1]=17;			//�������ݴ洢��ַ				
		ptcom->return_length[1]=11+length_2*4;//�������ݳ��ȣ���9���̶���3a��PLC��ַ2�����ȣ�������2�����ȣ�
		                                    //λ����ʾ2�����ȣ�У��2�����ȣ�0d��0a
		ptcom->return_start[1]=7;			//����������Ч��ʼ
		ptcom->return_length_available[1]=length_2*4;//������Ч���ݳ���	
		ptcom->send_add[1]=4096;			//�����������ַ������	
		
		ptcom->send_staradd[98]=4096;
		ptcom->send_staradd[99]=length_2;
		
		ptcom->Current_Times=0;				//��ǰ���ʹ���
		ptcom->send_times=2;				//���ʹ���											
	}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else									//δ��Խ�߽磬������
	{
		if (b<=4095 && b+length<=4096)
		{
			add=0x1000;
		}
		if (b>=4096)
		{
			b=b-4096;
			add=0x9000;
		}
		
		b=b+add;							//����ƫ�Ƶ�ַ	
		b1=(b&0xff00)>>8;           		//�Կ�ʼƫ�Ƶ�ַȡ�ߵ�λ��b1��λ��b2��λ��16����
		b2=b&0xff;	
		a1=b&0xf000;                		//�Կ�ʼƫ�Ƶ�ַ��asicc�������δӸߵ��� 
		a1=a1>>12;
		a1=a1&0xf;
		a2=b&0xf00;
		a2=a2>>8;
		a2=a2&0xf;
		a3=b&0xf0;
		a3=a3>>4;
		a3=a3&0xf;
		a4=b&0xf;
		*(U8 *)(AD1+0)=0x3a;        		//̨�￪ʼ��Ԫ":",��3a
		*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);//plcվ��ַ01��asicc����0x30��0x31
		*(U8 *)(AD1+2)=asicc(plcadd&0xf);
		*(U8 *)(AD1+3)=0x30;        		//������03��Ҫת��asicc��
		*(U8 *)(AD1+4)=0x33;	
		*(U8 *)(AD1+5)=asicc(a1);			//��ʼ��ַ�����δӸߵ��ͣ�Ҫת��asicc��
		*(U8 *)(AD1+6)=asicc(a2);
		*(U8 *)(AD1+7)=asicc(a3);
		*(U8 *)(AD1+8)=asicc(a4);
		t=length;   						// register_length�ǼĴ������ݳ���
		b3=(t&0xff00)>>8;           		//��Ԫ������ȡ�ߵ�λ��b3��λ��b4��λ��16����
		b4=t&0xff;
		t1=t&0xf000;                		//��Ԫ��������asicc�������δӸߵ���
		t1=t1>>12;
		t1=t1&0xf;
		t2=t&0xf00;
		t2=t2>>8;
		t2=t2&0xf;
		t3=t&0xf0;
		t3=t3>>4;
		t3=t3&0xf;
		t4=t&0xf;
		*(U8 *)(AD1+9)=asicc(t1);			//����Ԫ��������asicc����ʾ
		*(U8 *)(AD1+10)=asicc(t2);
		*(U8 *)(AD1+11)=asicc(t3);
		*(U8 *)(AD1+12)=asicc(t4);
		KK[1]=plcadd&0xff;                 		//LRCУ�飬16����У�飬asicc����ʾ
		KK[2]=0x03;
		KK[3]=b1;
		KK[4]=b2;
		KK[5]=b3;
		KK[6]=b4;
		
		aakj=CalLRC(KK,7);	      			//�����У�죬LRCУ�飬��λ��ǰ����λ�ں�
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13)=asicc(a1); 			//asicc����ʾ
		*(U8 *)(AD1+14)=asicc(a2);
		*(U8 *)(AD1+15)=0x0d;      			//������Ԫ0d,0a
		*(U8 *)(AD1+16)=0x0a;
		
		ptcom->send_length[0]=17;			//���ͳ���
		ptcom->send_staradd[0]=0;			//�������ݴ洢��ַ	
		ptcom->send_times=1;				//���ʹ���
			
		ptcom->return_length[0]=11+length*4;//�������ݳ��ȣ���9���̶���3a��PLC��ַ2�����ȣ�������2�����ȣ�
		                                    //λ����ʾ2�����ȣ�У��2�����ȣ�0d��0a
		ptcom->return_start[0]=7;			//����������Ч��ʼ
		ptcom->return_length_available[0]=length*4;//������Ч���ݳ���	
		ptcom->Current_Times=0;				//��ǰ���ʹ���	
		ptcom->send_add[0]=ptcom->address;	//�����������ַ������
	}
}
/************************************************************************************************************************
������Ϊtc�ļĴ���
*************************************************************************************************************************/
void Read_Analog_tc()
{
	U16 aakj;
	int b,t;
	int a1,a2,a3,a4;
	int add;
	int b1,b2,b3,b4;
	int t1,t2,t3,t4;
	int plcadd;
	
	plcadd=ptcom->plc_address;	            //PLCվ��ַ	
	b=ptcom->address;						//��ʼ��ַ	
	switch (ptcom->registerr)				//���ݼĴ������ͻ��ƫ�Ƶ�ַ
	{
		case 't':
			add=0x600;
			break;		
		case 'c':
			add=0xe00;
			break;						
	}
	
	if(ptcom->registerr=='c')
	{
      if (b>=200)
      {
          b=((b-200)/2)+200;
      }   

      if(b>=200)
      {
          t=ptcom->register_length/2;               // register_length�ǼĴ������ݳ���
      }
      else
      {
          t=ptcom->register_length;               // register_length�ǼĴ������ݳ���

      }      
	}
	else
	{
        t=ptcom->register_length;               // register_length�ǼĴ������ݳ���
	}
    
	b=b+add;								//����ƫ�Ƶ�ַ
	b1=(b&0xff00)>>8;           			//�Կ�ʼƫ�Ƶ�ַȡ�ߵ�λ��b1��λ��b2��λ��16����
	b2=b&0xff;	
	a1=b&0xf000;                			//�Կ�ʼƫ�Ƶ�ַ��asicc�������δӸߵ��� 
	a1=a1>>12;
	a1=a1&0xf;
	a2=b&0xf00;
	a2=a2>>8;
	a2=a2&0xf;
	a3=b&0xf0;
	a3=a3>>4;
	a3=a3&0xf;
	a4=b&0xf;
	*(U8 *)(AD1+0)=0x3a;        			//̨�￪ʼ��Ԫ":",��3a
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);//plcվ��ַ01��asicc����0x30��0x31
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);
	*(U8 *)(AD1+3)=0x30;        			//������03��Ҫת��asicc��
	*(U8 *)(AD1+4)=0x33;	
	*(U8 *)(AD1+5)=asicc(a1);				//��ʼ��ַ�����δӸߵ��ͣ�Ҫת��asicc��
	*(U8 *)(AD1+6)=asicc(a2);
	*(U8 *)(AD1+7)=asicc(a3);
	*(U8 *)(AD1+8)=asicc(a4);

	
	b3=(t&0xff00)>>8;          			 	//��Ԫ������ȡ�ߵ�λ��b3��λ��b4��λ��16����
	b4=t&0xff;
	t1=t&0xf000;                			//��Ԫ��������asicc�������δӸߵ���
	t1=t1>>12;
	t1=t1&0xf;
	t2=t&0xf00;
	t2=t2>>8;
	t2=t2&0xf;
	t3=t&0xf0;
	t3=t3>>4;
	t3=t3&0xf;
	t4=t&0xf;
	*(U8 *)(AD1+9)=asicc(t1);				//����Ԫ��������asicc����ʾ
	*(U8 *)(AD1+10)=asicc(t2);
	*(U8 *)(AD1+11)=asicc(t3);
	*(U8 *)(AD1+12)=asicc(t4);
	KK[1]=plcadd&0xff;                 			//LRCУ�飬16����У�飬asicc����ʾ
	KK[2]=0x03;
	KK[3]=b1;
	KK[4]=b2;
	KK[5]=b3;
	KK[6]=b4;
	
	aakj=CalLRC(KK,7);	       				//�����У�죬LRCУ�飬��λ��ǰ����λ�ں�
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+13)=asicc(a1); 				//asicc����ʾ
	*(U8 *)(AD1+14)=asicc(a2);
	*(U8 *)(AD1+15)=0x0d;      				//������Ԫ0d,0a
	*(U8 *)(AD1+16)=0x0a;
	if(ptcom->registerr=='c')
	{
        if (ptcom->address<200)
        {   
            ptcom->send_length[0]=17;               //���ͳ���
            ptcom->send_staradd[0]=0;               //�������ݴ洢��ַ  
            ptcom->send_times=1;                    //���ʹ���
                
            ptcom->return_length[0]=11+ptcom->register_length*4;//�������ݳ��ȣ���9���̶���3a��PLC��ַ2�����ȣ�������2�����ȣ�
                                                                //λ����ʾ2�����ȣ�У��2�����ȣ�0d��0a
            ptcom->return_start[0]=7;               //����������Ч��ʼ
            ptcom->return_length_available[0]=ptcom->register_length*4;//������Ч���ݳ���   
            ptcom->Current_Times=0;                 //��ǰ���ʹ���  
            ptcom->send_add[0]=ptcom->address;      //�����������ַ������
            
            ptcom->send_staradd[90]=1;
        }
        if (ptcom->address>=200)
        {   
            ptcom->send_length[0]=17;               //���ͳ���
            ptcom->send_staradd[0]=0;               //�������ݴ洢��ַ  
            ptcom->send_times=1;                    //���ʹ���
                
            ptcom->return_length[0]=11+(ptcom->register_length/2)*8;//�������ݳ��ȣ���9���̶���3a��PLC��ַ2�����ȣ�������2�����ȣ�
                                                                //λ����ʾ2�����ȣ�У��2�����ȣ�0d��0a
            ptcom->return_start[0]=7;               //����������Ч��ʼ
            ptcom->return_length_available[0]=ptcom->register_length*8;//������Ч���ݳ���   
            ptcom->Current_Times=0;                 //��ǰ���ʹ���  
            ptcom->send_add[0]=ptcom->address;      //�����������ַ������

            ptcom->register_length=ptcom->register_length;
            
            ptcom->send_staradd[90]=2;
        }
	}
	else
	{
        ptcom->send_length[0]=17;               //���ͳ���
        ptcom->send_staradd[0]=0;               //�������ݴ洢��ַ  
        ptcom->send_times=1;                    //���ʹ���
            
        ptcom->return_length[0]=11+ptcom->register_length*4;//�������ݳ��ȣ���9���̶���3a��PLC��ַ2�����ȣ�������2�����ȣ�
                                                            //λ����ʾ2�����ȣ�У��2�����ȣ�0d��0a
        ptcom->return_start[0]=7;               //����������Ч��ʼ
        ptcom->return_length_available[0]=ptcom->register_length*4;//������Ч���ݳ���   
        ptcom->Current_Times=0;                 //��ǰ���ʹ���  
        ptcom->send_add[0]=ptcom->address;      //�����������ַ������
        
        ptcom->send_staradd[90]=1;

	}

}
/************************************************************************************************************************
дģ�����Ӻ���
*************************************************************************************************************************/
void Write_Analog()							//дģ����
{	
	switch (ptcom->registerr)				//���ݼĴ������ͻ��ƫ�Ƶ�ַ
	{
	case 'D':
		Write_Analog_D();					//D�Ĵ�����ַ������������зֶβ���					
		break;
	case 't':	
		Write_Analog_t();					//tc�Ĵ�����ַ�������ģ���������
		break;						
	case 'c':
		Write_Analog_c();					//tc�Ĵ�����ַ�������ģ���������
		break;						
	}	
	ptcom->send_staradd[90]=0;	
}
/************************************************************************************************************************
д����ΪD�ļĴ����������ַ���������߽��ַΪ4095����ֶν��в���
*************************************************************************************************************************/
void Write_Analog_D()
{
	U16 aakj;
	int b,t,k1,k2,k3,k4,k5,k6,k7,k8;
	int a1,a2,a3,a4;
	int add,add_1;
	int b1,b2,b3,b4;
	int i;
	int length,length_1,length_2;
	int t1,t2,t3,t4;
	int plcadd;
	
	plcadd=ptcom->plc_address;	            //PLCվ��ַ	
	b=ptcom->address;						//��ʼ��ַ	
	length=ptcom->register_length;
	
	/*if(length==1)                   		//������ݽ϶̣�С��65535����length==1
	{
		if (b<=4095)
		{
			add=0x1000;
		}
		if (b>=4096)
		{
			b=b-4096;
			add=0x9000;
		}
		
		b=b+add;							//����ƫ�Ƶ�ַ
		b1=(b&0xff00)>>8;           		//�Կ�ʼƫ�Ƶ�ַȡ�ߵ�λ��b1��λ��b2��λ��16����
		b2=b&0xff;
		a1=b&0xf000;                		//�Կ�ʼƫ�Ƶ�ַ��asicc�������δӸߵ���
		a1=a1>>12;
		a1=a1&0xf;
		a2=b&0xf00;
		a2=a2>>8;
		a2=a2&0xf;
		a3=b&0xf0;
		a3=a3>>4;
		a3=a3&0xf;
		a4=b&0xf;

		*(U8 *)(AD1+0)=0x3a;        		//̨�￪ʼ��Ԫ":",��3a
		*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);//PLCվ��ַ��Ϊasicc����ʾ
		*(U8 *)(AD1+2)=asicc(plcadd&0xf);   
		*(U8 *)(AD1+3)=0x30;        		//������06��Ҫת��asicc��          
		*(U8 *)(AD1+4)=0x36;		
		*(U8 *)(AD1+5)=asicc(a1);			//��ʼ��ַ�����δӸߵ��ͣ�Ҫת��asicc��
		*(U8 *)(AD1+6)=asicc(a2);
		*(U8 *)(AD1+7)=asicc(a3);
		*(U8 *)(AD1+8)=asicc(a4);
		
		b4=ptcom->U8_Data[0];       		//��D[]������Ҫ���ݣ���Ӧ��b3Ϊ��λ��b4Ϊ��λ
		b3=ptcom->U8_Data[1];
		
		b5=(b3&0xf0)>>4;            		//�����ݽ���asicc��������ʾasicc�룬���δӸ�λ����λ
		b6=b3&0xf;
		b7=(b4&0xf0)>>4;
		b8=b4&0xf;
		*(U8 *)(AD1+9)=asicc(b5);
		*(U8 *)(AD1+10)=asicc(b6);
		*(U8 *)(AD1+11)=asicc(b7);
		*(U8 *)(AD1+12)=asicc(b8);
		KK[1]=plcadd&0xff;                 		//LRCУ�飬16����У�飬asicc����ʾ 
		KK[2]=0x06;
		KK[3]=b1;
		KK[4]=b2;
		KK[5]=b3;
		KK[6]=b4;
	
		aakj=CalLRC(KK,7);	        		//�����У�죬LRCУ�飬��λ��ǰ����λ�ں�
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13)=asicc(a1);  		//asicc����ʾ
		*(U8 *)(AD1+14)=asicc(a2); 
		*(U8 *)(AD1+15)=0x0d;       		//������Ԫ0d��0a
		*(U8 *)(AD1+16)=0x0a;
		
		ptcom->send_length[0]=17;			//���ͳ���
		ptcom->send_staradd[0]=0;			//�������ݴ洢��ַ	
		ptcom->send_times=1;				//���ʹ���
				
		ptcom->return_length[0]=17;			//�������ݳ���
		ptcom->return_start[0]=0;			//����������Ч��ʼ
		ptcom->return_length_available[0]=0;//������Ч���ݳ���	
		ptcom->Current_Times=0;	*/	
	//}
	//if(length!=1)                   		//������ݽϳ�������65535����length��=1  
	//{
		if (b<=4095 && b+length>4096)
		{
			add=0x1000;
			add_1=b;
			length_1=4096-b;
			
			add_1=add_1+add;				//����ƫ�Ƶ�ַ
			
			b1=(add_1&0xff00)>>8;           //�Կ�ʼƫ�Ƶ�ַȡ�ߵ�λ��b1��λ��b2��λ��16����
			b2=add_1&0xff;
			a1=add_1&0xf000;                //�Կ�ʼƫ�Ƶ�ַ��asicc�������δӸߵ���
			a1=a1>>12;
			a1=a1&0xf;
			a2=add_1&0xf00;
			a2=a2>>8;
			a2=a2&0xf;
			a3=add_1&0xf0;
			a3=a3>>4;
			a3=a3&0xf;
			a4=add_1&0xf;	
			
			*(U8 *)(AD1+0)=0x3a;        	//̨�￪ʼ��Ԫ":",��3a      
			*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);//PLCվ��ַ��Ϊasicc����ʾ
			*(U8 *)(AD1+2)=asicc(plcadd&0xf); 
			*(U8 *)(AD1+3)=0x31;        	//������16��ת��asicc��
			*(U8 *)(AD1+4)=0x30;		
			*(U8 *)(AD1+5)=asicc(a1);		//��ʼ��ַ�����δӸߵ��ͣ�Ҫת��asicc��
			*(U8 *)(AD1+6)=asicc(a2);
			*(U8 *)(AD1+7)=asicc(a3);
			*(U8 *)(AD1+8)=asicc(a4);
			
			t=length_1;   					//�����ݳ���ȡ�ߵ�λ��b1��λ��b2��λ��16����  
			b3=(t&0xff00)>>8;
			b4=t&0xff;
			t1=t&0xf000;                	//�����ݳ��Ƚ���asicc��������ʾasicc�룬���δӸ�λ����λ
			t1=t1>>12;
			t1=t1&0xf;
			t2=t&0xf00;
			t2=t2>>8;
			t2=t2&0xf;
			t3=t&0xf0;
			t3=t3>>4;
			t3=t3&0xf;
			t4=t&0xf;
			
			*(U8 *)(AD1+9)=asicc(t1);		//���ͳ��ȣ����δӸߵ��ͣ�Ҫת��asicc��
			*(U8 *)(AD1+10)=asicc(t2);
			*(U8 *)(AD1+11)=asicc(t3);
			*(U8 *)(AD1+12)=asicc(t4);
			
			KK[1]=plcadd&0xff;                 	//LRCУ�飬16����У�飬asicc����ʾ 
			KK[2]=0x10;
			KK[3]=b1;
			KK[4]=b2;
			KK[5]=b3;
			KK[6]=b4;
							
			k1=((length_1*2)&0xf0)>>4;    	//������λ���Ƚ���asicc��������ʾasicc�룬���δӸ�λ����λ
			k1=k1&0xf;
			k2=(length_1*2)&0xf;
			*(U8 *)(AD1+13)=asicc(k1);  	//����λ�ĳ��ȣ����δӸߵ��ͣ�Ҫת��asicc��
			*(U8 *)(AD1+14)=asicc(k2);
			KK[7]=length_1*2;	
						

			for (i=0;i<length_1;i++)      	//д����Ԫ��ֵ
			{				
				k4=ptcom->U8_Data[i*2];     //��D[]������Ҫ���ݣ���Ӧ��k3Ϊ��λ��k4Ϊ��λ
				k3=ptcom->U8_Data[i*2+1];
							
				k5=(k3&0xf0)>>4;            //�����ݽ���asicc��������ʾasicc�룬���δӸ�λ����λ
				k6=k3&0xf;
				k7=(k4&0xf0)>>4;
				k8=k4&0xf;
				*(U8 *)(AD1+15+i*4)=asicc(k5);//����λ�����ݣ����δӸߵ��ͣ�Ҫת��asicc��
				*(U8 *)(AD1+16+i*4)=asicc(k6);
				*(U8 *)(AD1+17+i*4)=asicc(k7);
				*(U8 *)(AD1+18+i*4)=asicc(k8);			
					
				KK[8+i*2]=k3;
				KK[9+i*2]=k4;		
			}
						
			aakj=CalLRC(KK,length_1*2+8);	//�����У�죬LRCУ�飬��λ��ǰ����λ�ں�
			a1=(aakj&0xf0)>>4;
			a1=a1&0xf;
			a2=aakj&0xf;
			*(U8 *)(AD1+19+(length_1-1)*4)=asicc(a1);//asicc����ʾ
			*(U8 *)(AD1+20+(length_1-1)*4)=asicc(a2);
			*(U8 *)(AD1+21+(length_1-1)*4)=0x0d;     //������Ԫ0d��0a
			*(U8 *)(AD1+22+(length_1-1)*4)=0x0a;
			
			ptcom->send_length[0]=19+length_1*4;//���ͳ���
			ptcom->send_staradd[0]=0;		//�������ݴ洢��ַ						
			ptcom->return_length[0]=17;		//�������ݳ���
			ptcom->return_start[0]=0;		//����������Ч��ʼ
			ptcom->return_length_available[0]=0;//������Ч���ݳ���	
//-------------------------------------------------------------------------------------------------------------------
			add=0x9000;		
			length_2=length-length_1;
			
			b1=(add&0xff00)>>8;           	//�Կ�ʼƫ�Ƶ�ַȡ�ߵ�λ��b1��λ��b2��λ��16����
			b2=add&0xff;	
			a1=add&0xf000;                	//�Կ�ʼƫ�Ƶ�ַ��asicc�������δӸߵ��� 
			a1=a1>>12;
			a1=a1&0xf;
			a2=add&0xf00;
			a2=a2>>8;
			a2=a2&0xf;
			a3=add&0xf0;
			a3=a3>>4;
			a3=a3&0xf;
			a4=add&0xf;	
			
			*(U8 *)(AD1+23+(length_1-1)*4)=0x3a;//̨�￪ʼ��Ԫ":",��3a      
			*(U8 *)(AD1+24+(length_1-1)*4)=asicc(((plcadd&0xf0)>>4)&0xf);//PLCվ��ַ��Ϊasicc����ʾ
			*(U8 *)(AD1+25+(length_1-1)*4)=asicc(plcadd&0xf); 
			*(U8 *)(AD1+26+(length_1-1)*4)=0x31; //������16��ת��asicc��
			*(U8 *)(AD1+27+(length_1-1)*4)=0x30;		
			*(U8 *)(AD1+28+(length_1-1)*4)=asicc(a1);//��ʼ��ַ�����δӸߵ��ͣ�Ҫת��asicc��
			*(U8 *)(AD1+29+(length_1-1)*4)=asicc(a2);
			*(U8 *)(AD1+30+(length_1-1)*4)=asicc(a3);
			*(U8 *)(AD1+31+(length_1-1)*4)=asicc(a4);		

			t=length_2;   						//�����ݳ���ȡ�ߵ�λ��b1��λ��b2��λ��16����  
			b3=(t&0xff00)>>8;
			b4=t&0xff;
			t1=t&0xf000;                		//�����ݳ��Ƚ���asicc��������ʾasicc�룬���δӸ�λ����λ
			t1=t1>>12;
			t1=t1&0xf;
			t2=t&0xf00;
			t2=t2>>8;
			t2=t2&0xf;
			t3=t&0xf0;
			t3=t3>>4;
			t3=t3&0xf;
			t4=t&0xf;

			*(U8 *)(AD1+32+(length_1-1)*4)=asicc(t1);//���ͳ��ȣ����δӸߵ��ͣ�Ҫת��asicc��
			*(U8 *)(AD1+33+(length_1-1)*4)=asicc(t2);
			*(U8 *)(AD1+34+(length_1-1)*4)=asicc(t3);
			*(U8 *)(AD1+35+(length_1-1)*4)=asicc(t4);
			
			KK[1]=plcadd&0xff;                 		//LRCУ�飬16����У�飬asicc����ʾ 
			KK[2]=0x10;
			KK[3]=b1;
			KK[4]=b2;
			KK[5]=b3;
			KK[6]=b4;
							
			k1=((length_2*2)&0xf0)>>4;    		//������λ���Ƚ���asicc��������ʾasicc�룬���δӸ�λ����λ
			k1=k1&0xf;
			k2=(length_2*2)&0xf;
			*(U8 *)(AD1+36+(length_1-1)*4)=asicc(k1);//����λ�ĳ��ȣ����δӸߵ��ͣ�Ҫת��asicc��
			*(U8 *)(AD1+37+(length_1-1)*4)=asicc(k2);
			KK[7]=length_2*2;	
						

			for (i=length_1;i<length;i++)      	//д����Ԫ��ֵ
			{				
				k4=ptcom->U8_Data[i*2];         //��D[]������Ҫ���ݣ���Ӧ��k3Ϊ��λ��k4Ϊ��λ
				k3=ptcom->U8_Data[i*2+1];
							
				k5=(k3&0xf0)>>4;                //�����ݽ���asicc��������ʾasicc�룬���δӸ�λ����λ
				k6=k3&0xf;
				k7=(k4&0xf0)>>4;
				k8=k4&0xf;
				*(U8 *)(AD1+38+(length_1-1)*4+(i-length_1)*4)=asicc(k5);//����λ�����ݣ����δӸߵ��ͣ�Ҫת��asicc��
				*(U8 *)(AD1+39+(length_1-1)*4+(i-length_1)*4)=asicc(k6);
				*(U8 *)(AD1+40+(length_1-1)*4+(i-length_1)*4)=asicc(k7);
				*(U8 *)(AD1+41+(length_1-1)*4+(i-length_1)*4)=asicc(k8);			
					
				KK[8+(i-length_1)*2]=k3;
				KK[9+(i-length_1)*2]=k4;		
			}
						
			aakj=CalLRC(KK,length_2*2+8);	     //�����У�죬LRCУ�飬��λ��ǰ����λ�ں�
			a1=(aakj&0xf0)>>4;
			a1=a1&0xf;
			a2=aakj&0xf;
			*(U8 *)(AD1+42+(length_1-1)*4+(length_2-1)*4)=asicc(a1);//asicc����ʾ
			*(U8 *)(AD1+43+(length_1-1)*4+(length_2-1)*4)=asicc(a2);
			*(U8 *)(AD1+44+(length_1-1)*4+(length_2-1)*4)=0x0d;     //������Ԫ0d��0a
			*(U8 *)(AD1+45+(length_1-1)*4+(length_2-1)*4)=0x0a;
			
			ptcom->send_length[1]=19+length_2*4;	//���ͳ���
			ptcom->send_staradd[1]=19+length_1*4;	//�������ݴ洢��ַ						
			ptcom->return_length[1]=17;				//�������ݳ���
			ptcom->return_start[1]=0;				//����������Ч��ʼ
			ptcom->return_length_available[1]=0;	//������Ч���ݳ���	
			
			ptcom->Current_Times=0;					//��ǰ���ʹ���
			ptcom->send_times=2;					//���ʹ���						
		}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////			
		else
		{
			if (b<=4095 && b+length<=4096)
			{
				add=0x1000;
			}
			if (b>=4096)
			{
				b=b-4096;
				add=0x9000;
			}
			
			b=b+add;								//����ƫ�Ƶ�ַ
			b1=(b&0xff00)>>8;           			//�Կ�ʼƫ�Ƶ�ַȡ�ߵ�λ��b1��λ��b2��λ��16����
			b2=b&0xff;
			a1=b&0xf000;                			//�Կ�ʼƫ�Ƶ�ַ��asicc�������δӸߵ���
			a1=a1>>12;
			a1=a1&0xf;
			a2=b&0xf00;
			a2=a2>>8;
			a2=a2&0xf;
			a3=b&0xf0;
			a3=a3>>4;
			a3=a3&0xf;
			a4=b&0xf;

			*(U8 *)(AD1+0)=0x3a;        			//̨�￪ʼ��Ԫ":",��3a      
			*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);//PLCվ��ַ��Ϊasicc����ʾ
			*(U8 *)(AD1+2)=asicc(plcadd&0xf); 
			*(U8 *)(AD1+3)=0x31;        			//������16��ת��asicc��
			*(U8 *)(AD1+4)=0x30;		
			*(U8 *)(AD1+5)=asicc(a1);				//��ʼ��ַ�����δӸߵ��ͣ�Ҫת��asicc��
			*(U8 *)(AD1+6)=asicc(a2);
			*(U8 *)(AD1+7)=asicc(a3);
			*(U8 *)(AD1+8)=asicc(a4);
			
			t=ptcom->register_length;   			//�����ݳ���ȡ�ߵ�λ��b1��λ��b2��λ��16����  
			b3=(t&0xff00)>>8;
			b4=t&0xff;
			t1=t&0xf000;                			//�����ݳ��Ƚ���asicc��������ʾasicc�룬���δӸ�λ����λ
			t1=t1>>12;
			t1=t1&0xf;
			t2=t&0xf00;
			t2=t2>>8;
			t2=t2&0xf;
			t3=t&0xf0;
			t3=t3>>4;
			t3=t3&0xf;
			t4=t&0xf;
			*(U8 *)(AD1+9)=asicc(t1);				//���ͳ��ȣ����δӸߵ��ͣ�Ҫת��asicc��
			*(U8 *)(AD1+10)=asicc(t2);
			*(U8 *)(AD1+11)=asicc(t3);
			*(U8 *)(AD1+12)=asicc(t4);
			
			KK[1]=plcadd&0xff;                 			//LRCУ�飬16����У�飬asicc����ʾ 
			KK[2]=0x10;
			KK[3]=b1;
			KK[4]=b2;
			KK[5]=b3;
			KK[6]=b4;
							
			k1=((length*2)&0xf0)>>4;    			//������λ���Ƚ���asicc��������ʾasicc�룬���δӸ�λ����λ
			k1=k1&0xf;
			k2=(length*2)&0xf;
			*(U8 *)(AD1+13)=asicc(k1);  			//����λ�ĳ��ȣ����δӸߵ��ͣ�Ҫת��asicc��
			*(U8 *)(AD1+14)=asicc(k2);
			KK[7]=length*2;	
						

			for (i=0;i<length;i++)      			//д����Ԫ��ֵ
			{				
				k4=ptcom->U8_Data[i*2];           	//��D[]������Ҫ���ݣ���Ӧ��k3Ϊ��λ��k4Ϊ��λ
				k3=ptcom->U8_Data[i*2+1];
							
				k5=(k3&0xf0)>>4;                 	//�����ݽ���asicc��������ʾasicc�룬���δӸ�λ����λ
				k6=k3&0xf;
				k7=(k4&0xf0)>>4;
				k8=k4&0xf;
				*(U8 *)(AD1+15+i*4)=asicc(k5);   	//����λ�����ݣ����δӸߵ��ͣ�Ҫת��asicc��
				*(U8 *)(AD1+16+i*4)=asicc(k6);
				*(U8 *)(AD1+17+i*4)=asicc(k7);
				*(U8 *)(AD1+18+i*4)=asicc(k8);			
					
				KK[8+i*2]=k3;
				KK[9+i*2]=k4;		
			}
					
			aakj=CalLRC(KK,length*2+8);	         	//�����У�죬LRCУ�飬��λ��ǰ����λ�ں�
			a1=(aakj&0xf0)>>4;
			a1=a1&0xf;
			a2=aakj&0xf;
			*(U8 *)(AD1+19+(length-1)*4)=asicc(a1);	//asicc����ʾ
			*(U8 *)(AD1+20+(length-1)*4)=asicc(a2);
			*(U8 *)(AD1+21+(length-1)*4)=0x0d;     	//������Ԫ0d��0a
			*(U8 *)(AD1+22+(length-1)*4)=0x0a;
			
			ptcom->send_length[0]=19+length*4;		//���ͳ���
			ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
			ptcom->send_times=1;					//���ʹ���
					
			ptcom->return_length[0]=17;				//�������ݳ���
			ptcom->return_start[0]=0;				//����������Ч��ʼ
			ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
			ptcom->Current_Times=0;			
		}						
	//}		
}
/************************************************************************************************************************
д����Ϊtc�ļĴ���
*************************************************************************************************************************/
void Write_Analog_t()
{
	U16 aakj;
	int b,t,k1,k2,k3,k4,k5,k6,k7,k8;
	int a1,a2,a3,a4;
	int add;
	int b1,b2,b3,b4,b5,b6,b7,b8;
	int i;
	int length;
	int t1,t2,t3,t4;
	int plcadd;

	plcadd=ptcom->plc_address;	            		//PLCվ��ַ	
	b=ptcom->address;								//��ʼ��ַ	
	length=ptcom->register_length;	
	
	switch (ptcom->registerr)						//���ݼĴ������ͻ��ƫ�Ƶ�ַ
	{
		case 't':
			add=0x600;
			break;		
	}	
    
	b=b+add;										//����ƫ�Ƶ�ַ
	b1=(b&0xff00)>>8;           					//�Կ�ʼƫ�Ƶ�ַȡ�ߵ�λ��b1��λ��b2��λ��16����
	b2=b&0xff;
	a1=b&0xf000;                					//�Կ�ʼƫ�Ƶ�ַ��asicc�������δӸߵ���
	a1=a1>>12;
	a1=a1&0xf;
	a2=b&0xf00;
	a2=a2>>8;
	a2=a2&0xf;
	a3=b&0xf0;
	a3=a3>>4;
	a3=a3&0xf;
	a4=b&0xf;
	if(length==1)                   				//������ݽ϶̣�С��65535����length==1
	{
		*(U8 *)(AD1+0)=0x3a;        				//̨�￪ʼ��Ԫ":",��3a
		*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);//PLCվ��ַ��Ϊasicc����ʾ
		*(U8 *)(AD1+2)=asicc(plcadd&0xf);   
		*(U8 *)(AD1+3)=0x30;        				//������06��Ҫת��asicc��          
		*(U8 *)(AD1+4)=0x36;		
		*(U8 *)(AD1+5)=asicc(a1);					//��ʼ��ַ�����δӸߵ��ͣ�Ҫת��asicc��
		*(U8 *)(AD1+6)=asicc(a2);
		*(U8 *)(AD1+7)=asicc(a3);
		*(U8 *)(AD1+8)=asicc(a4);
		
		b4=ptcom->U8_Data[0];       				//��D[]������Ҫ���ݣ���Ӧ��b3Ϊ��λ��b4Ϊ��λ
		b3=ptcom->U8_Data[1];
		
		b5=(b3&0xf0)>>4;            				//�����ݽ���asicc��������ʾasicc�룬���δӸ�λ����λ
		b6=b3&0xf;
		b7=(b4&0xf0)>>4;
		b8=b4&0xf;
		*(U8 *)(AD1+9)=asicc(b5);
		*(U8 *)(AD1+10)=asicc(b6);
		*(U8 *)(AD1+11)=asicc(b7);
		*(U8 *)(AD1+12)=asicc(b8);
		KK[1]=plcadd&0xff;                 				//LRCУ�飬16����У�飬asicc����ʾ 
		KK[2]=0x06;
		KK[3]=b1;
		KK[4]=b2;
		KK[5]=b3;
		KK[6]=b4;
	
		aakj=CalLRC(KK,7);	        				//�����У�죬LRCУ�飬��λ��ǰ����λ�ں�
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13)=asicc(a1);  				//asicc����ʾ
		*(U8 *)(AD1+14)=asicc(a2); 
		*(U8 *)(AD1+15)=0x0d;       				//������Ԫ0d��0a
		*(U8 *)(AD1+16)=0x0a;
		
		ptcom->send_length[0]=17;					//���ͳ���
		ptcom->send_staradd[0]=0;					//�������ݴ洢��ַ	
		ptcom->send_times=1;						//���ʹ���
				
		ptcom->return_length[0]=17;					//�������ݳ���
		ptcom->return_start[0]=0;					//����������Ч��ʼ
		ptcom->return_length_available[0]=0;		//������Ч���ݳ���	
		ptcom->Current_Times=0;	
	}
	if(length!=1)                  					//������ݽϳ�������65535����length��=1   
	{
		*(U8 *)(AD1+0)=0x3a;        				//̨�￪ʼ��Ԫ":",��3a      
		*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);//PLCվ��ַ��Ϊasicc����ʾ
		*(U8 *)(AD1+2)=asicc(plcadd&0xf); 
		*(U8 *)(AD1+3)=0x31;        				//������16��ת��asicc��
		*(U8 *)(AD1+4)=0x30;		
		*(U8 *)(AD1+5)=asicc(a1);					//��ʼ��ַ�����δӸߵ��ͣ�Ҫת��asicc��
		*(U8 *)(AD1+6)=asicc(a2);
		*(U8 *)(AD1+7)=asicc(a3);
		*(U8 *)(AD1+8)=asicc(a4);

        t=ptcom->register_length;                   //�����ݳ���ȡ�ߵ�λ��b1��λ��b2��λ��16����  
		
		b3=(t&0xff00)>>8;
		b4=t&0xff;
		t1=t&0xf000;                				//�����ݳ��Ƚ���asicc��������ʾasicc�룬���δӸ�λ����λ
		t1=t1>>12;
		t1=t1&0xf;
		t2=t&0xf00;
		t2=t2>>8;
		t2=t2&0xf;
		t3=t&0xf0;
		t3=t3>>4;
		t3=t3&0xf;
		t4=t&0xf;
		*(U8 *)(AD1+9)=asicc(t1);					//���ͳ��ȣ����δӸߵ��ͣ�Ҫת��asicc��
		*(U8 *)(AD1+10)=asicc(t2);
		*(U8 *)(AD1+11)=asicc(t3);
		*(U8 *)(AD1+12)=asicc(t4);
		
		KK[1]=plcadd&0xff;                 				//LRCУ�飬16����У�飬asicc����ʾ 
		KK[2]=0x10;
		KK[3]=b1;
		KK[4]=b2;
		KK[5]=b3;
		KK[6]=b4;
						
		k1=((length*2)&0xf0)>>4;    				//������λ���Ƚ���asicc��������ʾasicc�룬���δӸ�λ����λ
		k1=k1&0xf;
		k2=(length*2)&0xf;
		*(U8 *)(AD1+13)=asicc(k1);  				//����λ�ĳ��ȣ����δӸߵ��ͣ�Ҫת��asicc��
		*(U8 *)(AD1+14)=asicc(k2);
		KK[7]=length*2;	

        for (i=0;i<length;i++)                      //д����Ԫ��ֵ
        {               
            k4=ptcom->U8_Data[i*2];                 //��D[]������Ҫ���ݣ���Ӧ��k3Ϊ��λ��k4Ϊ��λ
            k3=ptcom->U8_Data[i*2+1];
                        
            k5=(k3&0xf0)>>4;                        //�����ݽ���asicc��������ʾasicc�룬���δӸ�λ����λ
            k6=k3&0xf;
            k7=(k4&0xf0)>>4;
            k8=k4&0xf;
            *(U8 *)(AD1+15+i*4)=asicc(k5);          //����λ�����ݣ����δӸߵ��ͣ�Ҫת��asicc��
            *(U8 *)(AD1+16+i*4)=asicc(k6);
            *(U8 *)(AD1+17+i*4)=asicc(k7);
            *(U8 *)(AD1+18+i*4)=asicc(k8);          
                
            KK[8+i*2]=k3;
            KK[9+i*2]=k4;       
        }   

        aakj=CalLRC(KK,length*2+8);	         	//�����У�죬LRCУ�飬��λ��ǰ����λ�ں�
        a1=(aakj&0xf0)>>4;
        a1=a1&0xf;
        a2=aakj&0xf;
        *(U8 *)(AD1+19+(length-1)*4)=asicc(a1);	//asicc����ʾ
        *(U8 *)(AD1+20+(length-1)*4)=asicc(a2);
        *(U8 *)(AD1+21+(length-1)*4)=0x0d;     	//������Ԫ0d��0a
        *(U8 *)(AD1+22+(length-1)*4)=0x0a;
        
        ptcom->send_length[0]=19+length*4;			//���ͳ���
        ptcom->send_staradd[0]=0;					//�������ݴ洢��ַ	
        ptcom->send_times=1;						//���ʹ���
        		
        ptcom->return_length[0]=17;				//�������ݳ���
        ptcom->return_start[0]=0;					//����������Ч��ʼ
        ptcom->return_length_available[0]=0;		//������Ч���ݳ���	
        ptcom->Current_Times=0;	

	}	
}


/************************************************************************************************************************
д����Ϊtc�ļĴ���
*************************************************************************************************************************/
void Write_Analog_c()
{
	U16 aakj;
	int b,t,k1,k2,k3,k4,k5,k6,k7,k8,k9,k10,k11,k12,k13,k14;
	int a1,a2,a3,a4;
	int add;
	int b1,b2,b3,b4,b5,b6,b7,b8;
	int i;
	int length,length1;
	int t1,t2,t3,t4;
	int plcadd;
	
	plcadd=ptcom->plc_address;	            		//PLCվ��ַ	
	b=ptcom->address;								//��ʼ��ַ	
	length=ptcom->register_length;	

	switch (ptcom->registerr)						//���ݼĴ������ͻ��ƫ�Ƶ�ַ
	{
		case 'c':
			add=0xe00;
			break;						
	}	
	if(ptcom->registerr=='c')
	{
        if (b>=200)
        {
            b=((b-200)/2)+200;
        }
	} 
	
	b=b+add;										//����ƫ�Ƶ�ַ
	b1=(b&0xff00)>>8;           					//�Կ�ʼƫ�Ƶ�ַȡ�ߵ�λ��b1��λ��b2��λ��16����
	b2=b&0xff;
	a1=b&0xf000;                					//�Կ�ʼƫ�Ƶ�ַ��asicc�������δӸߵ���
	a1=a1>>12;
	a1=a1&0xf;
	a2=b&0xf00;
	a2=a2>>8;
	a2=a2&0xf;
	a3=b&0xf0;
	a3=a3>>4;
	a3=a3&0xf;
	a4=b&0xf;
	
	if(length==1)                   				//������ݽ϶̣�С��65535����length==1
	{
	
		*(U8 *)(AD1+0)=0x3a;        				//̨�￪ʼ��Ԫ":",��3a
		*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);//PLCվ��ַ��Ϊasicc����ʾ
		*(U8 *)(AD1+2)=asicc(plcadd&0xf);   
		*(U8 *)(AD1+3)=0x30;        				//������06��Ҫת��asicc��          
		*(U8 *)(AD1+4)=0x36;		
		*(U8 *)(AD1+5)=asicc(a1);					//��ʼ��ַ�����δӸߵ��ͣ�Ҫת��asicc��
		*(U8 *)(AD1+6)=asicc(a2);
		*(U8 *)(AD1+7)=asicc(a3);
		*(U8 *)(AD1+8)=asicc(a4);
		
		b4=ptcom->U8_Data[0];       				//��D[]������Ҫ���ݣ���Ӧ��b3Ϊ��λ��b4Ϊ��λ
		b3=ptcom->U8_Data[1];
		
		b5=(b3&0xf0)>>4;            				//�����ݽ���asicc��������ʾasicc�룬���δӸ�λ����λ
		b6=b3&0xf;
		b7=(b4&0xf0)>>4;
		b8=b4&0xf;
		*(U8 *)(AD1+9)=asicc(b5);
		*(U8 *)(AD1+10)=asicc(b6);
		*(U8 *)(AD1+11)=asicc(b7);
		*(U8 *)(AD1+12)=asicc(b8);
		KK[1]=plcadd&0xff;                 				//LRCУ�飬16����У�飬asicc����ʾ 
		KK[2]=0x06;
		KK[3]=b1;
		KK[4]=b2;
		KK[5]=b3;
		KK[6]=b4;
	
		aakj=CalLRC(KK,7);	        				//�����У�죬LRCУ�飬��λ��ǰ����λ�ں�
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13)=asicc(a1);  				//asicc����ʾ
		*(U8 *)(AD1+14)=asicc(a2); 
		*(U8 *)(AD1+15)=0x0d;       				//������Ԫ0d��0a
		*(U8 *)(AD1+16)=0x0a;
		
		ptcom->send_length[0]=17;					//���ͳ���
		ptcom->send_staradd[0]=0;					//�������ݴ洢��ַ	
		ptcom->send_times=1;						//���ʹ���
				
		ptcom->return_length[0]=17;					//�������ݳ���
		ptcom->return_start[0]=0;					//����������Ч��ʼ
		ptcom->return_length_available[0]=0;		//������Ч���ݳ���	
		ptcom->Current_Times=0;	
	}
	if(length!=1)                  					//������ݽϳ�������65535����length��=1   
	{
		*(U8 *)(AD1+0)=0x3a;        				//̨�￪ʼ��Ԫ":",��3a      
		*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);//PLCվ��ַ��Ϊasicc����ʾ
		*(U8 *)(AD1+2)=asicc(plcadd&0xf); 
		*(U8 *)(AD1+3)=0x31;        				//������16��ת��asicc��
		*(U8 *)(AD1+4)=0x30;		
		*(U8 *)(AD1+5)=asicc(a1);					//��ʼ��ַ�����δӸߵ��ͣ�Ҫת��asicc��
		*(U8 *)(AD1+6)=asicc(a2);
		*(U8 *)(AD1+7)=asicc(a3);
		*(U8 *)(AD1+8)=asicc(a4);

        t=ptcom->register_length/2;                   //�����ݳ���ȡ�ߵ�λ��b1��λ��b2��λ��16����  
        length1 = ptcom->register_length/2; 

		b3=(t&0xff00)>>8;
		b4=t&0xff;
		t1=t&0xf000;                				//�����ݳ��Ƚ���asicc��������ʾasicc�룬���δӸ�λ����λ
		t1=t1>>12;
		t1=t1&0xf;
		t2=t&0xf00;
		t2=t2>>8;
		t2=t2&0xf;
		t3=t&0xf0;
		t3=t3>>4;
		t3=t3&0xf;
		t4=t&0xf;
		*(U8 *)(AD1+9)=asicc(t1);					//���ͳ��ȣ����δӸߵ��ͣ�Ҫת��asicc��
		*(U8 *)(AD1+10)=asicc(t2);
		*(U8 *)(AD1+11)=asicc(t3);
		*(U8 *)(AD1+12)=asicc(t4);
		
		KK[1]=plcadd&0xff;                 				//LRCУ�飬16����У�飬asicc����ʾ 
		KK[2]=0x10;
		KK[3]=b1;
		KK[4]=b2;
		KK[5]=b3;
		KK[6]=b4;
						
		k1=((length*2)&0xf0)>>4;    				//������λ���Ƚ���asicc��������ʾasicc�룬���δӸ�λ����λ
		k1=k1&0xf;
		k2=(length*2)&0xf;
		*(U8 *)(AD1+13)=asicc(k1);  				//����λ�ĳ��ȣ����δӸߵ��ͣ�Ҫת��asicc��
		*(U8 *)(AD1+14)=asicc(k2);
		KK[7]=length*2;	

            
        for (i=0;i<length1;i++)                    //д����Ԫ��ֵ
        {               
            k4=ptcom->U8_Data[i*4];                 //��D[]������Ҫ���ݣ���Ӧ��k3Ϊ��λ��k4Ϊ��λ
            k3=ptcom->U8_Data[i*4+1];
            k9=ptcom->U8_Data[i*4+2];               //��D[]������Ҫ���ݣ���Ӧ��k3Ϊ��λ��k4Ϊ��λ
            k10=ptcom->U8_Data[i*4+3];
                        
            k5=(k3&0xf0)>>4;                        //�����ݽ���asicc��������ʾasicc�룬���δӸ�λ����λ
            k6=k3&0xf;
            k7=(k4&0xf0)>>4;
            k8=k4&0xf;
            k11=(k10&0xf0)>>4;                      //�����ݽ���asicc��������ʾasicc�룬���δӸ�λ����λ
            k12=k10&0xf;
            k13=(k9&0xf0)>>4;
            k14=k9&0xf;             
    
            *(U8 *)(AD1+15+i*8)=asicc(k11);         //����λ�����ݣ����δӸߵ��ͣ�Ҫת��asicc��
            *(U8 *)(AD1+16+i*8)=asicc(k12);
            *(U8 *)(AD1+17+i*8)=asicc(k13);
            *(U8 *)(AD1+18+i*8)=asicc(k14); 
    
            *(U8 *)(AD1+19+i*8)=asicc(k5);          //����λ�����ݣ����δӸߵ��ͣ�Ҫת��asicc��
            *(U8 *)(AD1+20+i*8)=asicc(k6);
            *(U8 *)(AD1+21+i*8)=asicc(k7);
            *(U8 *)(AD1+22+i*8)=asicc(k8);          
    
            KK[8+i*4]=k10;
            KK[9+i*4]=k9;                   
            KK[10+i*4]=k3;
            KK[11+i*4]=k4;      
        }
        
        aakj=CalLRC(KK,length1*4+8);	         		//�����У�죬LRCУ�飬��λ��ǰ����λ�ں�
        a1=(aakj&0xf0)>>4;
        a1=a1&0xf;
        a2=aakj&0xf;
        
        *(U8 *)(AD1+23+(length1-1)*8)=asicc(a1);		//asicc����ʾ
        *(U8 *)(AD1+24+(length1-1)*8)=asicc(a2);
        *(U8 *)(AD1+25+(length1-1)*8)=0x0d;     		//������Ԫ0d��0a
        *(U8 *)(AD1+26+(length1-1)*8)=0x0a;
                    
        ptcom->send_length[0]=19+length1*8;			//���ͳ���
        ptcom->send_staradd[0]=0;					//�������ݴ洢��ַ	
        ptcom->send_times=1;						//���ʹ���
        		
        ptcom->return_length[0]=17;					//�������ݳ���
        ptcom->return_start[0]=0;					//����������Ч��ʼ
        ptcom->return_length_available[0]=0;		//������Ч���ݳ���	
        ptcom->Current_Times=0;	
		
	}	
}



/************************************************************************************************************************
дʱ���Ӻ���
*************************************************************************************************************************/
void Write_Time()                                	//дʱ�䵽PLC
{
	Write_Analog();									
}
/************************************************************************************************************************
��ʱ���Ӻ���
*************************************************************************************************************************/
void Read_Time()										//��PLC��ȡʱ��
{
	Read_Analog();
}
/************************************************************************************************************************
���䷽�Ӻ���
*************************************************************************************************************************/
void Read_Recipe()									//��ȡ�䷽
{
	U16 aakj;
	int b,b1,b2;
	int a1,a2,a3,a4;
	int i;
	int datalength;                             	//���ݳ���
	int p_start;                                	//���ݿ�ʼ��ַ
	int ps;
	int SendTimes;                              	//���ʹ���
	int LastTimeWord;								//���һ�η��ͳ���
	int currentlength;
	int plcadd;                                 	//PLCվ��ַ
	int add,add_1;
	int length_1,length_2;
	int SendTimes_1,SendTimes_2;
	int LastTimeWord_1,LastTimeWord_2;
	
		
	datalength=ptcom->register_length;				//�����ܳ���
	p_start=ptcom->address;							//��ʼ��ַ
	plcadd=ptcom->plc_address;	            		//PLCվ��ַ

	if(datalength>5000)                     		//���Ƴ���
	{
		datalength=5000;
	}

	if ((p_start<=4095 && p_start+datalength<=4096) || (p_start>=4096))
	{	
		if(datalength%15==0)                    	//̨������ܷ�15��D�����ݸպ���15D�ı���
		{
			SendTimes=datalength/15;            	//���ʹ���
			LastTimeWord=15;                    	//���һ�η��͵ĳ���Ϊ15D	
		}
		if(datalength%15!=0)                    	//̨������ܷ�15��D�����ݲ���15D�ı��� 
		{
			SendTimes=datalength/15+1;          	//���͵Ĵ���
			LastTimeWord=datalength%15;         	//���һ�η��͵ĳ���Ϊ��16������	
		}
		

		for (i=0;i<SendTimes;i++)
		{
			ps=i*17;                            	//ÿ�η�17������
			b=p_start+i*15;                     	//��ʼ��ַ
			if (b<=0xfff)
			{
				b=b+0x1000;
			}
			else
			{
				b=b-4096;
				b=b+0x9000;
			}
			b1=(b&0xff00)>>8;                   	//�Կ�ʼƫ�Ƶ�ַȡ�ߵ�λ��b1��λ��b2��λ��16����
			b2=b&0xff;	
			a1=b&0xf000;                        	//�Կ�ʼƫ�Ƶ�ַ��asicc�������δӸߵ��� 
			a1=a1>>12;
			a1=a1&0xf;
			a2=b&0xf00;
			a2=a2>>8;
			a2=a2&0xf;
			a3=b&0xf0;
			a3=a3>>4;
			a3=a3&0xf;
			a4=b&0xf;
			*(U8 *)(AD1+0+ps)=0x3a;             	//̨�￪ʼ��Ԫ":",��3a
			*(U8 *)(AD1+1+ps)=asicc(((plcadd&0xf0)>>4)&0xf);//PLCվ��ַ��Ϊasicc����ʾ
			*(U8 *)(AD1+2+ps)=asicc(plcadd&0xf);
			*(U8 *)(AD1+3+ps)=0x30;            	 	//������03��Ҫת��asicc��
			*(U8 *)(AD1+4+ps)=0x33;	
			*(U8 *)(AD1+5+ps)=asicc(a1);	    	//��ʼ��ַ�����δӸߵ��ͣ�Ҫת��asicc��
			*(U8 *)(AD1+6+ps)=asicc(a2);
			*(U8 *)(AD1+7+ps)=asicc(a3);
			*(U8 *)(AD1+8+ps)=asicc(a4);
			KK[1]=plcadd;                       	//LRCУ�飬16����У�飬asicc����ʾ                     
			KK[2]=0x03;
			KK[3]=b1;
			KK[4]=b2;
			
			if (i!=(SendTimes-1))	            	//�������һ�η���ʱ
			{
				*(U8 *)(AD1+9+ps)=0x30;         	//�̶�����15��D����32�ֽڣ���λ��asicc����ʾ
				*(U8 *)(AD1+10+ps)=0x30;
				*(U8 *)(AD1+11+ps)=0x30;        	//�̶�����15��D����32�ֽڣ���λ��asicc����ʾ
				*(U8 *)(AD1+12+ps)=0x46;
				KK[5]=0x00;
				KK[6]=0x0f;
				currentlength=15;               	//�̶�����16��D               
			}
			if (i==(SendTimes-1))	            	//���һ�η���ʱ
			{		
				*(U8 *)(AD1+9+ps)=0x30;         	//ʣ��LastTimeWord��D����λ��ת��asicc��
				*(U8 *)(AD1+10+ps)=0x30;
				*(U8 *)(AD1+11+ps)=asicc(((LastTimeWord&0xf0)>>4)&0xf);//ʣ��LastTimeWord��D��ת��asicc��
				*(U8 *)(AD1+12+ps)=asicc(LastTimeWord&0xf);
				KK[5]=0x00;
				KK[6]=LastTimeWord&0xff;
				currentlength=LastTimeWord;     	//ʣ��LastTimeWord��D 
			}
			aakj=CalLRC(KK,7);	                	//�����У�죬LRCУ�飬��λ��ǰ����λ�ں�
			a1=(aakj&0xf0)>>4;
			a1=a1&0xf;
			a2=aakj&0xf;
			*(U8 *)(AD1+13+ps)=asicc(a1);       	//asicc����ʾ
			*(U8 *)(AD1+14+ps)=asicc(a2);
			*(U8 *)(AD1+15+ps)=0x0d;            	//������Ԫ0d,0a
			*(U8 *)(AD1+16+ps)=0x0a;
						
			ptcom->send_length[i]=17;				//���ͳ���
			ptcom->send_staradd[i]=i*17;			//�������ݴ洢��ַ	
			ptcom->send_add[i]=p_start+i*15;		//�����������ַ������	
			ptcom->send_data_length[i]=currentlength;//�������һ�ζ���15��D
					
			ptcom->return_length[i]=11+currentlength*4;//�������ݳ��ȣ���9���̶���3a��PLC��ַ2�����ȣ�������2�����ȣ�
		                                            //λ����ʾ2�����ȣ�У��2�����ȣ�0d��0a
			ptcom->return_start[i]=7;				//����������Ч��ʼ
			ptcom->return_length_available[i]=currentlength*4;//������Ч���ݳ���		
		}
		ptcom->send_times=SendTimes;				//���ʹ���
		ptcom->Current_Times=0;	
	}
/////////////////////////////////////////////////////////////////////////////////////	
	else
	{
		add=p_start;
		length_1=4096-p_start;
		
		if(length_1%15==0)                    		//̨������ܷ�15��D�����ݸպ���15D�ı���
		{
			SendTimes_1=length_1/15;            	//���ʹ���
			LastTimeWord_1=15;                    	//���һ�η��͵ĳ���Ϊ15D	
		}
		if(length_1%15!=0)                    		//̨������ܷ�15��D�����ݲ���15D�ı��� 
		{
			SendTimes_1=length_1/15+1;          	//���͵Ĵ���
			LastTimeWord_1=length_1%15;         	//���һ�η��͵ĳ���Ϊ��16������	
		}
		
		for (i=0;i<SendTimes_1;i++)
		{
			ps=i*17;                            	//ÿ�η�17������
			b=add+i*15;                     		//��ʼ��ַ
			b=b+0x1000;
			
			b1=(b&0xff00)>>8;                   	//�Կ�ʼƫ�Ƶ�ַȡ�ߵ�λ��b1��λ��b2��λ��16����
			b2=b&0xff;	
			a1=b&0xf000;                        	//�Կ�ʼƫ�Ƶ�ַ��asicc�������δӸߵ��� 
			a1=a1>>12;
			a1=a1&0xf;
			a2=b&0xf00;
			a2=a2>>8;
			a2=a2&0xf;
			a3=b&0xf0;
			a3=a3>>4;
			a3=a3&0xf;
			a4=b&0xf;
			*(U8 *)(AD1+0+ps)=0x3a;             	//̨�￪ʼ��Ԫ":",��3a
			*(U8 *)(AD1+1+ps)=asicc(((plcadd&0xf0)>>4)&0xf);//PLCվ��ַ��Ϊasicc����ʾ
			*(U8 *)(AD1+2+ps)=asicc(plcadd&0xf);
			*(U8 *)(AD1+3+ps)=0x30;             	//������03��Ҫת��asicc��
			*(U8 *)(AD1+4+ps)=0x33;	
			*(U8 *)(AD1+5+ps)=asicc(a1);	    	//��ʼ��ַ�����δӸߵ��ͣ�Ҫת��asicc��
			*(U8 *)(AD1+6+ps)=asicc(a2);
			*(U8 *)(AD1+7+ps)=asicc(a3);
			*(U8 *)(AD1+8+ps)=asicc(a4);
			KK[1]=plcadd;                       	//LRCУ�飬16����У�飬asicc����ʾ                     
			KK[2]=0x03;
			KK[3]=b1;
			KK[4]=b2;
			
			if (i!=(SendTimes_1-1))	            	//�������һ�η���ʱ
			{
				*(U8 *)(AD1+9+ps)=0x30;         	//�̶�����15��D����32�ֽڣ���λ��asicc����ʾ
				*(U8 *)(AD1+10+ps)=0x30;
				*(U8 *)(AD1+11+ps)=0x30;        	//�̶�����15��D����32�ֽڣ���λ��asicc����ʾ
				*(U8 *)(AD1+12+ps)=0x46;
				KK[5]=0x00;
				KK[6]=0x0f;
				currentlength=15;               	//�̶�����16��D               
			}	
			if (i==(SendTimes_1-1))	            	//���һ�η���ʱ
			{		
				*(U8 *)(AD1+9+ps)=0x30;         	//ʣ��LastTimeWord��D����λ��ת��asicc��
				*(U8 *)(AD1+10+ps)=0x30;
				*(U8 *)(AD1+11+ps)=asicc(((LastTimeWord_1&0xf0)>>4)&0xf);//ʣ��LastTimeWord��D��ת��asicc��
				*(U8 *)(AD1+12+ps)=asicc(LastTimeWord_1&0xf);
				KK[5]=0x00;
				KK[6]=LastTimeWord_1&0xff;
				currentlength=LastTimeWord_1;     	//ʣ��LastTimeWord��D 
			}
			aakj=CalLRC(KK,7);	                	//�����У�죬LRCУ�飬��λ��ǰ����λ�ں�
			a1=(aakj&0xf0)>>4;
			a1=a1&0xf;
			a2=aakj&0xf;
			*(U8 *)(AD1+13+ps)=asicc(a1);       	//asicc����ʾ
			*(U8 *)(AD1+14+ps)=asicc(a2);
			*(U8 *)(AD1+15+ps)=0x0d;            	//������Ԫ0d,0a
			*(U8 *)(AD1+16+ps)=0x0a;
						
			ptcom->send_length[i]=17;				//���ͳ���
			ptcom->send_staradd[i]=i*17;			//�������ݴ洢��ַ	
			ptcom->send_add[i]=p_start+i*15;		//�����������ַ������	
			ptcom->send_data_length[i]=currentlength;//�������һ�ζ���15��D
					
			ptcom->return_length[i]=11+currentlength*4;//�������ݳ��ȣ���9���̶���3a��PLC��ַ2�����ȣ�������2�����ȣ�
		                                            //λ����ʾ2�����ȣ�У��2�����ȣ�0d��0a
			ptcom->return_start[i]=7;				//����������Ч��ʼ
			ptcom->return_length_available[i]=currentlength*4;//������Ч���ݳ���	
		}
//---------------------------------------------------------------------------------------------------
		add_1=0x9000;	
		length_2=datalength-length_1;
		
		if(length_2%15==0)                    		//̨������ܷ�15��D�����ݸպ���15D�ı���
		{
			SendTimes_2=length_2/15;            	//���ʹ���
			LastTimeWord_2=15;                    	//���һ�η��͵ĳ���Ϊ15D	
		}
		if(length_2%15!=0)                    		//̨������ܷ�15��D�����ݲ���15D�ı��� 
		{
			SendTimes_2=length_2/15+1;          	//���͵Ĵ���
			LastTimeWord_2=length_2%15;         	//���һ�η��͵ĳ���Ϊ��16������	
		}
		
		for (i=0;i<SendTimes_2;i++)
		{
			ps=i*17;                            	//ÿ�η�17������
			b=add_1+i*15;                     		//��ʼ��ַ
			
			b1=(b&0xff00)>>8;                   	//�Կ�ʼƫ�Ƶ�ַȡ�ߵ�λ��b1��λ��b2��λ��16����
			b2=b&0xff;	
			a1=b&0xf000;                        	//�Կ�ʼƫ�Ƶ�ַ��asicc�������δӸߵ��� 
			a1=a1>>12;
			a1=a1&0xf;
			a2=b&0xf00;
			a2=a2>>8;
			a2=a2&0xf;
			a3=b&0xf0;
			a3=a3>>4;
			a3=a3&0xf;
			a4=b&0xf;
			*(U8 *)(AD1+0+ps+SendTimes_1*17)=0x3a;  //̨�￪ʼ��Ԫ":",��3a
			*(U8 *)(AD1+1+ps+SendTimes_1*17)=asicc(((plcadd&0xf0)>>4)&0xf);//PLCվ��ַ��Ϊasicc����ʾ
			*(U8 *)(AD1+2+ps+SendTimes_1*17)=asicc(plcadd&0xf);
			*(U8 *)(AD1+3+ps+SendTimes_1*17)=0x30;  //������03��Ҫת��asicc��
			*(U8 *)(AD1+4+ps+SendTimes_1*17)=0x33;	
			*(U8 *)(AD1+5+ps+SendTimes_1*17)=asicc(a1);//��ʼ��ַ�����δӸߵ��ͣ�Ҫת��asicc��
			*(U8 *)(AD1+6+ps+SendTimes_1*17)=asicc(a2);
			*(U8 *)(AD1+7+ps+SendTimes_1*17)=asicc(a3);
			*(U8 *)(AD1+8+ps+SendTimes_1*17)=asicc(a4);
			KK[1]=plcadd;                       	//LRCУ�飬16����У�飬asicc����ʾ                     
			KK[2]=0x03;
			KK[3]=b1;
			KK[4]=b2;
			
			if (i!=(SendTimes_2-1))	            	//�������һ�η���ʱ
			{
				*(U8 *)(AD1+9+ps+SendTimes_1*17)=0x30;//�̶�����15��D����32�ֽڣ���λ��asicc����ʾ
				*(U8 *)(AD1+10+ps+SendTimes_1*17)=0x30;
				*(U8 *)(AD1+11+ps+SendTimes_1*17)=0x30;//�̶�����15��D����32�ֽڣ���λ��asicc����ʾ
				*(U8 *)(AD1+12+ps+SendTimes_1*17)=0x46;
				KK[5]=0x00;
				KK[6]=0x0f;
				currentlength=15;               	//�̶�����16��D               
			}
			if (i==(SendTimes_2-1))	            	//���һ�η���ʱ
			{		
				*(U8 *)(AD1+9+ps+SendTimes_1*17)=0x30;//ʣ��LastTimeWord��D����λ��ת��asicc��
				*(U8 *)(AD1+10+ps+SendTimes_1*17)=0x30;
				*(U8 *)(AD1+11+ps+SendTimes_1*17)=asicc(((LastTimeWord_2&0xf0)>>4)&0xf);//ʣ��LastTimeWord��D��ת��asicc��
				*(U8 *)(AD1+12+ps+SendTimes_1*17)=asicc(LastTimeWord_2&0xf);
				KK[5]=0x00;
				KK[6]=LastTimeWord_2&0xff;
				currentlength=LastTimeWord_2;     	//ʣ��LastTimeWord��D 
			}
			aakj=CalLRC(KK,7);	                	//�����У�죬LRCУ�飬��λ��ǰ����λ�ں�
			a1=(aakj&0xf0)>>4;
			a1=a1&0xf;
			a2=aakj&0xf;
			*(U8 *)(AD1+13+ps+SendTimes_1*17)=asicc(a1);//asicc����ʾ
			*(U8 *)(AD1+14+ps+SendTimes_1*17)=asicc(a2);
			*(U8 *)(AD1+15+ps+SendTimes_1*17)=0x0d; //������Ԫ0d,0a
			*(U8 *)(AD1+16+ps+SendTimes_1*17)=0x0a;
						
			ptcom->send_length[i+SendTimes_1]=17;	//���ͳ���
			ptcom->send_staradd[i+SendTimes_1]=i*17+SendTimes_1*17;//�������ݴ洢��ַ	
			ptcom->send_add[i+SendTimes_1]=4096+i*15;//�����������ַ������	
			ptcom->send_data_length[i+SendTimes_1]=currentlength;//�������һ�ζ���15��D
					
			ptcom->return_length[i+SendTimes_1]=11+currentlength*4;//�������ݳ��ȣ���9���̶���3a��PLC��ַ2�����ȣ�������2�����ȣ�
		                                            //λ����ʾ2�����ȣ�У��2�����ȣ�0d��0a
			ptcom->return_start[i+SendTimes_1]=7;	//����������Ч��ʼ
			ptcom->return_length_available[i+SendTimes_1]=currentlength*4;//������Ч���ݳ���	
		}
		ptcom->send_times=SendTimes_1+SendTimes_2;	//���ʹ���
		ptcom->Current_Times=0;												
	}
	ptcom->send_staradd[90]=1;		
}
/************************************************************************************************************************
д�䷽�Ӻ���
*************************************************************************************************************************/
void Write_Recipe()									//д�䷽��PLC
{
	int datalength;
	int staradd;
	int SendTimes;                              	//���͵Ĵ���
	int LastTimeWord;								//���һ�η��ͳ���
	int i,j;
	int ps;
	int b;
	int a1,a2,a3,a4;
	U16 aakj;
	int b1,b2,b3,b4;
	int length;
	int plcadd;	
	int t,k1,k2,k3,k4,k5,k6,k7,k8;
	int t1,t2,t3,t4;
	int add,add_1;
	int length_1,length_2;
	int SendTimes_1,SendTimes_2;
	int LastTimeWord_1,LastTimeWord_2;
	
	
	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));//���ݳ���
	staradd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//���ݿ�ʼ��ַ
	plcadd=*(U8 *)(PE+4);	                   		//PLCվ��ַ

	if ((staradd<=4095 && staradd+datalength<=4096) || (staradd>=4096))
	{
		if(datalength%15==0)                       	//̨������ܷ�15��D�����ݳ��ȸպ���16�ı���ʱ
		{
			SendTimes=datalength/15;               	//���͵Ĵ���Ϊdatalength/15               
			LastTimeWord=15;                       	//���һ�η��͵ĳ���Ϊ15��D
		}
		if(datalength%15!=0)                       	//���ݳ��Ȳ���15D�ı���ʱ
		{
			SendTimes=datalength/15+1;             	//���͵Ĵ���datalength/15+1
			LastTimeWord=datalength%15;            	//���һ�η��͵ĳ���Ϊ��16������
		}	
		
		ps=79;                                     	//��15��DҪ��83������
		
		for (i=0;i<SendTimes;i++)
		{   
			if (i!=(SendTimes-1))                  	//�������һ�η���ʱ
			{	
				length=15;                         	//��15��D
			}
			else                                   	//���һ�η���ʱ
			{
				length=LastTimeWord;               	//��ʣ��ĳ���             
			}
			
			b=staradd+i*15;                        	//��ʼ��ַ
			if (b<=0xfff)
			{
				b=b+0x1000;
			}
			else
			{
				b=b-4096;
				b=b+0x9000;
			}
			b1=(b&0xff00)>>8;                      	//�Ե�ַ��16���ƴ���b1Ϊ��λ��b2Ϊ��λ
			b2=b&0xff;
			a1=b&0xf000;                           	//�Ե�ַ��asicc��������ʾasicc�룬���δӸ�λ����λ
			a1=a1>>12;
			a1=a1&0xf;
			a2=b&0xf00;
			a2=a2>>8;
			a2=a2&0xf;
			a3=b&0xf0;
			a3=a3>>4;
			a3=a3&0xf;
			a4=b&0xf;
			*(U8 *)(AD1+0+ps*i)=0x3a;              	//̨����ʼ��Ԫ3a
			*(U8 *)(AD1+1+ps*i)=asicc(((plcadd&0xf0)>>4)&0xf);//PLCվ��ַ��asicc������ʾasicc�룬��λ��ǰ
			*(U8 *)(AD1+2+ps*i)=asicc(plcadd&0xf);
			*(U8 *)(AD1+3+ps*i)=0x31;              	//������16����asicc��ʾ
			*(U8 *)(AD1+4+ps*i)=0x30;
			*(U8 *)(AD1+5+ps*i)=asicc(a1);         	//��ʼ��ַ����asicc��ʾ�����δӸ�λ����λ
			*(U8 *)(AD1+6+ps*i)=asicc(a2);
			*(U8 *)(AD1+7+ps*i)=asicc(a3);
			*(U8 *)(AD1+8+ps*i)=asicc(a4);
			
			
			t=length;                              
			b3=(t&0xff00)>>8;                      	//�Գ�����16���ƴ���b3Ϊ��λ��b4Ϊ��λ
			b4=t&0xff;
			t1=t&0xf000;                           	//�Գ�����asicc��������ʾasicc�룬���δӸ�λ����λ
			t1=t1>>12;
			t1=t1&0xf;
			t2=t&0xf00;
			t2=t2>>8;
			t2=t2&0xf;
			t3=t&0xf0;
			t3=t3>>4;
			t3=t3&0xf;
			t4=t&0xf;
			*(U8 *)(AD1+9+ps*i)=asicc(t1);	       //���ͳ��ȣ���asicc�뷢��
			*(U8 *)(AD1+10+ps*i)=asicc(t2);
			*(U8 *)(AD1+11+ps*i)=asicc(t3);
			*(U8 *)(AD1+12+ps*i)=asicc(t4);
			
			k1=((length*2)&0xf0)>>4;               //���͵�λ������asicc��������ʾasicc�룬���δӸ�λ����λ
			k1=k1&0xf;
			k2=(length*2)&0xf;
			*(U8 *)(AD1+13+ps*i)=asicc(k1);        //���͵�λ���ȣ���asicc�뷢��
			*(U8 *)(AD1+14+ps*i)=asicc(k2);
							
			KK[1]=plcadd;                          //LRCУ���룬��16���Ʊ�ʾ����Ӧ��PLCվ��ַ�������룬��ʼ��ַ�����ȣ�λ����
			KK[2]=0x10;
			KK[3]=b1;
			KK[4]=b2;
			KK[5]=b3;
			KK[6]=b4;
			KK[7]=length*2;
							
			for(j=0;j<length;j++)                  //д����Ԫ��ֵ                 
			{	
				k4=*(U8 *)(PE+9+i*30+j*2);         //�Ӵ����ݵļĴ�����ʼ��ַPE+9ȡ���ݣ�k3Ϊ��λ��k4Ϊ��λ
				k3=*(U8 *)(PE+9+i*30+j*2+1);
							
				k5=(k3&0xf0)>>4;                   //��ȡ�õ�������asicc����
				k6=k3&0xf;
				k7=(k4&0xf0)>>4;
				k8=k4&0xf;
				*(U8 *)(AD1+15+j*4+ps*i)=asicc(k5);//����ȡ�õ����ݣ�����ʾasicc�룬���δӸ�λ����λ
				*(U8 *)(AD1+16+j*4+ps*i)=asicc(k6);
				*(U8 *)(AD1+17+j*4+ps*i)=asicc(k7);
				*(U8 *)(AD1+18+j*4+ps*i)=asicc(k8);			
					
				KK[8+j*2]=k3;
				KK[9+j*2]=k4;					
			}
			aakj=CalLRC(KK,length*2+8);	           //�����У�죬LRCУ�飬��λ��ǰ����λ�ں�
			a1=(aakj&0xf0)>>4;
			a1=a1&0xf;
			a2=aakj&0xf;
			*(U8 *)(AD1+19+(length-1)*4+ps*i)=asicc(a1);
			*(U8 *)(AD1+20+(length-1)*4+ps*i)=asicc(a2);
			*(U8 *)(AD1+21+(length-1)*4+ps*i)=0x0d;//̨�������Ԫ0d��0a
			*(U8 *)(AD1+22+(length-1)*4+ps*i)=0x0a;
		
			ptcom->send_length[i]=19+length*4;	   //���ͳ���
			ptcom->send_staradd[i]=i*ps;		   //�������ݴ洢��ַ	
			ptcom->return_length[i]=17;			   //�������ݳ���
			ptcom->return_start[i]=0;			   //����������Ч��ʼ
			ptcom->return_length_available[i]=0;   //������Ч���ݳ���	
					
		}
		ptcom->send_times=SendTimes;			   //���ʹ���
		ptcom->Current_Times=0;					   //��ǰ���ʹ���
	}
/////////////////////////////////////////////////////////////////////////////////////////////	
	else
	{
		add=staradd;
		length_1=4096-staradd;
		
		if(length_1%15==0)                       	//̨������ܷ�15��D�����ݳ��ȸպ���16�ı���ʱ
		{
			SendTimes_1=length_1/15;               	//���͵Ĵ���Ϊdatalength/15               
			LastTimeWord_1=15;                      //���һ�η��͵ĳ���Ϊ15��D
		}
		if(length_1%15!=0)                       	//���ݳ��Ȳ���15D�ı���ʱ
		{
			SendTimes_1=length_1/15+1;             	//���͵Ĵ���datalength/15+1
			LastTimeWord_1=length_1%15;            	//���һ�η��͵ĳ���Ϊ��16������
		}	
		
		ps=79;                                     	//��15��DҪ��83������
		
		for (i=0;i<SendTimes_1;i++)
		{   
			if (i!=(SendTimes_1-1))                 //�������һ�η���ʱ
			{	
				length=15;                         	//��15��D
			}
			else                                   	//���һ�η���ʱ
			{
				length=LastTimeWord_1;              //��ʣ��ĳ���             
			}
			
			b=add+i*15;                        		//��ʼ��ַ
			b=b+0x1000;

			b1=(b&0xff00)>>8;                      	//�Ե�ַ��16���ƴ���b1Ϊ��λ��b2Ϊ��λ
			b2=b&0xff;
			a1=b&0xf000;                           	//�Ե�ַ��asicc��������ʾasicc�룬���δӸ�λ����λ
			a1=a1>>12;
			a1=a1&0xf;
			a2=b&0xf00;
			a2=a2>>8;
			a2=a2&0xf;
			a3=b&0xf0;
			a3=a3>>4;
			a3=a3&0xf;
			a4=b&0xf;
			*(U8 *)(AD1+0+ps*i)=0x3a;              	//̨����ʼ��Ԫ3a
			*(U8 *)(AD1+1+ps*i)=asicc(((plcadd&0xf0)>>4)&0xf);//PLCվ��ַ��asicc������ʾasicc�룬��λ��ǰ
			*(U8 *)(AD1+2+ps*i)=asicc(plcadd&0xf);
			*(U8 *)(AD1+3+ps*i)=0x31;              	//������16����asicc��ʾ
			*(U8 *)(AD1+4+ps*i)=0x30;
			*(U8 *)(AD1+5+ps*i)=asicc(a1);         	//��ʼ��ַ����asicc��ʾ�����δӸ�λ����λ
			*(U8 *)(AD1+6+ps*i)=asicc(a2);
			*(U8 *)(AD1+7+ps*i)=asicc(a3);
			*(U8 *)(AD1+8+ps*i)=asicc(a4);
					
			t=length;                              
			b3=(t&0xff00)>>8;                      	//�Գ�����16���ƴ���b3Ϊ��λ��b4Ϊ��λ
			b4=t&0xff;
			t1=t&0xf000;                           	//�Գ�����asicc��������ʾasicc�룬���δӸ�λ����λ
			t1=t1>>12;
			t1=t1&0xf;
			t2=t&0xf00;
			t2=t2>>8;
			t2=t2&0xf;
			t3=t&0xf0;
			t3=t3>>4;
			t3=t3&0xf;
			t4=t&0xf;
			*(U8 *)(AD1+9+ps*i)=asicc(t1);	       //���ͳ��ȣ���asicc�뷢��
			*(U8 *)(AD1+10+ps*i)=asicc(t2);
			*(U8 *)(AD1+11+ps*i)=asicc(t3);
			*(U8 *)(AD1+12+ps*i)=asicc(t4);
			
			k1=((length*2)&0xf0)>>4;               //���͵�λ������asicc��������ʾasicc�룬���δӸ�λ����λ
			k1=k1&0xf;
			k2=(length*2)&0xf;
			*(U8 *)(AD1+13+ps*i)=asicc(k1);        //���͵�λ���ȣ���asicc�뷢��
			*(U8 *)(AD1+14+ps*i)=asicc(k2);
							
			KK[1]=plcadd;                          //LRCУ���룬��16���Ʊ�ʾ����Ӧ��PLCվ��ַ�������룬��ʼ��ַ�����ȣ�λ����
			KK[2]=0x10;
			KK[3]=b1;
			KK[4]=b2;
			KK[5]=b3;
			KK[6]=b4;
			KK[7]=length*2;
							
			for(j=0;j<length;j++)                  //д����Ԫ��ֵ                 
			{	
				k4=*(U8 *)(PE+9+i*30+j*2);         //�Ӵ����ݵļĴ�����ʼ��ַPE+9ȡ���ݣ�k3Ϊ��λ��k4Ϊ��λ
				k3=*(U8 *)(PE+9+i*30+j*2+1);
							
				k5=(k3&0xf0)>>4;                   //��ȡ�õ�������asicc����
				k6=k3&0xf;
				k7=(k4&0xf0)>>4;
				k8=k4&0xf;
				*(U8 *)(AD1+15+j*4+ps*i)=asicc(k5);//����ȡ�õ����ݣ�����ʾasicc�룬���δӸ�λ����λ
				*(U8 *)(AD1+16+j*4+ps*i)=asicc(k6);
				*(U8 *)(AD1+17+j*4+ps*i)=asicc(k7);
				*(U8 *)(AD1+18+j*4+ps*i)=asicc(k8);			
					
				KK[8+j*2]=k3;
				KK[9+j*2]=k4;					
			}
			aakj=CalLRC(KK,length*2+8);	           //�����У�죬LRCУ�飬��λ��ǰ����λ�ں�
			a1=(aakj&0xf0)>>4;
			a1=a1&0xf;
			a2=aakj&0xf;
			*(U8 *)(AD1+19+(length-1)*4+ps*i)=asicc(a1);
			*(U8 *)(AD1+20+(length-1)*4+ps*i)=asicc(a2);
			*(U8 *)(AD1+21+(length-1)*4+ps*i)=0x0d;//̨�������Ԫ0d��0a
			*(U8 *)(AD1+22+(length-1)*4+ps*i)=0x0a;
		
			ptcom->send_length[i]=19+length*4;	   //���ͳ���
			ptcom->send_staradd[i]=i*ps;		   //�������ݴ洢��ַ	
			ptcom->return_length[i]=17;			   //�������ݳ���
			ptcom->return_start[i]=0;			   //����������Ч��ʼ
			ptcom->return_length_available[i]=0;   //������Ч���ݳ���						
		}
//-------------------------------------------------------------------------------------------------------------		
		add_1=0x9000;		
		length_2=datalength-length_1;
		
		if(length_2%15==0)                       	//̨������ܷ�15��D�����ݳ��ȸպ���16�ı���ʱ
		{
			SendTimes_2=length_2/15;               	//���͵Ĵ���Ϊdatalength/15               
			LastTimeWord_2=15;                      //���һ�η��͵ĳ���Ϊ15��D
		}
		if(length_2%15!=0)                       	//���ݳ��Ȳ���15D�ı���ʱ
		{
			SendTimes_2=length_2/15+1;             	//���͵Ĵ���datalength/15+1
			LastTimeWord_2=length_2%15;            	//���һ�η��͵ĳ���Ϊ��16������
		}	
		ps=79;                                     	//��15��DҪ��83������
		
		for (i=0;i<SendTimes_2;i++)
		{   
			if (i!=(SendTimes_2-1))                 //�������һ�η���ʱ
			{	
				length=15;                         	//��15��D
			}
			else                                   	//���һ�η���ʱ
			{
				length=LastTimeWord_2;              //��ʣ��ĳ���             
			}
			
			b=add_1+i*15;                        	//��ʼ��ַ

			b1=(b&0xff00)>>8;                      	//�Ե�ַ��16���ƴ���b1Ϊ��λ��b2Ϊ��λ
			b2=b&0xff;
			a1=b&0xf000;                           	//�Ե�ַ��asicc��������ʾasicc�룬���δӸ�λ����λ
			a1=a1>>12;
			a1=a1&0xf;
			a2=b&0xf00;
			a2=a2>>8;
			a2=a2&0xf;
			a3=b&0xf0;
			a3=a3>>4;
			a3=a3&0xf;
			a4=b&0xf;
			*(U8 *)(AD1+0+ps*i+SendTimes_1*79)=0x3a;//̨����ʼ��Ԫ3a
			*(U8 *)(AD1+1+ps*i+SendTimes_1*79)=asicc(((plcadd&0xf0)>>4)&0xf);//PLCվ��ַ��asicc������ʾasicc�룬��λ��ǰ
			*(U8 *)(AD1+2+ps*i+SendTimes_1*79)=asicc(plcadd&0xf);
			*(U8 *)(AD1+3+ps*i+SendTimes_1*79)=0x31;//������16����asicc��ʾ
			*(U8 *)(AD1+4+ps*i+SendTimes_1*79)=0x30;
			*(U8 *)(AD1+5+ps*i+SendTimes_1*79)=asicc(a1);//��ʼ��ַ����asicc��ʾ�����δӸ�λ����λ
			*(U8 *)(AD1+6+ps*i+SendTimes_1*79)=asicc(a2);
			*(U8 *)(AD1+7+ps*i+SendTimes_1*79)=asicc(a3);
			*(U8 *)(AD1+8+ps*i+SendTimes_1*79)=asicc(a4);
			
			
			t=length;                              
			b3=(t&0xff00)>>8;                      	//�Գ�����16���ƴ���b3Ϊ��λ��b4Ϊ��λ
			b4=t&0xff;
			t1=t&0xf000;                           	//�Գ�����asicc��������ʾasicc�룬���δӸ�λ����λ
			t1=t1>>12;
			t1=t1&0xf;
			t2=t&0xf00;
			t2=t2>>8;
			t2=t2&0xf;
			t3=t&0xf0;
			t3=t3>>4;
			t3=t3&0xf;
			t4=t&0xf;
			*(U8 *)(AD1+9+ps*i+SendTimes_1*79)=asicc(t1);//���ͳ��ȣ���asicc�뷢��
			*(U8 *)(AD1+10+ps*i+SendTimes_1*79)=asicc(t2);
			*(U8 *)(AD1+11+ps*i+SendTimes_1*79)=asicc(t3);
			*(U8 *)(AD1+12+ps*i+SendTimes_1*79)=asicc(t4);
			
			k1=((length*2)&0xf0)>>4;               //���͵�λ������asicc��������ʾasicc�룬���δӸ�λ����λ
			k1=k1&0xf;
			k2=(length*2)&0xf;
			*(U8 *)(AD1+13+ps*i+SendTimes_1*79)=asicc(k1);//���͵�λ���ȣ���asicc�뷢��
			*(U8 *)(AD1+14+ps*i+SendTimes_1*79)=asicc(k2);
							
			KK[1]=plcadd;                          //LRCУ���룬��16���Ʊ�ʾ����Ӧ��PLCվ��ַ�������룬��ʼ��ַ�����ȣ�λ����
			KK[2]=0x10;
			KK[3]=b1;
			KK[4]=b2;
			KK[5]=b3;
			KK[6]=b4;
			KK[7]=length*2;
							
			for(j=0;j<length;j++)                  //д����Ԫ��ֵ                 
			{	
				k4=*(U8 *)(PE+9+i*30+j*2+length_1*2);//�Ӵ����ݵļĴ�����ʼ��ַPE+9ȡ���ݣ�k3Ϊ��λ��k4Ϊ��λ
				k3=*(U8 *)(PE+9+i*30+j*2+1+length_1*2);
							
				k5=(k3&0xf0)>>4;                   //��ȡ�õ�������asicc����
				k6=k3&0xf;
				k7=(k4&0xf0)>>4;
				k8=k4&0xf;
				*(U8 *)(AD1+15+j*4+ps*i+SendTimes_1*79)=asicc(k5);//����ȡ�õ����ݣ�����ʾasicc�룬���δӸ�λ����λ
				*(U8 *)(AD1+16+j*4+ps*i+SendTimes_1*79)=asicc(k6);
				*(U8 *)(AD1+17+j*4+ps*i+SendTimes_1*79)=asicc(k7);
				*(U8 *)(AD1+18+j*4+ps*i+SendTimes_1*79)=asicc(k8);			
					
				KK[8+j*2]=k3;
				KK[9+j*2]=k4;					
			}
			aakj=CalLRC(KK,length*2+8);	           //�����У�죬LRCУ�飬��λ��ǰ����λ�ں�
			a1=(aakj&0xf0)>>4;
			a1=a1&0xf;
			a2=aakj&0xf;
			*(U8 *)(AD1+19+(length-1)*4+ps*i+SendTimes_1*79)=asicc(a1);
			*(U8 *)(AD1+20+(length-1)*4+ps*i+SendTimes_1*79)=asicc(a2);
			*(U8 *)(AD1+21+(length-1)*4+ps*i+SendTimes_1*79)=0x0d;//̨�������Ԫ0d��0a
			*(U8 *)(AD1+22+(length-1)*4+ps*i+SendTimes_1*79)=0x0a;
		
			ptcom->send_length[i+SendTimes_1]=19+length*4;//���ͳ���
			ptcom->send_staradd[i+SendTimes_1]=i*ps+SendTimes_1*79;//�������ݴ洢��ַ	
			ptcom->return_length[i+SendTimes_1]=17;	//�������ݳ���
			ptcom->return_start[i+SendTimes_1]=0;	//����������Ч��ʼ
			ptcom->return_length_available[i+SendTimes_1]=0;//������Ч���ݳ���						
		}
		ptcom->send_times=SendTimes_1+SendTimes_2;	//���ʹ���
		ptcom->Current_Times=0;			
	}	
	ptcom->send_staradd[90]=0;			
}
/************************************************************************************************************************
���ݷ�����ȷ�󣬴��������Ӻ���
*************************************************************************************************************************/
void compxy(void)				              		//����ɱ�׼�洢��ʽ,��������
{
	int i;
	unsigned char a1,a2,a3,a4,a5,a6,a7,a8;
	int b,b1,b2,b3;
	
	if (ptcom->Current_Times==2)
	{
		ptcom->address=ptcom->send_staradd[98];		//��ʼ��ַ
		ptcom->register_length=ptcom->send_staradd[99];		
	}	

	if(ptcom->Simens_Count == 100)
	{
		ptcom->address = ptcom->send_add[ptcom->Current_Times - 1];
		ptcom->register_length =ptcom->send_data_length[ptcom->Current_Times - 1];
	}
	
	if (ptcom->send_staradd[90]==2)	
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/8;i++)	//ASC�귵�أ�����ҪתΪ16���ƣ�2��asicc�뻻��1��16������
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*8);
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*8+1);
			a3=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*8+2);
			a4=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*8+3);
			a5=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*8+4);
			a6=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*8+5);
			a7=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*8+6);
			a8=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*8+7);
			a1=bsicc(a1);
			a2=bsicc(a2);
			a3=bsicc(a3);
			a4=bsicc(a4);
			a5=bsicc(a5);
			a6=bsicc(a6);
			a7=bsicc(a7);
			a8=bsicc(a8);
			b=(a1<<4)+a2;
			b1=(a3<<4)+a4;
			b2=(a5<<4)+a6;
			b3=(a7<<4)+a8;
			*(U8 *)(COMad+i*4)=b2;					 		//���´�,�ӵ�0����ʼ��
			*(U8 *)(COMad+i*4+1)=b3;	
			*(U8 *)(COMad+i*4+2)=b;					 		
			*(U8 *)(COMad+i*4+3)=b1;	
		}
	}
	else
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/2;i++)	//ASC�귵�أ�����ҪתΪ16���ƣ�2��asicc�뻻��1��16������
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2);
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2+1);
			a1=bsicc(a1);
			a2=bsicc(a2);
			b=(a1<<4)+a2;
			*(U8 *)(COMad+i)=b;					 		//���´�,�ӵ�0����ʼ��
		}
	}		
	ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;	//���ȼ���	

}
/************************************************************************************************************************
��������У�麯��
*************************************************************************************************************************/
void watchcom(void)									//�������У��
{
	unsigned int aakj=0;
	aakj=remark();
	if(aakj==1)										//У������ȷ
	{
		ptcom->IfResultCorrect=1;
		compxy();									//�������ݴ������
	}
	else
	{
		ptcom->IfResultCorrect=0;
	}
}
/************************************************************************************************************************
asiccת������
*************************************************************************************************************************/
int asicc(int a)									//תΪAsc��
{
	int bl;
	if(a<10)
		bl=a+0x30;
	if(a>9)
		bl=a-10+0x41;
	return bl;	
}
/************************************************************************************************************************
bsiccת������
*************************************************************************************************************************/
int bsicc(int a)									//AscתΪ����
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
/************************************************************************************************************************
У�鷵�������Ƿ�ͷ��صļ�����һ��
*************************************************************************************************************************/
int remark()										//�����������ݼ���У�����Ƿ���ȷ
{
	unsigned int aakj1;
	unsigned int aakj2;
	unsigned int a1;
	unsigned int a2;
	unsigned int akj1,akj2;
	int i;	
	aakj2=(*(U8 *)(COMad+ptcom->return_length[ptcom->Current_Times-1]-3))&0xff;	//�ڷ������Current_Times++����ʱҪ--
	aakj1=(*(U8 *)(COMad+ptcom->return_length[ptcom->Current_Times-1]-4))&0xff; //������Ľ�����ԪȡУ����
	for(i=1;i<=(ptcom->return_length[ptcom->Current_Times-1]-5)/2;i++)    //���յ������ݽ������У���ȥ5������У��ĳ���
	{
		a1=bsicc(*(U8 *)(COMad+(i-1)*2+1));
		a2=bsicc(*(U8 *)(COMad+(i-1)*2+2));
		a1=a1&0xf;
		a2=a2&0xf;
		KK[i]=(a1<<4)+a2;
	}
	
	akj1=CalLRC(KK,1+(ptcom->return_length[ptcom->Current_Times-1]-5)/2);//���յ������ݽ���У�飬��ת����asicc��

	akj2=(akj1&0xf0)>>4;
	akj2=akj2&0xf;
	akj1=akj1&0xf;
	akj1=asicc(akj1);
	akj2=asicc(akj2);

	if((akj1==aakj2)&&(akj2==aakj1))     			//�Ƚ�У���룬�����ϣ�����ȼ���ȡ���ݽ���У��
		return 1;
	else
		return 0;	
}
/************************************************************************************************************************
LRCУ�麯��
*************************************************************************************************************************/
U16 CalLRC(unsigned char *chData,U16 uNo)			//LRCУ�飬��16������У�飬asicc����ʾ
{
	int i;
	U16 ab=0;
	for(i=1;i<uNo;i++)
	{
		ab=ab+chData[i];
	}
	ab=~(ab);
	ab=(ab&0xff)+1;
	return (ab);
}
