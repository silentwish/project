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
		case 'T':
		case 'C':						
			Read_Bool();  //进入是数字量读
			break;
		case 'D':
		case 't':
		case 'c':
		case 'R':
		case 'N':	
			Read_Analog();//进入是模拟量读
			break;			
		}
		break;
	case PLC_WRITE_DATA:				//进入驱动是写数据
		switch(ptcom->registerr)
		{
		case 'M':
		case 'Y':
		case 'T':
		case 'C':				
			Set_Reset();//进入是置位和复位 
			break;
		case 'D':
		case 't':
		case 'c':
		case 'R':
		case 'N':
			Write_Analog();//进入是写模拟量		
			break;			
		}
		break;	
	case PLC_WRITE_TIME:				//进入驱动是写时间到PLC
		switch(ptcom->registerr)
		{
		case 'D':
			
			Write_Time();		
			break;			
		}
		break;	
	case PLC_READ_TIME:				//进入驱动是读取时间到PLC
		switch(ptcom->registerr)
		{
		case 'D':		
			Read_Time();		
			break;			
		}
		break;
	case PLC_WRITE_RECIPE:				//进入驱动是写配方到PLC
		switch(*(U8 *)(PE+3))//配方寄存器名称
		{
		case 'D':		
			Write_Recipe();		
			break;
		case 'R':		
			R_Write_Recipe();		
			break;			
		}
		break;
	case PLC_READ_RECIPE:				//进入驱动是从PLC读取配方
		switch(*(U8 *)(PE+3))//配方寄存器名称
		{
		case 'D':		
			Read_Recipe();		
			break;	
		case 'R':		
			R_Read_Recipe();		
			break;		
		}
		break;							
	case PLC_CHECK_DATA:				//进入驱动是数据处理
		watchcom();
		break;				
	}	 
}



void Set_Reset()                    //置位和复位
{
	U16 aakj;
	int b;
	int a1,a2,a3,a4;
	int add;
	int plcadd;	

	b=ptcom->address;			  // 置位地址
	plcadd=ptcom->plc_address;	  //PLC站地址
	switch (ptcom->registerr)	  //根据寄存器类型获得偏移地址
	{
	case 'Y':
		add=0x0;
		break;
	case 'M':
		add=0x7d0;
		break;	
	case 'T':
		add=0x1f40;
		break;
	case 'C':
		add=0x23f0;
		break;					
	}
	b=b+add;					    //开始地址偏移
	a1=(b>>8)&0xff;	                //高位
	a2=b&0xff;	                    //低位

	
	*(U8 *)(AD1+0)=plcadd;          //PLC站地址
	*(U8 *)(AD1+1)=0x05;            //功能码0x05，强置置位和复位
	*(U8 *)(AD1+2)=a1;              //高位
	*(U8 *)(AD1+3)=a2;	            //低位

	if (ptcom->writeValue==1)	    //置位
	{
		*(U8 *)(AD1+4)=0xff;
		*(U8 *)(AD1+5)=0x00;		
	}
	if (ptcom->writeValue==0)	    //复位
	{
		*(U8 *)(AD1+4)=0x00;
		*(U8 *)(AD1+5)=0x00;
	}	
	aakj= CalcCrc((U8 *)AD1,6);         //校验
	a3=aakj/0x100;
	a4=aakj-a3*0x100;
	*(U8 *)(AD1+6)=a4&0xff;             //校验 低位
	*(U8 *)(AD1+7)=a3&0xff;             //校验 高位

	
	ptcom->send_length[0]=8;				//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址	
	ptcom->send_times=1;					//发送次数
		
	ptcom->return_length[0]=8;				//返回数据长度
	ptcom->return_start[0]=0;				//返回数据有效开始
	ptcom->return_length_available[0]=0;	//返回有效数据长度	
	ptcom->Current_Times=0;					//当前发送次数			
}


