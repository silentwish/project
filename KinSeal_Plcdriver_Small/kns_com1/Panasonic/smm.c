
#include "stdio.h"
#include "def.h"
#include "smm.h"

struct Com_struct_D *ptcom;

void SetBoolDTBit() ;
 void SetBoolWLBit();
void SetBoolDTBit();
void readBit();

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
		case 'L':		
		case 'T':
		case 'C':						
			Read_Bool();            //进入驱动是读位数据       
			break;
		case 'H':                  //DT_Bit
		case 'K':                  //WL_Bit
		    readBit();            /*读DT，WL位*/
		    break;
		case 'x':
		case 'y':
		case 'l':
		case 'D':
		case 'N':
		case 't':
		case 'c':
		case 'R':
			Read_Analog();         //进入驱动是读模拟数据 
			break;			
		}
		break;
	case PLC_WRITE_DATA:				
		switch(ptcom->registerr)
		{
		case 'M':
		case 'L':
		case 'Y':
		case 'T':
		case 'C':				
			Set_Reset();      //进入驱动是强置置位和复位
			break;
	    case 'H':
	    	SetBoolDTBit();  /*写DT位*/
	        break;
	    case 'K':
	        SetBoolWLBit();  /*写WL_Bit位*/
	        break;
		case 'y':
		case 'l':
		case 'D':
		case 'N':
		case 't':
		case 'c':
		case 'R':
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
	case PLC_CHECK_DATA:				//进入驱动是数据处理
		watchcom();
		break;				
	}	 
}



void Set_Reset()
{
	U16 aakj;
	int b,b1,b2,c;
	int a1,a2,a3,a4;
	int add;
	int plcadd;

	b=ptcom->address;			// 开始置位地址
	plcadd=ptcom->plc_address;	            //PLC站地址
	switch (ptcom->registerr)	//根据寄存器类型获得偏移地址
	{
	case 'Y':
		add=0x59;
		break;
	case 'M':
		add=0x52;
		break;	
	case 'L':
		add=0x4c;
		break;
	case 'T':
		add=0x54;
		break;
	case 'C':
		add=0x43;
		break;				
	}
	b1=(plcadd>>4)&0xf;		//站地址
	b2=(plcadd>>0)&0xf;	
	
	b=b/16;									//前几位
	c=(ptcom->address)-b*16;				//最后一位
	a1=(b/100);			//寄存器开始地址
	a2=((b%100)/10);	//寄存器开始地址
	a3=((b%10)/1);		//寄存器开始地址
	a4=c;				//寄存器开始地址,松下一次最少要读1个字节数据，所以最低位要为0

	*(U8 *)(AD1+0)=0x25;        	//松下开始代码
	*(U8 *)(AD1+1)=asicc(b1);        //plc站地址01，asicc码是0x30和0x31
	*(U8 *)(AD1+2)=asicc(b2);
	*(U8 *)(AD1+3)=0x23;        	//#
	*(U8 *)(AD1+4)=0x57;			//W
	*(U8 *)(AD1+5)=0x43;   			//C	
	*(U8 *)(AD1+6)=0x53;			//S
	*(U8 *)(AD1+7)=add;				//寄存器标示;	
	*(U8 *)(AD1+8)=asicc(a1);		//寄存器地址
	*(U8 *)(AD1+9)=asicc(a2);	
	*(U8 *)(AD1+10)=asicc(a3);
	*(U8 *)(AD1+11)=asicc(a4);

	if (ptcom->writeValue==1)		//置位
	{
		*(U8 *)(AD1+12)=0x31;
	}
	else if (ptcom->writeValue==0)	//复位0x0000
	{
		*(U8 *)(AD1+12)=0x30;
	}
	aakj=CalXor((U8 *)AD1,13);          //异或校验，高位在前，低位在后         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+13)=asicc(a1);  //asicc码显示
	*(U8 *)(AD1+14)=asicc(a2);
	*(U8 *)(AD1+15)=0x0d;       //结束字元0d
	
	ptcom->send_length[0]=16;				//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址	
	ptcom->send_times=1;					//发送次数
		
	ptcom->return_length[0]=9;				//返回数据长度
	ptcom->return_start[0]=0;				//返回数据有效开始
	ptcom->return_length_available[0]=0;	//返回有效数据长度	
	ptcom->Current_Times=0;					//当前发送次数			
}


void Read_Bool()				//读取数字量的信息
{
	U16 aakj;
	int b,t;
	int a1,a2,a3,a4;
	int add;
	int b1,b2;
	int t1,t2,t3,t4;
	int plcadd;
	int len;
	int actlen;
	
	plcadd=ptcom->plc_address;	            //PLC站地址	
	b=ptcom->address;						// 开始读取地址
	b=b/16;									//松下地址转换
	len=ptcom->register_length;				// 开始读取长度
		
	switch (ptcom->registerr)	//根据寄存器类型获得偏移地址
	{
	case 'X':
		add=0x58;
		break;	
	case 'Y':
		add=0x59;
		break;
	case 'M':
		add=0x52;
		break;
	case 'L':
		add=0x4c;
		break;			
	case 'T':
		add=0x54;
		break;
	case 'C':
		add=0x43;
		break;					
	}
	b1=(plcadd>>4)&0xf;		//站地址
	b2=(plcadd>>0)&0xf;	
		
	a1=(b/1000);		//寄存器开始地址
	a2=((b%1000)/100);	//寄存器开始地址
	a3=((b%100)/10);	//寄存器开始地址
	a4=((b%10));		//寄存器开始地址,松下一次最少要读1个字节数据，所以最低位要为0
	
	/*if (len%2==0)
	{
		actlen=len/2;
	}
	else
	{
		actlen=len/2+1;
	}*/
	actlen=len/2+1;
	
	t=b+actlen-1;		//结束地址
	t1=(t/1000);		//寄存器开始地址
	t2=((t%1000)/100);	//寄存器开始地址
	t3=((t%100)/10);	//寄存器开始地址
	t4=((t%10));		//寄存器开始地址,松下一次最少要读1个字节数据，所以最低位要为0	
	
	
	*(U8 *)(AD1+0)=0x25;        	//松下开始代码
	*(U8 *)(AD1+1)=asicc(b1);        //plc站地址01，asicc码是0x30和0x31
	*(U8 *)(AD1+2)=asicc(b2);
	*(U8 *)(AD1+3)=0x23;   			//#     	
	*(U8 *)(AD1+4)=0x52;			//R
	*(U8 *)(AD1+5)=0x43;   			//C
	*(U8 *)(AD1+6)=0x43;			//C
	*(U8 *)(AD1+7)=add;		//寄存器标示;	
	*(U8 *)(AD1+8)=asicc(a1);		//寄存器开始地址
	*(U8 *)(AD1+9)=asicc(a2);	
	*(U8 *)(AD1+10)=asicc(a3);
	*(U8 *)(AD1+11)=asicc(a4);
	*(U8 *)(AD1+12)=asicc(t1);		//寄存器结束地址
	*(U8 *)(AD1+13)=asicc(t2);	
	*(U8 *)(AD1+14)=asicc(t3);
	*(U8 *)(AD1+15)=asicc(t4);	

	aakj=CalXor((U8 *)AD1,16);          //异或校验，高位在前，低位在后         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+16)=asicc(a1);  //asicc码显示
	*(U8 *)(AD1+17)=asicc(a2);
	*(U8 *)(AD1+18)=0x0d;       //结束字元0d
	
	
	ptcom->send_length[0]=19;				//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址	
	ptcom->send_times=1;					//发送次数
		
	ptcom->return_length[0]=9+actlen*4;//返回数据长度，有9个固定，
	                                                    //位数显示2个长度，校检2个长度，0d，0a
	ptcom->return_start[0]=6;				//返回数据有效开始
	ptcom->return_length_available[0]=actlen*4;	//返回有效数据长度	
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->send_add[0]=b<<4;		//读的是这个地址的数据
	ptcom->address=b<<4;	
	ptcom->register_length=actlen*2;			//字节单位
	ptcom->send_staradd[99] = 0;
}

