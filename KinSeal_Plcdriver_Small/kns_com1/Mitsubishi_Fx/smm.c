#include "stdio.h"
#include "def.h"
#include "smm.h"

	
	
 struct Com_struct_D *ptcom;



void Enter_PlcDriver(void)
{

	ptcom=(struct Com_struct_D *)adcom;	
	//sysprintf("fx enter plc driver = %c\n",ptcom->registerr);
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
		case 'N':	
		case 'L':
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
		case 'N':	
		case 't':
		case 'c':
			Write_Analog();		
			break;		
		case 'L':
			Write_Wordbit();
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
	int add;	

	b=ptcom->address;			// ��λ��ַ
	switch (ptcom->registerr)	//���ݼĴ������ͻ��ƫ�Ƶ�ַ
	{
	case 'Y':
		add=0x500;
		break;
	case 'M':
	case 'H':	
		add=0x800;
		break;	
	case 'T':
		add=0x600;
		break;
	case 'C':
		add=0xE00;
		break;	
	}
	b=b+add;					//��ʼ��ַƫ��
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
	if (ptcom->writeValue==1)	//��λ
	{
		*(U8 *)(AD1+1)=0x37;
	}
	if (ptcom->writeValue==0)	//��λ
	{
		*(U8 *)(AD1+1)=0x38;
	}	
	*(U8 *)(AD1+2)=asicc(a3);
	*(U8 *)(AD1+3)=asicc(a4);
	*(U8 *)(AD1+4)=asicc(a1);
	*(U8 *)(AD1+5)=asicc(a2);
	*(U8 *)(AD1+6)=0x03;
	aakj=CalcHe((U8 *)AD1,7);
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+7)=asicc(a1);
	*(U8 *)(AD1+8)=asicc(a2);
	
	ptcom->send_length[0]=9;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
	ptcom->send_times=1;					//���ʹ���
		
	ptcom->return_length[0]=1;				//�������ݳ���
	ptcom->return_start[0]=0;				//����������Ч��ʼ
	ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
	ptcom->Current_Times=0;					//��ǰ���ʹ���			

	ptcom->send_staradd[99]=0;	
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
		add=0x80;
		break;
	case 'Y':
		add=0xA0;
		break;		
	case 'M':
	case 'H':
		add=0x100;
		break;	
	case 'T':
		add=0xC0;
		break;
	case 'C':
		add=0x1C0;
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
	*(U8 *)(AD1+1)=0x30;
	*(U8 *)(AD1+2)=asicc(a1);	//��ʼ��ַ
	*(U8 *)(AD1+3)=asicc(a2);
	*(U8 *)(AD1+4)=asicc(a3);
	*(U8 *)(AD1+5)=asicc(a4);
	b1=ptcom->register_length>>4;
	b2=ptcom->register_length&0x0f;
	*(U8 *)(AD1+6)=asicc(b1);	//���ͳ���
	*(U8 *)(AD1+7)=asicc(b2);
	*(U8 *)(AD1+8)=0x03;
	aakj=CalcHe((U8 *)AD1,9);
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+9)=asicc(a1);
	*(U8 *)(AD1+10)=asicc(a2);
	
	ptcom->send_length[0]=11;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
	ptcom->send_times=1;					//���ʹ���
		
	ptcom->return_length[0]=4+ptcom->register_length*2;	//�������ݳ��ȣ���4���̶���02��03��У��
	ptcom->return_start[0]=1;				//����������Ч��ʼ
	ptcom->return_length_available[0]=ptcom->register_length*2;	//������Ч���ݳ���	
	ptcom->Current_Times=0;				//��ǰ���ʹ���	
	ptcom->send_add[0]=ptcom->address;		//�����������ַ������		

	ptcom->send_staradd[99]=1;		
	
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
	case 'N':
	case 'L':	
		add=0x1000;
		break;
	case 't':
		add=0x0800;
		break;		
	case 'c':
		add=0x0A00;
		break;						
	}		
				
	b=ptcom->address;			//��ʼ��ַ
	//sysprintf("Read_Analog b=%d\n",b);
	if(ptcom->registerr == 'L')
	{
		b = b/16;
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
	*(U8 *)(AD1+1)=0x30;	
	*(U8 *)(AD1+2)=asicc(a1);	//��ʼ��ַ
	*(U8 *)(AD1+3)=asicc(a2);
	*(U8 *)(AD1+4)=asicc(a3);
	*(U8 *)(AD1+5)=asicc(a4);
	
	b1=(ptcom->register_length*2)>>4;		//һ��D��2��CHAR
	b2=(ptcom->register_length*2)&0x0f;
	*(U8 *)(AD1+6)=asicc(b1);	//���ͳ���
	*(U8 *)(AD1+7)=asicc(b2);
	
	*(U8 *)(AD1+8)=0x03;
	aakj=CalcHe((U8 *)AD1,9);	//�����У��
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+9)=asicc(a1);
	*(U8 *)(AD1+10)=asicc(a2);
	
	
	ptcom->send_length[0]=11;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
	ptcom->send_times=1;					//���ʹ���
		
	ptcom->return_length[0]=4+ptcom->register_length*4;				//�������ݳ��ȣ���4���̶���02��03��У��
	ptcom->return_start[0]=1;				//����������Ч��ʼ
	ptcom->return_length_available[0]=ptcom->register_length*4;	//������Ч���ݳ���	
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	ptcom->send_add[0]=ptcom->address;		//�����������ַ������

	ptcom->send_staradd[99]=1;
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
		ps=11*i;
		b=p_start+i*32;
		b=b*2+0x1000;				//����ƫ�Ƶ�ַ
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
		*(U8 *)(AD1+1+ps)=0x30;
		*(U8 *)(AD1+2+ps)=asicc(a1);
		*(U8 *)(AD1+3+ps)=asicc(a2);
		*(U8 *)(AD1+4+ps)=asicc(a3);
		*(U8 *)(AD1+5+ps)=asicc(a4);
	
		if (i!=(SendTimes-1))	//�������һ��ʱ
		{
			*(U8 *)(AD1+6+ps)=0x34;//�̶�����32������64�ֽ�
			*(U8 *)(AD1+7+ps)=0x30;
			currentlength=32;
		}
		if (i==(SendTimes-1))	//���һ��ʱ
		{
			*(U8 *)(AD1+6+ps)=asicc(((LastTimeWord*2)>>4)&0xf);//�̶�����16������32�ֽ�
			*(U8 *)(AD1+7+ps)=asicc(((LastTimeWord*2))&0xf);
			currentlength=LastTimeWord;
		}	
			
		*(U8 *)(AD1+8+ps)=0x03;
		aakj=CalcHe((U8 *)AD1+ps,9);
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+9+ps)=asicc(a1);
		*(U8 *)(AD1+10+ps)=asicc(a2);
		
		ptcom->send_length[i]=11;				//���ͳ���
		ptcom->send_staradd[i]=i*11;			//�������ݴ洢��ַ	
		ptcom->send_add[i]=p_start+i*32;		//�����������ַ������	
		ptcom->send_data_length[i]=currentlength;	//�������һ�ζ���32��D
				
		ptcom->return_length[i]=4+currentlength*4;				//�������ݳ��ȣ���4���̶���02��03��У��
		ptcom->return_start[i]=1;				//����������Ч��ʼ
		ptcom->return_length_available[i]=currentlength*4;	//������Ч���ݳ���		
	}
	ptcom->send_times=SendTimes;					//���ʹ���
	ptcom->Current_Times=0;					//��ǰ���ʹ���	

	ptcom->send_staradd[99]=1;
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
	case 'N':
		add=0x1000;
		break;
	case 't':
		add=0x0800;
		break;		
	case 'c':
		add=0x0A00;
		break;						
	}		
				
	b=ptcom->address;			//��ʼ��ַ
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
	*(U8 *)(AD1+1)=0x31;	
	*(U8 *)(AD1+2)=asicc(a1);	//��ʼ��ַ
	*(U8 *)(AD1+3)=asicc(a2);
	*(U8 *)(AD1+4)=asicc(a3);
	*(U8 *)(AD1+5)=asicc(a4);
	
	b1=(ptcom->register_length*2)>>4;		//һ��D��2��CHAR
	b2=(ptcom->register_length*2)&0x0f;
	*(U8 *)(AD1+6)=asicc(b1);				//���ͳ���
	*(U8 *)(AD1+7)=asicc(b2);

	k=0;
	length=ptcom->register_length*2;
	for (i=0;i<length;i++)
	{
		a1=((ptcom->U8_Data[i])&0xf0)>>4;
		a2=(ptcom->U8_Data[i])&0xf;	
		*(U8 *)(AD1+8+k)=asicc(a1);
		*(U8 *)(AD1+9+k)=asicc(a2);		
		k=k+2;	
	}
	*(U8 *)(AD1+8+ptcom->register_length*4)=0x03;
	
	aakj=CalcHe((U8 *)AD1,(8+ptcom->register_length*4+1));	//�����У��
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+8+ptcom->register_length*4+1)=asicc(a1);
	*(U8 *)(AD1+8+ptcom->register_length*4+2)=asicc(a2);	

	
	ptcom->send_length[0]=8+ptcom->register_length*4+3;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
	ptcom->send_times=1;					//���ʹ���
			
	ptcom->return_length[0]=1;				//�������ݳ���
	ptcom->return_start[0]=0;				//����������Ч��ʼ
	ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
	ptcom->Current_Times=0;					//��ǰ���ʹ���	

	ptcom->send_staradd[99]=0;
}

