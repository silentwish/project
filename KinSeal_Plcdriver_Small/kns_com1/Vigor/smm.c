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
		case 'T':
		case 'C':						
			Read_Bool();
			break;
		case 'D':
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
		case 'Y':
		case 'T':
		case 'C':				
			Set_Reset();
			break;
		case 'D':
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
			
			Write_Time();		
			break;			
		}
		break;	
	case PLC_READ_TIME:				//���������Ƕ�ȡʱ�䵽PLC
		switch(ptcom->registerr)
		{
		case 'D':		
			Read_Time();		
			break;			
		}
		break;
	case PLC_WRITE_RECIPE:				//����������д�䷽��PLC
		switch(*(U8 *)(PE+3))//�䷽�Ĵ�������
		{
		case 'D':		
			Write_Recipe();		
			break;			
		}
		break;
	case PLC_READ_RECIPE:				//���������Ǵ�PLC��ȡ�䷽
		switch(*(U8 *)(PE+3))//�䷽�Ĵ�������
		{
		case 'D':		
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
	int b;
	int a1,a2,a3,a4;
	int add,c;	

	b=ptcom->address;			// ��λ��ַ
	switch (ptcom->registerr)	//���ݼĴ������ͻ��ƫ�Ƶ�ַ
	{
	case 'Y':
		add=0x40;
		break;		
	case 'M':
		add=0x80;
		break;	
	case 'T':
		add=0x380;
		break;
	case 'C':
		add=0x3a0;
		break;					
	}
	c=b%8;	//�õ�����
	b=b/8+add;//�õ����ϵ�ַ,����ƫ�Ƶ�ַ
	b=b<<3;//����3λ
	b=b+c;
	
	a1=b&0xf000;
	a1=a1>>12;
	a1=a1&0xf;
	a2=b&0xf00;
	a2=a2>>8;
	a2=a2&0xf;
	a3=b&0xf0;
	a3=a3>>4;
	a3=a3&0xf;
	a4=b&0xf;
	
	*(U8 *)(AD1+0)=0x02;
	*(U8 *)(AD1+1)=asicc(((ptcom->plc_address)>>4)&0xff);			//PLCվ��ַ
	*(U8 *)(AD1+2)=asicc(((ptcom->plc_address)>>0)&0xff);	
	*(U8 *)(AD1+3)=0x37;									//������
	if (ptcom->writeValue==1)	//��λ
	{
		*(U8 *)(AD1+4)=0x30;
	}
	if (ptcom->writeValue==0)	//��λ
	{
		*(U8 *)(AD1+4)=0x31;
	}	
	*(U8 *)(AD1+5)=asicc(a1);
	*(U8 *)(AD1+6)=asicc(a2);
	*(U8 *)(AD1+7)=asicc(a3);
	*(U8 *)(AD1+8)=asicc(a4);
	*(U8 *)(AD1+9)=0x03;
	aakj=CalcHe((U8 *)AD1,10);
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+10)=asicc(a1);
	*(U8 *)(AD1+11)=asicc(a2);
	
	ptcom->send_length[0]=12;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
	ptcom->send_times=1;					//���ʹ���
		
	ptcom->return_length[0]=10;				//�������ݳ���
	ptcom->return_start[0]=0;				//����������Ч��ʼ
	ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
	ptcom->Current_Times=0;					//��ǰ���ʹ���			
}


