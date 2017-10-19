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
		case 'M':
		case 'Y':
		case 'H':
		case 'L':
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
		case 'L':
		case 'Y':
		case 'H':
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
		}
		break;
	case PLC_READ_RECIPE:				//进入驱动是从PLC读取配方
		switch(*(U8 *)(PE+3))//配方寄存器名称
		{
		case 'D':		
			Read_Recipe();		
			break;			
		}
		break;	
	case 7:				//进入驱动是把PLC的状态设置为监视
	/*	ptcom->R_W_Flag=0;
		ptcom->address=0;
		ptcom->registerr='M';
		ptcom->register_length=1;
		Read_Bool();	*/
		handshake();
		break;									
	case PLC_CHECK_DATA:				//进入驱动是数据处理
		watchcom();
		break;				
	}	 
}

void handshake()		//设置PLC状态
{
	int plcadd;
	U16 aakj;
	int a1,a2;		
	
	plcadd=ptcom->plc_address;	//PLC站地址
	*(U8 *)(AD1+0)=0x40;
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);  //plc站地址，高位在前
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);	
	*(U8 *)(AD1+3)=0x53;
	*(U8 *)(AD1+4)=0x43;
	*(U8 *)(AD1+5)=0x30;
	*(U8 *)(AD1+6)=0x32;	
	aakj=CalFCS((U8 *)AD1,7);    //FCS校验，高位在前，低位在后         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+7)=asicc(a1);    //asicc码显示
	*(U8 *)(AD1+8)=asicc(a2);
	*(U8 *)(AD1+9)=0x2a;         //结束字元2a,0d
	*(U8 *)(AD1+10)=0x0d;	
	ptcom->send_length[0]=11;				//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址	
	ptcom->send_times=1;					//发送次数
		
	ptcom->return_length[0]=11;				//返回数据长度
	ptcom->return_start[0]=0;				//返回数据有效开始
	ptcom->return_length_available[0]=0;	//返回有效数据长度	
	ptcom->Current_Times=0;					//当前发送次数			
	ptcom->Simens_Count=0;
}

void Set_Reset()               //置位和复位
{
	U16 aakj;
	int b,b1,b2,b3;
	int a1,a2,a3,a4;
	int plcadd;
	int t,c;	
	int c1,c2,c3,c4;
	int sendlength;
	
	
	sendlength=17;
	b=ptcom->address;			// 开始置位地址
	plcadd=ptcom->plc_address;	//PLC站地址
	
	b=(b+8)/100;						//求商
//先读	////////////////////////////////////
	switch (ptcom->registerr)	//操作数，根据寄存器的不同，有不同的功能码
	{
	case 'Y':
	case 'M':
	case 'L':
	case 'H':
		a1=(b/100);             //取百位数
		a2=((b-a1*100)/10);     //取十位数
		a3=(b-a1*100-a2*10);    //取个位数
		break;
	case 'T':
	case 'C':
		a1=(b/100);             //取百位数
		a2=((b-a1*100)/10);     //取十位数
		a3=(b-a1*100-a2*10);    //取个位数
		break;
		
	}

	*(U8 *)(AD1+0)=0x40;        //OMRON开始字元"@",既40
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf); //plc站地址，高位在前
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);
	
	switch (ptcom->registerr)	//操作数，根据寄存器的不同，有不同的功能码
	{
	case 'Y':
		*(U8 *)(AD1+3)=0x52;      //对IR/SR区，功能码为RR，即52 52
		*(U8 *)(AD1+4)=0x52;
		break;
	case 'M':
		*(U8 *)(AD1+3)=0x52;      //对AR区，功能码为RJ，即52 48
		*(U8 *)(AD1+4)=0x4A;
		break;	
	case 'T':
		*(U8 *)(AD1+3)=0x52;      //对T区，功能码为RG，即52 47
		*(U8 *)(AD1+4)=0x47;
		break;
	case 'C':
		*(U8 *)(AD1+3)=0x52;      //对C区，功能码为RG，即52 47
		*(U8 *)(AD1+4)=0x47;
		break;	
	case 'L':
		*(U8 *)(AD1+3)=0x52;      //对LR区，功能码为RL，即52 4C
		*(U8 *)(AD1+4)=0x4C;
		break;	
	case 'H':
		*(U8 *)(AD1+3)=0x52;      //对HR区，功能码为RH，即52 48
		*(U8 *)(AD1+4)=0x48;
		break;							
	}

	*(U8 *)(AD1+5)=0x30;         //开始地址，依次从高到低，要转成asicc码
	*(U8 *)(AD1+6)=asicc(a1);
	*(U8 *)(AD1+7)=asicc(a2);
	*(U8 *)(AD1+8)=asicc(a3);
	
	
	switch (ptcom->registerr)	//操作数，根据寄存器的不同，有不同的功能码
	{
	case 'Y':
	case 'M':
	case 'L':
	case 'H':
		c=1;   								//读1个元件
		c1=0;                    //取千位数
		c2=0;           //取百位数
		c3=0;     //取十位数
		c4=1;    //取个位数 
		*(U8 *)(AD1+9)=asicc(c1);	//发送元件个数，以asicc码发送，依次从高到低
		*(U8 *)(AD1+10)=asicc(c2);
		*(U8 *)(AD1+11)=asicc(c3);
		*(U8 *)(AD1+12)=asicc(c4);
		aakj=CalFCS((U8 *)AD1,13);    //FCS校验，高位在前，低位在后         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13)=asicc(a1);    //asicc码显示
		*(U8 *)(AD1+14)=asicc(a2);
		*(U8 *)(AD1+15)=0x2a;         //结束字元2a,0d
		*(U8 *)(AD1+16)=0x0d;	
		
		ptcom->Simens_Count=1;
		
		ptcom->register_length=c*2;				//返回字节数
		ptcom->send_length[0]=17;				//发送长度
		ptcom->send_staradd[0]=0;				//发送数据存储地址	
			
		ptcom->return_length[0]=11+4;
		                                          
		ptcom->return_start[0]=7;				//返回数据有效开始
		ptcom->return_length_available[0]=4 ;	//返回有效数据长度	
		ptcom->Current_Times=0;					//当前发送次数	
		ptcom->send_add[0]=b*100;		//读的是这个地址的数据             
		break;
	case 'T':
	case 'C':
		c=1;   								//读多少个元件
		c1=0;                    //取千位数
		c2=0;           //取百位数
		c3=0;     //取十位数
		c4=1;    //取个位数 
		
		ptcom->Simens_Count=1;
		
		*(U8 *)(AD1+9)=asicc(c1);	//发送元件个数，以asicc码发送，依次从高到低
		*(U8 *)(AD1+10)=asicc(c2);
		*(U8 *)(AD1+11)=asicc(c3);
		*(U8 *)(AD1+12)=asicc(c4);

		aakj=CalFCS((U8 *)AD1,13);    //FCS校验，高位在前，低位在后         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13)=asicc(a1);    //asicc码显示
		*(U8 *)(AD1+14)=asicc(a2);
		*(U8 *)(AD1+15)=0x2a;         //结束字元2a,0d
		*(U8 *)(AD1+16)=0x0d;	
		
		ptcom->send_length[0]=17;				//发送长度
		ptcom->send_staradd[0]=0;				//发送数据存储地址	
			
		ptcom->return_length[0]=15;				//返回数据长度
		                                                   
		ptcom->return_start[0]=7;				//返回数据有效开始
		ptcom->return_length_available[0]=4;	//返回有效数据长度	
		ptcom->Current_Times=0;					//当前发送次数	
		ptcom->send_add[0]=b*16;					//读的是这个地址的数据	
	break;
		
	}	
	
