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
		case 'Y':
		case 'M':
		case 'H':
		case 'T':
		case 'C':						
			Read_Bool();
			break;
		case 'D':
		case 'R':		
		case 't':
		case 'c':	
			Read_Analog();
			break;			
		}
		break;
	case PLC_WRITE_DATA:				//����������д����
		switch(ptcom->registerr)
		{
		case 'M':
		case 'H':
		case 'Y':
		case 'T':
		case 'C':				
			Set_Reset();
			break;
		case 'D':
		case 'R':	
		case 't':
		case 'c':
			Write_Analog();		
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
	case PLC_CHECK_DATA:				//�������������ݴ���
		watchcom();
		break;						
	}	 
}

void Set_Reset()
{
	U16 aakj;
	int b,b1,b2;
	int a1,a2,a3,a4;
	int add,plc_add;	

	b=ptcom->address;			// ��λ��ַ
	plc_add=ptcom->plc_address;	//plcվ��ַ
	switch (ptcom->registerr)	//���ݼĴ������ͻ��ƫ�Ƶ�ַ
	{
	case 'Y':
		add=0x59;
		break;
	case 'M':
		add=0x4d;
		break;
	case 'H':
		add=0x53;
		break;	
	case 'T':
		add=0x54;
		break;
	case 'C':
		add=0x43;
		break;					
	}

		b1=(plc_add>>4)&0xf;
		b2=(plc_add)&0xf;	
		a1=b/1000;
		a2=(b-a1*1000)/100;
		a3=(b-a1*1000-a2*100)/10;	
		a4=b-a1*1000-a2*100-a3*10;

		*(U8 *)(AD1+0)=0x02;
		*(U8 *)(AD1+1)=asicc(b1);	//plc add
		*(U8 *)(AD1+2)=asicc(b2);
		*(U8 *)(AD1+3)=0x34;		//command
		*(U8 *)(AD1+4)=0x32;	
		if (ptcom->writeValue==1)	//��λ
		{
			*(U8 *)(AD1+5)=0x33;
		}
		if (ptcom->writeValue==0)	//��λ
		{
			*(U8 *)(AD1+5)=0x34;
		}	
		*(U8 *)(AD1+6)=add;			//Y.M.T.C
		*(U8 *)(AD1+7)=asicc(a1);
		*(U8 *)(AD1+8)=asicc(a2);
		*(U8 *)(AD1+9)=asicc(a3);
		*(U8 *)(AD1+10)=asicc(a4);
		aakj=CalcHe((U8 *)AD1,11);
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+11)=asicc(a1);
		*(U8 *)(AD1+12)=asicc(a2);
		*(U8 *)(AD1+13)=0x03;	
		
		ptcom->send_length[0]=14;				//���ͳ���
		ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
		ptcom->send_times=1;					//���ʹ���
			
		ptcom->return_length[0]=9;				//�������ݳ���
		ptcom->return_start[0]=0;				//����������Ч��ʼ
		ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
		ptcom->Current_Times=0;					//��ǰ���ʹ���			
		ptcom->Simens_Count=0;
}


void Read_Bool()				//��ȡ����������Ϣ
{
	U16 aakj;
	int b,b1,b2;
	int a1,a2,a3,a4;
	int add,plc_add,len;	

	b=ptcom->address;			// ��λ��ַ
	plc_add=ptcom->plc_address;	//plcվ��ַ
	len=ptcom->register_length;	//���ͳ��ȡ��ֽ�
	
	switch (ptcom->registerr)	//���ݼĴ������ͻ��ƫ�Ƶ�ַ
	{
	case 'X':
		add=0x58;
		break;	
	case 'Y':
		add=0x59;
		break;
	case 'M':
		add=0x4d;
		break;	
	case 'H':
		add=0x53;
		break;	
	case 'T':
		add=0x54;
		break;
	case 'C':
		add=0x43;
		break;		
			
	}
	
	b1=(plc_add>>4)&0xf;
	b2=(plc_add)&0xf;	
	a1=b/1000;
	a2=(b-a1*1000)/100;
	a3=(b-a1*1000-a2*100)/10;	
	a4=b-a1*1000-a2*100-a3*10;
	len=(len/2)+len%2;			//���ͳ��ȣ���Ϊ��λ
	//ptcom->plc_address = ((len/2)+len%2 + 1)*2;

	*(U8 *)(AD1+0)=0x02;
	*(U8 *)(AD1+1)=asicc(b1);	//plc add
	*(U8 *)(AD1+2)=asicc(b2);
	*(U8 *)(AD1+3)=0x34;		//command
	*(U8 *)(AD1+4)=0x36;	
	*(U8 *)(AD1+5)=asicc((len>>4)&0xf);	//���ͳ���
	*(U8 *)(AD1+6)=asicc(len&0xf);		//���ͳ���

	*(U8 *)(AD1+7)=0x57;		//W
	*(U8 *)(AD1+8)=add;			//X,Y.M.T.C
	*(U8 *)(AD1+9)=asicc(a1);
	*(U8 *)(AD1+10)=asicc(a2);
	*(U8 *)(AD1+11)=asicc(a3);
	*(U8 *)(AD1+12)=asicc(a4);
	aakj=CalcHe((U8 *)AD1,13);
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+13)=asicc(a1);
	*(U8 *)(AD1+14)=asicc(a2);
	*(U8 *)(AD1+15)=0x03;	
	
	ptcom->send_length[0]=16;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
	ptcom->send_times=1;					//���ʹ���
		
	ptcom->return_length[0]=9+len*4;		//�������ݳ���
	ptcom->return_start[0]=6;				//����������Ч��ʼ
	ptcom->return_length_available[0]=len*4;//������Ч���ݳ���	
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	ptcom->Simens_Count=1;					//��ʾ������λ
}



