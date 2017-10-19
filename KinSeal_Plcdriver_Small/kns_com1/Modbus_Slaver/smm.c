#include "stdio.h"
#include "def.h"
#include "smm.h"


struct Com_struct_D *ptcom;

void Enter_PlcDriver(void)
{
	int result;
	int IfOK;	
	LB=*(U32 *)(LB_Address+0);
	LW=*(U32 *)(LW_Address+0);
	RWI=*(U32 *)(RWI_Address+0);	
	ptcom=(struct Com_struct_D *)adcom;
		
	result=checkCorrect();		//检查数据是否正确	
//	sysprintf("result %d \n",result);
	if (result==1)		//数据正确
	{	
		IfOK=senddata();
		if (IfOK==1)		//成功正确
		{
			ptcom->IfResultCorrect=1;
		}
	}	
	else if (result==8)		//CRC错误
	{
		errorsend(*(U8 *)(COMad+1),8);		//错误发送
		ptcom->IfResultCorrect=1;
	}
}

int checkCorrect()		//检查数据是否正确	
{	
	unsigned char crc_high;
	unsigned char crc_low;
	unsigned char crc_high_return;
	unsigned char crc_low_return;
	U16 aakj;	

//	sysprintf("ptcom->IQ %d \n",ptcom->IQ);	
	aakj=CalcCrc((U8 *)COMad,ptcom->IQ-2);//计算校检是否正确
//	sysprintf("aakj 0x%x \n",aakj);	

	crc_high=aakj/0x100;
	crc_low=aakj-crc_high*0x100;
	crc_high_return=*(U8 *)(COMad+ptcom->IQ-1);
	crc_low_return=*(U8 *)(COMad+ptcom->IQ-2);		
//	sysprintf("crc_high 0x%x,crc_low 0x%x,crc_high_return 0x%x,crc_low_return 0x%x\n",crc_high,crc_low,crc_high_return,crc_low_return);	

	if (crc_high==crc_high_return && crc_low==crc_low_return)	//校检正确
	{
//	sysprintf("here \n");	

		if (ptcom->R_W_Flag==*(U8 *)(COMad+0) || *(U8 *)(COMad+0)==0)	//与HMI站地址一致,或者广播模式
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}	
	else		//CRC校验错误
	{

//	sysprintf("here error\n");	
		if (ptcom->R_W_Flag!=*(U8 *)(COMad+0) || *(U8 *)(COMad+0)==0)	//站地址为0时，都不返回
		{
			return 0;
		}
		else
		{
			return 8;				//CRCErrorSend()
		}
	}
	return 0;	
}

/*********************************
接收错误发送
**********************************/
void errorsend(int func,int type)
{
	unsigned char crc_high;
	unsigned char crc_low;
	U16 aakj;
	
	*(U8 *)(AD1+0)=ptcom->R_W_Flag;		//HMI地址
	switch (type)
	{
	case 8:		//crc error
		*(U8 *)(AD1+1)=0x80+func;		//返回错误功能吗
		*(U8 *)(AD1+2)=0x08;			//crc error		
		break;
	case 1:		//function error
		*(U8 *)(AD1+1)=0x80+func;		//返回错误功能吗
		*(U8 *)(AD1+2)=0x01;			//function error			
		break;	
	case 2:		//address error
		*(U8 *)(AD1+1)=0x80+func;		//返回错误功能吗
		*(U8 *)(AD1+2)=0x02;			//address error		
		break;
	case 3:		//data error
		*(U8 *)(AD1+1)=0x80+func;		//返回错误功能吗
		*(U8 *)(AD1+2)=0x03;			//data error			
		break;		
	default:// error
		*(U8 *)(AD1+1)=0x80+func;		//返回错误功能吗
		*(U8 *)(AD1+2)=0x04;			// error		
		break;				
	}
	aakj=CalcCrc((U8 *)AD1,3);		//计算校检
	crc_high=(aakj>>8)&0xff;
	crc_low=aakj&0xff;
	*(U8 *)(AD1+3)=crc_low;		//CRC校检低字节
	*(U8 *)(AD1+4)=crc_high;		//CRC校检高字节	
	ptcom->send_length[0]=5;		//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址	
	ptcom->send_times=1;					//发送次数
			
	ptcom->return_length[0]=0;				//返回数据长度
	ptcom->return_start[0]=0;				//返回数据有效开始
	ptcom->return_length_available[0]=0;	//返回有效数据长度	
	ptcom->Current_Times=0;					//当前发送次数			
}