void Read_Bool()				//读取数字量的信息
{
	U16 aakj;
	int b;
	int a1,a2,a3,a4;
	int add;
	int b1,b2;
	int plcadd;
	int length;
		
	switch (ptcom->registerr)	//根据寄存器类型获得偏移地址
	{
	case 'X':
		add=0x04b0;
		break;
	case 'Y':
		add=0x0;
		break;		
	case 'M':
		add=0x7d0;
		break;	
	case 'T':
		add=0x1f40;
		break;
	case 'C':
		add=0x23f0;
		break;					
	}		
				
	b=ptcom->address;			//在主程序已经转换到该段的开始地址
	plcadd=ptcom->plc_address;	//PLC站地址
	length=ptcom->register_length;
	length=length*8;            //一个元件包涵8位数据
	
	b=b+add;					//加上偏移地址
	a1=(b>>8)&0xff;
	a2=b&0xff;
	
	a3=(length>>8)&0xff;
	a4=length&0xff;
	
	*(U8 *)(AD1+0)=plcadd;
	*(U8 *)(AD1+1)=0x01;
	*(U8 *)(AD1+2)=a1;	        //起始地址  高位
	*(U8 *)(AD1+3)=a2;          //起始地址  低位
	*(U8 *)(AD1+4)=a3;          //元件个数  高位
	*(U8 *)(AD1+5)=a4;          //元件个数  低位
	aakj= CalcCrc((U8 *)AD1,6); //校验
	b1=aakj/0x100;
	b2=aakj-b1*0x100;
	*(U8 *)(AD1+6)=b2&0xff;     //校验 低位
	*(U8 *)(AD1+7)=b1&0xff;     //校验 高位
	
	ptcom->send_length[0]=8;				     //发送长度
	ptcom->send_staradd[0]=0;				     //发送数据存储地址	
	ptcom->send_times=1;					     //发送次数
		
	ptcom->return_length[0]=5+length/8;		     //返回数据长度，有5个固定,校检
	ptcom->return_start[0]=3;				     //返回数据有效开始
	ptcom->return_length_available[0]=length/8;    //返回有效数据长度	
	ptcom->Current_Times=0;					     //当前发送次数	
	ptcom->send_add[0]=ptcom->address;		     //读的是这个地址的数据		
	ptcom->needShake=0;
}



void Read_Analog()				//读模拟量
{
	U16 aakj;
	int b;
	int a1,a2,a3,a4;
	int add;
	int b1,b2;
	int plcadd;
	int length;
		
	switch (ptcom->registerr)	//根据寄存器类型获得偏移地址
	{
	case 'D':
		add=0x0;
		break;
	case 't':
		add=0x2328;
		break;		
	case 'c':
		add=0x251c;
		break;
	case 'R':
		add=0x0;
		break;		
	case 'N':
		add=0x251c;
		break;					
	}		
				
	b=ptcom->address;			      //在主程序已经转换到该段的开始地址
	plcadd=ptcom->plc_address;	      //PLC站地址
	length=ptcom->register_length;
	
	b=b+add;					      //加上偏移地址
	a1=(b>>8)&0xff;
	a2=b&0xff;
	
	a3=(length>>8)&0xff;              //元件个数处理
	a4=length&0xff;
	
	*(U8 *)(AD1+0)=plcadd;
	*(U8 *)(AD1+1)=0x03;
	*(U8 *)(AD1+2)=a1;	             //起始地址 高位
	*(U8 *)(AD1+3)=a2;               //起始地址 低位
	*(U8 *)(AD1+4)=a3;               //元件个数 高位
	*(U8 *)(AD1+5)=a4;               //元件个数 低位
	aakj= CalcCrc((U8 *)AD1,6);      //校验
	b1=aakj/0x100;
	b2=aakj-b1*0x100;
	*(U8 *)(AD1+6)=b2&0xff;          //校验 低位
	*(U8 *)(AD1+7)=b1&0xff;          //校验 高位
	
	ptcom->send_length[0]=8;				//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址	
	ptcom->send_times=1;					//发送次数
		
	ptcom->return_length[0]=5+length*2;		//返回数据长度，有5个固定,校检
	ptcom->return_start[0]=3;				//返回数据有效开始
	ptcom->return_length_available[0]=length*2;//返回有效数据长度	
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->send_add[0]=ptcom->address;		//读的是这个地址的数据
	
	if(length==1)
	{
		ptcom->needShake=0;	
	}
	else
	{
		switch (ptcom->registerr)	//根据寄存器类型获得偏移地址
		{
		case 'R':
		case 'N':	
			ptcom->needShake=1;
			break;
		case 'D':
		case 't':
		case 'c':
			ptcom->needShake=0;	
			break;
		}	
	}	

}


