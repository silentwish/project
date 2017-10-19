#include "stdio.h"
#include "def.h"
#include "smm.h"

	
struct Com_struct_D *ptcom;


unsigned char KK[100];


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
		case 'Y':		
		case 'T':
		case 'C':						
			Read_Bool();   //���������Ƕ�λ����       
			break;
		case 'D':
		case 'R':				
		case 't':
		case 'c':	
			Read_Analog();  //���������Ƕ�ģ������ 
			break;			
		}
		break;
	case PLC_WRITE_DATA:				
		switch(ptcom->registerr)
		{	
		case 'Y':
		case 'T':
		case 'C':				
			Set_Reset();      //����������ǿ����λ�͸�λ
			break;
		case 'D':
		case 'R':
		case 't':
		case 'c':
			Write_Analog();	  //����������дģ������	
			break;			
		}
		break;	
	case PLC_WRITE_TIME:				//����������дʱ�䵽PLC
		switch(ptcom->registerr)
		{
		case 'D':
		case 'R':		
			Write_Time();		
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
		}
		break;
	case PLC_WRITE_RECIPE:				//����������д�䷽��PLC
		switch(*(U8 *)(PE+3))//�䷽�Ĵ�������
		{
		case 'D':	
		case 'R':	
			Write_Recipe();		
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
		}
		break;
	case 7:				//����������Ϊ������
		handshake();	//��������		
		break;					
	case PLC_CHECK_DATA:				//�������������ݴ���
		watchcom();
		break;				
	}	 
}


void handshake()		//���֣�ͨ�Ž���ǰǰ����
{
	*(U8 *)(AD1+0)=0x43;
	*(U8 *)(AD1+1)=0x52;
	*(U8 *)(AD1+2)=0x0d;
	*(U8 *)(AD1+3)=0x0a;
	ptcom->send_length[0]=4;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
	ptcom->send_times=1;					//���ʹ���
		
	ptcom->return_length[0]=4;				//�������ݳ���,����43 43 0D 0A 
	ptcom->return_start[0]=0;				//����������Ч��ʼ
	ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
	ptcom->Current_Times=0;					//��ǰ���ʹ���		
}

void Set_Reset()
{
	int b;
	int a1,a2,a3,a4,a5;
	int sendlength;
	int h_add,l_add;

	b=ptcom->address;			// ��ʼ��λ��ַ
	
	h_add=b/16;
	l_add=b-(b/16)*16;		
	
	a1=h_add/100;
	a2=h_add-a1*100;
	a2=a2/10;
	a3=h_add-a1*100-a2*10;
	a3=a3/1;
		
	a4=l_add/10;
	a5=l_add-a4*10;
	a5=a5/1;

	if (ptcom->writeValue==1)
	{
		*(U8 *)(AD1+0)=0x53;        	//S
		*(U8 *)(AD1+1)=0x54;       		//T
	}
	else if (ptcom->writeValue==0)
	{
		*(U8 *)(AD1+0)=0x52;        	//R
		*(U8 *)(AD1+1)=0x53;       		//S		
	}
	switch (ptcom->registerr)	//���ݼĴ������ͻ��ƫ�Ƶ�ַ
	{
	case 'Y':	//relay
		*(U8 *)(AD1+2)=0x20;
		*(U8 *)(AD1+3)=asicc(a1);		//�Ĵ�����ַ
		*(U8 *)(AD1+4)=asicc(a2);	
		*(U8 *)(AD1+5)=asicc(a3);
		*(U8 *)(AD1+6)=asicc(a4);
		*(U8 *)(AD1+7)=asicc(a5);	
		*(U8 *)(AD1+8)=0x0d;	
		sendlength=9;		
		break;
	case 'T':
		*(U8 *)(AD1+2)=0x20;
		*(U8 *)(AD1+3)=0x54;		
		*(U8 *)(AD1+4)=asicc(a1);		//�Ĵ�����ַ
		*(U8 *)(AD1+5)=asicc(a2);	
		*(U8 *)(AD1+6)=asicc(a3);
		*(U8 *)(AD1+7)=asicc(a4);
		*(U8 *)(AD1+8)=asicc(a5);	
		*(U8 *)(AD1+9)=0x0d;	 
		sendlength=10;	
		break;		
	case 'C':
		*(U8 *)(AD1+2)=0x20;
		*(U8 *)(AD1+3)=0x43;		
		*(U8 *)(AD1+4)=asicc(a1);		//�Ĵ�����ַ
		*(U8 *)(AD1+5)=asicc(a2);	
		*(U8 *)(AD1+6)=asicc(a3);
		*(U8 *)(AD1+7)=asicc(a4);
		*(U8 *)(AD1+8)=asicc(a5);	
		*(U8 *)(AD1+9)=0x0d;	
		sendlength=10;	
		break;				
	}	
	
	ptcom->send_length[0]=sendlength;		//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
	ptcom->send_times=1;					//���ʹ���
		
	ptcom->return_length[0]=4;				//�������ݳ���,����4F 4B 0D 0A 
	ptcom->return_start[0]=0;				//����������Ч��ʼ
	ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
	ptcom->Current_Times=0;					//��ǰ���ʹ���			
}