int senddata()	//发送数据到串口
{
	int result=0;
	switch (*(U8 *)(COMad+1))		//功能码
	{
	case 01:						//读取LB的内容
	case 02:						//读取LB的内容	
		result=slave_send_LB();	
		break;
	case 03:						//读取LW的内容
	case 04:						//读取LW的内容
		result=slave_send_LW();		
		break;	
	case 05:						//控制LB	
		result=slave_write_LB();
		break;
	case 0x06:						//写入单个LW	
	case 0x10:						//写入多个个LW		
		result=slave_write_LW();
		break;	
	default:
		errorsend(*(U8 *)(COMad+1),1);		//功能吗不存在错误发送
		result=1;												
	}
	if (*(U8 *)(COMad+0)==0)		//广播模式
	{
		result=0;					//广播模式下不返回代码
	}
	return result;
}

int slave_send_LB()					//发送LB的内容到主站
{
	U8 staradd_low;
	U8 staradd_high;	
	U8 length_low;
	U8 length_high;
	U16 length;
	U16 staradd;
	int i,j,k;
	U8 charGroup[255];
	unsigned char crc_high;
	unsigned char crc_low;
	U16 aakj;
	
	staradd_low=*(U8 *)(COMad+3);		//地址
	staradd_high=*(U8 *)(COMad+2);	
	staradd=(staradd_high<<8)+staradd_low;
	length_low=*(U8 *)(COMad+5);		//长度,单位是位
	length_high=*(U8 *)(COMad+4);	
	length=(length_high<<8)+length_low;
	
	if ((staradd+length)>=4096)//地址超过范围
	{
		errorsend(*(U8 *)(COMad+1),2);		//错误发送
		return 1;
	}
	if (length%8==0)		//刚好整除
	{
		length=length/8;
	}
	else if (length%8!=0)		//有余数
	{
		length=length/8+1;
	}	
	for (i=0;i<length;i++)	//数据
	{
		charGroup[i]=0;		//复位
		for (j=0;j<8;j++)
		{
			charGroup[i]=(((*(U8 *)(LB+staradd+i*8+j))&0x01)<<j)+charGroup[i];
		}
	}
//准备发送数据
	*(U8 *)(AD1+0)=ptcom->R_W_Flag;		//HMI地址
	*(U8 *)(AD1+1)=*(U8 *)(COMad+1);		//功能码
	*(U8 *)(AD1+2)=length&0xff;		//发送长度
	for (i=0;i<length;i++)
	{
		*(U8 *)(AD1+3+i)=charGroup[i];		//发送长度
	}	
	aakj=CalcCrc((U8 *)AD1,3+length);		//计算校检
	crc_high=(aakj>>8)&0xff;
	crc_low=aakj&0xff;
	*(U8 *)(AD1+3+length)=crc_low;		//CRC校检低字节
	*(U8 *)(AD1+4+length)=crc_high;		//CRC校检高字节	
//准备发送数据
	k=4+length+1;
	ptcom->send_length[0]=4+length+1;		//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址	
	ptcom->send_times=1;					//发送次数
			
	ptcom->return_length[0]=0;				//返回数据长度
	ptcom->return_start[0]=0;				//返回数据有效开始
	ptcom->return_length_available[0]=0;	//返回有效数据长度	
	ptcom->Current_Times=0;					//当前发送次数	
	return 1;
}