void Read_Analog()				//��ģ����
{
	U16 aakj;
	int b,b1,b2;
	int a1,a2,a3,a4,a5;
	int add,plc_add,len;	

	b=ptcom->address;			// ��λ��ַ
	plc_add=ptcom->plc_address;	//plcվ��ַ
	len=ptcom->register_length;	//���ͳ��ȡ���
	
	switch (ptcom->registerr)	//���ݼĴ������ͻ��ƫ�Ƶ�ַ
	{
	case 'D':
		add=0x44;
		break;	
	case 'R':
		add=0x52;
		break;	
	case 't':
		add='T';
		break;	
	case 'c':
		add='C';
		break;			
			
	}
	
	b1=(plc_add>>4)&0xf;
	b2=(plc_add)&0xf;	
	
	a1=b/10000;
	a2=(b-a1*10000)/1000;
	a3=(b-a1*10000-a2*1000)/100;	
	a4=(b-a1*10000-a2*1000-a3*100)/10;
	a5=(b-a1*10000-a2*1000-a3*100-a4*10)/1;	

	*(U8 *)(AD1+0)=0x02;
	*(U8 *)(AD1+1)=asicc(b1);	//plc add
	*(U8 *)(AD1+2)=asicc(b2);
	*(U8 *)(AD1+3)=0x34;		//command
	*(U8 *)(AD1+4)=0x36;	
	*(U8 *)(AD1+5)=asicc((len>>4)&0xf);	//���ͳ���
	*(U8 *)(AD1+6)=asicc(len&0xf);		//���ͳ���

	if (ptcom->registerr=='D' || ptcom->registerr=='R')
	{
		*(U8 *)(AD1+7)=add;			//D,R
		*(U8 *)(AD1+8)=asicc(a1);
		*(U8 *)(AD1+9)=asicc(a2);
		*(U8 *)(AD1+10)=asicc(a3);
		*(U8 *)(AD1+11)=asicc(a4);
		*(U8 *)(AD1+12)=asicc(a5);	
	}	
	if (ptcom->registerr=='t' || ptcom->registerr=='c')
	{
		*(U8 *)(AD1+7)='R';			//t c
		*(U8 *)(AD1+8)=add;
		*(U8 *)(AD1+9)=asicc(a2);
		*(U8 *)(AD1+10)=asicc(a3);
		*(U8 *)(AD1+11)=asicc(a4);
		*(U8 *)(AD1+12)=asicc(a5);	
	}	
	
	aakj=CalcHe((U8 *)AD1,13);
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+13)=asicc(a1);
	*(U8 *)(AD1+14)=asicc(a2);
	*(U8 *)(AD1+15)=0x03;	
	
	ptcom->send_length[0]=16;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
	ptcom->send_times=1;					//���ʹ���
		
	ptcom->return_length[0]=9+len*4;		//�������ݳ���
	ptcom->return_start[0]=6;				//����������Ч��ʼ
	ptcom->return_length_available[0]=len*4;//������Ч���ݳ���	
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	ptcom->Simens_Count=2;
}