void readBit()				//读取数字量的信息
{
	U16 aakj;
	int b,t;
	int a1,a2,a3,a4,a5;
	int add,f1,f2;
	int b1,b2;
	int t1,t2,t3,t4,t5;
	int plcadd;
	int len,sendlength;
	
	plcadd=ptcom->plc_address;	            //PLC站地址	
	b=ptcom->address;						// 开始读取地址
	len=ptcom->register_length/2 + 1;				// 开始读取长度
	
	ptcom->address = (b/16) * 16;	
	switch (ptcom->registerr)	//根据寄存器类型获得偏移地址
	{
	 case 'H':          //DT_Bit
		add=0x44;
		f1=0x52;
		f2=0x44;
		break;
	 case 'K':          //WL_Bit
		f1=0x52;        //R
		f2=0x43;        //C
	    add=0x43;       //C
	    break;
	}
	b1=(plcadd>>4)&0xf;		//站地址
	b2=(plcadd>>0)&0xf;	
	
	*(U8 *)(AD1+0)=0x25;        	//松下开始代码
	*(U8 *)(AD1+1)=asicc(b1);        //plc站地址01，asicc码是0x30和0x31
	*(U8 *)(AD1+2)=asicc(b2);
	*(U8 *)(AD1+3)=0x23;   			//#    	
	*(U8 *)(AD1+4)=f1;			
	*(U8 *)(AD1+5)=f2; 
	  			
    if(ptcom->registerr=='H')             //DT_Bit
    {
		*(U8 *)(AD1+6)=add;
		b/=16;
		a1=b/10000;
		a2=b/1000-a1*10;
		a3=b/100-a1*100-a2*10;
		a4=b/10-a1*1000-a2*100-a3*10;
		a5=b-a1*10000-a2*1000-a3*100-a4*10;

		t=b+len-1;			              //结束地址
		t1=t/10000;
		t2=t/1000-t1*10;
		t3=t/100-t1*100-t2*10;
		t4=t/10-t1*1000-t2*100-t3*10;
		t5=t-t1*10000-t2*1000-t3*100-t4*10;
				
		*(U8 *)(AD1+7)=asicc(a1);		//寄存器开始地址
		*(U8 *)(AD1+8)=asicc(a2);	
		*(U8 *)(AD1+9)=asicc(a3);
		*(U8 *)(AD1+10)=asicc(a4);	
		*(U8 *)(AD1+11)=asicc(a5);	

		*(U8 *)(AD1+12)=asicc(t1);		//寄存器结束地址
		*(U8 *)(AD1+13)=asicc(t2);	
		*(U8 *)(AD1+14)=asicc(t3);
		*(U8 *)(AD1+15)=asicc(t4);	
		*(U8 *)(AD1+16)=asicc(t5);			
						
		aakj=CalXor((U8 *)AD1,17);          //异或校验，高位在前，低位在后         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+17)=asicc(a1);  //asicc码显示
		*(U8 *)(AD1+18)=asicc(a2);
		*(U8 *)(AD1+19)=0x0d;       //结束字元0d	
		sendlength=20;	
	}	
	else if(ptcom->registerr=='K')  //WL_Bit
	{
	    *(U8 *)(AD1+6)=add;
	    *(U8 *)(AD1+7)=0x4c; 
	    b/=16;                       //开始地址
		a1=b/1000;
		a2=b/100-a1*10;
		a3=b/10-a1*100-a2*10;
		a4=b/1-a1*1000-a2*100-a3*10;

		t=b+len-1;			           //结束地址
		t1=t/1000;
		t2=t/100-t1*10;
		t3=t/10-t1*100-t2*10;
		t4=t/1-t1*1000-t2*100-t3*10;

		*(U8 *)(AD1+8)=asicc(a1);		//寄存器开始地址
		*(U8 *)(AD1+9)=asicc(a2);	
		*(U8 *)(AD1+10)=asicc(a3);
		*(U8 *)(AD1+11)=asicc(a4);	
		
		*(U8 *)(AD1+12)=asicc(t1);		//寄存器结束地址
		*(U8 *)(AD1+13)=asicc(t2);	
		*(U8 *)(AD1+14)=asicc(t3);
		*(U8 *)(AD1+15)=asicc(t4);			
				
		aakj=CalXor((U8 *)AD1,16);          //异或校验，高位在前，低位在后         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+16)=asicc(a1);  //asicc码显示
		*(U8 *)(AD1+17)=asicc(a2);
		*(U8 *)(AD1+18)=0x0d;       //结束字元0d	
		sendlength=19;		
	}								

	ptcom->send_length[0]=sendlength;				//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址	
	ptcom->send_times=1;					//发送次数
		
	ptcom->return_length[0]=9+len*4;//返回数据长度，有9个固定，
	                                                    //位数显示2个长度，校检2个长度，0d，0a
	ptcom->return_start[0]=6;				//返回数据有效开始
	ptcom->return_length_available[0]=len*4;	//返回有效数据长度	
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->send_add[0]=ptcom->address;		//读的是这个地址的数据
	ptcom->send_staradd[99] = 0;
}