//再写	//////////////////////////////////////

	switch (ptcom->registerr)	//操作数，根据寄存器的不同，有不同的操作数
	{
	case 'Y':
	case 'L':
	case 'H':	
		t=b;                   		 //指针给的地址是10进制的，并把*100，现/100取整
		a1=(t/100)&0xf;             //取百位数
		a2=((t-a1*100)/10)&0xf;     //取十位数
		a3=(t-a1*100-a2*10)&0xf;    //取个位数
		break;
	case 'T':
	case 'C':
		a1=(b/100)&0xf;             //取百位数
		a2=((b-a1*100)/10)&0xf;     //取十位数
		a3=(b-a1*100-a2*10)&0xf;    //取个位数
		break;		
	}
		
	*(U8 *)(AD1+0+sendlength)=0x40;        //OMRON开始字元"@",即40
	*(U8 *)(AD1+1+sendlength)=asicc(((plcadd&0xf0)>>4)&0xf);  //plc站地址，高位在前
	*(U8 *)(AD1+2+sendlength)=asicc(plcadd&0xf);
	
	
	switch (ptcom->registerr)	//操作数，根据寄存器的不同，有不同的操作数
	{
	case 'Y':
		*(U8 *)(AD1+3+sendlength)='W';      //对IR/SR区，操作数为C I O 空格
		*(U8 *)(AD1+4+sendlength)='R';
		break;
	case 'L':
		*(U8 *)(AD1+3+sendlength)='W';      //对IR/SR区，操作数为C I O 空格
		*(U8 *)(AD1+4+sendlength)='L';
		break;	
	case 'T':
		*(U8 *)(AD1+3+sendlength)='W';      //对IR/SR区，操作数为C I O 空格
		*(U8 *)(AD1+4+sendlength)='G';
		break;
	case 'C':
		*(U8 *)(AD1+3+sendlength)='W';      //对IR/SR区，操作数为C I O 空格
		*(U8 *)(AD1+4+sendlength)='G';
		break;
	case 'H':
		*(U8 *)(AD1+3+sendlength)='W';      //对IR/SR区，操作数为C I O 空格
		*(U8 *)(AD1+4+sendlength)='H';
		break;					
	}	
	
	*(U8 *)(AD1+5+sendlength)=0x30;					//地址
	*(U8 *)(AD1+6+sendlength)=asicc(a1);
	*(U8 *)(AD1+7+sendlength)=asicc(a2);
	*(U8 *)(AD1+8+sendlength)=asicc(a3);
	
	*(U8 *)(AD1+9+sendlength)=0x30;					//写入数据保留	
	*(U8 *)(AD1+10+sendlength)=0x31;
	*(U8 *)(AD1+11+sendlength)=0x32;
	*(U8 *)(AD1+12+sendlength)=0x33;	
	
			
	aakj=CalFCS((U8 *)AD1+sendlength,13);    //FCS校验，高位在前，低位在后         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+13+sendlength)=asicc(a1);    //asicc码显示
	*(U8 *)(AD1+14+sendlength)=asicc(a2);
	*(U8 *)(AD1+15+sendlength)=0x2a;         //结束字元2a,0d
	*(U8 *)(AD1+16+sendlength)=0x0d;
	
	ptcom->send_length[1]=17;				//发送长度
	ptcom->send_staradd[1]=17;				//发送数据存储地址	
		
	ptcom->return_length[1]=11;				//返回数据长度
	ptcom->return_start[1]=0;				//返回数据有效开始
	ptcom->return_length_available[1]=0;	//返回有效数据长度	
	ptcom->Current_Times=0;					//当前发送次数	
	
	ptcom->send_times=2;					//发送次数	
	
	if (ptcom->writeValue==1)		//职位
	{
		ptcom->Simens_Count=100;
	}	
	if (ptcom->writeValue==0)		//复位
	{
		ptcom->Simens_Count=200;
	}	
}



