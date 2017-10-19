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
		case 'M':
		case 'Y':
		case 'L':
		case 'K':
		case 'H':			
			Read_Bool();   //进入驱动是读位数据       
			break;
		case 'D':
		case 'R':
		case 'N':
		case 't':
		case 'c':
			Read_Analog();  //进入驱动是读模拟数据 
			break;
		default:
			handshake();
			break;			
		}
		break;
	case PLC_WRITE_DATA:				
		switch(ptcom->registerr)
		{
		case 'M':
		case 'Y':
		case 'L':
		case 'K':
		case 'H':			
			Set_Reset();      //进入驱动是强置置位和复位
			break;
		case 'D':
		case 'R':
		case 'N':
		case 't':
		case 'c':
			Write_Analog();	  //进入驱动是写模拟数据	
			break;
		default:
			handshake();
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
			handshake();
			break;			
		}
		break;	
	case PLC_READ_TIME:				//进入驱动是读取时间到PLC
		switch(ptcom->registerr)
		{
		case 'D':		
			Read_Time();		
			break;
		default:
			handshake();
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
			handshake();
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
		default:
			Read_Recipe();
			break;		
		}
		break;							
	case PLC_CHECK_DATA:				//进入驱动是数据处理
		watchcom();
		break;
	case 7:				//进入驱动是数据处理
		handshake();
		break;
	default:
		handshake();
		break;				
	}	 
}


void handshake()
{

}
/*************************************************
Set_Reset置位复位函数
	当判断控件为位控件时，且选择有置位或复位功能时，
	触摸控件，程序跳至此函数收集待发送的数据。
*************************************************/
void Set_Reset()
{
	U16 check;          //校验
	int check_H;		//校验高位
	int check_L;		//校验低位
	int startadd;		//开始地址	
	int startadd_1;	    //开始地址12到15位
	int startadd_2;		//开始地址8到11位
	int plcadd;         //PLC地址
	int k=0;

	startadd=ptcom->address;		// 开始置位地址
	plcadd=ptcom->plc_address;	    //PLC站地址
		
	*(U8 *)(AD1+0)=0x10;				//以下是发送代码段，前42个固定发送      
	*(U8 *)(AD1+1)=0x02;	
	*(U8 *)(AD1+2)=0x00;
	*(U8 *)(AD1+3)=0x00;
	
	*(U8 *)(AD1+4)=0xfc;	        
	*(U8 *)(AD1+5)=0x00;
	*(U8 *)(AD1+6)=0x00;
	*(U8 *)(AD1+7)=0x00;
	
	*(U8 *)(AD1+8)=0x11;       
	*(U8 *)(AD1+9)=0x11;	
	*(U8 *)(AD1+10)=0x00;
	*(U8 *)(AD1+11)=0xff;
		
	*(U8 *)(AD1+12)=0xff;
	*(U8 *)(AD1+13)=0x03;	
	*(U8 *)(AD1+14)=0x00;        
	*(U8 *)(AD1+15)=0x00;
	
	*(U8 *)(AD1+16)=0x00;
	*(U8 *)(AD1+17)=0x00;	
	*(U8 *)(AD1+18)=0x22;
	*(U8 *)(AD1+19)=0x00;
	       
	*(U8 *)(AD1+20)=0x1c;
	*(U8 *)(AD1+21)=0x08;		
	*(U8 *)(AD1+22)=0x0a;
	*(U8 *)(AD1+23)=0x08;
	        
	*(U8 *)(AD1+24)=0x00;
	*(U8 *)(AD1+25)=0x00;	
	*(U8 *)(AD1+26)=0x00;
	*(U8 *)(AD1+27)=0x00;
	        
	*(U8 *)(AD1+28)=0x00;
	*(U8 *)(AD1+29)=0x00;	
	*(U8 *)(AD1+30)=0x00;
	*(U8 *)(AD1+31)=0x00;
	        
	*(U8 *)(AD1+32)=0x14;
	*(U8 *)(AD1+33)=0x02;	
	*(U8 *)(AD1+34)=0x03;   //随机数
	*(U8 *)(AD1+35)=0x00;
	        
	*(U8 *)(AD1+36)=0x00;
	*(U8 *)(AD1+37)=0x00;
	*(U8 *)(AD1+38)=0x00;
	*(U8 *)(AD1+39)=0x01;
	
	*(U8 *)(AD1+40)=0x00;
	*(U8 *)(AD1+41)=0x00;		
	
	switch (ptcom->registerr)       				//根据不同的寄存器名称，发送不同I/O代码
	{
		case 'Y':
			*(U8 *)(AD1+42)=0x9d;			
			break;
		case 'M':
			*(U8 *)(AD1+42)=0x90;
			break;			
		case 'L':
			*(U8 *)(AD1+42)=0x92;
			break;			
		case 'H':
			*(U8 *)(AD1+42)=0x94;
			break;			
		case 'K':
			*(U8 *)(AD1+42)=0xA0;
			break;			
		default:
			break;			
	}
	
	*(U8 *)(AD1+43)=0x00;
	
	startadd_1=(startadd>>8)&0xff;
	startadd_2=(startadd)&0xff;

	if (startadd_2==0x10)
	{
		*(U8 *)(AD1+44)=0x10;
		*(U8 *)(AD1+45)=0x10;
		k++;		
	}
	else 
	{
		*(U8 *)(AD1+44)=startadd_2;
	}
	
	if (startadd_1==0x10)
	{
		*(U8 *)(AD1+45+k)=0x10;
		*(U8 *)(AD1+46+k)=0x10;
		k++;		
	}
	else 
	{
		*(U8 *)(AD1+45+k)=startadd_1;
	}
	
	*(U8 *)(AD1+46+k)=0x00;
	*(U8 *)(AD1+47+k)=0x00;
			
	*(U8 *)(AD1+48+k)=0x00;
	*(U8 *)(AD1+49+k)=0x00;	
	*(U8 *)(AD1+50+k)=0x00;
	*(U8 *)(AD1+51+k)=0x00;
	
	if (ptcom->writeValue==1)	    //置位
	{
		*(U8 *)(AD1+52+k)=0x01;		
	}
	if (ptcom->writeValue==0)	    //复位
	{
		*(U8 *)(AD1+52+k)=0x00;
	}
	
	*(U8 *)(AD1+53+k)=0x00;					//固定发送

	*(U8 *)(AD1+54+k)=0x10;					//结束码10 03
	*(U8 *)(AD1+55+k)=0x03;	
	
	check=CalcHe((U8 *)(AD1+2),52+k)-k*0x10; 	//除去开始字符02后的数进行和校验
	check_H=(check&0xf0)>>4;				//取校验低8位的高位和低位
	check_H=check_H&0xf;
	check_L=check&0xf;	
	*(U8 *)(AD1+56+k)=asicc(check_H);	//发送校验，以asicc码发送，先高后低        
	*(U8 *)(AD1+57+k)=asicc(check_L);
	 	
	ptcom->send_length[0]=58+k;			//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址		
	ptcom->return_length[0]=44;				//返回数据长度
	ptcom->return_start[0]=0;				//返回数据有效开始
	ptcom->return_length_available[0]=0;	//返回有效数据长度	
			
	ptcom->send_times=1;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数
	ptcom->Simens_Count=0;						//返回不处理标志			
}