void Read_Bool()				//��ȡ����������Ϣ
{
	U16 aakj;
	int b;
	int a1,a2,a3,a4;
	int add;
	int b1,b2;
		
	switch (ptcom->registerr)	//���ݼĴ������ͻ��ƫ�Ƶ�ַ
	{
	case 'X':
		add=0x0;
		break;
	case 'Y':
		add=0x40;
		break;		
	case 'M':
		add=0x80;
		break;	
	case 'T':
		add=0x380;
		break;
	case 'C':
		add=0x3a0;
		break;					
	}		
				
	b=ptcom->address;			//���������Ѿ�ת�����öεĿ�ʼ��ַ
	b=b/8;						//ÿ8��λռ��һ����ַ
	b=b+add;					//����ƫ�Ƶ�ַ
	a1=b&0xf000;
	a1=a1>>12;
	a1=a1&0xf;
	a2=b&0xf00;
	a2=a2>>8;
	a2=a2&0xf;
	a3=b&0xf0;
	a3=a3>>4;
	a3=a3&0xf;
	a4=b&0xf;
	*(U8 *)(AD1+0)=0x02;
	*(U8 *)(AD1+1)=asicc(((ptcom->plc_address)>>4)&0xff);			//PLCվ��ַ
	*(U8 *)(AD1+2)=asicc(((ptcom->plc_address)>>0)&0xff);	
	*(U8 *)(AD1+3)=0x35;									//�������������϶�ȡ
	*(U8 *)(AD1+4)=0x31;	
	*(U8 *)(AD1+5)=asicc(a1);								//��ʼ��ַ
	*(U8 *)(AD1+6)=asicc(a2);
	*(U8 *)(AD1+7)=asicc(a3);
	*(U8 *)(AD1+8)=asicc(a4);
	b1=ptcom->register_length>>4;
	b2=ptcom->register_length&0x0f;
	*(U8 *)(AD1+9)=asicc(b1);								//���ͳ���
	*(U8 *)(AD1+10)=asicc(b2);
	*(U8 *)(AD1+11)=0x03;
	aakj=CalcHe((U8 *)AD1,12);
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+12)=asicc(a1);								//��У��
	*(U8 *)(AD1+13)=asicc(a2);
	
	ptcom->send_length[0]=14;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
	ptcom->send_times=1;					//���ʹ���
		
	ptcom->return_length[0]=7+ptcom->register_length*2+3;				//�������ݳ��ȣ���7���̶�+����+03+2У��
	ptcom->return_start[0]=7;				//����������Ч��ʼ
	ptcom->return_length_available[0]=ptcom->register_length*2;	//������Ч���ݳ���	
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	ptcom->send_add[0]=ptcom->address;		//�����������ַ������		
	ptcom->Simens_Count=1;					//��BOOL��ʶ
}



void Read_Analog()				//��ģ����
{
	U16 aakj;
	int b;
	int a1,a2,a3,a4;
	int add;
	int b1,b2;
		
	switch (ptcom->registerr)	//���ݼĴ������ͻ��ƫ�Ƶ�ַ
	{
	case 'D':
		add=0x1c00;
		break;
	case 't':
		add=0x1400;
		break;		
	case 'c':
		add=0x1800;
		break;						
	}		
				
	b=ptcom->address;			//��ʼ��ַ

	if (b >= 9000)				//����Ĵ�����дD9000~D9255����Ӧ��ַΪ1600��1601----17FE,17FF
	{
		b=b-9000;
		add=0x1600;
	}

	b=b*2+add;					//����ƫ�Ƶ�ַ
	a1=b&0xf000;
	a1=a1>>12;
	a1=a1&0xf;
	a2=b&0xf00;
	a2=a2>>8;
	a2=a2&0xf;
	a3=b&0xf0;
	a3=a3>>4;
	a3=a3&0xf;
	a4=b&0xf;
	
	*(U8 *)(AD1+0)=0x02;
	*(U8 *)(AD1+1)=asicc(((ptcom->plc_address)>>4)&0xff);			//PLCվ��ַ
	*(U8 *)(AD1+2)=asicc(((ptcom->plc_address)>>0)&0xff);	
	*(U8 *)(AD1+3)=0x35;									//�������������϶�ȡ
	*(U8 *)(AD1+4)=0x31;	
	*(U8 *)(AD1+5)=asicc(a1);								//��ʼ��ַ
	*(U8 *)(AD1+6)=asicc(a2);
	*(U8 *)(AD1+7)=asicc(a3);
	*(U8 *)(AD1+8)=asicc(a4);
	b1=(ptcom->register_length*2)>>4;						//������������ĵ�λ����
	b2=(ptcom->register_length*2)&0x0f;
	*(U8 *)(AD1+9)=asicc(b1);								//���ͳ���
	*(U8 *)(AD1+10)=asicc(b2);
	*(U8 *)(AD1+11)=0x03;
	aakj=CalcHe((U8 *)AD1,12);
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+12)=asicc(a1);								//��У��
	*(U8 *)(AD1+13)=asicc(a2);
	
	ptcom->send_length[0]=14;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
	ptcom->send_times=1;					//���ʹ���
		
	ptcom->return_length[0]=7+ptcom->register_length*4+3;				//�������ݳ��ȣ���7���̶�+����+03+2У��
	ptcom->return_start[0]=7;				//����������Ч��ʼ
	ptcom->return_length_available[0]=ptcom->register_length*4;	//������Ч���ݳ���	
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	ptcom->send_add[0]=ptcom->address;		//�����������ַ������	
	ptcom->Simens_Count=2;					//��ANALOG��ʶ
}


