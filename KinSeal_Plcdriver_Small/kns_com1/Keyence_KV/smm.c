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
	case PLC_READ_DATA:				//进入驱动是读数据
	case PLC_READ_DIGITALALARM:			//进入驱动是读数据,报警	
	case PLC_READ_TREND:			//进入驱动是读数据,趋势图
	case PLC_READ_ANALOGALARM:			//进入驱动是读数据,类比报警	
	case PLC_READ_CONTROL:			//进入驱动是读PLC控制的数据	
		switch(ptcom->registerr)
		{
		case 'Y':		
		case 'T':
		case 'C':						
			Read_Bool();   //进入驱动是读位数据       
			break;
		case 'D':
		case 'R':				
		case 't':
		case 'c':	
			Read_Analog();  //进入驱动是读模拟数据 
			break;			
		}
		break;
	case PLC_WRITE_DATA:				
		switch(ptcom->registerr)
		{	
		case 'Y':
		case 'T':
		case 'C':				
			Set_Reset();      //进入驱动是强置置位和复位
			break;
		case 'D':
		case 'R':
		case 't':
		case 'c':
			Write_Analog();	  //进入驱动是写模拟数据	
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
	case 7:				//进入驱动是为了握手
		handshake();	//进行握手		
		break;					
	case PLC_CHECK_DATA:				//进入驱动是数据处理
		watchcom();
		break;				
	}	 
}


void handshake()		//握手，通信建立前前必须
{
	*(U8 *)(AD1+0)=0x43;
	*(U8 *)(AD1+1)=0x52;
	*(U8 *)(AD1+2)=0x0d;
	*(U8 *)(AD1+3)=0x0a;
	ptcom->send_length[0]=4;				//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址	
	ptcom->send_times=1;					//发送次数
		
	ptcom->return_length[0]=4;				//返回数据长度,返回43 43 0D 0A 
	ptcom->return_start[0]=0;				//返回数据有效开始
	ptcom->return_length_available[0]=0;	//返回有效数据长度	
	ptcom->Current_Times=0;					//当前发送次数		
}

void Set_Reset()
{
	int b;
	int a1,a2,a3,a4,a5;
	int sendlength;
	int h_add,l_add;

	b=ptcom->address;			// 开始置位地址
	
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
	switch (ptcom->registerr)	//根据寄存器类型获得偏移地址
	{
	case 'Y':	//relay
		*(U8 *)(AD1+2)=0x20;
		*(U8 *)(AD1+3)=asicc(a1);		//寄存器地址
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
		*(U8 *)(AD1+4)=asicc(a1);		//寄存器地址
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
		*(U8 *)(AD1+4)=asicc(a1);		//寄存器地址
		*(U8 *)(AD1+5)=asicc(a2);	
		*(U8 *)(AD1+6)=asicc(a3);
		*(U8 *)(AD1+7)=asicc(a4);
		*(U8 *)(AD1+8)=asicc(a5);	
		*(U8 *)(AD1+9)=0x0d;	
		sendlength=10;	
		break;				
	}	
	
	ptcom->send_length[0]=sendlength;		//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址	
	ptcom->send_times=1;					//发送次数
		
	ptcom->return_length[0]=4;				//返回数据长度,返回4F 4B 0D 0A 
	ptcom->return_start[0]=0;				//返回数据有效开始
	ptcom->return_length_available[0]=0;	//返回有效数据长度	
	ptcom->Current_Times=0;					//当前发送次数			
}