/*************************
协议规定LW的Modbus地址是0-4095
RWI的地址是4096-8191
*************************/
int slave_send_LW()					//发送LW的内容到主站
{
	U8 staradd_low;
	U8 staradd_high;	
	U8 length_low;
	U8 length_high;
	U16 length;
	U16 staradd;
	int i,k;
	U8 charGroup[512];
	unsigned char crc_high;
	unsigned char crc_low;
	U16 aakj;
	
	staradd_low=*(U8 *)(COMad+3);		//地址
	staradd_high=*(U8 *)(COMad+2);	
	staradd=(staradd_high<<8)+staradd_low;
	length_low=*(U8 *)(COMad+5);		//长度,单位是字
	length_high=*(U8 *)(COMad+4);	
	length=(length_high<<8)+length_low;
	
	if ((staradd+length)>8191)//地址超过范围
	{
		errorsend(*(U8 *)(COMad+1),2);		//错误发送
		return 1;
	}	
	if (staradd<4096)		//读的是LW的内容
	{
		for (i=0;i<length;i++)	//数据
		{
			charGroup[i*2+0]=((*(U16 *)(LW+(staradd+i)*2))>>8)&0xff;		//复位
			charGroup[i*2+1]=((*(U16 *)(LW+(staradd+i)*2)))&0xff;		//复位		
		}	
	}
	else if (staradd>=4096)	//读的是RWI的内容
	{
		staradd=staradd-4096;
		for (i=0;i<length;i++)	//数据
		{
			charGroup[i*2+0]=((*(U16 *)(RWI+(staradd+i)*2))>>8)&0xff;		//复位
			charGroup[i*2+1]=((*(U16 *)(RWI+(staradd+i)*2)))&0xff;		//复位		
		}
	}
	

//准备发送数据
	*(U8 *)(AD1+0)=ptcom->R_W_Flag;		//HMI地址
	*(U8 *)(AD1+1)=*(U8 *)(COMad+1);		//功能码
	*(U8 *)(AD1+2)=(length*2)&0xff;		//发送长度,单位是字节
	for (i=0;i<length*2;i++)
	{
		*(U8 *)(AD1+3+i)=charGroup[i];		//发送长度
	}	
	aakj=CalcCrc((U8 *)AD1,3+length*2);		//计算校检
	crc_high=(aakj>>8)&0xff;
	crc_low=aakj&0xff;
	*(U8 *)(AD1+3+length*2)=crc_low;		//CRC校检低字节
	*(U8 *)(AD1+4+length*2)=crc_high;		//CRC校检高字节	
		
	k=5+length*2;							//发送到串口的字节数
//准备发送数据
	ptcom->send_length[0]=k;			//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址	
	ptcom->send_times=1;					//发送次数
			
	ptcom->return_length[0]=0;				//返回数据长度
	ptcom->return_start[0]=0;				//返回数据有效开始
	ptcom->return_length_available[0]=0;	//返回有效数据长度	
	ptcom->Current_Times=0;					//当前发送次数	
	return 1;
}

int slave_write_LB()					//更改LB的数值
{
	U8 staradd_low;
	U8 staradd_high;	
	U8 V1;
	U8 V2;
	U16 staradd;
	int i,k;
	int Finish=0;
	
	staradd_low=*(U8 *)(COMad+3);		//地址
	staradd_high=*(U8 *)(COMad+2);	
	staradd=(staradd_high<<8)+staradd_low;
	V1=*(U8 *)(COMad+4);		//置位还是复位
	V2=*(U8 *)(COMad+5);
	if (staradd>=4096)//地址超过范围
	{
		errorsend(*(U8 *)(COMad+1),2);		//错误发送
		return 1;
	}
	if (V1==0xff && V2==0)	//置位
	{
		*(U8 *)(LB+staradd)=1;
		Finish=1;
	}
	else if (V1==0 && V2==0)			//复位
	{
		*(U8 *)(LB+staradd)=0;
		Finish=1;
	}
	else
	{
		errorsend(*(U8 *)(COMad+1),3);		//错误发送
		return 1;
	}
//准备发送数据
	if (Finish==1)						//写入的数值是正确的
	{
		for (i=0;i<8;i++)
		{
			*(U8 *)(AD1+i)=*(U8 *)(COMad+i);
		}
		
		k=8;							//发送到串口的字节数
//准备发送数据
		ptcom->send_length[0]=k;			//发送长度
		ptcom->send_staradd[0]=0;				//发送数据存储地址	
		ptcom->send_times=1;					//发送次数
				
		ptcom->return_length[0]=0;				//返回数据长度
		ptcom->return_start[0]=0;				//返回数据有效开始
		ptcom->return_length_available[0]=0;	//返回有效数据长度	
		ptcom->Current_Times=0;					//当前发送次数	
		return 1;
	}
	else
		return 0;

}