void Read_Bool()				//读取数字量的信息
{
	U16 aakj;
	int b,t,c;
	int a0,a1,a2,a3;
	int b1,b2,b3;
	int plcadd;	
	int c1,c2,c3,c4;
		
	b=ptcom->address;			//在主程序已经转换到该段的开始地址
	plcadd=ptcom->plc_address;	//PLC站地址
	
	switch (ptcom->registerr)	//操作数，根据寄存器的不同，有不同的功能码
	{
	case 'Y':
	case 'M':
	case 'L':
	case 'H':
	
		b=(b+8)/100;				//向整百靠拢
		t=b;
		ptcom->address=t*100;

		a0=(t/1000)&0xf;
		a1=((t-a0*1000)/100)&0xf;             //取百位数
		a2=((t-a1*100)/10)&0xf;     //取十位数
		a3=(t-a1*100-a2*10)&0xf;    //取个位数
		break;
	case 'T':
	case 'C':
		a0=(t/1000)&0xf;
		a1=((t-a0*1000)/100)&0xf;             //取百位数
		a2=((b-a1*100)/10)&0xf;     //取十位数
		a3=(b-a1*100-a2*10)&0xf;    //取个位数
		break;
		
	}

	*(U8 *)(AD1+0)=0x40;        //OMRON开始字元"@",既40
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf); //plc站地址，高位在前
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);
	
	switch (ptcom->registerr)	//操作数，根据寄存器的不同，有不同的功能码
	{
	case 'Y':
		*(U8 *)(AD1+3)=0x52;      //对IR/SR区，功能码为RR，即52 52
		*(U8 *)(AD1+4)=0x52;
		break;
	case 'M':
		*(U8 *)(AD1+3)=0x52;      //对AR区，功能码为RJ，即52 48
		*(U8 *)(AD1+4)=0x4A;
		break;	
	case 'T':
		*(U8 *)(AD1+3)=0x52;      //对T区，功能码为RG，即52 47
		*(U8 *)(AD1+4)=0x47;
		break;
	case 'C':
		*(U8 *)(AD1+3)=0x52;      //对C区，功能码为RG，即52 47
		*(U8 *)(AD1+4)=0x47;
		break;	
	case 'L':
		*(U8 *)(AD1+3)=0x52;      //对LR区，功能码为RL，即52 4C
		*(U8 *)(AD1+4)=0x4C;
		break;	
	case 'H':
		*(U8 *)(AD1+3)=0x52;      //对HR区，功能码为RH，即52 48
		*(U8 *)(AD1+4)=0x48;
		break;							
	}

	*(U8 *)(AD1+5)=asicc(a0);         //开始地址，依次从高到低，要转成asicc码
	*(U8 *)(AD1+6)=asicc(a1);
	*(U8 *)(AD1+7)=asicc(a2);
	*(U8 *)(AD1+8)=asicc(a3);
	
	
	switch (ptcom->registerr)	//操作数，根据寄存器的不同，有不同的功能码
	{
	case 'Y':
	case 'M':
	case 'L':
	case 'H':
		c=ptcom->register_length;   //读多少个元件
		c=(c/2)+(c%2);
		c1=(c/1000)&0xf;                    //取千位数
		c2=((c-c1*1000)/100)&0xf;           //取百位数
		c3=((c-c1*1000-c2*100)/10)&0xf;     //取十位数
		c4=(c-c1*1000-c2*100-c3*10)&0xf;    //取个位数 
		*(U8 *)(AD1+9)=asicc(c1);	//发送元件个数，以asicc码发送，依次从高到低
		*(U8 *)(AD1+10)=asicc(c2);
		*(U8 *)(AD1+11)=asicc(c3);
		*(U8 *)(AD1+12)=asicc(c4);
		aakj=CalFCS((U8 *)AD1,13);    //FCS校验，高位在前，低位在后         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13)=asicc(a1);    //asicc码显示
		*(U8 *)(AD1+14)=asicc(a2);
		*(U8 *)(AD1+15)=0x2a;         //结束字元2a,0d
		*(U8 *)(AD1+16)=0x0d;	
		
		ptcom->register_length=c*2;				//返回字节数
		ptcom->send_length[0]=17;				//发送长度
		ptcom->send_staradd[0]=0;				//发送数据存储地址	
		ptcom->send_times=1;					//发送次数
			
		ptcom->return_length[0]=11+c*4;//返回数据长度，有11个固定，40，PLC地址2个长度，功能码2个长度，
		                                                    //结束码2个长度，校检2个长度，0d，0a
		ptcom->return_start[0]=7;				//返回数据有效开始
		ptcom->return_length_available[0]=c*4 ;	//返回有效数据长度	
		ptcom->Current_Times=0;					//当前发送次数	
		ptcom->send_add[0]=ptcom->address;		//读的是这个地址的数据
		
		ptcom->Simens_Count=1;	                    //读数字量时颠倒顺序             
		break;
	case 'T':
	case 'C':
		c=ptcom->register_length;   //读多少个元件
		c=c*8;
		c1=(c/1000)&0xf;                    //取千位数
		c2=((c-c1*1000)/100)&0xf;           //取百位数
		c3=((c-c1*1000-c2*100)/10)&0xf;     //取十位数
		c4=(c-c1*1000-c2*100-c3*10)&0xf;    //取个位数
		
		ptcom->Simens_Count=1;
		
		*(U8 *)(AD1+9)=asicc(c1);	//发送元件个数，以asicc码发送，依次从高到低
		*(U8 *)(AD1+10)=asicc(c2);
		*(U8 *)(AD1+11)=asicc(c3);
		*(U8 *)(AD1+12)=asicc(c4);

		aakj=CalFCS((U8 *)AD1,13);    //FCS校验，高位在前，低位在后         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13)=asicc(a1);    //asicc码显示
		*(U8 *)(AD1+14)=asicc(a2);
		*(U8 *)(AD1+15)=0x2a;         //结束字元2a,0d
		*(U8 *)(AD1+16)=0x0d;	
		
		ptcom->send_length[0]=17;				//发送长度
		ptcom->send_staradd[0]=0;				//发送数据存储地址	
		ptcom->send_times=1;					//发送次数
			
		ptcom->return_length[0]=11+c;//返回数据长度，有11个固定，40，PLC地址2个长度，功能码2个长度，
		                                                    //结束码2个长度，校检2个长度，0d，0a
		ptcom->return_start[0]=7;				//返回数据有效开始
		ptcom->return_length_available[0]=c;	//返回有效数据长度	
		ptcom->Current_Times=0;					//当前发送次数	
		ptcom->send_add[0]=ptcom->address;		//读的是这个地址的数据
		ptcom->Simens_Count=1;	
	break;
		
	}	
}