void ReadY()
{
	int b,t,i;
	int a1,a2,a3,a4,a5;
	int h_add,l_add;
	int ps;
	int len;
	
	b=ptcom->address;						// 开始读取地址
	len=ptcom->register_length;				// 开始读取长度,kv700系列一次只允许读一个位,所以在这里的单位是位
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
		
	*(U8 *)(AD1+3)=asicc(a1);		//寄存器地址
	*(U8 *)(AD1+4)=asicc(a2);	
	*(U8 *)(AD1+5)=asicc(a3);
	*(U8 *)(AD1+6)=asicc(a4);
	*(U8 *)(AD1+7)=asicc(a5);	
	*(U8 *)(AD1+8)=0x0d;   		//	结束符
	*(U8 *)(AD1+9)=0x0a;   		//	结束符
					
	ptcom->send_length[0]=10;		//发送长度
	ptcom->send_staradd[0]=0;			//发送数据存储地址
	ptcom->send_add[0]=b;				//读的是这个地址的数据	
	ptcom->send_data_length[0]=1;	//不是最后一次都是1个位
					
	ptcom->return_length[0]=3;				//返回数据长度
	ptcom->return_start[0]=0;				//返回数据有效开始
	ptcom->return_length_available[0]=1;	//返回有效数据长度								

	ptcom->send_times=1;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->Simens_Count=1;					//表明读的是位
}



/****************************
读T时状态，当前值，设定值一起返回
****************************/
void ReadT()
{
	int b,t,i;
	int a1,a2,a3,a4,a5;
	int h_add,l_add;
	int ps;
	int len;
	
	b=ptcom->address;						// 开始读取地址
	len=ptcom->register_length;				// 开始读取长度,kv700系列一次只允许读一个位,所以在这里的单位是位
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
		
	*(U8 *)(AD1+4)=asicc(a1);		//寄存器地址
	*(U8 *)(AD1+5)=asicc(a2);	
	*(U8 *)(AD1+6)=asicc(a3);
	*(U8 *)(AD1+7)=0x0d;   		//	结束符
	*(U8 *)(AD1+8)=0x0a;   		//	结束符
					
	ptcom->send_length[0]=9;		//发送长度
	ptcom->send_staradd[0]=0;			//发送数据存储地址
	ptcom->send_add[0]=b;				//读的是这个地址的数据	
	ptcom->send_data_length[0]=1;	//不是最后一次都是1个位
					
	ptcom->return_length[0]=15;				//返回数据长度,状态 设定值 当前值 一起返回
	ptcom->return_start[0]=0;				//返回数据有效开始
	ptcom->return_length_available[0]=1;	//返回有效数据长度								

	ptcom->send_times=1;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->Simens_Count=4;					//表明读的是T的状态
}

void ReadC()
{
	int b,t,i;
	int a1,a2,a3,a4,a5;
	int h_add,l_add;
	int ps;
	int len;
	
	b=ptcom->address;						// 开始读取地址
	len=ptcom->register_length;				// 开始读取长度,kv700系列一次只允许读一个位,所以在这里的单位是位
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
		
	*(U8 *)(AD1+4)=asicc(a1);		//寄存器地址
	*(U8 *)(AD1+5)=asicc(a2);	
	*(U8 *)(AD1+6)=asicc(a3);
	*(U8 *)(AD1+7)=0x0d;   		//	结束符
	*(U8 *)(AD1+8)=0x0a;   		//	结束符
					
	ptcom->send_length[0]=9;		//发送长度
	ptcom->send_staradd[0]=0;			//发送数据存储地址
	ptcom->send_add[0]=b;				//读的是这个地址的数据	
	ptcom->send_data_length[0]=1;	//不是最后一次都是1个位
					
	ptcom->return_length[0]=15;				//返回数据长度,状态 设定值 当前值 一起返回
	ptcom->return_start[0]=0;				//返回数据有效开始
	ptcom->return_length_available[0]=1;	//返回有效数据长度								

	ptcom->send_times=1;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->Simens_Count=4;					//表明读的是T的状态
}
/***********************
keyence位地址形式
xxxyy
其中xxx是段地址0-599
yy是段内地址0-15
***********************/
void Read_Bool()				//读取数字量的信息
{
	switch (ptcom->registerr)	//根据寄存器类型
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
	
	b=ptcom->address;						// 开始读取地址
	datalength=ptcom->register_length;		// 读取长度,字
	
	
	ps=11;
	for (i=0;i<datalength;i++)				//一次只能读一个字
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
		
		*(U8 *)(AD1+5+ps*i)=asicc(a1);		//寄存器地址
		*(U8 *)(AD1+6+ps*i)=asicc(a2);	
		*(U8 *)(AD1+7+ps*i)=asicc(a3);
		*(U8 *)(AD1+8+ps*i)=asicc(a4);
		*(U8 *)(AD1+9+ps*i)=0x0d;   	//	
		*(U8 *)(AD1+10+ps*i)=0x0a;   	//			
			
		ptcom->send_length[i]=11;		//发送长度
		ptcom->send_staradd[i]=i*ps;	//发送数据存储地址
		ptcom->send_add[i]=ptcom->address;			//读的是这个地址的数据	
		ptcom->send_data_length[i]=datalength;	
					
		ptcom->return_length[i]=7;		//返回数据长度5个一个字，再加结束符0D 0A
		ptcom->return_start[i]=0;				//返回数据有效开始
		ptcom->return_length_available[i]=5;	//返回有效数据长度								
	}	
	for (i=0;i<datalength;i++)				//KV系列缺陷，定义的最多发送次数是20，若配方长度大于20，则会覆盖后面的数据
	{
		ptcom->return_length[i]=7;
		ptcom->send_add[i]=ptcom->address;			//读的是这个地址的数据	
		ptcom->send_data_length[i]=datalength;	
	}	
	
	ptcom->send_times=datalength;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->Simens_Count=2;					//表明读的是数据寄存器
}

