/***********************************************************************************************************************
  修改内容：台达位字读取最多能读15个word，位也需要分批写，越1536分界暂不做处理
*************************************************************************************************************************/
#include "stdio.h"
#include "def.h"
#include "smm.h"
struct Com_struct_D *ptcom;


unsigned char KK[100];

/************************************************************************************************************************
驱动进入函数，根据功能进入不同的处理子函数
*************************************************************************************************************************/
void Enter_PlcDriver(void)
{
	ptcom=(struct Com_struct_D *)adcom;	 
	//sysprintf("dvp enter plc driver=%c\n",ptcom->registerr);
	switch (ptcom->R_W_Flag)
	{
		case PLC_READ_DATA:								//进入驱动是读数据
		case PLC_READ_DIGITALALARM:							//进入驱动是读数据,报警	
		case PLC_READ_TREND:							//进入驱动是读数据,趋势图
		case PLC_READ_ANALOGALARM:							//进入驱动是读数据,类比报警	
		case PLC_READ_CONTROL:							//进入驱动是读PLC控制的数据	
			switch(ptcom->registerr)
			{
				case 'X':
				case 'Y':
				case 'M':
				case 'T':
				case 'C':
				case 'H':
					ptcom->Simens_Count = 0;						
					Read_Bool();   			//进入驱动是读位数据       
					break;
				case 'D':
				case 't':
				case 'c':
					ptcom->Simens_Count = 0;	
					Read_Analog();  			//进入驱动是读模拟数据 
					break;			
			}
			break;
		case PLC_WRITE_DATA:				
			switch(ptcom->registerr)
			{
				case 'M':
				case 'Y':
				case 'T':
				case 'C':
				case 'H':
					ptcom->Simens_Count = 0;				
					Set_Reset();      		//进入驱动是强置置位和复位
					break;
				case 'D':
				case 't':
				case 'c':
					ptcom->Simens_Count = 0;
					Write_Analog();	  		//进入驱动是写模拟数据	
					break;			
			}
			break;	
		case PLC_WRITE_TIME:								//进入驱动是写时间到PLC
			switch(ptcom->registerr)
			{
				case 'D':
					ptcom->Simens_Count = 0;
					Write_Time();		
					break;			
			}
			break;	
		case PLC_READ_TIME:								//进入驱动是读取时间到PLC
			switch(ptcom->registerr)
			{
				case 'D':
					ptcom->Simens_Count = 0;		
					Read_Time();		
					break;			
			}
			break;
		case PLC_WRITE_RECIPE:								//进入驱动是写配方到PLC
			switch(*(U8 *)(PE+3))			//配方寄存器名称
			{
				case 'D':
					ptcom->Simens_Count = 0;		
					Write_Recipe();		
					break;			
			}
			break;
		case PLC_READ_RECIPE:								//进入驱动是从PLC读取配方
			switch(*(U8 *)(PE+3))			//配方寄存器名称
			{
				case 'D':
					ptcom->Simens_Count = 0;		
					Read_Recipe();		
					break;			
			}
			break;							
		case PLC_CHECK_DATA:								//进入驱动是数据处理
			watchcom();
			break;				
	}	 
}
/************************************************************************************************************************
置位复位函数
*************************************************************************************************************************/
void Set_Reset()
{
	U16 aakj;
	int b,b1,b2;
	int a1,a2,a3,a4;
	int add;
	int t;	
	int plcadd;
	int i;

	b=ptcom->address;						// 开始置位地址
	plcadd=ptcom->plc_address;	            //PLC站地址
	switch (ptcom->registerr)				//根据寄存器类型获得偏移地址
	{
	case 'Y':
		add=0x500;
		break;
	case 'M':
		if (b<=1535)
		{
			add=0x800;
		}
		else
		{
			b=b-1536;
			add=0xB000;
		}
		break;	
	case 'T':
		add=0x600;
		break;
	case 'C':
		add=0xE00;
		break;		
	case 'H':
		add=0x000;
		break;				
	}
	b=b+add;								//开始地址偏移
	b1=(b&0xff00)>>8;           			//对开始偏移地址取高低位，b1高位，b2低位，16进制
	b2=b&0xff;
	a1=b&0xf000;                			//对开始偏移地址作asicc处理，依次从高到低
	a1=a1>>12;
	a1=a1&0xf;
	a2=b&0xf00;
	a2=a2>>8;
	a2=a2&0xf;
	a3=b&0xf0;
	a3=a3>>4;
	a3=a3&0xf;
	a4=b&0xf;
	*(U8 *)(AD1+0)=0x3a;        					//台达开始字元":",既3a
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);//plc站地址01，asicc码是0x30和0x31
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);
	*(U8 *)(AD1+3)=0x30;        			//功能码05（强置置位和复位），要转成asicc码
	*(U8 *)(AD1+4)=0x35;
	*(U8 *)(AD1+5)=asicc(a1);  	 			//开始地址，依次从高到低，要转成asicc码
	*(U8 *)(AD1+6)=asicc(a2);
	*(U8 *)(AD1+7)=asicc(a3);
	*(U8 *)(AD1+8)=asicc(a4);
	if (ptcom->writeValue==1)				//置位0xff00
	{
		*(U8 *)(AD1+9)=0x46;
		*(U8 *)(AD1+10)=0x46;
		*(U8 *)(AD1+11)=0x30;
		*(U8 *)(AD1+12)=0x30;
		t=0xff;
	}
	if (ptcom->writeValue==0)				//复位0x0000
	{
		*(U8 *)(AD1+9)=0x30;
		*(U8 *)(AD1+10)=0x30;
		*(U8 *)(AD1+11)=0x30;
		*(U8 *)(AD1+12)=0x30;
		t=0x00;
	}
	KK[1]=plcadd&0xff;                 			//LRC校验，16进制校验，asicc码显示
	KK[2]=0x05;
	KK[3]=b1;
	KK[4]=b2;
	KK[5]=t;
	KK[6]=0x00;	
	aakj=CalLRC(KK,7);          			//LRC校验，高位在前，低位在后         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+13)=asicc(a1);  			//asicc码显示
	*(U8 *)(AD1+14)=asicc(a2);
	*(U8 *)(AD1+15)=0x0d;       			//结束字元0d,0a
	*(U8 *)(AD1+16)=0x0a;

	//sysprintf("Set_Reset\n");
	//for(i=0;i<=16;i++)
	//{
	//	sysprintf("AD%d=%x\n",i,*(U8 *)(AD1+i));
	//}
	
	ptcom->send_length[0]=17;				//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址	
	ptcom->send_times=1;					//发送次数
		
	ptcom->return_length[0]=17;				//返回数据长度
	ptcom->return_start[0]=0;				//返回数据有效开始
	ptcom->return_length_available[0]=0;	//返回有效数据长度	
	ptcom->Current_Times=0;					//当前发送次数	
	
	ptcom->send_staradd[90]=0;		
}
/************************************************************************************************************************
读布尔量函数
*************************************************************************************************************************/
void Read_Bool()								
{
	switch (ptcom->registerr)//根据寄存器类型划分
	{
	case 'X':
	case 'Y':
	case 'T':
	case 'C':
	case 'H':
		Read_Bool_continous();//地址是连续的
		break;	
	case 'M':
		if ( ((ptcom->address <= 1535 && (ptcom->address + ptcom->register_length*8) <= 1535)) || ptcom->address > 1535 )
		{
			Read_Bool_continous();//地址是连续的
		}
		else
		{
			Read_Bool_discontinous();//地址不连续
		}
		break;					
	}
	ptcom->send_staradd[90]=1;				
}
/************************************************************************************************************************
读继电器名称为XYTC函数，因为地址是连续的，固正常读操作
*************************************************************************************************************************/
void Read_Bool_continous()
{
	int nFuncCode = 0x01;
	int nSendTimes = 0;
	int nLastTimeLen = 0;
	int ps = 0;
	int i=0;
	int nPlcStation = 0;
	int nPassAddress = 0;
	int nSendAddress = 0;
	int nSenLen = 0;
	U16 nCalCheck = 0;
	
	nPlcStation = ptcom->plc_address;	            	
	nSendAddress = ptcom->address;
	
	switch (ptcom->registerr)				
	{
		case 'X':
			nPassAddress = nSendAddress + 0x0400;
			nFuncCode = 0x02;
			break;
		case 'Y':
			nPassAddress = nSendAddress + 0x0500;
			break;		
		case 'T':
			nPassAddress = nSendAddress + 0x0600;
			break;
		case 'C':
			nPassAddress = nSendAddress + 0x0e00;
			break;	
		case 'H':
			nPassAddress = nSendAddress + 0x0000;
			break;	
		case 'M':
			if (nSendAddress <= 1535)
			{
				nPassAddress = nSendAddress + 0x0800;
			}
			else
			{
				nPassAddress = nSendAddress - 1536 + 0xb000;
			}
			break;				
	}		
	
	/*台达一次最大能读取15个word或者30个char*/
	if (ptcom->register_length % 30 == 0)
	{
		nSendTimes = ptcom->register_length / 30;
		nLastTimeLen = 30;
	}
	else
	{
		nSendTimes = (ptcom->register_length / 30) + 1;
		nLastTimeLen = ptcom->register_length % 30;
	}
	
	for (i = 0; i < nSendTimes; i++)
	{
		ps = i*17;
		nSendAddress = nPassAddress + i*30*8;
		
		if (i != nSendTimes - 1)
		{
			nSenLen = 30 * 8;
		}
		else
		{
			nSenLen = nLastTimeLen * 8;
		}
	
		
		*(U8 *)(AD1 + 0 + ps) = 0x3a;
		
		*(U8 *)(AD1 + 1 + ps) = asicc((nPlcStation >> 4) & 0x0f);
		*(U8 *)(AD1 + 2 + ps) = asicc(nPlcStation & 0x0f);
		
		*(U8 *)(AD1 + 3 + ps) = asicc((nFuncCode >> 4) & 0x0f);
		*(U8 *)(AD1 + 4 + ps) = asicc(nFuncCode & 0x0f);
		
		*(U8 *)(AD1 + 5 + ps) = asicc((nSendAddress >> 12) & 0x0f);
		*(U8 *)(AD1 + 6 + ps) = asicc((nSendAddress >> 8) & 0x0f);
		*(U8 *)(AD1 + 7 + ps) = asicc((nSendAddress >> 4) & 0x0f);
		*(U8 *)(AD1 + 8 + ps) = asicc(nSendAddress & 0x0f);
		
		*(U8 *)(AD1 + 9 + ps) = asicc((nSenLen >> 12) & 0x0f);
		*(U8 *)(AD1 + 10 + ps) = asicc((nSenLen >> 8) & 0x0f);
		*(U8 *)(AD1 + 11 + ps) = asicc((nSenLen >> 4) & 0x0f);
		*(U8 *)(AD1 + 12 + ps) = asicc(nSenLen & 0x0f);
		
		KK[1] = nPlcStation & 0xff; 
		KK[2] = nFuncCode & 0xff;
		KK[3] = (nSendAddress >> 8) & 0xff;
		KK[4] = nSendAddress & 0xff;
		KK[5] = (nSenLen >> 8) & 0xff;
		KK[6] = nSenLen & 0xff;
		
		nCalCheck = CalLRC( KK, 7 );          			
		*(U8 *)(AD1 + 13 + ps) = asicc((nCalCheck >> 4) & 0x0f);  			
		*(U8 *)(AD1 + 14 + ps) = asicc(nCalCheck & 0x0f);
		*(U8 *)(AD1 + 15 + ps) = 0x0d;       		
		*(U8 *)(AD1 + 16 + ps) = 0x0a;	
		
		ptcom->send_length[i] = 17;
		ptcom->send_staradd[i] = ps;											
		ptcom->return_length[i]= 11 + (nSenLen / 8) * 2;                                       
		ptcom->return_start[i] = 7;				
		ptcom->return_length_available[i] = (nSenLen / 8) * 2;
		
		ptcom->send_add[i] = ptcom->address + i*30*8;		
		ptcom->send_data_length[i] = nSenLen / 8;
			
		//ptcom->address = ptcom->address + i*30*8;
		//ptcom->register_length = nSenLen / 8;	
	}
	ptcom->Simens_Count = 100;
	ptcom->Current_Times = 0;
	ptcom->send_times = nSendTimes;		
}
/************************************************************************************************************************
读继电器名称为M函数，因为地址是不是连续的，固需分段读，边界地址为1535，跨越边界需分两次读
*************************************************************************************************************************/
void Read_Bool_discontinous()
{
	U16 aakj;
	int b,t;
	int a1,a2,a3,a4;
	int add;
	int b1,b2,b3,b4;
	int t1,t2,t3,t4;
	int plcadd;
	int length;
	int add_1;
	int	length_1,length_2;
	 
	plcadd=ptcom->plc_address;	            //PLC站地址	
	b=ptcom->address;						//在主程序已经转换到该段的开始地址		
	length=ptcom->register_length;
	
	if (b<1535 && b+length*8>1536)			//当跨越边界时，分两次读
	{
		add=0x800;							//对应PLC里开始地址，分界前
		add_1=b;
		length_1=(1536-b)/8;
		
		add_1=add_1+add;					//加上偏移地址
		
		b1=(add_1&0xff00)>>8;           	//对开始偏移地址取高低位，b1高位，b2低位，16进制
		b2=add_1&0xff;
		a1=add_1&0xf000;                	//对开始偏移地址作asicc处理，依次从高到低 
		a1=a1>>12;
		a1=a1&0xf;
		a2=add_1&0xf00;
		a2=a2>>8;
		a2=a2&0xf;
		a3=add_1&0xf0;
		a3=a3>>4;
		a3=a3&0xf;
		a4=add_1&0xf;
		*(U8 *)(AD1+0)=0x3a;        		//台达开始字元":",既3a
		*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);//plc站地址01，asicc码是0x30和0x31
		*(U8 *)(AD1+2)=asicc(plcadd&0xf);
		*(U8 *)(AD1+3)=0x30;        		//功能码02，要转成asicc码
		*(U8 *)(AD1+4)=0x31;
		*(U8 *)(AD1+5)=asicc(a1);   		//开始地址，依次从高到低，要转成asicc码
		*(U8 *)(AD1+6)=asicc(a2);
		*(U8 *)(AD1+7)=asicc(a3);
		*(U8 *)(AD1+8)=asicc(a4);
		t=length_1*8; 						//一个元件包涵8位数据
		b3=(t&0xff00)>>8;           		//对元件个数取高低位，b3高位，b4低位，16进制
		b4=t&0xff;
		t1=t&0xf000;                		//对元件个数作asicc处理，依次从高到低
		t1=t1>>12;
		t1=t1&0xf;
		t2=t&0xf00;
		t2=t2>>8;
		t2=t2&0xf;
		t3=t&0xf0;
		t3=t3>>4;
		t3=t3&0xf;
		t4=t&0xf;
		*(U8 *)(AD1+9)=asicc(t1);			//发送元件个数，以asicc码发送，依次从高到低
		*(U8 *)(AD1+10)=asicc(t2);
		*(U8 *)(AD1+11)=asicc(t3);
		*(U8 *)(AD1+12)=asicc(t4);
		KK[1]=plcadd&0xff;                 		//LRC校验，16进制校验，asicc码显示
		KK[2]=0x01;
		KK[3]=b1;
		KK[4]=b2;
		KK[5]=b3;
		KK[6]=b4;
		
		aakj=CalLRC(KK,7);          		//LRC校验，高位在前，低位在后
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13)=asicc(a1);  		//asicc码显示
		*(U8 *)(AD1+14)=asicc(a2);
		*(U8 *)(AD1+15)=0x0d;       		//结束字元0d,0a
		*(U8 *)(AD1+16)=0x0a;	
		
		ptcom->send_length[0]=17;			//发送长度
		ptcom->send_staradd[0]=0;			//发送数据存储地址				
		ptcom->return_length[0]=11+length_1*2;//返回数据长度，有9个固定，3a，PLC地址2个长度，功能码2个长度，
		                                    //位数显示2个长度，校检2个长度，0d，0a
		ptcom->return_start[0]=7;			//返回数据有效开始
		ptcom->return_length_available[0]=length_1*2;//返回有效数据长度	
		ptcom->send_add[0]=ptcom->address;	//读的是这个地址的数据	
		
		ptcom->register_length=length_1;