void Read_Analog()				           //读模拟量
{
	U16 aakj;
	int b,t;
	int a1,a2,a3,a4,a5;
	int add,f1,f2;
	int b1,b2;
	int t1,t2,t3,t4,t5;
	int plcadd;
	int len,sendlength;
	int i;
	
	plcadd=ptcom->plc_address;	            //PLC站地址	
	b=ptcom->address;						// 开始读取地址
	len=ptcom->register_length;				// 开始读取长度
		
	switch (ptcom->registerr)	//根据寄存器类型获得偏移地址
	{
	case 'x':          //WX值
		add=0x58;		//X
		f1=0x52;		//R
		f2=0x43;		//C
		break;
	case 'y':          //WY值
		add=0x59;		//Y
		f1=0x52;		//R
		f2=0x43;		//C
		break;
	case 'R':         	//WR
		add=0x52;		//R	
		f1=0x52;		//R
		f2=0x43;		//C
	    break;	
	case 'l':         	//WL
		add=0x4c;		//L	
		f1=0x52;		//R
		f2=0x43;		//C
	    break;		
	case 'N':          //LD值
		add=0x4c;		//L
		f1=0x52;		//R
		f2=0x44;		//D
		break;
	 case 'D':          //DT值
		add=0x44;		//D
		f1=0x52;		//R
		f2=0x44;		//D
		break;	
	case 'c':			//SV,设定值
		add=0x53;		//K
		f1=0x52;		//R
		f2=0x53;		//S
		break;	
	case 't':			//EV,经过值
		add=0x4b;		//K
		f1=0x52;		//R
		f2=0x4b;		//K
		break;

					
	}
	b1=(plcadd>>4)&0xf;		//站地址
	b2=(plcadd>>0)&0xf;	
	
		
	*(U8 *)(AD1+0)=0x25;        	//松下开始代码
	*(U8 *)(AD1+1)=asicc(b1);   	//plc站地址01，asicc码是0x30和0x31
	*(U8 *)(AD1+2)=asicc(b2);
	*(U8 *)(AD1+3)=0x23;   		//# 
	*(U8 *)(AD1+4)=f1;			
	*(U8 *)(AD1+5)=f2;   
	
	if (ptcom->registerr=='D'|| ptcom->registerr=='N')
	{
		*(U8 *)(AD1+6)=add;		//D ,L
		a1=b/10000;
		a2=b/1000-a1*10;
		a3=b/100-a1*100-a2*10;
		a4=b/10-a1*1000-a2*100-a3*10;
		a5=b-a1*10000-a2*1000-a3*100-a4*10;
		
		t=b+len-1;			//结束地址
		t1=t/10000;
		t2=t/1000-t1*10;
		t3=t/100-t1*100-t2*10;
		t4=t/10-t1*1000-t2*100-t3*10;
		t5=t-t1*10000-t2*1000-t3*100-t4*10;
				
		*(U8 *)(AD1+7)=asicc(a1);		//寄存器开始地址
		*(U8 *)(AD1+8)=asicc(a2);	
		*(U8 *)(AD1+9)=asicc(a3);
		*(U8 *)(AD1+10)=asicc(a4);	
		*(U8 *)(AD1+11)=asicc(a5);	
		
		*(U8 *)(AD1+12)=asicc(t1);		//寄存器结束地址
		*(U8 *)(AD1+13)=asicc(t2);	
		*(U8 *)(AD1+14)=asicc(t3);
		*(U8 *)(AD1+15)=asicc(t4);	
		*(U8 *)(AD1+16)=asicc(t5);			
						
		aakj=CalXor((U8 *)AD1,17);    //异或校验，高位在前，低位在后         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+17)=asicc(a1);  //asicc码显示
		*(U8 *)(AD1+18)=asicc(a2);
		*(U8 *)(AD1+19)=0x0d;       //结束字元0d	
		sendlength=20;										
	}
	
	else if (ptcom->registerr=='t' || ptcom->registerr=='c')			//EV SV  X Y R L
	{
		a1=b/1000;
		a2=b/100-a1*10;
		a3=b/10-a1*100-a2*10;
		a4=b/1-a1*1000-a2*100-a3*10;

		t=b+len-1;			//结束地址
		t1=t/1000;
		t2=t/100-t1*10;
		t3=t/10-t1*100-t2*10;
		t4=t/1-t1*1000-t2*100-t3*10;

		*(U8 *)(AD1+6)=asicc(a1);		//寄存器开始地址
		*(U8 *)(AD1+7)=asicc(a2);	
		*(U8 *)(AD1+8)=asicc(a3);
		*(U8 *)(AD1+9)=asicc(a4);	
		
		*(U8 *)(AD1+10)=asicc(t1);		//寄存器结束地址
		*(U8 *)(AD1+11)=asicc(t2);	
		*(U8 *)(AD1+12)=asicc(t3);
		*(U8 *)(AD1+13)=asicc(t4);			
				
		aakj=CalXor((U8 *)AD1,14);    //异或校验，高位在前，低位在后         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+14)=asicc(a1);  //asicc码显示
		*(U8 *)(AD1+15)=asicc(a2);
		*(U8 *)(AD1+16)=0x0d;       //结束字元0d	
		sendlength=17;											
	}
	else if(ptcom->registerr=='R'||ptcom->registerr=='x'|| ptcom->registerr=='y'|| ptcom->registerr=='l' )
	{
	   *(U8 *)(AD1+6)=0x43;  //C
	   *(U8 *)(AD1+7)=add; 
		a1=b/1000;
		a2=b/100-a1*10;
		a3=b/10-a1*100-a2*10;
		a4=b/1-a1*1000-a2*100-a3*10;

		t=b+len-1;			//结束地址
		t1=t/1000;
		t2=t/100-t1*10;
		t3=t/10-t1*100-t2*10;
		t4=t/1-t1*1000-t2*100-t3*10;

		*(U8 *)(AD1+8)=asicc(a1);		//寄存器开始地址
		*(U8 *)(AD1+9)=asicc(a2);	
		*(U8 *)(AD1+10)=asicc(a3);
		*(U8 *)(AD1+11)=asicc(a4);	
		
		*(U8 *)(AD1+12)=asicc(t1);		//寄存器结束地址
		*(U8 *)(AD1+13)=asicc(t2);	
		*(U8 *)(AD1+14)=asicc(t3);
		*(U8 *)(AD1+15)=asicc(t4);			
				
		aakj=CalXor((U8 *)AD1,16);          //异或校验，高位在前，低位在后         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+16)=asicc(a1);  //asicc码显示
		*(U8 *)(AD1+17)=asicc(a2);
		*(U8 *)(AD1+18)=0x0d;       //结束字元0d	
		sendlength=19;								
	}

	ptcom->send_length[0]=sendlength;				//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址	
	ptcom->send_times=1;					//发送次数
		
	ptcom->return_length[0]=9+len*4;//返回数据长度，有9个固定，
	                                                    //位数显示2个长度，校检2个长度，0d，0a
	ptcom->return_start[0]=6;				//返回数据有效开始
	ptcom->return_length_available[0]=len*4;	//返回有效数据长度	
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->send_add[0]=ptcom->address;		//读的是这个地址的数据
	ptcom->send_staradd[99] = 0;
}