void Read_Timer()	//读TC的当前值
{
	int b,t,i;
	int a1,a2,a3,a4,a5;
	int ps;
	int len,LastTimeWord,SendTimes,datalength;
	
	b=ptcom->address;						// 开始读取地址
	datalength=ptcom->register_length;		// 读取长度,字
	
	
	ps=9;
	for (i=0;i<datalength;i++)				//一次只能读一个字
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
		
		*(U8 *)(AD1+4+ps*i)=asicc(a1);		//寄存器地址
		*(U8 *)(AD1+5+ps*i)=asicc(a2);	
		*(U8 *)(AD1+6+ps*i)=asicc(a3);
		*(U8 *)(AD1+7+ps*i)=0x0d;   	//	
		*(U8 *)(AD1+8+ps*i)=0x0a;   	//			
			
		ptcom->send_length[i]=9;		//发送长度
		ptcom->send_staradd[i]=i*ps;	//发送数据存储地址
		ptcom->send_add[i]=t;			//读的是这个地址的数据	
		ptcom->send_data_length[i]=1;	
					
		ptcom->return_length[i]=15;		//返回数据长度,状态 当前值 设定值一起返回
		ptcom->return_start[i]=0;				//返回数据有效开始
		ptcom->return_length_available[i]=5;	//返回有效数据长度								
	}	
	ptcom->send_times=datalength;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->Simens_Count=5;					//表明读的是数据寄存器
}


void Read_Analog()				//读模拟量
{
	switch (ptcom->registerr)	//根据寄存器类型
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


void Read_Recipe()								//读取配方
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
	b=ptcom->address;						// 开始读取地址
	datalength=ptcom->register_length;				// 开始写入长度
	
	ps=17;
	for (i=0;i<datalength;i++)				//一次只能写一个字
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
			
		*(U8 *)(AD1+5+ps*i)=asicc(a1);		//寄存器地址
		*(U8 *)(AD1+6+ps*i)=asicc(a2);	
		*(U8 *)(AD1+7+ps*i)=asicc(a3);
		*(U8 *)(AD1+8+ps*i)=asicc(a4);
		*(U8 *)(AD1+9+ps*i)=0x20;        //SPACE	
			
		b1=ptcom->U8_Data[i*2];       	//数据
		b2=ptcom->U8_Data[i*2+1];       	//数据
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

		*(U8 *)(AD1+15+ps*i)=0x0d;   	//结束符
		*(U8 *)(AD1+16+ps*i)=0x0a;   	//结束符		
				
		ptcom->send_length[i]=17;		//发送长度
		ptcom->send_staradd[i]=ps*i;				//发送数据存储地址
		ptcom->send_add[i]=t;					//读的是这个地址的数据	
		ptcom->send_data_length[i]=1;	//
						
		ptcom->return_length[i]=4;				//返回数据长度
		ptcom->return_start[i]=0;				//返回数据有效开始
		ptcom->return_length_available[i]=0;	//返回有效数据长度	
	}							
	
	ptcom->send_times=datalength;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->Simens_Count=3;					//表明读的是写数据寄存器	
}