/*************************************************
Read_Bool读位函数
	当判断控件为位控件时，且选择有监视功能时，
	程序跳至此函数收集待发送的数据。
*************************************************/
void Read_Bool()	
{
	U16 check;          //校验
	int check_H;		//校验高位
	int check_L;		//校验低位
	int startadd;		//开始地址	
	int startadd_1;	    //开始地址12到15位
	int startadd_2;		//开始地址8到11位
	int nSendlen_L;
	int nSendlen_H;
	int plcadd;         //PLC地址	
	int length;			//读取的长度
	int k=0;

	plcadd=ptcom->plc_address;			//PLC站地址	
	length=ptcom->register_length;		//读取的长度
	startadd=ptcom->address;			//开始地址
	
	*(U8 *)(AD1+0)=0x10;				//以下是发送代码段，前38个固定发送      
	*(U8 *)(AD1+1)=0x02;
	
	*(U8 *)(AD1+2)=0x00;
	*(U8 *)(AD1+3)=0x00;
	*(U8 *)(AD1+4)=0xfc;
	        
	*(U8 *)(AD1+5)=0x00;
	*(U8 *)(AD1+6)=0x00;
	*(U8 *)(AD1+7)=0x00;
	*(U8 *)(AD1+8)=0x11;       
	*(U8 *)(AD1+9)=0x11;
	
	*(U8 *)(AD1+10)=0x00;
	*(U8 *)(AD1+11)=0xff;	
	*(U8 *)(AD1+12)=0xff;
	*(U8 *)(AD1+13)=0x03;
	
	*(U8 *)(AD1+14)=0x00;        
	*(U8 *)(AD1+15)=0x00;
	*(U8 *)(AD1+16)=0x00;
	*(U8 *)(AD1+17)=0x00;
	
	*(U8 *)(AD1+18)=0x1a;
	*(U8 *)(AD1+19)=0x00;        
	*(U8 *)(AD1+20)=0x1c;
	*(U8 *)(AD1+21)=0x08;	
	
	*(U8 *)(AD1+22)=0x0a;
	*(U8 *)(AD1+23)=0x08;        
	*(U8 *)(AD1+24)=0x00;
	*(U8 *)(AD1+25)=0x00;
	
	*(U8 *)(AD1+26)=0x00;
	*(U8 *)(AD1+27)=0x00;        
	*(U8 *)(AD1+28)=0x00;
	*(U8 *)(AD1+29)=0x00;
	
	*(U8 *)(AD1+30)=0x00;
	*(U8 *)(AD1+31)=0x00;        
	*(U8 *)(AD1+32)=0x04;
	*(U8 *)(AD1+33)=0x01;
	
	*(U8 *)(AD1+34)=0x1d;   //随机数
	*(U8 *)(AD1+35)=0x00;        
	*(U8 *)(AD1+36)=0x00;
	*(U8 *)(AD1+37)=0x00;		
	
	switch (ptcom->registerr)       				//根据不同的寄存器名称，发送不同I/O代码
	{
		case 'X':
			*(U8 *)(AD1+38)=0x9c;
			break;	
		case 'Y':
			*(U8 *)(AD1+38)=0x9d;			
			break;
		case 'M':
			*(U8 *)(AD1+38)=0x90;
			break;			
		case 'L'://L
			*(U8 *)(AD1+38)=0x92;
			break;			
		case 'H'://V
			*(U8 *)(AD1+38)=0x94;
			break;			
		case 'K'://B
			*(U8 *)(AD1+38)=0xa0;
			break;			
		default:
			break;			
	}
	
	*(U8 *)(AD1+39)=0x00;
	
	startadd=startadd/16*16;//除16取整，每次读16个位，且下位给的地址是8的倍数
	startadd_1=(startadd>>8)&0xff;
	startadd_2=	startadd & 0xff;
	
	if (startadd_2==0x10)
	{
		*(U8 *)(AD1+40)=0x10;
		*(U8 *)(AD1+41)=0x10;
		k++;		
	}
	else 
	{
		*(U8 *)(AD1+40)=startadd_2;
	}
	
	if (startadd_1==0x10)
	{
		*(U8 *)(AD1+41+k)=0x10;
		*(U8 *)(AD1+42+k)=0x10;
		k++;		
	}
	else 
	{
		*(U8 *)(AD1+41+k)=startadd_1;
	}	
	
	*(U8 *)(AD1+42+k)=0x00;
	*(U8 *)(AD1+43+k)=0x00;		
	
	length = (length/2) + 1 ;
    nSendlen_L = (length*16) & 0xff;
    nSendlen_H = ((length*16) >> 8) & 0xff;
    if (nSendlen_L == 0x10)
    {
		*(U8 *)(AD1+44+k)=0x10;
		*(U8 *)(AD1+45+k)=0x10;
        k++;
    }
    else *(U8 *)(AD1+44+k) = nSendlen_L;

    if (nSendlen_H == 0x10)
    {
		*(U8 *)(AD1+45+k)=0x10;
		*(U8 *)(AD1+46+k)=0x10;
        k++;
    }
    else *(U8 *)(AD1+45+k) = nSendlen_H;

	*(U8 *)(AD1+46+k)=0x10;					//结束码10 03
	*(U8 *)(AD1+47+k)=0x03;
	
	check=CalcHe((U8 *)(AD1+2),44+k)-k*0x10; 		//除去开始字符02后的数进行和校验
	check_H=(check&0xf0)>>4;				//取校验低8位的高位和低位
	check_H=check_H&0xf;
	check_L=check&0xf;	
	*(U8 *)(AD1+48+k)=asicc(check_H);		//发送校验，以asicc码发送，先高后低        
	*(U8 *)(AD1+49+k)=asicc(check_L);

	
	ptcom->send_length[0]=50+k;				//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址		
	ptcom->return_length[0]=44 + length * 2;//返回数据长度，当返回有0x10时，返回的数据长度要+1
	ptcom->return_start[0]=40;				//返回数据有效开始
	ptcom->return_length_available[0]=length*2;	//返回有效数据长度
		
	ptcom->register_length=length*2;		//读取的长度,已经改变
			
	ptcom->send_times=1;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->send_add[0]=startadd;		//读的是这个地址的数据
	ptcom->address = ptcom->send_add[0];
	ptcom->Simens_Count=1;						//位返回标志		
}