void ReadY()
{
	int b,t,i;
	int a1,a2,a3,a4,a5;
	int h_add,l_add;
	int ps;
	int len;
	
	b=ptcom->address;						// ��ʼ��ȡ��ַ
	len=ptcom->register_length;				// ��ʼ��ȡ����,kv700ϵ��һ��ֻ�����һ��λ,����������ĵ�λ��λ
								//

	*(U8 *)(AD1+0)=0x52;        //R
	*(U8 *)(AD1+1)=0x44;        //D
	*(U8 *)(AD1+2)=0x20;   	//SPACE 
				
	t=b;
	h_add=t/16;
	l_add=t-(t/16)*16;		
		
	a1=h_add/100;
	a2=h_add-a1*100;
	a2=a2/10;
	a3=h_add-a1*100-a2*10;
	a3=a3/1;
		
	a4=l_add/10;
	a5=l_add-a4*10;
	a5=a5/1;
		
	*(U8 *)(AD1+3)=asicc(a1);		//�Ĵ�����ַ
	*(U8 *)(AD1+4)=asicc(a2);	
	*(U8 *)(AD1+5)=asicc(a3);
	*(U8 *)(AD1+6)=asicc(a4);
	*(U8 *)(AD1+7)=asicc(a5);	
	*(U8 *)(AD1+8)=0x0d;   		//	������
	*(U8 *)(AD1+9)=0x0a;   		//	������
					
	ptcom->send_length[0]=10;		//���ͳ���
	ptcom->send_staradd[0]=0;			//�������ݴ洢��ַ
	ptcom->send_add[0]=b;				//�����������ַ������	
	ptcom->send_data_length[0]=1;	//�������һ�ζ���1��λ
					
	ptcom->return_length[0]=3;				//�������ݳ���
	ptcom->return_start[0]=0;				//����������Ч��ʼ
	ptcom->return_length_available[0]=1;	//������Ч���ݳ���								

	ptcom->send_times=1;					//���ʹ���
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	ptcom->Simens_Count=1;					//����������λ
}



