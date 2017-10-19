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
		case 'X':  //X
		case 'Y':  //Y
		case 'M':  //M
		case 'K':  //SM
		case 'T':  //T
		case 'C':  //C
		case 'H':	//S	
			Read_Bool();
			break;
		case 'D':  //D
		case 'N':  //SD
		case 'R':	//R	
		case 't':  //TV
		case 'c':  //CV
		case 'x':  //CV32(200-255)
		case 'L':	//D_Bit
			Read_Analog();
			break;		
		}
		break;
	case PLC_WRITE_DATA:				//进入驱动是写数据
		switch(ptcom->registerr)
		{
		case 'M':
		case 'K':  //SM
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
		case 'N':  //SD
		case 'x':  //CV32(200-255)
			Write_Analog();	
			break;
		case 'L':	//D_Bit
			Write_Wordbit();
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
	case PLC_CHECK_DATA:						//进入驱动是数据处理
		watchcom();
		break;
	default:					//纠错处理
		ptcom->R_W_Flag=0;		//强制读
		ptcom->registerr='M';	//强制读M
		ptcom->address=0;
		ptcom->register_length=1;
		ptcom->Circle_Control_ID=0;
		Read_Bool();
		break;									
	}	 
}

void Set_Reset()
{
	U16 aakj;
	int b;
	int a1,a2,a3,a4;
	int add;	
	int i;

	b=ptcom->address;			// 置位地址
	switch (ptcom->registerr)	//根据寄存器类型获得偏移地址
	{
	case 'Y':
		add=0x0c00;
		break;
	case 'M':
	case 'K':   //SM
		add=0x0000;
		break;	
	case 'H':					//S
		add=0x1400;
		break;			
	case 'T':
		add=0x1000;
		break;
	case 'C':
		add=0x0f00;
		break;		
			
	}
	if(ptcom->registerr == 'K')
	{
		b = b+8000;
	}
	//sysprintf("Set_Reset b=%d 0x%x\n",b,b);
	b=b+add;					//开始地址偏移
	a1=b&0xf000;
	a1=a1>>12;
	a1=a1&0xf;
	a2=b&0xf00;
	a2=a2>>8;
	a2=a2&0xf;
	a3=b&0xf0;
	a3=a3>>4;
	a3=a3&0xf;
	a4=b&0xf;
	
	*(U8 *)(AD1+0)=0x02;
	*(U8 *)(AD1+1)=0x45;	
	if (ptcom->writeValue==1)	//置位
	{
		*(U8 *)(AD1+2)=0x37;
	}
	if (ptcom->writeValue==0)	//复位
	{
		*(U8 *)(AD1+2)=0x38;
	}	
	*(U8 *)(AD1+3)=asicc(a3);
	*(U8 *)(AD1+4)=asicc(a4);
	*(U8 *)(AD1+5)=asicc(a1);
	*(U8 *)(AD1+6)=asicc(a2);
	*(U8 *)(AD1+7)=0x03;
	aakj=CalcHe((U8 *)AD1,8);
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+8)=asicc(a1);
	*(U8 *)(AD1+9)=asicc(a2);
	
	ptcom->send_length[0]=10;				//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址	
	ptcom->send_times=1;					//发送次数
		
	ptcom->return_length[0]=1;				//返回数据长度
	ptcom->return_start[0]=0;				//返回数据有效开始
	ptcom->return_length_available[0]=0;	//返回有效数据长度	
	ptcom->Current_Times=0;					//当前发送次数			
	ptcom->send_staradd[99]=0;	
}