/*************************************************
Read_Analog读字函数
	当判断控件为字按钮控件时，且选择有监视功能时，
	程序跳至此函数收集待发送的数据。
*************************************************/

void Read_Analog()		//读模拟量
{
	U16 check;          //校验
	int check_H;		//校验高位
	int check_L;		//校验低位
	int startadd;		//开始地址	
	int startadd_1;	    //开始地址12到15位
	int startadd_2;		//开始地址8到11位
	int plcadd;         //PLC地址	
	int length;			//读取的长度
	int k=0;


	plcadd=ptcom->plc_address;			//PLC站地址	
	length=ptcom->register_length;		//读取的长度
	startadd=ptcom->address;			//开始地址                                          
	
	*(U8 *)(AD1+0)=0x10;				//以下是发送代码段，前38个固定发送      
	*(U8 *)(AD1+1)=0x02;	
	*(U8 *)(AD1+2)=0x00;
	*(U8 *)(AD1+3)=0x00;
	
	*(U8 *)(AD1+4)=0xfc;	        
	*(U8 *)(AD1+5)=0x00;
	*(U8 *)(AD1+6)=0x00;
	*(U8 *)(AD1+7)=0x00;
		
	*(U8 *)(AD1+8)=0x11;       
	*(U8 *)(AD1+9)=0x11;	
	*(U8 *)(AD1+10)=0x00;
	*(U8 *)(AD1+11)=0xff;
		
	*(U8 *)(AD1+12)=0xff;
	*(U8 *)(AD1+13)=0x03;	
	*(U8 *)(AD1+14)=0x00;        
	*(U8 *)(AD1+15)=0x00;
	
	*(U8 *)(AD1+16)=0x00;
	*(U8 *)(AD1+17)=0x00;	
	*(U8 *)(AD1+18)=0x1a;
	*(U8 *)(AD1+19)=0x00;
	        
	*(U8 *)(AD1+20)=0x1c;
	*(U8 *)(AD1+21)=0x08;		
	*(U8 *)(AD1+22)=0x0a;
	*(U8 *)(AD1+23)=0x08;
	        
	*(U8 *)(AD1+24)=0x00;
	*(U8 *)(AD1+25)=0x00;	
	*(U8 *)(AD1+26)=0x00;
	*(U8 *)(AD1+27)=0x00;
	        
	*(U8 *)(AD1+28)=0x00;
	*(U8 *)(AD1+29)=0x00;	
	*(U8 *)(AD1+30)=0x00;
	*(U8 *)(AD1+31)=0x00;
	        
	*(U8 *)(AD1+32)=0x04;
	*(U8 *)(AD1+33)=0x01;	
//	*(U8 *)(AD1+34)=ptcom->send_staradd[99]&0xff;   //随机数
	*(U8 *)(AD1+34)=0x2C;   //随机数	
	*(U8 *)(AD1+35)=0x00;
	        
	*(U8 *)(AD1+36)=0x00;
	*(U8 *)(AD1+37)=0x00;
	
	switch (ptcom->registerr)       				//根据不同的寄存器名称，发送不同I/O代码
	{
		case 'D':
			*(U8 *)(AD1+38)=0xa8;
			break;	
		case 'R'://W
			*(U8 *)(AD1+38)=0xB4;			
			break;
		case 'N'://SW
			*(U8 *)(AD1+38)=0xb5;
			break;			
		case 't':
			*(U8 *)(AD1+38)=0xc2;
			break;			
		case 'c':
			*(U8 *)(AD1+38)=0xc5;
			break;	
		default:
			break;	
	}	
	
	*(U8 *)(AD1+39)=0x00;
	
	startadd_1=(startadd>>8)&0xff;
	startadd_2=	(startadd)&0xff;
	
	if (startadd_2==0x10)
	{
		*(U8 *)(AD1+40)=0x10;
		*(U8 *)(AD1+41)=0x10;
		k++;		
	}
	else 
	{
		*(U8 *)(AD1+40)=startadd_2;
	}
	
	if (startadd_1==0x10)
	{
		*(U8 *)(AD1+41+k)=0x10;
		*(U8 *)(AD1+42+k)=0x10;
		k++;		
	}
	else 
	{
		*(U8 *)(AD1+41+k)=startadd_1;
	}	
	
	*(U8 *)(AD1+42+k)=0x00;
	*(U8 *)(AD1+43+k)=0x00;	

	if (length==0x10)
	{
		*(U8 *)(AD1+44+k)=0x10;
		*(U8 *)(AD1+45+k)=0x10;
		k++;			
	}
	else
	{
		*(U8 *)(AD1+44+k)=length&0xff;	
	}
	
	*(U8 *)(AD1+45+k)=0x00;					//固定发送

	*(U8 *)(AD1+46+k)=0x10;					//结束码10 03
	*(U8 *)(AD1+47+k)=0x03;
	
	check=CalcHe((U8 *)(AD1+2),44+k)-k*0x10; 		//除去开始字符02后的数进行和校验
	check_H=(check&0xf0)>>4;						//取校验低8位的高位和低位
	check_H=check_H&0xf;
	check_L=check&0xf;	
	*(U8 *)(AD1+48+k)=asicc(check_H);			//发送校验，以asicc码发送，先高后低        
	*(U8 *)(AD1+49+k)=asicc(check_L);

	
	ptcom->send_length[0]=50+k;				//发送长度
	ptcom->send_staradd[0]=0;						//发送数据存储地址		
	ptcom->return_length[0]=46;						//返回数据长度
	ptcom->return_start[0]=40;						//返回数据有效开始
	ptcom->return_length_available[0]=length*2;			//返回有效数据长度	
		
	ptcom->send_times=1;							//发送次数
	ptcom->Current_Times=0;							//当前发送次数	
	ptcom->send_add[0]=ptcom->address;				//读的是这个地址的数据	

	ptcom->Simens_Count=2;						//字返回标志
	ptcom->send_staradd[99]++;
}