/****************************
��Tʱ״̬����ǰֵ���趨ֵһ�𷵻�
****************************/
void ReadT()
{
	int b,t,i;
	int a1,a2,a3,a4,a5;
	int h_add,l_add;
	int ps;
	int len;
	
	b=ptcom->address;						// ��ʼ��ȡ��ַ
	len=ptcom->register_length;				// ��ʼ��ȡ����,kv700ϵ��һ��ֻ�����һ��λ,����������ĵ�λ��λ
								//

	*(U8 *)(AD1+0)=0x52;        //R
	*(U8 *)(AD1+1)=0x44;        //D
	*(U8 *)(AD1+2)=0x20;   		//SPACE 
	*(U8 *)(AD1+3)=0x54;   		//T			
	t=b;
	a1=t/100;
	a2=t-a1*100;
	a2=a2/10;
	a3=t-a1*100-a2*10;
	a3=a3/1;
		
	*(U8 *)(AD1+4)=asicc(a1);		//�Ĵ�����ַ
	*(U8 *)(AD1+5)=asicc(a2);	
	*(U8 *)(AD1+6)=asicc(a3);
	*(U8 *)(AD1+7)=0x0d;   		//	������
	*(U8 *)(AD1+8)=0x0a;   		//	������
					
	ptcom->send_length[0]=9;		//���ͳ���
	ptcom->send_staradd[0]=0;			//�������ݴ洢��ַ
	ptcom->send_add[0]=b;				//�����������ַ������	
	ptcom->send_data_length[0]=1;	//�������һ�ζ���1��λ
					
	ptcom->return_length[0]=15;				//�������ݳ���,״̬ �趨ֵ ��ǰֵ һ�𷵻�
	ptcom->return_start[0]=0;				//����������Ч��ʼ
	ptcom->return_length_available[0]=1;	//������Ч���ݳ���								

	ptcom->send_times=1;					//���ʹ���
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	ptcom->Simens_Count=4;					//����������T��״̬
}

void ReadC()
{
	int b,t,i;
	int a1,a2,a3,a4,a5;
	int h_add,l_add;
	int ps;
	int len;
	
	b=ptcom->address;						// ��ʼ��ȡ��ַ
	len=ptcom->register_length;				// ��ʼ��ȡ����,kv700ϵ��һ��ֻ�����һ��λ,����������ĵ�λ��λ
								//

	*(U8 *)(AD1+0)=0x52;        //R
	*(U8 *)(AD1+1)=0x44;        //D
	*(U8 *)(AD1+2)=0x20;   		//SPACE 
	*(U8 *)(AD1+3)=0x43;   		//C		
	t=b;
	a1=t/100;
	a2=t-a1*100;
	a2=a2/10;
	a3=t-a1*100-a2*10;
	a3=a3/1;
		
	*(U8 *)(AD1+4)=asicc(a1);		//�Ĵ�����ַ
	*(U8 *)(AD1+5)=asicc(a2);	
	*(U8 *)(AD1+6)=asicc(a3);
	*(U8 *)(AD1+7)=0x0d;   		//	������
	*(U8 *)(AD1+8)=0x0a;   		//	������
					
	ptcom->send_length[0]=9;		//���ͳ���
	ptcom->send_staradd[0]=0;			//�������ݴ洢��ַ
	ptcom->send_add[0]=b;				//�����������ַ������	
	ptcom->send_data_length[0]=1;	//�������һ�ζ���1��λ
					
	ptcom->return_length[0]=15;				//�������ݳ���,״̬ �趨ֵ ��ǰֵ һ�𷵻�
	ptcom->return_start[0]=0;				//����������Ч��ʼ
	ptcom->return_length_available[0]=1;	//������Ч���ݳ���								

	ptcom->send_times=1;					//���ʹ���
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	ptcom->Simens_Count=4;					//����������T��״̬
}
/***********************
keyenceλ��ַ��ʽ
xxxyy
����xxx�Ƕε�ַ0-599
yy�Ƕ��ڵ�ַ0-15
***********************/
void Read_Bool()				//��ȡ����������Ϣ
{
	switch (ptcom->registerr)	//���ݼĴ�������
	{	
	case 'Y':
		ReadY();
		break;					
	case 'T':
		ReadT();
		break;
	case 'C':
		ReadC();
		break;					
	}	

}