void Read_Recipe()								//��ȡ�䷽
{
	U16 aakj;
	int add;
	int a1,a2,a3,a4,a5;
	int i,t,b1,b2;
	int datalength;
	int p_start;
	int ps,plc_add;
	int SendTimes;
	int LastTimeWord;							//���һ�η��ͳ���
	int currentlength;
	
	switch (ptcom->registerr)	//���ݼĴ������ͻ��ƫ�Ƶ�ַ
	{
	case 'D':
		add=0x44;
		break;	
	case 'R':
		add=0x52;
		break;		
	}
			
	datalength=ptcom->register_length;		//�����ܳ���
	p_start=ptcom->address;					//��ʼ��ַ
	plc_add=ptcom->plc_address;				//plcվ��ַ
	b1=(plc_add>>4)&0xf;
	b2=(plc_add)&0xf;	
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
	
	ps=16;
	for (i=0;i<SendTimes;i++)
	{
		if (i!=(SendTimes-1))	//�������һ��ʱ
		{
			currentlength=32;
		}
		if (i==(SendTimes-1))	//���һ��ʱ
		{
			currentlength=LastTimeWord;
		}	

		t=p_start+i*32;
		a1=t/10000;
		a2=(t-a1*10000)/1000;
		a3=(t-a1*10000-a2*1000)/100;	
		a4=(t-a1*10000-a2*1000-a3*100)/10;
		a5=(t-a1*10000-a2*1000-a3*100-a4*10)/1;	
			
		*(U8 *)(AD1+0+ps*i)=0x02;
		*(U8 *)(AD1+1+ps*i)=asicc(b1);	//plc add
		*(U8 *)(AD1+2+ps*i)=asicc(b2);
		*(U8 *)(AD1+3+ps*i)=0x34;		//command
		*(U8 *)(AD1+4+ps*i)=0x36;	
		*(U8 *)(AD1+5+ps*i)=asicc((currentlength>>4)&0xf);	//���ͳ���
		*(U8 *)(AD1+6+ps*i)=asicc(currentlength&0xf);		//���ͳ���

		*(U8 *)(AD1+7+ps*i)=add;			//D,R
		*(U8 *)(AD1+8+ps*i)=asicc(a1);
		*(U8 *)(AD1+9+ps*i)=asicc(a2);
		*(U8 *)(AD1+10+ps*i)=asicc(a3);
		*(U8 *)(AD1+11+ps*i)=asicc(a4);
		*(U8 *)(AD1+12+ps*i)=asicc(a5);	
		
		aakj=CalcHe((U8 *)AD1+ps*i,13);
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13+ps*i)=asicc(a1);
		*(U8 *)(AD1+14+ps*i)=asicc(a2);
		*(U8 *)(AD1+15+ps*i)=0x03;	
		
		ptcom->send_length[i]=16;				//���ͳ���
		ptcom->send_staradd[i]=i*16;			//�������ݴ洢��ַ	
		ptcom->send_add[i]=t;					//�����������ַ������	
		ptcom->send_data_length[i]=currentlength;	//�������һ�ζ���32��D
				
		ptcom->return_length[i]=9+currentlength*4;				//�������ݳ��ȣ���4���̶���02��03��У��
		ptcom->return_start[i]=6;				//����������Ч��ʼ
		ptcom->return_length_available[i]=currentlength*4;	//������Ч���ݳ���		
	}
	ptcom->send_times=SendTimes;					//���ʹ���
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	ptcom->Simens_Count=2;
}

