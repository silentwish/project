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
		case 'X':
		case 'Y':    //R寄存器
		case 'M':    //MR寄存器
		case 'L':    //LR寄存器
		case 'H':	 //CR寄存器	
		case 'T':
		case 'C':
            
			Read_Bool();   //进入驱动是读位数据       
			break;
		case 'D':
		case 'R':
		case 'N':				
		case 't':
		case 'c':	
			Read_Analog();  //进入驱动是读模拟数据 
			break;			
		}
		break;
	case PLC_WRITE_DATA:				
		switch(ptcom->registerr)
		{
		case 'M':
		case 'L':
		case 'H':		
		case 'Y':
		case 'T':
		case 'C':				
			Set_Reset();      //进入驱动是强置置位和复位
			break;
		case 'D':
		case 'R':
		case 'N':
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
		case 'N':			
			Write_Time();		
			break;			
		}
		break;	
	case PLC_READ_TIME:				//进入驱动是读取时间到PLC
		switch(ptcom->registerr)
		{
		case 'D':
		case 'R':
		case 'N':		
			Read_Time();		
			break;			
		}
		break;
	case PLC_WRITE_RECIPE:				//进入驱动是写配方到PLC
		switch(*(U8 *)(PE+3))//配方寄存器名称
		{
		case 'D':	
		case 'R':
		case 'N':	
			Write_Recipe();		
			break;			
		}
		break;
	case PLC_READ_RECIPE:				//进入驱动是从PLC读取配方
		switch(*(U8 *)(PE+3))//配方寄存器名称
		{
		case 'D':	
		case 'R':	
		case 'N':
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
	case 'M':	//mr
		*(U8 *)(AD1+2)=0x20;
		*(U8 *)(AD1+3)=0x4d;
		*(U8 *)(AD1+4)=0x52;		
		*(U8 *)(AD1+5)=asicc(a1);		//寄存器地址
		*(U8 *)(AD1+6)=asicc(a2);	
		*(U8 *)(AD1+7)=asicc(a3);
		*(U8 *)(AD1+8)=asicc(a4);
		*(U8 *)(AD1+9)=asicc(a5);	
		*(U8 *)(AD1+10)=0x0d;	
		sendlength=11;	
		break;	
	case 'L':	//lr
		*(U8 *)(AD1+2)=0x20;
		*(U8 *)(AD1+3)=0x4c;
		*(U8 *)(AD1+4)=0x52;		
		*(U8 *)(AD1+5)=asicc(a1);		//寄存器地址
		*(U8 *)(AD1+6)=asicc(a2);	
		*(U8 *)(AD1+7)=asicc(a3);
		*(U8 *)(AD1+8)=asicc(a4);
		*(U8 *)(AD1+9)=asicc(a5);	
		*(U8 *)(AD1+10)=0x0d;	
		sendlength=11;	
		break;
	case 'H':	//cr
		*(U8 *)(AD1+2)=0x20;
		*(U8 *)(AD1+3)=0x43;
		*(U8 *)(AD1+4)=0x52;		
		*(U8 *)(AD1+5)=asicc(a1);		//寄存器地址
		*(U8 *)(AD1+6)=asicc(a2);	
		*(U8 *)(AD1+7)=asicc(a3);
		*(U8 *)(AD1+8)=asicc(a4);
		*(U8 *)(AD1+9)=asicc(a5);	
		*(U8 *)(AD1+10)=0x0d;	
		sendlength=11;	
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
	int b,t;
	int a1,a2,a3,a4,a5;
	int h_add,l_add;
	int len;
    int ps = 13;
    int i = 0;
    
	b=ptcom->address;						// 开始读取地址
	len=ptcom->register_length;				// 开始读取长度
	

	
	for (i=0;i<len;i++)
	{
		*(U8 *)(AD1+0 + ps*i)=0x52;        //R
		*(U8 *)(AD1+1 + ps*i)=0x44;        //D
		*(U8 *)(AD1+2 + ps*i)=0x53;		   //S
		*(U8 *)(AD1+3 + ps*i)=0x20;   	   //SPACE 
				
		t=b + i*8;
		h_add=t/16;
		l_add=t-(t/16)*16;		
		
		a1=h_add/100;
		a2=(h_add%100)/10;
		a3=h_add%10;
        
		a4=l_add/10;
		a5=l_add%10;

        
		*(U8 *)(AD1+4 + ps*i)=asicc(a1);		//寄存器地址
		*(U8 *)(AD1+5 + ps*i)=asicc(a2);	
		*(U8 *)(AD1+6 + ps*i)=asicc(a3);
		*(U8 *)(AD1+7 + ps*i)=asicc(a4);
		*(U8 *)(AD1+8 + ps*i)=asicc(a5);	
		*(U8 *)(AD1+9 + ps*i)=0x20;        //SPACE
		*(U8 *)(AD1+10 + ps*i)=asicc((1 * 8)/10);		//长度，8个位KV1000只能读8个位，KV_N60AT能读16个位
		*(U8 *)(AD1+11 + ps*i)=asicc((1 * 8)%10);		//长度，8个位KV1000只能读8个位，KV_N60AT能读16个位
		*(U8 *)(AD1+12 + ps*i)=0x0d;   	       	
			
		ptcom->send_length[i]=13;		                //发送长度
		ptcom->send_staradd[i]=i*ps;			        //发送数据存储地址
        ptcom->send_add[i]=ptcom->address + i*8;        //读的是这个地址的数据	

		ptcom->return_length[i]=1 * 16 + 1;		        //返回数据长度
		ptcom->return_start[i]=0;				        //返回数据有效开始
		ptcom->return_length_available[i]=1 * 16 - 1;	//返回有效数据长度								
	}	
	ptcom->send_times=len;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->Simens_Count=1;					//表明读的是位
}