void Read_Recipe()								//读取配方
{
	U16 aakj;
	int b;
	int a1,a2;
	int i;
	int datalength;
	int p_start;
	int ps;
	int SendTimes;
	int LastTimeWord;							//最后一次发送长度
	int currentlength;
	int plcadd;
	int b1,b2;
	
		
	datalength=ptcom->register_length;		//发送总长度
	p_start=ptcom->address;					//开始地址
	plcadd=ptcom->plc_address;
	
	if(datalength>5000)
		datalength=5000;                //每次最多能发送32个D
		
	if(datalength%32==0)
	{
		SendTimes=datalength/32;
		LastTimeWord=32;                //固定长度32	
	}
	if(datalength%32!=0)
	{
		SendTimes=datalength/32+1;      //发送的次数
		LastTimeWord=datalength%32;     //最后一次发送的长度	
	}
	
	for (i=0;i<SendTimes;i++)
	{
		ps=8*i;                         //每次发8个长度
		b=(p_start+i*32);				//加上偏移地址
		
		a1=(b>>8)&0xff;
		a2=b&0xff;
			
		*(U8 *)(AD1+0+ps)=plcadd;      //PLC站地址
		*(U8 *)(AD1+1+ps)=0x03;        //功能码
		*(U8 *)(AD1+2+ps)=a1;          //起始地址高位
		*(U8 *)(AD1+3+ps)=a2;          //起始地址低位
	
		if (i!=(SendTimes-1))	//不是最后一次时
		{
			*(U8 *)(AD1+4+ps)=0x0;   //固定长度32个，即64字节 高位
			*(U8 *)(AD1+5+ps)=0x20;        //固定长度32个，即64字节 低位
			currentlength=32;
		}
		if (i==(SendTimes-1))	//最后一次时
		{
			*(U8 *)(AD1+4+ps)=(LastTimeWord>>8)&0xff;   //剩余长度LastTimeWord个， 高位
			*(U8 *)(AD1+5+ps)=LastTimeWord&0xff;        //剩余长度LastTimeWord个， 低位
			currentlength=LastTimeWord;
		}
		aakj= CalcCrc((U8 *)(AD1+ps),6);      //校验
		b1=aakj/0x100;
		b2=aakj-b1*0x100;
		*(U8 *)(AD1+6+ps)=b2&0xff;          //校验 低位
		*(U8 *)(AD1+7+ps)=b1&0xff;          //校验 高位			
			
		
		ptcom->send_length[i]=8;				    //发送长度
		ptcom->send_staradd[i]=i*8;			        //发送数据存储地址	
		ptcom->send_add[i]=p_start+i*32;		    //读的是这个地址的数据	
		ptcom->send_data_length[i]=currentlength;	//不是最后一次都是32个D
				
		ptcom->return_length[i]=5+currentlength*2;	//返回数据长度，有5个固定，plcadd，03，字节数，校检
		ptcom->return_start[i]=3;				    //返回数据有效开始
		ptcom->return_length_available[i]=currentlength*2;	//返回有效数据长度		
	}
	ptcom->send_times=SendTimes;					//发送次数
	ptcom->Current_Times=0;					        //当前发送次数
	ptcom->needShake=0;	
}