void Read_Recipe()								//读取配方
{
	U16 aakj;
	int b,t,i;
	int a1,a2,a3,a4,a5;
	int b1,b2;
	int t1,t2,t3,t4,t5;
	int plcadd,ps,staradd,SendTimes,LastTimeWord;
	int sendlength,datalength,length;
	
	plcadd=ptcom->plc_address;	            //PLC站地址	
	b=ptcom->address;						// 开始读取地址
	staradd=b;
	datalength=ptcom->register_length;				// 开始读取长度
	
	if(datalength%24==0)                       //松下24个D
	{
		SendTimes=datalength/24;               //发送的次数为datalength/24              
		LastTimeWord=24;                       //最后一次发送的长度为24个D
	}
	if(datalength%24!=0)                       //数据长度不是24的倍数时
	{
		SendTimes=datalength/24+1;             //发送的次数datalength/24+1
		LastTimeWord=datalength%24;            //最后一次发送的长度为除24的余数
	}
		

	b1=(plcadd>>4)&0xf;		//站地址
	b2=(plcadd>>0)&0xf;	
	ps=20;
	for (i=0;i<SendTimes;i++)
	{		
		*(U8 *)(AD1+0+ps*i)=0x25;        	//松下开始代码
		*(U8 *)(AD1+1+ps*i)=asicc(b1);        //plc站地址01，asicc码是0x30和0x31
		*(U8 *)(AD1+2+ps*i)=asicc(b2);
		*(U8 *)(AD1+3+ps*i)=0x23;   			//#    	
		*(U8 *)(AD1+4+ps*i)=0x52;			
		*(U8 *)(AD1+5+ps*i)=0x44;   			
		*(U8 *)(AD1+6+ps*i)=0x44;
		
		b=staradd+i*24;                        //起始地址
		a1=b/10000;
		a2=b/1000-a1*10;
		a3=b/100-a1*100-a2*10;
		a4=b/10-a1*1000-a2*100-a3*10;
		a5=b-a1*10000-a2*1000-a3*100-a4*10;
		
		if (i!=(SendTimes-1))			//不是最后一次
		{
			sendlength=24;
		}
		else
		{
			sendlength=LastTimeWord;
		}
		
		t=b+sendlength-1;			//结束地址

		t1=t/10000;
		t2=t/1000-t1*10;
		t3=t/100-t1*100-t2*10;
		t4=t/10-t1*1000-t2*100-t3*10;
		t5=t-t1*10000-t2*1000-t3*100-t4*10;
				
		*(U8 *)(AD1+7+ps*i)=asicc(a1);		//寄存器开始地址
		*(U8 *)(AD1+8+ps*i)=asicc(a2);	
		*(U8 *)(AD1+9+ps*i)=asicc(a3);
		*(U8 *)(AD1+10+ps*i)=asicc(a4);	
		*(U8 *)(AD1+11+ps*i)=asicc(a5);	
		
		*(U8 *)(AD1+12+ps*i)=asicc(t1);		//寄存器开始地址
		*(U8 *)(AD1+13+ps*i)=asicc(t2);	
		*(U8 *)(AD1+14+ps*i)=asicc(t3);
		*(U8 *)(AD1+15+ps*i)=asicc(t4);	
		*(U8 *)(AD1+16+ps*i)=asicc(t5);			
						
		aakj=CalXor((U8 *)AD1+ps*i,17);          //异或校验，高位在前，低位在后         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+17+ps*i)=asicc(a1);  //asicc码显示
		*(U8 *)(AD1+18+ps*i)=asicc(a2);
		*(U8 *)(AD1+19+ps*i)=0x0d;       //结束字元0d	
									

		ptcom->send_length[i]=20;				//发送长度
		ptcom->send_staradd[i]=ps*i;				//发送数据存储地址	
			
		ptcom->return_length[i]=9+sendlength*4;//返回数据长度，有9个固定，
		                                                    //位数显示2个长度，校检2个长度，0d，0a
		ptcom->return_start[i]=6;				//返回数据有效开始
		ptcom->return_length_available[i]=sendlength*4;	//返回有效数据长度	
		ptcom->send_add[i]=b;		//读的是这个地址的数据
		ptcom->send_data_length[i]=sendlength;			//要求读的配方长度
	}	
	ptcom->send_times=SendTimes;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->send_staradd[99] = 0;
}