void Write_Analog()								//дģ����
{
	U16 aakj;
	int b,b1,b2,i,k;
	int a1,a2,a3,a4,a5;
	int add,plc_add,len;	

	b=ptcom->address;			// ��λ��ַ
	plc_add=ptcom->plc_address;	//plcվ��ַ
	len=ptcom->register_length;	//���ͳ��ȡ���
	
	switch (ptcom->registerr)	//���ݼĴ������ͻ��ƫ�Ƶ�ַ
	{
	case 'D':
		add=0x44;
		break;	
	case 'R':
		add=0x52;
		break;
	case 't':
		add='T';
		break;	
	case 'c':
		add='C';
		break;					
	}
	
	b1=(plc_add>>4)&0xf;
	b2=(plc_add)&0xf;	
	
	a1=b/10000;
	a2=(b-a1*10000)/1000;
	a3=(b-a1*10000-a2*1000)/100;	
	a4=(b-a1*10000-a2*1000-a3*100)/10;
	a5=(b-a1*10000-a2*1000-a3*100-a4*10)/1;	

	*(U8 *)(AD1+0)=0x02;
	*(U8 *)(AD1+1)=asicc(b1);	//plc add
	*(U8 *)(AD1+2)=asicc(b2);
	*(U8 *)(AD1+3)=0x34;		//command
	*(U8 *)(AD1+4)=0x37;	
	*(U8 *)(AD1+5)=asicc((len>>4)&0xf);	//���ͳ���
	*(U8 *)(AD1+6)=asicc(len&0xf);		//���ͳ���
	
	if (ptcom->registerr=='D' || ptcom->registerr=='R')
	{
		*(U8 *)(AD1+7)=add;			//D,R
		*(U8 *)(AD1+8)=asicc(a1);
		*(U8 *)(AD1+9)=asicc(a2);
		*(U8 *)(AD1+10)=asicc(a3);
		*(U8 *)(AD1+11)=asicc(a4);
		*(U8 *)(AD1+12)=asicc(a5);	
	}	
	if (ptcom->registerr=='t' || ptcom->registerr=='c')
	{
		*(U8 *)(AD1+7)='R';			//t c
		*(U8 *)(AD1+8)=add;
		*(U8 *)(AD1+9)=asicc(a2);
		*(U8 *)(AD1+10)=asicc(a3);
		*(U8 *)(AD1+11)=asicc(a4);
		*(U8 *)(AD1+12)=asicc(a5);	
	}
		
	k=0;
	for (i=0;i<len;i++)
	{
		a1=((ptcom->U8_Data[i*2+0])&0xf0)>>4;
		a2=(ptcom->U8_Data[i*2+0])&0xf;	
		a3=((ptcom->U8_Data[i*2+1])&0xf0)>>4;
		a4=(ptcom->U8_Data[i*2+1])&0xf;			
		
		*(U8 *)(AD1+13+k)=asicc(a3);
		*(U8 *)(AD1+14+k)=asicc(a4);	
		*(U8 *)(AD1+15+k)=asicc(a1);
		*(U8 *)(AD1+16+k)=asicc(a2);		
			
		k=k+4;									
	}		
	aakj=CalcHe((U8 *)AD1,13+len*4);
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+13+len*4)=asicc(a1);
	*(U8 *)(AD1+14+len*4)=asicc(a2);
	*(U8 *)(AD1+15+len*4)=0x03;	
	
	ptcom->send_length[0]=16+len*4;			//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
	ptcom->send_times=1;					//���ʹ���
		
	ptcom->return_length[0]=9;				//�������ݳ���
	ptcom->return_start[0]=0;				//����������Ч��ʼ
	ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	ptcom->Simens_Count=0;
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
	int i,j,b1,b2;
	int ps,t,k;
	int add;
	int a1,a2,a3,a4,a5;
	U16 aakj,len,plcadd;
	
	if (*(U8 *)(PE+3)=='D')						//д��Ĵ���
	{
		add=0x44;
	}
	else
	{
		add=0x52;
	}
	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));//���ݳ���
	staradd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//���ݳ���
	plcadd=*(U8 *)(PE+4);	      //PLCվ��ַ	
	b1=(plcadd>>4)&0xf;
	b2=(plcadd)&0xf;		
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
	
	ps=144;
	for (i=0;i<SendTimes;i++)
	{
		if (i==SendTimes-1)	//���һ��
		{
			len=LastTimeWord;
		}
		else
		{
			len=32;
		}
		t=staradd+i*32;		//��ʼ��ַ
		a1=t/10000;
		a2=(t-a1*10000)/1000;
		a3=(t-a1*10000-a2*1000)/100;	
		a4=(t-a1*10000-a2*1000-a3*100)/10;
		a5=(t-a1*10000-a2*1000-a3*100-a4*10)/1;	

		*(U8 *)(AD1+0+ps*i)=0x02;
		*(U8 *)(AD1+1+ps*i)=asicc(b1);	//plc add
		*(U8 *)(AD1+2+ps*i)=asicc(b2);
		*(U8 *)(AD1+3+ps*i)=0x34;		//command
		*(U8 *)(AD1+4+ps*i)=0x37;	
		*(U8 *)(AD1+5+ps*i)=asicc((len>>4)&0xf);	//���ͳ���
		*(U8 *)(AD1+6+ps*i)=asicc(len&0xf);		//���ͳ���

		*(U8 *)(AD1+7+ps*i)=add;			//D,R
		*(U8 *)(AD1+8+ps*i)=asicc(a1);
		*(U8 *)(AD1+9+ps*i)=asicc(a2);
		*(U8 *)(AD1+10+ps*i)=asicc(a3);
		*(U8 *)(AD1+11+ps*i)=asicc(a4);
		*(U8 *)(AD1+12+ps*i)=asicc(a5);	
		
		k=0;
		for (j=0;j<len;j++)
		{
			a1=((*(U8 *)(PE+9+i*64+j*2+0))&0xf0)>>4;
			a2=(*(U8 *)(PE+9+i*64+j*2+0))&0xf;	
			a3=((*(U8 *)(PE+9+i*64+j*2+1))&0xf0)>>4;
			a4=(*(U8 *)(PE+9+i*64+j*2+1))&0xf;			
			
			*(U8 *)(AD1+13+k+ps*i)=asicc(a3);
			*(U8 *)(AD1+14+k+ps*i)=asicc(a4);	
			*(U8 *)(AD1+15+k+ps*i)=asicc(a1);
			*(U8 *)(AD1+16+k+ps*i)=asicc(a2);		
				
			k=k+4;									
		}
		
		aakj=CalcHe((U8 *)AD1+ps*i,13+len*4);
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13+len*4+ps*i)=asicc(a1);
		*(U8 *)(AD1+14+len*4+ps*i)=asicc(a2);
		*(U8 *)(AD1+15+len*4+ps*i)=0x03;	
		
		ptcom->send_length[i]=16+len*4;			//���ͳ���
		ptcom->send_staradd[i]=ps*i;			//�������ݴ洢��ַ	
			
		ptcom->return_length[i]=9;				//�������ݳ���
		ptcom->return_start[i]=0;				//����������Ч��ʼ
		ptcom->return_length_available[i]=0;	//������Ч���ݳ���				
	}
	ptcom->send_times=SendTimes;					//���ʹ���
	ptcom->Current_Times=0;					//��ǰ���ʹ���		
	ptcom->Simens_Count=0;
}