void ReadM()
{
	int b,t;
	int a1,a2,a3,a4,a5;
	int h_add,l_add;
	int len;
    int i,ps=15;
    
	b=ptcom->address;						// 开始读取地址
	len=ptcom->register_length;				// 开始读取长度

    	
	for (i=0;i<len;i++)
	{

		*(U8 *)(AD1+0+ps*i)=0x52;        //R
		*(U8 *)(AD1+1+ps*i)=0x44;        //D
		*(U8 *)(AD1+2+ps*i)=0x53;		//S
		*(U8 *)(AD1+3+ps*i)=0x20;   	//SPACE 

		*(U8 *)(AD1+4+ps*i)=0x4d;		//M
		*(U8 *)(AD1+5+ps*i)=0x52;   	//R 
				
		t=b+i*8;
		h_add=t/16;
		l_add=t-(t/16)*16;		
		
		a1=h_add/100;
		a2=(h_add%100)/10;
		a3=h_add%10;
		a4=l_add/10;
		
		a5=l_add%10;
		*(U8 *)(AD1+6+ps*i)=asicc(a1);		//寄存器地址
		*(U8 *)(AD1+7+ps*i)=asicc(a2);	
		*(U8 *)(AD1+8+ps*i)=asicc(a3);
		*(U8 *)(AD1+9+ps*i)=asicc(a4);
		*(U8 *)(AD1+10+ps*i)=asicc(a5);	
		*(U8 *)(AD1+11+ps*i)=0x20;        //SPACE
		*(U8 *)(AD1+12+ps*i)=asicc((1 * 8)/10);		//长度，8个位KV1000只能读8个位，KV_N60AT能读16个位
		*(U8 *)(AD1+13+ps*i)=asicc((1 * 8)%10);		//长度，8个位KV1000只能读8个位，KV_N60AT能读16个位
		*(U8 *)(AD1+14+ps*i)=0x0d;   	//	
			
		ptcom->send_length[i]=15;		//发送长度
		ptcom->send_staradd[i]=ps*i;			//发送数据存储地址
		ptcom->send_add[i]=ptcom->address+i*8;				//读的是这个地址的数据	
					
		ptcom->return_length[i]=1 * 16 + 1;				//返回数据长度
		ptcom->return_start[i]=0;				//返回数据有效开始
		ptcom->return_length_available[i]=1 * 16 - 1;	//返回有效数据长度				
    }
    
	ptcom->send_times=len;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->Simens_Count=1;					//表明读的是位
	
}

void ReadL()
{
	int b,t,i;
	int a1,a2,a3,a4,a5;
	int h_add,l_add;
	int len;
    int ps=15;
	
	b=ptcom->address;						// 开始读取地址
	len=ptcom->register_length;				// 开始读取长度
	
	for (i=0;i<len;i++)
	{
		*(U8 *)(AD1+0+ps*i)=0x52;        //R
		*(U8 *)(AD1+1+ps*i)=0x44;        //D
		*(U8 *)(AD1+2+ps*i)=0x53;		//S
		*(U8 *)(AD1+3+ps*i)=0x20;   	    //SPACE 

		*(U8 *)(AD1+4+ps*i)=0x4c;		//L
		*(U8 *)(AD1+5+ps*i)=0x52;     	//R 
				
		t=b+i*8;
		h_add=t/16;
		l_add=t-(t/16)*16;		
		
		a1=h_add/100;
		a2=(h_add%100)/10;
		a3=h_add%10;
		a4=l_add/10;
		
		a5=l_add%10;
		*(U8 *)(AD1+6+ps*i)=asicc(a1);		        //寄存器地址
		*(U8 *)(AD1+7+ps*i)=asicc(a2);	
		*(U8 *)(AD1+8+ps*i)=asicc(a3);
		*(U8 *)(AD1+9+ps*i)=asicc(a4);
		*(U8 *)(AD1+10+ps*i)=asicc(a5);	
		*(U8 *)(AD1+11+ps*i)=0x20;                   //SPACE
		*(U8 *)(AD1+12+ps*i)=asicc((1 * 8)/10);		//长度，8个位KV1000只能读8个位，KV_N60AT能读16个位
		*(U8 *)(AD1+13+ps*i)=asicc((1 * 8)%10);		//长度，8个位KV1000只能读8个位，KV_N60AT能读16个位
		*(U8 *)(AD1+14+ps*i)=0x0d;   	            //	
			
		ptcom->send_length[i]=15;		        //发送长度
		ptcom->send_staradd[i]=ps*i;			    //发送数据存储地址
		ptcom->send_add[i]=ptcom->address+8*i;	//读的是这个地址的数据	
		ptcom->send_data_length[i]=1;	        //不是最后一次都是1个字节
					
		ptcom->return_length[i]=1 * 16 + 1;	        //返回数据长度
		ptcom->return_start[i]=0;				                            //返回数据有效开始
		ptcom->return_length_available[i]=1* 16 -  1;	//返回有效数据长度								
	}	
	ptcom->send_times=len;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->Simens_Count=1;					//表明读的是位
}