void Write_Wordbit() //���ֽڵ�λ����
{
	U16 aakj,aakj1;
	int b;
	int a1,a2,a3,a4;
	int add;
	int b1,b2;
	int c1,c2,i;
		
	switch (ptcom->registerr)	//���ݼĴ������ͻ��ƫ�Ƶ�ַ
	{
	case 'L':	
		add=0x1000;
		break;					
	}		
				
	b=ptcom->address;			//��ʼ��ַ
	//sysprintf("Write_Wordbit  b=%d\n",b);
	if(ptcom->registerr == 'L')
	{
		b = b/16;
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
	*(U8 *)(AD1+1)=0x30;	
	*(U8 *)(AD1+2)=asicc(a1);	//��ʼ��ַ
	*(U8 *)(AD1+3)=asicc(a2);
	*(U8 *)(AD1+4)=asicc(a3);
	*(U8 *)(AD1+5)=asicc(a4);
	
	*(U8 *)(AD1+6)=0x30;	//���ͳ���
	*(U8 *)(AD1+7)=0x32;
	
	*(U8 *)(AD1+8)=0x03;
	aakj=CalcHe((U8 *)AD1,9);	//�����У��
	b1=(aakj&0xf0)>>4;
	b2=b1&0xf;
	b2=aakj&0xf;
	*(U8 *)(AD1+9)=asicc(b1);
	*(U8 *)(AD1+10)=asicc(b2);
		
	ptcom->send_length[0]=11;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
	ptcom->send_times=1;					//���ʹ���
		
	ptcom->return_length[0]=4+ptcom->register_length*4;				//�������ݳ��ȣ���4���̶���02��03��У��
	ptcom->return_start[0]=1;											//����������Ч��ʼ
	ptcom->return_length_available[0]=ptcom->register_length*4;		//������Ч���ݳ���	
	ptcom->Current_Times=0;											//��ǰ���ʹ���	
	ptcom->send_add[0]=ptcom->address;									//�����������ַ������

	*(U8 *)(AD1+11)=0x02;
	*(U8 *)(AD1+12)=0x31;	
	*(U8 *)(AD1+13)=asicc(a1);	//��ʼ��ַ
	*(U8 *)(AD1+14)=asicc(a2);
	*(U8 *)(AD1+15)=asicc(a3);
	*(U8 *)(AD1+16)=asicc(a4);
	
	b1=(ptcom->register_length*2)>>4;		//һ��D��2��CHAR
	b2=(ptcom->register_length*2)&0x0f;
	*(U8 *)(AD1+17)=asicc(b1);				//���ͳ���
	*(U8 *)(AD1+18)=asicc(b2);

	*(U8 *)(AD1+19)=0x30;
	*(U8 *)(AD1+20)=0x30;		
	*(U8 *)(AD1+21)=0x30;
	*(U8 *)(AD1+22)=0x30;

	*(U8 *)(AD1+23)=0x03;
	
	for(i=12;i<24;i++)
	{
		aakj1=aakj1+*(U8 *)(AD1+i);//�����У��
	}
	c1=(aakj1&0xf0)>>4;
	c1=c1&0xf;
	c2=aakj1&0xf;
	*(U8 *)(AD1+24)=asicc(c1);
	*(U8 *)(AD1+25)=asicc(c2);	

	ptcom->send_length[1]=15;				//���ͳ���
	ptcom->send_staradd[1]=11;				//�������ݴ洢��ַ	
	ptcom->send_times=2;					//���ʹ���
			
	ptcom->return_length[1]=1;				//�������ݳ���
	ptcom->return_start[1]=0;				//����������Ч��ʼ
	ptcom->return_length_available[1]=0;	//������Ч���ݳ���	
	ptcom->Current_Times=0;				//��ǰ���ʹ���	

	ptcom->send_staradd[99]= 100;//�ȶ���д	

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
	if(datalength%16==0)
	{
		SendTimes=datalength/16;
		LastTimeWord=16;//���һ�η��͵ĳ���	
	}
	if(datalength%16!=0)
	{
		SendTimes=datalength/16+1;//���͵Ĵ���
		LastTimeWord=datalength%16;//���һ�η��͵ĳ���	
	}	
	
	ps=75;
	for (i=0;i<SendTimes;i++)
	{
		b=staradd+i*16;//��ʼ��ַ
		b=b*2+0x1000;
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
		*(U8 *)(AD1+0+ps*i)=0x02;;
		*(U8 *)(AD1+1+ps*i)=0x31;
		*(U8 *)(AD1+2+ps*i)=asicc(a1);
		*(U8 *)(AD1+3+ps*i)=asicc(a2);
		*(U8 *)(AD1+4+ps*i)=asicc(a3);
		*(U8 *)(AD1+5+ps*i)=asicc(a4);

		if (i!=(SendTimes-1))//����һ�η��ͣ��򷢹̶�����32��D����64���ֽ�,����PLC�У���ASC�뷢�ͣ�һ��Dռ4��CHAR
		{
			*(U8 *)(AD1+6+ps*i)=0x32;
			*(U8 *)(AD1+7+ps*i)=0x30;
			for(j=0;j<32;j++)
			{
				temp=*(U8 *)(PE+9+i*32+j);
				a1=(temp&0xf0)>>4;
				a2=temp&0xf;
				*(U8 *)(AD1+8+j*2+ps*i)=asicc(a1);
				*(U8 *)(AD1+8+j*2+ps*i+1)=asicc(a2);				
			}
			*(U8 *)(AD1+72+ps*i)=0x03;
			aakj=CalcHe((U8 *)(AD1+i*ps),73);
			a1=(aakj&0xf0)>>4;
			a1=a1&0xf;
			a2=aakj&0xf;
			*(U8 *)(AD1+73+i*ps)=asicc(a1);
			*(U8 *)(AD1+74+i*ps)=asicc(a2);
			ptcom->send_length[i]=ps;				//���ͳ���
			ptcom->send_staradd[i]=i*ps;			//�������ݴ洢��ַ	
			ptcom->return_length[i]=1;				//�������ݳ���
			ptcom->return_start[i]=0;				//����������Ч��ʼ
			ptcom->return_length_available[i]=0;	//������Ч���ݳ���				
		}
		if (i==(SendTimes-1))//���һ�η���
		{
			*(U8 *)(AD1+6+ps*i)=asicc(((LastTimeWord*2)>>4)&0xf);
			*(U8 *)(AD1+7+ps*i)=asicc((LastTimeWord*2)&0xf);
			for(j=0;j<LastTimeWord*2;j++)
			{
				temp=*(U8 *)(PE+9+i*32+j);
				a1=(temp&0xf0)>>4;
				a2=temp&0xf;		
				*(U8 *)(AD1+8+j*2+ps*i)=asicc(a1);
				*(U8 *)(AD1+8+j*2+ps*i+1)=asicc(a2);							
			}
			*(U8 *)(AD1+LastTimeWord*4+8+ps*i)=0x03;
			aakj=CalcHe((U8 *)(AD1+i*ps),LastTimeWord*4+8+1);
			a1=(aakj&0xf0)>>4;
			a1=a1&0xf;
			a2=aakj&0xf;
			*(U8 *)(AD1+LastTimeWord*4+8+1+i*ps)=asicc(a1);
			*(U8 *)(AD1+LastTimeWord*4+8+2+i*ps)=asicc(a2);
			ptcom->send_length[i]=LastTimeWord*4+8+3;				//���ͳ���
			ptcom->send_staradd[i]=i*ps;			//�������ݴ洢��ַ	
			ptcom->return_length[i]=1;				//�������ݳ���
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
	unsigned char a1,a2,b;
	int nBitPos = 0;
	unsigned short nTemp = 0;
	int c1,c2;
	U16 aakj1;
	
	if(ptcom->send_staradd[99] == 100)
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
		nTemp = nTemp |  *(U8 *)(COMad+1);
		nTemp = (nTemp << 8) & 0xff00;
		nTemp = nTemp |  *(U8 *)(COMad+0);
		nBitPos = ptcom->address%16;
		if (ptcom->writeValue==1)	    	//��λ
		{
			nTemp = nTemp | (0x01 << nBitPos);
		}
		else if (ptcom->writeValue==0)	    //��λ
		{
			nTemp = nTemp & (~(0x01 << nBitPos));
		}
	
		*(U8 *)(AD1+19)=asicc((nTemp >> 4)&0x0f);	
		*(U8 *)(AD1+20)=asicc((nTemp >> 0)&0x0f);
		*(U8 *)(AD1+21)=asicc((nTemp >> 12)&0x0f);
		*(U8 *)(AD1+22)=asicc((nTemp >> 8)&0x0f);
		
		for(i=12;i<24;i++)
		{
			aakj1=aakj1+*(U8 *)(AD1+i);//�����У��
		}
		c1=(aakj1&0xf0)>>4;
		c1=c1&0xf;
		c2=aakj1&0xf;
		
		*(U8 *)(AD1+24)=asicc(c1);
		*(U8 *)(AD1+25)=asicc(c2);	

	}
	else 
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
	}
	
	ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;	//���ȼ���	
}


void SJ_TO_PLC(int n)
{

}


void SJ_TO_PLCC(int nb,float dd)
{

}


void PLC_TO_SJ(int n)
{

}


void float_to_char()
{

}

void char_to_float()
{

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