void Read_Analog()				//读模拟量
{
	U16 aakj;
	int b,c;
	int a1,a2,a3,a4;
	int c1,c2,c3,c4;
	int plcadd;
				
	b=ptcom->address;			//开始地址
	plcadd=ptcom->plc_address;	//PLC站地址
	
	a1=(b/1000)&0xf;                    //取10进制千位数
	a2=((b-a1*1000)/100)&0xf;           //取10进制百位数
	a3=((b-a1*1000-a2*100)/10)&0xf;     //取10进制十位数
	a4=(b-a1*1000-a2*100-a3*10)&0xf;    //取10进制个位数	

	*(U8 *)(AD1+0)=0x40;          //OMRON开始字元"@",既40
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);        //plc站地址，高位在前
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);
	
	switch (ptcom->registerr)	  //操作数，根据寄存器的不同，有不同的功能码
	{
	case 'D':
		*(U8 *)(AD1+3)=0x52;      //对IR/SR区，功能码为RD，即52 44
		*(U8 *)(AD1+4)=0x44;
		break;
	case 'R':
		*(U8 *)(AD1+3)=0x52;      //对IR/SR区，功能码为RD，即52 52
		*(U8 *)(AD1+4)=0x52;
		break;
	case 't':
		*(U8 *)(AD1+3)=0x52;      //对T区，功能码为RC，即52 43
		*(U8 *)(AD1+4)=0x43;
		break;
	case 'c':
		*(U8 *)(AD1+3)=0x52;      //对C区，功能码为RC，即52 43
		*(U8 *)(AD1+4)=0x43;
		break;					
	}	
	
	*(U8 *)(AD1+5)=asicc(a1);     //开始地址，依次从高到低，要转成asicc码
	*(U8 *)(AD1+6)=asicc(a2);
	*(U8 *)(AD1+7)=asicc(a3);
	*(U8 *)(AD1+8)=asicc(a4);
	
	c=ptcom->register_length;     //读多少个元件
 
	c1=(c/1000)&0xf;                    //取千位数
	c2=((c-c1*1000)/100)&0xf;           //取百位数
	c3=((c-c1*1000-c2*100)/10)&0xf;     //取十位数
	c4=(c-c1*1000-c2*100-c3*10)&0xf;    //取个位数

	*(U8 *)(AD1+9)=asicc(c1);	 //发送元件个数，以asicc码发送，依次从高到低
	*(U8 *)(AD1+10)=asicc(c2);
	*(U8 *)(AD1+11)=asicc(c3);
	*(U8 *)(AD1+12)=asicc(c4);

	aakj=CalFCS((U8 *)AD1,13);    //FCS校验，高位在前，低位在后         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+13)=asicc(a1);    //asicc码显示
	*(U8 *)(AD1+14)=asicc(a2);
	*(U8 *)(AD1+15)=0x2a;         //结束字元2a,0d
	*(U8 *)(AD1+16)=0x0d;	
	
	ptcom->send_length[0]=17;				//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址	
	ptcom->send_times=1;					//发送次数
		
	ptcom->return_length[0]=11+ptcom->register_length*4;//返回数据长度，有11个固定，40，PLC地址2个长度，功能码2个长度，
	                                                    //结束码2个长度，校检2个长度，2a，0d
	ptcom->return_start[0]=7;				//返回数据有效开始
	ptcom->return_length_available[0]=ptcom->register_length*4;	//返回有效数据长度	
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->send_add[0]=ptcom->address;		//读的是这个地址的数据

	ptcom->Simens_Count=5;
}


