#include "stdio.h"
#include "def.h"
#include "smm.h"

struct Com_struct_D *ptcom;

unsigned char CHECK[100];
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Enter_PlcDriver(void)//�ж����������Ƿ���ȷ
{
	int result;
	int IfOK;	
	LB=*(U32 *)(LB_Address+0);
	LW=*(U32 *)(LW_Address+0);
	RWI=*(U32 *)(RWI_Address+0);	
	ptcom=(struct Com_struct_D *)adcom;
		
	result=checkCorrect();			//��������Ƿ���ȷ	
	
	if (result == 1)				//������ȷ
	{	
		IfOK=senddata();
		
		if (IfOK==1)				//�ɹ���ȷ
		{
			ptcom->IfResultCorrect=1;
		}
	}	 
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int checkCorrect()//��������Ƿ���ȷ	
{	
	unsigned char LRC_high;
	unsigned char LRC_low;
	unsigned char LRC_high_return;
	unsigned char LRC_low_return;
	int Temp,Temp1,Temp2;
	int i;
	U16 Check;	

	LRC_low_return=*(U8 *)(COMad+ptcom->IQ-3);					//ȡ���յ��Ĵ��봮��LRCУ���λ
	LRC_high_return=*(U8 *)(COMad+ptcom->IQ-4);					//ȡ���յ��Ĵ��봮��LRCУ���λ
	LRC_low_return=bsicc(LRC_low_return);
	LRC_high_return=bsicc(LRC_high_return);

	for (i=0;i<(ptcom->IQ-5)/2;i++)								//��ȥ5������У��ģ�����2��ASC��ϳ�һ���ֽ�
	{
		Temp1=bsicc(*(U8 *)(COMad+i*2+1));
		Temp2=bsicc(*(U8 *)(COMad+i*2+2));
		Temp=((Temp1<<4)+Temp2)&0xff;
		CHECK[i]=Temp;
	}

	Check=CalLRC(CHECK,(ptcom->IQ-5)/2);						//�����յ��Ĵ��봮��У��
	LRC_high=((Check&0xf0)>>4)&0xf;								//����У������λ
	LRC_low=Check&0xf;											//��λ

	if (LRC_low_return == LRC_low && LRC_high_return == LRC_high)//�ߵ�λУ��һ�����յ���������ȷ
	{
		Temp1=bsicc(*(U8 *)(COMad+1));							//��2��3������վ��ַ
		Temp2=bsicc(*(U8 *)(COMad+2));
		Temp=((Temp1<<4)+Temp2)&0xff;
		*((U16 *)LW + 10)= ptcom->R_W_Flag;
		*((U16 *)LW + 11)= Temp;
		
		*((U16 *)LW + 12)= 10;//*(U8 *)(COMad+ptcom->IQ-3);
		*((U16 *)LW + 13)= *(U8 *)(COMad+ptcom->IQ-4);
		
		*((U16 *)LW + 14)= *(U8 *)(COMad+1);
		*((U16 *)LW + 15)= *(U8 *)(COMad+2);

		if (ptcom->R_W_Flag == Temp || Temp==0)					//վ��ַ��HMIվ��ַһ��,���߹㲥ģʽ
		{
			return 1;											//����1��ʾ��ȷ
		}
		else
		{
			return 0;											//����0��ʾ�յ�������ȷ��ֱ���˳�����
		}			
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int senddata()//�������ݵ�����
{
	int result=0;
	int Temp,Temp1,Temp2;
	
	Temp1=bsicc(*(U8 *)(COMad+3));								//��4��5������ʾ������
	Temp2=bsicc(*(U8 *)(COMad+4));
	Temp=((Temp1<<4)+Temp2)&0xff;
	
		
	switch (Temp)												//���ݹ����벻ͬ���벻ͬ�����ݴ�����
	{
		case 01:												//��ȡLB������
		case 02:												//��ȡLB������	
			result=slave_send_LB();	
			break;
		case 03:												//��ȡLW������
		case 04:												//��ȡLW������
			result=slave_send_LW();		
			break;	
		case 05:												//����LB	
			result=slave_write_LB();
			break;
		case 0x06:												//д�뵥��LW	
		case 0x10:												//д������LW		
			result=slave_write_LW();
			break;													
	}
	return result;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int slave_send_LB()//����LB�����ݵ���վ
{
	U8 staradd_low_L;
	U8 staradd_low_H;
	U8 staradd_high_L;	
	U8 staradd_high_H;
	U16 staradd;
	U8 length_low_L;
	U8 length_low_H;
	U8 length_high_L;
	U8 length_high_H;
	U16 length;
	int i,j;
	U8 charGroup[255];
	unsigned char Result_LRC;
	unsigned char Result_LRC_L;
	unsigned char Result_LRC_H;
	int Temp_1,Temp_2;
	
	staradd_high_H=bsicc(*(U8 *)(COMad+5));							//��ַ���λ���������µ����λ
	staradd_high_L=bsicc(*(U8 *)(COMad+6));
	staradd_low_H=bsicc(*(U8 *)(COMad+7));
	staradd_low_L=bsicc(*(U8 *)(COMad+8));
	staradd=(staradd_high_H<<12)+(staradd_high_L<<8)+(staradd_low_H<<4)+staradd_low_L;
	
	length_high_H=bsicc(*(U8 *)(COMad+9));							//��ȡ�������λ���������µ����λ
	length_high_L=bsicc(*(U8 *)(COMad+10));
	length_low_H=bsicc(*(U8 *)(COMad+11));
	length_low_L=bsicc(*(U8 *)(COMad+12));
	length=(length_high_H<<12)+(length_high_L<<8)+(length_low_H<<4)+length_low_L;
	
	if ((staradd+length)>=4096)										//��ַ������Χ
	{
		return 0;
	}
	if (length%8==0)												//�պ�����
	{
		length=length/8;
	}
	else if (length%8!=0)											//������
	{
		length=length/8+1;
	}	
		
	for (i=0;i<length;i++)											//����
	{
		charGroup[i]=0;												//����ϴ�����
		for (j=0;j<8;j++)
		{
			charGroup[i]=(((*(U8 *)(LB+i*8+j+staradd))&0x01)<<j)+charGroup[i];
		}	
	}
//׼����������
	*(U8 *)(AD1+0)=0x3a;											//��ʼ
	*(U8 *)(AD1+1)=*(U8 *)(COMad+1);								//վ��ַ	
	*(U8 *)(AD1+2)=*(U8 *)(COMad+2);
	*(U8 *)(AD1+3)=*(U8 *)(COMad+3);								//������	
	*(U8 *)(AD1+4)=*(U8 *)(COMad+4);
	*(U8 *)(AD1+5)=asicc((length>>4)&0xf);							//����	
	*(U8 *)(AD1+6)=asicc(length&0xf);	

	for (i=0;i<length;i++)											//����
	{
		*(U8 *)(AD1+7+i*2)=asicc((charGroup[i]>>4)&0xf);		
		*(U8 *)(AD1+8+i*2)=asicc(charGroup[i]&0xf);		
	}
	
	//��ʼУ��	
	Temp_1=bsicc(*(U8 *)(COMad+1));
	Temp_2=bsicc(*(U8 *)(COMad+2));
	CHECK[0]=(Temp_1<<4)+Temp_2;
	Temp_1=bsicc(*(U8 *)(COMad+3));
	Temp_2=bsicc(*(U8 *)(COMad+4));	
	CHECK[1]=(Temp_1<<4)+Temp_2;	
	CHECK[2]=length;
	for (i=0;i<length;i++)
	{
		CHECK[3+i]=charGroup[i];		
	}	
	Result_LRC=CalLRC(CHECK,3+length);								//LRCУ��
	Result_LRC_H=(Result_LRC>>4)&0xf;
	Result_LRC_L=Result_LRC&0xf;
	*(U8 *)(AD1+7+length*2)=asicc(Result_LRC_H);          			//У�� ��λ
	*(U8 *)(AD1+8+length*2)=asicc(Result_LRC_L);          			//У�� ��λ
	
	*(U8 *)(AD1+9+length*2)=0x0d;									//ASCIIģʽ�����ַ����س�CR������LF
	*(U8 *)(AD1+10+length*2)=0x0a;		
	
	ptcom->send_length[0]=11+length*2;								//���ͳ���
	ptcom->send_staradd[0]=0;										//�������ݴ洢��ַ	
	ptcom->send_times=1;											//���ʹ���
			
	ptcom->return_length[0]=0;										//�������ݳ���
	ptcom->return_start[0]=0;										//����������Ч��ʼ
	ptcom->return_length_available[0]=0;							//������Ч���ݳ���	
	ptcom->Current_Times=0;											//��ǰ���ʹ���	
	return 1;
}
/*************************************************************************************************************************
Э��涨LW��Modbus��ַ��0-4095
RWI�ĵ�ַ��4096-8191
*************************************************************************************************************************/
int slave_send_LW()//����LW�����ݵ���վ
{
	U8 staradd_low_L;
	U8 staradd_low_H;
	U8 staradd_high_L;	
	U8 staradd_high_H;
	U16 staradd;
	U8 length_low_L;
	U8 length_low_H;
	U8 length_high_L;
	U8 length_high_H;
	U16 length;
	int i;
	U8 charGroup[255];
	unsigned char Result_LRC;
	unsigned char Result_LRC_L;
	unsigned char Result_LRC_H;
	int Temp_1,Temp_2;

	staradd_high_H=bsicc(*(U8 *)(COMad+5));							//��ַ���λ���������µ����λ
	staradd_high_L=bsicc(*(U8 *)(COMad+6));
	staradd_low_H=bsicc(*(U8 *)(COMad+7));
	staradd_low_L=bsicc(*(U8 *)(COMad+8));
	staradd=(staradd_high_H<<12)+(staradd_high_L<<8)+(staradd_low_H<<4)+staradd_low_L;
	
	length_high_H=bsicc(*(U8 *)(COMad+9));							//��ȡ�������λ���������µ����λ
	length_high_L=bsicc(*(U8 *)(COMad+10));
	length_low_H=bsicc(*(U8 *)(COMad+11));
	length_low_L=bsicc(*(U8 *)(COMad+12));
	length=(length_high_H<<12)+(length_high_L<<8)+(length_low_H<<4)+length_low_L;

	if ((staradd+length)>8191)										//��ַ������Χ
	{
		return 0;
	}	

	if (staradd<4096)												//������LW������
	{
		for (i=0;i<length;i++)										//����
		{
			charGroup[i*2+0]=((*(U16 *)(LW+(staradd+i)*2))>>8)&0xff;		
			charGroup[i*2+1]=((*(U16 *)(LW+(staradd+i)*2)))&0xff;				
		}	
	}
	else if (staradd>=4096)											//������RWI������
	{
		staradd=staradd-4096;
		for (i=0;i<length;i++)										//����
		{
			charGroup[i*2+0]=((*(U16 *)(RWI+(staradd+i)*2))>>8)&0xff;		
			charGroup[i*2+1]=((*(U16 *)(RWI+(staradd+i)*2)))&0xff;			
		}
	}
//׼����������	
	*(U8 *)(AD1+0)=0x3a;											//��ʼ
	*(U8 *)(AD1+1)=*(U8 *)(COMad+1);								//վ��ַ	
	*(U8 *)(AD1+2)=*(U8 *)(COMad+2);
	*(U8 *)(AD1+3)=*(U8 *)(COMad+3);								//������	
	*(U8 *)(AD1+4)=*(U8 *)(COMad+4);
	*(U8 *)(AD1+5)=asicc((length*2>>4)&0xf);						//����	
	*(U8 *)(AD1+6)=asicc(length*2&0xf);

	for (i=0;i<length;i++)											//����
	{
		*(U8 *)(AD1+7+i*4)=asicc((charGroup[i*2]>>4)&0xf);		
		*(U8 *)(AD1+8+i*4)=asicc(charGroup[i*2]&0xf);
		*(U8 *)(AD1+9+i*4)=asicc((charGroup[i*2+1]>>4)&0xf);		
		*(U8 *)(AD1+10+i*4)=asicc(charGroup[i*2+1]&0xf);		
	}

	//��ʼУ��	
	Temp_1=bsicc(*(U8 *)(COMad+1));
	Temp_2=bsicc(*(U8 *)(COMad+2));
	CHECK[0]=(Temp_1<<4)+Temp_2;
	Temp_1=bsicc(*(U8 *)(COMad+3));
	Temp_2=bsicc(*(U8 *)(COMad+4));	
	CHECK[1]=(Temp_1<<4)+Temp_2;	
	CHECK[2]=length*2;
	for (i=0;i<length*2;i++)
	{
		CHECK[3+i]=charGroup[i];		
	}	
	Result_LRC=CalLRC(CHECK,3+length*2);							//LRCУ��
	Result_LRC_H=(Result_LRC>>4)&0xf;
	Result_LRC_L=Result_LRC&0xf;
	*(U8 *)(AD1+7+length*4)=asicc(Result_LRC_H);          			//У�� ��λ
	*(U8 *)(AD1+8+length*4)=asicc(Result_LRC_L);          			//У�� ��λ	
	
	*(U8 *)(AD1+9+length*4)=0x0d;									//ASCIIģʽ�����ַ����س�CR������LF
	*(U8 *)(AD1+10+length*4)=0x0a;	

	ptcom->send_length[0]=11+length*4;								//���ͳ���
	ptcom->send_staradd[0]=0;										//�������ݴ洢��ַ	
	ptcom->send_times=1;											//���ʹ���
			
	ptcom->return_length[0]=0;										//�������ݳ���
	ptcom->return_start[0]=0;										//����������Ч��ʼ
	ptcom->return_length_available[0]=0;							//������Ч���ݳ���	
	ptcom->Current_Times=0;											//��ǰ���ʹ���	
	return 1;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int slave_write_LB()//����LB����ֵ
{
	int Finish=0;
	U8 staradd_low_L;
	U8 staradd_low_H;
	U8 staradd_high_L;	
	U8 staradd_high_H;
	U16 staradd;
	U8 WriteValue_low_L;
	U8 WriteValue_low_H;
	U8 WriteValue_high_L;	
	U8 WriteValue_high_H;
	U8 WriteValue_low;	
	U8 WriteValue_high;
	int i;
	
	staradd_high_H=bsicc(*(U8 *)(COMad+5));							//��ַ���λ���������µ����λ
	staradd_high_L=bsicc(*(U8 *)(COMad+6));
	staradd_low_H=bsicc(*(U8 *)(COMad+7));
	staradd_low_L=bsicc(*(U8 *)(COMad+8));
	staradd=(staradd_high_H<<12)+(staradd_high_L<<8)+(staradd_low_H<<4)+staradd_low_L;

	WriteValue_high_H=bsicc(*(U8 *)(COMad+9));						//д���ֵ������λ/��λ
	WriteValue_high_L=bsicc(*(U8 *)(COMad+10));
	WriteValue_high=(WriteValue_high_H<<4)+WriteValue_high_L;		//��λ
	WriteValue_low_H=bsicc(*(U8 *)(COMad+11));
	WriteValue_low_L=bsicc(*(U8 *)(COMad+12));
	WriteValue_low=(WriteValue_low_H<<4)+WriteValue_low_L;			//��λ
	
	if (staradd>=4096)												//��ַ������Χ
	{
		return 0;
	}

	if (WriteValue_high == 0xff && WriteValue_low == 0x00)			//��λ
	{
		*(U8 *)(LB+staradd)=1;
		Finish=1;
	}
	else if (WriteValue_high == 0x00 && WriteValue_low == 0x00)		//��λ
	{
		*(U8 *)(LB+staradd)=0;
		Finish=1;
	}
//׼����������
	if (Finish==1)													//д�����ֵ����ȷ��
	{
		for (i=0;i<17;i++)
		{
			*(U8 *)(AD1+i)=*(U8 *)(COMad+i);
		}
		
		ptcom->send_length[0]=17;									//���ͳ���
		ptcom->send_staradd[0]=0;									//�������ݴ洢��ַ	
		ptcom->send_times=1;										//���ʹ���
				
		ptcom->return_length[0]=0;									//�������ݳ���
		ptcom->return_start[0]=0;									//����������Ч��ʼ
		ptcom->return_length_available[0]=0;						//������Ч���ݳ���	
		ptcom->Current_Times=0;										//��ǰ���ʹ���	
		return 1;	
	}
	else
	{
		return 0;
	}
}
/*************************************************************************************************************************
Э��涨LW��Modbus��ַ��0-4095
RWI�ĵ�ַ��4096-8191
*************************************************************************************************************************/
int slave_write_LW()//����LW����ֵ
{
	U8 staradd_low_L;
	U8 staradd_low_H;
	U8 staradd_high_L;	
	U8 staradd_high_H;
	U16 staradd;
	U8 Length_low_L;
	U8 Length_low_H;
	U8 Length_high_L;	
	U8 Length_high_H;
	U16 Length;
	U16 Length_Byte;	
	int i,k;
	U8 WriteValue_low_L;
	U8 WriteValue_low_H;
	U8 WriteValue_high_L;	
	U8 WriteValue_high_H;
	U8 WriteValue_low;	
	U8 WriteValue_high;
	unsigned char Result_LRC;
	unsigned char Result_LRC_L;
	unsigned char Result_LRC_H;
	int Temp_1,Temp_2;
	
	if (*(U8 *)(COMad+3) == 0x31 && *(U8 *)(COMad+4) == 0x30)		//д���ֽ�
	{
		Length_high_H=bsicc(*(U8 *)(COMad+9));						//���ȣ��������µ����λ
		Length_high_L=bsicc(*(U8 *)(COMad+10));
		Length_low_H=bsicc(*(U8 *)(COMad+11));
		Length_low_L=bsicc(*(U8 *)(COMad+12));
		Length=(Length_high_H<<12)+(Length_high_L<<8)+(Length_low_H<<4)+Length_low_L;
		Length_Byte=(bsicc(*(U8 *)(COMad+13))<<4)+bsicc(*(U8 *)(COMad+14));
		if ((Length*2) != Length_Byte)								//�ֽڵ�λ���ֵ�λ��ƥ��
		{
			return 0;
		}
	}

	staradd_high_H=bsicc(*(U8 *)(COMad+5));							//��ַ���λ���������µ����λ
	staradd_high_L=bsicc(*(U8 *)(COMad+6));
	staradd_low_H=bsicc(*(U8 *)(COMad+7));
	staradd_low_L=bsicc(*(U8 *)(COMad+8));
	staradd=(staradd_high_H<<12)+(staradd_high_L<<8)+(staradd_low_H<<4)+staradd_low_L;

	if (*(U8 *)(COMad+3) == 0x30 && *(U8 *)(COMad+4) == 0x36)		//д�뵥���Ĵ���
	{
		Length=1;
		k=0;
	}
	else if (*(U8 *)(COMad+3) == 0x31 && *(U8 *)(COMad+4) == 0x30)	//д���ֽ�
	{
		Length=Length;
		k=6;
	}

	if((staradd+Length)>=8192)										//��ַ������Χ
	{
		return 0;	
	}	

	if(staradd<4096)												//LW
	{
		for (i=0;i<Length;i++)
		{
			WriteValue_high_H=bsicc(*(U8 *)(COMad+9+k+i*4));		//д���ֵ������λ/��λ
			WriteValue_high_L=bsicc(*(U8 *)(COMad+10+k+i*4));
			WriteValue_high=(WriteValue_high_H<<4)+WriteValue_high_L;//��λ
			WriteValue_low_H=bsicc(*(U8 *)(COMad+11+k+i*4));
			WriteValue_low_L=bsicc(*(U8 *)(COMad+12+k+i*4));
			WriteValue_low=(WriteValue_low_H<<4)+WriteValue_low_L;	//��λ			
			*(U16 *)(LW+(staradd+i)*2)=(WriteValue_high<<8)+WriteValue_low;
		}		
	}
	else if(staradd>=4096)											//RWI
	{
		staradd=staradd-4096;
		for (i=0;i<Length;i++)
		{
			WriteValue_high_H=bsicc(*(U8 *)(COMad+9+k+i*4));		//д���ֵ������λ/��λ
			WriteValue_high_L=bsicc(*(U8 *)(COMad+10+k+i*4));
			WriteValue_high=(WriteValue_high_H<<4)+WriteValue_high_L;//��λ
			WriteValue_low_H=bsicc(*(U8 *)(COMad+11+k+i*4));
			WriteValue_low_L=bsicc(*(U8 *)(COMad+12+k+i*4));
			WriteValue_low=(WriteValue_low_H<<4)+WriteValue_low_L;	//��λ			
			*(U16 *)(RWI+(staradd+i)*2)=(WriteValue_high<<8)+WriteValue_low;
		}
	}
//׼����������
	if (*(U8 *)(COMad+3) == 0x30 && *(U8 *)(COMad+4) == 0x36)		//�����������0x06,�򷵻غͷ��ʹ���һ���Ķ���
	{
		for (i=0;i<17;i++)
		{
			*(U8 *)(AD1+i)=*(U8 *)(COMad+i);
		}
		
		ptcom->send_length[0]=17;									//���ͳ���
		ptcom->send_staradd[0]=0;									//�������ݴ洢��ַ	
		ptcom->send_times=1;										//���ʹ���
				
		ptcom->return_length[0]=0;									//�������ݳ���
		ptcom->return_start[0]=0;									//����������Ч��ʼ
		ptcom->return_length_available[0]=0;						//������Ч���ݳ���	
		ptcom->Current_Times=0;										//��ǰ���ʹ���	
		return 1;	
	}
	else if (*(U8 *)(COMad+3) == 0x31 && *(U8 *)(COMad+4) == 0x30)	//��������0x10ʱ��������������
	{
		*(U8 *)(AD1+0)=0x3a;										//��ʼ
		for (i=1;i<13;i++)
		{
			*(U8 *)(AD1+i)=*(U8 *)(COMad+i);
		}
		
		//��ʼУ��
		for (i=0;i<6;i++)
		{
			Temp_1=bsicc(*(U8 *)(COMad+1+i*2));
			Temp_2=bsicc(*(U8 *)(COMad+2+i*2));
			CHECK[i]=(Temp_1<<4)+Temp_2;		
		}
				
		Result_LRC=CalLRC(CHECK,6);									//LRCУ��
		Result_LRC_H=(Result_LRC>>4)&0xf;
		Result_LRC_L=Result_LRC&0xf;
		*(U8 *)(AD1+13)=asicc(Result_LRC_H);          				//У�� ��λ
		*(U8 *)(AD1+14)=asicc(Result_LRC_L);          				//У�� ��λ	
		
		*(U8 *)(AD1+15)=0x0d;										//ASCIIģʽ�����ַ����س�CR������LF
		*(U8 *)(AD1+16)=0x0a;	
		
		ptcom->send_length[0]=17;									//���ͳ���
		ptcom->send_staradd[0]=0;									//�������ݴ洢��ַ	
		ptcom->send_times=1;										//���ʹ���
					
		ptcom->return_length[0]=0;									//�������ݳ���
		ptcom->return_start[0]=0;									//����������Ч��ʼ
		ptcom->return_length_available[0]=0;						//������Ч���ݳ���	
		ptcom->Current_Times=0;										//��ǰ���ʹ���	
		return 1;		
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