void Read_Bool()				//读取数字量的信息
{
	U16 aakj;
	int b;
	int a1,a2,a3,a4;
	int add;
	int b1,b2;
		
	switch (ptcom->registerr)	//根据寄存器类型获得偏移地址
	{
	case 'X':
		add=0x240;
		break;
	case 'Y':
		add=0x180;
		break;		
	case 'M':
	case 'K':   //SM	
		add=0x8800;
		break;	
	case 'H':	//S
		add=0x8ce0;	
		break;			
	case 'T':
		add=0x0200;
		break;
	case 'C':
		add=0x01e0;
		break;					
	}		
				
	b=ptcom->address;			//在主程序已经转换到该段的开始地址
	if(ptcom->registerr == 'K')
	{
		b = b+8000;
	}
	b=b/8;						//每8个位占用一个地址
	b=b+add;					//加上偏移地址
	a1=b&0xf000;
	a1=a1>>12;
	a1=a1&0xf;
	a2=b&0xf00;
	a2=a2>>8;
	a2=a2&0xf;
	a3=b&0xf0;
	a3=a3>>4;
	a3=a3&0xf;
	a4=b&0xf;
	*(U8 *)(AD1+0)=0x02;
	*(U8 *)(AD1+1)=0x45;	
	*(U8 *)(AD1+2)=0x30;
	*(U8 *)(AD1+3)=0x30;	
	*(U8 *)(AD1+4)=asicc(a1);	//起始地址
	*(U8 *)(AD1+5)=asicc(a2);
	*(U8 *)(AD1+6)=asicc(a3);
	*(U8 *)(AD1+7)=asicc(a4);
	b1=ptcom->register_length>>4;
	b2=ptcom->register_length&0x0f;
	*(U8 *)(AD1+8)=asicc(b1);	//发送长度
	*(U8 *)(AD1+9)=asicc(b2);
	*(U8 *)(AD1+10)=0x03;
	aakj=CalcHe((U8 *)AD1,11);
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+11)=asicc(a1);
	*(U8 *)(AD1+12)=asicc(a2);
	
	ptcom->send_length[0]=13;				//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址	
	ptcom->send_times=1;					//发送次数
		
	ptcom->return_length[0]=4+ptcom->register_length*2;				//返回数据长度，有4个固定，02，03，校检
	ptcom->return_start[0]=1;				//返回数据有效开始
	ptcom->return_length_available[0]=ptcom->register_length*2;	//返回有效数据长度	
	ptcom->Current_Times=0;				//当前发送次数	
	ptcom->send_add[0]=ptcom->address;		//读的是这个地址的数据		
	
	ptcom->send_staradd[99]=0;	
}



void Read_Analog()				//读模拟量
{
	U16 aakj;
	int b;
	int a1,a2,a3,a4;
	int add;
	int b1,b2;
	int iSendLen;
	int i;
	
	switch (ptcom->registerr)	//根据寄存器类型获得偏移地址
	{
	case 'D':					//数据寄存器D 
	case 'N': 
	case 'L':					//D_Bit
		add=0x4000;
		break;
	case 'R':                    //文件寄存器R
		add=0x0;				
		break;		
	case 't':                    //T
		add=0x1000;
		break;		
	case 'c':                    //C
		add=0x0A00;
		break;	
	case 'x':                   //CV32(200-255)
	    add=0x0c00;
	    break;					
	}		
				
	b=ptcom->address;			    //开始地址
	
	if(ptcom->registerr == 'L')
	{
		b = b/16;
	}
	if(ptcom->registerr == 'N')
	{
		b = b+8000;
	}
/*
	if(ptcom->registerr=='v')
	{
		  b=b*4+add;        //加上偏移地址
    }
	else  
*/
     b=b*2+add;                       //加上偏移地址

	a1=b&0xf000;
	a1=a1>>12;
	a1=a1&0xf;
	a2=b&0xf00;
	a2=a2>>8;
	a2=a2&0xf;
	a3=b&0xf0;
	a3=a3>>4;
	a3=a3&0xf;
	a4=b&0xf;
	*(U8 *)(AD1+0)=0x02;
	*(U8 *)(AD1+1)=0x45;
	if(ptcom->registerr=='R')
	{
		*(U8 *)(AD1+2)=0x30;
		*(U8 *)(AD1+3)=0x36;		//R标识	
	}
	else							//一般标识
	{
		*(U8 *)(AD1+2)=0x30;
		*(U8 *)(AD1+3)=0x30;
	}	
	*(U8 *)(AD1+4)=asicc(a1);	//起始地址
	*(U8 *)(AD1+5)=asicc(a2);
	*(U8 *)(AD1+6)=asicc(a3);
	*(U8 *)(AD1+7)=asicc(a4);
	
    iSendLen = ptcom->register_length*2;  //一个寄存器需要访问2个字节  最总返回4个半字  

	b1=(iSendLen>>4)&0x0f;		
	b2=iSendLen&0x0f;

	*(U8 *)(AD1+8)=asicc(b1);	//发送长度
	*(U8 *)(AD1+9)=asicc(b2);
	
	*(U8 *)(AD1+10)=0x03;
	aakj=CalcHe((U8 *)AD1,11);	//计算和校检
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+11)=asicc(a1);
	*(U8 *)(AD1+12)=asicc(a2);
	ptcom->send_length[0]=13;				//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址	
	ptcom->send_times=1;					//发送次数
    if(ptcom->registerr=='v')
    {
        ptcom->return_length_available[0]=ptcom->register_length*8; //返回有效数据长度  
        ptcom->return_length[0]=4+ptcom->register_length*8;         //返回数据长度，有4个固定，02，03，校检
    }
    else
    {
        ptcom->return_length_available[0]=ptcom->register_length*4; //返回有效数据长度  
        ptcom->return_length[0]=4+ptcom->register_length*4;         //返回数据长度，有4个固定，02，03，校检
    }
	ptcom->return_start[0]=1;				//返回数据有效开始
	ptcom->Current_Times=0;				//当前发送次数	
	ptcom->send_add[0]=ptcom->address;		//读的是这个地址的数据
	ptcom->send_staradd[99]=1;	
	if(ptcom->registerr == 'x')
	{
		ptcom->send_staradd[99]=2;
	}		
}


