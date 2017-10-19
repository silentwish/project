#include "stdio.h"
#include "def.h"
#include "smm.h"

unsigned char Check[100]; 

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
					Read_Bool();
					break;
				case 'D':
				case 'R':	
				case 't':
				case 'c':	
					Read_Analog();
					break;
				default:
					break;			
			}
			break;
		case PLC_WRITE_DATA:				//����������д����
			switch(ptcom->registerr)
			{
				case 'M':			
					Set_Reset(); 
					break;
				case 'D':
				case 'c':			
					Write_Analog();		
					break;
				default:
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
					break;			
			}
			break;	
		case PLC_READ_TIME:				//���������Ƕ�ȡʱ�䵽PLC
			switch(ptcom->registerr)
			{
				case 'D':
				case 'R':		
					Read_Time();		
					break;
				default:
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
					break;				
			}
			break;
		case PLC_READ_RECIPE:				//���������Ǵ�PLC��ȡ�䷽
			switch(*(U8 *)(PE+3))//�䷽�Ĵ�������
			{
				case 'D':	
				case 'R':		
					Read_Recipe();		
					break;
				default:
					break;							
			}
			break;							
		case PLC_CHECK_DATA:				//�������������ݴ���
			watchcom();
			break;
		default:
			break;					
	}	 
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Set_Reset()//��λ�͸�λ
{
	U16 check;
	int check1,check2;
	int add;
	int a1,a2,a3,a4;
	int plcadd;	

	add=ptcom->address;			     				// ��λ��ַ
	plcadd=ptcom->plc_address;	     				//PLCվ��ַ
	
	*(U8 *)(AD1+0)=0x3a;							//��Ϣ֡��ð��(:)Ϊ��ʼ
	
	a1=(plcadd>>4)&0xf;
	a2=plcadd&0xf;
	*(U8 *)(AD1+1)=asicc(a1);          				//PLCվ��ַ
	*(U8 *)(AD1+2)=asicc(a2);          		
	
	*(U8 *)(AD1+3)=0x30;            				//������0x05��ǿ����λ�͸�λ
	*(U8 *)(AD1+4)=0x35;
	
	a1=(add>>12)&0xf;
	a2=(add>>8)&0xf;
	a3=(add>>4)&0xf;
	a4=add&0xf;
	*(U8 *)(AD1+5)=asicc(a1);              			//��ʼ��ַ���ɸߵ���	
	*(U8 *)(AD1+6)=asicc(a2);	            		
	*(U8 *)(AD1+7)=asicc(a3);
	*(U8 *)(AD1+8)=asicc(a4);

	if (ptcom->writeValue==1)	    				//��λ
	{
		*(U8 *)(AD1+9)=0x46;
		*(U8 *)(AD1+10)=0x46;
		*(U8 *)(AD1+11)=0x30;
		*(U8 *)(AD1+12)=0x30;		
	}
	if (ptcom->writeValue==0)	    				//��λ
	{
		*(U8 *)(AD1+9)=0x30;
		*(U8 *)(AD1+10)=0x30;
		*(U8 *)(AD1+11)=0x30;
		*(U8 *)(AD1+12)=0x30;
	}
	
	Check[0]=plcadd;
	Check[1]=0x05;
	Check[2]=(add>>8)&0xff;
	Check[3]=add&0xff;
	if (ptcom->writeValue==1)	    				//��λ
	{
		Check[4]=0xff;
		Check[5]=0x00;
	}
	if (ptcom->writeValue==0)	    				//��λ
	{
		Check[4]=0x00;
		Check[5]=0x00;			
	}	
	check=CalLRC(Check,6);         					//LRCУ��
	check1=(check>>4)&0xf;
	check2=check&0xf;
	*(U8 *)(AD1+13)=asicc(check1);          		//У�� ��λ
	*(U8 *)(AD1+14)=asicc(check2);          		//У�� ��λ
	
	*(U8 *)(AD1+15)=0x0d;							//ASCIIģʽ�����ַ����س�CR������LF
	*(U8 *)(AD1+16)=0x0a;

	if (plcadd==0)								//�㲥ģʽ
	{
		ptcom->send_length[0]=17;				//���ͳ���
		ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
		ptcom->send_times=1;					//���ʹ���
			
		ptcom->return_length[0]=0;				//�������ݳ���
		ptcom->return_start[0]=0;				//����������Ч��ʼ
		ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
		ptcom->Current_Times=0;					//��ǰ���ʹ���	
	}
	else
	{
		ptcom->send_length[0]=17;				//���ͳ���
		ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
		ptcom->send_times=1;					//���ʹ���
			
		ptcom->return_length[0]=17;				//�������ݳ���
		ptcom->return_start[0]=0;				//����������Ч��ʼ
		ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
		ptcom->Current_Times=0;					//��ǰ���ʹ���			
	}	
	ptcom->send_staradd[99]=0;		
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Read_Bool()//��ȡ����������Ϣ
{
	U16 check;
	int check1,check2;
	int add;
	int a1,a2,a3,a4;
	int plcadd;
	int length;	
				
	add=ptcom->address;								//���������Ѿ�ת�����öεĿ�ʼ��ַ
	plcadd=ptcom->plc_address;						//PLCվ��ַ
	length=ptcom->register_length;
	length=length*8;            					//һ��Ԫ������8λ����

	*(U8 *)(AD1+0)=0x3a;							//��Ϣ֡��ð��(:)Ϊ��ʼ
	
	a1=(plcadd>>4)&0xf;
	a2=plcadd&0xf;
	*(U8 *)(AD1+1)=asicc(a1);          				//PLCվ��ַ
	*(U8 *)(AD1+2)=asicc(a2);          		
	
	switch (ptcom->registerr)						//���ݼĴ�������ѡ��ͬ������
	{
		case 'X':
			*(U8 *)(AD1+3)=0x30;            		//������0x02��ֻ���̵���1x
			*(U8 *)(AD1+4)=0x32;
			break;	
		case 'M':
			*(U8 *)(AD1+3)=0x30;            		//������0x01���ɶ���д�̵���0x
			*(U8 *)(AD1+4)=0x31;
			break;					
	}	
	
	a1=(add>>12)&0xf;
	a2=(add>>8)&0xf;
	a3=(add>>4)&0xf;
	a4=add&0xf;
	*(U8 *)(AD1+5)=asicc(a1);              			//��ʼ��ַ���ɸߵ���	
	*(U8 *)(AD1+6)=asicc(a2);	            		
	*(U8 *)(AD1+7)=asicc(a3);
	*(U8 *)(AD1+8)=asicc(a4);                 
	
	a1=(length>>12)&0xf;
	a2=(length>>8)&0xf;
	a3=(length>>4)&0xf;
	a4=length&0xf;
	*(U8 *)(AD1+9)=asicc(a1);              			//��ȡ���ȣ��ɸߵ���	
	*(U8 *)(AD1+10)=asicc(a2);	            		
	*(U8 *)(AD1+11)=asicc(a3);
	*(U8 *)(AD1+12)=asicc(a4);
	
	Check[0]=plcadd&0xff;	 
	switch (ptcom->registerr)						//���ݼĴ�������ѡ��ͬ������
	{
		case 'X':
			Check[1]=0x02;
			break;	
		case 'M':
			Check[1]=0x01;
			break;			
	}
	Check[2]=(add>>8)&0xff;
	Check[3]=add&0xff;
	Check[4]=(length>>8)&0xff;
	Check[5]=length&0xff;
	check=CalLRC(Check,6);         					//LRCУ��
	check1=(check>>4)&0xf;
	check2=check&0xf;
	*(U8 *)(AD1+13)=asicc(check1);          		//У�� ��λ
	*(U8 *)(AD1+14)=asicc(check2);          		//У�� ��λ
	
	*(U8 *)(AD1+15)=0x0d;							//ASCIIģʽ�����ַ����س�CR������LF
	*(U8 *)(AD1+16)=0x0a;

	if (plcadd==0)								//�㲥ģʽ
	{	
		ptcom->send_length[0]=17;				     //���ͳ���
		ptcom->send_staradd[0]=0;				     //�������ݴ洢��ַ	
		ptcom->send_times=1;					     //���ʹ���
			
		ptcom->return_length[0]=0;		     		//�������ݳ��ȣ���5���̶�,У��
		ptcom->return_start[0]=0;				     //����������Ч��ʼ
		ptcom->return_length_available[0]=0;    //������Ч���ݳ���	
		ptcom->Current_Times=0;					     //��ǰ���ʹ���	
		ptcom->send_add[0]=ptcom->address;		     //�����������ַ������	
		ptcom->address=ptcom->address;
	}
	else
	{
		ptcom->send_length[0]=17;				     //���ͳ���
		ptcom->send_staradd[0]=0;				     //�������ݴ洢��ַ	
		ptcom->send_times=1;					     //���ʹ���
			
		ptcom->return_length[0]=11+length/4;		     //�������ݳ��ȣ���5���̶�,У��
		ptcom->return_start[0]=7;				     //����������Ч��ʼ
		ptcom->return_length_available[0]=length/4;    //������Ч���ݳ���	
		ptcom->Current_Times=0;					     //��ǰ���ʹ���	
		ptcom->send_add[0]=ptcom->address;		     //�����������ַ������	
		ptcom->address=ptcom->address;		
	}
	ptcom->send_staradd[99]=1;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Read_Analog()//��ģ����
{
	U16 check;
	int check1,check2;
	int a1,a2,a3,a4;
	int add;
	int plcadd;
	int length;		
				
	add=ptcom->address;			      				//���������Ѿ�ת�����öεĿ�ʼ��ַ
	plcadd=ptcom->plc_address;	      				//PLCվ��ַ
	length=ptcom->register_length;
	
	*(U8 *)(AD1+0)=0x3a;							//��Ϣ֡��ð��(:)Ϊ��ʼ
	
	a1=(plcadd>>4)&0xf;
	a2=plcadd&0xf;
	*(U8 *)(AD1+1)=asicc(a1);          				//PLCվ��ַ
	*(U8 *)(AD1+2)=asicc(a2);          		

	switch (ptcom->registerr)	//���ݼĴ������ͻ��ƫ�Ƶ�ַ
	{
		case 'D':
		case 'c':
			*(U8 *)(AD1+3)=0x30;            		//������0x04���ɶ���д�Ĵ���3x
			*(U8 *)(AD1+4)=0x33;
			break;			
		case 'R':
		case 't':	
			*(U8 *)(AD1+3)=0x30;            		//������0x04��ֻ���Ĵ���4x
			*(U8 *)(AD1+4)=0x34;
			break;				
	}		
	
	a1=(add>>12)&0xf;
	a2=(add>>8)&0xf;
	a3=(add>>4)&0xf;
	a4=add&0xf;
	*(U8 *)(AD1+5)=asicc(a1);              			//��ʼ��ַ���ɸߵ���	
	*(U8 *)(AD1+6)=asicc(a2);	            		
	*(U8 *)(AD1+7)=asicc(a3);
	*(U8 *)(AD1+8)=asicc(a4);                 
	
	a1=(length>>12)&0xf;
	a2=(length>>8)&0xf;
	a3=(length>>4)&0xf;
	a4=length&0xf;
	*(U8 *)(AD1+9)=asicc(a1);              			//��ȡ���ȣ��ɸߵ���	
	*(U8 *)(AD1+10)=asicc(a2);	            		
	*(U8 *)(AD1+11)=asicc(a3);
	*(U8 *)(AD1+12)=asicc(a4);	
	
	Check[0]=plcadd;	 
	switch (ptcom->registerr)						//���ݼĴ�������ѡ��ͬ������
	{
		case 'D':
		case 'c':
			Check[1]=0x03;
			break;	
		case 'R':
		case 't':
			Check[1]=0x04;
			break;			
	}
	Check[2]=(add>>8)&0xff;
	Check[3]=add&0xff;
	Check[4]=(length>>8)&0xff;
	Check[5]=length&0xff;
	check=CalLRC(Check,6);         					//LRCУ��
	check1=(check>>4)&0xf;
	check2=check&0xf;
	*(U8 *)(AD1+13)=asicc(check1);          		//У�� ��λ
	*(U8 *)(AD1+14)=asicc(check2);          		//У�� ��λ
	
	*(U8 *)(AD1+15)=0x0d;							//ASCIIģʽ�����ַ����س�CR������LF
	*(U8 *)(AD1+16)=0x0a;	

	if (plcadd==0)								//�㲥ģʽ
	{	
		ptcom->send_length[0]=17;				//���ͳ���
		ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
		ptcom->send_times=1;					//���ʹ���
			
		ptcom->return_length[0]=0;				//�������ݳ��ȣ���5���̶�,У��
		ptcom->return_start[0]=0;				//����������Ч��ʼ
		ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
		ptcom->Current_Times=0;					//��ǰ���ʹ���	
		ptcom->send_add[0]=ptcom->address;		//�����������ַ������		
	}
	else
	{
		ptcom->send_length[0]=17;				//���ͳ���
		ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
		ptcom->send_times=1;					//���ʹ���
			
		ptcom->return_length[0]=11+length*4;		//�������ݳ��ȣ���5���̶�,У��
		ptcom->return_start[0]=7;				//����������Ч��ʼ
		ptcom->return_length_available[0]=length*4;//������Ч���ݳ���	
		ptcom->Current_Times=0;					//��ǰ���ʹ���	
		ptcom->send_add[0]=ptcom->address;		//�����������ַ������		
	}
	
	switch (ptcom->registerr)	//���ݼĴ������ͷ��ش���ͬ
	{
		case 'D':
		case 'R':
			ptcom->send_staradd[99]=1;
			break;			
		case 't':
		case 'c':	
			ptcom->send_staradd[99]=2;
			break;				
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Write_Analog()//дģ����
{
	U16 check;
	int check1,check2;
	int a1,a2,a3,a4;
	int add;
	int b1,b2,b3,b4,b5,b6,b7,b8;
	int i;
	int length;
	int plcadd;		
				
	add=ptcom->address;								//��ʼ��ַ
	plcadd=ptcom->plc_address;						//PLCվ��ַ
	length=ptcom->register_length;
	
	*(U8 *)(AD1+0)=0x3a;							//��Ϣ֡��ð��(:)Ϊ��ʼ
	
	if(length==1)
	{
		a1=(plcadd>>4)&0xf;
		a2=plcadd&0xf;
		*(U8 *)(AD1+1)=asicc(a1);          			//PLCվ��ַ
		*(U8 *)(AD1+2)=asicc(a2);          		
		
		*(U8 *)(AD1+3)=0x30;            			//������0x06��д�����Ĵ���
		*(U8 *)(AD1+4)=0x36;
	
		a1=(add>>12)&0xf;
		a2=(add>>8)&0xf;
		a3=(add>>4)&0xf;
		a4=add&0xf;
		*(U8 *)(AD1+5)=asicc(a1);              		//��ʼ��ַ���ɸߵ���	
		*(U8 *)(AD1+6)=asicc(a2);	            		
		*(U8 *)(AD1+7)=asicc(a3);
		*(U8 *)(AD1+8)=asicc(a4);	

		a1=ptcom->U8_Data[0];
		b1=(a1>>4)&0xf;
		b2=a1&0xf;
		a2=ptcom->U8_Data[1];
		b3=(a2>>4)&0xf;
		b4=a2&0xf;
		*(U8 *)(AD1+9)=asicc(b3);              		//д�����ֵ���ɸߵ���	
		*(U8 *)(AD1+10)=asicc(b4);	            		
		*(U8 *)(AD1+11)=asicc(b1);
		*(U8 *)(AD1+12)=asicc(b2);
		
		Check[0]=plcadd;
		Check[1]=0x06;
		Check[2]=(add>>8)&0xff;
		Check[3]=add&0xff;
		Check[4]=a1;
		Check[5]=a2;		
		check=CalLRC(Check,6);         				//LRCУ��
		check1=(check>>4)&0xf;
		check2=check&0xf;
		*(U8 *)(AD1+13)=asicc(check1);          	//У�� ��λ
		*(U8 *)(AD1+14)=asicc(check2);          	//У�� ��λ
		
		*(U8 *)(AD1+15)=0x0d;						//ASCIIģʽ�����ַ����س�CR������LF
		*(U8 *)(AD1+16)=0x0a;
		
		if (plcadd==0)								//�㲥ģʽ
		{
			ptcom->send_length[0]=17;		//���ͳ���
			ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
			ptcom->send_times=1;					//���ʹ���
				
			ptcom->return_length[0]=0;				//�������ݳ���
			ptcom->return_start[0]=0;				//����������Ч��ʼ
			ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
			ptcom->Current_Times=0;					//��ǰ���ʹ���	
		}
		else
		{
			ptcom->send_length[0]=17;		//���ͳ���
			ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
			ptcom->send_times=1;					//���ʹ���
				
			ptcom->return_length[0]=17;				//�������ݳ���
			ptcom->return_start[0]=0;				//����������Ч��ʼ
			ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
			ptcom->Current_Times=0;					//��ǰ���ʹ���	

		}				
	}
	
	if(length!=1)
	{
		a1=(plcadd>>4)&0xf;
		a2=plcadd&0xf;
		*(U8 *)(AD1+1)=asicc(a1);          			//PLCվ��ַ
		*(U8 *)(AD1+2)=asicc(a2);          		
		
		*(U8 *)(AD1+3)=0x31;            			//������0x10��д����Ĵ���
		*(U8 *)(AD1+4)=0x30;
	
		a1=(add>>12)&0xf;
		a2=(add>>8)&0xf;
		a3=(add>>4)&0xf;
		a4=add&0xf;
		*(U8 *)(AD1+5)=asicc(a1);              		//��ʼ��ַ���ɸߵ���	
		*(U8 *)(AD1+6)=asicc(a2);	            		
		*(U8 *)(AD1+7)=asicc(a3);
		*(U8 *)(AD1+8)=asicc(a4);
		
		a1=(length>>12)&0xf;
		a2=(length>>8)&0xf;
		a3=(length>>4)&0xf;
		a4=length&0xf;
		*(U8 *)(AD1+9)=asicc(a1);              		//д�볤��(����)���ɸߵ���	
		*(U8 *)(AD1+10)=asicc(a2);	            		
		*(U8 *)(AD1+11)=asicc(a3);
		*(U8 *)(AD1+12)=asicc(a4);
		
		a1=((length*2)>>4)&0xf;
		a2=(length*2)&0xf;
		*(U8 *)(AD1+13)=asicc(a1);					//д�볤��(�ֽ���)
		*(U8 *)(AD1+14)=asicc(a2);	

		switch (ptcom->registerr)
		{
			case 'D':
				for (i=0;i<length;i++)              		//д����Ԫ��ֵ
				{
					a1=ptcom->U8_Data[i*2];
					b1=(a1>>4)&0xf;
					b2=a1&0xf;
					a2=ptcom->U8_Data[i*2+1];
					b3=(a2>>4)&0xf;
					b4=a2&0xf;
					*(U8 *)(AD1+15+i*4)=asicc(b3);          //д�����ֵ���ɸߵ���	
					*(U8 *)(AD1+16+i*4)=asicc(b4);	            		
					*(U8 *)(AD1+17+i*4)=asicc(b1);
					*(U8 *)(AD1+18+i*4)=asicc(b2);			
				}
				break;
			case 'c':
				for (i=0;i<length/2;i++)              //д����Ԫ��ֵ
				{
					a1=ptcom->U8_Data[i*4];
					b1=(a1>>4)&0xf;
					b2=a1&0xf;
					a2=ptcom->U8_Data[i*4+1];
					b3=(a2>>4)&0xf;
					b4=a2&0xf;
					a3=ptcom->U8_Data[i*4+2];
					b5=(a3>>4)&0xf;
					b6=a3&0xf;
					a4=ptcom->U8_Data[i*4+3];
					b7=(a4>>4)&0xf;
					b8=a4&0xf;
					*(U8 *)(AD1+15+i*8)=asicc(b7);          //д�����ֵ���ɸߵ���	
					*(U8 *)(AD1+16+i*8)=asicc(b8);	            		
					*(U8 *)(AD1+17+i*8)=asicc(b5);
					*(U8 *)(AD1+18+i*8)=asicc(b6);					
					*(U8 *)(AD1+19+i*8)=asicc(b3);          //д�����ֵ���ɸߵ���	
					*(U8 *)(AD1+20+i*8)=asicc(b4);	            		
					*(U8 *)(AD1+21+i*8)=asicc(b1);
					*(U8 *)(AD1+22+i*8)=asicc(b2);					
				}
				break;
		}
		
		Check[0]=plcadd;
		Check[1]=0x10;
		Check[2]=(add>>8)&0xff;
		Check[3]=add&0xff;
		Check[4]=(length>>8)&0xff;
		Check[5]=length&0xff;		
		Check[6]=((length*2)>>4)&0xff;
		Check[7]=(length*2)&0xff;
		for (i=0;i<length;i++)              		//д����Ԫ��ֵ
		{
			Check[8+i*2]=ptcom->U8_Data[i*2];
			Check[9+i*2]=ptcom->U8_Data[i*2+1];
		}					
		check=CalLRC(Check,8+length*2); 			//LRCУ��
		check1=(check>>4)&0xf;
		check2=check&0xf;
		*(U8 *)(AD1+15+length*4)=asicc(check1);     //У�� ��λ
		*(U8 *)(AD1+16+length*4)=asicc(check2);     //У�� ��λ
		
		*(U8 *)(AD1+17+length*4)=0x0d;				//ASCIIģʽ�����ַ����س�CR������LF
		*(U8 *)(AD1+18+length*4)=0x0a;

		if (plcadd==0)								//�㲥ģʽ
		{
			ptcom->send_length[0]=19+length*4;		//���ͳ���
			ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
			ptcom->send_times=1;					//���ʹ���
				
			ptcom->return_length[0]=0;				//�������ݳ���
			ptcom->return_start[0]=0;				//����������Ч��ʼ
			ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
			ptcom->Current_Times=0;					//��ǰ���ʹ���	
		}
		else
		{
			ptcom->send_length[0]=19+length*4;		//���ͳ���
			ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
			ptcom->send_times=1;					//���ʹ���
				
			ptcom->return_length[0]=17;				//�������ݳ���
			ptcom->return_start[0]=0;				//����������Ч��ʼ
			ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
			ptcom->Current_Times=0;					//��ǰ���ʹ���	

		}		
	}
	ptcom->send_staradd[99]=0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Read_Recipe()//��ȡ�䷽
{
	U16 check;
	int check1,check2;
	int add;	
	int a1,a2,a3,a4;
	int i;
	int datalength;
	int p_start;
	int ps;
	int SendTimes;
	int LastTimeWord;								//���һ�η��ͳ���
	int currentlength;
	int plcadd;
			
	datalength=ptcom->register_length;				//�����ܳ���
	p_start=ptcom->address;							//��ʼ��ַ
	plcadd=ptcom->plc_address;
	
	if(datalength>5000)
		datalength=5000;                			//ÿ������ܷ���32��D
		
	if(datalength%32==0)
	{
		SendTimes=datalength/32;
		LastTimeWord=32;                			//�̶�����32	
	}
	if(datalength%32!=0)
	{
		SendTimes=datalength/32+1;      			//���͵Ĵ���
		LastTimeWord=datalength%32;     			//���һ�η��͵ĳ���	
	}
	
	for (i=0;i<SendTimes;i++)
	{
		ps=17*i;
		add=(p_start+i*32);							//����ƫ�Ƶ�ַ
		
		a1=(plcadd>>4)&0xf;
		a2=plcadd&0xf;		
			
		*(U8 *)(AD1+0+ps)=0x3a;						//��Ϣ֡��ð��(:)Ϊ��ʼ
		*(U8 *)(AD1+1+ps)=asicc(a1);          			//PLCվ��ַ
		*(U8 *)(AD1+2+ps)=asicc(a2);
		
		*(U8 *)(AD1+3+ps)=0x30;            			//������0x03�����Ĵ���
		*(U8 *)(AD1+4+ps)=0x33;		
	
		a1=(add>>12)&0xf;
		a2=(add>>8)&0xf;
		a3=(add>>4)&0xf;
		a4=add&0xf;
		*(U8 *)(AD1+5+ps)=asicc(a1);              		//��ʼ��ַ���ɸߵ���	
		*(U8 *)(AD1+6+ps)=asicc(a2);	            		
		*(U8 *)(AD1+7+ps)=asicc(a3);
		*(U8 *)(AD1+8+ps)=asicc(a4);	
	
		if (i!=(SendTimes-1))						//�������һ��ʱ
		{
			*(U8 *)(AD1+9+ps)=0x30;   				//�̶�����32������64�ֽ� ��λ
			*(U8 *)(AD1+10+ps)=0x30;      
			*(U8 *)(AD1+11+ps)=0x32; 				//�̶�����32������64�ֽ� ��λ
			*(U8 *)(AD1+12+ps)=0x30;        
			currentlength=32;
		}
		if (i==(SendTimes-1))						//���һ��ʱ
		{
			a1=(LastTimeWord>>12)&0xf;
			a2=(LastTimeWord>>8)&0xf;
			a3=(LastTimeWord>>4)&0xf;
			a4=LastTimeWord&0xf;		
			*(U8 *)(AD1+9+ps)=asicc(a1);   			//ʣ�೤��LastTimeWord���� ��λ
			*(U8 *)(AD1+10+ps)=asicc(a2);      
			*(U8 *)(AD1+11+ps)=asicc(a3); 			//ʣ�೤��LastTimeWord���� ��λ
			*(U8 *)(AD1+12+ps)=asicc(a4); 		
			currentlength=LastTimeWord;
		}
		
		Check[0]=plcadd;	 
		Check[1]=0x03;
		Check[2]=(add>>8)&0xff;
		Check[3]=add&0xff;
		Check[4]=(currentlength>>8)&0xff;
		Check[5]=currentlength&0xff;
		check=CalLRC(Check,6);         				//LRCУ��
		check1=(check>>4)&0xf;
		check2=check&0xf;
		*(U8 *)(AD1+13+ps)=asicc(check1);          	//У�� ��λ
		*(U8 *)(AD1+14+ps)=asicc(check2);          	//У�� ��λ
		
		*(U8 *)(AD1+15+ps)=0x0d;					//ASCIIģʽ�����ַ����س�CR������LF
		*(U8 *)(AD1+16+ps)=0x0a;	

		ptcom->send_length[i]=17;				    //���ͳ���
		ptcom->send_staradd[i]=i*17;			    //�������ݴ洢��ַ	
		ptcom->send_add[i]=p_start+i*32;		    //�����������ַ������	
		ptcom->send_data_length[i]=currentlength;	//�������һ�ζ���32��D
				
		ptcom->return_length[i]=1+currentlength*4;	//�������ݳ��ȣ���5���̶���plcadd��03���ֽ�����У��
		ptcom->return_start[i]=7;				    //����������Ч��ʼ
		ptcom->return_length_available[i]=currentlength*4;	//������Ч���ݳ���		
	}
	ptcom->send_times=SendTimes;					//���ʹ���
	ptcom->Current_Times=0;					        //��ǰ���ʹ���
	ptcom->send_staradd[99]=1;	
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Write_Recipe()//д�䷽��PLC
{
	int datalength;
	int staradd;
	int SendTimes;
	int LastTimeWord;								//���һ�η��ͳ���
	int i,j;
	int ps;
	int add;
	int a1,a2,a3,a4;
	int b1,b2,b3,b4;
	U16 check;
	int check1,check2;
	int plcadd;
	int length;
	
	
	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));//���ݳ���
	staradd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//���ݵ�ַ	
	plcadd=*(U8 *)(PE+4);	      					//PLCվ��ַ		  

	if(datalength%32==0)           					//ÿ������ܷ���32��D������ǳ�����32��D�ı���
	{
		SendTimes=datalength/32;   					//���͵Ĵ���
		LastTimeWord=32;           					//���һ�η��͵ĳ���	
	}
	if(datalength%32!=0)           					//������ǳ�����32��D�ı���
	{
		SendTimes=datalength/32+1; 					//���͵Ĵ���
		LastTimeWord=datalength%32;					//���һ�η��͵ĳ���	
	}
			
	ps=147;
	
	for (i=0;i<SendTimes;i++)
	{		
		if (i!=(SendTimes-1))     					//����һ�η���
		{	
			length=32;
		}
		else
		{
			length=LastTimeWord;
		}
		
		*(U8 *)(AD1+0+ps*i)=0x3a;					//��Ϣ֡��ð��(:)Ϊ��ʼ			
		
		a1=(plcadd>>4)&0xf;
		a2=plcadd&0xf;
		*(U8 *)(AD1+1+ps*i)=asicc(a1);          	//PLCվ��ַ
		*(U8 *)(AD1+2+ps*i)=asicc(a2);          		
		
		*(U8 *)(AD1+3+ps*i)=0x31;            		//������0x10��д����Ĵ���
		*(U8 *)(AD1+4+ps*i)=0x30;
	
		add=staradd+i*32;           				//��ʼ��ַ
	
		a1=(add>>12)&0xf;
		a2=(add>>8)&0xf;
		a3=(add>>4)&0xf;
		a4=add&0xf;
		*(U8 *)(AD1+5+ps*i)=asicc(a1);              //��ʼ��ַ���ɸߵ���	
		*(U8 *)(AD1+6+ps*i)=asicc(a2);	            		
		*(U8 *)(AD1+7+ps*i)=asicc(a3);
		*(U8 *)(AD1+8+ps*i)=asicc(a4);
		
		a1=(length>>12)&0xf;
		a2=(length>>8)&0xf;
		a3=(length>>4)&0xf;
		a4=length&0xf;
		*(U8 *)(AD1+9+ps*i)=asicc(a1);              //д�볤��(����)���ɸߵ���	
		*(U8 *)(AD1+10+ps*i)=asicc(a2);	            		
		*(U8 *)(AD1+11+ps*i)=asicc(a3);
		*(U8 *)(AD1+12+ps*i)=asicc(a4);
		
		a1=((length*2)>>4)&0xf;
		a2=(length*2)&0xf;
		*(U8 *)(AD1+13+ps*i)=asicc(a1);				//д�볤��(�ֽ���)
		*(U8 *)(AD1+14+ps*i)=asicc(a2);	

		for (j=0;j<length;j++)              		//д����Ԫ��ֵ
		{
			a1=*(U8 *)(PE+9+i*64+j*2+1);
			b1=(a1>>4)&0xf;
			b2=a1&0xf;
			a2=*(U8 *)(PE+9+i*64+j*2);
			b3=(a2>>4)&0xf;
			b4=a2&0xf;
			*(U8 *)(AD1+15+j*4+ps*i)=asicc(b1);     //д�����ֵ���ɸߵ���	
			*(U8 *)(AD1+16+j*4+ps*i)=asicc(b2);	            		
			*(U8 *)(AD1+17+j*4+ps*i)=asicc(b3);
			*(U8 *)(AD1+18+j*4+ps*i)=asicc(b4);			
		}
		
		Check[0]=plcadd;
		Check[1]=0x10;
		Check[2]=(add>>8)&0xff;
		Check[3]=add&0xff;
		Check[4]=(length>>8)&0xff;
		Check[5]=length&0xff;		
		Check[6]=(length*2)&0xff;
		for (j=0;j<length;j++)              		//д����Ԫ��ֵ
		{
			Check[7+j*2]=*(U8 *)(PE+9+i*64+j*2+1);
			Check[8+j*2]=*(U8 *)(PE+9+i*64+j*2);
		}					
		check=CalLRC(Check,7+length*2); 			//LRCУ��
		check1=(check>>4)&0xf;
		check2=check&0xf;
		*(U8 *)(AD1+15+length*4+ps*i)=asicc(check1);//У�� ��λ
		*(U8 *)(AD1+16+length*4+ps*i)=asicc(check2);//У�� ��λ
		
		*(U8 *)(AD1+17+length*4+ps*i)=0x0d;			//ASCIIģʽ�����ַ����س�CR������LF
		*(U8 *)(AD1+18+length*4+ps*i)=0x0a;		

		if (plcadd==0)								//�㲥ģʽ
		{		
			ptcom->send_length[i]=19+length*4;		//���ͳ���
			ptcom->send_staradd[i]=i*ps;			//�������ݴ洢��ַ
				
			ptcom->return_length[i]=0;				//�������ݳ���
			ptcom->return_start[i]=0;				//����������Ч��ʼ
			ptcom->return_length_available[i]=0;	//������Ч���ݳ���	
		}
		else
		{
			ptcom->send_length[i]=19+length*4;		//���ͳ���
			ptcom->send_staradd[i]=i*ps;			//�������ݴ洢��ַ
				
			ptcom->return_length[i]=17;				//�������ݳ���
			ptcom->return_start[i]=0;				//����������Ч��ʼ
			ptcom->return_length_available[i]=0;	//������Ч���ݳ���			
		}			
	}
	ptcom->send_times=SendTimes;					//���ʹ���
	ptcom->Current_Times=0;					    	//��ǰ���ʹ���
	ptcom->send_staradd[99]=0;				
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Write_Time()//дʱ�䵽PLC
{
	Write_Analog();							
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Read_Time()//��PLC��ȡʱ��
{
	Read_Analog();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void compxy(void)//����ɱ�׼�洢��ʽ
{
	int i;
	unsigned char a1,a2,a3,a4,a5,a6,a7,a8;
	int b1,b2,b3,b4;

	if (ptcom->send_staradd[99] == 1)
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/2;i++)
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2+0);
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2+1);
			a1=bsicc(a1);
			a2=bsicc(a2);
			b1=(a1<<4)+a2;	
			*(U8 *)(COMad+i)=b1;
		}
		ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;
	}
	if (ptcom->send_staradd[99] == 2)
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/8;i++)
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*8+0);
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
			b1=(a1<<4)+a2;
			b2=(a3<<4)+a4;
			b3=(a5<<4)+a6;
			b4=(a7<<4)+a8;	
			*(U8 *)(COMad+i*4)=b3;
			*(U8 *)(COMad+i*4+1)=b4;
			*(U8 *)(COMad+i*4+2)=b1;
			*(U8 *)(COMad+i*4+3)=b2;
		}
		ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;	
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int remark()//�����������ݼ���У�����Ƿ���ȷ
{	
	unsigned int Return_Check1;								//���ش�����У��ֵ��λ
	unsigned int Return_Check2;								//���ش�����У��ֵ��λ
	unsigned int Check_LRC;									//�÷��صĴ������У��������
	unsigned int Check1;									//�÷��صĴ������У����������λ
	unsigned int Check2;									//�÷��صĴ������У����������λ
	int a1,a2;
	int i,b1;

	Return_Check2=*(U8 *)(COMad+ptcom->return_length[ptcom->Current_Times-1]-3)&0xff;//�ڷ������Current_Times++����ʱҪ--
	Return_Check1=*(U8 *)(COMad+ptcom->return_length[ptcom->Current_Times-1]-4)&0xff;
	Return_Check1=bsicc(Return_Check1);
	Return_Check2=bsicc(Return_Check2);	
	
	for (i=0;i<(ptcom->return_length[ptcom->Current_Times-1]-5)/2;i++)
	{
		a1=*(U8 *)(COMad+i*2+1);
		a2=*(U8 *)(COMad+i*2+2);
		a1=bsicc(a1);
		a2=bsicc(a2);;
		b1=(a1<<4)+a2;			
		Check[i]=b1;		
	}
		
	Check_LRC=CalLRC(Check,(ptcom->return_length[ptcom->Current_Times-1]-5)/2);
	Check1=((Check_LRC&0xf0)>>4)&0xf;
	Check2=Check_LRC&0xf;
	
	if((Check1==Return_Check1)&&(Check2==Return_Check2))
	{
		return 1;
	}
	else
	{
		return 0;
	}			
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
U16 CalLRC(unsigned char *chData,U16 uNo)//LRCУ�飬��16������У�飬asicc����ʾ
{
	int i;
	U16 ab=0;
	for(i=0;i<uNo;i++)
	{
		ab=ab+chData[i];
	}
	ab=~(ab);
	ab=(ab&0xff)+1;
	return (ab);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int asicc(int a)//תΪAsc��
{
	int bl;
	if(a<10)
		bl=a+0x30;
	if(a>9)
		bl=a-10+0x41;
	return bl;	
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int bsicc(int a)//AscתΪ����
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