//--------------------------------------------------------------------------------------------------------		
		add=0xB000;							//对应PLC里开始地址，分界后
		length_2=length-length_1;
		
		b1=(add&0xff00)>>8;           		//对开始偏移地址取高低位，b1高位，b2低位，16进制
		b2=add&0xff;
		a1=add&0xf000;                		//对开始偏移地址作asicc处理，依次从高到低 
		a1=a1>>12;
		a1=a1&0xf;
		a2=add&0xf00;
		a2=a2>>8;
		a2=a2&0xf;
		a3=add&0xf0;
		a3=a3>>4;
		a3=a3&0xf;
		a4=add&0xf;
		*(U8 *)(AD1+17)=0x3a;        		//台达开始字元":",既3a
		*(U8 *)(AD1+18)=asicc(((plcadd&0xf0)>>4)&0xf);//plc站地址01，asicc码是0x30和0x31
		*(U8 *)(AD1+19)=asicc(plcadd&0xf);
		*(U8 *)(AD1+20)=0x30;        		//功能码02，要转成asicc码
		*(U8 *)(AD1+21)=0x31;
		*(U8 *)(AD1+22)=asicc(a1);   		//开始地址，依次从高到低，要转成asicc码
		*(U8 *)(AD1+23)=asicc(a2);
		*(U8 *)(AD1+24)=asicc(a3);
		*(U8 *)(AD1+25)=asicc(a4);
		
		t=length_2*8; 						//一个元件包涵8位数据
		b3=(t&0xff00)>>8;           		//对元件个数取高低位，b3高位，b4低位，16进制
		b4=t&0xff;
		t1=t&0xf000;                		//对元件个数作asicc处理，依次从高到低
		t1=t1>>12;
		t1=t1&0xf;
		t2=t&0xf00;
		t2=t2>>8;
		t2=t2&0xf;
		t3=t&0xf0;
		t3=t3>>4;
		t3=t3&0xf;
		t4=t&0xf;
		*(U8 *)(AD1+26)=asicc(t1);			//发送元件个数，以asicc码发送，依次从高到低
		*(U8 *)(AD1+27)=asicc(t2);
		*(U8 *)(AD1+28)=asicc(t3);
		*(U8 *)(AD1+29)=asicc(t4);
		KK[1]=plcadd&0xff;                 		//LRC校验，16进制校验，asicc码显示
		KK[2]=0x01;
		KK[3]=b1;
		KK[4]=b2;
		KK[5]=b3;
		KK[6]=b4;
		
		aakj=CalLRC(KK,7);          		//LRC校验，高位在前，低位在后
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+30)=asicc(a1);  		//asicc码显示
		*(U8 *)(AD1+31)=asicc(a2);
		*(U8 *)(AD1+32)=0x0d;       		//结束字元0d,0a
		*(U8 *)(AD1+33)=0x0a;	
		
		ptcom->send_length[1]=17;			//发送长度
		ptcom->send_staradd[1]=17;			//发送数据存储地址				
		ptcom->return_length[1]=11+length_2*2;//返回数据长度，有9个固定，3a，PLC地址2个长度，功能码2个长度，
		                                    //位数显示2个长度，校检2个长度，0d，0a
		ptcom->return_start[1]=7;			//返回数据有效开始
		ptcom->return_length_available[1]=length_2*2;//返回有效数据长度	
		ptcom->send_add[1]=1536;			//读的是这个地址的数据	
		
		ptcom->send_staradd[98]=1536;
		ptcom->send_staradd[99]=length_2;
		
		ptcom->Current_Times=0;				//当前发送次数
		ptcom->send_times=2;				//发送次数					
	}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	else									//未跨越边界地址，正常读操作
	{
		if (b<1535 && b+length*8<=1536)
		{
			add=0x0800;
		}
		if (b>=1536)
		{
			b=b-1536;
			add=0xB000;
		}
		
		b=b+add;							//加上偏移地址
		b1=(b&0xff00)>>8;           		//对开始偏移地址取高低位，b1高位，b2低位，16进制
		b2=b&0xff;
		a1=b&0xf000;               			//对开始偏移地址作asicc处理，依次从高到低 
		a1=a1>>12;
		a1=a1&0xf;
		a2=b&0xf00;
		a2=a2>>8;
		a2=a2&0xf;
		a3=b&0xf0;
		a3=a3>>4;
		a3=a3&0xf;
		a4=b&0xf;
		*(U8 *)(AD1+0)=0x3a;        		//台达开始字元":",既3a
		*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);//plc站地址01，asicc码是0x30和0x31
		*(U8 *)(AD1+2)=asicc(plcadd&0xf);
		*(U8 *)(AD1+3)=0x30;        		//功能码02，要转成asicc码
		*(U8 *)(AD1+4)=0x31;
		*(U8 *)(AD1+5)=asicc(a1);   		//开始地址，依次从高到低，要转成asicc码
		*(U8 *)(AD1+6)=asicc(a2);
		*(U8 *)(AD1+7)=asicc(a3);
		*(U8 *)(AD1+8)=asicc(a4);
		t=ptcom->register_length*8; 		//一个元件包涵8位数据
		b3=(t&0xff00)>>8;           		//对元件个数取高低位，b3高位，b4低位，16进制
		b4=t&0xff;
		t1=t&0xf000;                		//对元件个数作asicc处理，依次从高到低
		t1=t1>>12;
		t1=t1&0xf;
		t2=t&0xf00;
		t2=t2>>8;
		t2=t2&0xf;
		t3=t&0xf0;
		t3=t3>>4;
		t3=t3&0xf;
		t4=t&0xf;
		*(U8 *)(AD1+9)=asicc(t1);			//发送元件个数，以asicc码发送，依次从高到低
		*(U8 *)(AD1+10)=asicc(t2);
		*(U8 *)(AD1+11)=asicc(t3);
		*(U8 *)(AD1+12)=asicc(t4);
		KK[1]=plcadd&0xff;                 		//LRC校验，16进制校验，asicc码显示
		KK[2]=0x01;
		KK[3]=b1;
		KK[4]=b2;
		KK[5]=b3;
		KK[6]=b4;
		
		aakj=CalLRC(KK,7);          		//LRC校验，高位在前，低位在后
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13)=asicc(a1);  		//asicc码显示
		*(U8 *)(AD1+14)=asicc(a2);
		*(U8 *)(AD1+15)=0x0d;       		//结束字元0d,0a
		*(U8 *)(AD1+16)=0x0a;	
		
		ptcom->send_length[0]=17;			//发送长度
		ptcom->send_staradd[0]=0;			//发送数据存储地址	
		ptcom->send_times=1;				//发送次数
			
		ptcom->return_length[0]=11+ptcom->register_length*2;//返回数据长度，有9个固定，3a，PLC地址2个长度，功能码2个长度，
		                                    //位数显示2个长度，校检2个长度，0d，0a
		ptcom->return_start[0]=7;			//返回数据有效开始
		ptcom->return_length_available[0]=ptcom->register_length*2;	//返回有效数据长度	
		ptcom->Current_Times=0;				//当前发送次数	
		ptcom->send_add[0]=ptcom->address;	//读的是这个地址的数据					
	}	
}
/************************************************************************************************************************
读模拟量函数，
*************************************************************************************************************************/
void Read_Analog()							//读模拟量
{
	switch (ptcom->registerr)				//根据寄存器类型划分
	{
		case 'D':		
			Read_Analog_D();					//D寄存器地址不连续，需进行分段操作
			ptcom->send_staradd[90]=1;
			break;
		case 't':	
		case 'c':
			Read_Analog_tc();				//tc寄存器地址是连续的，正常读
			break;						
	}		
}
/************************************************************************************************************************
读名称为D的寄存器，因其地址不连续，边界地址为4095，需分段进行读
*************************************************************************************************************************/
void Read_Analog_D()
{
	U16 aakj;
	int b,t;
	int a1,a2,a3,a4;
	int add;
	int b1,b2,b3,b4;
	int t1,t2,t3,t4;
	int plcadd;
	int length,length_1,length_2;
	int add_1;
	
	plcadd=ptcom->plc_address;	            //PLC站地址	
	b=ptcom->address;						//开始地址	
	length=ptcom->register_length;
	
	if (b<=4095 && (b+length)>4096)			//当跨越边界时，分两次读
	{
		add=0x1000;
		add_1=b;
		length_1=4096-b;
		
		add_1=add_1+add;					//加上偏移地址
		
		b1=(add_1&0xff00)>>8;           	//对开始偏移地址取高低位，b1高位，b2低位，16进制
		b2=add_1&0xff;	
		a1=add_1&0xf000;                	//对开始偏移地址作asicc处理，依次从高到低 
		a1=a1>>12;
		a1=a1&0xf;
		a2=add_1&0xf00;
		a2=a2>>8;
		a2=a2&0xf;
		a3=add_1&0xf0;
		a3=a3>>4;
		a3=a3&0xf;
		a4=add_1&0xf;
		*(U8 *)(AD1+0)=0x3a;        		//台达开始字元":",既3a
		*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);//plc站地址01，asicc码是0x30和0x31
		*(U8 *)(AD1+2)=asicc(plcadd&0xf);
		*(U8 *)(AD1+3)=0x30;        		//功能码03，要转成asicc码
		*(U8 *)(AD1+4)=0x33;	
		*(U8 *)(AD1+5)=asicc(a1);			//开始地址，依次从高到低，要转成asicc码
		*(U8 *)(AD1+6)=asicc(a2);
		*(U8 *)(AD1+7)=asicc(a3);
		*(U8 *)(AD1+8)=asicc(a4);
		t=length_1;   						// register_length是寄存器数据长度
		b3=(t&0xff00)>>8;           		//对元件个数取高低位，b3高位，b4低位，16进制
		b4=t&0xff;
		t1=t&0xf000;                		//对元件个数作asicc处理，依次从高到低
		t1=t1>>12;
		t1=t1&0xf;
		t2=t&0xf00;
		t2=t2>>8;
		t2=t2&0xf;
		t3=t&0xf0;
		t3=t3>>4;
		t3=t3&0xf;
		t4=t&0xf;
		*(U8 *)(AD1+9)=asicc(t1);			//发送元件个数，asicc码显示
		*(U8 *)(AD1+10)=asicc(t2);
		*(U8 *)(AD1+11)=asicc(t3);
		*(U8 *)(AD1+12)=asicc(t4);
		KK[1]=plcadd&0xff;                 		//LRC校验，16进制校验，asicc码显示
		KK[2]=0x03;
		KK[3]=b1;
		KK[4]=b2;
		KK[5]=b3;
		KK[6]=b4;
		
		aakj=CalLRC(KK,7);	       			//计算和校检，LRC校验，高位在前，低位在后
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13)=asicc(a1); 			//asicc码显示
		*(U8 *)(AD1+14)=asicc(a2);
		*(U8 *)(AD1+15)=0x0d;      			//结束字元0d,0a
		*(U8 *)(AD1+16)=0x0a;
		
		ptcom->send_length[0]=17;			//发送长度
		ptcom->send_staradd[0]=0;			//发送数据存储地址				
		ptcom->return_length[0]=11+length_1*4;//返回数据长度，有9个固定，3a，PLC地址2个长度，功能码2个长度，
		                                    //位数显示2个长度，校检2个长度，0d，0a
		ptcom->return_start[0]=7;			//返回数据有效开始
		ptcom->return_length_available[0]=length_1*4;//返回有效数据长度	
		ptcom->send_add[0]=ptcom->address;	//读的是这个地址的数据	
		
		ptcom->register_length=length_1;