/*************************************************
Write_Analog写字函数
	当判断控件为字按钮控件时，触摸按键，打开键盘，
	输入数值后，程序跳至此函数收集待发送的数据。
*************************************************/

void Write_Analog()		//写模拟量
{
	U16 check;          //校验
	int check_H;		//校验高位
	int check_L;		//校验低位
	int startadd;		//开始地址	
	int startadd_1;	    //开始地址12到15位
	int startadd_2;		//开始地址8到11位
	int plcadd;         //PLC地址	
	int length;			//读取的长度
	int i;				//循环取数用
	int data_H,data_L;  //输入数字高8位和低8位
	int k=0;

	plcadd=ptcom->plc_address;			//PLC站地址	
	length=ptcom->register_length;		//读取的长度
	startadd=ptcom->address;			//开始地址

	*(U8 *)(AD1+0)=0x10;				//以下是发送代码段，前38个固定发送      
	*(U8 *)(AD1+1)=0x02;	
	*(U8 *)(AD1+2)=0x00;
	*(U8 *)(AD1+3)=0x00;
	
	*(U8 *)(AD1+4)=0xfc;	        
	*(U8 *)(AD1+5)=0x00;
	*(U8 *)(AD1+6)=0x00;
	*(U8 *)(AD1+7)=0x00;
		
	*(U8 *)(AD1+8)=0x11;       
	*(U8 *)(AD1+9)=0x11;	
	*(U8 *)(AD1+10)=0x00;
	*(U8 *)(AD1+11)=0xff;
		
	*(U8 *)(AD1+12)=0xff;
	*(U8 *)(AD1+13)=0x03;	
	*(U8 *)(AD1+14)=0x00;        
	*(U8 *)(AD1+15)=0x00;
	
	*(U8 *)(AD1+16)=0x00;
	*(U8 *)(AD1+17)=0x00;	
	*(U8 *)(AD1+18)=0x1a + (length * 2);	//从24开始到最后面的数据长度				
	*(U8 *)(AD1+19)=0x00;
	        
	*(U8 *)(AD1+20)=0x1c;
	*(U8 *)(AD1+21)=0x08;		
	*(U8 *)(AD1+22)=0x0a;
	*(U8 *)(AD1+23)=0x08;
	        
	*(U8 *)(AD1+24)=0x00;
	*(U8 *)(AD1+25)=0x00;	
	*(U8 *)(AD1+26)=0x00;
	*(U8 *)(AD1+27)=0x00;
	        
	*(U8 *)(AD1+28)=0x00;
	*(U8 *)(AD1+29)=0x00;	
	*(U8 *)(AD1+30)=0x00;
	*(U8 *)(AD1+31)=0x00;
	        
	*(U8 *)(AD1+32)=0x14;
	*(U8 *)(AD1+33)=0x01;	
	*(U8 *)(AD1+34)=0x06;   //随机数
	*(U8 *)(AD1+35)=0x00;
	        
	*(U8 *)(AD1+36)=0x00;
	*(U8 *)(AD1+37)=0x00;
	
	switch (ptcom->registerr)       				//根据不同的寄存器名称，发送不同I/O代码
	{
		case 'D':
			*(U8 *)(AD1+38)=0xa8;
			break;	
		case 'R'://w
			*(U8 *)(AD1+38)=0xb4;			
			break;
		case 'N'://sw
			*(U8 *)(AD1+38)=0xb5;
			break;			
		case 't':
			*(U8 *)(AD1+38)=0xc2;
			break;			
		case 'c':
			*(U8 *)(AD1+38)=0xc5;
			break;	
		default:
			break;	
	}		
	
	*(U8 *)(AD1+39)=0x00;
		
	startadd_1=(startadd>>8)&0xff;
	startadd_2=	(startadd)&0xff;	
	
	if (startadd_2==0x10)
	{
		*(U8 *)(AD1+40)=0x10;
		*(U8 *)(AD1+41)=0x10;
		k++;		
	}
	else 
	{
		*(U8 *)(AD1+40)=startadd_2;
	}
	
	if (startadd_1==0x10)
	{
		*(U8 *)(AD1+41+k)=0x10;
		*(U8 *)(AD1+42+k)=0x10;
		k++;		
	}
	else 
	{
		*(U8 *)(AD1+41+k)=startadd_1;
	}	
	
	*(U8 *)(AD1+42+k)=0x00;
	*(U8 *)(AD1+43+k)=0x00;
	
	if (length==0x10)
	{
		*(U8 *)(AD1+44+k)=0x10;
		*(U8 *)(AD1+45+k)=0x10;
		k++;			
	}
	else
	{
		*(U8 *)(AD1+44+k)=length&0xff;	
	}

	*(U8 *)(AD1+45+k)=0x00;					//固定发送

	for (i=0;i<length;i++)							//循环写入多个数值
	{				
		data_L=ptcom->U8_Data[i*2];                 //从输入数值的D[]数组中取数据，对应的data_H为高位，data_L为低位
		data_H=ptcom->U8_Data[i*2+1];			

		if (data_L==0x10)
		{
			*(U8 *)(AD1+46+k+i*2)=0x10;
			*(U8 *)(AD1+47+k+i*2)=0x10;
			k++;
		}
		else
		{
			*(U8 *)(AD1+46+k+i*2)=data_L;
		}
		
		if (data_H==0x10)
		{
			*(U8 *)(AD1+47+k+i*2)=0x10;
			*(U8 *)(AD1+48+k+i*2)=0x10;
			k++;
		}
		else
		{
			*(U8 *)(AD1+47+k+i*2)=data_H;
		}
	}

	*(U8 *)(AD1+46+k+length*2)=0x10;					//结束码10 03
	*(U8 *)(AD1+47+k+length*2)=0x03;
	
	*(U8 *)(AD1+18) = *(U8 *)(AD1+18);				//考虑到有0x10存在，所以多发送的长度在这里要增加
		
	check=CalcHe((U8 *)(AD1+2),44+k+length*2)-k*0x10;   //除去开始字符02后的数进行和校验
	check_H=(check&0xf0)>>4;							//取校验低8位的高位和低位
	check_H=check_H&0xf;
	check_L=check&0xf;	
	*(U8 *)(AD1+48+k+length*2)=asicc(check_H);			//发送校验，以asicc码发送，先高后低        
	*(U8 *)(AD1+49+k+length*2)=asicc(check_L);	
	
	ptcom->send_length[0]=50+k+length*2;				//发送长度
	ptcom->send_staradd[0]=0;						//发送数据存储地址		
	ptcom->return_length[0]=44;						//返回数据长度
	ptcom->return_start[0]=0;						//返回数据有效开始
	ptcom->return_length_available[0]=0;			//返回有效数据长度	
	
	ptcom->send_times=1;							//发送次数
	ptcom->Current_Times=0;							//当前发送次数
	ptcom->Simens_Count=0;						//返回不处理标志		
}
/*************************************************
Read_Recipe读取配方函数
	当组态配置了配方，且触摸从PLC中读取配方按钮时，
	程序跳至此函数收集待发送的数据。
*************************************************/
void Read_Recipe()		//读取配方
{
	U16 check;          //校验
	int check_H;		//校验高位
	int check_L;		//校验低位
	int startadd;		//开始地址	
	int startadd_1;	    //开始地址12到15位
	int startadd_2;		//开始地址8到11位
	int plcadd;         //PLC地址	
	int length;			//读取的长度
	int ps=0;		//每次发送的长度
	int SendTimes;      //发送次数
	int LastTimeWord;	//最后一次发送长度
	int datalength;		//限制长度
	int i = 0,k = 0;				//循环用
	int p_startadd;		//每次开始地址
	



	plcadd=ptcom->plc_address;				//PLC站地址	
	datalength=ptcom->register_length;			//读取的长度
	startadd=ptcom->address;			    //开始地址
	
	if(datalength>5000)                     //限制长度
		datalength=5000;

	if(datalength%64==0)                    //规定每次最多能发64个D，数据刚好是64的倍数时
	{
		SendTimes=datalength/64;            //发送次数
		LastTimeWord=64;                    //最后一次发送的长度为64	
	}
	if(datalength%64!=0)                    //数据不是64的倍数时 
	{
		SendTimes=datalength/64+1;          //发送的次数
		LastTimeWord=datalength%64;         //最后一次发送的长度为除64的余数	
	}

*(U8 *)(AD1+300) = SendTimes + 67;


	ps = 0;
	for (i=0;i<SendTimes;i++)
	{	
		p_startadd=startadd+i*64;			//每次开始的地址往后靠64个字
		
		*(U8 *)(AD1+0+ps)=0x10;				//以下是发送代码段，前38个固定发送      
		*(U8 *)(AD1+1+ps)=0x02;	
		*(U8 *)(AD1+2+ps)=0x00;
		*(U8 *)(AD1+3+ps)=0x00;
		
		*(U8 *)(AD1+4+ps)=0xfc;	        
		*(U8 *)(AD1+5+ps)=0x00;
		*(U8 *)(AD1+6+ps)=0x00;
		*(U8 *)(AD1+7+ps)=0x00;
			
		*(U8 *)(AD1+8+ps)=0x11;       
		*(U8 *)(AD1+9+ps)=0x11;	
		*(U8 *)(AD1+10+ps)=0x00;
		*(U8 *)(AD1+11+ps)=0xff;
			
		*(U8 *)(AD1+12+ps)=0xff;
		*(U8 *)(AD1+13+ps)=0x03;	
		*(U8 *)(AD1+14+ps)=0x00;        
		*(U8 *)(AD1+15+ps)=0x00;
		
		*(U8 *)(AD1+16+ps)=0x00;
		*(U8 *)(AD1+17+ps)=0x00;	
		*(U8 *)(AD1+18+ps)=0x1a;
		*(U8 *)(AD1+19+ps)=0x00;
		        
		*(U8 *)(AD1+20+ps)=0x1c;
		*(U8 *)(AD1+21+ps)=0x08;		
		*(U8 *)(AD1+22+ps)=0x0a;
		*(U8 *)(AD1+23+ps)=0x08;
		        
		*(U8 *)(AD1+24+ps)=0x00;
		*(U8 *)(AD1+25+ps)=0x00;	
		*(U8 *)(AD1+26+ps)=0x00;
		*(U8 *)(AD1+27+ps)=0x00;
		        
		*(U8 *)(AD1+28+ps)=0x00;
		*(U8 *)(AD1+29+ps)=0x00;	
		*(U8 *)(AD1+30+ps)=0x00;
		*(U8 *)(AD1+31+ps)=0x00;
		        
		*(U8 *)(AD1+32+ps)=0x04;
		*(U8 *)(AD1+33+ps)=0x01;	
		*(U8 *)(AD1+34+ps)=0x29;   //随机数
		*(U8 *)(AD1+35+ps)=0x00;
		        
		*(U8 *)(AD1+36+ps)=0x00;
		*(U8 *)(AD1+37+ps)=0x00;
		
		switch (ptcom->registerr)       				//根据不同的寄存器名称，发送不同I/O代码
		{
			case 'D':
				*(U8 *)(AD1+38+ps)=0xa8;
				break;	
			case 'R':
				*(U8 *)(AD1+38+ps)=0xcc;			
				break;
			case 'N':
				*(U8 *)(AD1+38+ps)=0xb4;
				break;			
			case 't':
				*(U8 *)(AD1+38+ps)=0xc2;
				break;			
			case 'c':
				*(U8 *)(AD1+38+ps)=0xc5;
				break;	
			default:
				break;	
		}	
		
		*(U8 *)(AD1+39+ps)=0x00;		
			
		startadd_1=(p_startadd>>8)&0xff;
		startadd_2=	(p_startadd)&0xff;
		
		if (startadd_2==0x10)
		{
			*(U8 *)(AD1+40+ps)=0x10;
			*(U8 *)(AD1+41+ps)=0x10;
			k++;		
		}
		else 
		{
			*(U8 *)(AD1+40+ps)=startadd_2;
		}
		
		if (startadd_1==0x10)
		{
			*(U8 *)(AD1+41+k+ps)=0x10;
			*(U8 *)(AD1+42+k+ps)=0x10;
			k++;		
		}
		else 
		{
			*(U8 *)(AD1+41+k+ps)=startadd_1;
		}	
		
		*(U8 *)(AD1+42+k+ps)=0x00;
		*(U8 *)(AD1+43+k+ps)=0x00;			
		
		if (i!=((SendTimes/2)-1))   //不是最后一次发送时
		{
			length=64;              //每次发的是32个字
		}
		if (i==((SendTimes/2)-1))   //最后依次发的时候
		{
			length=LastTimeWord;    //发的长度是所剩下的字
		}		
		
		if (length==0x10)
		{
			*(U8 *)(AD1+44+k+ps)=0x10;
			*(U8 *)(AD1+45+k+ps)=0x10;
			k++;			
		}
		else
		{
			*(U8 *)(AD1+44+k+ps)=length&0xff;	
		}
		
		*(U8 *)(AD1+45+k+ps)=0x00;					//固定发送

		*(U8 *)(AD1+46+k+ps)=0x10;					//结束码10 03
		*(U8 *)(AD1+47+k+ps)=0x03;
		
		check=CalcHe((U8 *)(AD1+2+ps),44+k)-k*0x10; 		//除去开始字符02后的数进行和校验
		check_H=(check&0xf0)>>4;						//取校验低8位的高位和低位
		check_H=check_H&0xf;
		check_L=check&0xf;	
		*(U8 *)(AD1+48+k+ps)=asicc(check_H);			//发送校验，以asicc码发送，先高后低        
		*(U8 *)(AD1+49+k+ps)=asicc(check_L);		
			
		ptcom->send_length[0+i]=50+k;						//发送长度
		ptcom->send_staradd[0+i]=ps;						//发送数据存储地址		
		ptcom->return_length[0+i]=46;						//返回数据长度
		ptcom->return_start[0+i]=40;						//返回数据有效开始
		ptcom->return_length_available[0+i]=length*2;			//返回有效数据长度	
		ptcom->send_add[0+i]=ptcom->address+i*64;			//读的是这个地址的数据	
		
		ps=ps+50+k;						//累计发送的长度	
	}			
	ptcom->send_times=SendTimes;							//发送次数
	ptcom->Current_Times=0;									//当前发送次数		

	ptcom->Simens_Count=2;						//字返回标志	
}