void Read_Recipe()								//��ȡ�䷽
{
	U16 aakj;
	int b;
	int a1,a2,a3,a4;
	int i;
	int datalength;
	int p_start;
	int ps;
	int SendTimes;
	int LastTimeWord;							//���һ�η��ͳ���
	int currentlength;
	
		
	datalength=ptcom->register_length;		//�����ܳ���
	p_start=ptcom->address;					//��ʼ��ַ
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
	

	for (i=0;i<SendTimes;i++)
	{
		ps=14*i;
		b=p_start+i*32;
		b=b*2+0x1c00;				//����ƫ�Ƶ�ַ
		a1=b&0xf000;
		a1=a1>>12;
		a1=a1&0xf;
		a2=b&0xf00;
		a2=a2>>8;
		a2=a2&0xf;
		a3=b&0xf0;
		a3=a3>>4;
		a3=a3&0xf;
		a4=b&0xf;
		
			
		*(U8 *)(AD1+0+ps)=0x02;
		*(U8 *)(AD1+1+ps)=asicc(((ptcom->plc_address)>>4)&0xff);			//PLCվ��ַ
		*(U8 *)(AD1+2+ps)=asicc(((ptcom->plc_address)>>0)&0xff);	
		*(U8 *)(AD1+3+ps)=0x35;									//�������������϶�ȡ
		*(U8 *)(AD1+4+ps)=0x31;	
		*(U8 *)(AD1+5+ps)=asicc(a1);
		*(U8 *)(AD1+6+ps)=asicc(a2);
		*(U8 *)(AD1+7+ps)=asicc(a3);
		*(U8 *)(AD1+8+ps)=asicc(a4);
	
		if (i!=(SendTimes-1))	//�������һ��ʱ
		{
			*(U8 *)(AD1+9+ps)=0x34;//�̶�����32������64�ֽ�
			*(U8 *)(AD1+10+ps)=0x30;
			currentlength=32;
		}
		if (i==(SendTimes-1))	//���һ��ʱ
		{
			*(U8 *)(AD1+9+ps)=asicc(((LastTimeWord*2)>>4)&0xf);//�̶�����16������32�ֽ�
			*(U8 *)(AD1+10+ps)=asicc(((LastTimeWord*2))&0xf);
			currentlength=LastTimeWord;
		}	
			
		*(U8 *)(AD1+11+ps)=0x03;
		aakj=CalcHe((U8 *)AD1+ps,12);
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+12+ps)=asicc(a1);
		*(U8 *)(AD1+13+ps)=asicc(a2);
		
		ptcom->send_length[i]=14;				//���ͳ���
		ptcom->send_staradd[i]=i*14;			//�������ݴ洢��ַ	
		ptcom->send_add[i]=p_start+i*32;		//�����������ַ������	
		ptcom->send_data_length[i]=currentlength;	//�������һ�ζ���32��D
				
		ptcom->return_length[i]=7+currentlength*4+3;				//�������ݳ��ȣ���7���̶�+����+03+2У��
		ptcom->return_start[i]=7;				//����������Ч��ʼ
		ptcom->return_length_available[i]=currentlength*4;	//������Ч���ݳ���		
	}
	ptcom->send_times=SendTimes;					//���ʹ���
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	ptcom->Simens_Count=2;					//��ANALOG��ʶ
}