void Read_Recipe()							//读取配方
{
	U16 aakj;
	int b;
	int c,c1,c2,c3,c4;
	int a1,a2,a3,a4;
	int i;
	int datalength;                         //数据长度
	int p_start;                            //数据开始地址
	int ps;
	int SendTimes;                          //发送次数
	int LastTimeWord;						//最后一次发送长度
	int currentlength;
	int plcadd;                             //PLC站地址
	
	datalength=ptcom->register_length;		//发送总长度
	p_start=ptcom->address;					//开始地址
	plcadd=ptcom->plc_address;	            //PLC站地址
	
	if(datalength>5000)                     //限制长度
		datalength=5000;

	if(datalength%28==0)                    //台达最多能发28个D，数据刚好是28D的倍数
	{
		SendTimes=datalength/28;            //发送次数
		LastTimeWord=28;                    //最后一次发送的长度为28D	
	}
	if(datalength%28!=0)                    //台达最多能发28个D，数据不是28D的倍数 
	{
		SendTimes=datalength/28+1;          //发送的次数
		LastTimeWord=datalength%28;         //最后一次发送的长度为除28的余数	
	}
	
	for (i=0;i<SendTimes;i++)
	{
		ps=i*17;                            //每次发17个长度
		b=p_start+i*28;                     //起始地址
		a1=(b/1000)&0xf;                    //取千位数
		a2=((b-a1*1000)/100)&0xf;           //取百位数
		a3=((b-a1*1000-a2*100)/10)&0xf;     //取十位数
		a4=(b-a1*1000-a2*100-a3*10)&0xf;    //取个位数
		
		*(U8 *)(AD1+0+ps)=0x40;             //OMRON开始字元"@",既40
		*(U8 *)(AD1+1+ps)=asicc(((plcadd&0xf0)>>4)&0xf);        //plc站地址，高位在前
		*(U8 *)(AD1+2+ps)=asicc(plcadd&0xf);
		

		*(U8 *)(AD1+3+ps)=0x52;      //对DM区，功能码为RD，即52 44
		*(U8 *)(AD1+4+ps)=0x44;

		*(U8 *)(AD1+5+ps)=asicc(a1);         //开始地址，依次从高到低，要转成asicc码
		*(U8 *)(AD1+6+ps)=asicc(a2);
		*(U8 *)(AD1+7+ps)=asicc(a3);
		*(U8 *)(AD1+8+ps)=asicc(a4);
		
		if (i!=(SendTimes-1))	     //不是最后一次发送时
		{
			*(U8 *)(AD1+9+ps)=0x30;             //固定长度28个D，即56字节，高位，asicc码显示
			*(U8 *)(AD1+10+ps)=0x30;
			*(U8 *)(AD1+11+ps)=0x32;            //固定长度28个D，即56字节，低位，asicc码显示
			*(U8 *)(AD1+12+ps)=0x38;
			currentlength=28;                   //固定长度28个D
		}
		if (i==(SendTimes-1))	     //最后一次发送时
		{
			c=LastTimeWord;                     //读多少个元件
			c1=(c/1000)&0xf;                    //取千位数
			c2=((c-c1*1000)/100)&0xf;           //取百位数
			c3=((c-c1*1000-c2*100)/10)&0xf;     //取十位数
			c4=(c-c1*1000-c2*100-c3*10)&0xf;    //取个位数
			*(U8 *)(AD1+9+ps)=asicc(c1);         //发送的长度，依次从高到低，要转成asicc码
			*(U8 *)(AD1+10+ps)=asicc(c2);
			*(U8 *)(AD1+11+ps)=asicc(c3);
			*(U8 *)(AD1+12+ps)=asicc(c4);
			currentlength=LastTimeWord;         //剩余LastTimeWord个D
		}

		aakj=CalFCS((U8 *)(AD1+ps),13);    //FCS校验，高位在前，低位在后         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13+ps)=asicc(a1);    //asicc码显示
		*(U8 *)(AD1+14+ps)=asicc(a2);
		*(U8 *)(AD1+15+ps)=0x2a;         //结束字元2a,0d
		*(U8 *)(AD1+16+ps)=0x0d;
		
		ptcom->send_length[i]=17;				    //发送长度
		ptcom->send_staradd[i]=i*17;			    //发送数据存储地址	
		ptcom->send_add[i]=p_start+i*28;		    //读的是这个地址的数据	
		ptcom->send_data_length[i]=currentlength;	//不是最后一次都是28个D
				
		ptcom->return_length[i]=11+currentlength*4; //返回数据长度，有11个固定，40，PLC地址2个长度，功能码2个长度，
	                                                //结束码2个长度，校检2个长度，0d，0a
		ptcom->return_start[i]=7;				    //返回数据有效开始
		ptcom->return_length_available[i]=currentlength*4;	//返回有效数据长度	
		
	}
	ptcom->send_times=SendTimes;					//发送次数
	ptcom->Current_Times=0;		
	ptcom->Simens_Count=5;	
}