void SetBoolDTBit()    
{
	U16 aakj;
	int b,t;
	int a1,a2,a3,a4,a5;
	int b1,b2;
	int t1,t2,t3,t4,t5;
	int plcadd;
	int addr1,addr2,addr3,addr4,addr5;
	//U32 lw = *(U32*)(lw_word + 0);
	
	plcadd=ptcom->plc_address;	            //PLC站地址	
	b=ptcom->address;						// 置位地址
	
    //先读------------------------------------------
	b1=(plcadd>>4)&0xf;		//站地址
	b2=(plcadd>>0)&0xf;	
	
	*(U8 *)(AD1+0)=0x25;        	//松下开始代码
	*(U8 *)(AD1+1)=asicc(b1);        //plc站地址01，asicc码是0x30和0x31
	*(U8 *)(AD1+2)=asicc(b2);
	*(U8 *)(AD1+3)=0x23;   			//#    	
	*(U8 *)(AD1+4)=0x52;			//R
	*(U8 *)(AD1+5)=0x44;  			//D
	*(U8 *)(AD1+6)=0x44;            //D
	
	b=b/16;
	a1=b/10000;
	a2=b/1000-a1*10;
	a3=b/100-a1*100-a2*10;
	a4=b/10-a1*1000-a2*100-a3*10;
	a5=b-a1*10000-a2*1000-a3*100-a4*10;
	
	addr1 = a1;
	addr2 = a2;
	addr3 = a3;
	addr4 = a4;
	addr5 = a5;
	
	t=b;			               //结束地址
	t1=t/10000;
	t2=t/1000-t1*10;
	t3=t/100-t1*100-t2*10;
	t4=t/10-t1*1000-t2*100-t3*10;
	t5=t-t1*10000-t2*1000-t3*100-t4*10;
			
	*(U8 *)(AD1+7)=asicc(a1);		//寄存器开始地址
	*(U8 *)(AD1+8)=asicc(a2);	
	*(U8 *)(AD1+9)=asicc(a3);
	*(U8 *)(AD1+10)=asicc(a4);	
	*(U8 *)(AD1+11)=asicc(a5);	
	
	*(U8 *)(AD1+12)=asicc(t1);		//寄存器结束地址
	*(U8 *)(AD1+13)=asicc(t2);	
	*(U8 *)(AD1+14)=asicc(t3);
	*(U8 *)(AD1+15)=asicc(t4);	
	*(U8 *)(AD1+16)=asicc(t5);			
					
	aakj=CalXor((U8 *)AD1,17);     //异或校验，高位在前，低位在后         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+17)=asicc(a1);      //asicc码显示
	*(U8 *)(AD1+18)=asicc(a2);
	*(U8 *)(AD1+19)=0x0d;          //结束字元0d									
	
	ptcom->send_length[0]=20;		//发送长度
	ptcom->send_staradd[0]=0;		//发送数据存储地址	
		
	ptcom->return_length[0]=9+4;    //返回数据长度，有9个固定，
	                                //位数显示2个长度，校检2个长度，0d，0a
	ptcom->return_start[0]=6;				//返回数据有效开始
	ptcom->return_length_available[0]=4;	//返回有效数据长度	

	//再写------------------------------------------
	*(U8 *)(AD1+20)=0x25;        	//松下开始代码
	*(U8 *)(AD1+21)=asicc(b1);        //plc站地址01，asicc码是0x30和0x31
	*(U8 *)(AD1+22)=asicc(b2);
	*(U8 *)(AD1+23)=0x23;   			//#    	
	*(U8 *)(AD1+24)=0x57;			    //W
	*(U8 *)(AD1+25)=0x44;   			//D
	*(U8 *)(AD1+26)=0x44;               //D
		
	*(U8 *)(AD1+27)=asicc(addr1);		//寄存器开始地址
	*(U8 *)(AD1+28)=asicc(addr2);	
	*(U8 *)(AD1+29)=asicc(addr3);
	*(U8 *)(AD1+30)=asicc(addr4);	
	*(U8 *)(AD1+31)=asicc(addr5);	
	
	*(U8 *)(AD1+32)=asicc(addr1);		//寄存器结束地址
	*(U8 *)(AD1+33)=asicc(addr2);	
	*(U8 *)(AD1+34)=asicc(addr3);
	*(U8 *)(AD1+35)=asicc(addr4);	
	*(U8 *)(AD1+36)=asicc(addr5);	

	
	a1=(0x00&0xf0)>>4;
	a2=0x00&0xf;	
	*(U8 *)(AD1+37)=asicc(a1);
	*(U8 *)(AD1+38)=asicc(a2);
	*(U8 *)(AD1+39)=asicc(a1);
	*(U8 *)(AD1+40)=asicc(a2);
			
	aakj=CalXor((U8 *)(AD1+20),21);          //异或校验，高位在前，低位在后         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+41)=asicc(a1);             //asicc码显示
	*(U8 *)(AD1+42)=asicc(a2);
	*(U8 *)(AD1+43)=0x0d;                   //结束字元0d	
									
	ptcom->send_length[1]=24;		        //发送长度
	ptcom->send_staradd[1]=20;				//发送数据存储地址	

	ptcom->return_length[1]=9;			    //返回数据长度，有9个固定
	ptcom->return_start[1]=0;				//返回数据有效开始
	ptcom->return_length_available[1]=0;	//返回有效数据长度
	
	ptcom->send_times=2;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数	 
	
	ptcom->send_add[0]=b*16;		       //读的是这个地址的数据
	
	if (ptcom->writeValue==1)	    	  //置位
	{
		ptcom->send_staradd[99]=3;			
	}
	if (ptcom->writeValue==0)	    	   //复位
	{
		ptcom->send_staradd[99]=4;
	}
}


void SetBoolWLBit()
{
	U16 aakj;
	int b;
	int a1,a2,a3,a4;
	int b1,b2;
	int plcadd;
	int addr1,addr2,addr3,addr4;
	
	plcadd=ptcom->plc_address;	            //PLC站地址	
	b=ptcom->address;						// 置位地址
	
    //先读------------------------------------------
	b1=(plcadd>>4)&0xf;		//站地址
	b2=(plcadd>>0)&0xf;	
	
	*(U8 *)(AD1+0)=0x25;        	//松下开始代码
	*(U8 *)(AD1+1)=asicc(b1);        //plc站地址01，asicc码是0x30和0x31
	*(U8 *)(AD1+2)=asicc(b2);
	*(U8 *)(AD1+3)=0x23;   			//#    	
	*(U8 *)(AD1+4)=0x52;			//R
	*(U8 *)(AD1+5)=0x43;  			//C
	*(U8 *)(AD1+6)=0x43;            //C
	*(U8 *)(AD1+7)=0x4c;            //L
	
	b=b/16;
	a1=b/1000;
	a2=b/100-a1*10;
	a3=b/10-a1*100-a2*10;
	a4=b/1-a1*1000-a2*100-a3*10;
	
	addr1 = a1;
	addr2 = a2;
	addr3 = a3;
	addr4 = a4;
			
	*(U8 *)(AD1+8)=asicc(addr1);		//寄存器开始地址
	*(U8 *)(AD1+9)=asicc(addr2);	
	*(U8 *)(AD1+10)=asicc(addr3);
	*(U8 *)(AD1+11)=asicc(addr4);		
	
	*(U8 *)(AD1+12)=asicc(addr1);		//寄存器结束地址
	*(U8 *)(AD1+13)=asicc(addr2);	
	*(U8 *)(AD1+14)=asicc(addr3);
	*(U8 *)(AD1+15)=asicc(addr4);				
					
	aakj=CalXor((U8 *)AD1,16);     //异或校验，高位在前，低位在后         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+16)=asicc(a1);      //asicc码显示
	*(U8 *)(AD1+17)=asicc(a2);
	*(U8 *)(AD1+18)=0x0d;          //结束字元0d									
	
	ptcom->send_length[0]=19;		//发送长度
	ptcom->send_staradd[0]=0;		//发送数据存储地址	
		
	ptcom->return_length[0]=9+4;    //返回数据长度，有9个固定，
	                                //位数显示2个长度，校检2个长度，0d，0a
	ptcom->return_start[0]=6;				//返回数据有效开始
	ptcom->return_length_available[0]=4;	//返回有效数据长度	

	//再写------------------------------------------
	*(U8 *)(AD1+19)=0x25;        	//松下开始代码
	*(U8 *)(AD1+20)=asicc(b1);        //plc站地址01，asicc码是0x30和0x31
	*(U8 *)(AD1+21)=asicc(b2);
	*(U8 *)(AD1+22)=0x23;   			//#    	
	*(U8 *)(AD1+23)=0x57;			    //W
	*(U8 *)(AD1+24)=0x43;   			//C
	*(U8 *)(AD1+25)=0x43;               //C
	*(U8 *)(AD1+26)=0x4c;               //L
		
	*(U8 *)(AD1+27)=asicc(addr1);		//寄存器开始地址
	*(U8 *)(AD1+28)=asicc(addr2);	
	*(U8 *)(AD1+29)=asicc(addr3);
	*(U8 *)(AD1+30)=asicc(addr4);	

	*(U8 *)(AD1+31)=asicc(addr1);		//寄存器结束地址
	*(U8 *)(AD1+32)=asicc(addr2);	
	*(U8 *)(AD1+33)=asicc(addr3);
	*(U8 *)(AD1+34)=asicc(addr4);	

	a1=(0x00&0xf0)>>4;
	a2=0x00&0xf;	
	*(U8 *)(AD1+35)=asicc(a1);
	*(U8 *)(AD1+36)=asicc(a2);
	*(U8 *)(AD1+37)=asicc(a1);
	*(U8 *)(AD1+38)=asicc(a2);
			
	aakj=CalXor((U8 *)(AD1+19),20);          //异或校验，高位在前，低位在后         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+39)=asicc(a1);             //asicc码显示
	*(U8 *)(AD1+40)=asicc(a2);
	*(U8 *)(AD1+41)=0x0d;                   //结束字元0d	
									
	ptcom->send_length[1]=23;		        //发送长度
	ptcom->send_staradd[1]=19;				//发送数据存储地址	

	ptcom->return_length[1]=9;			    //返回数据长度，有9个固定
	ptcom->return_start[1]=0;				//返回数据有效开始
	ptcom->return_length_available[1]=0;	//返回有效数据长度
	
	ptcom->send_times=2;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数	 
	
	ptcom->send_add[0]=b*16;		       //读的是这个地址的数据
	
	if (ptcom->writeValue==1)	    	  //置位
	{
		ptcom->send_staradd[99]=5;			
	}
	if (ptcom->writeValue==0)	    	   //复位
	{
		ptcom->send_staradd[99]=6;
	}
}