//-----------------------------------------------------------------------------------------------------------------		
		add=0x9000;		
		length_2=length-length_1;
		
		b1=(add&0xff00)>>8;           		//对开始偏移地址取高低位，b1高位，b2低位，16进制
		b2=add&0xff;	
		a1=add&0xf000;                		//对开始偏移地址作asicc处理，依次从高到低 
		a1=a1>>12;
		a1=a1&0xf;
		a2=add&0xf00;
		a2=a2>>8;
		a2=a2&0xf;
		a3=add&0xf0;
		a3=a3>>4;
		a3=a3&0xf;
		a4=add&0xf;
		*(U8 *)(AD1+17)=0x3a;        		//台达开始字元":",既3a
		*(U8 *)(AD1+18)=asicc(((plcadd&0xf0)>>4)&0xf);//plc站地址01，asicc码是0x30和0x31
		*(U8 *)(AD1+19)=asicc(plcadd&0xf);
		*(U8 *)(AD1+20)=0x30;        		//功能码03，要转成asicc码
		*(U8 *)(AD1+21)=0x33;	
		*(U8 *)(AD1+22)=asicc(a1);			//开始地址，依次从高到低，要转成asicc码
		*(U8 *)(AD1+23)=asicc(a2);
		*(U8 *)(AD1+24)=asicc(a3);
		*(U8 *)(AD1+25)=asicc(a4);
		t=length_2;   						// register_length是寄存器数据长度
		b3=(t&0xff00)>>8;           		//对元件个数取高低位，b3高位，b4低位，16进制
		b4=t&0xff;
		t1=t&0xf000;                		//对元件个数作asicc处理，依次从高到低
		t1=t1>>12;
		t1=t1&0xf;
		t2=t&0xf00;
		t2=t2>>8;
		t2=t2&0xf;
		t3=t&0xf0;
		t3=t3>>4;
		t3=t3&0xf;
		t4=t&0xf;
		*(U8 *)(AD1+26)=asicc(t1);			//发送元件个数，asicc码显示
		*(U8 *)(AD1+27)=asicc(t2);
		*(U8 *)(AD1+28)=asicc(t3);
		*(U8 *)(AD1+29)=asicc(t4);
		KK[1]=plcadd&0xff;                 		//LRC校验，16进制校验，asicc码显示
		KK[2]=0x03;
		KK[3]=b1;
		KK[4]=b2;
		KK[5]=b3;
		KK[6]=b4;
		
		aakj=CalLRC(KK,7);	       			//计算和校检，LRC校验，高位在前，低位在后
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+30)=asicc(a1); 			//asicc码显示
		*(U8 *)(AD1+31)=asicc(a2);
		*(U8 *)(AD1+32)=0x0d;      			//结束字元0d,0a
		*(U8 *)(AD1+33)=0x0a;
		
		ptcom->send_length[1]=17;			//发送长度
		ptcom->send_staradd[1]=17;			//发送数据存储地址				
		ptcom->return_length[1]=11+length_2*4;//返回数据长度，有9个固定，3a，PLC地址2个长度，功能码2个长度，
		                                    //位数显示2个长度，校检2个长度，0d，0a
		ptcom->return_start[1]=7;			//返回数据有效开始
		ptcom->return_length_available[1]=length_2*4;//返回有效数据长度	
		ptcom->send_add[1]=4096;			//读的是这个地址的数据	
		
		ptcom->send_staradd[98]=4096;
		ptcom->send_staradd[99]=length_2;
		
		ptcom->Current_Times=0;				//当前发送次数
		ptcom->send_times=2;				//发送次数											
	}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else									//未跨越边界，正常读
	{
		if (b<=4095 && b+length<=4096)
		{
			add=0x1000;
		}
		if (b>=4096)
		{
			b=b-4096;
			add=0x9000;
		}
		
		b=b+add;							//加上偏移地址	
		b1=(b&0xff00)>>8;           		//对开始偏移地址取高低位，b1高位，b2低位，16进制
		b2=b&0xff;	
		a1=b&0xf000;                		//对开始偏移地址作asicc处理，依次从高到低 
		a1=a1>>12;
		a1=a1&0xf;
		a2=b&0xf00;
		a2=a2>>8;
		a2=a2&0xf;
		a3=b&0xf0;
		a3=a3>>4;
		a3=a3&0xf;
		a4=b&0xf;
		*(U8 *)(AD1+0)=0x3a;        		//台达开始字元":",既3a
		*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);//plc站地址01，asicc码是0x30和0x31
		*(U8 *)(AD1+2)=asicc(plcadd&0xf);
		*(U8 *)(AD1+3)=0x30;        		//功能码03，要转成asicc码
		*(U8 *)(AD1+4)=0x33;	
		*(U8 *)(AD1+5)=asicc(a1);			//开始地址，依次从高到低，要转成asicc码
		*(U8 *)(AD1+6)=asicc(a2);
		*(U8 *)(AD1+7)=asicc(a3);
		*(U8 *)(AD1+8)=asicc(a4);
		t=length;   						// register_length是寄存器数据长度
		b3=(t&0xff00)>>8;           		//对元件个数取高低位，b3高位，b4低位，16进制
		b4=t&0xff;
		t1=t&0xf000;                		//对元件个数作asicc处理，依次从高到低
		t1=t1>>12;
		t1=t1&0xf;
		t2=t&0xf00;
		t2=t2>>8;
		t2=t2&0xf;
		t3=t&0xf0;
		t3=t3>>4;
		t3=t3&0xf;
		t4=t&0xf;
		*(U8 *)(AD1+9)=asicc(t1);			//发送元件个数，asicc码显示
		*(U8 *)(AD1+10)=asicc(t2);
		*(U8 *)(AD1+11)=asicc(t3);
		*(U8 *)(AD1+12)=asicc(t4);
		KK[1]=plcadd&0xff;                 		//LRC校验，16进制校验，asicc码显示
		KK[2]=0x03;
		KK[3]=b1;
		KK[4]=b2;
		KK[5]=b3;
		KK[6]=b4;
		
		aakj=CalLRC(KK,7);	      			//计算和校检，LRC校验，高位在前，低位在后
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13)=asicc(a1); 			//asicc码显示
		*(U8 *)(AD1+14)=asicc(a2);
		*(U8 *)(AD1+15)=0x0d;      			//结束字元0d,0a
		*(U8 *)(AD1+16)=0x0a;
		
		ptcom->send_length[0]=17;			//发送长度
		ptcom->send_staradd[0]=0;			//发送数据存储地址	
		ptcom->send_times=1;				//发送次数
			
		ptcom->return_length[0]=11+length*4;//返回数据长度，有9个固定，3a，PLC地址2个长度，功能码2个长度，
		                                    //位数显示2个长度，校检2个长度，0d，0a
		ptcom->return_start[0]=7;			//返回数据有效开始
		ptcom->return_length_available[0]=length*4;//返回有效数据长度	
		ptcom->Current_Times=0;				//当前发送次数	
		ptcom->send_add[0]=ptcom->address;	//读的是这个地址的数据
	}
}
/************************************************************************************************************************
读名称为tc的寄存器
*************************************************************************************************************************/
void Read_Analog_tc()
{
	U16 aakj;
	int b,t;
	int a1,a2,a3,a4;
	int add;
	int b1,b2,b3,b4;
	int t1,t2,t3,t4;
	int plcadd;
	
	plcadd=ptcom->plc_address;	            //PLC站地址	
	b=ptcom->address;						//开始地址	
	switch (ptcom->registerr)				//根据寄存器类型获得偏移地址
	{
		case 't':
			add=0x600;
			break;		
		case 'c':
			add=0xe00;
			break;						
	}
	
	if(ptcom->registerr=='c')
	{
      if (b>=200)
      {
          b=((b-200)/2)+200;
      }   

      if(b>=200)
      {
          t=ptcom->register_length/2;               // register_length是寄存器数据长度
      }
      else
      {
          t=ptcom->register_length;               // register_length是寄存器数据长度

      }      
	}
	else
	{
        t=ptcom->register_length;               // register_length是寄存器数据长度
	}
    
	b=b+add;								//加上偏移地址
	b1=(b&0xff00)>>8;           			//对开始偏移地址取高低位，b1高位，b2低位，16进制
	b2=b&0xff;	
	a1=b&0xf000;                			//对开始偏移地址作asicc处理，依次从高到低 
	a1=a1>>12;
	a1=a1&0xf;
	a2=b&0xf00;
	a2=a2>>8;
	a2=a2&0xf;
	a3=b&0xf0;
	a3=a3>>4;
	a3=a3&0xf;
	a4=b&0xf;
	*(U8 *)(AD1+0)=0x3a;        			//台达开始字元":",既3a
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);//plc站地址01，asicc码是0x30和0x31
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);
	*(U8 *)(AD1+3)=0x30;        			//功能码03，要转成asicc码
	*(U8 *)(AD1+4)=0x33;	
	*(U8 *)(AD1+5)=asicc(a1);				//开始地址，依次从高到低，要转成asicc码
	*(U8 *)(AD1+6)=asicc(a2);
	*(U8 *)(AD1+7)=asicc(a3);
	*(U8 *)(AD1+8)=asicc(a4);

	
	b3=(t&0xff00)>>8;          			 	//对元件个数取高低位，b3高位，b4低位，16进制
	b4=t&0xff;
	t1=t&0xf000;                			//对元件个数作asicc处理，依次从高到低
	t1=t1>>12;
	t1=t1&0xf;
	t2=t&0xf00;
	t2=t2>>8;
	t2=t2&0xf;
	t3=t&0xf0;
	t3=t3>>4;
	t3=t3&0xf;
	t4=t&0xf;
	*(U8 *)(AD1+9)=asicc(t1);				//发送元件个数，asicc码显示
	*(U8 *)(AD1+10)=asicc(t2);
	*(U8 *)(AD1+11)=asicc(t3);
	*(U8 *)(AD1+12)=asicc(t4);
	KK[1]=plcadd&0xff;                 			//LRC校验，16进制校验，asicc码显示
	KK[2]=0x03;
	KK[3]=b1;
	KK[4]=b2;
	KK[5]=b3;
	KK[6]=b4;
	
	aakj=CalLRC(KK,7);	       				//计算和校检，LRC校验，高位在前，低位在后
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+13)=asicc(a1); 				//asicc码显示
	*(U8 *)(AD1+14)=asicc(a2);
	*(U8 *)(AD1+15)=0x0d;      				//结束字元0d,0a
	*(U8 *)(AD1+16)=0x0a;
	if(ptcom->registerr=='c')
	{
        if (ptcom->address<200)
        {   
            ptcom->send_length[0]=17;               //发送长度
            ptcom->send_staradd[0]=0;               //发送数据存储地址  
            ptcom->send_times=1;                    //发送次数
                
            ptcom->return_length[0]=11+ptcom->register_length*4;//返回数据长度，有9个固定，3a，PLC地址2个长度，功能码2个长度，
                                                                //位数显示2个长度，校检2个长度，0d，0a
            ptcom->return_start[0]=7;               //返回数据有效开始
            ptcom->return_length_available[0]=ptcom->register_length*4;//返回有效数据长度   
            ptcom->Current_Times=0;                 //当前发送次数  
            ptcom->send_add[0]=ptcom->address;      //读的是这个地址的数据
            
            ptcom->send_staradd[90]=1;
        }
        if (ptcom->address>=200)
        {   
            ptcom->send_length[0]=17;               //发送长度
            ptcom->send_staradd[0]=0;               //发送数据存储地址  
            ptcom->send_times=1;                    //发送次数
                
            ptcom->return_length[0]=11+(ptcom->register_length/2)*8;//返回数据长度，有9个固定，3a，PLC地址2个长度，功能码2个长度，
                                                                //位数显示2个长度，校检2个长度，0d，0a
            ptcom->return_start[0]=7;               //返回数据有效开始
            ptcom->return_length_available[0]=ptcom->register_length*8;//返回有效数据长度   
            ptcom->Current_Times=0;                 //当前发送次数  
            ptcom->send_add[0]=ptcom->address;      //读的是这个地址的数据

            ptcom->register_length=ptcom->register_length;
            
            ptcom->send_staradd[90]=2;
        }
	}
	else
	{
        ptcom->send_length[0]=17;               //发送长度
        ptcom->send_staradd[0]=0;               //发送数据存储地址  
        ptcom->send_times=1;                    //发送次数
            
        ptcom->return_length[0]=11+ptcom->register_length*4;//返回数据长度，有9个固定，3a，PLC地址2个长度，功能码2个长度，
                                                            //位数显示2个长度，校检2个长度，0d，0a
        ptcom->return_start[0]=7;               //返回数据有效开始
        ptcom->return_length_available[0]=ptcom->register_length*4;//返回有效数据长度   
        ptcom->Current_Times=0;                 //当前发送次数  
        ptcom->send_add[0]=ptcom->address;      //读的是这个地址的数据
        
        ptcom->send_staradd[90]=1;

	}

}
/************************************************************************************************************************
写模拟量子函数
*************************************************************************************************************************/
void Write_Analog()							//写模拟量
{	
	switch (ptcom->registerr)				//根据寄存器类型获得偏移地址
	{
	case 'D':
		Write_Analog_D();					//D寄存器地址不连续，需进行分段操作					
		break;
	case 't':	
		Write_Analog_t();					//tc寄存器地址是连续的，正常操作
		break;						
	case 'c':
		Write_Analog_c();					//tc寄存器地址是连续的，正常操作
		break;						
	}	
	ptcom->send_staradd[90]=0;	
}
/************************************************************************************************************************
写名称为D的寄存器，因其地址不连续，边界地址为4095，需分段进行操作
*************************************************************************************************************************/
void Write_Analog_D()
{
	U16 aakj;
	int b,t,k1,k2,k3,k4,k5,k6,k7,k8;
	int a1,a2,a3,a4;
	int add,add_1;
	int b1,b2,b3,b4;
	int i;
	int length,length_1,length_2;
	int t1,t2,t3,t4;
	int plcadd;
	
	plcadd=ptcom->plc_address;	            //PLC站地址	
	b=ptcom->address;						//开始地址	
	length=ptcom->register_length;
	
	/*if(length==1)                   		//如果数据较短，小于65535，则length==1
	{
		if (b<=4095)
		{
			add=0x1000;
		}
		if (b>=4096)
		{
			b=b-4096;
			add=0x9000;
		}
		
		b=b+add;							//加上偏移地址
		b1=(b&0xff00)>>8;           		//对开始偏移地址取高低位，b1高位，b2低位，16进制
		b2=b&0xff;
		a1=b&0xf000;                		//对开始偏移地址作asicc处理，依次从高到低
		a1=a1>>12;
		a1=a1&0xf;
		a2=b&0xf00;
		a2=a2>>8;
		a2=a2&0xf;
		a3=b&0xf0;
		a3=a3>>4;
		a3=a3&0xf;
		a4=b&0xf;

		*(U8 *)(AD1+0)=0x3a;        		//台达开始字元":",既3a
		*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);//PLC站地址化为asicc码显示
		*(U8 *)(AD1+2)=asicc(plcadd&0xf);   
		*(U8 *)(AD1+3)=0x30;        		//功能码06，要转成asicc码          
		*(U8 *)(AD1+4)=0x36;		
		*(U8 *)(AD1+5)=asicc(a1);			//起始地址，依次从高到低，要转成asicc码
		*(U8 *)(AD1+6)=asicc(a2);
		*(U8 *)(AD1+7)=asicc(a3);
		*(U8 *)(AD1+8)=asicc(a4);
		
		b4=ptcom->U8_Data[0];       		//从D[]数组中要数据，对应的b3为高位，b4为低位
		b3=ptcom->U8_Data[1];
		
		b5=(b3&0xf0)>>4;            		//对数据进行asicc处理，以显示asicc码，依次从高位到低位
		b6=b3&0xf;
		b7=(b4&0xf0)>>4;
		b8=b4&0xf;
		*(U8 *)(AD1+9)=asicc(b5);
		*(U8 *)(AD1+10)=asicc(b6);
		*(U8 *)(AD1+11)=asicc(b7);
		*(U8 *)(AD1+12)=asicc(b8);
		KK[1]=plcadd&0xff;                 		//LRC校验，16进制校验，asicc码显示 
		KK[2]=0x06;
		KK[3]=b1;
		KK[4]=b2;
		KK[5]=b3;
		KK[6]=b4;
	
		aakj=CalLRC(KK,7);	        		//计算和校检，LRC校验，高位在前，低位在后
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13)=asicc(a1);  		//asicc码显示
		*(U8 *)(AD1+14)=asicc(a2); 
		*(U8 *)(AD1+15)=0x0d;       		//结束字元0d，0a
		*(U8 *)(AD1+16)=0x0a;
		
		ptcom->send_length[0]=17;			//发送长度
		ptcom->send_staradd[0]=0;			//发送数据存储地址	
		ptcom->send_times=1;				//发送次数
				
		ptcom->return_length[0]=17;			//返回数据长度
		ptcom->return_start[0]=0;			//返回数据有效开始
		ptcom->return_length_available[0]=0;//返回有效数据长度	
		ptcom->Current_Times=0;	*/	
	//}
	//if(length!=1)                   		//如果数据较长，大于65535，则length！=1  
	//{
		if (b<=4095 && b+length>4096)
		{
			add=0x1000;
			add_1=b;
			length_1=4096-b;
			
			add_1=add_1+add;				//加上偏移地址
			
			b1=(add_1&0xff00)>>8;           //对开始偏移地址取高低位，b1高位，b2低位，16进制
			b2=add_1&0xff;
			a1=add_1&0xf000;                //对开始偏移地址作asicc处理，依次从高到低
			a1=a1>>12;
			a1=a1&0xf;
			a2=add_1&0xf00;
			a2=a2>>8;
			a2=a2&0xf;
			a3=add_1&0xf0;
			a3=a3>>4;
			a3=a3&0xf;
			a4=add_1&0xf;	
			
			*(U8 *)(AD1+0)=0x3a;        	//台达开始字元":",既3a      
			*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);//PLC站地址化为asicc码显示
			*(U8 *)(AD1+2)=asicc(plcadd&0xf); 
			*(U8 *)(AD1+3)=0x31;        	//功能码16，转成asicc码
			*(U8 *)(AD1+4)=0x30;		
			*(U8 *)(AD1+5)=asicc(a1);		//起始地址，依次从高到低，要转成asicc码
			*(U8 *)(AD1+6)=asicc(a2);
			*(U8 *)(AD1+7)=asicc(a3);
			*(U8 *)(AD1+8)=asicc(a4);
			
			t=length_1;   					//对数据长度取高低位，b1高位，b2低位，16进制  
			b3=(t&0xff00)>>8;
			b4=t&0xff;
			t1=t&0xf000;                	//对数据长度进行asicc处理，以显示asicc码，依次从高位到低位
			t1=t1>>12;
			t1=t1&0xf;
			t2=t&0xf00;
			t2=t2>>8;
			t2=t2&0xf;
			t3=t&0xf0;
			t3=t3>>4;
			t3=t3&0xf;
			t4=t&0xf;
			
			*(U8 *)(AD1+9)=asicc(t1);		//发送长度，依次从高到低，要转成asicc码
			*(U8 *)(AD1+10)=asicc(t2);
			*(U8 *)(AD1+11)=asicc(t3);
			*(U8 *)(AD1+12)=asicc(t4);
			
			KK[1]=plcadd&0xff;                 	//LRC校验，16进制校验，asicc码显示 
			KK[2]=0x10;
			KK[3]=b1;
			KK[4]=b2;
			KK[5]=b3;
			KK[6]=b4;
							
			k1=((length_1*2)&0xf0)>>4;    	//对数据位长度进行asicc处理，以显示asicc码，依次从高位到低位
			k1=k1&0xf;
			k2=(length_1*2)&0xf;
			*(U8 *)(AD1+13)=asicc(k1);  	//发送位的长度，依次从高到低，要转成asicc码
			*(U8 *)(AD1+14)=asicc(k2);
			KK[7]=length_1*2;	
						

			for (i=0;i<length_1;i++)      	//写入多个元件值
			{				
				k4=ptcom->U8_Data[i*2];     //从D[]数组中要数据，对应的k3为高位，k4为低位
				k3=ptcom->U8_Data[i*2+1];
							
				k5=(k3&0xf0)>>4;            //对数据进行asicc处理，以显示asicc码，依次从高位到低位
				k6=k3&0xf;
				k7=(k4&0xf0)>>4;
				k8=k4&0xf;
				*(U8 *)(AD1+15+i*4)=asicc(k5);//发送位的数据，依次从高到低，要转成asicc码
				*(U8 *)(AD1+16+i*4)=asicc(k6);
				*(U8 *)(AD1+17+i*4)=asicc(k7);
				*(U8 *)(AD1+18+i*4)=asicc(k8);			
					
				KK[8+i*2]=k3;
				KK[9+i*2]=k4;		
			}
						
			aakj=CalLRC(KK,length_1*2+8);	//计算和校检，LRC校验，高位在前，低位在后
			a1=(aakj&0xf0)>>4;
			a1=a1&0xf;
			a2=aakj&0xf;
			*(U8 *)(AD1+19+(length_1-1)*4)=asicc(a1);//asicc码显示
			*(U8 *)(AD1+20+(length_1-1)*4)=asicc(a2);
			*(U8 *)(AD1+21+(length_1-1)*4)=0x0d;     //结束字元0d，0a
			*(U8 *)(AD1+22+(length_1-1)*4)=0x0a;
			
			ptcom->send_length[0]=19+length_1*4;//发送长度
			ptcom->send_staradd[0]=0;		//发送数据存储地址						
			ptcom->return_length[0]=17;		//返回数据长度
			ptcom->return_start[0]=0;		//返回数据有效开始
			ptcom->return_length_available[0]=0;//返回有效数据长度	
//-------------------------------------------------------------------------------------------------------------------
			add=0x9000;		
			length_2=length-length_1;
			
			b1=(add&0xff00)>>8;           	//对开始偏移地址取高低位，b1高位，b2低位，16进制
			b2=add&0xff;	
			a1=add&0xf000;                	//对开始偏移地址作asicc处理，依次从高到低 
			a1=a1>>12;
			a1=a1&0xf;
			a2=add&0xf00;
			a2=a2>>8;
			a2=a2&0xf;
			a3=add&0xf0;
			a3=a3>>4;
			a3=a3&0xf;
			a4=add&0xf;	
			
			*(U8 *)(AD1+23+(length_1-1)*4)=0x3a;//台达开始字元":",既3a      
			*(U8 *)(AD1+24+(length_1-1)*4)=asicc(((plcadd&0xf0)>>4)&0xf);//PLC站地址化为asicc码显示
			*(U8 *)(AD1+25+(length_1-1)*4)=asicc(plcadd&0xf); 
			*(U8 *)(AD1+26+(length_1-1)*4)=0x31; //功能码16，转成asicc码
			*(U8 *)(AD1+27+(length_1-1)*4)=0x30;		
			*(U8 *)(AD1+28+(length_1-1)*4)=asicc(a1);//起始地址，依次从高到低，要转成asicc码
			*(U8 *)(AD1+29+(length_1-1)*4)=asicc(a2);
			*(U8 *)(AD1+30+(length_1-1)*4)=asicc(a3);
			*(U8 *)(AD1+31+(length_1-1)*4)=asicc(a4);		

			t=length_2;   						//对数据长度取高低位，b1高位，b2低位，16进制  
			b3=(t&0xff00)>>8;
			b4=t&0xff;
			t1=t&0xf000;                		//对数据长度进行asicc处理，以显示asicc码，依次从高位到低位
			t1=t1>>12;
			t1=t1&0xf;
			t2=t&0xf00;
			t2=t2>>8;
			t2=t2&0xf;
			t3=t&0xf0;
			t3=t3>>4;
			t3=t3&0xf;
			t4=t&0xf;

			*(U8 *)(AD1+32+(length_1-1)*4)=asicc(t1);//发送长度，依次从高到低，要转成asicc码
			*(U8 *)(AD1+33+(length_1-1)*4)=asicc(t2);
			*(U8 *)(AD1+34+(length_1-1)*4)=asicc(t3);
			*(U8 *)(AD1+35+(length_1-1)*4)=asicc(t4);
			
			KK[1]=plcadd&0xff;                 		//LRC校验，16进制校验，asicc码显示 
			KK[2]=0x10;
			KK[3]=b1;
			KK[4]=b2;
			KK[5]=b3;
			KK[6]=b4;
							
			k1=((length_2*2)&0xf0)>>4;    		//对数据位长度进行asicc处理，以显示asicc码，依次从高位到低位
			k1=k1&0xf;
			k2=(length_2*2)&0xf;
			*(U8 *)(AD1+36+(length_1-1)*4)=asicc(k1);//发送位的长度，依次从高到低，要转成asicc码
			*(U8 *)(AD1+37+(length_1-1)*4)=asicc(k2);
			KK[7]=length_2*2;	
						

			for (i=length_1;i<length;i++)      	//写入多个元件值
			{				
				k4=ptcom->U8_Data[i*2];         //从D[]数组中要数据，对应的k3为高位，k4为低位
				k3=ptcom->U8_Data[i*2+1];
							
				k5=(k3&0xf0)>>4;                //对数据进行asicc处理，以显示asicc码，依次从高位到低位
				k6=k3&0xf;
				k7=(k4&0xf0)>>4;
				k8=k4&0xf;
				*(U8 *)(AD1+38+(length_1-1)*4+(i-length_1)*4)=asicc(k5);//发送位的数据，依次从高到低，要转成asicc码
				*(U8 *)(AD1+39+(length_1-1)*4+(i-length_1)*4)=asicc(k6);
				*(U8 *)(AD1+40+(length_1-1)*4+(i-length_1)*4)=asicc(k7);
				*(U8 *)(AD1+41+(length_1-1)*4+(i-length_1)*4)=asicc(k8);			
					
				KK[8+(i-length_1)*2]=k3;
				KK[9+(i-length_1)*2]=k4;		
			}
						
			aakj=CalLRC(KK,length_2*2+8);	     //计算和校检，LRC校验，高位在前，低位在后
			a1=(aakj&0xf0)>>4;
			a1=a1&0xf;
			a2=aakj&0xf;
			*(U8 *)(AD1+42+(length_1-1)*4+(length_2-1)*4)=asicc(a1);//asicc码显示
			*(U8 *)(AD1+43+(length_1-1)*4+(length_2-1)*4)=asicc(a2);
			*(U8 *)(AD1+44+(length_1-1)*4+(length_2-1)*4)=0x0d;     //结束字元0d，0a
			*(U8 *)(AD1+45+(length_1-1)*4+(length_2-1)*4)=0x0a;
			
			ptcom->send_length[1]=19+length_2*4;	//发送长度
			ptcom->send_staradd[1]=19+length_1*4;	//发送数据存储地址						
			ptcom->return_length[1]=17;				//返回数据长度
			ptcom->return_start[1]=0;				//返回数据有效开始
			ptcom->return_length_available[1]=0;	//返回有效数据长度	
			
			ptcom->Current_Times=0;					//当前发送次数
			ptcom->send_times=2;					//发送次数						
		}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////			
		else
		{
			if (b<=4095 && b+length<=4096)
			{
				add=0x1000;
			}
			if (b>=4096)
			{
				b=b-4096;
				add=0x9000;
			}
			
			b=b+add;								//加上偏移地址
			b1=(b&0xff00)>>8;           			//对开始偏移地址取高低位，b1高位，b2低位，16进制
			b2=b&0xff;
			a1=b&0xf000;                			//对开始偏移地址作asicc处理，依次从高到低
			a1=a1>>12;
			a1=a1&0xf;
			a2=b&0xf00;
			a2=a2>>8;
			a2=a2&0xf;
			a3=b&0xf0;
			a3=a3>>4;
			a3=a3&0xf;
			a4=b&0xf;

			*(U8 *)(AD1+0)=0x3a;        			//台达开始字元":",既3a      
			*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);//PLC站地址化为asicc码显示
			*(U8 *)(AD1+2)=asicc(plcadd&0xf); 
			*(U8 *)(AD1+3)=0x31;        			//功能码16，转成asicc码
			*(U8 *)(AD1+4)=0x30;		
			*(U8 *)(AD1+5)=asicc(a1);				//起始地址，依次从高到低，要转成asicc码
			*(U8 *)(AD1+6)=asicc(a2);
			*(U8 *)(AD1+7)=asicc(a3);
			*(U8 *)(AD1+8)=asicc(a4);
			
			t=ptcom->register_length;   			//对数据长度取高低位，b1高位，b2低位，16进制  
			b3=(t&0xff00)>>8;
			b4=t&0xff;
			t1=t&0xf000;                			//对数据长度进行asicc处理，以显示asicc码，依次从高位到低位
			t1=t1>>12;
			t1=t1&0xf;
			t2=t&0xf00;
			t2=t2>>8;
			t2=t2&0xf;
			t3=t&0xf0;
			t3=t3>>4;
			t3=t3&0xf;
			t4=t&0xf;
			*(U8 *)(AD1+9)=asicc(t1);				//发送长度，依次从高到低，要转成asicc码
			*(U8 *)(AD1+10)=asicc(t2);
			*(U8 *)(AD1+11)=asicc(t3);
			*(U8 *)(AD1+12)=asicc(t4);
			
			KK[1]=plcadd&0xff;                 			//LRC校验，16进制校验，asicc码显示 
			KK[2]=0x10;
			KK[3]=b1;
			KK[4]=b2;
			KK[5]=b3;
			KK[6]=b4;
							
			k1=((length*2)&0xf0)>>4;    			//对数据位长度进行asicc处理，以显示asicc码，依次从高位到低位
			k1=k1&0xf;
			k2=(length*2)&0xf;
			*(U8 *)(AD1+13)=asicc(k1);  			//发送位的长度，依次从高到低，要转成asicc码
			*(U8 *)(AD1+14)=asicc(k2);
			KK[7]=length*2;	
						

			for (i=0;i<length;i++)      			//写入多个元件值
			{				
				k4=ptcom->U8_Data[i*2];           	//从D[]数组中要数据，对应的k3为高位，k4为低位
				k3=ptcom->U8_Data[i*2+1];
							
				k5=(k3&0xf0)>>4;                 	//对数据进行asicc处理，以显示asicc码，依次从高位到低位
				k6=k3&0xf;
				k7=(k4&0xf0)>>4;
				k8=k4&0xf;
				*(U8 *)(AD1+15+i*4)=asicc(k5);   	//发送位的数据，依次从高到低，要转成asicc码
				*(U8 *)(AD1+16+i*4)=asicc(k6);
				*(U8 *)(AD1+17+i*4)=asicc(k7);
				*(U8 *)(AD1+18+i*4)=asicc(k8);			
					
				KK[8+i*2]=k3;
				KK[9+i*2]=k4;		
			}
					
			aakj=CalLRC(KK,length*2+8);	         	//计算和校检，LRC校验，高位在前，低位在后
			a1=(aakj&0xf0)>>4;
			a1=a1&0xf;
			a2=aakj&0xf;
			*(U8 *)(AD1+19+(length-1)*4)=asicc(a1);	//asicc码显示
			*(U8 *)(AD1+20+(length-1)*4)=asicc(a2);
			*(U8 *)(AD1+21+(length-1)*4)=0x0d;     	//结束字元0d，0a
			*(U8 *)(AD1+22+(length-1)*4)=0x0a;
			
			ptcom->send_length[0]=19+length*4;		//发送长度
			ptcom->send_staradd[0]=0;				//发送数据存储地址	
			ptcom->send_times=1;					//发送次数
					
			ptcom->return_length[0]=17;				//返回数据长度
			ptcom->return_start[0]=0;				//返回数据有效开始
			ptcom->return_length_available[0]=0;	//返回有效数据长度	
			ptcom->Current_Times=0;			
		}						
	//}		
}
/************************************************************************************************************************
写名称为tc的寄存器
*************************************************************************************************************************/
void Write_Analog_t()
{
	U16 aakj;
	int b,t,k1,k2,k3,k4,k5,k6,k7,k8;
	int a1,a2,a3,a4;
	int add;
	int b1,b2,b3,b4,b5,b6,b7,b8;
	int i;
	int length;
	int t1,t2,t3,t4;
	int plcadd;

	plcadd=ptcom->plc_address;	            		//PLC站地址	
	b=ptcom->address;								//开始地址	
	length=ptcom->register_length;	
	
	switch (ptcom->registerr)						//根据寄存器类型获得偏移地址
	{
		case 't':
			add=0x600;
			break;		
	}	
    
	b=b+add;										//加上偏移地址
	b1=(b&0xff00)>>8;           					//对开始偏移地址取高低位，b1高位，b2低位，16进制
	b2=b&0xff;
	a1=b&0xf000;                					//对开始偏移地址作asicc处理，依次从高到低
	a1=a1>>12;
	a1=a1&0xf;
	a2=b&0xf00;
	a2=a2>>8;
	a2=a2&0xf;
	a3=b&0xf0;
	a3=a3>>4;
	a3=a3&0xf;
	a4=b&0xf;
	if(length==1)                   				//如果数据较短，小于65535，则length==1
	{
		*(U8 *)(AD1+0)=0x3a;        				//台达开始字元":",既3a
		*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);//PLC站地址化为asicc码显示
		*(U8 *)(AD1+2)=asicc(plcadd&0xf);   
		*(U8 *)(AD1+3)=0x30;        				//功能码06，要转成asicc码          
		*(U8 *)(AD1+4)=0x36;		
		*(U8 *)(AD1+5)=asicc(a1);					//起始地址，依次从高到低，要转成asicc码
		*(U8 *)(AD1+6)=asicc(a2);
		*(U8 *)(AD1+7)=asicc(a3);
		*(U8 *)(AD1+8)=asicc(a4);
		
		b4=ptcom->U8_Data[0];       				//从D[]数组中要数据，对应的b3为高位，b4为低位
		b3=ptcom->U8_Data[1];
		
		b5=(b3&0xf0)>>4;            				//对数据进行asicc处理，以显示asicc码，依次从高位到低位
		b6=b3&0xf;
		b7=(b4&0xf0)>>4;
		b8=b4&0xf;
		*(U8 *)(AD1+9)=asicc(b5);
		*(U8 *)(AD1+10)=asicc(b6);
		*(U8 *)(AD1+11)=asicc(b7);
		*(U8 *)(AD1+12)=asicc(b8);
		KK[1]=plcadd&0xff;                 				//LRC校验，16进制校验，asicc码显示 
		KK[2]=0x06;
		KK[3]=b1;
		KK[4]=b2;
		KK[5]=b3;
		KK[6]=b4;
	
		aakj=CalLRC(KK,7);	        				//计算和校检，LRC校验，高位在前，低位在后
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13)=asicc(a1);  				//asicc码显示
		*(U8 *)(AD1+14)=asicc(a2); 
		*(U8 *)(AD1+15)=0x0d;       				//结束字元0d，0a
		*(U8 *)(AD1+16)=0x0a;
		
		ptcom->send_length[0]=17;					//发送长度
		ptcom->send_staradd[0]=0;					//发送数据存储地址	
		ptcom->send_times=1;						//发送次数
				
		ptcom->return_length[0]=17;					//返回数据长度
		ptcom->return_start[0]=0;					//返回数据有效开始
		ptcom->return_length_available[0]=0;		//返回有效数据长度	
		ptcom->Current_Times=0;	
	}
	if(length!=1)                  					//如果数据较长，大于65535，则length！=1   
	{
		*(U8 *)(AD1+0)=0x3a;        				//台达开始字元":",既3a      
		*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);//PLC站地址化为asicc码显示
		*(U8 *)(AD1+2)=asicc(plcadd&0xf); 
		*(U8 *)(AD1+3)=0x31;        				//功能码16，转成asicc码
		*(U8 *)(AD1+4)=0x30;		
		*(U8 *)(AD1+5)=asicc(a1);					//起始地址，依次从高到低，要转成asicc码
		*(U8 *)(AD1+6)=asicc(a2);
		*(U8 *)(AD1+7)=asicc(a3);
		*(U8 *)(AD1+8)=asicc(a4);

        t=ptcom->register_length;                   //对数据长度取高低位，b1高位，b2低位，16进制  
		
		b3=(t&0xff00)>>8;
		b4=t&0xff;
		t1=t&0xf000;                				//对数据长度进行asicc处理，以显示asicc码，依次从高位到低位
		t1=t1>>12;
		t1=t1&0xf;
		t2=t&0xf00;
		t2=t2>>8;
		t2=t2&0xf;
		t3=t&0xf0;
		t3=t3>>4;
		t3=t3&0xf;
		t4=t&0xf;
		*(U8 *)(AD1+9)=asicc(t1);					//发送长度，依次从高到低，要转成asicc码
		*(U8 *)(AD1+10)=asicc(t2);
		*(U8 *)(AD1+11)=asicc(t3);
		*(U8 *)(AD1+12)=asicc(t4);
		
		KK[1]=plcadd&0xff;                 				//LRC校验，16进制校验，asicc码显示 
		KK[2]=0x10;
		KK[3]=b1;
		KK[4]=b2;
		KK[5]=b3;
		KK[6]=b4;
						
		k1=((length*2)&0xf0)>>4;    				//对数据位长度进行asicc处理，以显示asicc码，依次从高位到低位
		k1=k1&0xf;
		k2=(length*2)&0xf;
		*(U8 *)(AD1+13)=asicc(k1);  				//发送位的长度，依次从高到低，要转成asicc码
		*(U8 *)(AD1+14)=asicc(k2);
		KK[7]=length*2;	

        for (i=0;i<length;i++)                      //写入多个元件值
        {               
            k4=ptcom->U8_Data[i*2];                 //从D[]数组中要数据，对应的k3为高位，k4为低位
            k3=ptcom->U8_Data[i*2+1];
                        
            k5=(k3&0xf0)>>4;                        //对数据进行asicc处理，以显示asicc码，依次从高位到低位
            k6=k3&0xf;
            k7=(k4&0xf0)>>4;
            k8=k4&0xf;
            *(U8 *)(AD1+15+i*4)=asicc(k5);          //发送位的数据，依次从高到低，要转成asicc码
            *(U8 *)(AD1+16+i*4)=asicc(k6);
            *(U8 *)(AD1+17+i*4)=asicc(k7);
            *(U8 *)(AD1+18+i*4)=asicc(k8);          
                
            KK[8+i*2]=k3;
            KK[9+i*2]=k4;       
        }   

        aakj=CalLRC(KK,length*2+8);	         	//计算和校检，LRC校验，高位在前，低位在后
        a1=(aakj&0xf0)>>4;
        a1=a1&0xf;
        a2=aakj&0xf;
        *(U8 *)(AD1+19+(length-1)*4)=asicc(a1);	//asicc码显示
        *(U8 *)(AD1+20+(length-1)*4)=asicc(a2);
        *(U8 *)(AD1+21+(length-1)*4)=0x0d;     	//结束字元0d，0a
        *(U8 *)(AD1+22+(length-1)*4)=0x0a;
        
        ptcom->send_length[0]=19+length*4;			//发送长度
        ptcom->send_staradd[0]=0;					//发送数据存储地址	
        ptcom->send_times=1;						//发送次数
        		
        ptcom->return_length[0]=17;				//返回数据长度
        ptcom->return_start[0]=0;					//返回数据有效开始
        ptcom->return_length_available[0]=0;		//返回有效数据长度	
        ptcom->Current_Times=0;	

	}	
}