void Write_Timer()
{
	int b,t,i,a;
	int a1,a2,a3,a4,a5;
	int b1,b2;
	int datalength;
	int ps;

	a=0;
	b=ptcom->address;						// 开始读取地址
	datalength=ptcom->register_length;				// 开始写入长度
	
	ps=15;
	for (i=0;i<datalength;i++)				//一次只能写一个字
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
	
		*(U8 *)(AD1+4+ps*i)=asicc(a1);		//寄存器地址
		*(U8 *)(AD1+5+ps*i)=asicc(a2);	
		*(U8 *)(AD1+6+ps*i)=asicc(a3);
		*(U8 *)(AD1+7+ps*i)=0x20;        //SPACE	
			
		b1=ptcom->U8_Data[i*2];       	//数据
		b2=ptcom->U8_Data[i*2+1];       	//数据
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

		*(U8 *)(AD1+13+ps*i)=0x0d;   	//结束符
		*(U8 *)(AD1+14+ps*i)=0x0a;   	//结束符		
				
		ptcom->send_length[i]=15;		//发送长度
		ptcom->send_staradd[i]=ps*i;				//发送数据存储地址
		ptcom->send_add[i]=t;					//读的是这个地址的数据	
		ptcom->send_data_length[i]=1;	//
						
		ptcom->return_length[i]=4;				//返回数据长度
		ptcom->return_start[i]=0;				//返回数据有效开始
		ptcom->return_length_available[i]=0;	//返回有效数据长度	
	}							
	
	ptcom->send_times=datalength;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->Simens_Count=3;					//表明读的是写数据寄存器	
}

void Write_Analog()								    //写模拟量
{
	switch (ptcom->registerr)	//根据寄存器类型
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


void Write_Time()                                //写时间到PLC
{
	Write_Analog();									
}


void Read_Time()									//从PLC读取时间
{
	Read_Analog();
}


void Write_Recipe()								//写配方到PLC
{
	int t,i,j,a;
	int a1,a2,a3,a4,a5;
	int ps,staradd;
	int b1,b2;
	int len,LastTimeWord,SendTimes,datalength;
	
	
	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));//数据长度
	staradd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//数据开始地址
	
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
			
		*(U8 *)(AD1+5+ps*i)=asicc(a1);		//寄存器地址
		*(U8 *)(AD1+6+ps*i)=asicc(a2);	
		*(U8 *)(AD1+7+ps*i)=asicc(a3);
		*(U8 *)(AD1+8+ps*i)=asicc(a4);
		*(U8 *)(AD1+9+ps*i)=0x20;        //SPACE	
		b1=*(U8 *)(PE+9+2*i+j*2);       	//数据
		b2=*(U8 *)(PE+9+2*i+j*2+1);       	//数据
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
		ptcom->send_length[i]=17;		//发送长度
		ptcom->send_staradd[i]=ps*i;	//发送数据存储地址
		ptcom->send_add[i]=t;			//读的是这个地址的数据	
		ptcom->send_data_length[i]=1;	//
		ptcom->return_length[i]=4;		//返回数据长度
		ptcom->return_start[i]=0;		//返回数据有效开始
		ptcom->return_length_available[i]=0;//返回有效数据长度								
	}
	
	for (i=0;i<SendTimes;i++)				//KV系列缺陷，定义的最多发送次数是20，若配方长度大于20，则会覆盖后面的数据
	{
		ptcom->return_length[i]=4;
	}
	
	ptcom->send_times=SendTimes;			//发送次数
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->Simens_Count=3;					//表明读的是写数据寄存器
}