void Write_Analog()								    //写模拟量
{
	U16 aakj;
	int b;
	int a1,a2,a3,a4;
	int i;
	int length;
	int c0,c01,c02,c03,c04;
	int c1;
	int plcadd;
	
	b=ptcom->address;			//开始地址
	plcadd=ptcom->plc_address;	//PLC站地址
	length=ptcom->register_length;//长度
	
	a1=(b/1000)&0xf;                    //取千位数
	a2=((b-a1*1000)/100)&0xf;           //取百位数
	a3=((b-a1*1000-a2*100)/10)&0xf;     //取十位数
	a4=(b-a1*1000-a2*100-a3*10)&0xf;    //取个位数	
	
	*(U8 *)(AD1+0)=0x40;        //OMRON开始字元"@",既40
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf); //plc站地址，高位在前
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);

	switch (ptcom->registerr)	  //操作数，根据寄存器的不同，有不同的功能码
	{
	case 'D':
		*(U8 *)(AD1+3)=0x57;      //对DM区，功能码为WD，即57 44
		*(U8 *)(AD1+4)=0x44;
		break;
	case 'R':
		*(U8 *)(AD1+3)=0x57;      //对DM区，功能码为WD，即57 52
		*(U8 *)(AD1+4)=0x52;
		break;
	case 't':
		*(U8 *)(AD1+3)=0x57;      //对T区，功能码为WC，即57 43
		*(U8 *)(AD1+4)=0x43;
		break;
	case 'c':
		*(U8 *)(AD1+3)=0x57;      //对C区，功能码为WC，即57 43
		*(U8 *)(AD1+4)=0x43;
		break;					
	}	
	
	*(U8 *)(AD1+5)=asicc(a1);   //开始地址，依次从高到低，要转成asicc码
	*(U8 *)(AD1+6)=asicc(a2);
	*(U8 *)(AD1+7)=asicc(a3);
	*(U8 *)(AD1+8)=asicc(a4);
	
	for (i=0;i<length;i++)        //写入多个元件值
	{				
		c0=ptcom->U8_Data[i*2];   //从D[]数组中要数据，对应的c1为高位，c0为低位
		c1=ptcom->U8_Data[i*2+1];
						
		c01=c0&0xf;               //对所写数据作asicc处理，c0为低位
		c02=(c0>>4)&0xf;          //对所写数据作asicc处理，c0为低位		
		c03=c1&0xf;               //对所写数据作asicc处理，c1为高位
		c04=(c1>>4)&0xf;          //对所写数据作asicc处理，c0为低位				
	
		*(U8 *)(AD1+9+i*4)=asicc(c04);
		*(U8 *)(AD1+10+i*4)=asicc(c03);
		*(U8 *)(AD1+11+i*4)=asicc(c02);
		*(U8 *)(AD1+12+i*4)=asicc(c01);	
	} 
		
		aakj=CalFCS((U8 *)AD1,9+length*4);      //FCS校验，高位在前，低位在后         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+9+length*4)=asicc(a1);      //asicc码显示
		*(U8 *)(AD1+10+length*4)=asicc(a2);
		*(U8 *)(AD1+11+length*4)=0x2a;          //结束字元2a,0d
		*(U8 *)(AD1+12+length*4)=0x0d;

		
		ptcom->send_length[0]=13+length*4;		//发送长度
		ptcom->send_staradd[0]=0;				//发送数据存储地址	
		ptcom->send_times=1;					//发送次数
				
		ptcom->return_length[0]=11;				//返回数据长度
		ptcom->return_start[0]=0;				//返回数据有效开始
		ptcom->return_length_available[0]=0;	//返回有效数据长度	
		ptcom->Current_Times=0;	
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
	int datalength;
	int staradd;
	int SendTimes;                              //发送的次数
	int LastTimeWord;							//最后一次发送长度
	int i,j;
	int ps;
	int b;
	int c0,c1,c01,c02,c03,c04;
	int a1,a2,a3,a4;
	U16 aakj;
	int length;
	int plcadd;	
	
	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));//数据长度
	staradd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//数据开始地址
	plcadd=*(U8 *)(PE+4);

	if(datalength%28==0)                       //OMRON最多能发28个D，数据长度刚好是28的倍数时
	{
		SendTimes=datalength/28;               //发送的次数为datalength/28               
		LastTimeWord=28;                       //最后一次发送的长度为28个D
	}
	if(datalength%28!=0)                       //数据长度不是28D的倍数时
	{
		SendTimes=datalength/28+1;             //发送的次数datalength/28+1
		LastTimeWord=datalength%28;            //最后一次发送的长度为除28的余数
	}

    ps=125;                                    //当发超过28个D的时候，前面每次发125个

	for (i=0;i<SendTimes;i++)
	{   
		if (i!=(SendTimes-1))                  //不是最后一次发送时
		{	
			length=28;                         //发28个D
		}
		else                                   //最后一次发送时
		{
			length=LastTimeWord;               //发剩余的长度             
		}
		
		b=staradd+i*28;                        //每次偏移28个D
		a1=(b/1000)&0xf;                       //取千位数
		a2=((b-a1*1000)/100)&0xf;              //取百位数
		a3=((b-a1*1000-a2*100)/10)&0xf;        //取十位数
		a4=(b-a1*1000-a2*100-a3*10)&0xf;       //取个位数

		*(U8 *)(AD1+0+ps*i)=0x40;              //OMRON开始字元"@",既40
		*(U8 *)(AD1+1+ps*i)=asicc(((plcadd&0xf0)>>4)&0xf); //plc站地址，高位在前
		*(U8 *)(AD1+2+ps*i)=asicc(plcadd&0xf);

		*(U8 *)(AD1+3+ps*i)=0x57;      //对DM区，功能码为WD，即57 44
		*(U8 *)(AD1+4+ps*i)=0x44;

		*(U8 *)(AD1+5+ps*i)=asicc(a1);         //开始地址，依次从高到低，要转成asicc码
		*(U8 *)(AD1+6+ps*i)=asicc(a2);
		*(U8 *)(AD1+7+ps*i)=asicc(a3);
		*(U8 *)(AD1+8+ps*i)=asicc(a4);

		for(j=0;j<length;j++)                  //写入多个元件值                 
		{	
			c0=*(U8 *)(PE+9+i*56+j*2);         //从存数据的寄存器开始地址PE+9取数据，k3为高位，k4为低位
			c1=*(U8 *)(PE+9+i*56+j*2+1);
							
			c01=c0&0xf;               //对所写数据作asicc处理，c0为低位
			c02=(c0>>4)&0xf;          //对所写数据作asicc处理，c0为低位		
			c03=c1&0xf;               //对所写数据作asicc处理，c1为高位
			c04=(c1>>4)&0xf;          //对所写数据作asicc处理，c0为低位				
		;
			*(U8 *)(AD1+9+j*4+ps*i)=asicc(c04);
			*(U8 *)(AD1+10+j*4+ps*i)=asicc(c03);
			*(U8 *)(AD1+11+j*4+ps*i)=asicc(c02);
			*(U8 *)(AD1+12+j*4+ps*i)=asicc(c01);							
		}

		aakj=CalFCS((U8 *)(AD1+ps*i),9+length*4);      //FCS校验，高位在前，低位在后         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13+(length-1)*4+ps*i)=asicc(a1);      //asicc码显示
		*(U8 *)(AD1+14+(length-1)*4+ps*i)=asicc(a2);
		*(U8 *)(AD1+15+(length-1)*4+ps*i)=0x2a;          //结束字元2a,0d
		*(U8 *)(AD1+16+(length-1)*4+ps*i)=0x0d;
		
		ptcom->send_length[i]=13+length*4;	   //发送长度
		ptcom->send_staradd[i]=i*ps;		   //发送数据存储地址	
		
		ptcom->return_length[i]=11;			   //返回数据长度
		ptcom->return_start[i]=0;			   //返回数据有效开始
		ptcom->return_length_available[i]=0;   //返回有效数据长度	
    }
	ptcom->send_times=SendTimes;			   //发送次数
	ptcom->Current_Times=0;				
}