void Write_Analog()					      //写模拟量
{
	U16 aakj;
	int b,t,i,k;
	int a1,a2,a3,a4,a5;
	int add,f1,f2;
	int b1,b2;
	int t1,t2,t3,t4,t5;
	int plcadd;
	int len,sendlength;
	
	plcadd=ptcom->plc_address;	            //PLC站地址	
	b=ptcom->address;						// 开始读取地址
	len=ptcom->register_length;				// 开始读取长度
		
	switch (ptcom->registerr)	//根据寄存器类型获得偏移地址
	{
	case 'y':          //WY值
		add=0x59;		//Y
		f1=0x57;		//W
		f2=0x43;		//C
		break;
	case 'R':         	//WR
		add=0x52;		//R	
		f1=0x57;		//W
		f2=0x43;		//C
	    break;	
	case 'l':          //WL值
		add=0x4c;		//L
		f1=0x57;		//W
		f2=0x43;		//C
		break;
	case 'N':          //LD值
		add=0x4c;		//L
		f1=0x57;		//W
		f2=0x44;		//D
		break;
	case 'D':			//DT
		add=0x44;		//D
		f1=0x57;		//W
		f2=0x44;		//D
		break;	
	case 'c':			//SV，目标值
		add=0x53;		//S
		f1=0x57;		//W
		f2=0x53;		//S
		break;	
	case 't':			//EV,经过值
		add=0x4b;		//K
		f1=0x57;		//W
		f2=0x4b;		//K
		break;		
	}
	b1=(plcadd>>4)&0xf;		//站地址
	b2=(plcadd>>0)&0xf;	
	
	*(U8 *)(AD1+0)=0x25;       //松下开始代码
	*(U8 *)(AD1+1)=asicc(b1);  //plc站地址01，asicc码是0x30和0x31
	*(U8 *)(AD1+2)=asicc(b2);
	*(U8 *)(AD1+3)=0x23;   	//#    	
	*(U8 *)(AD1+4)=f1;			
	*(U8 *)(AD1+5)=f2;   		
	
	if (ptcom->registerr=='D'|| ptcom->registerr=='N')
	{
		*(U8 *)(AD1+6)=add;  //D　，Ｌ
		a1=b/10000;
		a2=b/1000-a1*10;
		a3=b/100-a1*100-a2*10;
		a4=b/10-a1*1000-a2*100-a3*10;
		a5=b-a1*10000-a2*1000-a3*100-a4*10;
		
		t=b+len-1;			//结束地址
		t1=t/10000;
		t2=t/1000-t1*10;
		t3=t/100-t1*100-t2*10;
		t4=t/10-t1*1000-t2*100-t3*10;
		t5=t-t1*10000-t2*1000-t3*100-t4*10;
				
		*(U8 *)(AD1+7)=asicc(a1);		//寄存器开始地址
		*(U8 *)(AD1+8)=asicc(a2);	
		*(U8 *)(AD1+9)=asicc(a3);
		*(U8 *)(AD1+10)=asicc(a4);	
		*(U8 *)(AD1+11)=asicc(a5);	
		
		*(U8 *)(AD1+12)=asicc(t1);		//寄存器结束地址
		*(U8 *)(AD1+13)=asicc(t2);	
		*(U8 *)(AD1+14)=asicc(t3);
		*(U8 *)(AD1+15)=asicc(t4);	
		*(U8 *)(AD1+16)=asicc(t5);	
		
		k=0;
		for (i=0;i<len*2;i++)
		{
			a1=((ptcom->U8_Data[i])&0xf0)>>4;
			a2=(ptcom->U8_Data[i])&0xf;	
			*(U8 *)(AD1+17+k)=asicc(a1);
			*(U8 *)(AD1+18+k)=asicc(a2);		
			k=k+2;				
		}
				
		aakj=CalXor((U8 *)AD1,17+len*4);          //异或校验，高位在前，低位在后         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+17+len*4)=asicc(a1);  //asicc码显示
		*(U8 *)(AD1+18+len*4)=asicc(a2);
		*(U8 *)(AD1+19+len*4)=0x0d;       //结束字元0d	
		sendlength=20+len*4;										
	}
	else if (ptcom->registerr=='t' || ptcom->registerr=='c')
	{
		a1=b/1000;
		a2=b/100-a1*10;
		a3=b/10-a1*100-a2*10;
		a4=b/1-a1*1000-a2*100-a3*10;

		t=b+len-1;			          	//结束地址
		t1=t/1000;
		t2=t/100-t1*10;
		t3=t/10-t1*100-t2*10;
		t4=t/1-t1*1000-t2*100-t3*10;

		*(U8 *)(AD1+6)=asicc(a1);		//寄存器开始地址
		*(U8 *)(AD1+7)=asicc(a2);	
		*(U8 *)(AD1+8)=asicc(a3);
		*(U8 *)(AD1+9)=asicc(a4);	
		
		*(U8 *)(AD1+10)=asicc(t1);		//寄存器结束地址
		*(U8 *)(AD1+11)=asicc(t2);	
		*(U8 *)(AD1+12)=asicc(t3);
		*(U8 *)(AD1+13)=asicc(t4);	
		
		k=0;
		for (i=0;i<len*2;i++)
		{
			a1=((ptcom->U8_Data[i])&0xf0)>>4;
			a2=(ptcom->U8_Data[i])&0xf;	
			*(U8 *)(AD1+14+k)=asicc(a1);
			*(U8 *)(AD1+15+k)=asicc(a2);		
			k=k+2;				
		}		
				
		aakj=CalXor((U8 *)AD1,14+len*4);  //异或校验，高位在前，低位在后         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+14+len*4)=asicc(a1);  //asicc码显示
		*(U8 *)(AD1+15+len*4)=asicc(a2);
		*(U8 *)(AD1+16+len*4)=0x0d;       //结束字元0d	
		sendlength=17+len*4;											
	}	
	else if(ptcom->registerr=='R'|| ptcom->registerr=='y'|| ptcom->registerr=='l' )
	{
	   *(U8 *)(AD1+6)=0x43;   	//C
	   *(U8 *)(AD1+7)=add; 		
	   
		a1=b/1000;
		a2=b/100-a1*10;
		a3=b/10-a1*100-a2*10;
		a4=b/1-a1*1000-a2*100-a3*10;

		t=b+len-1;			           //结束地址
		t1=t/1000;
		t2=t/100-t1*10;
		t3=t/10-t1*100-t2*10;
		t4=t/1-t1*1000-t2*100-t3*10;

		*(U8 *)(AD1+8)=asicc(a1);		//寄存器开始地址
		*(U8 *)(AD1+9)=asicc(a2);	
		*(U8 *)(AD1+10)=asicc(a3);
		*(U8 *)(AD1+11)=asicc(a4);	
		
		*(U8 *)(AD1+12)=asicc(t1);		//寄存器结束地址
		*(U8 *)(AD1+13)=asicc(t2);	
		*(U8 *)(AD1+14)=asicc(t3);
		*(U8 *)(AD1+15)=asicc(t4);			
				
		k=0;
		for (i=0;i<len*2;i++)
		{
			a1=((ptcom->U8_Data[i])&0xf0)>>4;
			a2=(ptcom->U8_Data[i])&0xf;	
			*(U8 *)(AD1+16+k)=asicc(a1);
			*(U8 *)(AD1+17+k)=asicc(a2);		
			k=k+2;				
		}		
				
		aakj=CalXor((U8 *)AD1,16+len*4);  //异或校验，高位在前，低位在后         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+16+len*4)=asicc(a1);  //asicc码显示
		*(U8 *)(AD1+17+len*4)=asicc(a2);
		*(U8 *)(AD1+18+len*4)=0x0d;       //结束字元0d	
		sendlength=19+len*4;						
	}	

	ptcom->send_length[0]=sendlength;				//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址	
	ptcom->send_times=1;					//发送次数
		
	ptcom->return_length[0]=9;				//返回数据长度，有9个固定，
	                                                    //位数显示2个长度，校检2个长度，0d，0a
	ptcom->return_start[0]=0;				//返回数据有效开始
	ptcom->return_length_available[0]=0;	//返回有效数据长度	
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->send_add[0]=ptcom->address;		//读的是这个地址的数据
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
	int i,j,t;
	int ps;
	int b;
	int a1,a2,a3,a4,a5;
	U16 aakj;
	int length;
	int plcadd;	
	int k3,k4,k5,k6,k7,k8;
	int t1,t2,t3,t4,t5;
	
	
	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));//数据长度
	staradd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//数据开始地址
	plcadd=*(U8 *)(PE+4);	                   //PLC站地址
	
	if(datalength%24==0)                       //松下24个D
	{
		SendTimes=datalength/24;               //发送的次数为datalength/24              
		LastTimeWord=24;                       //最后一次发送的长度为24个D
	}
	if(datalength%24!=0)                       //数据长度不是24D的倍数时
	{
		SendTimes=datalength/24+1;             //发送的次数datalength/24+1
		LastTimeWord=datalength%24;            //最后一次发送的长度为除24的余数
	}	
	
	ps=116;                                     
	
	for (i=0;i<SendTimes;i++)
	{   
		if (i!=(SendTimes-1))                  //不是最后一次发送时
		{	
			length=24;                         //发15个D
		}
		else                                   //最后一次发送时
		{
			length=LastTimeWord;               //发剩余的长度             
		}
		
		b=staradd+i*24;                        //起始地址
		
		a1=b/10000;
		a2=b/1000-a1*10;
		a3=b/100-a1*100-a2*10;
		a4=b/10-a1*1000-a2*100-a3*10;
		a5=b-a1*10000-a2*1000-a3*100-a4*10;
		
		t=b+length-1;			//结束地址
		t1=t/10000;
		t2=t/1000-t1*10;
		t3=t/100-t1*100-t2*10;
		t4=t/10-t1*1000-t2*100-t3*10;
		t5=t-t1*10000-t2*1000-t3*100-t4*10;
		
		
		*(U8 *)(AD1+0+ps*i)=0x25;              //台达起始字元3a
		*(U8 *)(AD1+1+ps*i)=asicc(((plcadd&0xf0)>>4)&0xf);//PLC站地址做asicc处理并显示asicc码，高位在前
		*(U8 *)(AD1+2+ps*i)=asicc(plcadd&0xf);
		*(U8 *)(AD1+3+ps*i)=0x23;              //功能码16，以asicc显示
		*(U8 *)(AD1+4+ps*i)=0x57;
		*(U8 *)(AD1+5+ps*i)=0x44;		
		*(U8 *)(AD1+6+ps*i)=0x44;				//D
		*(U8 *)(AD1+7+ps*i)=asicc(a1);         //开始地址，以asicc显示，依次从高位到低位
		*(U8 *)(AD1+8+ps*i)=asicc(a2);
		*(U8 *)(AD1+9+ps*i)=asicc(a3);
		*(U8 *)(AD1+10+ps*i)=asicc(a4);
		*(U8 *)(AD1+11+ps*i)=asicc(a5);		
		*(U8 *)(AD1+12+ps*i)=asicc(t1);         //开始地址，以asicc显示，依次从高位到低位
		*(U8 *)(AD1+13+ps*i)=asicc(t2);
		*(U8 *)(AD1+14+ps*i)=asicc(t3);
		*(U8 *)(AD1+15+ps*i)=asicc(t4);
		*(U8 *)(AD1+16+ps*i)=asicc(t5);			
		
						
		for(j=0;j<length;j++)                  //写入多个元件值                 
		{	
			k4=*(U8 *)(PE+9+i*48+j*2);         //从存数据的寄存器开始地址PE+9取数据，k3为高位，k4为低位
			k3=*(U8 *)(PE+9+i*48+j*2+1);
						
			k5=(k3&0xf0)>>4;                   //对取得的数据做asicc处理
			k6=k3&0xf;
			k7=(k4&0xf0)>>4;
			k8=k4&0xf;
			*(U8 *)(AD1+17+j*4+ps*i)=asicc(k7);//发送取得的数据，以显示asicc码，依次从低位到高位
			*(U8 *)(AD1+18+j*4+ps*i)=asicc(k8);
			*(U8 *)(AD1+19+j*4+ps*i)=asicc(k5);
			*(U8 *)(AD1+20+j*4+ps*i)=asicc(k6);								
		}
		aakj=CalXor((U8 *)AD1+ps*i,17+length*4);	           //计算和校检，LRC校验，高位在前，低位在后
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+17+length*4+ps*i)=asicc(a1);
		*(U8 *)(AD1+18+length*4+ps*i)=asicc(a2);
		*(U8 *)(AD1+19+length*4+ps*i)=0x0d;		//结束字元0d
	
		ptcom->send_length[i]=20+length*4;	   //发送长度
		ptcom->send_staradd[i]=i*ps;		   //发送数据存储地址	
		ptcom->return_length[i]=9;			   //返回数据长度
		ptcom->return_start[i]=0;			   //返回数据有效开始
		ptcom->return_length_available[i]=0;   //返回有效数据长度	
				
	}
	ptcom->send_times=SendTimes;			   //发送次数
	ptcom->Current_Times=0;					   //当前发送次数			
}


