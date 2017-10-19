#include "stdio.h"
#include "def.h"
#include "smm.h"
#include "math.h"

	
	
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
		case 'Y':
		case 'M':
		case 'K':
		case 'L':						
			Read_Bool();//��������
			break;
		case 'D':	
			Read_Analog();//��ģ����
			break;			
		}
		break;
	case PLC_WRITE_DATA:				//����������д����
		switch(ptcom->registerr)
		{
		case 'M':
		case 'Y':
		case 'K':
		case 'L':				
			Set_Reset();//��λ��λ
			break;
		case 'D':
			Write_Analog();//дģ����		
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
		switch(ptcom->registerr)//�䷽�Ĵ�������
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
	int b,i;
	int a1;
	int func1;
	unsigned int code1,code2,code3,code4,code;
	int b1,b2,b3;

	b=ptcom->address;			// �Ĵ������Ŀ�ʼ��ַ
	b1 = b/16;
	b2 = b%16;	
	b3 = b1 * 2;
	
	if (ptcom->writeValue==1)	//��ʾ
	{
		func1=0x6f;
		code = 1;
		for (i=0;i<b2;i++)
		{
			code = code * 2;
		}
		code1 = asicc((code>>12)&0xf);	
		code2 = asicc((code>>8)&0xf);
		code3 = asicc((code>>4)&0xf);	
		code4 = asicc((code>>0)&0xf);			
	}
	else
	{
		func1=0x6e;
		code = 1;
		for (i=0;i<b2;i++)
		{
			code = code * 2;
		}
		code = 0xffff - code;
		code1 = asicc((code>>12)&0xf);	
		code2 = asicc((code>>8)&0xf);
		code3 = asicc((code>>4)&0xf);	
		code4 = asicc((code>>0)&0xf);			
	}
	
	*(U8 *)(AD1+0)=0x02;        //ͷENQ����02
	*(U8 *)(AD1+1)=func1;  		//fix
	*(U8 *)(AD1+2)=0x4d;		//
	*(U8 *)(AD1+3)=asicc((b3>>4)&0xf);		//��ַ
	*(U8 *)(AD1+4)=asicc((b3>>0)&0xf);		
	
	switch (ptcom->registerr)	//���ݼĴ������ͷ��Ͳ�ͬ�������ַ�
	{
	case 'Y':
		*(U8 *)(AD1+5)=0x35;        //�Ĵ�������
		*(U8 *)(AD1+6)=0x43;
		break;
				
	case 'M':
		*(U8 *)(AD1+5)=0x35;        //�Ĵ�������
		*(U8 *)(AD1+6)=0x44; 
		break;	
		
	case 'L':						//L
		*(U8 *)(AD1+5)=0x35;        //�Ĵ�������
		*(U8 *)(AD1+6)=0x46;
		break;
		
	case 'K':						//K
		*(U8 *)(AD1+5)=0x36;        //�Ĵ�������
		*(U8 *)(AD1+6)=0x30;
		break;							
	}		
	
	*(U8 *)(AD1+7)=0x30;        //�̶�
	*(U8 *)(AD1+8)=0x30;
	
	*(U8 *)(AD1+9)=code3;       
	*(U8 *)(AD1+10)=code4;
	
	*(U8 *)(AD1+11)=code1;	
	*(U8 *)(AD1+12)=code2;

	aakj=CalcHe((U8 *)AD1+1,12);  //��У�飬У��ǰ�������
	a1=aakj&0xff;                 //ֻ�����λ
	*(U8 *)(AD1+13)=asicc((a1>>4)&0xf);		//check
	*(U8 *)(AD1+14)=asicc((a1>>0)&0xf);	
	*(U8 *)(AD1+15)=0x03;			//END
	
	
	ptcom->send_length[0]=16;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
	ptcom->send_times=1;					//���ʹ���
		
	ptcom->return_length[0]=5;				//�������ݳ���
	ptcom->return_start[0]=0;				//����������Ч��ʼ
	ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
	ptcom->Current_Times=0;					//��ǰ���ʹ���			
}