void R_Read_Recipe()								//读取配方
{
	U16 aakj;
	int b;
	int a1,a2;
	int i;
	int datalength;
	int p_start;
	int ps;
	int SendTimes;
	int LastTimeWord;							//最后一次发送长度
	int currentlength;
	int plcadd;
	int b1,b2;
	
		
	datalength=ptcom->register_length;		//发送总长度
	p_start=ptcom->address;					//开始地址
	plcadd=ptcom->plc_address;
	
	if(datalength>5000)
		datalength=5000;                //每次最多能发送16个D
	if(datalength%16==0)
	{
		SendTimes=datalength/16;
		LastTimeWord=16;                //固定长度16	
	}
	if(datalength%16!=0)
	{
		SendTimes=datalength/16+1;      //发送的次数
		LastTimeWord=datalength%16;     //最后一次发送的长度	
	}
	
	for (i=0;i<SendTimes;i++)
	{
		ps=8*i;                         //每次发8个长度
		b=(p_start+i*16);				//加上偏移地址
		
		a1=(b>>8)&0xff;
		a2=b&0xff;
			
		*(U8 *)(AD1+0+ps)=plcadd;      //PLC站地址
		*(U8 *)(AD1+1+ps)=0x03;        //功能码
		*(U8 *)(AD1+2+ps)=a1;          //起始地址高位
		*(U8 *)(AD1+3+ps)=a2;          //起始地址低位
	
		if (i!=(SendTimes-1))	//不是最后一次时
		{
			*(U8 *)(AD1+4+ps)=0x0;   //固定长度16个，即16字节 高位
			*(U8 *)(AD1+5+ps)=0x10;        //固定长度32个，即64字节 低位
			currentlength=16;
		}
		if (i==(SendTimes-1))	//最后一次时
		{
			*(U8 *)(AD1+4+ps)=(LastTimeWord>>8)&0xff;   //剩余长度LastTimeWord个， 高位
			*(U8 *)(AD1+5+ps)=LastTimeWord&0xff;        //剩余长度LastTimeWord个， 低位
			currentlength=LastTimeWord;
		}
		aakj= CalcCrc((U8 *)(AD1+ps),6);      //校验
		b1=aakj/0x100;
		b2=aakj-b1*0x100;
		*(U8 *)(AD1+6+ps)=b2&0xff;          //校验 低位
		*(U8 *)(AD1+7+ps)=b1&0xff;          //校验 高位			
			
		
		ptcom->send_length[i]=8;				    //发送长度
		ptcom->send_staradd[i]=i*8;			        //发送数据存储地址	
		ptcom->send_add[i]=p_start+i*16;		    //读的是这个地址的数据	
		ptcom->send_data_length[i]=currentlength;	//不是最后一次都是32个D
				
		ptcom->return_length[i]=5+currentlength*2;	//返回数据长度，有5个固定，plcadd，03，字节数，校检
		ptcom->return_start[i]=3;				    //返回数据有效开始
		ptcom->return_length_available[i]=currentlength*2;	//返回有效数据长度		
	}
	ptcom->send_times=SendTimes;					//发送次数
	ptcom->Current_Times=0;					        //当前发送次数
	ptcom->needShake=1;		
}