void ReadD()
{
	int b,t,i;
	int a1,a2,a3,a4,a5;
	int ps;
	int len,LastTimeWord,SendTimes,datalength;
	
	b=ptcom->address;						// ��ʼ��ȡ��ַ
	datalength=ptcom->register_length;		// ��ȡ����,��
	
	
	ps=11;
	for (i=0;i<datalength;i++)				//һ��ֻ�ܶ�һ����
	{
		*(U8 *)(AD1+0+ps*i)=0x52;        //R
		*(U8 *)(AD1+1+ps*i)=0x44;        //D
		*(U8 *)(AD1+2+ps*i)=0x20;		//S

		if (ptcom->registerr=='D')
			*(U8 *)(AD1+3+ps*i)=0x44;		//DM
		else if (ptcom->registerr=='R')
			*(U8 *)(AD1+3+ps*i)=0x54;		//TM			
		*(U8 *)(AD1+4+ps*i)=0x4d;   	//M
				
		t=b+i;
		a1=t/1000;
		a2=t-a1*1000;
		a2=a2/100;
		a3=t-a1*1000-a2*100;
		a3=a3/10;
		a4=t-a1*1000-a2*100-a3*10;
		a4=a4/1;
		
		*(U8 *)(AD1+5+ps*i)=asicc(a1);		//�Ĵ�����ַ
		*(U8 *)(AD1+6+ps*i)=asicc(a2);	
		*(U8 *)(AD1+7+ps*i)=asicc(a3);
		*(U8 *)(AD1+8+ps*i)=asicc(a4);
		*(U8 *)(AD1+9+ps*i)=0x0d;   	//	
		*(U8 *)(AD1+10+ps*i)=0x0a;   	//			
			
		ptcom->send_length[i]=11;		//���ͳ���
		ptcom->send_staradd[i]=i*ps;	//�������ݴ洢��ַ
		ptcom->send_add[i]=ptcom->address;			//�����������ַ������	
		ptcom->send_data_length[i]=datalength;	
					
		ptcom->return_length[i]=7;		//�������ݳ���5��һ���֣��ټӽ�����0D 0A
		ptcom->return_start[i]=0;				//����������Ч��ʼ
		ptcom->return_length_available[i]=5;	//������Ч���ݳ���								
	}	
	for (i=0;i<datalength;i++)				//KVϵ��ȱ�ݣ��������෢�ʹ�����20�����䷽���ȴ���20����Ḳ�Ǻ��������
	{
		ptcom->return_length[i]=7;
		ptcom->send_add[i]=ptcom->address;			//�����������ַ������	
		ptcom->send_data_length[i]=datalength;	
	}	
	
	ptcom->send_times=datalength;					//���ʹ���
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	ptcom->Simens_Count=2;					//�������������ݼĴ���
}

void Read_Timer()	//��TC�ĵ�ǰֵ
{
	int b,t,i;
	int a1,a2,a3,a4,a5;
	int ps;
	int len,LastTimeWord,SendTimes,datalength;
	
	b=ptcom->address;						// ��ʼ��ȡ��ַ
	datalength=ptcom->register_length;		// ��ȡ����,��
	
	
	ps=9;
	for (i=0;i<datalength;i++)				//һ��ֻ�ܶ�һ����
	{
		*(U8 *)(AD1+0+ps*i)=0x52;        //R
		*(U8 *)(AD1+1+ps*i)=0x44;        //D
		*(U8 *)(AD1+2+ps*i)=0x20;		//S

		if (ptcom->registerr=='t')
			*(U8 *)(AD1+3+ps*i)=0x54;		//T
		else if (ptcom->registerr=='c')
			*(U8 *)(AD1+3+ps*i)=0x43;		//C		
				
		t=b+i;
		a1=t/100;
		a2=t-a1*100;
		a2=a2/10;
		a3=t-a1*100-a2*10;
		a3=a3/1;
		
		*(U8 *)(AD1+4+ps*i)=asicc(a1);		//�Ĵ�����ַ
		*(U8 *)(AD1+5+ps*i)=asicc(a2);	
		*(U8 *)(AD1+6+ps*i)=asicc(a3);
		*(U8 *)(AD1+7+ps*i)=0x0d;   	//	
		*(U8 *)(AD1+8+ps*i)=0x0a;   	//			
			
		ptcom->send_length[i]=9;		//���ͳ���
		ptcom->send_staradd[i]=i*ps;	//�������ݴ洢��ַ
		ptcom->send_add[i]=t;			//�����������ַ������	
		ptcom->send_data_length[i]=1;	
					
		ptcom->return_length[i]=15;		//�������ݳ���,״̬ ��ǰֵ �趨ֵһ�𷵻�
		ptcom->return_start[i]=0;				//����������Ч��ʼ
		ptcom->return_length_available[i]=5;	//������Ч���ݳ���								
	}	
	ptcom->send_times=datalength;					//���ʹ���
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	ptcom->Simens_Count=5;					//�������������ݼĴ���
}