void Write_Analog()								//дģ����
{
	U16 aakj;
	int b;
	int a1,a2,a3,a4;
	int add;
	int b1,b2;
	int i,k;
	int length;
	
	switch (ptcom->registerr)	//���ݼĴ������ͻ��ƫ�Ƶ�ַ
	{
	case 'D':
		add=0x1c00;
		break;
	case 't':
		add=0x1400;
		break;		
	case 'c':
		add=0x1800;
		break;						
	}		
	b=ptcom->address;			//��ʼ��ַ
	
	if (b >= 9000)				//����Ĵ�����дD9000~D9255����Ӧ��ַΪ1600��1601----17FE,17FF
	{
		b=b-9000;
		add=0x1600;
	}
	
	b=b*2+add;					//����ƫ�Ƶ�ַ
	a1=b&0xf000;
	a1=a1>>12;
	a1=a1&0xf;
	a2=b&0xf00;
	a2=a2>>8;
	a2=a2&0xf;
	a3=b&0xf0;
	a3=a3>>4;
	a3=a3&0xf;
	a4=b&0xf;
	
	*(U8 *)(AD1+0)=0x02;
	*(U8 *)(AD1+1)=asicc(((ptcom->plc_address)>>4)&0xff);			//PLCվ��ַ
	*(U8 *)(AD1+2)=asicc(((ptcom->plc_address)>>0)&0xff);	
	*(U8 *)(AD1+3)=0x36;									//�������������϶�ȡ
	*(U8 *)(AD1+4)=0x31;	
	*(U8 *)(AD1+5)=asicc(a1);								//��ʼ��ַ
	*(U8 *)(AD1+6)=asicc(a2);
	*(U8 *)(AD1+7)=asicc(a3);
	*(U8 *)(AD1+8)=asicc(a4);
	b1=(ptcom->register_length*2)>>4;		//һ��D��2��CHAR
	b2=(ptcom->register_length*2)&0x0f;
	*(U8 *)(AD1+9)=asicc(b1);				//���ͳ���
	*(U8 *)(AD1+10)=asicc(b2);
	
	k=0;
	length=ptcom->register_length*2;
	for (i=0;i<length;i++)
	{
		a1=((ptcom->U8_Data[i])&0xf0)>>4;
		a2=(ptcom->U8_Data[i])&0xf;	
		*(U8 *)(AD1+11+k)=asicc(a1);
		*(U8 *)(AD1+12+k)=asicc(a2);		
		k=k+2;	
	}
	*(U8 *)(AD1+11+ptcom->register_length*4)=0x03;
	
	aakj=CalcHe((U8 *)AD1,(11+ptcom->register_length*4+1));	//�����У��
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+11+ptcom->register_length*4+1)=asicc(a1);
	*(U8 *)(AD1+11+ptcom->register_length*4+2)=asicc(a2);	

	ptcom->send_length[0]=11+ptcom->register_length*4+3;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
	ptcom->send_times=1;					//���ʹ���
			
	ptcom->return_length[0]=10;				//�������ݳ���
	ptcom->return_start[0]=0;				//����������Ч��ʼ
	ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
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
	int a1,a2,a3,a4;
	U8 temp;
	U16 aakj;
	
	
	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));//���ݳ���
	staradd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//���ݳ���
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
	
	ps=142;
	for (i=0;i<SendTimes;i++)
	{
		b=staradd+i*32;//��ʼ��ַ
		b=b*2+0x1c00;	//����ƫ�Ƶ�ַ
		a1=b&0xf000;
		a1=a1>>12;
		a1=a1&0xf;
		a2=b&0xf00;
		a2=a2>>8;
		a2=a2&0xf;
		a3=b&0xf0;
		a3=a3>>4;
		a3=a3&0xf;
		a4=b&0xf;
		*(U8 *)(AD1+0+ps*i)=0x02;
		*(U8 *)(AD1+1+ps*i)=asicc(((ptcom->plc_address)>>4)&0xff);			//PLCվ��ַ
		*(U8 *)(AD1+2+ps*i)=asicc(((ptcom->plc_address)>>0)&0xff);	
		*(U8 *)(AD1+3+ps*i)=0x36;									//�������������϶�ȡ
		*(U8 *)(AD1+4+ps*i)=0x31;
		
		*(U8 *)(AD1+5+ps*i)=asicc(a1);
		*(U8 *)(AD1+6+ps*i)=asicc(a2);
		*(U8 *)(AD1+7+ps*i)=asicc(a3);
		*(U8 *)(AD1+8+ps*i)=asicc(a4);

		if (i!=(SendTimes-1))//����һ�η��ͣ��򷢹̶�����32��D����64���ֽ�,����PLC�У���ASC�뷢�ͣ�һ��Dռ4��CHAR
		{
			*(U8 *)(AD1+9+ps*i)=0x34;
			*(U8 *)(AD1+10+ps*i)=0x30;
			for(j=0;j<64;j++)
			{
				temp=*(U8 *)(PE+9+i*64+j);
				a1=(temp&0xf0)>>4;
				a2=temp&0xf;
				*(U8 *)(AD1+11+j*2+ps*i)=asicc(a1);
				*(U8 *)(AD1+11+j*2+ps*i+1)=asicc(a2);				
			}
			*(U8 *)(AD1+139+ps*i)=0x03;
			aakj=CalcHe((U8 *)(AD1+i*ps),140);
			a1=(aakj&0xf0)>>4;
			a1=a1&0xf;
			a2=aakj&0xf;
			*(U8 *)(AD1+140+i*ps)=asicc(a1);
			*(U8 *)(AD1+141+i*ps)=asicc(a2);
			ptcom->send_length[i]=142+i*ps;				//���ͳ���
			ptcom->send_staradd[i]=i*ps;			//�������ݴ洢��ַ	
			ptcom->return_length[i]=10;				//�������ݳ���
			ptcom->return_start[i]=0;				//����������Ч��ʼ
			ptcom->return_length_available[i]=0;	//������Ч���ݳ���				
		}
		if (i==(SendTimes-1))//���һ�η���
		{
			*(U8 *)(AD1+9+ps*i)=asicc(((LastTimeWord*2)>>4)&0xf);
			*(U8 *)(AD1+10+ps*i)=asicc((LastTimeWord*2)&0xf);
			for(j=0;j<LastTimeWord*2;j++)
			{
				temp=*(U8 *)(PE+9+i*64+j);
				a1=(temp&0xf0)>>4;
				a2=temp&0xf;		
				*(U8 *)(AD1+11+j*2+ps*i)=asicc(a1);
				*(U8 *)(AD1+11+j*2+ps*i+1)=asicc(a2);							
			}
			*(U8 *)(AD1+LastTimeWord*4+11+ps*i)=0x03;
			aakj=CalcHe((U8 *)(AD1+i*ps),LastTimeWord*4+11+1);
			a1=(aakj&0xf0)>>4;
			a1=a1&0xf;
			a2=aakj&0xf;
			*(U8 *)(AD1+LastTimeWord*4+11+1+i*ps)=asicc(a1);
			*(U8 *)(AD1+LastTimeWord*4+11+2+i*ps)=asicc(a2);
			ptcom->send_length[i]=LastTimeWord*4+11+3;				//���ͳ���
			ptcom->send_staradd[i]=i*ps;			//�������ݴ洢��ַ	
			ptcom->return_length[i]=10;				//�������ݳ���
			ptcom->return_start[i]=0;				//����������Ч��ʼ
			ptcom->return_length_available[i]=0;	//������Ч���ݳ���	
		}		
	}
	ptcom->send_times=SendTimes;					//���ʹ���
	ptcom->Current_Times=0;					//��ǰ���ʹ���		
}