void Write_Analog()				//写模拟量
{
	U16 aakj;
	int b;
	int a1,a2,a3,a4,a5,a6;
	int add;
	int b1,b2;
	int i;
	int length;
	int plcadd;
	
	switch (ptcom->registerr)	//根据寄存器类型获得偏移地址
	{
	case 'D':
		add=0x0;
		break;
	case 't':
		add=0x2328;
		break;		
	case 'c':
		add=0x251c;
		break;
	case 'R':
		add=0x0;
		break;		
	case 'N':
		add=0x251c;
		break;							
	}
	
	
	b=ptcom->address;			//开始地址
	plcadd=ptcom->plc_address;	//PLC站地址
	length=ptcom->register_length;
			
	b=b+add;					//加上偏移地址
	a1=(b>>8)&0xff;	            
	a2=b&0xff;
			
	if(length==1)
	{	
		*(U8 *)(AD1+0)=plcadd;        //PLC地址
		*(U8 *)(AD1+1)=0x06;          //功能码 写单个寄存器
		*(U8 *)(AD1+2)=a1;            //起始地址 高位
		*(U8 *)(AD1+3)=a2;            //起始地址 低位
		a3=ptcom->U8_Data[0];
		a4=ptcom->U8_Data[1];
		*(U8 *)(AD1+4)=a4;            //写入元件值高位
		*(U8 *)(AD1+5)=a3;            //写入元件值高位
		aakj= CalcCrc((U8 *)AD1,6);   //校验
		b1=aakj/0x100;
		b2=aakj-b1*0x100;
		*(U8 *)(AD1+6)=b2&0xff;       //校验 低位
		*(U8 *)(AD1+7)=b1&0xff;	      //校验 高位
			
		ptcom->send_length[0]=8;			    //发送长度
		ptcom->send_staradd[0]=0;				//发送数据存储地址	
		ptcom->send_times=1;					//发送次数
				
		ptcom->return_length[0]=8;				//返回数据长度
		ptcom->return_start[0]=0;				//返回数据有效开始
		ptcom->return_length_available[0]=0;	//返回有效数据长度	
		ptcom->Current_Times=0;					//当前发送次数			
	}
		
	if(length!=1)
	{
		*(U8 *)(AD1+0)=plcadd;        //PLC地址
		*(U8 *)(AD1+1)=0x10;          //功能码 写多个寄存器
		*(U8 *)(AD1+2)=a1;            //起始地址 高位
		*(U8 *)(AD1+3)=a2;            //起始地址 低位
			
		switch (ptcom->registerr)	//根据寄存器类型获得偏移地址
		{
			case 'D':
			case 't':
			case 'c':
				a3=(length>>8)&0xff;
				a4=length&0xff;
				
				*(U8 *)(AD1+4)=a3;            //元件个数高位
				*(U8 *)(AD1+5)=a4;            //元件个数低位
				*(U8 *)(AD1+6)=length*2;
			
				for (i=0;i<length;i++)              //写入多个元件值
				{				
					a3=ptcom->U8_Data[i*2];
					a4=ptcom->U8_Data[i*2+1];
					*(U8 *)(AD1+7+i*2)=a4;          //写入元件值高位
					*(U8 *)(AD1+8+i*2)=a3;          //写入元件值低位		
				}
				aakj=CalcCrc((U8 *)AD1,7+length*2); //校验
				b1=aakj/0x100;
				b2=aakj-b1*0x100;
				*(U8 *)(AD1+7+length*2)=b2&0xff;    //校验 低位
				*(U8 *)(AD1+8+length*2)=b1&0xff;    //校验 高位
				
				ptcom->send_length[0]=9+length*2;		//发送长度
				ptcom->send_staradd[0]=0;				//发送数据存储地址	
				ptcom->send_times=1;					//发送次数
					
				ptcom->return_length[0]=8;				//返回数据长度
				ptcom->return_start[0]=0;				//返回数据有效开始
				ptcom->return_length_available[0]=0;	//返回有效数据长度	
				ptcom->Current_Times=0;					//当前发送次数
				break;
				
			case 'R':
			case 'N':
				a3=(length>>8)&0xff;
				a4=length&0xff;
				
				*(U8 *)(AD1+4)=a3;            //元件个数高位
				*(U8 *)(AD1+5)=a4;            //元件个数低位
				*(U8 *)(AD1+6)=length*2;
				
				for(i=0;i<length;i=i+2)
				{
					a3=ptcom->U8_Data[0+i*2];
					a4=ptcom->U8_Data[1+i*2];
					a5=ptcom->U8_Data[2+i*2];
					a6=ptcom->U8_Data[3+i*2];
				
					*(U8 *)(AD1+7+i*2)=a6;          //写入元件值高位
					*(U8 *)(AD1+8+i*2)=a5;          //写入元件值低位
				
					*(U8 *)(AD1+9+i*2)=a4;          //写入元件值高位
					*(U8 *)(AD1+10+i*2)=a3;          //写入元件值低位
				}
			
				aakj=CalcCrc((U8 *)AD1,7+length*2); //校验
				b1=aakj/0x100;
				b2=aakj-b1*0x100;
				*(U8 *)(AD1+7+length*2)=b2&0xff;    //校验 低位
				*(U8 *)(AD1+8+length*2)=b1&0xff;    //校验 高位
				
				ptcom->send_length[0]=9+length*2;		//发送长度
				ptcom->send_staradd[0]=0;				//发送数据存储地址	
				ptcom->send_times=1;					//发送次数
					
				ptcom->return_length[0]=8;				//返回数据长度
				ptcom->return_start[0]=0;				//返回数据有效开始
				ptcom->return_length_available[0]=0;	//返回有效数据长度	
				ptcom->Current_Times=0;					//当前发送次数	
				
				ptcom->needShake=1;
				break;	
		}	
	}
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
	int i,j;
	int ps;
	int b;
	int a1,a2;
	U16 aakj;
	int plcadd;
	int length;
	
	
	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));  //数据长度
	staradd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//开始地址 	
	plcadd=*(U8 *)(PE+4);	   //PLC站地址		  

	if(datalength%32==0)           //每次最多能发送32个D，如果是长度是32个D的倍数
	{
		SendTimes=datalength/32;   //发送的次数
		LastTimeWord=32;           //最后一次发送的长度	
	}
	if(datalength%32!=0)           //如果不是长度是32个D的倍数
	{
		SendTimes=datalength/32+1; //发送的次数
		LastTimeWord=datalength%32;//最后一次发送的长度	
	}	
	
	ps=73;                         //每次发73个长度
	for (i=0;i<SendTimes;i++)
	{
		b=staradd+i*32;           //起始地址
		a1=(b>>8)&0xff;
		a2=b&0xff;
		if (i!=(SendTimes-1))     //不是第一次发送
		{	
			length=32;
		}
		else
		{
			length=LastTimeWord;
		}
		*(U8 *)(AD1+0+ps*i)=plcadd;
		*(U8 *)(AD1+1+ps*i)=0x10;
		*(U8 *)(AD1+2+ps*i)=a1;
		*(U8 *)(AD1+3+ps*i)=a2;
		*(U8 *)(AD1+4+ps*i)=(length>>8)&0xff;
		*(U8 *)(AD1+5+ps*i)=(length)&0xff;
		*(U8 *)(AD1+6+ps*i)=length*2;

		for(j=0;j<length;j++)
		{
			*(U8 *)(AD1+7+ps*i+j*2)=*(U8 *)(PE+9+i*64+j*2+1);	  //取数据
			*(U8 *)(AD1+7+ps*i+j*2+1)=*(U8 *)(PE+9+i*64+j*2);			
		}
		aakj=CalcCrc((U8 *)AD1+ps*i,7+length*2);
		a1=aakj/0x100;
		a2=aakj-a1*0x100;
		*(U8 *)(AD1+7+ps*i+length*2)=a2;
		*(U8 *)(AD1+8+ps*i+length*2)=a1;
		
		ptcom->send_length[i]=9+length*2;		//发送长度
		ptcom->send_staradd[i]=i*ps;			//发送数据存储地址
			
		ptcom->return_length[i]=8;				//返回数据长度
		ptcom->return_start[i]=0;				//返回数据有效开始
		ptcom->return_length_available[i]=0;	//返回有效数据长度				
	}
	ptcom->send_times=SendTimes;				//发送次数
	ptcom->Current_Times=0;					    //当前发送次数		
}