void Read_Recipe()								//读取配方
{
	U16 aakj;
	int b;
	int a1,a2,a3,a4;
	int i;
	int datalength;
	int p_start;
	int ps;
	int SendTimes;
	int LastTimeWord;							//最后一次发送长度
	int currentlength,Registerr;
	int staradd,nda1,nda2;
	
		
	datalength=ptcom->register_length;		//发送总长度
	p_start=ptcom->address;					//开始地址
	Registerr=ptcom->registerr;					//读取寄存器
	
	if (Registerr=='R')						//文件寄存器
	{
		staradd=0;
		nda1=0x30;
		nda2=0x36;
	}	
	else										//数据寄存器 D
	{
		staradd=0x4000;
		nda1=0x30;
		nda2=0x30;		
	}
		
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
	

	for (i=0;i<SendTimes;i++)
	{
		ps=13*i;
		b=(p_start+i*32)*2+staradd;				//加上偏移地址
		a1=b&0xf000;
		a1=a1>>12;
		a1=a1&0xf;
		a2=b&0xf00;
		a2=a2>>8;
		a2=a2&0xf;
		a3=b&0xf0;
		a3=a3>>4;
		a3=a3&0xf;
		a4=b&0xf;
		
			
		*(U8 *)(AD1+0+ps)=0x02;
		*(U8 *)(AD1+1+ps)=0x45;
		*(U8 *)(AD1+2+ps)=nda1;
		*(U8 *)(AD1+3+ps)=nda2;
		*(U8 *)(AD1+4+ps)=asicc(a1);
		*(U8 *)(AD1+5+ps)=asicc(a2);
		*(U8 *)(AD1+6+ps)=asicc(a3);
		*(U8 *)(AD1+7+ps)=asicc(a4);
	
		if (i!=(SendTimes-1))	//不是最后一次时
		{
			*(U8 *)(AD1+8+ps)=0x34;//固定长度32个，即64字节
			*(U8 *)(AD1+9+ps)=0x30;
			currentlength=32;
		}
		if (i==(SendTimes-1))	//最后一次时
		{
			*(U8 *)(AD1+8+ps)=asicc(((LastTimeWord*2)>>4)&0xf);//固定长度16个，即32字节
			*(U8 *)(AD1+9+ps)=asicc(((LastTimeWord*2))&0xf);
			currentlength=LastTimeWord;
		}	
			
		*(U8 *)(AD1+10+ps)=0x03;
		aakj=CalcHe((U8 *)AD1+ps,11);
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+11+ps)=asicc(a1);
		*(U8 *)(AD1+12+ps)=asicc(a2);
		
		ptcom->send_length[i]=13;				//发送长度
		ptcom->send_staradd[i]=i*13;			//发送数据存储地址	
		ptcom->send_add[i]=p_start+i*32;		//读的是这个地址的数据	
		ptcom->send_data_length[i]=currentlength;	//不是最后一次都是32个D
				
		ptcom->return_length[i]=4+currentlength*4;				//返回数据长度，有4个固定，02，03，校检
		ptcom->return_start[i]=1;				//返回数据有效开始
		ptcom->return_length_available[i]=currentlength*4;	//返回有效数据长度		
	}
	ptcom->send_times=SendTimes;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->send_staradd[99]=1;
}