void compxy(void)				//����ɱ�׼�洢��ʽ
{
	int i;
	unsigned char a1,a2,a3,a4;
	int b,b1;
	
	if(ptcom->Simens_Count==1)								//��BOOL��
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/2;i++)						//ASC�귵�أ�����ҪתΪ16����
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2);
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2+1);
			a1=bsicc(a1);
			a2=bsicc(a2);
			b=(a1<<4)+a2;
			*(U8 *)(COMad+i)=b;							//���´�,�ӵ�0����ʼ��
		}
		ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;	//���ȼ���	
	}
	else if(ptcom->Simens_Count==2)								//��ģ����,�ߵ�ǰ���ֽ�
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
			b=(a1<<4)+a2;
			b1=(a3<<4)+a4;			
			*(U8 *)(COMad+i*2+0)=b1;							//���´�,�ӵ�0����ʼ��
			*(U8 *)(COMad+i*2+1)=b;							//���´�,�ӵ�0����ʼ��			
		}
		ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;	//���ȼ���	
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
	unsigned int aakj1;
	unsigned int aakj2;
	unsigned int akj1;
	unsigned int akj2;
	aakj2=*(U8 *)(COMad+ptcom->return_length[ptcom->Current_Times-1]-1)&0xff;		//�ڷ������Current_Times++����ʱҪ--
	aakj1=*(U8 *)(COMad+ptcom->return_length[ptcom->Current_Times-1]-2)&0xff;
	akj1=CalcHe((U8 *)COMad,ptcom->return_length[ptcom->Current_Times-1]-2);
	akj2=(akj1&0xf0)>>4;
	akj2=akj2&0xf;
	akj1=akj1&0xf;
	akj1=asicc(akj1);
	akj2=asicc(akj2);
	if((akj1==aakj2)&&(akj2==aakj1))
		return 1;
	else
	//	*(U8 *)(AD1+100)=aakj2;
		return 0;
}


U16 CalcHe(unsigned char *chData,U16 uNo)		//�����У��
{
	int i;
	int ab=0;
	for(i=1;i<uNo;i++)
	{
		ab=ab+chData[i];
	}
	return (ab);
}