/************************************************************************************************************************
写名称为tc的寄存器
*************************************************************************************************************************/
void Write_Analog_c()
{
	U16 aakj;
	int b,t,k1,k2,k3,k4,k5,k6,k7,k8,k9,k10,k11,k12,k13,k14;
	int a1,a2,a3,a4;
	int add;
	int b1,b2,b3,b4,b5,b6,b7,b8;
	int i;
	int length,length1;
	int t1,t2,t3,t4;
	int plcadd;
	
	plcadd=ptcom->plc_address;	            		//PLC站地址	
	b=ptcom->address;								//开始地址	
	length=ptcom->register_length;	

	switch (ptcom->registerr)						//根据寄存器类型获得偏移地址
	{
		case 'c':
			add=0xe00;
			break;						
	}	
	if(ptcom->registerr=='c')
	{
        if (b>=200)
        {
            b=((b-200)/2)+200;
        }
	} 
	
	b=b+add;										//加上偏移地址
	b1=(b&0xff00)>>8;           					//对开始偏移地址取高低位，b1高位，b2低位，16进制
	b2=b&0xff;
	a1=b&0xf000;                					//对开始偏移地址作asicc处理，依次从高到低
	a1=a1>>12;
	a1=a1&0xf;
	a2=b&0xf00;
	a2=a2>>8;
	a2=a2&0xf;
	a3=b&0xf0;
	a3=a3>>4;
	a3=a3&0xf;
	a4=b&0xf;
	
	if(length==1)                   				//如果数据较短，小于65535，则length==1
	{
	
		*(U8 *)(AD1+0)=0x3a;        				//台达开始字元":",既3a
		*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);//PLC站地址化为asicc码显示
		*(U8 *)(AD1+2)=asicc(plcadd&0xf);   
		*(U8 *)(AD1+3)=0x30;        				//功能码06，要转成asicc码          
		*(U8 *)(AD1+4)=0x36;		
		*(U8 *)(AD1+5)=asicc(a1);					//起始地址，依次从高到低，要转成asicc码
		*(U8 *)(AD1+6)=asicc(a2);
		*(U8 *)(AD1+7)=asicc(a3);
		*(U8 *)(AD1+8)=asicc(a4);
		
		b4=ptcom->U8_Data[0];       				//从D[]数组中要数据，对应的b3为高位，b4为低位
		b3=ptcom->U8_Data[1];
		
		b5=(b3&0xf0)>>4;            				//对数据进行asicc处理，以显示asicc码，依次从高位到低位
		b6=b3&0xf;
		b7=(b4&0xf0)>>4;
		b8=b4&0xf;
		*(U8 *)(AD1+9)=asicc(b5);
		*(U8 *)(AD1+10)=asicc(b6);
		*(U8 *)(AD1+11)=asicc(b7);
		*(U8 *)(AD1+12)=asicc(b8);
		KK[1]=plcadd&0xff;                 				//LRC校验，16进制校验，asicc码显示 
		KK[2]=0x06;
		KK[3]=b1;
		KK[4]=b2;
		KK[5]=b3;
		KK[6]=b4;
	
		aakj=CalLRC(KK,7);	        				//计算和校检，LRC校验，高位在前，低位在后
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13)=asicc(a1);  				//asicc码显示
		*(U8 *)(AD1+14)=asicc(a2); 
		*(U8 *)(AD1+15)=0x0d;       				//结束字元0d，0a
		*(U8 *)(AD1+16)=0x0a;
		
		ptcom->send_length[0]=17;					//发送长度
		ptcom->send_staradd[0]=0;					//发送数据存储地址	
		ptcom->send_times=1;						//发送次数
				
		ptcom->return_length[0]=17;					//返回数据长度
		ptcom->return_start[0]=0;					//返回数据有效开始
		ptcom->return_length_available[0]=0;		//返回有效数据长度	
		ptcom->Current_Times=0;	
	}
	if(length!=1)                  					//如果数据较长，大于65535，则length！=1   
	{
		*(U8 *)(AD1+0)=0x3a;        				//台达开始字元":",既3a      
		*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);//PLC站地址化为asicc码显示
		*(U8 *)(AD1+2)=asicc(plcadd&0xf); 
		*(U8 *)(AD1+3)=0x31;        				//功能码16，转成asicc码
		*(U8 *)(AD1+4)=0x30;		
		*(U8 *)(AD1+5)=asicc(a1);					//起始地址，依次从高到低，要转成asicc码
		*(U8 *)(AD1+6)=asicc(a2);
		*(U8 *)(AD1+7)=asicc(a3);
		*(U8 *)(AD1+8)=asicc(a4);

        t=ptcom->register_length/2;                   //对数据长度取高低位，b1高位，b2低位，16进制  
        length1 = ptcom->register_length/2; 

		b3=(t&0xff00)>>8;
		b4=t&0xff;
		t1=t&0xf000;                				//对数据长度进行asicc处理，以显示asicc码，依次从高位到低位
		t1=t1>>12;
		t1=t1&0xf;
		t2=t&0xf00;
		t2=t2>>8;
		t2=t2&0xf;
		t3=t&0xf0;
		t3=t3>>4;
		t3=t3&0xf;
		t4=t&0xf;
		*(U8 *)(AD1+9)=asicc(t1);					//发送长度，依次从高到低，要转成asicc码
		*(U8 *)(AD1+10)=asicc(t2);
		*(U8 *)(AD1+11)=asicc(t3);
		*(U8 *)(AD1+12)=asicc(t4);
		
		KK[1]=plcadd&0xff;                 				//LRC校验，16进制校验，asicc码显示 
		KK[2]=0x10;
		KK[3]=b1;
		KK[4]=b2;
		KK[5]=b3;
		KK[6]=b4;
						
		k1=((length*2)&0xf0)>>4;    				//对数据位长度进行asicc处理，以显示asicc码，依次从高位到低位
		k1=k1&0xf;
		k2=(length*2)&0xf;
		*(U8 *)(AD1+13)=asicc(k1);  				//发送位的长度，依次从高到低，要转成asicc码
		*(U8 *)(AD1+14)=asicc(k2);
		KK[7]=length*2;	

            
        for (i=0;i<length1;i++)                    //写入多个元件值
        {               
            k4=ptcom->U8_Data[i*4];                 //从D[]数组中要数据，对应的k3为高位，k4为低位
            k3=ptcom->U8_Data[i*4+1];
            k9=ptcom->U8_Data[i*4+2];               //从D[]数组中要数据，对应的k3为高位，k4为低位
            k10=ptcom->U8_Data[i*4+3];
                        
            k5=(k3&0xf0)>>4;                        //对数据进行asicc处理，以显示asicc码，依次从高位到低位
            k6=k3&0xf;
            k7=(k4&0xf0)>>4;
            k8=k4&0xf;
            k11=(k10&0xf0)>>4;                      //对数据进行asicc处理，以显示asicc码，依次从高位到低位
            k12=k10&0xf;
            k13=(k9&0xf0)>>4;
            k14=k9&0xf;             
    
            *(U8 *)(AD1+15+i*8)=asicc(k11);         //发送位的数据，依次从高到低，要转成asicc码
            *(U8 *)(AD1+16+i*8)=asicc(k12);
            *(U8 *)(AD1+17+i*8)=asicc(k13);
            *(U8 *)(AD1+18+i*8)=asicc(k14); 
    
            *(U8 *)(AD1+19+i*8)=asicc(k5);          //发送位的数据，依次从高到低，要转成asicc码
            *(U8 *)(AD1+20+i*8)=asicc(k6);
            *(U8 *)(AD1+21+i*8)=asicc(k7);
            *(U8 *)(AD1+22+i*8)=asicc(k8);          
    
            KK[8+i*4]=k10;
            KK[9+i*4]=k9;                   
            KK[10+i*4]=k3;
            KK[11+i*4]=k4;      
        }
        
        aakj=CalLRC(KK,length1*4+8);	         		//计算和校检，LRC校验，高位在前，低位在后
        a1=(aakj&0xf0)>>4;
        a1=a1&0xf;
        a2=aakj&0xf;
        
        *(U8 *)(AD1+23+(length1-1)*8)=asicc(a1);		//asicc码显示
        *(U8 *)(AD1+24+(length1-1)*8)=asicc(a2);
        *(U8 *)(AD1+25+(length1-1)*8)=0x0d;     		//结束字元0d，0a
        *(U8 *)(AD1+26+(length1-1)*8)=0x0a;
                    
        ptcom->send_length[0]=19+length1*8;			//发送长度
        ptcom->send_staradd[0]=0;					//发送数据存储地址	
        ptcom->send_times=1;						//发送次数
        		
        ptcom->return_length[0]=17;					//返回数据长度
        ptcom->return_start[0]=0;					//返回数据有效开始
        ptcom->return_length_available[0]=0;		//返回有效数据长度	
        ptcom->Current_Times=0;	
		
	}	
}