void compxy(void)				              //处理成标准存储格式,重新排列
{
	int i;
	unsigned char a1,a2,a3,a4;
	int b1,b2;
	U16 temp;
	int j;
	U16 aakj;
	
	if(ptcom->Simens_Count==1)
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/4;i++)
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
			*(U8 *)(COMad+i*2+0)=b2;
			*(U8 *)(COMad+i*2+1)=b1;			
			ptcom->Simens_Count=0;	
		}
	}
	else if(ptcom->Simens_Count==5)
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/4;i++)	//ASC玛返回，所以要转为16进制，4个asicc码换成2个16进制数
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
		//	if(ptcom->Simens_Count==1)	                         //读数字量的时候为1，倒顺序
		//	{
		//		*(U8 *)(COMad+i*2)=b2;					     //重新存,从第0个开始存
		//		*(U8 *)(COMad+i*2+1)=b1;					 //重新存,从第0个开始存
		//	}		
		//	else
		//	{ 
				*(U8 *)(COMad+i*2)=b1;					     //重新存,从第0个开始存
				*(U8 *)(COMad+i*2+1)=b2;					 //重新存,从第0个开始存		
		//	}	
		}
		ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;	//长度减半	
	} 
	else if (ptcom->Simens_Count==100)			//置位
	{
		a1=*(U8 *)(COMad+7);
		a2=*(U8 *)(COMad+8);
		a3=*(U8 *)(COMad+9);
		a4=*(U8 *)(COMad+10);
		a1=bsicc(a1);
		a2=bsicc(a2);
		a3=bsicc(a3);
		a4=bsicc(a4);
		b1=(a1<<4)+a2;
		b2=(a3<<4)+a4;	
		b1=(b1<<8)+b2;
				
		j=ptcom->address-ptcom->send_add[0];			//偏移多少位

		temp=1<<j;
		
		temp=b1|temp;
		
		a1=(temp>>12)&0xf;
		a2=(temp>>8)&0xf;		
		a3=(temp>>4)&0xf;		
		a4=(temp>>0)&0xf;		

		//更改
		*(U8 *)(AD1+9+17)=asicc(a1);					//写入数据保留	
		*(U8 *)(AD1+10+17)=asicc(a2);
		*(U8 *)(AD1+11+17)=asicc(a3);
		*(U8 *)(AD1+12+17)=asicc(a4);			
		
		aakj=CalFCS((U8 *)AD1+17,13);    //FCS校验，高位在前，低位在后         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13+17)=asicc(a1);    //asicc码显示
		*(U8 *)(AD1+14+17)=asicc(a2);
			
		ptcom->Simens_Count=0;	
				
	}
	else if (ptcom->Simens_Count==200)			//复位
	{
		a1=*(U8 *)(COMad+7);
		a2=*(U8 *)(COMad+8);
		a3=*(U8 *)(COMad+9);
		a4=*(U8 *)(COMad+10);
		a1=bsicc(a1);
		a2=bsicc(a2);
		a3=bsicc(a3);
		a4=bsicc(a4);
		b1=(a1<<4)+a2;
		b2=(a3<<4)+a4;	
		b1=(b1<<8)+b2;
				
		j=ptcom->address-ptcom->send_add[0];			//偏移多少位

		temp=1<<j;
		temp=~temp;
		
		temp=b1&temp;
		
		a1=(temp>>12)&0xf;
		a2=(temp>>8)&0xf;		
		a3=(temp>>4)&0xf;		
		a4=(temp>>0)&0xf;		

		//更改
		*(U8 *)(AD1+9+17)=asicc(a1);					//写入数据保留	
		*(U8 *)(AD1+10+17)=asicc(a2);
		*(U8 *)(AD1+11+17)=asicc(a3);
		*(U8 *)(AD1+12+17)=asicc(a4);			
		
		aakj=CalFCS((U8 *)AD1+17,13);    //FCS校验，高位在前，低位在后         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13+17)=asicc(a1);    //asicc码显示
		*(U8 *)(AD1+14+17)=asicc(a2);
			
		ptcom->Simens_Count=0;			
	}	
	else
	{
		ptcom->IfResultCorrect=0;
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
	unsigned int aakj1;
	unsigned int aakj2;
	unsigned int akj1;
	unsigned int akj2;
	aakj2=*(U8 *)(COMad+ptcom->return_length[ptcom->Current_Times-1]-3)&0xff;		//在发送完后Current_Times++，此时要--
	aakj1=*(U8 *)(COMad+ptcom->return_length[ptcom->Current_Times-1]-4)&0xff;
	akj1=CalFCS((U8 *)COMad,ptcom->return_length[ptcom->Current_Times-1]-4);
	akj2=(akj1&0xf0)>>4;
	akj2=akj2&0xf;
	akj1=akj1&0xf;
	akj1=asicc(akj1);
	akj2=asicc(akj2);
	if((akj1==aakj2)&&(akj2==aakj1))
		return 1;
	else
		return 0;
}


U16 CalFCS(unsigned char *chData,unsigned short uNo)
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