void Write_Analog()								//写模拟量
{
	U16 aakj;
	int b;
	int a1,a2,a3,a4;
	int add;
	int b1,b2;
	int i,k;
	int length;
	
	switch (ptcom->registerr)	//根据寄存器类型获得偏移地址
	{
	case 'D':
	case 'N':	
		add=0x4000;
		break;
	case 'R':					//文件寄存器
		add=0x0;
		break;		
	case 't':
		add=0x1000;
		break;		
	case 'c':
		add=0x0A00;
		break;	
    case 'x':                //cv32(计数器200-255)
        add=0x0c00;
        break;					
	}		
				
	b=ptcom->address;			//开始地址
	
	if(ptcom->registerr == 'N')
	{
		b = b+8000;
	}
/*
	if(ptcom->registerr=='v')  //双字
	{
		b=b*4+add;				//加上偏移地址				
	}
	else                      //单字
*/
	 b=b*2+add;					//加上偏移地址
	
	a1=b&0xf000;
	a1=a1>>12;
	a1=a1&0xf;
	a2=b&0xf00;
	a2=a2>>8;
	a2=a2&0xf;
	a3=b&0xf0;
	a3=a3>>4;
	a3=a3&0xf;
	a4=b&0xf;
	*(U8 *)(AD1+0)=0x02;
	*(U8 *)(AD1+1)=0x45;
	
	if(ptcom->registerr=='R')
	{
		*(U8 *)(AD1+2)=0x31;
		*(U8 *)(AD1+3)=0x36;		//R标识	
	}
	else							//一般标识
	{
		*(U8 *)(AD1+2)=0x31;
		*(U8 *)(AD1+3)=0x30;
	}	
	
	*(U8 *)(AD1+4)=asicc(a1);	//起始地址
	*(U8 *)(AD1+5)=asicc(a2);
	*(U8 *)(AD1+6)=asicc(a3);
	*(U8 *)(AD1+7)=asicc(a4);
	
    

    length= ptcom->register_length*2; //一个普通的D要发出2个字节   最后发出时要分成4个半字  
    
    b1=(length>>4)&0x0f;       
    b2=length&0x0f;
	*(U8 *)(AD1+8)=asicc(b1);				//发送长度
	*(U8 *)(AD1+9)=asicc(b2);

	k=0;
	
	for (i=0;i<length;i++)
	{
		a1=(ptcom->U8_Data[i]&0xf0)>>4;
		a2=ptcom->U8_Data[i]&0xf;	
		*(U8 *)(AD1+10+k)=asicc(a1);
		*(U8 *)(AD1+11+k)=asicc(a2);		
		k=k+2;	
	}
	*(U8 *)(AD1+10+length*2)=0x03; // 03是结束符 拆成半字发出  所以要乘以2   
	
	//*(U8 *)(AD1+200)=length;

	aakj=CalcHe((U8 *)AD1,(10+length*2+1));	//计算和校检
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+10+length*2+1)=asicc(a1);
	*(U8 *)(AD1+10+length*2+2)=asicc(a2);	
