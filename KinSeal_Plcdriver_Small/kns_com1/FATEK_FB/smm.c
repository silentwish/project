#include "stdio.h"
#include "def.h"
#include "smm.h"


	
struct Com_struct_D *ptcom;


 
void Enter_PlcDriver(void)
{	
	ptcom=(struct Com_struct_D *)adcom;
	switch (ptcom->R_W_Flag)
	{
	case PLC_READ_DATA:				//进入驱动是读数据
	case PLC_READ_DIGITALALARM:			//进入驱动是读数据,报警	
	case PLC_READ_TREND:			//进入驱动是读数据,趋势图
	case PLC_READ_ANALOGALARM:			//进入驱动是读数据,类比报警	
	case PLC_READ_CONTROL:			//进入驱动是读PLC控制的数据	
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
	case PLC_WRITE_DATA:				//进入驱动是写数据
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
	case PLC_WRITE_TIME:				//进入驱动是写时间到PLC
		switch(ptcom->registerr)
		{
		case 'D':
		case 'R':				
			Write_Time();		
			break;			
		}
		break;	
	case PLC_READ_TIME:				//进入驱动是读取时间到PLC
		switch(ptcom->registerr)
		{
		case 'D':
		case 'R':				
			Read_Time();		
			break;			
		}
		break;
	case PLC_WRITE_RECIPE:				//进入驱动是写配方到PLC
		switch(*(U8 *)(PE+3))//配方寄存器名称
		{
		case 'D':	
		case 'R':		
			Write_Recipe();		
			break;			
		}
		break;
	case PLC_READ_RECIPE:				//进入驱动是从PLC读取配方
		switch(*(U8 *)(PE+3))//配方寄存器名称
		{
		case 'D':	
		case 'R':	
			Read_Recipe();		
			break;			
		}
		break;							
	case PLC_CHECK_DATA:				//进入驱动是数据处理
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

	b=ptcom->address;			// 置位地址
	plc_add=ptcom->plc_address;	//plc站地址
	switch (ptcom->registerr)	//根据寄存器类型获得偏移地址
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
		if (ptcom->writeValue==1)	//置位
		{
			*(U8 *)(AD1+5)=0x33;
		}
		if (ptcom->writeValue==0)	//复位
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
		
		ptcom->send_length[0]=14;				//发送长度
		ptcom->send_staradd[0]=0;				//发送数据存储地址	
		ptcom->send_times=1;					//发送次数
			
		ptcom->return_length[0]=9;				//返回数据长度
		ptcom->return_start[0]=0;				//返回数据有效开始
		ptcom->return_length_available[0]=0;	//返回有效数据长度	
		ptcom->Current_Times=0;					//当前发送次数			
		ptcom->Simens_Count=0;
}


void Read_Bool()				//读取数字量的信息
{
	U16 aakj;
	int b,b1,b2;
	int a1,a2,a3,a4;
	int add,plc_add,len;	

	b=ptcom->address;			// 置位地址
	plc_add=ptcom->plc_address;	//plc站地址
	len=ptcom->register_length;	//发送长度。字节
	
	switch (ptcom->registerr)	//根据寄存器类型获得偏移地址
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
	len=(len/2)+len%2;			//发送长度，字为单位
	//ptcom->plc_address = ((len/2)+len%2 + 1)*2;

	*(U8 *)(AD1+0)=0x02;
	*(U8 *)(AD1+1)=asicc(b1);	//plc add
	*(U8 *)(AD1+2)=asicc(b2);
	*(U8 *)(AD1+3)=0x34;		//command
	*(U8 *)(AD1+4)=0x36;	
	*(U8 *)(AD1+5)=asicc((len>>4)&0xf);	//发送长度
	*(U8 *)(AD1+6)=asicc(len&0xf);		//发送长度

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
	
	ptcom->send_length[0]=16;				//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址	
	ptcom->send_times=1;					//发送次数
		
	ptcom->return_length[0]=9+len*4;		//返回数据长度
	ptcom->return_start[0]=6;				//返回数据有效开始
	ptcom->return_length_available[0]=len*4;//返回有效数据长度	
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->Simens_Count=1;					//标示读的是位
}



void Read_Analog()				//读模拟量
{
	U16 aakj;
	int b,b1,b2;
	int a1,a2,a3,a4,a5;
	int add,plc_add,len;	

	b=ptcom->address;			// 置位地址
	plc_add=ptcom->plc_address;	//plc站地址
	len=ptcom->register_length;	//发送长度。字
	
	switch (ptcom->registerr)	//根据寄存器类型获得偏移地址
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
	*(U8 *)(AD1+5)=asicc((len>>4)&0xf);	//发送长度
	*(U8 *)(AD1+6)=asicc(len&0xf);		//发送长度

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
	
	ptcom->send_length[0]=16;				//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址	
	ptcom->send_times=1;					//发送次数
		
	ptcom->return_length[0]=9+len*4;		//返回数据长度
	ptcom->return_start[0]=6;				//返回数据有效开始
	ptcom->return_length_available[0]=len*4;//返回有效数据长度	
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->Simens_Count=2;
}


void Read_Recipe()								//读取配方
{
	U16 aakj;
	int add;
	int a1,a2,a3,a4,a5;
	int i,t,b1,b2;
	int datalength;
	int p_start;
	int ps,plc_add;
	int SendTimes;
	int LastTimeWord;							//最后一次发送长度
	int currentlength;
	
	switch (ptcom->registerr)	//根据寄存器类型获得偏移地址
	{
	case 'D':
		add=0x44;
		break;	
	case 'R':
		add=0x52;
		break;		
	}
			
	datalength=ptcom->register_length;		//发送总长度
	p_start=ptcom->address;					//开始地址
	plc_add=ptcom->plc_address;				//plc站地址
	b1=(plc_add>>4)&0xf;
	b2=(plc_add)&0xf;	
	if(datalength>5000)
		datalength=5000;
	
	//每次最多能发送32个D
	if(datalength%32==0)
	{
		SendTimes=datalength/32;
		LastTimeWord=32;//最后一次发送的长度	
	}
	if(datalength%32!=0)
	{
		SendTimes=datalength/32+1;//发送的次数
		LastTimeWord=datalength%32;//最后一次发送的长度	
	}
	
	ps=16;
	for (i=0;i<SendTimes;i++)
	{
		if (i!=(SendTimes-1))	//不是最后一次时
		{
			currentlength=32;
		}
		if (i==(SendTimes-1))	//最后一次时
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
		*(U8 *)(AD1+5+ps*i)=asicc((currentlength>>4)&0xf);	//发送长度
		*(U8 *)(AD1+6+ps*i)=asicc(currentlength&0xf);		//发送长度

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
		
		ptcom->send_length[i]=16;				//发送长度
		ptcom->send_staradd[i]=i*16;			//发送数据存储地址	
		ptcom->send_add[i]=t;					//读的是这个地址的数据	
		ptcom->send_data_length[i]=currentlength;	//不是最后一次都是32个D
				
		ptcom->return_length[i]=9+currentlength*4;				//返回数据长度，有4个固定，02，03，校检
		ptcom->return_start[i]=6;				//返回数据有效开始
		ptcom->return_length_available[i]=currentlength*4;	//返回有效数据长度		
	}
	ptcom->send_times=SendTimes;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->Simens_Count=2;
}

void Write_Analog()								//写模拟量
{
	U16 aakj;
	int b,b1,b2,i,k;
	int a1,a2,a3,a4,a5;
	int add,plc_add,len;	

	b=ptcom->address;			// 置位地址
	plc_add=ptcom->plc_address;	//plc站地址
	len=ptcom->register_length;	//发送长度。字
	
	switch (ptcom->registerr)	//根据寄存器类型获得偏移地址
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
	*(U8 *)(AD1+5)=asicc((len>>4)&0xf);	//发送长度
	*(U8 *)(AD1+6)=asicc(len&0xf);		//发送长度
	
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
	
	ptcom->send_length[0]=16+len*4;			//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址	
	ptcom->send_times=1;					//发送次数
		
	ptcom->return_length[0]=9;				//返回数据长度
	ptcom->return_start[0]=0;				//返回数据有效开始
	ptcom->return_length_available[0]=0;	//返回有效数据长度	
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->Simens_Count=0;
}

void Write_Time()
{
	Write_Analog();
}

void Read_Time()									//从PLC读取时间
{
	Read_Analog();
}

void Write_Recipe()								//写配方到PLC
{
	int datalength;
	int staradd;
	int SendTimes;
	int LastTimeWord;							//最后一次发送长度
	int i,j,b1,b2;
	int ps,t,k;
	int add;
	int a1,a2,a3,a4,a5;
	U16 aakj,len,plcadd;
	
	if (*(U8 *)(PE+3)=='D')						//写入寄存器
	{
		add=0x44;
	}
	else
	{
		add=0x52;
	}
	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));//数据长度
	staradd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//数据长度
	plcadd=*(U8 *)(PE+4);	      //PLC站地址	
	b1=(plcadd>>4)&0xf;
	b2=(plcadd)&0xf;		
	//每次最多能发送32个D
	if(datalength%32==0)
	{
		SendTimes=datalength/32;
		LastTimeWord=32;//最后一次发送的长度	
	}
	if(datalength%32!=0)
	{
		SendTimes=datalength/32+1;//发送的次数
		LastTimeWord=datalength%32;//最后一次发送的长度	
	}	
	
	ps=144;
	for (i=0;i<SendTimes;i++)
	{
		if (i==SendTimes-1)	//最后一次
		{
			len=LastTimeWord;
		}
		else
		{
			len=32;
		}
		t=staradd+i*32;		//开始地址
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
		*(U8 *)(AD1+5+ps*i)=asicc((len>>4)&0xf);	//发送长度
		*(U8 *)(AD1+6+ps*i)=asicc(len&0xf);		//发送长度

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
		
		ptcom->send_length[i]=16+len*4;			//发送长度
		ptcom->send_staradd[i]=ps*i;			//发送数据存储地址	
			
		ptcom->return_length[i]=9;				//返回数据长度
		ptcom->return_start[i]=0;				//返回数据有效开始
		ptcom->return_length_available[i]=0;	//返回有效数据长度				
	}
	ptcom->send_times=SendTimes;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数		
	ptcom->Simens_Count=0;
}