void compxy(void)				              //处理成标准存储格式,重新排列
{
	int i;
	int b=0;
	char temp[64];
	char kk[4];
	if (ptcom->Simens_Count==1)	//读的是位,每次都是读一个位
	{
		kk[0]=*(U8 *)(COMad+0);
		kk[0]=bsicc(kk[0]);
		b=ptcom->address%8;			//求地址的余数，为了和主程序一致
		kk[0]=kk[0]<<b;
		*(U8 *)(COMad+0)=kk[0];		//重新存
	}
	else if (ptcom->Simens_Count==2)	//读的是数据寄存器
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
		b=temp[0]*10000+temp[1]*1000+temp[2]*100+temp[3]*10+temp[4];//通过相加的到真实数据
		kk[0]=(b>>8)&0xff;
		kk[1]=b&0xff;
		*(U8 *)(COMad+1000+(ptcom->Current_Times-1)*2)=kk[0];					 //重新存
		*(U8 *)(COMad+1001+(ptcom->Current_Times-1)*2)=kk[1];					 //重新存	
		
		if (ptcom->Current_Times==ptcom->send_times)		//已经发玩,可以把数据给控件了 
		{
			for (i=0;i<ptcom->send_times;i++)				//发几次就获得几个字的数据
			{
				*(U8 *)(COMad+i*2+0)=*(U8 *)(COMad+1000+i*2+0);
				*(U8 *)(COMad+i*2+1)=*(U8 *)(COMad+1000+i*2+1);
			}
		}
		else
		{
			ptcom->IfResultCorrect=0;						//没收到最后一次，不把数据给控件
		}		
	}
	else if (ptcom->Simens_Count==4)	//读的是TC的状态
	{
		kk[0]=*(U8 *)(COMad+0);
		kk[0]=bsicc(kk[0]);
		b=ptcom->address%8;			//求地址的余数，为了和主程序一致
		kk[0]=kk[0]<<b;
		*(U8 *)(COMad+0)=kk[0];		//重新存
	}
	else if (ptcom->Simens_Count==5)	//读的是TC的当前值
	{
		temp[0]=*(U8 *)(COMad+2);		//从第二个返回的是当前值
		temp[1]=*(U8 *)(COMad+3);		
		temp[2]=*(U8 *)(COMad+4);
		temp[3]=*(U8 *)(COMad+5);				
		temp[4]=*(U8 *)(COMad+6);			
		
		temp[0]=bsicc(temp[0]);
		temp[1]=bsicc(temp[1]);				
		temp[2]=bsicc(temp[2]);
		temp[3]=bsicc(temp[3]);
		temp[4]=bsicc(temp[4]);					
		b=temp[0]*10000+temp[1]*1000+temp[2]*100+temp[3]*10+temp[4];//通过相加的到真实数据
		kk[0]=(b>>8)&0xff;
		kk[1]=b&0xff;
		*(U8 *)(COMad+1000+(ptcom->Current_Times-1)*2)=kk[0];					 //重新存
		*(U8 *)(COMad+1001+(ptcom->Current_Times-1)*2)=kk[1];					 //重新存	

		if (ptcom->Current_Times==ptcom->send_times)		//已经发玩,可以把数据给控件了 
		{
			for (i=0;i<ptcom->send_times;i++)				//发几次就获得几个字的数据
			{
				*(U8 *)(COMad+i*2+0)=*(U8 *)(COMad+1000+i*2+0);
				*(U8 *)(COMad+i*2+1)=*(U8 *)(COMad+1000+i*2+1);
			}
		}
		else
		{
			ptcom->IfResultCorrect=0;						//没收到最后一次，不把数据给控件
		}
	}		
}


void watchcom(void)		//检查数据校检
{
	unsigned int aakj=0;
	aakj=remark();
	if(aakj==1)	//读取位
	{
		ptcom->IfResultCorrect=1;
		compxy();		//进入数据处理程序
	}
	else
	{
		ptcom->IfResultCorrect=0;
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
	return 1;				//keyence无校检	
}


unsigned short CalXor(unsigned char *chData,unsigned short uNo)		//异或校检
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