void compxy(void)				              //处理成标准存储格式,重新排列
{
	int i,Bit;
	unsigned short a1,a2,a3,a4;
	unsigned short b1,b2,b3,b4;
	int b;
	int Temp,Temp1;
	U16 R_CRC;
	
	if (ptcom->send_staradd[99] == 3 || ptcom->send_staradd[99] == 4) //WL_Bit做处理
	{	
        a3=*(U8 *)(COMad+6);					
		a4=*(U8 *)(COMad+7);
		a1=*(U8 *)(COMad+8);
		a2=*(U8 *)(COMad+9);
			
		a1 = bsicc(a1);
		a2 = bsicc(a2);
		a3 = bsicc(a3);
		a4 = bsicc(a4);	
		
		Temp=a4;
		Temp|=((a3<<4)&0x00f0);
		Temp|=((a2<<8)&0x0f00);
		Temp|=((a1<<12)&0xf000);
		
		Bit=ptcom->address%16;		//偏移多少位
		if(ptcom->send_staradd[99] == 3)
		{
			Temp1=1<<Bit;
			Temp|=Temp1;		
		}
		else if(ptcom->send_staradd[99] == 4)
		{
			Temp1=1<<Bit;
		    Temp1=~Temp1;
		    
		    Temp&=Temp1;	
		}
				
		a1=(Temp>>4)&0x0f;
		a2=Temp&0x0f;	
		*(U8 *)(AD1+37)=asicc(a1);
		*(U8 *)(AD1+38)=asicc(a2);
		
		a1=(Temp>>12)&0x0f;
		a2=(Temp>>8)&0x0f;			
		*(U8 *)(AD1+39)=asicc(a1);
		*(U8 *)(AD1+40)=asicc(a2);			
		
		R_CRC=CalXor((U8 *)(AD1+20),21);          //异或校验，高位在前，低位在后         
		a1=(R_CRC&0xf0)>>4;
		a1=a1&0xf;
		a2=R_CRC&0xf;
		*(U8 *)(AD1+41)=asicc(a1);  //asicc码显示
		*(U8 *)(AD1+42)=asicc(a2);
	}
	else if (ptcom->send_staradd[99] == 5|| ptcom->send_staradd[99] == 6)
	{	
        b3=*(U8 *)(COMad+6);					
		b4=*(U8 *)(COMad+7);
		b1=*(U8 *)(COMad+8);
		b2=*(U8 *)(COMad+9);
			
		b1 = bsicc(b1);
		b2 = bsicc(b2);
		b3 = bsicc(b3);
		b4 = bsicc(b4);	
		
		Temp=b4;
		Temp|=((b3<<4)&0x00f0);
		Temp|=((b2<<8)&0x0f00);
		Temp|=((b1<<12)&0xf000);
		
		Bit=ptcom->address%16;		//偏移多少位
		if(ptcom->send_staradd[99] == 5)
		{
			Temp1=1<<Bit;
			Temp|=Temp1;		
		}
		else if(ptcom->send_staradd[99] == 6)
		{
			Temp1=1<<Bit;
		    Temp1=~Temp1;
		    
		    Temp&=Temp1;	
		}
						
		b1=(Temp>>4)&0x0f;
		b2=Temp&0x0f;	
		*(U8 *)(AD1+35)=asicc(b1);
		*(U8 *)(AD1+36)=asicc(b2);
		
		b1=(Temp>>12)&0x0f;
		b2=(Temp>>8)&0x0f;			
		*(U8 *)(AD1+37)=asicc(b1);
		*(U8 *)(AD1+38)=asicc(b2);	
		


		
		R_CRC=CalXor((U8 *)(AD1+19),20);          //异或校验，高位在前，低位在后         
		b1=(R_CRC&0xf0)>>4;
		b1=b1&0xf;
		b2=R_CRC&0xf;
		*(U8 *)(AD1+39)=asicc(b1);  //asicc码显示
		*(U8 *)(AD1+40)=asicc(b2);
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
			*(U8 *)(COMad+i)=b;					 //重新存,从第0个开始存
		}
		
		ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;	//长度减半		
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
	unsigned int akj1,akj2;	
	aakj2=(*(U8 *)(COMad+ptcom->return_length[ptcom->Current_Times-1]-2))&0xff;	//在发送完后Current_Times++，此时要--
	aakj1=(*(U8 *)(COMad+ptcom->return_length[ptcom->Current_Times-1]-3))&0xff; //减后面的结束字元取校验码
	akj1=CalXor((U8 *)COMad,(ptcom->return_length[ptcom->Current_Times-1]-3));//对收到的数据进行校验，并转化成asicc码

	akj2=(akj1&0xf0)>>4;
	akj2=akj2&0xf;
	akj1=akj1&0xf;
	akj1=asicc(akj1);
	akj2=asicc(akj2);
	sysprintf("akj1 %d aakj2 %d,akj2 %d,aakj1 %d\n",akj1,aakj2,akj2,aakj1);
	if((akj1==aakj2)&&(akj2==aakj1))     //比较校验码，相等完毕，不相等继续取数据进行校验
		return 1;
	else
		return 0;	
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