void Read_Bool()				//��ȡ����������Ϣ
{
	U16 aakj;
	int b;
	int a1;
	int length;
	int b1,b2,b3;
	
	b=ptcom->address;			    // �Ĵ���������ַ
	length=ptcom->register_length;  //�����ĳ���

	b1 = b/16;
	b2 = b%16;	
	b3 = b1 * 2;
	
	if (b2 >= 8)					//�������ڵ���8
	{
		length = length /2 + 1;
	}
	else 
	{
		if (length % 2 == 0)
		{
			length = length / 2;
		}
		else
		{
			length = length /2 + 1;
		}
	}	

	length = length * 2;

	*(U8 *)(AD1+0)=0x02;            //ͷENQ����02
	*(U8 *)(AD1+1)=0x72;  			//plcվ��ַ����λ��ǰ
	*(U8 *)(AD1+2)=0x4d;
	*(U8 *)(AD1+3)=asicc((b3>>4)&0xf);		//��ַ
	*(U8 *)(AD1+4)=asicc((b3>>0)&0xf);		
	
	switch (ptcom->registerr)	//���ݼĴ������ͷ��Ͳ�ͬ�������ַ�
	{
	case 'Y':
		*(U8 *)(AD1+5)=0x35;        //�Ĵ�������
		*(U8 *)(AD1+6)=0x43;
		break;
				
	case 'M':
		*(U8 *)(AD1+5)=0x35;        //�Ĵ�������
		*(U8 *)(AD1+6)=0x44; 
		break;	
		
	case 'L':						//L
		*(U8 *)(AD1+5)=0x35;        //�Ĵ�������
		*(U8 *)(AD1+6)=0x46;
		break;
		
	case 'K':						//K
		*(U8 *)(AD1+5)=0x36;        //�Ĵ�������
		*(U8 *)(AD1+6)=0x30;
		break;							
	}
	
	*(U8 *)(AD1+7)=asicc((length>>12)&0xf);		//len
	*(U8 *)(AD1+8)=asicc((length>>8)&0xf);	
	*(U8 *)(AD1+9)=asicc((length>>4)&0xf);		//len
	*(U8 *)(AD1+10)=asicc((length>>0)&0xf);		
	
		
	aakj=CalcHe((U8 *)AD1+1,10);      //��У�飬У��ǰ�������
	a1=aakj&0xff;                   //ֻ�����λ
	*(U8 *)(AD1+11)=asicc((a1>>4)&0xf);		//check
	*(U8 *)(AD1+12)=asicc((a1>>0)&0xf);	
	*(U8 *)(AD1+13)=0x03;			//END

	ptcom->send_length[0]=14;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
	ptcom->send_times=1;					//���ʹ���
			
			
	ptcom->return_length[0]=5+length*2;	//�������ݳ��ȣ���5���̶�
	ptcom->return_start[0]=2;				//����������Ч��ʼ����2��
	ptcom->return_length_available[0]=length*2;	//������Ч���ݳ���	
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	ptcom->send_add[0]=ptcom->address;		//�����������ַ������	
	ptcom->address=b1 * 16;				//������ʵ���ݵ�ַ	
	ptcom->register_length = length;
	ptcom->needShake=0;
}


