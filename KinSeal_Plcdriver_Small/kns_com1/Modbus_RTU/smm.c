/******************************************************************************************
 * 2011-12-07
 * ����3x_D��4x_D��д�䷽��־����Ӻ����ݵĵߵ�
******************************************************************************************/
#include "stdio.h"
#include "def.h"
#include "smm.h"


	
struct Com_struct_D *ptcom;




void Enter_PlcDriver(void)
{
//	sysprintf("Enter_PlcDriver !!!!!!\n");		

	LB=*(U32 *)(LB_Address+0);
	LW=*(U32 *)(LW_Address+0);
	RWI=*(U32 *)(RWI_Address+0);
	
	ptcom=(struct Com_struct_D *)adcom;
	
//	sysprintf("enter plc driver %c,ptcom->R_W_Flag %d\n",ptcom->registerr,ptcom->R_W_Flag);		
		
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
				case 'K': //4x_bit	
				case 'H':
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
				case 'K':
					Write_Wordbit();
					break;
				default:
				ptcom->send_length[0]=0;		//���ͳ���
				ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
				ptcom->send_times=0;					//���ʹ���
				
				ptcom->return_length[0]=0;				//�������ݳ���
				ptcom->return_start[0]=0;				//����������Ч��ʼ
				ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
				ptcom->Current_Times=0;	
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



void Set_Reset()                     //��λ�͸�λ
{
	U16 aakj;
	int b;
	int a1,a2,a3,a4;
	int add;
	int plcadd;	

	b=ptcom->address;			     // ��λ��ַ
	plcadd=ptcom->plc_address;	     //PLCվ��ַ
	switch (ptcom->registerr)	     //���ݼĴ������ͻ��ƫ�Ƶ�ַ
	{
	case 'M':
		add=0x0;
		break;					
	}
	b=b+add;					    //��ʼ��ַƫ�ƣ�PLC��ַ��һ��ʼ
	a1=(b>>8)&0xff;	                //��λ
	a2=b&0xff;	                    //��λ

	
	*(U8 *)(AD1+0)=plcadd;          //PLC��ַ
	*(U8 *)(AD1+1)=0x05;            //������0x05��ǿ����λ�͸�λ
	*(U8 *)(AD1+2)=a1;              //��λ
	*(U8 *)(AD1+3)=a2;	            //��λ

	if (ptcom->writeValue==1)	    //��λ
	{
		*(U8 *)(AD1+4)=0xff;
		*(U8 *)(AD1+5)=0x00;		
	}
	if (ptcom->writeValue==0)	    //��λ
	{
		*(U8 *)(AD1+4)=0x00;
		*(U8 *)(AD1+5)=0x00;
	}	
	aakj= CalcCrc((U8 *)AD1,6);         //У��
	a3=aakj/0x100;
	a4=aakj-a1*0x100;
	*(U8 *)(AD1+6)=a4&0xff;             //У�� ��λ
	*(U8 *)(AD1+7)=a3&0xff;             //У�� ��λ

	if (plcadd==0)								//�㲥ģʽ
	{
		ptcom->send_length[0]=8;				//���ͳ���
		ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
		ptcom->send_times=1;					//���ʹ���
			
		ptcom->return_length[0]=0;				//�������ݳ���
		ptcom->return_start[0]=0;				//����������Ч��ʼ
		ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
		ptcom->Current_Times=0;					//��ǰ���ʹ���	
	}
	else
	{
		ptcom->send_length[0]=8;				//���ͳ���
		ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
		ptcom->send_times=1;					//���ʹ���
			
		ptcom->return_length[0]=8;				//�������ݳ���
		ptcom->return_start[0]=0;				//����������Ч��ʼ
		ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
		ptcom->Current_Times=0;					//��ǰ���ʹ���			
	}
	
	ptcom->send_staradd[99]=0;			
}


void Read_Bool()				//��ȡ����������Ϣ
{
	U16 aakj;
	int b;
	int a1,a2,a3,a4;
	int add;
	int b1,b2;
	int plcadd;
	int length;
	int t;
		
	switch (ptcom->registerr)	//���ݼĴ������ͻ��ƫ�Ƶ�ַ
	{
	case 'X':
		add=0x0;
		t=0x02;
		break;	
	case 'M':
		add=0x0;
		t=0x01;
		break;					
	}		
				
	b=ptcom->address;			//���������Ѿ�ת�����öεĿ�ʼ��ַ
	plcadd=ptcom->plc_address;	//PLCվ��ַ
	length=ptcom->register_length;
	length=length*8;            //һ��Ԫ������8λ����
	
	b=b+add;					//����ƫ�Ƶ�ַ
	a1=(b>>8)&0xff;             //��λ
	a2=b&0xff;                  //��λ
	
	a3=(length>>8)&0xff;        //��λ
	a4=length&0xff;             //��λ
	
	*(U8 *)(AD1+0)=plcadd;
	*(U8 *)(AD1+1)=t&0xff;
	*(U8 *)(AD1+2)=a1;	        //��ʼ��ַ  ��λ
	*(U8 *)(AD1+3)=a2;          //��ʼ��ַ  ��λ
	*(U8 *)(AD1+4)=a3;          //Ԫ������  ��λ
	*(U8 *)(AD1+5)=a4;          //Ԫ������  ��λ
	aakj= CalcCrc((U8 *)AD1,6); //У��
	b1=aakj/0x100;
	b2=aakj-a1*0x100;
	*(U8 *)(AD1+6)=b2&0xff;     //У�� ��λ
	*(U8 *)(AD1+7)=b1&0xff;     //У�� ��λ
	
	if (plcadd==0)								//�㲥ģʽ
	{	
		ptcom->send_length[0]=8;				     //���ͳ���
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
		ptcom->send_length[0]=8;				     //���ͳ���
		ptcom->send_staradd[0]=0;				     //�������ݴ洢��ַ	
		ptcom->send_times=1;					     //���ʹ���
			
		ptcom->return_length[0]=5+length/8;		     //�������ݳ��ȣ���5���̶�,У��
		ptcom->return_start[0]=3;				     //����������Ч��ʼ
		ptcom->return_length_available[0]=length/8;    //������Ч���ݳ���	
		ptcom->Current_Times=0;					     //��ǰ���ʹ���	
		ptcom->send_add[0]=ptcom->address;		     //�����������ַ������	
		ptcom->address=ptcom->address;		
	}	
	ptcom->send_staradd[99]=1;		
}



void Read_Analog()				//��ģ����
{
	U16 aakj;
	int b;
	int a1,a2,a3,a4;
	int add;
	int b1,b2;
	int plcadd;
	int length;
	int t;
			
	switch (ptcom->registerr)	//���ݼĴ������ͻ��ƫ�Ƶ�ַ
	{
	case 'D':
	case 'c':
	case 'K':
		add=0x0;
		t=0x03;
		break;			
	case 'R':
	case 't':
	case 'H':	
		add=0x0;
		t=0x04;
		break;				
	}		
				
	b=ptcom->address;			      //���������Ѿ�ת�����öεĿ�ʼ��ַ
	plcadd=ptcom->plc_address;	      //PLCվ��ַ
	length=ptcom->register_length;
	
	if(ptcom->registerr == 'K' || ptcom->registerr == 'H')
	{
		b = b/16;
	}
	b=b+add;					      //����ƫ�Ƶ�ַ
	a1=(b>>8)&0xff;
	a2=b&0xff;
	
	a3=(length>>8)&0xff;
	a4=length&0xff;
	
	*(U8 *)(AD1+0)=plcadd;
	*(U8 *)(AD1+1)=t&0xff;
	*(U8 *)(AD1+2)=a1;	             //��ʼ��ַ ��λ
	*(U8 *)(AD1+3)=a2;               //��ʼ��ַ ��λ
	*(U8 *)(AD1+4)=a3;               //Ԫ������ ��λ
	*(U8 *)(AD1+5)=a4;               //Ԫ������ ��λ
	aakj= CalcCrc((U8 *)AD1,6);      //У��
	b1=aakj/0x100;
	b2=aakj-a1*0x100;
	*(U8 *)(AD1+6)=b2&0xff;     //У�� ��λ
	*(U8 *)(AD1+7)=b1&0xff;     //У�� ��λ
	
	if (plcadd==0)								//�㲥ģʽ
	{	
		ptcom->send_length[0]=8;				//���ͳ���
		ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
		ptcom->send_times=1;					//���ʹ���
			
		ptcom->return_length[0]=0;				//�������ݳ��ȣ���5���̶�,У��
		ptcom->return_start[0]=3;				//����������Ч��ʼ
		ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
		ptcom->Current_Times=0;					//��ǰ���ʹ���	
		ptcom->send_add[0]=ptcom->address;		//�����������ַ������		
	}
	else
	{
		ptcom->send_length[0]=8;				//���ͳ���
		ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
		ptcom->send_times=1;					//���ʹ���
			
		ptcom->return_length[0]=5+length*2;		//�������ݳ��ȣ���5���̶�,У��
		ptcom->return_start[0]=3;				//����������Ч��ʼ
		ptcom->return_length_available[0]=length*2;//������Ч���ݳ���	
		ptcom->Current_Times=0;					//��ǰ���ʹ���	
		ptcom->send_add[0]=ptcom->address;		//�����������ַ������		
	}	
	if(ptcom->registerr == 'K' || ptcom->registerr == 'H')
	{
		ptcom->send_staradd[99]=99;
	}
	else
	{
		if(ptcom->registerr == 'D' || ptcom->registerr == 'R')
		{
			ptcom->send_staradd[99]=1;
		}
		else if(ptcom->registerr == 't' || ptcom->registerr == 'c')
		{
			ptcom->send_staradd[99]=2;
		}			
	}
}


void Read_Recipe()								//��ȡ�䷽
{
	U16 aakj;
	int b;
	int a1,a2;
	int i;
	int datalength;
	int p_start;
	int ps;
	int SendTimes;
	int LastTimeWord;							//���һ�η��ͳ���
	int currentlength;
	int plcadd;
	int b1,b2;
	int t;
	
		
	datalength=ptcom->register_length;		//�����ܳ���
	p_start=ptcom->address;					//��ʼ��ַ
	plcadd=ptcom->plc_address;
	
	if(datalength>5000)
		datalength=5000;                //ÿ������ܷ���32��D
		
	if(datalength%32==0)
	{
		SendTimes=datalength/32;
		LastTimeWord=32;                //�̶�����32	
	}
	if(datalength%32!=0)
	{
		SendTimes=datalength/32+1;      //���͵Ĵ���
		LastTimeWord=datalength%32;     //���һ�η��͵ĳ���	
	}

	switch (ptcom->registerr)	//���ݼĴ������ͻ��ƫ�Ƶ�ַ
	{
	case 'D':
	case 'c':
		t=0x03;
		break;			
	case 'R':
	case 't':	
		t=0x04;
		break;				
	}
	
	for (i=0;i<SendTimes;i++)
	{
		ps=8*i;
		b=(p_start+i*32);				//����ƫ�Ƶ�ַ
		
		a1=(b>>8)&0xff;
		a2=b&0xff;
			
		*(U8 *)(AD1+0+ps)=plcadd;      //PLCվ��ַ
		*(U8 *)(AD1+1+ps)=t;        //������
		*(U8 *)(AD1+2+ps)=a1;          //��ʼ��ַ��λ
		*(U8 *)(AD1+3+ps)=a2;          //��ʼ��ַ��λ
	
		if (i!=(SendTimes-1))	//�������һ��ʱ
		{
			*(U8 *)(AD1+4+ps)=0x0;   //�̶�����32������64�ֽ� ��λ
			*(U8 *)(AD1+5+ps)=0x20;        //�̶�����32������64�ֽ� ��λ
			currentlength=32;
		}
		if (i==(SendTimes-1))	//���һ��ʱ
		{
			*(U8 *)(AD1+4+ps)=(LastTimeWord>>8)&0xff;   //ʣ�೤��LastTimeWord���� ��λ
			*(U8 *)(AD1+5+ps)=LastTimeWord&0xff;        //ʣ�೤��LastTimeWord���� ��λ
			currentlength=LastTimeWord;
		}
		aakj= CalcCrc((U8 *)(AD1+ps),6);      //У��
		b1=aakj/0x100;
		b2=aakj-a1*0x100;
		*(U8 *)(AD1+6+ps)=b2&0xff;     //У�� ��λ
		*(U8 *)(AD1+7+ps)=b1&0xff;     //У�� ��λ			
			
		
		ptcom->send_length[i]=8;				    //���ͳ���
		ptcom->send_staradd[i]=i*8;			        //�������ݴ洢��ַ	
		ptcom->send_add[i]=p_start+i*32;		    //�����������ַ������	
		ptcom->send_data_length[i]=currentlength;	//�������һ�ζ���32��D
				
		ptcom->return_length[i]=5+currentlength*2;	//�������ݳ��ȣ���5���̶���plcadd��03���ֽ�����У��
		ptcom->return_start[i]=3;				    //����������Ч��ʼ
		ptcom->return_length_available[i]=currentlength*2;	//������Ч���ݳ���		
	}
	ptcom->send_times=SendTimes;					//���ʹ���
	ptcom->Current_Times=0;					        //��ǰ���ʹ���	

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
void Write_Wordbit()
{
	U16 aakj;
	int b;
	int a1,a2,a3,a4;
	int add;
	int b1,b2;
	int plcadd;
	int length;
	int t;
		
	switch (ptcom->registerr)	//���ݼĴ������ͻ��ƫ�Ƶ�ַ
	{
	case 'K':
		add=0x0;
		t=0x03;
		break;			
	case 'H':	
		add=0x0;
		t=0x04;
		break;				
	}		
				
	b=ptcom->address;			      //���������Ѿ�ת�����öεĿ�ʼ��ַ
	plcadd=ptcom->plc_address;	      //PLCվ��ַ
	length=ptcom->register_length;
	
	if(ptcom->registerr == 'K')
	{
		b = b/16;
	}
	b=b+add;					      //����ƫ�Ƶ�ַ
	a1=(b>>8)&0xff;
	a2=b&0xff;
	
	a3=(length>>8)&0xff;
	a4=length&0xff;
	
	*(U8 *)(AD1+0)=plcadd;
	*(U8 *)(AD1+1)=t&0xff;
	*(U8 *)(AD1+2)=a1;	             //��ʼ��ַ ��λ
	*(U8 *)(AD1+3)=a2;               //��ʼ��ַ ��λ
	*(U8 *)(AD1+4)=a3;               //Ԫ������ ��λ
	*(U8 *)(AD1+5)=a4;               //Ԫ������ ��λ
	aakj= CalcCrc((U8 *)AD1,6);      //У��
	b1=aakj/0x100;
	b2=aakj-a1*0x100;
	*(U8 *)(AD1+6)=b2&0xff;     //У�� ��λ
	*(U8 *)(AD1+7)=b1&0xff;     //У�� ��λ
	
	if (plcadd==0)								//�㲥ģʽ
	{	
		ptcom->send_length[0]=8;				//���ͳ���
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
		ptcom->send_length[0]=8;				//���ͳ���
		ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
		ptcom->send_times=2;					//���ʹ���
			
		ptcom->return_length[0]=5+length*2;		//�������ݳ��ȣ���5���̶�,У��
		ptcom->return_start[0]=3;				//����������Ч��ʼ
		ptcom->return_length_available[0]=length*2;//������Ч���ݳ���	
		ptcom->Current_Times=0;					//��ǰ���ʹ���	
		ptcom->send_add[0]=ptcom->address;		//�����������ַ������		
	}	

	*(U8 *)(AD1+8)=plcadd;        //PLC��ַ
	*(U8 *)(AD1+9)=0x06;          //������ д�����Ĵ���
	*(U8 *)(AD1+10)=a1;            //��ʼ��ַ ��λ
	*(U8 *)(AD1+11)=a2;            //��ʼ��ַ ��λ
	*(U8 *)(AD1+12)=0x00;            //д��Ԫ��ֵ��λ
	*(U8 *)(AD1+13)=0x00;            //д��Ԫ��ֵ��λ
	aakj= CalcCrc(((U8 *)AD1 + 8),6);   //У��
	b1=aakj/0x100;
	b2=aakj-a1*0x100;
	*(U8 *)(AD1+14)=b2&0xff;     //У�� ��λ
	*(U8 *)(AD1+15)=b1&0xff;     //У�� ��λ
	
	
	ptcom->send_length[1]=8;				//���ͳ���
	ptcom->send_staradd[1]=8;				//�������ݴ洢��ַ	
		ptcom->send_times=2;					//���ʹ���
			
		ptcom->return_length[1]=8;		//�������ݳ��ȣ���5���̶�,У��
		ptcom->return_start[1]=0;				//����������Ч��ʼ
		ptcom->return_length_available[1]=0;//������Ч���ݳ���	
		ptcom->Current_Times=0;					//��ǰ���ʹ���	
		ptcom->send_add[1]=0;		//�����������ַ������	
		
	ptcom->send_staradd[99]= 100;//�ȶ���д	
}


void Write_Analog()				//дģ����
{
	U16 aakj;
	int b;
	int a1,a2,a3,a4;
	int b1,b2;
	int i;
	int length;
	int plcadd;		
				
	b=ptcom->address;			//��ʼ��ַ
	plcadd=ptcom->plc_address;	//PLCվ��ַ
	length=ptcom->register_length;
	
		
	a1=(b>>8)&0xff;	            
	a2=b&0xff;
		
	if(length==1)
	{	
		*(U8 *)(AD1+0)=plcadd;        //PLC��ַ
		*(U8 *)(AD1+1)=0x06;          //������ д�����Ĵ���
		*(U8 *)(AD1+2)=a1;            //��ʼ��ַ ��λ
		*(U8 *)(AD1+3)=a2;            //��ʼ��ַ ��λ
		a3=ptcom->U8_Data[0];
		a4=ptcom->U8_Data[1];
		*(U8 *)(AD1+4)=a4;            //д��Ԫ��ֵ��λ
		*(U8 *)(AD1+5)=a3;            //д��Ԫ��ֵ��λ
		aakj= CalcCrc((U8 *)AD1,6);   //У��
		b1=aakj/0x100;
		b2=aakj-a1*0x100;
		*(U8 *)(AD1+6)=b2&0xff;     //У�� ��λ
		*(U8 *)(AD1+7)=b1&0xff;     //У�� ��λ
		
		if (plcadd==0)								//�㲥ģʽ
		{
			ptcom->send_length[0]=8;		//���ͳ���
			ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
			ptcom->send_times=1;					//���ʹ���
				
			ptcom->return_length[0]=0;				//�������ݳ���
			ptcom->return_start[0]=0;				//����������Ч��ʼ
			ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
			ptcom->Current_Times=0;					//��ǰ���ʹ���	
		}
		else
		{
			ptcom->send_length[0]=8;		//���ͳ���
			ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
			ptcom->send_times=1;					//���ʹ���
				
			ptcom->return_length[0]=8;				//�������ݳ���
			ptcom->return_start[0]=0;				//����������Ч��ʼ
			ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
			ptcom->Current_Times=0;					//��ǰ���ʹ���	

		}						
	}
	
	if(length!=1)
	{
		*(U8 *)(AD1+0)=plcadd;        //PLC��ַ
		*(U8 *)(AD1+1)=0x10;          //������ д����Ĵ���
		*(U8 *)(AD1+2)=a1;            //��ʼ��ַ ��λ
		*(U8 *)(AD1+3)=a2;            //��ʼ��ַ ��λ
		
		a3=(length>>8)&0xff;
		a4=length&0xff;
		
		*(U8 *)(AD1+4)=a3;            //Ԫ��������λ
		*(U8 *)(AD1+5)=a4;            //Ԫ��������λ
		*(U8 *)(AD1+6)=length*2;
		switch (ptcom->registerr)
		{
			case 'D':
				for (i=0;i<length;i++)              //д����Ԫ��ֵ
				{				
					a3=ptcom->U8_Data[i*2];
					a4=ptcom->U8_Data[i*2+1];
					*(U8 *)(AD1+7+i*2)=a4;          //д��Ԫ��ֵ��λ
					*(U8 *)(AD1+8+i*2)=a3;          //д��Ԫ��ֵ��λ		
				}
				break;
			case 'c':
				for (i=0;i<length/2;i++)              //д����Ԫ��ֵ
				{				
					a1=ptcom->U8_Data[i*4];
					a2=ptcom->U8_Data[i*4+1];					
					a3=ptcom->U8_Data[i*4+2];
					a4=ptcom->U8_Data[i*4+3];
					*(U8 *)(AD1+7+i*4)=a4;          //д��Ԫ��ֵ��λ
					*(U8 *)(AD1+8+i*4)=a3;          //д��Ԫ��ֵ��λ
					*(U8 *)(AD1+9+i*4)=a2;          //д��Ԫ��ֵ��λ
					*(U8 *)(AD1+10+i*4)=a1;          //д��Ԫ��ֵ��λ		
				}
				break;
		}
		aakj=CalcCrc((U8 *)AD1,7+length*2); //У��
		b1=aakj/0x100;
		b2=aakj-a1*0x100;
		*(U8 *)(AD1+7+length*2)=b2&0xff;     //У�� ��λ
		*(U8 *)(AD1+8+length*2)=b1&0xff;     //У�� ��λ
		
		if (plcadd==0)								//�㲥ģʽ
		{
			ptcom->send_length[0]=9+length*2;		//���ͳ���
			ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
			ptcom->send_times=1;					//���ʹ���
				
			ptcom->return_length[0]=0;				//�������ݳ���
			ptcom->return_start[0]=0;				//����������Ч��ʼ
			ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
			ptcom->Current_Times=0;					//��ǰ���ʹ���	
		}
		else
		{
			ptcom->send_length[0]=9+length*2;		//���ͳ���
			ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
			ptcom->send_times=1;					//���ʹ���
				
			ptcom->return_length[0]=8;				//�������ݳ���
			ptcom->return_start[0]=0;				//����������Ч��ʼ
			ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
			ptcom->Current_Times=0;					//��ǰ���ʹ���	

		}				//��ǰ���ʹ���	
	}
	ptcom->send_staradd[99]=0;
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
	int a1,a2;
	U16 aakj;
	int plcadd;
	int length;
	
	
	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));  //���ݳ���
	staradd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//���ݳ��� 	
	plcadd=*(U8 *)(PE+4);	      //PLCվ��ַ		  


	if(datalength%32==0)           //ÿ������ܷ���32��D������ǳ�����32��D�ı���
	{
		SendTimes=datalength/32;   //���͵Ĵ���
		LastTimeWord=32;           //���һ�η��͵ĳ���	
	}
	if(datalength%32!=0)           //������ǳ�����32��D�ı���
	{
		SendTimes=datalength/32+1; //���͵Ĵ���
		LastTimeWord=datalength%32;//���һ�η��͵ĳ���	
	}
			
	ps=73;
	
	for (i=0;i<SendTimes;i++)
	{		
	if (i!=(SendTimes-1))     //����һ�η���
		{	
			length=32;
		}
		else
		{
			length=LastTimeWord;
		}
		
		b=staradd+i*32;           //��ʼ��ַ?
		a1=(b>>8)&0xff;
		a2=b&0xff;	
		*(U8 *)(AD1+0+ps*i)=plcadd;
		*(U8 *)(AD1+1+ps*i)=0x10;
		*(U8 *)(AD1+2+ps*i)=a1;
		*(U8 *)(AD1+3+ps*i)=a2;
		*(U8 *)(AD1+4+ps*i)=(length>>8)&0xff;
		*(U8 *)(AD1+5+ps*i)=(length)&0xff;
		*(U8 *)(AD1+6+ps*i)=length*2;	

		//for(j=0;j<length;j++)
		//{
			//*(U8 *)(AD1+7+ps*i+j*2)=*(U8 *)(PE+9+i*64+j*2+1);	
			//*(U8 *)(AD1+7+ps*i+j*2+1)=*(U8 *)(PE+9+i*64+j*2);			
		//}
		
	    switch (ptcom->registerr)
		{
			case 'D':
				for (j=0;j<length;j++)              //д����Ԫ��ֵ
				{				
					//a3=ptcom->U8_Data[i*2];
					//a4=ptcom->U8_Data[i*2+1];
					//*(U8 *)(AD1+7+i*2)=a4;          //д��Ԫ��ֵ��λ
					//*(U8 *)(AD1+8+i*2)=a3;          //д��Ԫ��ֵ��λ
				    *(U8 *)(AD1+7+ps*i+j*2)=*(U8 *)(PE+9+i*64+j*2+1);	
					*(U8 *)(AD1+7+ps*i+j*2+1)=*(U8 *)(PE+9+i*64+j*2);		
				}
				break;
			case 'c':
				for (j=0;j<length/2;j++)              //д����Ԫ��ֵ
				{				
					//a1=ptcom->U8_Data[i*4];
					//a2=ptcom->U8_Data[i*4+1];					
					//a3=ptcom->U8_Data[i*4+2];
					////a4=ptcom->U8_Data[i*4+3];
					//*(U8 *)(AD1+7+i*4)=a4;          //д��Ԫ��ֵ��λ
					//*(U8 *)(AD1+8+i*4)=a3;          //д��Ԫ��ֵ��λ
					//*(U8 *)(AD1+9+i*4)=a2;          //д��Ԫ��ֵ��λ
					//*(U8 *)(AD1+10+i*4)=a1;         //д��Ԫ��ֵ��λ	
					*(U8 *)(AD1+7+ps*i+j*4)=*(U8 *)(PE+9+i*64+j*4+4);	
					*(U8 *)(AD1+7+ps*i+j*4+1)=*(U8 *)(PE+9+i*64+j*4 + 3);
					*(U8 *)(AD1+7+ps*i+j*4+2)=*(U8 *)(PE+9+i*64+j*4+2);	
					*(U8 *)(AD1+7+ps*i+j*4+3)=*(U8 *)(PE+9+i*64+j*4 + 1);	
				}
				break;
		}		
		
		aakj=CalcCrc((U8 *)AD1+ps*i,7+length*2);
		a1=aakj/0x100;
		a2=aakj-a1*0x100;
		*(U8 *)(AD1+7+ps*i+length*2)=a2;
		*(U8 *)(AD1+8+ps*i+length*2)=a1;
		
		if (plcadd==0)								//�㲥ģʽ
		{		
			ptcom->send_length[i]=9+length*2;		//���ͳ���
			ptcom->send_staradd[i]=i*ps;			//�������ݴ洢��ַ
				
			ptcom->return_length[i]=0;				//�������ݳ���
			ptcom->return_start[i]=0;				//����������Ч��ʼ
			ptcom->return_length_available[i]=0;	//������Ч���ݳ���	
		}
		else
		{
			ptcom->send_length[i]=9+length*2;		//���ͳ���
			ptcom->send_staradd[i]=i*ps;			//�������ݴ洢��ַ
				
			ptcom->return_length[i]=8;				//�������ݳ���
			ptcom->return_start[i]=0;				//����������Ч��ʼ
			ptcom->return_length_available[i]=0;	//������Ч���ݳ���			
		}				
	}
	ptcom->send_times=SendTimes;				//���ʹ���
	ptcom->Current_Times=0;					    //��ǰ���ʹ���
	ptcom->send_staradd[99]=0;		
}