void R_Write_Recipe()								//写配方到PLC
{
	int datalength;
	int staradd;
	int SendTimes;
	int LastTimeWord;							//最后一次发送长度
	int i,j;
	int ps;
	int b;
	int a1,a2;
	U16 aakj;
	int plcadd;
	int length;
	
	
	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));  //数据长度
	staradd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//开始地址 	
	plcadd=*(U8 *)(PE+4);	   //PLC站地址		  

	if(datalength%32==0)           //每次最多能发送32个D，如果是长度是32个D的倍数
	{
		SendTimes=datalength/32;   //发送的次数
		LastTimeWord=32;           //最后一次发送的长度	
	}
	if(datalength%32!=0)           //如果不是长度是32个D的倍数
	{
		SendTimes=datalength/32+1; //发送的次数
		LastTimeWord=datalength%32;//最后一次发送的长度	
	}	
	
	ps=73;                         //每次发73个长度
	for (i=0;i<SendTimes;i++)
	{
		b=staradd+i*32;           //起始地址
		a1=(b>>8)&0xff;
		a2=b&0xff;
		if (i!=(SendTimes-1))     //不是第一次发送
		{	
			length=32;
		}
		else
		{
			length=LastTimeWord;
		}
		*(U8 *)(AD1+0+ps*i)=plcadd;
		*(U8 *)(AD1+1+ps*i)=0x10;
		*(U8 *)(AD1+2+ps*i)=a1;
		*(U8 *)(AD1+3+ps*i)=a2;
		*(U8 *)(AD1+4+ps*i)=(length>>8)&0xff;
		*(U8 *)(AD1+5+ps*i)=(length)&0xff;
		*(U8 *)(AD1+6+ps*i)=length*2;

		for(j=0;j<length;j=j+2)
		{
			*(U8 *)(AD1+7+ps*i+j*2+0)=*(U8 *)(PE+9+i*64+j*2+3);	  //取数据
			*(U8 *)(AD1+7+ps*i+j*2+1)=*(U8 *)(PE+9+i*64+j*2+2);
			*(U8 *)(AD1+7+ps*i+j*2+2)=*(U8 *)(PE+9+i*64+j*2+1);	  //取数据
			*(U8 *)(AD1+7+ps*i+j*2+3)=*(U8 *)(PE+9+i*64+j*2+0);					
		}
		aakj=CalcCrc((U8 *)AD1+ps*i,7+length*2);
		a1=aakj/0x100;
		a2=aakj-a1*0x100;
		*(U8 *)(AD1+7+ps*i+length*2)=a2;
		*(U8 *)(AD1+8+ps*i+length*2)=a1;
		
		ptcom->send_length[i]=9+length*2;		//发送长度
		ptcom->send_staradd[i]=i*ps;			//发送数据存储地址
			
		ptcom->return_length[i]=8;				//返回数据长度
		ptcom->return_start[i]=0;				//返回数据有效开始
		ptcom->return_length_available[i]=0;	//返回有效数据长度				
	}
	ptcom->send_times=SendTimes;				//发送次数
	ptcom->Current_Times=0;					    //当前发送次数
	ptcom->needShake=1;		
}