void ReadH()
{
	int b,t,i;
	int a1,a2,a3,a4,a5;
	int h_add,l_add;
	int len;
    int ps=15;
	
	b=ptcom->address;						// 开始读取地址
	len=ptcom->register_length;				// 开始读取长度
	
	for (i=0;i<len;i++)
	{
		*(U8 *)(AD1+0+ps*i)=0x52;        //R
		*(U8 *)(AD1+1+ps*i)=0x44;        //D
		*(U8 *)(AD1+2+ps*i)=0x53;		//S
		*(U8 *)(AD1+3+ps*i)=0x20;   	//SPACE 

		*(U8 *)(AD1+4+ps*i)=0x43;		//C
		*(U8 *)(AD1+5+ps*i)=0x52;   	//R 
				
		t=b+i*8;
		h_add=t/16;
		l_add=t-(t/16)*16;		
		
		a1=h_add/100;
		a2=(h_add%100)/10;
		a3=h_add%10;
		a4=l_add/10;
		
		a5=l_add%10;
		*(U8 *)(AD1+6+ps*i)=asicc(a1);		//寄存器地址
		*(U8 *)(AD1+7+ps*i)=asicc(a2);	
		*(U8 *)(AD1+8+ps*i)=asicc(a3);
		*(U8 *)(AD1+9+ps*i)=asicc(a4);
		*(U8 *)(AD1+10+ps*i)=asicc(a5);	
		*(U8 *)(AD1+11+ps*i)=0x20;        //SPACE
		*(U8 *)(AD1+12+ps*i)=asicc((1* 8)/10);		//长度，8个位KV1000只能读8个位，KV_N60AT能读16个位
		*(U8 *)(AD1+13+ps*i)=asicc((1* 8)%10);		//长度，8个位KV1000只能读8个位，KV_N60AT能读16个位
		*(U8 *)(AD1+14+ps*i)=0x0d;   	//	
			
		ptcom->send_length[i]=15;		//发送长度
		ptcom->send_staradd[i]=ps*i;			//发送数据存储地址
		ptcom->send_add[i]=ptcom->address;				//读的是这个地址的数据	
		ptcom->send_data_length[i]=1;	//不是最后一次都是1个字节
					
		ptcom->return_length[i]=1 * 16 + 1;				//返回数据长度
		ptcom->return_start[i]=0;				//返回数据有效开始
		ptcom->return_length_available[i]=1* 16 - 1;	//返回有效数据长度								
	}	
	ptcom->send_times=len;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->Simens_Count=1;					//表明读的是位
}

/****************************
读T时状态，当前值，设定值一起返回
****************************/
void ReadT()
{
	int b,t,i;
	int a1,a2,a3,a4;
	int ps;
	int len;
	
	b=ptcom->address;						// 开始读取地址
	len=ptcom->register_length;				// 开始读取长度,keyence每次只能读一个T，从主程序传过来的值最大长度是1个字节。len始终为1
	
	ps=9;
	for (i=0;i<8;i++)
	{
		*(U8 *)(AD1+0+ps*i)=0x52;        //R
		*(U8 *)(AD1+1+ps*i)=0x44;        //D
		*(U8 *)(AD1+2+ps*i)=0x20;   	//SPACE 

		*(U8 *)(AD1+3+ps*i)=0x54;		//T
				
		t=b+i;
		a1=t/1000;
		a2=t-a1*1000;
		a2=a2/100;
		a3=t-a1*1000-a2*100;
		a3=a3/10;
		a4=t-a1*1000-a2*100-a3*10;
		a4=a4/1;

		
		*(U8 *)(AD1+4+ps*i)=asicc(a1);		//寄存器地址
		*(U8 *)(AD1+5+ps*i)=asicc(a2);	
		*(U8 *)(AD1+6+ps*i)=asicc(a3);
		*(U8 *)(AD1+7+ps*i)=asicc(a4);
		*(U8 *)(AD1+8+ps*i)=0x0d;   	//	
			
		ptcom->send_length[i]=9;		//发送长度
		ptcom->send_staradd[i]=i*ps;			//发送数据存储地址
		ptcom->send_add[i]=t;				//读的是这个地址的数据	
		ptcom->send_data_length[i]=1;	//不是最后一次都是1个字节
					
		ptcom->return_length[i]=25;				//返回数据长度
		ptcom->return_start[i]=0;				//返回数据有效开始
		ptcom->return_length_available[i]=1;	//返回有效数据长度								
	}	
	ptcom->register_length=1;
	ptcom->send_times=8;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->Simens_Count=4;					//表明读的是T的状态
}