void Read_Analog()				//��ģ����
{
	U16 aakj;
	int b;
	int a1;
	int length;
	int b3;
	
	b=ptcom->address;			    // �Ĵ���������ַ
	length=ptcom->register_length;  //�����ĳ���
	length = length *2;

	b3 = b * 2;
		

	*(U8 *)(AD1+0)=0x02;            //ͷENQ����02
	*(U8 *)(AD1+1)=0x72;  			//plcվ��ַ����λ��ǰ
	*(U8 *)(AD1+2)=0x4d;
	*(U8 *)(AD1+3)=asicc((b3>>4)&0xf);		//��ַ
	*(U8 *)(AD1+4)=asicc((b3>>0)&0xf);		
	
	*(U8 *)(AD1+5)=asicc((b3>>12)&0xf);		//��ַ
	*(U8 *)(AD1+6)=asicc((b3>>8)&0xf);
						

	
	*(U8 *)(AD1+7)=asicc((length>>12)&0xf);		//len
	*(U8 *)(AD1+8)=asicc((length>>8)&0xf);	
	*(U8 *)(AD1+9)=asicc((length>>4)&0xf);		//len
	*(U8 *)(AD1+10)=asicc((length>>0)&0xf);		
	
		
	aakj=CalcHe((U8 *)AD1+1,10);      //��У�飬У��ǰ�������
	a1=aakj&0xff;                   //ֻ�����λ
	*(U8 *)(AD1+11)=asicc((a1>>4)&0xf);		//check
	*(U8 *)(AD1+12)=asicc((a1>>0)&0xf);	
	*(U8 *)(AD1+13)=0x03;			//END

	ptcom->send_length[0]=14;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
	ptcom->send_times=1;					//���ʹ���
			
	ptcom->return_length[0]=5+length*2;	//�������ݳ��ȣ���5���̶�
	ptcom->return_start[0]=2;				//����������Ч��ʼ����2��
	ptcom->return_length_available[0]=length*2;	//������Ч���ݳ���	
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	ptcom->send_add[0]=ptcom->address;		//�����������ַ������	
	
	ptcom->needShake=1;
}


void Read_Recipe()								//��ȡ�䷽
{
	U16 aakj;
	int b;
	int a1;
	int i;
	int datalength;
	int p_start;
	int ps;
	int SendTimes;
	int LastTimeWord;							//���һ�η��ͳ���
	int currentlength;
	int length;

	
		
	datalength=ptcom->register_length;		//�����ܳ���
	p_start=ptcom->address;					//��ʼ��ַ
	length=ptcom->register_length;          //���ĳ��ȣ����ٸ��䷽

	if(datalength>5000)                     //���Ƴ���
		datalength=5000;

	if(datalength%16==0)                    //ÿ�η�16���䷽�������䷽���պ�Ϊ16�ı���ʱ
	{
		SendTimes=datalength/16;            //���͵Ĵ���            
		LastTimeWord=16;                    //���һ�η��͵ĳ���Ϊ16	
	}
	if(datalength%16!=0)                    //ÿ�η�16���䷽�������䷽������16�ı���ʱ
	{
		SendTimes=datalength/16+1;          //���͵Ĵ���
		LastTimeWord=datalength%16;         //���һ�η��͵ĳ���Ϊ��16������	
	}
		
	for (i=0;i<SendTimes;i++)               //С�ڷ��ʹ�����������
	{
		ps=14*i;                            //ÿ�η�19�����ȣ��ڶ��η�����19��ʼ
		b=p_start+i*16;                     //ÿ�εĿ�ʼ��ַ
		b=b*2;

		
		*(U8 *)(AD1+0+ps)=0x02;            //ͷENQ����02
		*(U8 *)(AD1+1+ps)=0x72;  			//plcվ��ַ����λ��ǰ
		*(U8 *)(AD1+2+ps)=0x4d;
		*(U8 *)(AD1+3+ps)=asicc((b>>4)&0xf);		//��ַ
		*(U8 *)(AD1+4+ps)=asicc((b>>0)&0xf);		
		
		*(U8 *)(AD1+5+ps)=asicc((b>>12)&0xf);		//��ַ
		*(U8 *)(AD1+6+ps)=asicc((b>>8)&0xf);	
							
		if (i!=(SendTimes-1))	          //�������һ�η���ʱ
		{
			*(U8 *)(AD1+7+ps)=asicc((32>>12)&0xf);		//len
			*(U8 *)(AD1+8+ps)=asicc((32>>8)&0xf);	
			*(U8 *)(AD1+9+ps)=asicc((32>>4)&0xf);		//len
			*(U8 *)(AD1+10+ps)=asicc((32>>0)&0xf);		
			currentlength=16;             //���͵����ݳ���
		}
		if (i==(SendTimes-1))	          //���һ�η���ʱ
		{
			*(U8 *)(AD1+7+ps)=asicc((LastTimeWord*2>>12)&0xf);		//len
			*(U8 *)(AD1+8+ps)=asicc((LastTimeWord*2>>8)&0xf);	
			*(U8 *)(AD1+9+ps)=asicc((LastTimeWord*2>>4)&0xf);		//len
			*(U8 *)(AD1+10+ps)=asicc((LastTimeWord*2>>0)&0xf);		
			currentlength=LastTimeWord;   //���͵����ݳ���
		}		
			
		aakj=CalcHe((U8 *)AD1+ps+1,10);      //��У�飬У��ǰ�������
		a1=aakj&0xff;                   //ֻ�����λ
		*(U8 *)(AD1+11+ps)=asicc((a1>>4)&0xf);		//check
		*(U8 *)(AD1+12+ps)=asicc((a1>>0)&0xf);	
		*(U8 *)(AD1+13+ps)=0x03;			//END
	
		
		ptcom->send_length[i]=14;				//���ͳ���
		ptcom->send_staradd[i]=i*14;			//�������ݴ洢��ַ	
		ptcom->send_add[i]=p_start+i*16;		//�����������ַ������	
		ptcom->send_data_length[i]=currentlength;//�������һ�ζ���16��
				
		ptcom->return_length[i]=5+currentlength*4;	//�������ݳ��ȣ���11���̶�
		ptcom->return_start[i]=2;				    //����������Ч��ʼ
		ptcom->return_length_available[i]=currentlength*4;	//������Ч���ݳ���			
	}

	ptcom->Current_Times=0;					        //��ǰ���ʹ���
	ptcom->needShake=1;		
	ptcom->send_times=SendTimes;					//���ʹ���		

}