void Read_Analog()				//��ģ����
{
	switch (ptcom->registerr)	//���ݼĴ�������
	{	
	case 'D':
	case 'R':		
		ReadD();
		break;
	case 't':
	case 'c':	
		Read_Timer();
		break;				
	}
}


void Read_Recipe()								//��ȡ�䷽
{
	ReadD();
}

void writeD()
{
	int b,t,i,a;
	int a1,a2,a3,a4,a5;
	int b1,b2;
	int datalength;
	int ps;

	a=0;
	b=ptcom->address;						// ��ʼ��ȡ��ַ
	datalength=ptcom->register_length;				// ��ʼд�볤��
	
	ps=17;
	for (i=0;i<datalength;i++)				//һ��ֻ��дһ����
	{	
		*(U8 *)(AD1+0+ps*i)=0x57;        //W
		*(U8 *)(AD1+1+ps*i)=0x52;        //R
		*(U8 *)(AD1+2+ps*i)=0x20;   	//SPACE 

		if (ptcom->registerr=='D')
			*(U8 *)(AD1+3+ps*i)=0x44;		//D
		else if (ptcom->registerr=='R')
			*(U8 *)(AD1+3+ps*i)=0x54;		//T				
		*(U8 *)(AD1+4+ps*i)=0x4d;   	//M
					
		t=b+i;
		a1=t/1000;
		a2=t-a1*1000;
		a2=a2/100;
		a3=t-a1*1000-a2*100;
		a3=a3/10;
		a4=t-a1*1000-a2*100-a3*10;
		a4=a4/1;
			
		*(U8 *)(AD1+5+ps*i)=asicc(a1);		//�Ĵ�����ַ
		*(U8 *)(AD1+6+ps*i)=asicc(a2);	
		*(U8 *)(AD1+7+ps*i)=asicc(a3);
		*(U8 *)(AD1+8+ps*i)=asicc(a4);
		*(U8 *)(AD1+9+ps*i)=0x20;        //SPACE	
			
		b1=ptcom->U8_Data[i*2];       	//����
		b2=ptcom->U8_Data[i*2+1];       	//����
		a=(b2<<8)+b1;
		a1=a/10000;
		a2=(a-a1*10000)/1000;
	    a3=(a-a1*10000-a2*1000)/100;
		a4=(a-a1*10000-a2*1000-a3*100)/10;
		a5=(a-a1*10000-a2*1000-a3*100-a4*10)/1;
		*(U8 *)(AD1+10+ps*i)=asicc(a1);
		*(U8 *)(AD1+11+ps*i)=asicc(a2);			
		*(U8 *)(AD1+12+ps*i)=asicc(a3);
		*(U8 *)(AD1+13+ps*i)=asicc(a4);	
		*(U8 *)(AD1+14+ps*i)=asicc(a5);								    

		*(U8 *)(AD1+15+ps*i)=0x0d;   	//������
		*(U8 *)(AD1+16+ps*i)=0x0a;   	//������		
				
		ptcom->send_length[i]=17;		//���ͳ���
		ptcom->send_staradd[i]=ps*i;				//�������ݴ洢��ַ
		ptcom->send_add[i]=t;					//�����������ַ������	
		ptcom->send_data_length[i]=1;	//
						
		ptcom->return_length[i]=4;				//�������ݳ���
		ptcom->return_start[i]=0;				//����������Ч��ʼ
		ptcom->return_length_available[i]=0;	//������Ч���ݳ���	
	}							
	
	ptcom->send_times=datalength;					//���ʹ���
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	ptcom->Simens_Count=3;					//����������д���ݼĴ���	
}