/*************************************************
Write_Recipe写配方函数
	当组态配置了配方，且触摸写配方到PLC按钮时，
	程序跳至此函数收集待发送的数据。
*************************************************/

void Write_Recipe()		//写配方到PLC
{
	U16 check;          //校验
	int check_H;		//校验高位
	int check_L;		//校验低位
	int startadd;		//开始地址	
	int startadd_1;	    //开始地址12到15位
	int startadd_2;		//开始地址8到11位
	int p_startadd;		//每次开始地址
	int plcadd;         //PLC地址	
	int length;			//读取的长度
	int data_H,data_L;  //输入数字高8位和低8位
	int datalength;		//限制长度
	int SendTimes;      //发送次数
	int LastTimeWord;	//最后一次发送长度	
	int ps=0;		//每次发送的长度
	int i = 0;				//循环用
	int j = 0,k = 0;
	int regname;

	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));											//数据长度
	startadd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));	//数据开始地址
	plcadd=*(U8 *)(PE+4);
	
	switch (*(U8 *)(PE+3))       				//根据不同的寄存器名称，发送不同I/O代码
	{
		case 'D':
			regname=0xa8;
			break;	
		case 'R':
			regname=0xcc;			
			break;
		case 'N':
			regname=0xb4;
			break;				
		default:
			regname=0xa8;
			break;	
	}	

	if(datalength>5000)                     //限制长度
		datalength=5000;

	if(datalength%64==0)                    //规定每次最多能发64个D，数据刚好是64的倍数时
	{
		SendTimes=datalength/64;            //发送次数
		LastTimeWord=64;                    //最后一次发送的长度为64	
	}
	if(datalength%64!=0)                    //数据不是64的倍数时 
	{
		SendTimes=datalength/64+1;          //发送的次数
		LastTimeWord=datalength%64;         //最后一次发送的长度为除64的余数	
	}
	
	ps=0;								//前面发送的长度
	
	for (i=0;i<SendTimes;i++)
	{
		k = 0;			//复位k
		*(U8 *)(AD1+0+ps)=0x10;				//以下是发送代码段，前38个固定发送      
		*(U8 *)(AD1+1+ps)=0x02;	
		*(U8 *)(AD1+2+ps)=0x00;
		*(U8 *)(AD1+3+ps)=0x00;
		
		*(U8 *)(AD1+4+ps)=0xfc;	        
		*(U8 *)(AD1+5+ps)=0x00;
		*(U8 *)(AD1+6+ps)=0x00;
		*(U8 *)(AD1+7+ps)=0x00;
			
		*(U8 *)(AD1+8+ps)=0x11;       
		*(U8 *)(AD1+9+ps)=0x11;	
		*(U8 *)(AD1+10+ps)=0x00;
		*(U8 *)(AD1+11+ps)=0xff;
			
		*(U8 *)(AD1+12+ps)=0xff;
		*(U8 *)(AD1+13+ps)=0x03;	
		*(U8 *)(AD1+14+ps)=0x00;        
		*(U8 *)(AD1+15+ps)=0x00;
		
		*(U8 *)(AD1+16+ps)=0x00;
		*(U8 *)(AD1+17+ps)=0x00;	
		*(U8 *)(AD1+18+ps)=0x1a;				//预先设定后面发送长度为初始值
		*(U8 *)(AD1+19+ps)=0x00;
		        
		*(U8 *)(AD1+20+ps)=0x1c;
		*(U8 *)(AD1+21+ps)=0x08;		
		*(U8 *)(AD1+22+ps)=0x0a;
		*(U8 *)(AD1+23+ps)=0x08;
		        
		*(U8 *)(AD1+24+ps)=0x00;
		*(U8 *)(AD1+25+ps)=0x00;	
		*(U8 *)(AD1+26+ps)=0x00;
		*(U8 *)(AD1+27+ps)=0x00;
		        
		*(U8 *)(AD1+28+ps)=0x00;
		*(U8 *)(AD1+29+ps)=0x00;	
		*(U8 *)(AD1+30+ps)=0x00;
		*(U8 *)(AD1+31+ps)=0x00;
		        
		*(U8 *)(AD1+32+ps)=0x14;
		*(U8 *)(AD1+33+ps)=0x01;	
		*(U8 *)(AD1+34+ps)=0x04;   //随机数
		*(U8 *)(AD1+35+ps)=0x00;
		        
		*(U8 *)(AD1+36+ps)=0x00;
		*(U8 *)(AD1+37+ps)=0x00;
		

		*(U8 *)(AD1+38+ps)=regname;
		
		*(U8 *)(AD1+39+ps)=0x00;	//fix
				
		p_startadd=startadd+i*64;
		
		startadd_1=(p_startadd>>8)&0xff;
		startadd_2=	(p_startadd)&0xff;	
		
		if (startadd_2==0x10)
		{
			*(U8 *)(AD1+40+ps)=0x10;
			*(U8 *)(AD1+41+ps)=0x10;
			k++;		
		}
		else 
		{
			*(U8 *)(AD1+40+ps)=startadd_2;
		}
		
		if (startadd_1==0x10)
		{
			*(U8 *)(AD1+41+k+ps)=0x10;
			*(U8 *)(AD1+42+k+ps)=0x10;
			k++;		
		}
		else 
		{
			*(U8 *)(AD1+41+k+ps)=startadd_1;
		}	
		
		*(U8 *)(AD1+42+k+ps)=0x00;
		*(U8 *)(AD1+43+k+ps)=0x00;
		
		if (i==SendTimes-1)	//最后一次
		{
			length=LastTimeWord;
		}
		else
		{
			length=64;
		}			
		
		if (length==0x10)
		{
			*(U8 *)(AD1+44+k+ps)=0x10;
			*(U8 *)(AD1+45+k+ps)=0x10;
			k++;			
		}
		else
		{
			*(U8 *)(AD1+44+k+ps)=length&0xff;	
		}

		*(U8 *)(AD1+45+k+ps)=0x00;					//固定发送	
		
		for (j=0;j<length;j++)							//循环写入多个数值
		{				
			data_L=*(U8 *)(PE+9+j*2+64*i*2);                 //从输入数值的D[]数组中取数据，对应的data_H为高位，data_L为低位
			data_H=*(U8 *)(PE+10+j*2+64*i*2);			

			if (data_L==0x10)
			{
				*(U8 *)(AD1+46+k+j*2+ps)=0x10;
				*(U8 *)(AD1+47+k+j*2+ps)=0x10;
				k++;
			}
			else
			{
				*(U8 *)(AD1+46+k+j*2+ps)=data_L;
			}
			
			if (data_H==0x10)
			{
				*(U8 *)(AD1+47+k+j*2+ps)=0x10;
				*(U8 *)(AD1+48+k+j*2+ps)=0x10;
				k++;
			}
			else
			{
				*(U8 *)(AD1+47+k+j*2+ps)=data_H;
			}
		}

		*(U8 *)(AD1+46+k+length*2+ps)=0x10;					//结束码10 03
		*(U8 *)(AD1+47+k+length*2+ps)=0x03;
		
		
		*(U8 *)(AD1+18+ps) = 0x1a + (length * 2);		
		
		check=CalcHe((U8 *)(AD1+2+ps),44+k+length*2)-k*0x10;   //除去开始字符02后的数进行和校验	

		check_H = (check >> 4) & 0xf;
		check_L = (check >> 0) & 0xf;		
	
		*(U8 *)(AD1+48+k+length*2+ps)=asicc(check_H);			//发送校验，以asicc码发送，先高后低        
		*(U8 *)(AD1+49+k+length*2+ps)=asicc(check_L);		
		
		ptcom->send_length[i]=50+k+length*2;				//发送长度
		ptcom->send_staradd[i]=ps;						//发送数据存储地址		
		ptcom->return_length[i]=44;						//返回数据长度
		ptcom->return_start[i]=0;						//返回数据有效开始
		ptcom->return_length_available[i]=0;			//返回有效数据长度	
		
		ps=ps+50+k+length*2;//累计发送长度
	}	
	ptcom->send_times=SendTimes;							//发送次数
	ptcom->Current_Times=0;									//当前发送次数	
	ptcom->Simens_Count=0;						//返回不处理标志			
}