void ReadC()
{
	int b,t,i;
	int a1,a2,a3,a4;
	int ps;
	int len;
	
	b=ptcom->address;						// 开始读取地址
	len=ptcom->register_length;				// 开始读取长度,keyence每次只能读一个T，从主程序传过来的值最大长度是1个字节。len始终为1
	
	ps=9;
	for (i=0;i<8;i++)
	{
		*(U8 *)(AD1+0+ps*i)=0x52;        //R
		*(U8 *)(AD1+1+ps*i)=0x44;        //D
		*(U8 *)(AD1+2+ps*i)=0x20;   	//SPACE 

		*(U8 *)(AD1+3+ps*i)=0x43;		//C
				
		t=b+i;
		a1=t/1000;
		a2=t-a1*1000;
		a2=a2/100;
		a3=t-a1*1000-a2*100;
		a3=a3/10;
		a4=t-a1*1000-a2*100-a3*10;
		a4=a4/1;

		
		*(U8 *)(AD1+4+ps*i)=asicc(a1);		//寄存器地址
		*(U8 *)(AD1+5+ps*i)=asicc(a2);	
		*(U8 *)(AD1+6+ps*i)=asicc(a3);
		*(U8 *)(AD1+7+ps*i)=asicc(a4);
		*(U8 *)(AD1+8+ps*i)=0x0d;   	//	
			
		ptcom->send_length[i]=9;		//发送长度
		ptcom->send_staradd[i]=i*ps;			//发送数据存储地址
		ptcom->send_add[i]=t;				//读的是这个地址的数据	
		ptcom->send_data_length[i]=1;	//不是最后一次都是1个字节
					
		ptcom->return_length[i]=25;				//返回数据长度
		ptcom->return_start[i]=0;				//返回数据有效开始
		ptcom->return_length_available[i]=1;	//返回有效数据长度								
	}	
	ptcom->register_length=1;
	ptcom->send_times=8;					//发送次数
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
	case 'M':
		ReadM();
		break;
	case 'L':
		ReadL();
		break;	
	case 'H':
		ReadH();
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
	datalength=ptcom->register_length;				// 开始读取长度
	
	if(datalength%8==0)                       //kenyence最多能读8个D
	{
		SendTimes=datalength/8;               //发送的次数为datalength/8              
		LastTimeWord=8;                       //最后一次发送的长度为8个D
	}
	if(datalength%8!=0)                       //数据长度不是8的倍数时
	{
		SendTimes=datalength/8+1;             //发送的次数datalength/8+1
		LastTimeWord=datalength%8;            //最后一次发送的长度为除8的余数
	}
	
	ps=14;
	for (i=0;i<SendTimes;i++)
	{
		if (i==SendTimes-1)					//最后一次
		{
			len=LastTimeWord;
		}
		else
		{
			len=8;
		}
		*(U8 *)(AD1+0+ps*i)=0x52;        //R
		*(U8 *)(AD1+1+ps*i)=0x44;        //D
		*(U8 *)(AD1+2+ps*i)=0x53;		//S
		*(U8 *)(AD1+3+ps*i)=0x20;   	//SPACE 

		if (ptcom->registerr=='D')
			*(U8 *)(AD1+4+ps*i)=0x44;		//D
		else if (ptcom->registerr=='R')
			*(U8 *)(AD1+4+ps*i)=0x54;		//T	
		else if (ptcom->registerr=='N')
			*(U8 *)(AD1+4+ps*i)=0x45;		//E					
		*(U8 *)(AD1+5+ps*i)=0x4d;   	//M
				
		t=b+i*8;
		a1=t/10000;
		a2=t-a1*10000;
		a2=a2/1000;
		a3=t-a1*10000-a2*1000;
		a3=a3/100;
		a4=t-a1*10000-a2*1000-a3*100;
		a4=a4/10;
		a5=t-a1*10000-a2*1000-a3*100-a4*10;
		
		*(U8 *)(AD1+6+ps*i)=asicc(a1);		//寄存器地址
		*(U8 *)(AD1+7+ps*i)=asicc(a2);	
		*(U8 *)(AD1+8+ps*i)=asicc(a3);
		*(U8 *)(AD1+9+ps*i)=asicc(a4);
		*(U8 *)(AD1+10+ps*i)=asicc(a5);	
		*(U8 *)(AD1+11+ps*i)=0x20;        //SPACE
		*(U8 *)(AD1+12+ps*i)=asicc(len); 
		*(U8 *)(AD1+13+ps*i)=0x0d;   	//	
			
		ptcom->send_length[i]=14;		//发送长度
		ptcom->send_staradd[i]=i*ps;			//发送数据存储地址
		ptcom->send_add[i]=t;				//读的是这个地址的数据	
		ptcom->send_data_length[i]=len;	//不是最后一次都是8个字
					
		ptcom->return_length[i]=len*6+1;		//返回数据长度5个一个字，每个字之间用0x20隔开
		ptcom->return_start[i]=0;				//返回数据有效开始
		ptcom->return_length_available[i]=len*6-1;	//返回有效数据长度								
	}	
	ptcom->send_times=SendTimes;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->Simens_Count=2;					//表明读的是数据寄存器
}