void Write_Timer()
{
	int b,t,i,a;
	int a1,a2,a3,a4,a5;
	int b1,b2;
	int datalength;
	int ps;

	a=0;
	b=ptcom->address;						// ��ʼ��ȡ��ַ
	datalength=ptcom->register_length;				// ��ʼд�볤��
	
	ps=15;
	for (i=0;i<datalength;i++)				//һ��ֻ��дһ����
	{	
		*(U8 *)(AD1+0+ps*i)=0x57;        //W
		*(U8 *)(AD1+1+ps*i)=0x52;        //R
		*(U8 *)(AD1+2+ps*i)=0x20;   	//SPACE 

		if (ptcom->registerr=='t')
			*(U8 *)(AD1+3+ps*i)=0x54;		//T
		else if (ptcom->registerr=='c')
			*(U8 *)(AD1+3+ps*i)=0x43;		//C			
					
		t=b+i;
		a1=t/100;
		a2=t-a1*100;
		a2=a2/10;
		a3=t-a1*100-a2*10;
		a3=a3/1;
	
		*(U8 *)(AD1+4+ps*i)=asicc(a1);		//�Ĵ�����ַ
		*(U8 *)(AD1+5+ps*i)=asicc(a2);	
		*(U8 *)(AD1+6+ps*i)=asicc(a3);
		*(U8 *)(AD1+7+ps*i)=0x20;        //SPACE	
			
		b1=ptcom->U8_Data[i*2];       	//����
		b2=ptcom->U8_Data[i*2+1];       	//����
		a=(b2<<8)+b1;
		a1=a/10000;
		a2=(a-a1*10000)/1000;
	    a3=(a-a1*10000-a2*1000)/100;
		a4=(a-a1*10000-a2*1000-a3*100)/10;
		a5=(a-a1*10000-a2*1000-a3*100-a4*10)/1;
		*(U8 *)(AD1+8+ps*i)=asicc(a1);
		*(U8 *)(AD1+9+ps*i)=asicc(a2);			
		*(U8 *)(AD1+10+ps*i)=asicc(a3);
		*(U8 *)(AD1+11+ps*i)=asicc(a4);	
		*(U8 *)(AD1+12+ps*i)=asicc(a5);								    

		*(U8 *)(AD1+13+ps*i)=0x0d;   	//������
		*(U8 *)(AD1+14+ps*i)=0x0a;   	//������		
				
		ptcom->send_length[i]=15;		//���ͳ���
		ptcom->send_staradd[i]=ps*i;				//�������ݴ洢��ַ
		ptcom->send_add[i]=t;					//�����������ַ������	
		ptcom->send_data_length[i]=1;	//
						
		ptcom->return_length[i]=4;				//�������ݳ���
		ptcom->return_start[i]=0;				//����������Ч��ʼ
		ptcom->return_length_available[i]=0;	//������Ч���ݳ���	
	}							
	
	ptcom->send_times=datalength;					//���ʹ���
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	ptcom->Simens_Count=3;					//����������д���ݼĴ���	
}

void Write_Analog()								    //дģ����
{
	switch (ptcom->registerr)	//���ݼĴ�������
	{	
	case 'D':
	case 'R':
	case 'N':		
		writeD();
		break;
	case 't':
	case 'c':	
		Write_Timer();
		break;				
	}
}


void Write_Time()                                //дʱ�䵽PLC
{
	Write_Analog();									
}


void Read_Time()									//��PLC��ȡʱ��
{
	Read_Analog();
}