/*************************************************
Write_Time写时间函数
	当组在时钟里设置了时钟选项，
	程序跳至此函数收集待发送的数据。
*************************************************/
void Write_Time()			//写时间到PLC
{
	Write_Analog();			//跳转到写模拟两函数									
}

/*************************************************
Read_Time读时间函数
	当组在时钟里设置了时钟选项，
	程序跳至此函数收集待发送的数据。
*************************************************/
void Read_Time()				//从PLC读取时间
{
	Read_Analog();			//跳转到读模拟两函数
}

/*************************************************
compxy数据处理函数
	由于不同的PLC返回的数值大不相同，HMI不能识别，
	故需此函数来进行必要的转换，转换成HMI所需的数值。
*************************************************/
void compxy(void)				              	//数据处理函数
{
	int i,k,j;
	unsigned char a1;
	k=0;

	if (ptcom->Simens_Count==1)//位返回处理
	{
		j = ptcom->return_length_available[ptcom->Current_Times-1];		//有效数据长度
		k = 0;		
		for(i=0;i<j;i++)		//16进制返回
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i);	
			*(U8 *)(COMad+k)=a1;	
			k++;	
			if (a1 == 0x10)
			{
				i++;		//下一个不取
				j++;		//有效扩展长度
			}								
		}		
	}
	if (ptcom->Simens_Count==2)//字返回处理
	{
		j = ptcom->return_length_available[ptcom->Current_Times-1];		//有效数据长度
		k = 0;
		for(i=0;i<j;i++)		//16进制返回
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i);
			*(U8 *)(COMad+k)=a1;	
			k++;	
			if (a1 == 0x10)
			{
				i++;		//下一个不取
				j++;		//有效扩展长度
			}					
		}	
	}	
}