void compxy(void)				//处理成标准存储格式
{
	int i;
	unsigned char a1,a2,a3,a4;
	if(ptcom->needShake==1)
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1];i=i+2)
		{	
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2);
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2+1);
			a3=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2+2);
			a4=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2+3);
			*(U8 *)(COMad+i*2+0)=a3;							//重新存,从第0个开始存
			*(U8 *)(COMad+i*2+1)=a4;
			*(U8 *)(COMad+i*2+2)=a1;							//重新存,从第0个开始存
			*(U8 *)(COMad+i*2+3)=a2;
		}	
	}
	else
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1];i++)						//ASC玛返回，所以要转为16进制
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i);
			*(U8 *)(COMad+i)=a1;							//重新存,从第0个开始存
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


int remark()				//返回来的数据计算校检码是否正确
{
	unsigned int aakj1;
	unsigned int aakj2;
	unsigned int akj1;
	unsigned int akj2;
	unsigned int aakj;	
	aakj2=(*(U8 *)(COMad+ptcom->return_length[ptcom->Current_Times-1]-1))&0xff;		//在发送完后Current_Times++，此时要--
	aakj1=(*(U8 *)(COMad+ptcom->return_length[ptcom->Current_Times-1]-2))&0xff;
	aakj=CalcCrc((U8 *)COMad,ptcom->return_length[ptcom->Current_Times-1]-2);
	akj1=aakj/0x100;
	akj2=aakj-akj1*0x100;
	if((akj1==aakj2)&&(akj2==aakj1))
		return 1;
	else
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