void Write_Recipe()								//д�䷽��PLC
{
	int t,i,j,a;
	int a1,a2,a3,a4,a5;
	int ps,staradd;
	int b1,b2;
	int len,LastTimeWord,SendTimes,datalength;
	
	
	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));//���ݳ���
	staradd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//���ݿ�ʼ��ַ
	
	SendTimes=datalength;
	ps=17;                                     
	for (i=0;i<SendTimes;i++)
	{	
		*(U8 *)(AD1+0+ps*i)=0x57;        //R
		*(U8 *)(AD1+1+ps*i)=0x52;        //D
		*(U8 *)(AD1+2+ps*i)=0x20;   	//SPACE 

		*(U8 *)(AD1+3+ps*i)=0x44;		//D			
		*(U8 *)(AD1+4+ps*i)=0x4d;   	//M
					
		t=staradd+i;
		a1=t/1000;
		a2=t-a1*1000;
		a2=a2/100;
		a3=t-a1*1000-a2*100;
		a3=a3/10;
		a4=t-a1*1000-a2*100-a3*10;
		a4=a4/1;
			
		*(U8 *)(AD1+5+ps*i)=asicc(a1);		//�Ĵ�����ַ
		*(U8 *)(AD1+6+ps*i)=asicc(a2);	
		*(U8 *)(AD1+7+ps*i)=asicc(a3);
		*(U8 *)(AD1+8+ps*i)=asicc(a4);
		*(U8 *)(AD1+9+ps*i)=0x20;        //SPACE	
		b1=*(U8 *)(PE+9+2*i+j*2);       	//����
		b2=*(U8 *)(PE+9+2*i+j*2+1);       	//����
		a=(b2<<8)+b1;
		a1=a/10000;
		a2=(a-a1*10000)/1000;
	   	a3=(a-a1*10000-a2*1000)/100;
		a4=(a-a1*10000-a2*1000-a3*100)/10;
		a5=(a-a1*10000-a2*1000-a3*100-a4*10)/1;
		*(U8 *)(AD1+10+ps*i)=asicc(a1);
		*(U8 *)(AD1+11+ps*i)=asicc(a2);			
		*(U8 *)(AD1+12+ps*i)=asicc(a3);
		*(U8 *)(AD1+13+ps*i)=asicc(a4);	
		*(U8 *)(AD1+14+ps*i)=asicc(a5);							    
		*(U8 *)(AD1+15+ps*i)=0x0d;  
		*(U8 *)(AD1+16+ps*i)=0x0a;  		
		ptcom->send_length[i]=17;		//���ͳ���
		ptcom->send_staradd[i]=ps*i;	//�������ݴ洢��ַ
		ptcom->send_add[i]=t;			//�����������ַ������	
		ptcom->send_data_length[i]=1;	//
		ptcom->return_length[i]=4;		//�������ݳ���
		ptcom->return_start[i]=0;		//����������Ч��ʼ
		ptcom->return_length_available[i]=0;//������Ч���ݳ���								
	}
	
	for (i=0;i<SendTimes;i++)				//KVϵ��ȱ�ݣ��������෢�ʹ�����20�����䷽���ȴ���20����Ḳ�Ǻ��������
	{
		ptcom->return_length[i]=4;
	}
	
	ptcom->send_times=SendTimes;			//���ʹ���
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	ptcom->Simens_Count=3;					//����������д���ݼĴ���
}