void Write_Analog()								//дģ����
{
	U16 aakj;
	int b,i;
	int a1;
	int length;
	int plcadd;
	int c,c0,c1,c2,c3,c4;
	int b3;
	int iSend_Len;
			
	b=ptcom->address;			    //��ʼ��ַ
	plcadd=ptcom->plc_address;	    //PLCվ��ַ
	length=ptcom->register_length;  //д���䷽��
	
	b3 = b * 2;
	
	*(U8 *)(AD1+0)=0x02;            //ͷSTX����02
	*(U8 *)(AD1+1)=0x77; 			//plcվ��ַ����λ��ǰ
	*(U8 *)(AD1+2)=0x4d;
	
	*(U8 *)(AD1+3)=asicc((b3>>4)&0xf);		//��ַ
	*(U8 *)(AD1+4)=asicc((b3>>0)&0xf);		
	
	*(U8 *)(AD1+5)=asicc((b3>>12)&0xf);		//��ַ
	*(U8 *)(AD1+6)=asicc((b3>>8)&0xf);	
	
	iSend_Len = length*2;
	*(U8 *)(AD1+7)=asicc((iSend_Len>>12)&0xf);		//len
	*(U8 *)(AD1+8)=asicc((iSend_Len>>8)&0xf);	
	*(U8 *)(AD1+9)=asicc((iSend_Len>>4)&0xf);		//len
	*(U8 *)(AD1+10)=asicc((iSend_Len>>0)&0xf);		
	
	for(i=0;i<length;i++)
	{			
		c0=ptcom->U8_Data[i*2];   //��D[]������Ҫ���ݣ���Ӧ��c0Ϊ��λ��cΪ��λ
		c=ptcom->U8_Data[i*2+1];
							
		c1=c&0xf;                //����д������asicc����cΪ��λ
		c2=(c>>4)&0xf;           //����д������asicc����cΪ��λ		
		c3=c0&0xf;               //����д������asicc����c1Ϊ��λ
		c4=(c0>>4)&0xf;          //����д������asicc����c1Ϊ��λ				
		
		*(U8 *)(AD1+11+i*4)=asicc(c4);        //�����Ǹ�λ�ȷ�
		*(U8 *)(AD1+12+i*4)=asicc(c3);
		*(U8 *)(AD1+13+i*4)=asicc(c2);
		*(U8 *)(AD1+14+i*4)=asicc(c1);	
	}
	
	aakj=CalcHe((U8 *)AD1+1,10+i*4);      //��У�飬У��ǰ�������
	a1=aakj&0xff;                   //ֻ�����λ
	*(U8 *)(AD1+11+length*4)=asicc((a1>>4)&0xf);		//check
	*(U8 *)(AD1+12+length*4)=asicc((a1>>0)&0xf);	
	*(U8 *)(AD1+13+length*4)=0x03;			//END

	ptcom->send_length[0]=14+length*4;		//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
	ptcom->send_times=1;					//���ʹ���
			
	ptcom->return_length[0]=5;				//�������ݳ���
	ptcom->return_start[0]=0;				//����������Ч��ʼ
	ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
}