// 真正发出时是拆成半字 所以要乘以2
	ptcom->send_length[0]=10+length*2+3;				//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址	
	ptcom->send_times=1;					//发送次数
			
	ptcom->return_length[0]=1;				//返回数据长度
	ptcom->return_start[0]=0;				//返回数据有效开始
	ptcom->return_length_available[0]=0;	//返回有效数据长度	
	ptcom->Current_Times=0;				//当前发送次数	
	ptcom->send_staradd[99]=0;
}
void Write_Wordbit() //对字节的位处理
{
	U16 aakj;
	int b;
	int b1,b2;
	int a1,a2,a3,a4;
	int add;
	int i;
	
	switch (ptcom->registerr)	//根据寄存器类型获得偏移地址
	{
	case 'L':	//D_Bit	
		add=0x4000;
		break;
	}		
	b=ptcom->address;			    //开始地址
	if(ptcom->registerr == 'L')
	{
		b = b/16;
	}
    b=b*2+add;                       //加上偏移地址
 
	a1=b&0xf000;
	a1=a1>>12;
	a1=a1&0xf;
	a2=b&0xf00;
	a2=a2>>8;
	a2=a2&0xf;
	a3=b&0xf0;
	a3=a3>>4;
	a3=a3&0xf;
	a4=b&0xf;
	*(U8 *)(AD1+0)=0x02;
	*(U8 *)(AD1+1)=0x45;

	*(U8 *)(AD1+2)=0x30;
	*(U8 *)(AD1+3)=0x30;
	
	*(U8 *)(AD1+4)=asicc(a1);	//起始地址
	*(U8 *)(AD1+5)=asicc(a2);
	*(U8 *)(AD1+6)=asicc(a3);
	*(U8 *)(AD1+7)=asicc(a4);

	*(U8 *)(AD1+8)=0x30;	//发送长度
	*(U8 *)(AD1+9)=0x32;
	
	*(U8 *)(AD1+10)=0x03;
	
	aakj=CalcHe((U8 *)AD1,11);	//计算和校检
	b1=(aakj&0xf0)>>4;
	b1=b1&0xf;
	b2=aakj&0xf;
	*(U8 *)(AD1+11)=asicc(b1);
	*(U8 *)(AD1+12)=asicc(b2);
	ptcom->send_length[0]=13;				//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址	
	ptcom->send_times=1;					//发送次数
    
    ptcom->return_length_available[0]=ptcom->register_length*4; //返回有效数据长度  
    ptcom->return_length[0]=4+ptcom->register_length*4;         //返回数据长度，有4个固定，02，03，校检
	
	ptcom->return_start[0]=1;				//返回数据有效开始
	ptcom->Current_Times=0;				//当前发送次数	
	ptcom->send_add[0]=ptcom->address;		//读的是这个地址的数据
	/*write*/
	*(U8 *)(AD1+13)=0x02;
	*(U8 *)(AD1+14)=0x45;
	
	*(U8 *)(AD1+15)=0x31;
	*(U8 *)(AD1+16)=0x30;

	*(U8 *)(AD1+17)=asicc(a1);			//起始地址
	*(U8 *)(AD1+18)=asicc(a2);
	*(U8 *)(AD1+19)=asicc(a3);
	*(U8 *)(AD1+20)=asicc(a4);
	
	*(U8 *)(AD1+21)=0x30;			//发送长度
	*(U8 *)(AD1+22)=0x32;

    *(U8 *)(AD1+23)=0x30;
	*(U8 *)(AD1+24)=0x30;
	*(U8 *)(AD1+25)=0x30;
	*(U8 *)(AD1+26)=0x30;
	
	*(U8 *)(AD1+27)=0x03; 	// 03是结束符 拆成半字发出  所以要乘以2   
	
	aakj=CalcHe((U8 *)(AD1+13),15);	//计算和校检
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+28)=asicc(a1);
	*(U8 *)(AD1+29)=asicc(a2);	

	//sysprintf("Write_Wordbit\n");
	//for(i=0;i<=29;i++)
	//	sysprintf("*(U8 *)(AD1+%d)=0x%x\n",i,*(U8 *)(AD1+i));
	
	// 真正发出时是拆成半字 所以要乘以2
	ptcom->send_length[1]=17;				//发送长度
	ptcom->send_staradd[1]=13;				//发送数据存储地址	
	ptcom->send_times=2;					//发送次数
			
	ptcom->return_length[1]=1;				//返回数据长度
	ptcom->return_start[1]=0;				//返回数据有效开始
	ptcom->return_length_available[1]=0;	//返回有效数据长度	
	ptcom->Current_Times=0;				//当前发送次数	

	ptcom->send_staradd[99]= 100;			//先读后写	

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
	int Registerr;								//写入寄存器
	int a1,a2,a3,a4;
	U8 temp;
	U16 aakj;
	int fixadd,nda1,nda2;
	int n;
	
	
	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));//数据长度
	staradd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//数据长度
	Registerr=*(U8 *)(PE+3);					//写入寄存器
	
	if (Registerr=='R')							//文件寄存器
	{
		fixadd=0;
		nda1=0x31;
		nda2=0x36;
	}	
	else										//数据寄存器 D
	{
		fixadd=0x4000;
		nda1=0x31;
		nda2=0x30;		
	}
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
	
	ps=141;
	for (i=0;i<SendTimes;i++)
	{
		b=staradd+i*32;//起始地址
		b=b*2+fixadd;
		a1=b&0xf000;
		a1=a1>>12;
		a1=a1&0xf;
		a2=b&0xf00;
		a2=a2>>8;
		a2=a2&0xf;
		a3=b&0xf0;
		a3=a3>>4;
		a3=a3&0xf;
		a4=b&0xf;
		*(U8 *)(AD1+0+ps*i)=0x02;;
		*(U8 *)(AD1+1+ps*i)=0x45;
		*(U8 *)(AD1+2+ps*i)=nda1;				//写入寄存器标识
		*(U8 *)(AD1+3+ps*i)=nda2;
		*(U8 *)(AD1+4+ps*i)=asicc(a1);
		*(U8 *)(AD1+5+ps*i)=asicc(a2);
		*(U8 *)(AD1+6+ps*i)=asicc(a3);
		*(U8 *)(AD1+7+ps*i)=asicc(a4);

		if (i!=(SendTimes-1))//不是一次发送，则发固定长度32个D，即64个字节,三菱PLC中，按ASC码发送，一个D占4个CHAR
		{
			*(U8 *)(AD1+8+ps*i)=0x34;
			*(U8 *)(AD1+9+ps*i)=0x30;
			for(j=0;j<64;j++)
			{
				temp=*(U8 *)(PE+9+i*64+j);
				a1=(temp&0xf0)>>4;
				a2=temp&0xf;
				*(U8 *)(AD1+10+j*2+ps*i)=asicc(a1);
				*(U8 *)(AD1+10+j*2+1+ps*i)=asicc(a2);				
			}
			*(U8 *)(AD1+138+ps*i)=0x03;
			aakj=CalcHe((U8 *)(AD1+i*ps),139);
			a1=(aakj&0xf0)>>4;
			a1=a1&0xf;
			a2=aakj&0xf;
			*(U8 *)(AD1+139+i*ps)=asicc(a1);
			*(U8 *)(AD1+140+i*ps)=asicc(a2);
			ptcom->send_length[i]=141;				//发送长度
			ptcom->send_staradd[i]=i*ps;			//发送数据存储地址	
			ptcom->return_length[i]=1;				//返回数据长度
			ptcom->return_start[i]=0;				//返回数据有效开始
			ptcom->return_length_available[i]=0;	//返回有效数据长度				
		}
		if (i==(SendTimes-1))//最后一次发送
		{
			*(U8 *)(AD1+8+ps*i)=asicc(((LastTimeWord*2)>>4)&0xf);
			*(U8 *)(AD1+9+ps*i)=asicc((LastTimeWord*2)&0xf);
			for(j=0;j<LastTimeWord*2;j++)
			{
				temp=*(U8 *)(PE+9+i*64+j);
				a1=(temp&0xf0)>>4;
				a2=temp&0xf;		
				*(U8 *)(AD1+10+j*2+ps*i)=asicc(a1);
				*(U8 *)(AD1+10+j*2+1+ps*i)=asicc(a2);							
			}
			*(U8 *)(AD1+LastTimeWord*4+10+ps*i)=0x03;
			aakj=CalcHe((U8 *)(AD1+i*ps),LastTimeWord*4+10+1);
			a1=(aakj&0xf0)>>4;
			a1=a1&0xf;
			a2=aakj&0xf;
			*(U8 *)(AD1+LastTimeWord*4+10+1+i*ps)=asicc(a1);
			*(U8 *)(AD1+LastTimeWord*4+10+2+i*ps)=asicc(a2);
			ptcom->send_length[i]=LastTimeWord*4+10+3;				//发送长度
			ptcom->send_staradd[i]=i*ps;			//发送数据存储地址	
			ptcom->return_length[i]=1;				//返回数据长度
			ptcom->return_start[i]=0;				//返回数据有效开始
			ptcom->return_length_available[i]=0;	//返回有效数据长度	
		}		
	}
	ptcom->send_times=SendTimes;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数		
}


