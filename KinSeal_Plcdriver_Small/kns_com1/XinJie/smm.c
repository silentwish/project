#include "stdio.h"
#include "def.h"
#include "smm.h"


	
struct Com_struct_D *ptcom;

 
void Enter_PlcDriver(void)
{
	//sysprintf("ptcom->R_W_Flag=%d   ptcom->registerr=%c\n",ptcom->R_W_Flag,ptcom->registerr);
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
		case 'H':	     /*H��Ӧ֮ǰSK�е�S*/
		case 'T':
		case 'C':					
			Read_Bool();  //��������������
			break;
		case 'D':
		case 't':
		case 'c':
		case 'R':           /*R=FD*/
		case 'N':           /*N=ED*/
			Read_Analog();//������ģ������
			break;			
		}
		break;
		case PLC_WRITE_DATA:				//����������д����
		switch(ptcom->registerr)
		{
		case 'Y':
		case 'M':
		case 'H':         /*H��Ӧ֮ǰSK�е�S*/
		case 'T':
		case 'C':				
			Set_Reset();//��������λ�͸�λ 
			break;
		case 'D':
		case 't':
		case 'c':
		case 'R':           /*R=FD*/
		case 'N':           /*N=ED*/
			Write_Analog();//������дģ����		
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



void Set_Reset()                    //��λ�͸�λ
{
	U16 aakj;
	int b;
	int a1,a2,a3,a4;
	int add;
	int plcadd;	

	b=ptcom->address;			  // ��λ��ַ
	plcadd=ptcom->plc_address;	  //PLCվ��ַ
	switch (ptcom->registerr)	  //���ݼĴ������ͻ��ƫ�Ƶ�ַ
	{
	case 'Y':
		add=0x4800;
		break;
	case 'M':
	    if(b < 8000)
	    {
		 add=0x0000;
	    }
	    else
	    {
	      b = b-8000;
	      add = 0x6000;
	    }
		break;
    case 'H':
        add=0x5000;
		break;
	case 'T':
		add=0x6400;
		break;
	case 'C':
		add=0x6c00;
		break;					
	}
	b=b+add;					    //��ʼ��ַƫ��
	a1=(b>>8)&0xff;	                //��λ
	a2=b&0xff;	                    //��λ

	
	*(U8 *)(AD1+0)=plcadd;          //PLCվ��ַ
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
	a4=aakj-a3*0x100;
	*(U8 *)(AD1+6)=a4&0xff;             //У�� ��λ
	*(U8 *)(AD1+7)=a3&0xff;             //У�� ��λ

	
	ptcom->send_length[0]=8;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
	ptcom->send_times=1;					//���ʹ���
		
	ptcom->return_length[0]=8;				//�������ݳ���
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
	int plcadd;
	int length;
	
	b=ptcom->address;			//���������Ѿ�ת�����öεĿ�ʼ��ַ

	switch (ptcom->registerr)	//���ݼĴ������ͻ��ƫ�Ƶ�ַ
	{
	case 'X':
		add=0x4000;
		break;
	case 'Y':
		add=0x4800;
		break;
	case 'M':
	    if(b < 8000)
	    {
		 add=0x0000;
	    }
	    else
	    {
	      b = b-8000;
	      add = 0x6000;
	    }
		break;
    case 'H':
        add=0x5000;
		break;	
	case 'T':
		add=0x6400;
		break;
	case 'C':
		add=0x6c00;
		break;					
	}	
		
	plcadd=ptcom->plc_address;	//PLCվ��ַ				
	length=ptcom->register_length;
	length=length*8;            //һ��Ԫ������8λ����
	
	b=b+add;					//����ƫ�Ƶ�ַ
	a1=(b>>8)&0xff;
	a2=b&0xff;
	
	a3=(length>>8)&0xff;
	a4=length&0xff;
	
	*(U8 *)(AD1+0)=plcadd;
	*(U8 *)(AD1+1)=0x01;
	*(U8 *)(AD1+2)=a1;	        //��ʼ��ַ  ��λ
	*(U8 *)(AD1+3)=a2;          //��ʼ��ַ  ��λ
	*(U8 *)(AD1+4)=a3;          //Ԫ������  ��λ
	*(U8 *)(AD1+5)=a4;          //Ԫ������  ��λ
	aakj= CalcCrc((U8 *)AD1,6); //У��
	b1=aakj/0x100;
	b2=aakj-b1*0x100;
	*(U8 *)(AD1+6)=b2&0xff;     //У�� ��λ
	*(U8 *)(AD1+7)=b1&0xff;     //У�� ��λ
	
	ptcom->send_length[0]=8;				     //���ͳ���
	ptcom->send_staradd[0]=0;				     //�������ݴ洢��ַ	
	ptcom->send_times=1;					     //���ʹ���
		
	ptcom->return_length[0]=5+length/8;		     //�������ݳ��ȣ���5���̶�,У��
	ptcom->return_start[0]=3;				     //����������Ч��ʼ
	ptcom->return_length_available[0]=length/8;    //������Ч���ݳ���	
	ptcom->Current_Times=0;					     //��ǰ���ʹ���	
	ptcom->send_add[0]=ptcom->address;		     //�����������ַ������		
	
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

	b=ptcom->address;			      //���������Ѿ�ת�����öεĿ�ʼ��ַ
	plcadd=ptcom->plc_address;	      //PLCվ��ַ
	length=ptcom->register_length;
			
	switch (ptcom->registerr)	//���ݼĴ������ͻ��ƫ�Ƶ�ַ
	{
	case 'D':
		add=0x0;
		break;
	case 't':
		add=0x3000;
		break;		
	case 'c':
		add=0x3800;
		break;	
	case 'R':           //FD
        if (b < 8000)
        {
            add = 0x4800;
        }
        else//�����ڲ���Ȧʱƫ�Ƶ�ַ��һ��
        {
            add = 0x6800;
        }
		break;
	case 'N':          //ED
		add = 0x7000;
		break;				
	}		
				
	b=b+add;					      //����ƫ�Ƶ�ַ
	a1=(b>>8)&0xff;
	a2=b&0xff;
	
	a3=(length>>8)&0xff;              //Ԫ����������
	a4=length&0xff;
	
	*(U8 *)(AD1+0)=plcadd;
	*(U8 *)(AD1+1)=0x03;
	*(U8 *)(AD1+2)=a1;	             //��ʼ��ַ ��λ
	*(U8 *)(AD1+3)=a2;               //��ʼ��ַ ��λ
	*(U8 *)(AD1+4)=a3;               //Ԫ������ ��λ
	*(U8 *)(AD1+5)=a4;               //Ԫ������ ��λ
	aakj= CalcCrc((U8 *)AD1,6);      //У��
	b1=aakj/0x100;
	b2=aakj-b1*0x100;
	*(U8 *)(AD1+6)=b2&0xff;          //У�� ��λ
	*(U8 *)(AD1+7)=b1&0xff;          //У�� ��λ
	
	ptcom->send_length[0]=8;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
	ptcom->send_times=1;					//���ʹ���
		
	ptcom->return_length[0]=5+length*2;		//�������ݳ��ȣ���5���̶�,У��
	ptcom->return_start[0]=3;				//����������Ч��ʼ
	ptcom->return_length_available[0]=length*2;//������Ч���ݳ���	
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	ptcom->send_add[0]=ptcom->address;		//�����������ַ������		

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
	
		
	datalength=ptcom->register_length;		//�����ܳ���
	p_start=ptcom->address;					//��ʼ��ַ
	plcadd=ptcom->plc_address;
	
	if(datalength>5000)
		datalength=5000;                //ÿ������ܷ���32��D
		
	if(datalength%27==0)
	{
		SendTimes=datalength/27;
		LastTimeWord=27;                //�̶�����32	
	}
	if(datalength%27!=0)
	{
		SendTimes=datalength/27+1;      //���͵Ĵ���
		LastTimeWord=datalength%27;     //���һ�η��͵ĳ���	
	}
	
	for (i=0;i<SendTimes;i++)
	{
		ps=8*i;                         //ÿ�η�8������
		b=(p_start+i*27);				//����ƫ�Ƶ�ַ
		
		a1=(b>>8)&0xff;
		a2=b&0xff;
			
		*(U8 *)(AD1+0+ps)=plcadd;      //PLCվ��ַ
		*(U8 *)(AD1+1+ps)=0x03;        //������
		*(U8 *)(AD1+2+ps)=a1;          //��ʼ��ַ��λ
		*(U8 *)(AD1+3+ps)=a2;          //��ʼ��ַ��λ
	
		if (i!=(SendTimes-1))	//�������һ��ʱ
		{
			*(U8 *)(AD1+4+ps)=0x0;   //�̶�����32������64�ֽ� ��λ
			*(U8 *)(AD1+5+ps)=0x1b;        //�̶�����32������64�ֽ� ��λ
			currentlength=27;
		}
		if (i==(SendTimes-1))	//���һ��ʱ
		{
			*(U8 *)(AD1+4+ps)=(LastTimeWord>>8)&0xff;   //ʣ�೤��LastTimeWord���� ��λ
			*(U8 *)(AD1+5+ps)=LastTimeWord&0xff;        //ʣ�೤��LastTimeWord���� ��λ
			currentlength=LastTimeWord;
		}
		aakj= CalcCrc((U8 *)(AD1+ps),6);      //У��
		b1=aakj/0x100;
		b2=aakj-b1*0x100;
		*(U8 *)(AD1+6+ps)=b2&0xff;          //У�� ��λ
		*(U8 *)(AD1+7+ps)=b1&0xff;          //У�� ��λ			
			
		
		ptcom->send_length[i]=8;				    //���ͳ���
		ptcom->send_staradd[i]=i*8;			        //�������ݴ洢��ַ	
		ptcom->send_add[i]=p_start+i*27;		    //�����������ַ������	
		ptcom->send_data_length[i]=currentlength;	//�������һ�ζ���32��D
				
		ptcom->return_length[i]=5+currentlength*2;	//�������ݳ��ȣ���5���̶���plcadd��03���ֽ�����У��
		ptcom->return_start[i]=3;				    //����������Ч��ʼ
		ptcom->return_length_available[i]=currentlength*2;	//������Ч���ݳ���		
	}
	ptcom->send_times=SendTimes;					//���ʹ���
	ptcom->Current_Times=0;					        //��ǰ���ʹ���	
}

void Write_Analog()				//дģ����
{
	U16 aakj;
	int b;
	int a1,a2,a3,a4;
	int add;
	int b1,b2;
	int i;
	int length;
	int plcadd;
	
	b=ptcom->address;			//��ʼ��ַ
	plcadd=ptcom->plc_address;	//PLCվ��ַ
	length=ptcom->register_length;
	
	switch (ptcom->registerr)	//���ݼĴ������ͻ��ƫ�Ƶ�ַ
	{
	case 'D':
		add=0x0;
		break;
	case 't':
		add=0x3000;
		break;		
	case 'c':
		add=0x3800;
		break;	
	case 'R':         //FD
        if (b < 8000)
        {
            add = 0x4800;
        }
        else//�����ڲ���Ȧʱƫ�Ƶ�ַ��һ��
        {
            add = 0x6800;
        }
		break;	
	case 'N':        //ED
		add = 0x7000;
		break;				
	}		
				
	b=b+add;					//����ƫ�Ƶ�ַ
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
		b2=aakj-b1*0x100;
		*(U8 *)(AD1+6)=b2&0xff;       //У�� ��λ
		*(U8 *)(AD1+7)=b1&0xff;	      //У�� ��λ
		
		ptcom->send_length[0]=8;			    //���ͳ���
		ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
		ptcom->send_times=1;					//���ʹ���
			
		ptcom->return_length[0]=8;				//�������ݳ���
		ptcom->return_start[0]=0;				//����������Ч��ʼ
		ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
		ptcom->Current_Times=0;					//��ǰ���ʹ���			
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
		for (i=0;i<length;i++)              //д����Ԫ��ֵ
		{				
			a3=ptcom->U8_Data[i*2];
			a4=ptcom->U8_Data[i*2+1];
			*(U8 *)(AD1+7+i*2)=a4;          //д��Ԫ��ֵ��λ
			*(U8 *)(AD1+8+i*2)=a3;          //д��Ԫ��ֵ��λ		
		}
		aakj=CalcCrc((U8 *)AD1,7+length*2); //У��
		b1=aakj/0x100;
		b2=aakj-b1*0x100;
		*(U8 *)(AD1+7+length*2)=b2&0xff;    //У�� ��λ
		*(U8 *)(AD1+8+length*2)=b1&0xff;    //У�� ��λ
		
		ptcom->send_length[0]=9+length*2;		//���ͳ���
		ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
		ptcom->send_times=1;					//���ʹ���
			
		ptcom->return_length[0]=8;				//�������ݳ���
		ptcom->return_start[0]=0;				//����������Ч��ʼ
		ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
		ptcom->Current_Times=0;					//��ǰ���ʹ���	
	}
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
	int n;
	
	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));  //���ݳ���
	staradd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//��ʼ��ַ 	
	plcadd=*(U8 *)(PE+4);	   //PLCվ��ַ		  

	if(datalength%25==0)           //ÿ������ܷ���32��D������ǳ�����32��D�ı���
	{
		SendTimes=datalength/25;   //���͵Ĵ���
		LastTimeWord=25;           //���һ�η��͵ĳ���	
	}
	if(datalength%25!=0)           //������ǳ�����32��D�ı���
	{
		SendTimes=datalength/25+1; //���͵Ĵ���
		LastTimeWord=datalength%25;//���һ�η��͵ĳ���	
	}	
	ps=59;                         //ÿ�η�73������
	for (i=0;i<SendTimes;i++)
	{
		b=staradd+i*25;           //��ʼ��ַ
		a1=(b>>8)&0xff;
		a2=b&0xff;
		if (i!=(SendTimes-1))     //���ǵ�һ�η���
		{	
			length=25;
		}
		else
		{
			length=LastTimeWord;
		}
		*(U8 *)(AD1+0+ps*i)=plcadd;
		*(U8 *)(AD1+1+ps*i)=0x10;
		*(U8 *)(AD1+2+ps*i)=a1;
		*(U8 *)(AD1+3+ps*i)=a2;
		*(U8 *)(AD1+4+ps*i)=(length>>8)&0xff;
		*(U8 *)(AD1+5+ps*i)=(length)&0xff;
		*(U8 *)(AD1+6+ps*i)=length*2;

		for(j=0;j<length;j++)
		{
			*(U8 *)(AD1+7+ps*i+j*2)=*(U8 *)(PE+9+i*64+j*2+1);	  //ȡ����
			*(U8 *)(AD1+7+ps*i+j*2+1)=*(U8 *)(PE+9+i*64+j*2);			
		}
		aakj=CalcCrc((U8 *)AD1+ps*i,7+length*2);
		a1=aakj/0x100;
		a2=aakj-a1*0x100;
		*(U8 *)(AD1+7+ps*i+length*2)=a2;
		*(U8 *)(AD1+8+ps*i+length*2)=a1;

		
		ptcom->send_length[i]=9+length*2;		//���ͳ���
		ptcom->send_staradd[i]=i*ps;			//�������ݴ洢��ַ
			
		ptcom->return_length[i]=8;				//�������ݳ���
		ptcom->return_start[i]=0;				//����������Ч��ʼ
		ptcom->return_length_available[i]=0;	//������Ч���ݳ���				
	}
	ptcom->send_times=SendTimes;				//���ʹ���
	ptcom->Current_Times=0;					    //��ǰ���ʹ���		
}


void compxy(void)				//����ɱ�׼�洢��ʽ
{
	int i;
	unsigned char a1;
	for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1];i++)						//ASC�귵�أ�����ҪתΪ16����
	{
		a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i);
		*(U8 *)(COMad+i)=a1;							//���´�,�ӵ�0����ʼ��
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