void compxy(void)				//����ɱ�׼�洢��ʽ
{
	int i;
	unsigned char a1,a2,a3,a4;
	unsigned short nTemp = 0;
	int nBitPos = 0;
	int b1,b2;
	unsigned short aakj;
	
	if (ptcom->send_staradd[99] == 99)//K H
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/2;i++)						//ASC�귵�أ�����ҪתΪ16����
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2 + 1);
			a2 = *(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2 + 0);
			*(U8 *)(COMad+i*2 + 0)=a1;
			*(U8 *)(COMad+i*2 + 1)=a2;							//���´�,�ӵ�0����ʼ��
		}
		return;	
	}
	else if (ptcom->send_staradd[99] == 1)
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1];i++)						//ASC�귵�أ�����ҪתΪ16����
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i);
			*(U8 *)(COMad+i)=a1;							//���´�,�ӵ�0����ʼ��
		}
	}
	else if (ptcom->send_staradd[99] == 2)
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/4;i++)						//ASC�귵�أ�����ҪתΪ16����
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+0);
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+1);
			a3=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+2);
			a4=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+3);
	//		sysprintf("ptcom->registerr %c i %d a1 0x%x,a2 0x%x,a3 0x%x,a4 0x%x\n",ptcom->registerr,i,a1,a2,a3,a4);
			*(U8 *)(COMad+i*4+0)=a3;							//���´�,�ӵ�0����ʼ��
			*(U8 *)(COMad+i*4+1)=a4;
			*(U8 *)(COMad+i*4+2)=a1;
			*(U8 *)(COMad+i*4+3)=a2;
		}
	}
	else if(ptcom->send_staradd[99] == 100)
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1];i++)						//ASC�귵�أ�����ҪתΪ16����
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i);
			*(U8 *)(COMad+i)=a1;							//���´�,�ӵ�0����ʼ��
		}
		nTemp = *(U8 *)(COMad+0) & 0xff;
		nTemp = (nTemp << 8) & 0xff00;
		nTemp = nTemp |  *(U8 *)(COMad+1);
		nBitPos = ptcom->address%16;
		
		if (ptcom->writeValue==1)	    //��λ
		{
			nTemp = nTemp | (1 << nBitPos)	;	
		}
		else if (ptcom->writeValue==0)	    //��λ
		{
			nTemp = nTemp & (~(1 << nBitPos));
		}
		
		*(U8 *)(AD1+13)=nTemp & 0xff;            //д��Ԫ��ֵ��λ
		*(U8 *)(AD1+12)=(nTemp >> 8) &0xff;            //д��Ԫ��ֵ��λ
		
		aakj= CalcCrc(((U8 *)AD1 + 8),6);   //У��
		b1=aakj/0x100;
		b2=aakj-a1*0x100;
		*(U8 *)(AD1+14)=b2&0xff;     //У�� ��λ
		*(U8 *)(AD1+15)=b1&0xff;     //У�� ��λ
		
	}
}