void compxy(void)				//处理成标准存储格式
{
	int i;
	unsigned char a1,a2,a3,a4;
	int b,b1,b2;
	U8 temp[500];
	if (ptcom->Simens_Count==1)			//位
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/4;i++)						//ASC玛返回，所以要转为16进制
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
			*(U8 *)(COMad+i*2+0)=b2;							//重新存,从第0个开始存
			*(U8 *)(COMad+i*2+1)=b1;							//重新存,从第0个开始存						
		}
		ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;	//长度减半	
		ptcom->IfResultCorrect=1;
	
	}
	else if (ptcom->Simens_Count==2)
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/2;i++)						//ASC玛返回，所以要转为16进制
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2);
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2+1);
			a1=bsicc(a1);
			a2=bsicc(a2);
			b=(a1<<4)+a2;
		//	*(U8 *)(COMad+i)=b;							//重新存,从第0个开始存
			temp[i]=b;
		}
		for (i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/2;i++)
		{
			*(U8 *)(COMad+i)=temp[i];			
		}
		ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;	//长度减半	
		ptcom->IfResultCorrect=1;	
	}
}

void watchcom(void)		//检查数据校检
{
	unsigned int aakj=0;
	aakj=remark();
	if(aakj==1)			//校检玛正确
	{
		ptcom->IfResultCorrect=1;
		compxy();		//进入数据处理程序
	}
	else
	{
		ptcom->IfResultCorrect=0;
		ptcom->Simens_Count=0;
	}
}

int asicc(int a)			//转为Asc码
{
	int bl;
	if(a<10)
		bl=a+0x30;
	if(a>9)
		bl=a-10+0x41;
	return bl;	
}

int bsicc(int a)			//Asc转为数字
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

int remark()				//返回来的数据计算校检码是否正确
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


U16 CalcHe(unsigned char *chData,U16 uNo)		//计算和校检
{
	int i;
	int ab=0;
	for(i=0;i<uNo;i++)
	{
		ab=ab+chData[i];
	}
	return (ab);
}