void compxy(void)				//����ɱ�׼�洢��ʽ
{
	int i;
	unsigned char a1,a2,a3,a4;
	int b,b1,b2;
	U8 temp[500];
	if (ptcom->Simens_Count==1)			//λ
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/4;i++)						//ASC�귵�أ�����ҪתΪ16����
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4);
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+1);
			a3=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+2);
			a4=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+3);			
			a1=bsicc(a1);
			a2=bsicc(a2);
			a3=bsicc(a3);
			a4=bsicc(a4);			
			b1=(a1<<4)+a2;
			b2=(a3<<4)+a4;			
			*(U8 *)(COMad+i*2+0)=b2;							//���´�,�ӵ�0����ʼ��
			*(U8 *)(COMad+i*2+1)=b1;							//���´�,�ӵ�0����ʼ��						
		}
		ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;	//���ȼ���	
		ptcom->IfResultCorrect=1;
	
	}
	else if (ptcom->Simens_Count==2)
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/2;i++)						//ASC�귵�أ�����ҪתΪ16����
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2);
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2+1);
			a1=bsicc(a1);
			a2=bsicc(a2);
			b=(a1<<4)+a2;
		//	*(U8 *)(COMad+i)=b;							//���´�,�ӵ�0����ʼ��
			temp[i]=b;
		}
		for (i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/2;i++)
		{
			*(U8 *)(COMad+i)=temp[i];			
		}
		ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;	//���ȼ���	
		ptcom->IfResultCorrect=1;	
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
		ptcom->Simens_Count=0;
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
	unsigned int aakj1;
	unsigned int aakj2;
	unsigned int akj1;
	unsigned int akj2;
	aakj2=*(U8 *)(COMad+ptcom->return_length[ptcom->Current_Times-1]-2)&0xff;		
	aakj1=*(U8 *)(COMad+ptcom->return_length[ptcom->Current_Times-1]-3)&0xff;
	akj1=CalcHe((U8 *)COMad,ptcom->return_length[ptcom->Current_Times-1]-3);
	akj2=(akj1&0xf0)>>4;
	akj2=akj2&0xf;
	akj1=akj1&0xf;
	akj1=asicc(akj1);
	akj2=asicc(akj2);
	if((akj1==aakj2)&&(akj2==aakj1))
		return 1;
	else
		return 1;
}


U16 CalcHe(unsigned char *chData,U16 uNo)		//�����У��
{
	int i;
	int ab=0;
	for(i=0;i<uNo;i++)
	{
		ab=ab+chData[i];
	}
	return (ab);
}