void Write_Time()                                //��PLCдʱ��  
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
	int b1,b2,b3,b4;
	int c1,c2,c3,c4;
	U16 aakj;
	int plcadd;
	int length;
	int addr;
		
	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));//���ݳ���
	staradd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//���ݳ���

	if(datalength%16==0)                           //д���䷽���պ���16�ı���ʱ             
	{
		SendTimes=datalength/16;                   //���͵Ĵ���
		LastTimeWord=16;                           //���һ�η��͵ĳ���	
	}
	if(datalength%16!=0)                           //����16�ı���ʱ
	{
		SendTimes=datalength/16+1;                 //���͵Ĵ���
		LastTimeWord=datalength%16;                //���һ�η��͵ĳ���	
	}	
	
	ps=78;                                         //д16���䷽ʱ�����͵���83������
	for (i=0;i<SendTimes;i++)
	{
		addr = staradd + i*16;
		
		if (i!=(SendTimes-1))                      //�������һ�η���ʱ
		{	
			length=16;                             //��16��
		}
		else                                       //���һ�η���ʱ
		{
			length=LastTimeWord;                   //��ʣ����䷽��             
		}

		*(U8 *)(AD1+0+ps*i)=0x02;            //ͷSTX����02
		*(U8 *)(AD1+1+ps*i)=0x77; 			//plcվ��ַ����λ��ǰ
		*(U8 *)(AD1+2+ps*i)=0x4d;
		
		*(U8 *)(AD1+3+ps*i)=asicc((addr*2>>4)&0xf);		//��ַ
		*(U8 *)(AD1+4+ps*i)=asicc((addr*2>>0)&0xf);		
		
		*(U8 *)(AD1+5+ps*i)=asicc((addr*2>>12)&0xf);		//��ַ
		*(U8 *)(AD1+6+ps*i)=asicc((addr*2>>8)&0xf);	
		

		*(U8 *)(AD1+7+ps*i)=asicc((length*2>>12)&0xf);		//len
		*(U8 *)(AD1+8+ps*i)=asicc((length*2>>8)&0xf);	
		*(U8 *)(AD1+9+ps*i)=asicc((length*2>>4)&0xf);		//len
		*(U8 *)(AD1+10+ps*i)=asicc((length*2>>0)&0xf);	
		
		for(j=0;j<length;j++)
		{
			b4=*(U8 *)(PE+9+i*32+j*2);         //��������ȡ���ݣ�B3�ǵ�λ��B4�Ǹ�λ
			b3=*(U8 *)(PE+9+i*32+j*2+1);
				
			c1=b3&0xf;               //����д������asicc����
			c2=(b3>>4)&0xf;          //����д������asicc����		
			c3=b4&0xf;               //����д������asicc����
			c4=(b4>>4)&0xf;          //����д������asicc����				
		
			*(U8 *)(AD1+11+j*4+ps*i)=asicc(c4);   //��λ�ȷ�
			*(U8 *)(AD1+12+j*4+ps*i)=asicc(c3);
			*(U8 *)(AD1+13+j*4+ps*i)=asicc(c2);
			*(U8 *)(AD1+14+j*4+ps*i)=asicc(c1);				
		}
		aakj=CalcHe((U8 *)AD1+1+ps*i,10+j*4);      //��У�飬У��ǰ�������
		a1=aakj&0xff;                   //ֻ�����λ
		*(U8 *)(AD1+11+length*4+ps*i)=asicc((a1>>4)&0xf);		//check
		*(U8 *)(AD1+12+length*4+ps*i)=asicc((a1>>0)&0xf);	
		*(U8 *)(AD1+13+length*4+ps*i)=0x03;			//END

		ptcom->send_length[i]=14+length*4;				//���ͳ���
		ptcom->send_staradd[i]=i*ps;			//�������ݴ洢��ַ	
		ptcom->return_length[i]=5;				//�������ݳ���
		ptcom->return_start[i]=0;				//����������Ч��ʼ
		ptcom->return_length_available[i]=0;	//������Ч���ݳ���	
			
	}
	ptcom->send_times=SendTimes;					//���ʹ���
	ptcom->Current_Times=0;					//��ǰ���ʹ���		
}


