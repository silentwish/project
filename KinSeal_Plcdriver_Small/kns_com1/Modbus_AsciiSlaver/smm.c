#include "stdio.h"
#include "def.h"
#include "smm.h"

struct Com_struct_D *ptcom;

unsigned char CHECK[100];
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Enter_PlcDriver(void)//判断所收数据是否正确
{
	int result;
	int IfOK;	
	LB=*(U32 *)(LB_Address+0);
	LW=*(U32 *)(LW_Address+0);
	RWI=*(U32 *)(RWI_Address+0);	
	ptcom=(struct Com_struct_D *)adcom;
		
	result=checkCorrect();			//检查数据是否正确	
	
	if (result == 1)				//数据正确
	{	
		IfOK=senddata();
		
		if (IfOK==1)				//成功正确
		{
			ptcom->IfResultCorrect=1;
		}
	}	 
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int checkCorrect()//检查数据是否正确	
{	
	unsigned char LRC_high;
	unsigned char LRC_low;
	unsigned char LRC_high_return;
	unsigned char LRC_low_return;
	int Temp,Temp1,Temp2;
	int i;
	U16 Check;	

	LRC_low_return=*(U8 *)(COMad+ptcom->IQ-3);					//取所收到的代码串的LRC校验低位
	LRC_high_return=*(U8 *)(COMad+ptcom->IQ-4);					//取所收到的代码串的LRC校验高位
	LRC_low_return=bsicc(LRC_low_return);
	LRC_high_return=bsicc(LRC_high_return);

	for (i=0;i<(ptcom->IQ-5)/2;i++)								//减去5个不做校验的，并把2个ASC码合成一个字节
	{
		Temp1=bsicc(*(U8 *)(COMad+i*2+1));
		Temp2=bsicc(*(U8 *)(COMad+i*2+2));
		Temp=((Temp1<<4)+Temp2)&0xff;
		CHECK[i]=Temp;
	}

	Check=CalLRC(CHECK,(ptcom->IQ-5)/2);						//计算收到的代码串的校验
	LRC_high=((Check&0xf0)>>4)&0xf;								//计算校验结果高位
	LRC_low=Check&0xf;											//低位

	if (LRC_low_return == LRC_low && LRC_high_return == LRC_high)//高低位校验一样，收到的数据正确
	{
		Temp1=bsicc(*(U8 *)(COMad+1));							//第2，3个数是站地址
		Temp2=bsicc(*(U8 *)(COMad+2));
		Temp=((Temp1<<4)+Temp2)&0xff;
		*((U16 *)LW + 10)= ptcom->R_W_Flag;
		*((U16 *)LW + 11)= Temp;
		
		*((U16 *)LW + 12)= 10;//*(U8 *)(COMad+ptcom->IQ-3);
		*((U16 *)LW + 13)= *(U8 *)(COMad+ptcom->IQ-4);
		
		*((U16 *)LW + 14)= *(U8 *)(COMad+1);
		*((U16 *)LW + 15)= *(U8 *)(COMad+2);

		if (ptcom->R_W_Flag == Temp || Temp==0)					//站地址与HMI站地址一致,或者广播模式
		{
			return 1;											//返回1表示正确
		}
		else
		{
			return 0;											//返回0表示收到数不正确，直接退出驱动
		}			
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int senddata()//发送数据到串口
{
	int result=0;
	int Temp,Temp1,Temp2;
	
	Temp1=bsicc(*(U8 *)(COMad+3));								//第4，5个数表示功能码
	Temp2=bsicc(*(U8 *)(COMad+4));
	Temp=((Temp1<<4)+Temp2)&0xff;
	
		
	switch (Temp)												//根据功能码不同进入不同的数据处理函数
	{
		case 01:												//读取LB的内容
		case 02:												//读取LB的内容	
			result=slave_send_LB();	
			break;
		case 03:												//读取LW的内容
		case 04:												//读取LW的内容
			result=slave_send_LW();		
			break;	
		case 05:												//控制LB	
			result=slave_write_LB();
			break;
		case 0x06:												//写入单个LW	
		case 0x10:												//写入多个个LW		
			result=slave_write_LW();
			break;													
	}
	return result;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int slave_send_LB()//发送LB的内容到主站
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
	
	staradd_high_H=bsicc(*(U8 *)(COMad+5));							//地址最高位，依次往下到最低位
	staradd_high_L=bsicc(*(U8 *)(COMad+6));
	staradd_low_H=bsicc(*(U8 *)(COMad+7));
	staradd_low_L=bsicc(*(U8 *)(COMad+8));
	staradd=(staradd_high_H<<12)+(staradd_high_L<<8)+(staradd_low_H<<4)+staradd_low_L;
	
	length_high_H=bsicc(*(U8 *)(COMad+9));							//读取长度最高位，依次往下到最低位
	length_high_L=bsicc(*(U8 *)(COMad+10));
	length_low_H=bsicc(*(U8 *)(COMad+11));
	length_low_L=bsicc(*(U8 *)(COMad+12));
	length=(length_high_H<<12)+(length_high_L<<8)+(length_low_H<<4)+length_low_L;
	
	if ((staradd+length)>=4096)										//地址超过范围
	{
		return 0;
	}
	if (length%8==0)												//刚好整除
	{
		length=length/8;
	}
	else if (length%8!=0)											//有余数
	{
		length=length/8+1;
	}	
		
	for (i=0;i<length;i++)											//数据
	{
		charGroup[i]=0;												//清楚上次数据
		for (j=0;j<8;j++)
		{
			charGroup[i]=(((*(U8 *)(LB+i*8+j+staradd))&0x01)<<j)+charGroup[i];
		}	
	}
//准备发送数据
	*(U8 *)(AD1+0)=0x3a;											//开始
	*(U8 *)(AD1+1)=*(U8 *)(COMad+1);								//站地址	
	*(U8 *)(AD1+2)=*(U8 *)(COMad+2);
	*(U8 *)(AD1+3)=*(U8 *)(COMad+3);								//功能码	
	*(U8 *)(AD1+4)=*(U8 *)(COMad+4);
	*(U8 *)(AD1+5)=asicc((length>>4)&0xf);							//长度	
	*(U8 *)(AD1+6)=asicc(length&0xf);	

	for (i=0;i<length;i++)											//数据
	{
		*(U8 *)(AD1+7+i*2)=asicc((charGroup[i]>>4)&0xf);		
		*(U8 *)(AD1+8+i*2)=asicc(charGroup[i]&0xf);		
	}
	
	//开始校验	
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
	Result_LRC=CalLRC(CHECK,3+length);								//LRC校验
	Result_LRC_H=(Result_LRC>>4)&0xf;
	Result_LRC_L=Result_LRC&0xf;
	*(U8 *)(AD1+7+length*2)=asicc(Result_LRC_H);          			//校验 高位
	*(U8 *)(AD1+8+length*2)=asicc(Result_LRC_L);          			//校验 低位
	
	*(U8 *)(AD1+9+length*2)=0x0d;									//ASCII模式结束字符，回车CR，换行LF
	*(U8 *)(AD1+10+length*2)=0x0a;		
	
	ptcom->send_length[0]=11+length*2;								//发送长度
	ptcom->send_staradd[0]=0;										//发送数据存储地址	
	ptcom->send_times=1;											//发送次数
			
	ptcom->return_length[0]=0;										//返回数据长度
	ptcom->return_start[0]=0;										//返回数据有效开始
	ptcom->return_length_available[0]=0;							//返回有效数据长度	
	ptcom->Current_Times=0;											//当前发送次数	
	return 1;
}
/*************************************************************************************************************************
协议规定LW的Modbus地址是0-4095
RWI的地址是4096-8191
*************************************************************************************************************************/
int slave_send_LW()//发送LW的内容到主站
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

	staradd_high_H=bsicc(*(U8 *)(COMad+5));							//地址最高位，依次往下到最低位
	staradd_high_L=bsicc(*(U8 *)(COMad+6));
	staradd_low_H=bsicc(*(U8 *)(COMad+7));
	staradd_low_L=bsicc(*(U8 *)(COMad+8));
	staradd=(staradd_high_H<<12)+(staradd_high_L<<8)+(staradd_low_H<<4)+staradd_low_L;
	
	length_high_H=bsicc(*(U8 *)(COMad+9));							//读取长度最高位，依次往下到最低位
	length_high_L=bsicc(*(U8 *)(COMad+10));
	length_low_H=bsicc(*(U8 *)(COMad+11));
	length_low_L=bsicc(*(U8 *)(COMad+12));
	length=(length_high_H<<12)+(length_high_L<<8)+(length_low_H<<4)+length_low_L;

	if ((staradd+length)>8191)										//地址超过范围
	{
		return 0;
	}	

	if (staradd<4096)												//读的是LW的内容
	{
		for (i=0;i<length;i++)										//数据
		{
			charGroup[i*2+0]=((*(U16 *)(LW+(staradd+i)*2))>>8)&0xff;		
			charGroup[i*2+1]=((*(U16 *)(LW+(staradd+i)*2)))&0xff;				
		}	
	}
	else if (staradd>=4096)											//读的是RWI的内容
	{
		staradd=staradd-4096;
		for (i=0;i<length;i++)										//数据
		{
			charGroup[i*2+0]=((*(U16 *)(RWI+(staradd+i)*2))>>8)&0xff;		
			charGroup[i*2+1]=((*(U16 *)(RWI+(staradd+i)*2)))&0xff;			
		}
	}
//准备发送数据	
	*(U8 *)(AD1+0)=0x3a;											//开始
	*(U8 *)(AD1+1)=*(U8 *)(COMad+1);								//站地址	
	*(U8 *)(AD1+2)=*(U8 *)(COMad+2);
	*(U8 *)(AD1+3)=*(U8 *)(COMad+3);								//功能码	
	*(U8 *)(AD1+4)=*(U8 *)(COMad+4);
	*(U8 *)(AD1+5)=asicc((length*2>>4)&0xf);						//长度	
	*(U8 *)(AD1+6)=asicc(length*2&0xf);

	for (i=0;i<length;i++)											//数据
	{
		*(U8 *)(AD1+7+i*4)=asicc((charGroup[i*2]>>4)&0xf);		
		*(U8 *)(AD1+8+i*4)=asicc(charGroup[i*2]&0xf);
		*(U8 *)(AD1+9+i*4)=asicc((charGroup[i*2+1]>>4)&0xf);		
		*(U8 *)(AD1+10+i*4)=asicc(charGroup[i*2+1]&0xf);		
	}

	//开始校验	
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
	Result_LRC=CalLRC(CHECK,3+length*2);							//LRC校验
	Result_LRC_H=(Result_LRC>>4)&0xf;
	Result_LRC_L=Result_LRC&0xf;
	*(U8 *)(AD1+7+length*4)=asicc(Result_LRC_H);          			//校验 高位
	*(U8 *)(AD1+8+length*4)=asicc(Result_LRC_L);          			//校验 低位	
	
	*(U8 *)(AD1+9+length*4)=0x0d;									//ASCII模式结束字符，回车CR，换行LF
	*(U8 *)(AD1+10+length*4)=0x0a;	

	ptcom->send_length[0]=11+length*4;								//发送长度
	ptcom->send_staradd[0]=0;										//发送数据存储地址	
	ptcom->send_times=1;											//发送次数
			
	ptcom->return_length[0]=0;										//返回数据长度
	ptcom->return_start[0]=0;										//返回数据有效开始
	ptcom->return_length_available[0]=0;							//返回有效数据长度	
	ptcom->Current_Times=0;											//当前发送次数	
	return 1;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int slave_write_LB()//更改LB的数值
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
	
	staradd_high_H=bsicc(*(U8 *)(COMad+5));							//地址最高位，依次往下到最低位
	staradd_high_L=bsicc(*(U8 *)(COMad+6));
	staradd_low_H=bsicc(*(U8 *)(COMad+7));
	staradd_low_L=bsicc(*(U8 *)(COMad+8));
	staradd=(staradd_high_H<<12)+(staradd_high_L<<8)+(staradd_low_H<<4)+staradd_low_L;

	WriteValue_high_H=bsicc(*(U8 *)(COMad+9));						//写入的值，即置位/复位
	WriteValue_high_L=bsicc(*(U8 *)(COMad+10));
	WriteValue_high=(WriteValue_high_H<<4)+WriteValue_high_L;		//高位
	WriteValue_low_H=bsicc(*(U8 *)(COMad+11));
	WriteValue_low_L=bsicc(*(U8 *)(COMad+12));
	WriteValue_low=(WriteValue_low_H<<4)+WriteValue_low_L;			//低位
	
	if (staradd>=4096)												//地址超过范围
	{
		return 0;
	}

	if (WriteValue_high == 0xff && WriteValue_low == 0x00)			//置位
	{
		*(U8 *)(LB+staradd)=1;
		Finish=1;
	}
	else if (WriteValue_high == 0x00 && WriteValue_low == 0x00)		//复位
	{
		*(U8 *)(LB+staradd)=0;
		Finish=1;
	}
//准备发送数据
	if (Finish==1)													//写入的数值是正确的
	{
		for (i=0;i<17;i++)
		{
			*(U8 *)(AD1+i)=*(U8 *)(COMad+i);
		}
		
		ptcom->send_length[0]=17;									//发送长度
		ptcom->send_staradd[0]=0;									//发送数据存储地址	
		ptcom->send_times=1;										//发送次数
				
		ptcom->return_length[0]=0;									//返回数据长度
		ptcom->return_start[0]=0;									//返回数据有效开始
		ptcom->return_length_available[0]=0;						//返回有效数据长度	
		ptcom->Current_Times=0;										//当前发送次数	
		return 1;	
	}
	else
	{
		return 0;
	}
}
/*************************************************************************************************************************
协议规定LW的Modbus地址是0-4095
RWI的地址是4096-8191
*************************************************************************************************************************/
int slave_write_LW()//更改LW的数值
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
	
	if (*(U8 *)(COMad+3) == 0x31 && *(U8 *)(COMad+4) == 0x30)		//写多字节
	{
		Length_high_H=bsicc(*(U8 *)(COMad+9));						//长度，依次往下到最低位
		Length_high_L=bsicc(*(U8 *)(COMad+10));
		Length_low_H=bsicc(*(U8 *)(COMad+11));
		Length_low_L=bsicc(*(U8 *)(COMad+12));
		Length=(Length_high_H<<12)+(Length_high_L<<8)+(Length_low_H<<4)+Length_low_L;
		Length_Byte=(bsicc(*(U8 *)(COMad+13))<<4)+bsicc(*(U8 *)(COMad+14));
		if ((Length*2) != Length_Byte)								//字节单位与字单位不匹配
		{
			return 0;
		}
	}

	staradd_high_H=bsicc(*(U8 *)(COMad+5));							//地址最高位，依次往下到最低位
	staradd_high_L=bsicc(*(U8 *)(COMad+6));
	staradd_low_H=bsicc(*(U8 *)(COMad+7));
	staradd_low_L=bsicc(*(U8 *)(COMad+8));
	staradd=(staradd_high_H<<12)+(staradd_high_L<<8)+(staradd_low_H<<4)+staradd_low_L;

	if (*(U8 *)(COMad+3) == 0x30 && *(U8 *)(COMad+4) == 0x36)		//写入单个寄存器
	{
		Length=1;
		k=0;
	}
	else if (*(U8 *)(COMad+3) == 0x31 && *(U8 *)(COMad+4) == 0x30)	//写多字节
	{
		Length=Length;
		k=6;
	}

	if((staradd+Length)>=8192)										//地址超过范围
	{
		return 0;	
	}	

	if(staradd<4096)												//LW
	{
		for (i=0;i<Length;i++)
		{
			WriteValue_high_H=bsicc(*(U8 *)(COMad+9+k+i*4));		//写入的值，即置位/复位
			WriteValue_high_L=bsicc(*(U8 *)(COMad+10+k+i*4));
			WriteValue_high=(WriteValue_high_H<<4)+WriteValue_high_L;//高位
			WriteValue_low_H=bsicc(*(U8 *)(COMad+11+k+i*4));
			WriteValue_low_L=bsicc(*(U8 *)(COMad+12+k+i*4));
			WriteValue_low=(WriteValue_low_H<<4)+WriteValue_low_L;	//低位			
			*(U16 *)(LW+(staradd+i)*2)=(WriteValue_high<<8)+WriteValue_low;
		}		
	}
	else if(staradd>=4096)											//RWI
	{
		staradd=staradd-4096;
		for (i=0;i<Length;i++)
		{
			WriteValue_high_H=bsicc(*(U8 *)(COMad+9+k+i*4));		//写入的值，即置位/复位
			WriteValue_high_L=bsicc(*(U8 *)(COMad+10+k+i*4));
			WriteValue_high=(WriteValue_high_H<<4)+WriteValue_high_L;//高位
			WriteValue_low_H=bsicc(*(U8 *)(COMad+11+k+i*4));
			WriteValue_low_L=bsicc(*(U8 *)(COMad+12+k+i*4));
			WriteValue_low=(WriteValue_low_H<<4)+WriteValue_low_L;	//低位			
			*(U16 *)(RWI+(staradd+i)*2)=(WriteValue_high<<8)+WriteValue_low;
		}
	}
//准备发送数据
	if (*(U8 *)(COMad+3) == 0x30 && *(U8 *)(COMad+4) == 0x36)		//如果功能码是0x06,则返回和发送代码一样的东西
	{
		for (i=0;i<17;i++)
		{
			*(U8 *)(AD1+i)=*(U8 *)(COMad+i);
		}
		
		ptcom->send_length[0]=17;									//发送长度
		ptcom->send_staradd[0]=0;									//发送数据存储地址	
		ptcom->send_times=1;										//发送次数
				
		ptcom->return_length[0]=0;									//返回数据长度
		ptcom->return_start[0]=0;									//返回数据有效开始
		ptcom->return_length_available[0]=0;						//返回有效数据长度	
		ptcom->Current_Times=0;										//当前发送次数	
		return 1;	
	}
	else if (*(U8 *)(COMad+3) == 0x31 && *(U8 *)(COMad+4) == 0x30)	//功能码是0x10时，则发送其他东西
	{
		*(U8 *)(AD1+0)=0x3a;										//开始
		for (i=1;i<13;i++)
		{
			*(U8 *)(AD1+i)=*(U8 *)(COMad+i);
		}
		
		//开始校验
		for (i=0;i<6;i++)
		{
			Temp_1=bsicc(*(U8 *)(COMad+1+i*2));
			Temp_2=bsicc(*(U8 *)(COMad+2+i*2));
			CHECK[i]=(Temp_1<<4)+Temp_2;		
		}
				
		Result_LRC=CalLRC(CHECK,6);									//LRC校验
		Result_LRC_H=(Result_LRC>>4)&0xf;
		Result_LRC_L=Result_LRC&0xf;
		*(U8 *)(AD1+13)=asicc(Result_LRC_H);          				//校验 高位
		*(U8 *)(AD1+14)=asicc(Result_LRC_L);          				//校验 低位	
		
		*(U8 *)(AD1+15)=0x0d;										//ASCII模式结束字符，回车CR，换行LF
		*(U8 *)(AD1+16)=0x0a;	
		
		ptcom->send_length[0]=17;									//发送长度
		ptcom->send_staradd[0]=0;									//发送数据存储地址	
		ptcom->send_times=1;										//发送次数
					
		ptcom->return_length[0]=0;									//返回数据长度
		ptcom->return_start[0]=0;									//返回数据有效开始
		ptcom->return_length_available[0]=0;						//返回有效数据长度	
		ptcom->Current_Times=0;										//当前发送次数	
		return 1;		
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
U16 CalLRC(unsigned char *chData,U16 uNo)//LRC校验，用16进制数校验，asicc码显示
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
int asicc(int a)//转为Asc码
{
	int bl;
	if(a<10)
		bl=a+0x30;
	if(a>9)
		bl=a-10+0x41;
	return bl;	
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int bsicc(int a)//Asc转为数字
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