void compxy(void)				              //����ɱ�׼�洢��ʽ,��������
{
	int i;
	int b=0;
	char temp[64];
	char kk[4];
	if (ptcom->Simens_Count==1)	//������λ,ÿ�ζ��Ƕ�һ��λ
	{
		kk[0]=*(U8 *)(COMad+0);
		kk[0]=bsicc(kk[0]);
		b=ptcom->address%8;			//���ַ��������Ϊ�˺�������һ��
		kk[0]=kk[0]<<b;
		*(U8 *)(COMad+0)=kk[0];		//���´�
	}
	else if (ptcom->Simens_Count==2)	//���������ݼĴ���
	{
		temp[0]=*(U8 *)(COMad+0);
		temp[1]=*(U8 *)(COMad+1);		
		temp[2]=*(U8 *)(COMad+2);
		temp[3]=*(U8 *)(COMad+3);				
		temp[4]=*(U8 *)(COMad+4);			
		
		temp[0]=bsicc(temp[0]);
		temp[1]=bsicc(temp[1]);				
		temp[2]=bsicc(temp[2]);
		temp[3]=bsicc(temp[3]);
		temp[4]=bsicc(temp[4]);					
		b=temp[0]*10000+temp[1]*1000+temp[2]*100+temp[3]*10+temp[4];//ͨ����ӵĵ���ʵ����
		kk[0]=(b>>8)&0xff;
		kk[1]=b&0xff;
		*(U8 *)(COMad+1000+(ptcom->Current_Times-1)*2)=kk[0];					 //���´�
		*(U8 *)(COMad+1001+(ptcom->Current_Times-1)*2)=kk[1];					 //���´�	
		
		if (ptcom->Current_Times==ptcom->send_times)		//�Ѿ�����,���԰����ݸ��ؼ��� 
		{
			for (i=0;i<ptcom->send_times;i++)				//�����ξͻ�ü����ֵ�����
			{
				*(U8 *)(COMad+i*2+0)=*(U8 *)(COMad+1000+i*2+0);
				*(U8 *)(COMad+i*2+1)=*(U8 *)(COMad+1000+i*2+1);
			}
		}
		else
		{
			ptcom->IfResultCorrect=0;						//û�յ����һ�Σ��������ݸ��ؼ�
		}		
	}
	else if (ptcom->Simens_Count==4)	//������TC��״̬
	{
		kk[0]=*(U8 *)(COMad+0);
		kk[0]=bsicc(kk[0]);
		b=ptcom->address%8;			//���ַ��������Ϊ�˺�������һ��
		kk[0]=kk[0]<<b;
		*(U8 *)(COMad+0)=kk[0];		//���´�
	}
	else if (ptcom->Simens_Count==5)	//������TC�ĵ�ǰֵ
	{
		temp[0]=*(U8 *)(COMad+2);		//�ӵڶ������ص��ǵ�ǰֵ
		temp[1]=*(U8 *)(COMad+3);		
		temp[2]=*(U8 *)(COMad+4);
		temp[3]=*(U8 *)(COMad+5);				
		temp[4]=*(U8 *)(COMad+6);			
		
		temp[0]=bsicc(temp[0]);
		temp[1]=bsicc(temp[1]);				
		temp[2]=bsicc(temp[2]);
		temp[3]=bsicc(temp[3]);
		temp[4]=bsicc(temp[4]);					
		b=temp[0]*10000+temp[1]*1000+temp[2]*100+temp[3]*10+temp[4];//ͨ����ӵĵ���ʵ����
		kk[0]=(b>>8)&0xff;
		kk[1]=b&0xff;
		*(U8 *)(COMad+1000+(ptcom->Current_Times-1)*2)=kk[0];					 //���´�
		*(U8 *)(COMad+1001+(ptcom->Current_Times-1)*2)=kk[1];					 //���´�	

		if (ptcom->Current_Times==ptcom->send_times)		//�Ѿ�����,���԰����ݸ��ؼ��� 
		{
			for (i=0;i<ptcom->send_times;i++)				//�����ξͻ�ü����ֵ�����
			{
				*(U8 *)(COMad+i*2+0)=*(U8 *)(COMad+1000+i*2+0);
				*(U8 *)(COMad+i*2+1)=*(U8 *)(COMad+1000+i*2+1);
			}
		}
		else
		{
			ptcom->IfResultCorrect=0;						//û�յ����һ�Σ��������ݸ��ؼ�
		}
	}		
}


void watchcom(void)		//�������У��
{
	unsigned int aakj=0;
	aakj=remark();
	if(aakj==1)	//��ȡλ
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
	return 1;				//keyence��У��	
}


unsigned short CalXor(unsigned char *chData,unsigned short uNo)		//���У��
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