void watchcom(void)		//�������У��
{
	unsigned int aakj=0;
	aakj=remark();
	sysprintf("aakj %d\n",aakj);		
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


int remark()				//�����������ݼ���У�����Ƿ���ȷ
{
	unsigned int aakj1;
	unsigned int aakj2;
	unsigned int akj1;
	unsigned int akj2;
	unsigned int aakj;	
	aakj2=(*(U8 *)(COMad+ptcom->return_length[ptcom->Current_Times-1]-1))&0xff;		//�ڷ������Current_Times++����ʱҪ--
	aakj1=(*(U8 *)(COMad+ptcom->return_length[ptcom->Current_Times-1]-2))&0xff;
	aakj=CalcCrc((U8 *)COMad,ptcom->return_length[ptcom->Current_Times-1]-2);
	akj1=aakj/0x100;
	akj2=aakj-akj1*0x100;
	if((akj1==aakj2)&&(akj2==aakj1))
		return 1;
	else
		return 0;
}


U16 CalcCrc(unsigned char *chData,unsigned short uNo)
{
	U16 crc=0xffff;
	U16 i,j;
	for(i=0;i<uNo;i++)
	{
	  crc^=chData[i];
	  for(j=0;j<8;j++)
	  {
	    if(crc&1)
		{
	     crc>>=1;
		 crc^=0xA001;
		}
	    else
		 crc>>=1;
	  }
	}
	return (crc);
}