void compxy(void)				//处理成标准存储格式
{
	int i;
	unsigned char a;
	unsigned char b1,b2,b3,b4;
	int nBitPos = 0;
	unsigned short nTemp = 0;
	int c1,c2;
	U16 aakj1;
    U16 t;

	if(ptcom->send_staradd[99] == 100)
	{
		t=0;
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/2;i++)						//ASC玛返回，所以要转为16进制
		{
			a=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2);
	        b1=bsicc(a);
			a=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2+1);
	        b2=bsicc(a);
	        *(U8 *)(COMad+t)=(b1<<4)+b2;
			//sysprintf("compxy  *(U8 *)(COMad+t)=0x%x\n", *(U8 *)(COMad+t));
	        t++;						
		}
		
		nTemp = nTemp |  *(U8 *)(COMad+1);
		nTemp = (nTemp << 8) & 0xff00;
		nTemp = nTemp |  *(U8 *)(COMad+0);
		nBitPos = ptcom->address%16;
		//sysprintf("nBitPos=%x\n",nBitPos);
		if (ptcom->writeValue==1)	    	//置位
		{
			nTemp = nTemp | (0x01 << nBitPos);
		}
		else if (ptcom->writeValue==0)	    //复位
		{
			nTemp = nTemp & (~(0x01 << nBitPos));
		}
		//sysprintf("compxy nTemp=0x%x\n",nTemp);
	
		*(U8 *)(AD1+23)=asicc((nTemp >> 4)&0x0f);	
		*(U8 *)(AD1+24)=asicc((nTemp >> 0)&0x0f);
		*(U8 *)(AD1+25)=asicc((nTemp >> 12)&0x0f);
		*(U8 *)(AD1+26)=asicc((nTemp >> 8)&0x0f);

	
		aakj1=CalcHe((U8 *)(AD1+13),15);	//计算和校检
		c1=(aakj1&0xf0)>>4;
		c1=c1&0xf;
		c2=aakj1&0xf;
	
		*(U8 *)(AD1+28)=asicc(c1);
		*(U8 *)(AD1+29)=asicc(c2);	
	
	//	for(i=13;i<=29;i++)
	//	sysprintf("(AD1+%d)=0x%x\n",i,*(U8 *)(AD1+i));

	}
	else if (ptcom->send_staradd[99] == 2)
	{
		t=0;
//		sysprintf("ptcom->return_length_available[ptcom->Current_Times-1]=%d\n",ptcom->return_length_available[ptcom->Current_Times-1]);
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/4;i++)						//ASC玛返回，所以要转为16进制
		{
			a=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+0);
	        b1=bsicc(a);
			a=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+1);
	        b2=bsicc(a);
			a=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+2);
	        b3=bsicc(a);
			a=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+3);
	        b4=bsicc(a);
	        *(U8 *)(COMad+t)=(b1<<4)+b2;
			*(U8 *)(COMad+t+1)=(b3<<4)+b4;
	//		sysprintf("compxy t=%d\n",t);
	        t+=2;						
		}
	}
	else
	{
	    t=0;
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/2;i++)						//ASC玛返回，所以要转为16进制
		{
			a=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2);
	        b1=bsicc(a);
			a=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2+1);
	        b2=bsicc(a);
	        *(U8 *)(COMad+t)=(b1<<4)+b2;
			//sysprintf("compxy t=%d\n",t);
	        t++;						
		}
	}
	ptcom->IfResultCorrect=1;
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
	unsigned int aakj1;
	unsigned int aakj2;
	unsigned int akj1;
	unsigned int akj2;
    if(ptcom->return_length[ptcom->Current_Times-1]!=ptcom->IQ)
    {
        return 0;
    }

    
	aakj2=*(U8 *)(COMad+ptcom->IQ-1)&0xff;		
	aakj1=*(U8 *)(COMad+ptcom->IQ-2)&0xff;
	akj1=CalcHe((U8 *)COMad,ptcom->IQ-2);
	akj2=(akj1&0xf0)>>4;
	akj2=akj2&0xf;
	akj1=akj1&0xf;
	akj1=asicc(akj1);
	akj2=asicc(akj2);
	if((akj1==aakj2)&&(akj2==aakj1))
		return 1;

        return 0;
}


U16 CalcHe(unsigned char *chData,U16 uNo)		//计算和校检
{
	int i;
	int ab=0;
	for(i=1;i<uNo;i++)
	{
		ab=ab+chData[i];
	}
	return (ab);
}