/*************************
协议规定LW的Modbus地址是0-4095
RWI的地址是4096-8191
*************************/
int slave_write_LW()					//更改LW的数值
{
	U8 staradd_low;
	U8 staradd_high;
	U8 L1,L2;	
	U16 length;
	U16 staradd;
	int i,k;
	U8 V1,V2;
	unsigned char crc_high;
	unsigned char crc_low;
	U16 aakj;
	
	
	
	if (*(U8 *)(COMad+1)==0x10)				//写多字节
	{
		L1=(*(U8 *)(COMad+4)<<8)+*(U8 *)(COMad+5);	//modbus协议规定4，5是字单位，6是字节单位
		L2=*(U8 *)(COMad+6);
		if ((L1*2)!=L2)						//字节单位与字单位不匹配
		{
			errorsend(*(U8 *)(COMad+1),3);		//错误发送
			return 1;
		}
		if (ptcom->IQ!=L2+9)
		{
			errorsend(*(U8 *)(COMad+1),3);		//接收到的数据长度与实际不一致
			return 1;			
		}
	}
	
	if (*(U8 *)(COMad+1)==0x06)
	{
		if (ptcom->IQ!=8)
		{
			errorsend(*(U8 *)(COMad+1),3);		//接收到的数据长度与实际不一致
			return 1;			
		}		
	}

	staradd_low=*(U8 *)(COMad+3);		//地址
	staradd_high=*(U8 *)(COMad+2);	
	staradd=(staradd_high<<8)+staradd_low;
	
	if (*(U8 *)(COMad+1)==0x06)			//写入单个寄存器
	{
		length=1;
		k=4;
	}
	else
	{
		length=L1;
		k=7;
	}
	if((staradd+length)>=8192)					//地址超过范围
	{
		errorsend(*(U8 *)(COMad+1),2);		//错误发送
		return 1;	
	}	
	if(staradd<4096)			//LW
	{
		for (i=0;i<length;i++)
		{
			V1=*(U8 *)(COMad+k+i*2+0);
			V2=*(U8 *)(COMad+k+i*2+1);			
			*(U16 *)(LW+(staradd+i)*2)=(V1<<8)+V2;
		}		
	}
	else if(staradd>=4096)			//RWI
	{
		staradd=staradd-4096;
		for (i=0;i<length;i++)
		{
			V1=*(U8 *)(COMad+k+i*2+0);
			V2=*(U8 *)(COMad+k+i*2+1);			
			*(U16 *)(RWI+(staradd+i)*2)=(V1<<8)+V2;
		}
	}	
	if (*(U8 *)(COMad+1)==0x06)		//如果功能码是0x06,则返回和发送代码一样的东西
	{
		for (i=0;i<8;i++)
		{
			*(U8 *)(AD1+i)=*(U8 *)(COMad+i);
		}
		
		k=8;							//发送到串口的字节数
//准备发送数据
		ptcom->send_length[0]=k;			//发送长度
		ptcom->send_staradd[0]=0;				//发送数据存储地址	
		ptcom->send_times=1;					//发送次数
				
		ptcom->return_length[0]=0;				//返回数据长度
		ptcom->return_start[0]=0;				//返回数据有效开始
		ptcom->return_length_available[0]=0;	//返回有效数据长度	
		ptcom->Current_Times=0;					//当前发送次数	
		return 1;
	}
	else if (*(U8 *)(COMad+1)==0x10)		//功能码是0x10时，则发送其他东西
	{
		*(U8 *)(AD1+0)=*(U8 *)(COMad+0);
		*(U8 *)(AD1+1)=*(U8 *)(COMad+1);		
		*(U8 *)(AD1+2)=*(U8 *)(COMad+2);		
		*(U8 *)(AD1+3)=*(U8 *)(COMad+3);		
		*(U8 *)(AD1+4)=*(U8 *)(COMad+4);		
		*(U8 *)(AD1+5)=*(U8 *)(COMad+5);	
		aakj=CalcCrc((U8 *)AD1,6);		//计算校检
		crc_high=(aakj>>8)&0xff;
		crc_low=aakj&0xff;
		*(U8 *)(AD1+6)=crc_low;		//CRC校检低字节
		*(U8 *)(AD1+7)=crc_high;		//CRC校检高字节					
	
		k=8;					//发送到串口的字节数
//准备发送数据
		ptcom->send_length[0]=k;			//发送长度
		ptcom->send_staradd[0]=0;				//发送数据存储地址	
		ptcom->send_times=1;					//发送次数
				
		ptcom->return_length[0]=0;				//返回数据长度
		ptcom->return_start[0]=0;				//返回数据有效开始
		ptcom->return_length_available[0]=0;	//返回有效数据长度	
		ptcom->Current_Times=0;					//当前发送次数	
		return 1;	
	}
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