/*************************************************
watchcom检查数据校检函数
	由于不同的PLC使用的校验大不相同，
	故需此函数来进行必要的数据校检。以验证数据是否正常
*************************************************/
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

/*************************************************
asicc Asc码转换函数
	有些PLC在进行数据的交流时，使用的是Asc码，故需此函数来转换成Asc码
	整型的参数a：需转换成Asc码的数值
*************************************************/
int asicc(int result)			//转为Asc码
{
	int temp;
	if(result<10)
		temp=result+0x30;
	if(result>9)
		temp=result-10+0x41;
	return temp;	
}

/*************************************************
asicc 数字转换函数
	有些PLC在进行数据的交流时，使用的是Asc码，
	但在数据的处理时又需要数据为数字格式，故需此函数来转换
	整型的参数a：需转换成Asc码的数值
*************************************************/
int bsicc(int result)			//Asc转为数字
{
	int temp=0;
	if(result>=0x30)
	{
		if(result<0x40)
			temp=result-0x30;
		if(result>0x40)
			temp=result-0x41+10;
	}
	return temp;
}

/*************************************************
remark 检查校验函数
	此函数为检查校验是否正确，返回1则收到的数据正常，返回0则收到的数据不正常
*************************************************/
int remark()				//返回来的数据计算校检码是否正确
{
	if (ptcom->IQ<40)		//返回长度不够
	{
		return 0;
	}
	return 1;	
}

/*************************************************
CalcHe 和校验函数
	此函数是对一数据列进行求和
*************************************************/
U16 CalcHe(unsigned char *chData,U16 uNo)		//计算和校检
{
	int i;
	int sun=0;
	for(i=0;i<uNo;i++)
	{
		sun=sun+chData[i];
	}
	return (sun);
}