void compxy(void)				//����ɱ�׼�洢��ʽ
{
	int i;
	unsigned char a1,a2,a3,a4;
	int b,b1;
	if(ptcom->needShake==1)                                //KK==0ʱ���Է����������ߵ�λ����
	{                                  
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/4;i++)	//���ص����ó���/4������Ҫ�ĳ���
 		 {		
  		    a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4);
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+1);
			a3=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+2);
			a4=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+3);	
			a1=bsicc(a1);
			a2=bsicc(a2);
			a3=bsicc(a3);
			a4=bsicc(a4);                       //ASC�귵�أ�����ҪתΪ16����
			b=(a1<<4)+a2;                       //���ص�ǰ����
			b1=(a3<<4)+a4;                      //���صĺ���
			*(U8 *)(COMad+i*2)=b1;				//���´�,�ӵ�0����ʼ�棬�����ߵ�λ
			*(U8 *)(COMad+i*2+1)=b;	
		}
	}
	else                                      //�Է����������ߵ�λ����
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/4;i++)	//���ص����ó���/4������Ҫ�ĳ���
		{
		a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4);
		a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+1);
		a3=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+2);
		a4=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+3);	
		a1=bsicc(a1);                           //ASC�귵�أ�����ҪתΪ16����
		a2=bsicc(a2);
		a3=bsicc(a3);
		a4=bsicc(a4);
		b=(a1<<4)+a2;                          //���ص�ǰ����
		b1=(a3<<4)+a4;                         //���صĺ�����
		*(U8 *)(COMad+i*2)=b;				   //���´�,�ӵ�0����ʼ�棬�ߵ�λ����
		*(U8 *)(COMad+i*2+1)=b1;	
		}
	}			
	ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;	//���ȼ���	
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

int remark()				//�����������ݼ���У�����Ƿ���ȷ��LG���ص���û��У�飬�ʲ������Ƿ���ȷ��return 1
{
	unsigned int aakj1,aakj2;
	unsigned int akj1;
	unsigned int a1,a2;	
	aakj1=*(U8 *)(COMad+ptcom->return_length[ptcom->Current_Times-1]-2)&0xff;	
	aakj2=*(U8 *)(COMad+ptcom->return_length[ptcom->Current_Times-1]-3)&0xff;
				
	akj1=CalcHe((U8 *)COMad+1,ptcom->return_length[ptcom->Current_Times-1]-2);
	a1=asicc((akj1>>4)&0xf);		//check
	a2=asicc((akj1>>0)&0xf);	
	if(aakj1==a1 && aakj2 == a2)
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