/************************************************************************************************************************
写时间子函数
*************************************************************************************************************************/
void Write_Time()                                	//写时间到PLC
{
	Write_Analog();									
}
/************************************************************************************************************************
读时间子函数
*************************************************************************************************************************/
void Read_Time()										//从PLC读取时间
{
	Read_Analog();
}
/************************************************************************************************************************
读配方子函数
*************************************************************************************************************************/
void Read_Recipe()									//读取配方
{
	U16 aakj;
	int b,b1,b2;
	int a1,a2,a3,a4;
	int i;
	int datalength;                             	//数据长度
	int p_start;                                	//数据开始地址
	int ps;
	int SendTimes;                              	//发送次数
	int LastTimeWord;								//最后一次发送长度
	int currentlength;
	int plcadd;                                 	//PLC站地址
	int add,add_1;
	int length_1,length_2;
	int SendTimes_1,SendTimes_2;
	int LastTimeWord_1,LastTimeWord_2;
	
		
	datalength=ptcom->register_length;				//发送总长度
	p_start=ptcom->address;							//开始地址
	plcadd=ptcom->plc_address;	            		//PLC站地址

	if(datalength>5000)                     		//限制长度
	{
		datalength=5000;
	}

	if ((p_start<=4095 && p_start+datalength<=4096) || (p_start>=4096))
	{	
		if(datalength%15==0)                    	//台达最多能发15个D，数据刚好是15D的倍数
		{
			SendTimes=datalength/15;            	//发送次数
			LastTimeWord=15;                    	//最后一次发送的长度为15D	
		}
		if(datalength%15!=0)                    	//台达最多能发15个D，数据不是15D的倍数 
		{
			SendTimes=datalength/15+1;          	//发送的次数
			LastTimeWord=datalength%15;         	//最后一次发送的长度为除16的余数	
		}
		

		for (i=0;i<SendTimes;i++)
		{
			ps=i*17;                            	//每次发17个长度
			b=p_start+i*15;                     	//起始地址
			if (b<=0xfff)
			{
				b=b+0x1000;
			}
			else
			{
				b=b-4096;
				b=b+0x9000;
			}
			b1=(b&0xff00)>>8;                   	//对开始偏移地址取高低位，b1高位，b2低位，16进制
			b2=b&0xff;	
			a1=b&0xf000;                        	//对开始偏移地址作asicc处理，依次从高到低 
			a1=a1>>12;
			a1=a1&0xf;
			a2=b&0xf00;
			a2=a2>>8;
			a2=a2&0xf;
			a3=b&0xf0;
			a3=a3>>4;
			a3=a3&0xf;
			a4=b&0xf;
			*(U8 *)(AD1+0+ps)=0x3a;             	//台达开始字元":",既3a
			*(U8 *)(AD1+1+ps)=asicc(((plcadd&0xf0)>>4)&0xf);//PLC站地址化为asicc码显示
			*(U8 *)(AD1+2+ps)=asicc(plcadd&0xf);
			*(U8 *)(AD1+3+ps)=0x30;            	 	//功能码03，要转成asicc码
			*(U8 *)(AD1+4+ps)=0x33;	
			*(U8 *)(AD1+5+ps)=asicc(a1);	    	//开始地址，依次从高到低，要转成asicc码
			*(U8 *)(AD1+6+ps)=asicc(a2);
			*(U8 *)(AD1+7+ps)=asicc(a3);
			*(U8 *)(AD1+8+ps)=asicc(a4);
			KK[1]=plcadd;                       	//LRC校验，16进制校验，asicc码显示                     
			KK[2]=0x03;
			KK[3]=b1;
			KK[4]=b2;
			
			if (i!=(SendTimes-1))	            	//不是最后一次发送时
			{
				*(U8 *)(AD1+9+ps)=0x30;         	//固定长度15个D，即32字节，高位，asicc码显示
				*(U8 *)(AD1+10+ps)=0x30;
				*(U8 *)(AD1+11+ps)=0x30;        	//固定长度15个D，即32字节，低位，asicc码显示
				*(U8 *)(AD1+12+ps)=0x46;
				KK[5]=0x00;
				KK[6]=0x0f;
				currentlength=15;               	//固定长度16个D               
			}
			if (i==(SendTimes-1))	            	//最后一次发送时
			{		
				*(U8 *)(AD1+9+ps)=0x30;         	//剩余LastTimeWord个D，高位，转成asicc码
				*(U8 *)(AD1+10+ps)=0x30;
				*(U8 *)(AD1+11+ps)=asicc(((LastTimeWord&0xf0)>>4)&0xf);//剩余LastTimeWord个D，转成asicc码
				*(U8 *)(AD1+12+ps)=asicc(LastTimeWord&0xf);
				KK[5]=0x00;
				KK[6]=LastTimeWord&0xff;
				currentlength=LastTimeWord;     	//剩余LastTimeWord个D 
			}
			aakj=CalLRC(KK,7);	                	//计算和校检，LRC校验，高位在前，低位在后
			a1=(aakj&0xf0)>>4;
			a1=a1&0xf;
			a2=aakj&0xf;
			*(U8 *)(AD1+13+ps)=asicc(a1);       	//asicc码显示
			*(U8 *)(AD1+14+ps)=asicc(a2);
			*(U8 *)(AD1+15+ps)=0x0d;            	//结束字元0d,0a
			*(U8 *)(AD1+16+ps)=0x0a;
						
			ptcom->send_length[i]=17;				//发送长度
			ptcom->send_staradd[i]=i*17;			//发送数据存储地址	
			ptcom->send_add[i]=p_start+i*15;		//读的是这个地址的数据	
			ptcom->send_data_length[i]=currentlength;//不是最后一次都是15个D
					
			ptcom->return_length[i]=11+currentlength*4;//返回数据长度，有9个固定，3a，PLC地址2个长度，功能码2个长度，
		                                            //位数显示2个长度，校检2个长度，0d，0a
			ptcom->return_start[i]=7;				//返回数据有效开始
			ptcom->return_length_available[i]=currentlength*4;//返回有效数据长度		
		}
		ptcom->send_times=SendTimes;				//发送次数
		ptcom->Current_Times=0;	
	}
/////////////////////////////////////////////////////////////////////////////////////	
	else
	{
		add=p_start;
		length_1=4096-p_start;
		
		if(length_1%15==0)                    		//台达最多能发15个D，数据刚好是15D的倍数
		{
			SendTimes_1=length_1/15;            	//发送次数
			LastTimeWord_1=15;                    	//最后一次发送的长度为15D	
		}
		if(length_1%15!=0)                    		//台达最多能发15个D，数据不是15D的倍数 
		{
			SendTimes_1=length_1/15+1;          	//发送的次数
			LastTimeWord_1=length_1%15;         	//最后一次发送的长度为除16的余数	
		}
		
		for (i=0;i<SendTimes_1;i++)
		{
			ps=i*17;                            	//每次发17个长度
			b=add+i*15;                     		//起始地址
			b=b+0x1000;
			
			b1=(b&0xff00)>>8;                   	//对开始偏移地址取高低位，b1高位，b2低位，16进制
			b2=b&0xff;	
			a1=b&0xf000;                        	//对开始偏移地址作asicc处理，依次从高到低 
			a1=a1>>12;
			a1=a1&0xf;
			a2=b&0xf00;
			a2=a2>>8;
			a2=a2&0xf;
			a3=b&0xf0;
			a3=a3>>4;
			a3=a3&0xf;
			a4=b&0xf;
			*(U8 *)(AD1+0+ps)=0x3a;             	//台达开始字元":",既3a
			*(U8 *)(AD1+1+ps)=asicc(((plcadd&0xf0)>>4)&0xf);//PLC站地址化为asicc码显示
			*(U8 *)(AD1+2+ps)=asicc(plcadd&0xf);
			*(U8 *)(AD1+3+ps)=0x30;             	//功能码03，要转成asicc码
			*(U8 *)(AD1+4+ps)=0x33;	
			*(U8 *)(AD1+5+ps)=asicc(a1);	    	//开始地址，依次从高到低，要转成asicc码
			*(U8 *)(AD1+6+ps)=asicc(a2);
			*(U8 *)(AD1+7+ps)=asicc(a3);
			*(U8 *)(AD1+8+ps)=asicc(a4);
			KK[1]=plcadd;                       	//LRC校验，16进制校验，asicc码显示                     
			KK[2]=0x03;
			KK[3]=b1;
			KK[4]=b2;
			
			if (i!=(SendTimes_1-1))	            	//不是最后一次发送时
			{
				*(U8 *)(AD1+9+ps)=0x30;         	//固定长度15个D，即32字节，高位，asicc码显示
				*(U8 *)(AD1+10+ps)=0x30;
				*(U8 *)(AD1+11+ps)=0x30;        	//固定长度15个D，即32字节，低位，asicc码显示
				*(U8 *)(AD1+12+ps)=0x46;
				KK[5]=0x00;
				KK[6]=0x0f;
				currentlength=15;               	//固定长度16个D               
			}	
			if (i==(SendTimes_1-1))	            	//最后一次发送时
			{		
				*(U8 *)(AD1+9+ps)=0x30;         	//剩余LastTimeWord个D，高位，转成asicc码
				*(U8 *)(AD1+10+ps)=0x30;
				*(U8 *)(AD1+11+ps)=asicc(((LastTimeWord_1&0xf0)>>4)&0xf);//剩余LastTimeWord个D，转成asicc码
				*(U8 *)(AD1+12+ps)=asicc(LastTimeWord_1&0xf);
				KK[5]=0x00;
				KK[6]=LastTimeWord_1&0xff;
				currentlength=LastTimeWord_1;     	//剩余LastTimeWord个D 
			}
			aakj=CalLRC(KK,7);	                	//计算和校检，LRC校验，高位在前，低位在后
			a1=(aakj&0xf0)>>4;
			a1=a1&0xf;
			a2=aakj&0xf;
			*(U8 *)(AD1+13+ps)=asicc(a1);       	//asicc码显示
			*(U8 *)(AD1+14+ps)=asicc(a2);
			*(U8 *)(AD1+15+ps)=0x0d;            	//结束字元0d,0a
			*(U8 *)(AD1+16+ps)=0x0a;
						
			ptcom->send_length[i]=17;				//发送长度
			ptcom->send_staradd[i]=i*17;			//发送数据存储地址	
			ptcom->send_add[i]=p_start+i*15;		//读的是这个地址的数据	
			ptcom->send_data_length[i]=currentlength;//不是最后一次都是15个D
					
			ptcom->return_length[i]=11+currentlength*4;//返回数据长度，有9个固定，3a，PLC地址2个长度，功能码2个长度，
		                                            //位数显示2个长度，校检2个长度，0d，0a
			ptcom->return_start[i]=7;				//返回数据有效开始
			ptcom->return_length_available[i]=currentlength*4;//返回有效数据长度	
		}
//---------------------------------------------------------------------------------------------------
		add_1=0x9000;	
		length_2=datalength-length_1;
		
		if(length_2%15==0)                    		//台达最多能发15个D，数据刚好是15D的倍数
		{
			SendTimes_2=length_2/15;            	//发送次数
			LastTimeWord_2=15;                    	//最后一次发送的长度为15D	
		}
		if(length_2%15!=0)                    		//台达最多能发15个D，数据不是15D的倍数 
		{
			SendTimes_2=length_2/15+1;          	//发送的次数
			LastTimeWord_2=length_2%15;         	//最后一次发送的长度为除16的余数	
		}
		
		for (i=0;i<SendTimes_2;i++)
		{
			ps=i*17;                            	//每次发17个长度
			b=add_1+i*15;                     		//起始地址
			
			b1=(b&0xff00)>>8;                   	//对开始偏移地址取高低位，b1高位，b2低位，16进制
			b2=b&0xff;	
			a1=b&0xf000;                        	//对开始偏移地址作asicc处理，依次从高到低 
			a1=a1>>12;
			a1=a1&0xf;
			a2=b&0xf00;
			a2=a2>>8;
			a2=a2&0xf;
			a3=b&0xf0;
			a3=a3>>4;
			a3=a3&0xf;
			a4=b&0xf;
			*(U8 *)(AD1+0+ps+SendTimes_1*17)=0x3a;  //台达开始字元":",既3a
			*(U8 *)(AD1+1+ps+SendTimes_1*17)=asicc(((plcadd&0xf0)>>4)&0xf);//PLC站地址化为asicc码显示
			*(U8 *)(AD1+2+ps+SendTimes_1*17)=asicc(plcadd&0xf);
			*(U8 *)(AD1+3+ps+SendTimes_1*17)=0x30;  //功能码03，要转成asicc码
			*(U8 *)(AD1+4+ps+SendTimes_1*17)=0x33;	
			*(U8 *)(AD1+5+ps+SendTimes_1*17)=asicc(a1);//开始地址，依次从高到低，要转成asicc码
			*(U8 *)(AD1+6+ps+SendTimes_1*17)=asicc(a2);
			*(U8 *)(AD1+7+ps+SendTimes_1*17)=asicc(a3);
			*(U8 *)(AD1+8+ps+SendTimes_1*17)=asicc(a4);
			KK[1]=plcadd;                       	//LRC校验，16进制校验，asicc码显示                     
			KK[2]=0x03;
			KK[3]=b1;
			KK[4]=b2;
			
			if (i!=(SendTimes_2-1))	            	//不是最后一次发送时
			{
				*(U8 *)(AD1+9+ps+SendTimes_1*17)=0x30;//固定长度15个D，即32字节，高位，asicc码显示
				*(U8 *)(AD1+10+ps+SendTimes_1*17)=0x30;
				*(U8 *)(AD1+11+ps+SendTimes_1*17)=0x30;//固定长度15个D，即32字节，低位，asicc码显示
				*(U8 *)(AD1+12+ps+SendTimes_1*17)=0x46;
				KK[5]=0x00;
				KK[6]=0x0f;
				currentlength=15;               	//固定长度16个D               
			}
			if (i==(SendTimes_2-1))	            	//最后一次发送时
			{		
				*(U8 *)(AD1+9+ps+SendTimes_1*17)=0x30;//剩余LastTimeWord个D，高位，转成asicc码
				*(U8 *)(AD1+10+ps+SendTimes_1*17)=0x30;
				*(U8 *)(AD1+11+ps+SendTimes_1*17)=asicc(((LastTimeWord_2&0xf0)>>4)&0xf);//剩余LastTimeWord个D，转成asicc码
				*(U8 *)(AD1+12+ps+SendTimes_1*17)=asicc(LastTimeWord_2&0xf);
				KK[5]=0x00;
				KK[6]=LastTimeWord_2&0xff;
				currentlength=LastTimeWord_2;     	//剩余LastTimeWord个D 
			}
			aakj=CalLRC(KK,7);	                	//计算和校检，LRC校验，高位在前，低位在后
			a1=(aakj&0xf0)>>4;
			a1=a1&0xf;
			a2=aakj&0xf;
			*(U8 *)(AD1+13+ps+SendTimes_1*17)=asicc(a1);//asicc码显示
			*(U8 *)(AD1+14+ps+SendTimes_1*17)=asicc(a2);
			*(U8 *)(AD1+15+ps+SendTimes_1*17)=0x0d; //结束字元0d,0a
			*(U8 *)(AD1+16+ps+SendTimes_1*17)=0x0a;
						
			ptcom->send_length[i+SendTimes_1]=17;	//发送长度
			ptcom->send_staradd[i+SendTimes_1]=i*17+SendTimes_1*17;//发送数据存储地址	
			ptcom->send_add[i+SendTimes_1]=4096+i*15;//读的是这个地址的数据	
			ptcom->send_data_length[i+SendTimes_1]=currentlength;//不是最后一次都是15个D
					
			ptcom->return_length[i+SendTimes_1]=11+currentlength*4;//返回数据长度，有9个固定，3a，PLC地址2个长度，功能码2个长度，
		                                            //位数显示2个长度，校检2个长度，0d，0a
			ptcom->return_start[i+SendTimes_1]=7;	//返回数据有效开始
			ptcom->return_length_available[i+SendTimes_1]=currentlength*4;//返回有效数据长度	
		}
		ptcom->send_times=SendTimes_1+SendTimes_2;	//发送次数
		ptcom->Current_Times=0;												
	}
	ptcom->send_staradd[90]=1;		
}
/************************************************************************************************************************
写配方子函数
*************************************************************************************************************************/
void Write_Recipe()									//写配方到PLC
{
	int datalength;
	int staradd;
	int SendTimes;                              	//发送的次数
	int LastTimeWord;								//最后一次发送长度
	int i,j;
	int ps;
	int b;
	int a1,a2,a3,a4;
	U16 aakj;
	int b1,b2,b3,b4;
	int length;
	int plcadd;	
	int t,k1,k2,k3,k4,k5,k6,k7,k8;
	int t1,t2,t3,t4;
	int add,add_1;
	int length_1,length_2;
	int SendTimes_1,SendTimes_2;
	int LastTimeWord_1,LastTimeWord_2;
	
	
	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));//数据长度
	staradd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//数据开始地址
	plcadd=*(U8 *)(PE+4);	                   		//PLC站地址

	if ((staradd<=4095 && staradd+datalength<=4096) || (staradd>=4096))
	{
		if(datalength%15==0)                       	//台达最多能发15个D，数据长度刚好是16的倍数时
		{
			SendTimes=datalength/15;               	//发送的次数为datalength/15               
			LastTimeWord=15;                       	//最后一次发送的长度为15个D
		}
		if(datalength%15!=0)                       	//数据长度不是15D的倍数时
		{
			SendTimes=datalength/15+1;             	//发送的次数datalength/15+1
			LastTimeWord=datalength%15;            	//最后一次发送的长度为除16的余数
		}	
		
		ps=79;                                     	//发15个D要发83个长度
		
		for (i=0;i<SendTimes;i++)
		{   
			if (i!=(SendTimes-1))                  	//不是最后一次发送时
			{	
				length=15;                         	//发15个D
			}
			else                                   	//最后一次发送时
			{
				length=LastTimeWord;               	//发剩余的长度             
			}
			
			b=staradd+i*15;                        	//起始地址
			if (b<=0xfff)
			{
				b=b+0x1000;
			}
			else
			{
				b=b-4096;
				b=b+0x9000;
			}
			b1=(b&0xff00)>>8;                      	//对地址做16进制处理，b1为高位，b2为低位
			b2=b&0xff;
			a1=b&0xf000;                           	//对地址做asicc处理，以显示asicc码，依次从高位到低位
			a1=a1>>12;
			a1=a1&0xf;
			a2=b&0xf00;
			a2=a2>>8;
			a2=a2&0xf;
			a3=b&0xf0;
			a3=a3>>4;
			a3=a3&0xf;
			a4=b&0xf;
			*(U8 *)(AD1+0+ps*i)=0x3a;              	//台达起始字元3a
			*(U8 *)(AD1+1+ps*i)=asicc(((plcadd&0xf0)>>4)&0xf);//PLC站地址做asicc处理并显示asicc码，高位在前
			*(U8 *)(AD1+2+ps*i)=asicc(plcadd&0xf);
			*(U8 *)(AD1+3+ps*i)=0x31;              	//功能码16，以asicc显示
			*(U8 *)(AD1+4+ps*i)=0x30;
			*(U8 *)(AD1+5+ps*i)=asicc(a1);         	//开始地址，以asicc显示，依次从高位到低位
			*(U8 *)(AD1+6+ps*i)=asicc(a2);
			*(U8 *)(AD1+7+ps*i)=asicc(a3);
			*(U8 *)(AD1+8+ps*i)=asicc(a4);
			
			
			t=length;                              
			b3=(t&0xff00)>>8;                      	//对长度做16进制处理，b3为高位，b4为低位
			b4=t&0xff;
			t1=t&0xf000;                           	//对长度做asicc处理，以显示asicc码，依次从高位到低位
			t1=t1>>12;
			t1=t1&0xf;
			t2=t&0xf00;
			t2=t2>>8;
			t2=t2&0xf;
			t3=t&0xf0;
			t3=t3>>4;
			t3=t3&0xf;
			t4=t&0xf;
			*(U8 *)(AD1+9+ps*i)=asicc(t1);	       //发送长度，以asicc码发送
			*(U8 *)(AD1+10+ps*i)=asicc(t2);
			*(U8 *)(AD1+11+ps*i)=asicc(t3);
			*(U8 *)(AD1+12+ps*i)=asicc(t4);
			
			k1=((length*2)&0xf0)>>4;               //发送的位长度做asicc处理，以显示asicc码，依次从高位到低位
			k1=k1&0xf;
			k2=(length*2)&0xf;
			*(U8 *)(AD1+13+ps*i)=asicc(k1);        //发送的位长度，以asicc码发送
			*(U8 *)(AD1+14+ps*i)=asicc(k2);
							
			KK[1]=plcadd;                          //LRC校验码，以16进制表示，对应上PLC站地址，功能码，起始地址，长度，位长度
			KK[2]=0x10;
			KK[3]=b1;
			KK[4]=b2;
			KK[5]=b3;
			KK[6]=b4;
			KK[7]=length*2;
							
			for(j=0;j<length;j++)                  //写入多个元件值                 
			{	
				k4=*(U8 *)(PE+9+i*30+j*2);         //从存数据的寄存器开始地址PE+9取数据，k3为高位，k4为低位
				k3=*(U8 *)(PE+9+i*30+j*2+1);
							
				k5=(k3&0xf0)>>4;                   //对取得的数据做asicc处理
				k6=k3&0xf;
				k7=(k4&0xf0)>>4;
				k8=k4&0xf;
				*(U8 *)(AD1+15+j*4+ps*i)=asicc(k5);//发送取得的数据，以显示asicc码，依次从高位到低位
				*(U8 *)(AD1+16+j*4+ps*i)=asicc(k6);
				*(U8 *)(AD1+17+j*4+ps*i)=asicc(k7);
				*(U8 *)(AD1+18+j*4+ps*i)=asicc(k8);			
					
				KK[8+j*2]=k3;
				KK[9+j*2]=k4;					
			}
			aakj=CalLRC(KK,length*2+8);	           //计算和校检，LRC校验，高位在前，低位在后
			a1=(aakj&0xf0)>>4;
			a1=a1&0xf;
			a2=aakj&0xf;
			*(U8 *)(AD1+19+(length-1)*4+ps*i)=asicc(a1);
			*(U8 *)(AD1+20+(length-1)*4+ps*i)=asicc(a2);
			*(U8 *)(AD1+21+(length-1)*4+ps*i)=0x0d;//台达结束字元0d，0a
			*(U8 *)(AD1+22+(length-1)*4+ps*i)=0x0a;
		
			ptcom->send_length[i]=19+length*4;	   //发送长度
			ptcom->send_staradd[i]=i*ps;		   //发送数据存储地址	
			ptcom->return_length[i]=17;			   //返回数据长度
			ptcom->return_start[i]=0;			   //返回数据有效开始
			ptcom->return_length_available[i]=0;   //返回有效数据长度	
					
		}
		ptcom->send_times=SendTimes;			   //发送次数
		ptcom->Current_Times=0;					   //当前发送次数
	}
