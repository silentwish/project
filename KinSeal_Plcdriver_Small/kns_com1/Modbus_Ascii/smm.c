#include "stdio.h"
#include "def.h"
#include "smm.h"

unsigned char Check[100]; 

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
				case 'M':								
					Read_Bool();
					break;
				case 'D':
				case 'R':	
				case 't':
				case 'c':	
					Read_Analog();
					break;
				default:
					break;			
			}
			break;
		case PLC_WRITE_DATA:				//进入驱动是写数据
			switch(ptcom->registerr)
			{
				case 'M':			
					Set_Reset(); 
					break;
				case 'D':
				case 'c':			
					Write_Analog();		
					break;
				default:
					break;				
			}
			break;	
		case PLC_WRITE_TIME:				//进入驱动是写时间到PLC
			switch(ptcom->registerr)
			{
				case 'D':				
					Write_Time();		
					break;	
				default:
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
				default:
					break;				
			}
			break;
		case PLC_WRITE_RECIPE:				//进入驱动是写配方到PLC
			switch(*(U8 *)(PE+3))//配方寄存器名称
			{
				case 'D':		
					Write_Recipe();		
					break;
				default:
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
				default:
					break;							
			}
			break;							
		case PLC_CHECK_DATA:				//进入驱动是数据处理
			watchcom();
			break;
		default:
			break;					
	}	 
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Set_Reset()//置位和复位
{
	U16 check;
	int check1,check2;
	int add;
	int a1,a2,a3,a4;
	int plcadd;	

	add=ptcom->address;			     				// 置位地址
	plcadd=ptcom->plc_address;	     				//PLC站地址
	
	*(U8 *)(AD1+0)=0x3a;							//消息帧以冒号(:)为开始
	
	a1=(plcadd>>4)&0xf;
	a2=plcadd&0xf;
	*(U8 *)(AD1+1)=asicc(a1);          				//PLC站地址
	*(U8 *)(AD1+2)=asicc(a2);          		
	
	*(U8 *)(AD1+3)=0x30;            				//功能码0x05，强置置位和复位
	*(U8 *)(AD1+4)=0x35;
	
	a1=(add>>12)&0xf;
	a2=(add>>8)&0xf;
	a3=(add>>4)&0xf;
	a4=add&0xf;
	*(U8 *)(AD1+5)=asicc(a1);              			//开始地址，由高到低	
	*(U8 *)(AD1+6)=asicc(a2);	            		
	*(U8 *)(AD1+7)=asicc(a3);
	*(U8 *)(AD1+8)=asicc(a4);

	if (ptcom->writeValue==1)	    				//置位
	{
		*(U8 *)(AD1+9)=0x46;
		*(U8 *)(AD1+10)=0x46;
		*(U8 *)(AD1+11)=0x30;
		*(U8 *)(AD1+12)=0x30;		
	}
	if (ptcom->writeValue==0)	    				//复位
	{
		*(U8 *)(AD1+9)=0x30;
		*(U8 *)(AD1+10)=0x30;
		*(U8 *)(AD1+11)=0x30;
		*(U8 *)(AD1+12)=0x30;
	}
	
	Check[0]=plcadd;
	Check[1]=0x05;
	Check[2]=(add>>8)&0xff;
	Check[3]=add&0xff;
	if (ptcom->writeValue==1)	    				//置位
	{
		Check[4]=0xff;
		Check[5]=0x00;
	}
	if (ptcom->writeValue==0)	    				//复位
	{
		Check[4]=0x00;
		Check[5]=0x00;			
	}	
	check=CalLRC(Check,6);         					//LRC校验
	check1=(check>>4)&0xf;
	check2=check&0xf;
	*(U8 *)(AD1+13)=asicc(check1);          		//校验 高位
	*(U8 *)(AD1+14)=asicc(check2);          		//校验 低位
	
	*(U8 *)(AD1+15)=0x0d;							//ASCII模式结束字符，回车CR，换行LF
	*(U8 *)(AD1+16)=0x0a;

	if (plcadd==0)								//广播模式
	{
		ptcom->send_length[0]=17;				//发送长度
		ptcom->send_staradd[0]=0;				//发送数据存储地址	
		ptcom->send_times=1;					//发送次数
			
		ptcom->return_length[0]=0;				//返回数据长度
		ptcom->return_start[0]=0;				//返回数据有效开始
		ptcom->return_length_available[0]=0;	//返回有效数据长度	
		ptcom->Current_Times=0;					//当前发送次数	
	}
	else
	{
		ptcom->send_length[0]=17;				//发送长度
		ptcom->send_staradd[0]=0;				//发送数据存储地址	
		ptcom->send_times=1;					//发送次数
			
		ptcom->return_length[0]=17;				//返回数据长度
		ptcom->return_start[0]=0;				//返回数据有效开始
		ptcom->return_length_available[0]=0;	//返回有效数据长度	
		ptcom->Current_Times=0;					//当前发送次数			
	}	
	ptcom->send_staradd[99]=0;		
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Read_Bool()//读取数字量的信息
{
	U16 check;
	int check1,check2;
	int add;
	int a1,a2,a3,a4;
	int plcadd;
	int length;	
				
	add=ptcom->address;								//在主程序已经转换到该段的开始地址
	plcadd=ptcom->plc_address;						//PLC站地址
	length=ptcom->register_length;
	length=length*8;            					//一个元件包涵8位数据

	*(U8 *)(AD1+0)=0x3a;							//消息帧以冒号(:)为开始
	
	a1=(plcadd>>4)&0xf;
	a2=plcadd&0xf;
	*(U8 *)(AD1+1)=asicc(a1);          				//PLC站地址
	*(U8 *)(AD1+2)=asicc(a2);          		
	
	switch (ptcom->registerr)						//根据寄存器类型选择不同功能码
	{
		case 'X':
			*(U8 *)(AD1+3)=0x30;            		//功能码0x02，只读继电器1x
			*(U8 *)(AD1+4)=0x32;
			break;	
		case 'M':
			*(U8 *)(AD1+3)=0x30;            		//功能码0x01，可读可写继电器0x
			*(U8 *)(AD1+4)=0x31;
			break;					
	}	
	
	a1=(add>>12)&0xf;
	a2=(add>>8)&0xf;
	a3=(add>>4)&0xf;
	a4=add&0xf;
	*(U8 *)(AD1+5)=asicc(a1);              			//开始地址，由高到低	
	*(U8 *)(AD1+6)=asicc(a2);	            		
	*(U8 *)(AD1+7)=asicc(a3);
	*(U8 *)(AD1+8)=asicc(a4);                 
	
	a1=(length>>12)&0xf;
	a2=(length>>8)&0xf;
	a3=(length>>4)&0xf;
	a4=length&0xf;
	*(U8 *)(AD1+9)=asicc(a1);              			//读取长度，由高到低	
	*(U8 *)(AD1+10)=asicc(a2);	            		
	*(U8 *)(AD1+11)=asicc(a3);
	*(U8 *)(AD1+12)=asicc(a4);
	
	Check[0]=plcadd&0xff;	 
	switch (ptcom->registerr)						//根据寄存器类型选择不同功能码
	{
		case 'X':
			Check[1]=0x02;
			break;	
		case 'M':
			Check[1]=0x01;
			break;			
	}
	Check[2]=(add>>8)&0xff;
	Check[3]=add&0xff;
	Check[4]=(length>>8)&0xff;
	Check[5]=length&0xff;
	check=CalLRC(Check,6);         					//LRC校验
	check1=(check>>4)&0xf;
	check2=check&0xf;
	*(U8 *)(AD1+13)=asicc(check1);          		//校验 高位
	*(U8 *)(AD1+14)=asicc(check2);          		//校验 低位
	
	*(U8 *)(AD1+15)=0x0d;							//ASCII模式结束字符，回车CR，换行LF
	*(U8 *)(AD1+16)=0x0a;

	if (plcadd==0)								//广播模式
	{	
		ptcom->send_length[0]=17;				     //发送长度
		ptcom->send_staradd[0]=0;				     //发送数据存储地址	
		ptcom->send_times=1;					     //发送次数
			
		ptcom->return_length[0]=0;		     		//返回数据长度，有5个固定,校检
		ptcom->return_start[0]=0;				     //返回数据有效开始
		ptcom->return_length_available[0]=0;    //返回有效数据长度	
		ptcom->Current_Times=0;					     //当前发送次数	
		ptcom->send_add[0]=ptcom->address;		     //读的是这个地址的数据	
		ptcom->address=ptcom->address;
	}
	else
	{
		ptcom->send_length[0]=17;				     //发送长度
		ptcom->send_staradd[0]=0;				     //发送数据存储地址	
		ptcom->send_times=1;					     //发送次数
			
		ptcom->return_length[0]=11+length/4;		     //返回数据长度，有5个固定,校检
		ptcom->return_start[0]=7;				     //返回数据有效开始
		ptcom->return_length_available[0]=length/4;    //返回有效数据长度	
		ptcom->Current_Times=0;					     //当前发送次数	
		ptcom->send_add[0]=ptcom->address;		     //读的是这个地址的数据	
		ptcom->address=ptcom->address;		
	}
	ptcom->send_staradd[99]=1;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Read_Analog()//读模拟量
{
	U16 check;
	int check1,check2;
	int a1,a2,a3,a4;
	int add;
	int plcadd;
	int length;		
				
	add=ptcom->address;			      				//在主程序已经转换到该段的开始地址
	plcadd=ptcom->plc_address;	      				//PLC站地址
	length=ptcom->register_length;
	
	*(U8 *)(AD1+0)=0x3a;							//消息帧以冒号(:)为开始
	
	a1=(plcadd>>4)&0xf;
	a2=plcadd&0xf;
	*(U8 *)(AD1+1)=asicc(a1);          				//PLC站地址
	*(U8 *)(AD1+2)=asicc(a2);          		

	switch (ptcom->registerr)	//根据寄存器类型获得偏移地址
	{
		case 'D':
		case 'c':
			*(U8 *)(AD1+3)=0x30;            		//功能码0x04，可读可写寄存器3x
			*(U8 *)(AD1+4)=0x33;
			break;			
		case 'R':
		case 't':	
			*(U8 *)(AD1+3)=0x30;            		//功能码0x04，只读寄存器4x
			*(U8 *)(AD1+4)=0x34;
			break;				
	}		
	
	a1=(add>>12)&0xf;
	a2=(add>>8)&0xf;
	a3=(add>>4)&0xf;
	a4=add&0xf;
	*(U8 *)(AD1+5)=asicc(a1);              			//开始地址，由高到低	
	*(U8 *)(AD1+6)=asicc(a2);	            		
	*(U8 *)(AD1+7)=asicc(a3);
	*(U8 *)(AD1+8)=asicc(a4);                 
	
	a1=(length>>12)&0xf;
	a2=(length>>8)&0xf;
	a3=(length>>4)&0xf;
	a4=length&0xf;
	*(U8 *)(AD1+9)=asicc(a1);              			//读取长度，由高到低	
	*(U8 *)(AD1+10)=asicc(a2);	            		
	*(U8 *)(AD1+11)=asicc(a3);
	*(U8 *)(AD1+12)=asicc(a4);	
	
	Check[0]=plcadd;	 
	switch (ptcom->registerr)						//根据寄存器类型选择不同功能码
	{
		case 'D':
		case 'c':
			Check[1]=0x03;
			break;	
		case 'R':
		case 't':
			Check[1]=0x04;
			break;			
	}
	Check[2]=(add>>8)&0xff;
	Check[3]=add&0xff;
	Check[4]=(length>>8)&0xff;
	Check[5]=length&0xff;
	check=CalLRC(Check,6);         					//LRC校验
	check1=(check>>4)&0xf;
	check2=check&0xf;
	*(U8 *)(AD1+13)=asicc(check1);          		//校验 高位
	*(U8 *)(AD1+14)=asicc(check2);          		//校验 低位
	
	*(U8 *)(AD1+15)=0x0d;							//ASCII模式结束字符，回车CR，换行LF
	*(U8 *)(AD1+16)=0x0a;	

	if (plcadd==0)								//广播模式
	{	
		ptcom->send_length[0]=17;				//发送长度
		ptcom->send_staradd[0]=0;				//发送数据存储地址	
		ptcom->send_times=1;					//发送次数
			
		ptcom->return_length[0]=0;				//返回数据长度，有5个固定,校检
		ptcom->return_start[0]=0;				//返回数据有效开始
		ptcom->return_length_available[0]=0;	//返回有效数据长度	
		ptcom->Current_Times=0;					//当前发送次数	
		ptcom->send_add[0]=ptcom->address;		//读的是这个地址的数据		
	}
	else
	{
		ptcom->send_length[0]=17;				//发送长度
		ptcom->send_staradd[0]=0;				//发送数据存储地址	
		ptcom->send_times=1;					//发送次数
			
		ptcom->return_length[0]=11+length*4;		//返回数据长度，有5个固定,校检
		ptcom->return_start[0]=7;				//返回数据有效开始
		ptcom->return_length_available[0]=length*4;//返回有效数据长度	
		ptcom->Current_Times=0;					//当前发送次数	
		ptcom->send_add[0]=ptcom->address;		//读的是这个地址的数据		
	}
	
	switch (ptcom->registerr)	//根据寄存器类型返回处理不同
	{
		case 'D':
		case 'R':
			ptcom->send_staradd[99]=1;
			break;			
		case 't':
		case 'c':	
			ptcom->send_staradd[99]=2;
			break;				
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Write_Analog()//写模拟量
{
	U16 check;
	int check1,check2;
	int a1,a2,a3,a4;
	int add;
	int b1,b2,b3,b4,b5,b6,b7,b8;
	int i;
	int length;
	int plcadd;		
				
	add=ptcom->address;								//开始地址
	plcadd=ptcom->plc_address;						//PLC站地址
	length=ptcom->register_length;
	
	*(U8 *)(AD1+0)=0x3a;							//消息帧以冒号(:)为开始
	
	if(length==1)
	{
		a1=(plcadd>>4)&0xf;
		a2=plcadd&0xf;
		*(U8 *)(AD1+1)=asicc(a1);          			//PLC站地址
		*(U8 *)(AD1+2)=asicc(a2);          		
		
		*(U8 *)(AD1+3)=0x30;            			//功能码0x06，写单个寄存器
		*(U8 *)(AD1+4)=0x36;
	
		a1=(add>>12)&0xf;
		a2=(add>>8)&0xf;
		a3=(add>>4)&0xf;
		a4=add&0xf;
		*(U8 *)(AD1+5)=asicc(a1);              		//开始地址，由高到低	
		*(U8 *)(AD1+6)=asicc(a2);	            		
		*(U8 *)(AD1+7)=asicc(a3);
		*(U8 *)(AD1+8)=asicc(a4);	

		a1=ptcom->U8_Data[0];
		b1=(a1>>4)&0xf;
		b2=a1&0xf;
		a2=ptcom->U8_Data[1];
		b3=(a2>>4)&0xf;
		b4=a2&0xf;
		*(U8 *)(AD1+9)=asicc(b3);              		//写入软件值，由高到低	
		*(U8 *)(AD1+10)=asicc(b4);	            		
		*(U8 *)(AD1+11)=asicc(b1);
		*(U8 *)(AD1+12)=asicc(b2);
		
		Check[0]=plcadd;
		Check[1]=0x06;
		Check[2]=(add>>8)&0xff;
		Check[3]=add&0xff;
		Check[4]=a1;
		Check[5]=a2;		
		check=CalLRC(Check,6);         				//LRC校验
		check1=(check>>4)&0xf;
		check2=check&0xf;
		*(U8 *)(AD1+13)=asicc(check1);          	//校验 高位
		*(U8 *)(AD1+14)=asicc(check2);          	//校验 低位
		
		*(U8 *)(AD1+15)=0x0d;						//ASCII模式结束字符，回车CR，换行LF
		*(U8 *)(AD1+16)=0x0a;
		
		if (plcadd==0)								//广播模式
		{
			ptcom->send_length[0]=17;		//发送长度
			ptcom->send_staradd[0]=0;				//发送数据存储地址	
			ptcom->send_times=1;					//发送次数
				
			ptcom->return_length[0]=0;				//返回数据长度
			ptcom->return_start[0]=0;				//返回数据有效开始
			ptcom->return_length_available[0]=0;	//返回有效数据长度	
			ptcom->Current_Times=0;					//当前发送次数	
		}
		else
		{
			ptcom->send_length[0]=17;		//发送长度
			ptcom->send_staradd[0]=0;				//发送数据存储地址	
			ptcom->send_times=1;					//发送次数
				
			ptcom->return_length[0]=17;				//返回数据长度
			ptcom->return_start[0]=0;				//返回数据有效开始
			ptcom->return_length_available[0]=0;	//返回有效数据长度	
			ptcom->Current_Times=0;					//当前发送次数	

		}				
	}
	
	if(length!=1)
	{
		a1=(plcadd>>4)&0xf;
		a2=plcadd&0xf;
		*(U8 *)(AD1+1)=asicc(a1);          			//PLC站地址
		*(U8 *)(AD1+2)=asicc(a2);          		
		
		*(U8 *)(AD1+3)=0x31;            			//功能码0x10，写多个寄存器
		*(U8 *)(AD1+4)=0x30;
	
		a1=(add>>12)&0xf;
		a2=(add>>8)&0xf;
		a3=(add>>4)&0xf;
		a4=add&0xf;
		*(U8 *)(AD1+5)=asicc(a1);              		//开始地址，由高到低	
		*(U8 *)(AD1+6)=asicc(a2);	            		
		*(U8 *)(AD1+7)=asicc(a3);
		*(U8 *)(AD1+8)=asicc(a4);
		
		a1=(length>>12)&0xf;
		a2=(length>>8)&0xf;
		a3=(length>>4)&0xf;
		a4=length&0xf;
		*(U8 *)(AD1+9)=asicc(a1);              		//写入长度(字数)，由高到低	
		*(U8 *)(AD1+10)=asicc(a2);	            		
		*(U8 *)(AD1+11)=asicc(a3);
		*(U8 *)(AD1+12)=asicc(a4);
		
		a1=((length*2)>>4)&0xf;
		a2=(length*2)&0xf;
		*(U8 *)(AD1+13)=asicc(a1);					//写入长度(字节数)
		*(U8 *)(AD1+14)=asicc(a2);	

		switch (ptcom->registerr)
		{
			case 'D':
				for (i=0;i<length;i++)              		//写入多个元件值
				{
					a1=ptcom->U8_Data[i*2];
					b1=(a1>>4)&0xf;
					b2=a1&0xf;
					a2=ptcom->U8_Data[i*2+1];
					b3=(a2>>4)&0xf;
					b4=a2&0xf;
					*(U8 *)(AD1+15+i*4)=asicc(b3);          //写入软件值，由高到低	
					*(U8 *)(AD1+16+i*4)=asicc(b4);	            		
					*(U8 *)(AD1+17+i*4)=asicc(b1);
					*(U8 *)(AD1+18+i*4)=asicc(b2);			
				}
				break;
			case 'c':
				for (i=0;i<length/2;i++)              //写入多个元件值
				{
					a1=ptcom->U8_Data[i*4];
					b1=(a1>>4)&0xf;
					b2=a1&0xf;
					a2=ptcom->U8_Data[i*4+1];
					b3=(a2>>4)&0xf;
					b4=a2&0xf;
					a3=ptcom->U8_Data[i*4+2];
					b5=(a3>>4)&0xf;
					b6=a3&0xf;
					a4=ptcom->U8_Data[i*4+3];
					b7=(a4>>4)&0xf;
					b8=a4&0xf;
					*(U8 *)(AD1+15+i*8)=asicc(b7);          //写入软件值，由高到低	
					*(U8 *)(AD1+16+i*8)=asicc(b8);	            		
					*(U8 *)(AD1+17+i*8)=asicc(b5);
					*(U8 *)(AD1+18+i*8)=asicc(b6);					
					*(U8 *)(AD1+19+i*8)=asicc(b3);          //写入软件值，由高到低	
					*(U8 *)(AD1+20+i*8)=asicc(b4);	            		
					*(U8 *)(AD1+21+i*8)=asicc(b1);
					*(U8 *)(AD1+22+i*8)=asicc(b2);					
				}
				break;
		}
		
		Check[0]=plcadd;
		Check[1]=0x10;
		Check[2]=(add>>8)&0xff;
		Check[3]=add&0xff;
		Check[4]=(length>>8)&0xff;
		Check[5]=length&0xff;		
		Check[6]=((length*2)>>4)&0xff;
		Check[7]=(length*2)&0xff;
		for (i=0;i<length;i++)              		//写入多个元件值
		{
			Check[8+i*2]=ptcom->U8_Data[i*2];
			Check[9+i*2]=ptcom->U8_Data[i*2+1];
		}					
		check=CalLRC(Check,8+length*2); 			//LRC校验
		check1=(check>>4)&0xf;
		check2=check&0xf;
		*(U8 *)(AD1+15+length*4)=asicc(check1);     //校验 高位
		*(U8 *)(AD1+16+length*4)=asicc(check2);     //校验 低位
		
		*(U8 *)(AD1+17+length*4)=0x0d;				//ASCII模式结束字符，回车CR，换行LF
		*(U8 *)(AD1+18+length*4)=0x0a;

		if (plcadd==0)								//广播模式
		{
			ptcom->send_length[0]=19+length*4;		//发送长度
			ptcom->send_staradd[0]=0;				//发送数据存储地址	
			ptcom->send_times=1;					//发送次数
				
			ptcom->return_length[0]=0;				//返回数据长度
			ptcom->return_start[0]=0;				//返回数据有效开始
			ptcom->return_length_available[0]=0;	//返回有效数据长度	
			ptcom->Current_Times=0;					//当前发送次数	
		}
		else
		{
			ptcom->send_length[0]=19+length*4;		//发送长度
			ptcom->send_staradd[0]=0;				//发送数据存储地址	
			ptcom->send_times=1;					//发送次数
				
			ptcom->return_length[0]=17;				//返回数据长度
			ptcom->return_start[0]=0;				//返回数据有效开始
			ptcom->return_length_available[0]=0;	//返回有效数据长度	
			ptcom->Current_Times=0;					//当前发送次数	

		}		
	}
	ptcom->send_staradd[99]=0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Read_Recipe()//读取配方
{
	U16 check;
	int check1,check2;
	int add;	
	int a1,a2,a3,a4;
	int i;
	int datalength;
	int p_start;
	int ps;
	int SendTimes;
	int LastTimeWord;								//最后一次发送长度
	int currentlength;
	int plcadd;
			
	datalength=ptcom->register_length;				//发送总长度
	p_start=ptcom->address;							//开始地址
	plcadd=ptcom->plc_address;
	
	if(datalength>5000)
		datalength=5000;                			//每次最多能发送32个D
		
	if(datalength%32==0)
	{
		SendTimes=datalength/32;
		LastTimeWord=32;                			//固定长度32	
	}
	if(datalength%32!=0)
	{
		SendTimes=datalength/32+1;      			//发送的次数
		LastTimeWord=datalength%32;     			//最后一次发送的长度	
	}
	
	for (i=0;i<SendTimes;i++)
	{
		ps=17*i;
		add=(p_start+i*32);							//加上偏移地址
		
		a1=(plcadd>>4)&0xf;
		a2=plcadd&0xf;		
			
		*(U8 *)(AD1+0+ps)=0x3a;						//消息帧以冒号(:)为开始
		*(U8 *)(AD1+1+ps)=asicc(a1);          			//PLC站地址
		*(U8 *)(AD1+2+ps)=asicc(a2);
		
		*(U8 *)(AD1+3+ps)=0x30;            			//功能码0x03，读寄存器
		*(U8 *)(AD1+4+ps)=0x33;		
	
		a1=(add>>12)&0xf;
		a2=(add>>8)&0xf;
		a3=(add>>4)&0xf;
		a4=add&0xf;
		*(U8 *)(AD1+5+ps)=asicc(a1);              		//开始地址，由高到低	
		*(U8 *)(AD1+6+ps)=asicc(a2);	            		
		*(U8 *)(AD1+7+ps)=asicc(a3);
		*(U8 *)(AD1+8+ps)=asicc(a4);	
	
		if (i!=(SendTimes-1))						//不是最后一次时
		{
			*(U8 *)(AD1+9+ps)=0x30;   				//固定长度32个，即64字节 高位
			*(U8 *)(AD1+10+ps)=0x30;      
			*(U8 *)(AD1+11+ps)=0x32; 				//固定长度32个，即64字节 低位
			*(U8 *)(AD1+12+ps)=0x30;        
			currentlength=32;
		}
		if (i==(SendTimes-1))						//最后一次时
		{
			a1=(LastTimeWord>>12)&0xf;
			a2=(LastTimeWord>>8)&0xf;
			a3=(LastTimeWord>>4)&0xf;
			a4=LastTimeWord&0xf;		
			*(U8 *)(AD1+9+ps)=asicc(a1);   			//剩余长度LastTimeWord个， 高位
			*(U8 *)(AD1+10+ps)=asicc(a2);      
			*(U8 *)(AD1+11+ps)=asicc(a3); 			//剩余长度LastTimeWord个， 低位
			*(U8 *)(AD1+12+ps)=asicc(a4); 		
			currentlength=LastTimeWord;
		}
		
		Check[0]=plcadd;	 
		Check[1]=0x03;
		Check[2]=(add>>8)&0xff;
		Check[3]=add&0xff;
		Check[4]=(currentlength>>8)&0xff;
		Check[5]=currentlength&0xff;
		check=CalLRC(Check,6);         				//LRC校验
		check1=(check>>4)&0xf;
		check2=check&0xf;
		*(U8 *)(AD1+13+ps)=asicc(check1);          	//校验 高位
		*(U8 *)(AD1+14+ps)=asicc(check2);          	//校验 低位
		
		*(U8 *)(AD1+15+ps)=0x0d;					//ASCII模式结束字符，回车CR，换行LF
		*(U8 *)(AD1+16+ps)=0x0a;	

		ptcom->send_length[i]=17;				    //发送长度
		ptcom->send_staradd[i]=i*17;			    //发送数据存储地址	
		ptcom->send_add[i]=p_start+i*32;		    //读的是这个地址的数据	
		ptcom->send_data_length[i]=currentlength;	//不是最后一次都是32个D
				
		ptcom->return_length[i]=1+currentlength*4;	//返回数据长度，有5个固定，plcadd，03，字节数，校检
		ptcom->return_start[i]=7;				    //返回数据有效开始
		ptcom->return_length_available[i]=currentlength*4;	//返回有效数据长度		
	}
	ptcom->send_times=SendTimes;					//发送次数
	ptcom->Current_Times=0;					        //当前发送次数
	ptcom->send_staradd[99]=1;	
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Write_Recipe()//写配方到PLC
{
	int datalength;
	int staradd;
	int SendTimes;
	int LastTimeWord;								//最后一次发送长度
	int i,j;
	int ps;
	int add;
	int a1,a2,a3,a4;
	int b1,b2,b3,b4;
	U16 check;
	int check1,check2;
	int plcadd;
	int length;
	
	
	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));//数据长度
	staradd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//数据地址	
	plcadd=*(U8 *)(PE+4);	      					//PLC站地址		  

	if(datalength%32==0)           					//每次最多能发送32个D，如果是长度是32个D的倍数
	{
		SendTimes=datalength/32;   					//发送的次数
		LastTimeWord=32;           					//最后一次发送的长度	
	}
	if(datalength%32!=0)           					//如果不是长度是32个D的倍数
	{
		SendTimes=datalength/32+1; 					//发送的次数
		LastTimeWord=datalength%32;					//最后一次发送的长度	
	}
			
	ps=147;
	
	for (i=0;i<SendTimes;i++)
	{		
		if (i!=(SendTimes-1))     					//不是一次发送
		{	
			length=32;
		}
		else
		{
			length=LastTimeWord;
		}
		
		*(U8 *)(AD1+0+ps*i)=0x3a;					//消息帧以冒号(:)为开始			
		
		a1=(plcadd>>4)&0xf;
		a2=plcadd&0xf;
		*(U8 *)(AD1+1+ps*i)=asicc(a1);          	//PLC站地址
		*(U8 *)(AD1+2+ps*i)=asicc(a2);          		
		
		*(U8 *)(AD1+3+ps*i)=0x31;            		//功能码0x10，写多个寄存器
		*(U8 *)(AD1+4+ps*i)=0x30;
	
		add=staradd+i*32;           				//起始地址
	
		a1=(add>>12)&0xf;
		a2=(add>>8)&0xf;
		a3=(add>>4)&0xf;
		a4=add&0xf;
		*(U8 *)(AD1+5+ps*i)=asicc(a1);              //开始地址，由高到低	
		*(U8 *)(AD1+6+ps*i)=asicc(a2);	            		
		*(U8 *)(AD1+7+ps*i)=asicc(a3);
		*(U8 *)(AD1+8+ps*i)=asicc(a4);
		
		a1=(length>>12)&0xf;
		a2=(length>>8)&0xf;
		a3=(length>>4)&0xf;
		a4=length&0xf;
		*(U8 *)(AD1+9+ps*i)=asicc(a1);              //写入长度(字数)，由高到低	
		*(U8 *)(AD1+10+ps*i)=asicc(a2);	            		
		*(U8 *)(AD1+11+ps*i)=asicc(a3);
		*(U8 *)(AD1+12+ps*i)=asicc(a4);
		
		a1=((length*2)>>4)&0xf;
		a2=(length*2)&0xf;
		*(U8 *)(AD1+13+ps*i)=asicc(a1);				//写入长度(字节数)
		*(U8 *)(AD1+14+ps*i)=asicc(a2);	

		for (j=0;j<length;j++)              		//写入多个元件值
		{
			a1=*(U8 *)(PE+9+i*64+j*2+1);
			b1=(a1>>4)&0xf;
			b2=a1&0xf;
			a2=*(U8 *)(PE+9+i*64+j*2);
			b3=(a2>>4)&0xf;
			b4=a2&0xf;
			*(U8 *)(AD1+15+j*4+ps*i)=asicc(b1);     //写入软件值，由高到低	
			*(U8 *)(AD1+16+j*4+ps*i)=asicc(b2);	            		
			*(U8 *)(AD1+17+j*4+ps*i)=asicc(b3);
			*(U8 *)(AD1+18+j*4+ps*i)=asicc(b4);			
		}
		
		Check[0]=plcadd;
		Check[1]=0x10;
		Check[2]=(add>>8)&0xff;
		Check[3]=add&0xff;
		Check[4]=(length>>8)&0xff;
		Check[5]=length&0xff;		
		Check[6]=(length*2)&0xff;
		for (j=0;j<length;j++)              		//写入多个元件值
		{
			Check[7+j*2]=*(U8 *)(PE+9+i*64+j*2+1);
			Check[8+j*2]=*(U8 *)(PE+9+i*64+j*2);
		}					
		check=CalLRC(Check,7+length*2); 			//LRC校验
		check1=(check>>4)&0xf;
		check2=check&0xf;
		*(U8 *)(AD1+15+length*4+ps*i)=asicc(check1);//校验 高位
		*(U8 *)(AD1+16+length*4+ps*i)=asicc(check2);//校验 低位
		
		*(U8 *)(AD1+17+length*4+ps*i)=0x0d;			//ASCII模式结束字符，回车CR，换行LF
		*(U8 *)(AD1+18+length*4+ps*i)=0x0a;		

		if (plcadd==0)								//广播模式
		{		
			ptcom->send_length[i]=19+length*4;		//发送长度
			ptcom->send_staradd[i]=i*ps;			//发送数据存储地址
				
			ptcom->return_length[i]=0;				//返回数据长度
			ptcom->return_start[i]=0;				//返回数据有效开始
			ptcom->return_length_available[i]=0;	//返回有效数据长度	
		}
		else
		{
			ptcom->send_length[i]=19+length*4;		//发送长度
			ptcom->send_staradd[i]=i*ps;			//发送数据存储地址
				
			ptcom->return_length[i]=17;				//返回数据长度
			ptcom->return_start[i]=0;				//返回数据有效开始
			ptcom->return_length_available[i]=0;	//返回有效数据长度			
		}			
	}
	ptcom->send_times=SendTimes;					//发送次数
	ptcom->Current_Times=0;					    	//当前发送次数
	ptcom->send_staradd[99]=0;				
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Write_Time()//写时间到PLC
{
	Write_Analog();							
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Read_Time()//从PLC读取时间
{
	Read_Analog();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void compxy(void)//处理成标准存储格式
{
	int i;
	unsigned char a1,a2,a3,a4,a5,a6,a7,a8;
	int b1,b2,b3,b4;

	if (ptcom->send_staradd[99] == 1)
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/2;i++)
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2+0);
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2+1);
			a1=bsicc(a1);
			a2=bsicc(a2);
			b1=(a1<<4)+a2;	
			*(U8 *)(COMad+i)=b1;
		}
		ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;
	}
	if (ptcom->send_staradd[99] == 2)
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/8;i++)
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*8+0);
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*8+1);
			a3=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*8+2);
			a4=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*8+3);
			a5=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*8+4);
			a6=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*8+5);
			a7=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*8+6);
			a8=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*8+7);
			a1=bsicc(a1);
			a2=bsicc(a2);
			a3=bsicc(a3);
			a4=bsicc(a4);
			a5=bsicc(a5);
			a6=bsicc(a6);
			a7=bsicc(a7);
			a8=bsicc(a8);
			b1=(a1<<4)+a2;
			b2=(a3<<4)+a4;
			b3=(a5<<4)+a6;
			b4=(a7<<4)+a8;	
			*(U8 *)(COMad+i*4)=b3;
			*(U8 *)(COMad+i*4+1)=b4;
			*(U8 *)(COMad+i*4+2)=b1;
			*(U8 *)(COMad+i*4+3)=b2;
		}
		ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;	
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int remark()//返回来的数据计算校检码是否正确
{	
	unsigned int Return_Check1;								//返回代码中校验值高位
	unsigned int Return_Check2;								//返回代码中校验值低位
	unsigned int Check_LRC;									//用返回的代码进行校验运算结果
	unsigned int Check1;									//用返回的代码进行校验运算结果高位
	unsigned int Check2;									//用返回的代码进行校验运算结果低位
	int a1,a2;
	int i,b1;

	Return_Check2=*(U8 *)(COMad+ptcom->return_length[ptcom->Current_Times-1]-3)&0xff;//在发送完后Current_Times++，此时要--
	Return_Check1=*(U8 *)(COMad+ptcom->return_length[ptcom->Current_Times-1]-4)&0xff;
	Return_Check1=bsicc(Return_Check1);
	Return_Check2=bsicc(Return_Check2);	
	
	for (i=0;i<(ptcom->return_length[ptcom->Current_Times-1]-5)/2;i++)
	{
		a1=*(U8 *)(COMad+i*2+1);
		a2=*(U8 *)(COMad+i*2+2);
		a1=bsicc(a1);
		a2=bsicc(a2);;
		b1=(a1<<4)+a2;			
		Check[i]=b1;		
	}
		
	Check_LRC=CalLRC(Check,(ptcom->return_length[ptcom->Current_Times-1]-5)/2);
	Check1=((Check_LRC&0xf0)>>4)&0xf;
	Check2=Check_LRC&0xf;
	
	if((Check1==Return_Check1)&&(Check2==Return_Check2))
	{
		return 1;
	}
	else
	{
		return 0;
	}			
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