void Read_Timer()	//读TC的当前值
{
	int b,t,i;
	int a1,a2,a3,a4;
	int ps;
	int len;
	
	b=ptcom->address;						// 开始读取地址
	len=ptcom->register_length;				// 开始读取长度,keyence每次只能读一个T
	
	ps=9;
	for (i=0;i<len;i++)
	{
		*(U8 *)(AD1+0+ps*i)=0x52;        //R
		*(U8 *)(AD1+1+ps*i)=0x44;        //D
		*(U8 *)(AD1+2+ps*i)=0x20;   	//SPACE 

		if (ptcom->registerr=='t')
			*(U8 *)(AD1+3+ps*i)=0x54;		//t
		else if (ptcom->registerr=='c')
			*(U8 *)(AD1+3+ps*i)=0x43;		//c		
				
		t=b+i;
		a1=t/1000;
		a2=t-a1*1000;
		a2=a2/100;
		a3=t-a1*1000-a2*100;
		a3=a3/10;
		a4=t-a1*1000-a2*100-a3*10;
		a4=a4/1;

		
		*(U8 *)(AD1+4+ps*i)=asicc(a1);		//寄存器地址
		*(U8 *)(AD1+5+ps*i)=asicc(a2);	
		*(U8 *)(AD1+6+ps*i)=asicc(a3);
		*(U8 *)(AD1+7+ps*i)=asicc(a4);
		*(U8 *)(AD1+8+ps*i)=0x0d;   	//	
			
		ptcom->send_length[i]=9;		//发送长度
		ptcom->send_staradd[i]=i*ps;			//发送数据存储地址
		ptcom->send_add[i]=t;				//读的是这个地址的数据	
		ptcom->send_data_length[i]=1;	//不是最后一次都是1个字节
					
		ptcom->return_length[i]=25;				//返回数据长度
		ptcom->return_start[i]=2;				//返回数据有效开始
		ptcom->return_length_available[i]=10;	//返回有效数据长度								
	}	
	ptcom->register_length=1;
	ptcom->send_times=len;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->Simens_Count=5;					//表明读的是T的当前值
}
//KEYENCE最多能读8个字
void Read_Analog()				//读模拟量
{
	switch (ptcom->registerr)	//根据寄存器类型
	{	
	case 'D':
	case 'R':
	case 'N':		
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
	int b,t,i;
	int a1,a2,a3,a4,a5;
	int ps;
	int len,LastTimeWord,SendTimes,datalength;
	
	b=ptcom->address;						// 开始读取地址
	datalength=ptcom->register_length;				// 开始读取长度
	
	if(datalength%8==0)                       //kenyence最多能读8个D
	{
		SendTimes=datalength/8;               //发送的次数为datalength/8              
		LastTimeWord=8;                       //最后一次发送的长度为8个D
	}
	if(datalength%8!=0)                       //数据长度不是8的倍数时
	{
		SendTimes=datalength/8+1;             //发送的次数datalength/8+1
		LastTimeWord=datalength%8;            //最后一次发送的长度为除8的余数
	}
	
	ps=14;
	for (i=0;i<SendTimes;i++)
	{
		if (i==SendTimes-1)					//最后一次
		{
			len=LastTimeWord;
		}
		else
		{
			len=8;
		}
		*(U8 *)(AD1+0+ps*i)=0x52;        //R
		*(U8 *)(AD1+1+ps*i)=0x44;        //D
		*(U8 *)(AD1+2+ps*i)=0x53;		//S
		*(U8 *)(AD1+3+ps*i)=0x20;   	//SPACE 

		if (ptcom->registerr=='D')
			*(U8 *)(AD1+4+ps*i)=0x44;		//D
		else if (ptcom->registerr=='R')
			*(U8 *)(AD1+4+ps*i)=0x54;		//T	
		else if (ptcom->registerr=='N')
			*(U8 *)(AD1+4+ps*i)=0x45;		//E					
		*(U8 *)(AD1+5+ps*i)=0x4d;   	//M
				
		t=b+i*8;
		a1=t/10000;
		a2=t-a1*10000;
		a2=a2/1000;
		a3=t-a1*10000-a2*1000;
		a3=a3/100;
		a4=t-a1*10000-a2*1000-a3*100;
		a4=a4/10;
		a5=t-a1*10000-a2*1000-a3*100-a4*10;
		
		*(U8 *)(AD1+6+ps*i)=asicc(a1);		//寄存器地址
		*(U8 *)(AD1+7+ps*i)=asicc(a2);	
		*(U8 *)(AD1+8+ps*i)=asicc(a3);
		*(U8 *)(AD1+9+ps*i)=asicc(a4);
		*(U8 *)(AD1+10+ps*i)=asicc(a5);	
		*(U8 *)(AD1+11+ps*i)=0x20;        //SPACE
		*(U8 *)(AD1+12+ps*i)=asicc(len); 
		*(U8 *)(AD1+13+ps*i)=0x0d;   	//	
			
		ptcom->send_length[i]=14;		//发送长度
		ptcom->send_staradd[i]=i*ps;			//发送数据存储地址
		ptcom->send_add[i]=t;				//读的是这个地址的数据	
		ptcom->send_data_length[i]=len;	//不是最后一次都是8个字
					
		ptcom->return_length[i]=len*6+1;		//返回数据长度5个一个字，每个字之间用0x20隔开
		ptcom->return_start[i]=0;				//返回数据有效开始
		ptcom->return_length_available[i]=len*6-1;	//返回有效数据长度								
	}	
	ptcom->send_times=SendTimes;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->Simens_Count=2;					//表明读的是数据寄存器

}

void writeD()
{
	int b,t,i,a;
	int a1,a2,a3,a4,a5;
	int b1,b2;
	int datalength;

	a=0;
	b=ptcom->address;						// 开始读取地址
	datalength=ptcom->register_length;				// 开始写入长度
	
	*(U8 *)(AD1+0)=0x57;        //R
	*(U8 *)(AD1+1)=0x52;        //D
	*(U8 *)(AD1+2)=0x53;		//S
	*(U8 *)(AD1+3)=0x20;   	//SPACE 

	if (ptcom->registerr=='D')
		*(U8 *)(AD1+4)=0x44;		//D
	else if (ptcom->registerr=='R')
		*(U8 *)(AD1+4)=0x54;		//T		
	else if (ptcom->registerr=='N')
		*(U8 *)(AD1+4)=0x45;		//E			
	*(U8 *)(AD1+5)=0x4d;   	//M
				
	t=b;
	a1=t/10000;
	a2=t-a1*10000;
	a2=a2/1000;
	a3=t-a1*10000-a2*1000;
	a3=a3/100;
	a4=t-a1*10000-a2*1000-a3*100;
	a4=a4/10;
	a5=t-a1*10000-a2*1000-a3*100-a4*10;
		
	*(U8 *)(AD1+6)=asicc(a1);		//寄存器地址
	*(U8 *)(AD1+7)=asicc(a2);	
	*(U8 *)(AD1+8)=asicc(a3);
	*(U8 *)(AD1+9)=asicc(a4);
	*(U8 *)(AD1+10)=asicc(a5);	
	*(U8 *)(AD1+11)=0x20;        //SPACE
	*(U8 *)(AD1+12)=asicc(datalength); 
	*(U8 *)(AD1+13)=0x20;        //SPACE		
		
	for (i=0;i<datalength;i++)
	{
		b1=ptcom->U8_Data[i*2];       	//数据
		b2=ptcom->U8_Data[i*2+1];       	//数据
		a=(b2<<8)+b1;
	    a1=a/10000;
	    a2=(a-a1*10000)/1000;
    	a3=(a-a1*10000-a2*1000)/100;
	    a4=(a-a1*10000-a2*1000-a3*100)/10;
		a5=(a-a1*10000-a2*1000-a3*100-a4*10)/1;
		*(U8 *)(AD1+14+6*i)=asicc(a1);
		*(U8 *)(AD1+15+6*i)=asicc(a2);			
		*(U8 *)(AD1+16+6*i)=asicc(a3);
		*(U8 *)(AD1+17+6*i)=asicc(a4);	
		*(U8 *)(AD1+18+6*i)=asicc(a5);			
		*(U8 *)(AD1+19+6*i)=0x20;	//分隔符					    
	}
	*(U8 *)(AD1+14+datalength*6-1)=0x0d;   	//屏蔽掉最后一个0x20	
			
	ptcom->send_length[0]=14+datalength*6;		//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址
	ptcom->send_add[0]=t;					//读的是这个地址的数据	
	ptcom->send_data_length[0]=datalength;	//
					
	ptcom->return_length[0]=4;				//返回数据长度
	ptcom->return_start[0]=0;				//返回数据有效开始
	ptcom->return_length_available[0]=0;	//返回有效数据长度								
	
	ptcom->send_times=1;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->Simens_Count=3;					//表明读的是写数据寄存器	
}

void Write_Timer()
{
	int b,t,a;
	int a1,a2,a3,a4,a5,a6,a7,a8,a9,a10;
	int b1,b2;
	int datalength;

	a=0;
	b=ptcom->address;						// 开始读取地址
	datalength=ptcom->register_length;				// 开始写入长度
	
	*(U8 *)(AD1+0)=0x57;        //R
	*(U8 *)(AD1+1)=0x53;		//S
	*(U8 *)(AD1+2)=0x20;   	//SPACE 

	if (ptcom->registerr=='t')
		*(U8 *)(AD1+3)=0x54;		//T
	else if (ptcom->registerr=='c')
		*(U8 *)(AD1+3)=0x43;		//C		
				
	t=b;
	a1=t/10000;
	a2=t-a1*10000;
	a2=a2/1000;
	a3=t-a1*10000-a2*1000;
	a3=a3/100;
	a4=t-a1*10000-a2*1000-a3*100;
	a4=a4/10;
	a5=t-a1*10000-a2*1000-a3*100-a4*10;
		
	*(U8 *)(AD1+4)=asicc(a1);		//寄存器地址
	*(U8 *)(AD1+5)=asicc(a2);	
	*(U8 *)(AD1+6)=asicc(a3);
	*(U8 *)(AD1+7)=asicc(a4);
	*(U8 *)(AD1+8)=asicc(a5);	
	*(U8 *)(AD1+9)=0x20;        //SPACE	
		
	b1=ptcom->U8_Data[0];       	//数据
	b2=ptcom->U8_Data[1];       	//数据
	a=(b2<<8)+b1;
    a1=a/1000000000;
    a2=(a-a1*1000000000)/100000000;
    a3=(a-a1*1000000000-a2*100000000)/10000000;
    a4=(a-a1*1000000000-a2*100000000-a3*10000000)/1000000;
    a5=(a-a1*1000000000-a2*100000000-a3*10000000-a4*1000000)/100000;
    a6=(a-a1*1000000000-a2*100000000-a3*10000000-a4*1000000-a5*100000)/10000;    
    a7=(a-a1*1000000000-a2*100000000-a3*10000000-a4*1000000-a5*100000-a6*10000)/1000;   
    a8=(a-a1*1000000000-a2*100000000-a3*10000000-a4*1000000-a5*100000-a6*10000-a7*1000)/100; 
    a9=(a-a1*1000000000-a2*100000000-a3*10000000-a4*1000000-a5*100000-a6*10000-a7*1000-a8*100)/10;    
    a10=(a-a1*1000000000-a2*100000000-a3*10000000-a4*1000000-a5*100000-a6*10000-a7*1000-a8*100-a9*10)/1;  
	*(U8 *)(AD1+10)=asicc(a1);
	*(U8 *)(AD1+11)=asicc(a2);			
	*(U8 *)(AD1+12)=asicc(a3);
	*(U8 *)(AD1+13)=asicc(a4);	
	*(U8 *)(AD1+14)=asicc(a5);	
	*(U8 *)(AD1+15)=asicc(a6);
	*(U8 *)(AD1+16)=asicc(a7);			
	*(U8 *)(AD1+17)=asicc(a8);
	*(U8 *)(AD1+18)=asicc(a9);	
	*(U8 *)(AD1+19)=asicc(a10);		
	*(U8 *)(AD1+20)=0x0d;   		
			
	ptcom->send_length[0]=21;				//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址
	ptcom->send_add[0]=t;					//读的是这个地址的数据	
	ptcom->send_data_length[0]=0;	//
					
	ptcom->return_length[0]=4;				//返回数据长度
	ptcom->return_start[0]=0;				//返回数据有效开始
	ptcom->return_length_available[0]=0;	//返回有效数据长度								
	
	ptcom->send_times=1;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->Simens_Count=3;					//表明写tc数据寄存器	
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

	if(datalength%8==0)                       //kenyence最多能读8个D
	{
		SendTimes=datalength/8;               //发送的次数为datalength/8              
		LastTimeWord=8;                       //最后一次发送的长度为8个D
	}
	if(datalength%8!=0)                       //数据长度不是8的倍数时
	{
		SendTimes=datalength/8+1;             //发送的次数datalength/8+1
		LastTimeWord=datalength%8;            //最后一次发送的长度为除8的余数
	}	
	
	ps=62;                                     
	
	for (i=0;i<SendTimes;i++)
	{
		if (i==SendTimes-1)					//最后一次
		{
			len=LastTimeWord;
		}
		else
		{
			len=8;
		}		
		*(U8 *)(AD1+0+ps*i)=0x57;        //R
		*(U8 *)(AD1+1+ps*i)=0x52;        //D
		*(U8 *)(AD1+2+ps*i)=0x53;		//S
		*(U8 *)(AD1+3+ps*i)=0x20;   	//SPACE 

		if (ptcom->registerr=='D')
			*(U8 *)(AD1+4+ps*i)=0x44;		//D
		else if (ptcom->registerr=='R')
			*(U8 *)(AD1+4+ps*i)=0x54;		//T	
		else if (ptcom->registerr=='N')
			*(U8 *)(AD1+4+ps*i)=0x45;		//E					
		*(U8 *)(AD1+5+ps*i)=0x4d;   	//M
					
		t=staradd+i*8;
		a1=t/10000;
		a2=t-a1*10000;
		a2=a2/1000;
		a3=t-a1*10000-a2*1000;
		a3=a3/100;
		a4=t-a1*10000-a2*1000-a3*100;
		a4=a4/10;
		a5=t-a1*10000-a2*1000-a3*100-a4*10;
			
		*(U8 *)(AD1+6+ps*i)=asicc(a1);		//寄存器地址
		*(U8 *)(AD1+7+ps*i)=asicc(a2);	
		*(U8 *)(AD1+8+ps*i)=asicc(a3);
		*(U8 *)(AD1+9+ps*i)=asicc(a4);
		*(U8 *)(AD1+10+ps*i)=asicc(a5);	
		*(U8 *)(AD1+11+ps*i)=0x20;        //SPACE
		*(U8 *)(AD1+12+ps*i)=asicc(len); 
		*(U8 *)(AD1+13+ps*i)=0x20;        //SPACE		
			
		for (j=0;j<len;j++)
		{
			b1=*(U8 *)(PE+9+16*i+j*2);       	//数据
			b2=*(U8 *)(PE+9+16*i+j*2+1);       	//数据
			a=(b2<<8)+b1;
		    a1=a/10000;
		    a2=(a-a1*10000)/1000;
	    	a3=(a-a1*10000-a2*1000)/100;
		    a4=(a-a1*10000-a2*1000-a3*100)/10;
			a5=(a-a1*10000-a2*1000-a3*100-a4*10)/1;
			*(U8 *)(AD1+14+6*j+ps*i)=asicc(a1);
			*(U8 *)(AD1+15+6*j+ps*i)=asicc(a2);			
			*(U8 *)(AD1+16+6*j+ps*i)=asicc(a3);
			*(U8 *)(AD1+17+6*j+ps*i)=asicc(a4);	
			*(U8 *)(AD1+18+6*j+ps*i)=asicc(a5);			
			*(U8 *)(AD1+19+6*j+ps*i)=0x20;	//分隔符					    
		}
		*(U8 *)(AD1+14+len*6-1+ps*i)=0x0d;   	//屏蔽掉最后一个0x20	
				
		ptcom->send_length[i]=14+len*6;		//发送长度
		ptcom->send_staradd[i]=ps*i;				//发送数据存储地址
		ptcom->send_add[i]=t;					//读的是这个地址的数据	
		ptcom->send_data_length[i]=len;	//
						
		ptcom->return_length[i]=4;				//返回数据长度
		ptcom->return_start[i]=0;				//返回数据有效开始
		ptcom->return_length_available[i]=0;	//返回有效数据长度								
	}	
	ptcom->send_times=SendTimes;			//发送次数
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->Simens_Count=3;					//表明读的是写数据寄存器
		
}


void compxy(void)				              //处理成标准存储格式,重新排列
{
	int i;
	int b=0;
	char temp[128];
	char kk[4];
	int j = 0;
	int t = 0; 
    int nIndex=0;
    
	if (ptcom->Simens_Count==1)	//读的是位,每次都是读一个字节
	{
        
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1];i++)	//30 20 30 20 这样的格式返回
		{
			if(*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i) == 0x20)
			{
				continue;
			}
			else
			{
			    temp[j] =*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i);
			    j++;
			}
			
			
		}

		b = 0;
		for(t = 0; t < 8; t++)
		{
			if(temp[t] == 0x31)
			{
				b = ((1 << t) | b) & 0xff;
			}
		}
		
		nIndex = ptcom->Current_Times - 1;
		ptcom->return_length[60+nIndex] = b;//这个寄存器比较特殊、一次只读一个字节，先把数据存入这里、读完后一次放入控件
            

		if(ptcom->send_times != ptcom->Current_Times)
		{
			ptcom->IfResultCorrect = 0;
		}
		else
		{
			for(i = 0; i < ptcom->send_times; i++)
			{
				*(U8 *)(COMad+i)=ptcom->return_length[60+i];
			}
		}	
        
		
	}
	else if (ptcom->Simens_Count==2)	//读的是数据寄存器
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1];i=i+6)	//30 30 30 30 30 20 30 30 30 30 30 20 这样的格式返回
		{
			temp[i+0]=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i+0);
			temp[i+1]=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i+1);		
			temp[i+2]=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i+2);
			temp[i+3]=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i+3);				
			temp[i+4]=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i+4);			
			
			temp[i+0]=bsicc(temp[i+0]);
			temp[i+1]=bsicc(temp[i+1]);				
			temp[i+2]=bsicc(temp[i+2]);
			temp[i+3]=bsicc(temp[i+3]);
			temp[i+4]=bsicc(temp[i+4]);					
			b=temp[i+0]*10000+temp[i+1]*1000+temp[i+2]*100+temp[i+3]*10+temp[i+4];//通过相加的到真实数据
			kk[0]=(b>>8)&0xff;
			kk[1]=b&0xff;
			*(U8 *)(COMad+(i/6)*2)=kk[0];					 //重新存
			*(U8 *)(COMad+(i/6)*2+1)=kk[1];					 //重新存				   
		}
		ptcom->return_length_available[ptcom->Current_Times-1]=1;	
		ptcom->register_length=ptcom->send_data_length[ptcom->Current_Times-1];//返回长度
		ptcom->address=ptcom->send_add[ptcom->Current_Times-1];//回来数据的开始地址			
	}
	else if (ptcom->Simens_Count==4)	//读的是TC的状态
	{
		temp[0]=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+0);//取第一个字节
		temp[0]=bsicc(temp[0]);
		ptcom->U8_Data[50+ptcom->Current_Times-1]=temp[0];		//数据暂存于此
		if(ptcom->Current_Times<ptcom->send_times)
		{
			ptcom->IfResultCorrect=0;		//没到最后一次时，都认为没处理完
		}
		else
		{
			for (i=0;i<8;i++)
			{
				b=(ptcom->U8_Data[50+i]<<i)+b;
			}
			*(U8 *)(COMad+0)=b;		//全部读取1个字节的T后的数据
			ptcom->IfResultCorrect=1;
		}
	}
	else if (ptcom->Simens_Count==5)	//读的是TC的当前值
	{
		for (i=0;i<10;i++)
		{
			temp[i]=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i);
			temp[i]=bsicc(temp[i]);
		}
		b=temp[5]*10000+temp[6]*1000+temp[7]*100+temp[8]*10+temp[9];//通过相加的到真实数据
		kk[0]=(b>>8)&0xff;
		kk[1]=b&0xff;				
		ptcom->U8_Data[20+(ptcom->Current_Times-1)*2+0]=kk[0];		//数据暂存于此
		ptcom->U8_Data[20+(ptcom->Current_Times-1)*2+1]=kk[1];		//数据暂存于此		
		if(ptcom->Current_Times<ptcom->send_times)
		{
			ptcom->IfResultCorrect=0;		//没到最后一次时，都认为没处理完
		}
		else
		{
			for (i=0;i<(ptcom->send_times)*2;i++)
			{
				*(U8 *)(COMad+i)=ptcom->U8_Data[20+i];
			}
			ptcom->IfResultCorrect=1;
		}
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