/////////////////////////////////////////////////////////////////////////////////////////////	
	else
	{
		add=staradd;
		length_1=4096-staradd;
		
		if(length_1%15==0)                       	//台达最多能发15个D，数据长度刚好是16的倍数时
		{
			SendTimes_1=length_1/15;               	//发送的次数为datalength/15               
			LastTimeWord_1=15;                      //最后一次发送的长度为15个D
		}
		if(length_1%15!=0)                       	//数据长度不是15D的倍数时
		{
			SendTimes_1=length_1/15+1;             	//发送的次数datalength/15+1
			LastTimeWord_1=length_1%15;            	//最后一次发送的长度为除16的余数
		}	
		
		ps=79;                                     	//发15个D要发83个长度
		
		for (i=0;i<SendTimes_1;i++)
		{   
			if (i!=(SendTimes_1-1))                 //不是最后一次发送时
			{	
				length=15;                         	//发15个D
			}
			else                                   	//最后一次发送时
			{
				length=LastTimeWord_1;              //发剩余的长度             
			}
			
			b=add+i*15;                        		//起始地址
			b=b+0x1000;

			b1=(b&0xff00)>>8;                      	//对地址做16进制处理，b1为高位，b2为低位
			b2=b&0xff;
			a1=b&0xf000;                           	//对地址做asicc处理，以显示asicc码，依次从高位到低位
			a1=a1>>12;
			a1=a1&0xf;
			a2=b&0xf00;
			a2=a2>>8;
			a2=a2&0xf;
			a3=b&0xf0;
			a3=a3>>4;
			a3=a3&0xf;
			a4=b&0xf;
			*(U8 *)(AD1+0+ps*i)=0x3a;              	//台达起始字元3a
			*(U8 *)(AD1+1+ps*i)=asicc(((plcadd&0xf0)>>4)&0xf);//PLC站地址做asicc处理并显示asicc码，高位在前
			*(U8 *)(AD1+2+ps*i)=asicc(plcadd&0xf);
			*(U8 *)(AD1+3+ps*i)=0x31;              	//功能码16，以asicc显示
			*(U8 *)(AD1+4+ps*i)=0x30;
			*(U8 *)(AD1+5+ps*i)=asicc(a1);         	//开始地址，以asicc显示，依次从高位到低位
			*(U8 *)(AD1+6+ps*i)=asicc(a2);
			*(U8 *)(AD1+7+ps*i)=asicc(a3);
			*(U8 *)(AD1+8+ps*i)=asicc(a4);
					
			t=length;                              
			b3=(t&0xff00)>>8;                      	//对长度做16进制处理，b3为高位，b4为低位
			b4=t&0xff;
			t1=t&0xf000;                           	//对长度做asicc处理，以显示asicc码，依次从高位到低位
			t1=t1>>12;
			t1=t1&0xf;
			t2=t&0xf00;
			t2=t2>>8;
			t2=t2&0xf;
			t3=t&0xf0;
			t3=t3>>4;
			t3=t3&0xf;
			t4=t&0xf;
			*(U8 *)(AD1+9+ps*i)=asicc(t1);	       //发送长度，以asicc码发送
			*(U8 *)(AD1+10+ps*i)=asicc(t2);
			*(U8 *)(AD1+11+ps*i)=asicc(t3);
			*(U8 *)(AD1+12+ps*i)=asicc(t4);
			
			k1=((length*2)&0xf0)>>4;               //发送的位长度做asicc处理，以显示asicc码，依次从高位到低位
			k1=k1&0xf;
			k2=(length*2)&0xf;
			*(U8 *)(AD1+13+ps*i)=asicc(k1);        //发送的位长度，以asicc码发送
			*(U8 *)(AD1+14+ps*i)=asicc(k2);
							
			KK[1]=plcadd;                          //LRC校验码，以16进制表示，对应上PLC站地址，功能码，起始地址，长度，位长度
			KK[2]=0x10;
			KK[3]=b1;
			KK[4]=b2;
			KK[5]=b3;
			KK[6]=b4;
			KK[7]=length*2;
							
			for(j=0;j<length;j++)                  //写入多个元件值                 
			{	
				k4=*(U8 *)(PE+9+i*30+j*2);         //从存数据的寄存器开始地址PE+9取数据，k3为高位，k4为低位
				k3=*(U8 *)(PE+9+i*30+j*2+1);
							
				k5=(k3&0xf0)>>4;                   //对取得的数据做asicc处理
				k6=k3&0xf;
				k7=(k4&0xf0)>>4;
				k8=k4&0xf;
				*(U8 *)(AD1+15+j*4+ps*i)=asicc(k5);//发送取得的数据，以显示asicc码，依次从高位到低位
				*(U8 *)(AD1+16+j*4+ps*i)=asicc(k6);
				*(U8 *)(AD1+17+j*4+ps*i)=asicc(k7);
				*(U8 *)(AD1+18+j*4+ps*i)=asicc(k8);			
					
				KK[8+j*2]=k3;
				KK[9+j*2]=k4;					
			}
			aakj=CalLRC(KK,length*2+8);	           //计算和校检，LRC校验，高位在前，低位在后
			a1=(aakj&0xf0)>>4;
			a1=a1&0xf;
			a2=aakj&0xf;
			*(U8 *)(AD1+19+(length-1)*4+ps*i)=asicc(a1);
			*(U8 *)(AD1+20+(length-1)*4+ps*i)=asicc(a2);
			*(U8 *)(AD1+21+(length-1)*4+ps*i)=0x0d;//台达结束字元0d，0a
			*(U8 *)(AD1+22+(length-1)*4+ps*i)=0x0a;
		
			ptcom->send_length[i]=19+length*4;	   //发送长度
			ptcom->send_staradd[i]=i*ps;		   //发送数据存储地址	
			ptcom->return_length[i]=17;			   //返回数据长度
			ptcom->return_start[i]=0;			   //返回数据有效开始
			ptcom->return_length_available[i]=0;   //返回有效数据长度						
		}
//-------------------------------------------------------------------------------------------------------------		
		add_1=0x9000;		
		length_2=datalength-length_1;
		
		if(length_2%15==0)                       	//台达最多能发15个D，数据长度刚好是16的倍数时
		{
			SendTimes_2=length_2/15;               	//发送的次数为datalength/15               
			LastTimeWord_2=15;                      //最后一次发送的长度为15个D
		}
		if(length_2%15!=0)                       	//数据长度不是15D的倍数时
		{
			SendTimes_2=length_2/15+1;             	//发送的次数datalength/15+1
			LastTimeWord_2=length_2%15;            	//最后一次发送的长度为除16的余数
		}	
		ps=79;                                     	//发15个D要发83个长度
		
		for (i=0;i<SendTimes_2;i++)
		{   
			if (i!=(SendTimes_2-1))                 //不是最后一次发送时
			{	
				length=15;                         	//发15个D
			}
			else                                   	//最后一次发送时
			{
				length=LastTimeWord_2;              //发剩余的长度             
			}
			
			b=add_1+i*15;                        	//起始地址

			b1=(b&0xff00)>>8;                      	//对地址做16进制处理，b1为高位，b2为低位
			b2=b&0xff;
			a1=b&0xf000;                           	//对地址做asicc处理，以显示asicc码，依次从高位到低位
			a1=a1>>12;
			a1=a1&0xf;
			a2=b&0xf00;
			a2=a2>>8;
			a2=a2&0xf;
			a3=b&0xf0;
			a3=a3>>4;
			a3=a3&0xf;
			a4=b&0xf;
			*(U8 *)(AD1+0+ps*i+SendTimes_1*79)=0x3a;//台达起始字元3a
			*(U8 *)(AD1+1+ps*i+SendTimes_1*79)=asicc(((plcadd&0xf0)>>4)&0xf);//PLC站地址做asicc处理并显示asicc码，高位在前
			*(U8 *)(AD1+2+ps*i+SendTimes_1*79)=asicc(plcadd&0xf);
			*(U8 *)(AD1+3+ps*i+SendTimes_1*79)=0x31;//功能码16，以asicc显示
			*(U8 *)(AD1+4+ps*i+SendTimes_1*79)=0x30;
			*(U8 *)(AD1+5+ps*i+SendTimes_1*79)=asicc(a1);//开始地址，以asicc显示，依次从高位到低位
			*(U8 *)(AD1+6+ps*i+SendTimes_1*79)=asicc(a2);
			*(U8 *)(AD1+7+ps*i+SendTimes_1*79)=asicc(a3);
			*(U8 *)(AD1+8+ps*i+SendTimes_1*79)=asicc(a4);
			
			
			t=length;                              
			b3=(t&0xff00)>>8;                      	//对长度做16进制处理，b3为高位，b4为低位
			b4=t&0xff;
			t1=t&0xf000;                           	//对长度做asicc处理，以显示asicc码，依次从高位到低位
			t1=t1>>12;
			t1=t1&0xf;
			t2=t&0xf00;
			t2=t2>>8;
			t2=t2&0xf;
			t3=t&0xf0;
			t3=t3>>4;
			t3=t3&0xf;
			t4=t&0xf;
			*(U8 *)(AD1+9+ps*i+SendTimes_1*79)=asicc(t1);//发送长度，以asicc码发送
			*(U8 *)(AD1+10+ps*i+SendTimes_1*79)=asicc(t2);
			*(U8 *)(AD1+11+ps*i+SendTimes_1*79)=asicc(t3);
			*(U8 *)(AD1+12+ps*i+SendTimes_1*79)=asicc(t4);
			
			k1=((length*2)&0xf0)>>4;               //发送的位长度做asicc处理，以显示asicc码，依次从高位到低位
			k1=k1&0xf;
			k2=(length*2)&0xf;
			*(U8 *)(AD1+13+ps*i+SendTimes_1*79)=asicc(k1);//发送的位长度，以asicc码发送
			*(U8 *)(AD1+14+ps*i+SendTimes_1*79)=asicc(k2);
							
			KK[1]=plcadd;                          //LRC校验码，以16进制表示，对应上PLC站地址，功能码，起始地址，长度，位长度
			KK[2]=0x10;
			KK[3]=b1;
			KK[4]=b2;
			KK[5]=b3;
			KK[6]=b4;
			KK[7]=length*2;
							
			for(j=0;j<length;j++)                  //写入多个元件值                 
			{	
				k4=*(U8 *)(PE+9+i*30+j*2+length_1*2);//从存数据的寄存器开始地址PE+9取数据，k3为高位，k4为低位
				k3=*(U8 *)(PE+9+i*30+j*2+1+length_1*2);
							
				k5=(k3&0xf0)>>4;                   //对取得的数据做asicc处理
				k6=k3&0xf;
				k7=(k4&0xf0)>>4;
				k8=k4&0xf;
				*(U8 *)(AD1+15+j*4+ps*i+SendTimes_1*79)=asicc(k5);//发送取得的数据，以显示asicc码，依次从高位到低位
				*(U8 *)(AD1+16+j*4+ps*i+SendTimes_1*79)=asicc(k6);
				*(U8 *)(AD1+17+j*4+ps*i+SendTimes_1*79)=asicc(k7);
				*(U8 *)(AD1+18+j*4+ps*i+SendTimes_1*79)=asicc(k8);			
					
				KK[8+j*2]=k3;
				KK[9+j*2]=k4;					
			}
			aakj=CalLRC(KK,length*2+8);	           //计算和校检，LRC校验，高位在前，低位在后
			a1=(aakj&0xf0)>>4;
			a1=a1&0xf;
			a2=aakj&0xf;
			*(U8 *)(AD1+19+(length-1)*4+ps*i+SendTimes_1*79)=asicc(a1);
			*(U8 *)(AD1+20+(length-1)*4+ps*i+SendTimes_1*79)=asicc(a2);
			*(U8 *)(AD1+21+(length-1)*4+ps*i+SendTimes_1*79)=0x0d;//台达结束字元0d，0a
			*(U8 *)(AD1+22+(length-1)*4+ps*i+SendTimes_1*79)=0x0a;
		
			ptcom->send_length[i+SendTimes_1]=19+length*4;//发送长度
			ptcom->send_staradd[i+SendTimes_1]=i*ps+SendTimes_1*79;//发送数据存储地址	
			ptcom->return_length[i+SendTimes_1]=17;	//返回数据长度
			ptcom->return_start[i+SendTimes_1]=0;	//返回数据有效开始
			ptcom->return_length_available[i+SendTimes_1]=0;//返回有效数据长度						
		}
		ptcom->send_times=SendTimes_1+SendTimes_2;	//发送次数
		ptcom->Current_Times=0;			
	}	
	ptcom->send_staradd[90]=0;			
}
/************************************************************************************************************************
数据返回正确后，处理数据子函数
*************************************************************************************************************************/
void compxy(void)				              		//处理成标准存储格式,重新排列
{
	int i;
	unsigned char a1,a2,a3,a4,a5,a6,a7,a8;
	int b,b1,b2,b3;
	
	if (ptcom->Current_Times==2)
	{
		ptcom->address=ptcom->send_staradd[98];		//开始地址
		ptcom->register_length=ptcom->send_staradd[99];		
	}	

	if(ptcom->Simens_Count == 100)
	{
		ptcom->address = ptcom->send_add[ptcom->Current_Times - 1];
		ptcom->register_length =ptcom->send_data_length[ptcom->Current_Times - 1];
	}
	
	if (ptcom->send_staradd[90]==2)	
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/8;i++)	//ASC玛返回，所以要转为16进制，2个asicc码换成1个16进制数
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*8);
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
			b=(a1<<4)+a2;
			b1=(a3<<4)+a4;
			b2=(a5<<4)+a6;
			b3=(a7<<4)+a8;
			*(U8 *)(COMad+i*4)=b2;					 		//重新存,从第0个开始存
			*(U8 *)(COMad+i*4+1)=b3;	
			*(U8 *)(COMad+i*4+2)=b;					 		
			*(U8 *)(COMad+i*4+3)=b1;	
		}
	}
	else
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/2;i++)	//ASC玛返回，所以要转为16进制，2个asicc码换成1个16进制数
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2);
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2+1);
			a1=bsicc(a1);
			a2=bsicc(a2);
			b=(a1<<4)+a2;
			*(U8 *)(COMad+i)=b;					 		//重新存,从第0个开始存
		}
	}		
	ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;	//长度减半	

}
/************************************************************************************************************************
返回数据校验函数
*************************************************************************************************************************/
void watchcom(void)									//检查数据校检
{
	unsigned int aakj=0;
	aakj=remark();
	if(aakj==1)										//校检玛正确
	{
		ptcom->IfResultCorrect=1;
		compxy();									//进入数据处理程序
	}
	else
	{
		ptcom->IfResultCorrect=0;
	}
}
/************************************************************************************************************************
asicc转换函数
*************************************************************************************************************************/
int asicc(int a)									//转为Asc码
{
	int bl;
	if(a<10)
		bl=a+0x30;
	if(a>9)
		bl=a-10+0x41;
	return bl;	
}
/************************************************************************************************************************
bsicc转换函数
*************************************************************************************************************************/
int bsicc(int a)									//Asc转为数字
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
/************************************************************************************************************************
校验返回数据是否和返回的检验码一致
*************************************************************************************************************************/
int remark()										//返回来的数据计算校检码是否正确
{
	unsigned int aakj1;
	unsigned int aakj2;
	unsigned int a1;
	unsigned int a2;
	unsigned int akj1,akj2;
	int i;	
	aakj2=(*(U8 *)(COMad+ptcom->return_length[ptcom->Current_Times-1]-3))&0xff;	//在发送完后Current_Times++，此时要--
	aakj1=(*(U8 *)(COMad+ptcom->return_length[ptcom->Current_Times-1]-4))&0xff; //减后面的结束字元取校验码
	for(i=1;i<=(ptcom->return_length[ptcom->Current_Times-1]-5)/2;i++)    //对收到的数据进行排列，减去5个不做校验的长度
	{
		a1=bsicc(*(U8 *)(COMad+(i-1)*2+1));
		a2=bsicc(*(U8 *)(COMad+(i-1)*2+2));
		a1=a1&0xf;
		a2=a2&0xf;
		KK[i]=(a1<<4)+a2;
	}
	
	akj1=CalLRC(KK,1+(ptcom->return_length[ptcom->Current_Times-1]-5)/2);//对收到的数据进行校验，并转化成asicc码

	akj2=(akj1&0xf0)>>4;
	akj2=akj2&0xf;
	akj1=akj1&0xf;
	akj1=asicc(akj1);
	akj2=asicc(akj2);

	if((akj1==aakj2)&&(akj2==aakj1))     			//比较校验码，相等完毕，不相等继续取数据进行校验
		return 1;
	else
		return 0;	
}
/************************************************************************************************************************
LRC校验函数
*************************************************************************************************************************/
U16 CalLRC(unsigned char *chData,U16 uNo)			//LRC校验，用16进制数校验，asicc码显示
{
	int i;
	U16 ab=0;
	for(i=1;i<uNo;i++)
	{
		ab=ab+chData[i];
	}
	ab=~(ab);
	ab=(ab&0xff)+1;
	return (ab);
}
