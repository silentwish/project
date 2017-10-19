#include "stdio.h"
#include "def.h"
#include "smm.h"
#include "math.h"

	
	
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
		case 'Y':
		case 'M':
		case 'K':
		case 'L':						
			Read_Bool();//读数字量
			break;
		case 'D':	
			Read_Analog();//读模拟量
			break;			
		}
		break;
	case PLC_WRITE_DATA:				//进入驱动是写数据
		switch(ptcom->registerr)
		{
		case 'M':
		case 'Y':
		case 'K':
		case 'L':				
			Set_Reset();//置位复位
			break;
		case 'D':
			Write_Analog();//写模拟量		
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
		switch(ptcom->registerr)//配方寄存器名称
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
	int b,i;
	int a1;
	int func1;
	unsigned int code1,code2,code3,code4,code;
	int b1,b2,b3;

	b=ptcom->address;			// 寄存器给的开始地址
	b1 = b/16;
	b2 = b%16;	
	b3 = b1 * 2;
	
	if (ptcom->writeValue==1)	//标示
	{
		func1=0x6f;
		code = 1;
		for (i=0;i<b2;i++)
		{
			code = code * 2;
		}
		code1 = asicc((code>>12)&0xf);	
		code2 = asicc((code>>8)&0xf);
		code3 = asicc((code>>4)&0xf);	
		code4 = asicc((code>>0)&0xf);			
	}
	else
	{
		func1=0x6e;
		code = 1;
		for (i=0;i<b2;i++)
		{
			code = code * 2;
		}
		code = 0xffff - code;
		code1 = asicc((code>>12)&0xf);	
		code2 = asicc((code>>8)&0xf);
		code3 = asicc((code>>4)&0xf);	
		code4 = asicc((code>>0)&0xf);			
	}
	
	*(U8 *)(AD1+0)=0x02;        //头ENQ，即02
	*(U8 *)(AD1+1)=func1;  		//fix
	*(U8 *)(AD1+2)=0x4d;		//
	*(U8 *)(AD1+3)=asicc((b3>>4)&0xf);		//地址
	*(U8 *)(AD1+4)=asicc((b3>>0)&0xf);		
	
	switch (ptcom->registerr)	//根据寄存器类型发送不同的命令字符
	{
	case 'Y':
		*(U8 *)(AD1+5)=0x35;        //寄存器代码
		*(U8 *)(AD1+6)=0x43;
		break;
				
	case 'M':
		*(U8 *)(AD1+5)=0x35;        //寄存器代码
		*(U8 *)(AD1+6)=0x44; 
		break;	
		
	case 'L':						//L
		*(U8 *)(AD1+5)=0x35;        //寄存器代码
		*(U8 *)(AD1+6)=0x46;
		break;
		
	case 'K':						//K
		*(U8 *)(AD1+5)=0x36;        //寄存器代码
		*(U8 *)(AD1+6)=0x30;
		break;							
	}		
	
	*(U8 *)(AD1+7)=0x30;        //固定
	*(U8 *)(AD1+8)=0x30;
	
	*(U8 *)(AD1+9)=code3;       
	*(U8 *)(AD1+10)=code4;
	
	*(U8 *)(AD1+11)=code1;	
	*(U8 *)(AD1+12)=code2;

	aakj=CalcHe((U8 *)AD1+1,12);  //和校验，校验前的数求和
	a1=aakj&0xff;                 //只发最低位
	*(U8 *)(AD1+13)=asicc((a1>>4)&0xf);		//check
	*(U8 *)(AD1+14)=asicc((a1>>0)&0xf);	
	*(U8 *)(AD1+15)=0x03;			//END
	
	
	ptcom->send_length[0]=16;				//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址	
	ptcom->send_times=1;					//发送次数
		
	ptcom->return_length[0]=5;				//返回数据长度
	ptcom->return_start[0]=0;				//返回数据有效开始
	ptcom->return_length_available[0]=0;	//返回有效数据长度	
	ptcom->Current_Times=0;					//当前发送次数			
}


void Read_Bool()				//读取数字量的信息
{
	U16 aakj;
	int b;
	int a1;
	int length;
	int b1,b2,b3;
	
	b=ptcom->address;			    // 寄存器所给地址
	length=ptcom->register_length;  //所读的长度

	b1 = b/16;
	b2 = b%16;	
	b3 = b1 * 2;
	
	if (b2 >= 8)					//余数大于等于8
	{
		length = length /2 + 1;
	}
	else 
	{
		if (length % 2 == 0)
		{
			length = length / 2;
		}
		else
		{
			length = length /2 + 1;
		}
	}	

	length = length * 2;

	*(U8 *)(AD1+0)=0x02;            //头ENQ，即02
	*(U8 *)(AD1+1)=0x72;  			//plc站地址，高位在前
	*(U8 *)(AD1+2)=0x4d;
	*(U8 *)(AD1+3)=asicc((b3>>4)&0xf);		//地址
	*(U8 *)(AD1+4)=asicc((b3>>0)&0xf);		
	
	switch (ptcom->registerr)	//根据寄存器类型发送不同的命令字符
	{
	case 'Y':
		*(U8 *)(AD1+5)=0x35;        //寄存器代码
		*(U8 *)(AD1+6)=0x43;
		break;
				
	case 'M':
		*(U8 *)(AD1+5)=0x35;        //寄存器代码
		*(U8 *)(AD1+6)=0x44; 
		break;	
		
	case 'L':						//L
		*(U8 *)(AD1+5)=0x35;        //寄存器代码
		*(U8 *)(AD1+6)=0x46;
		break;
		
	case 'K':						//K
		*(U8 *)(AD1+5)=0x36;        //寄存器代码
		*(U8 *)(AD1+6)=0x30;
		break;							
	}
	
	*(U8 *)(AD1+7)=asicc((length>>12)&0xf);		//len
	*(U8 *)(AD1+8)=asicc((length>>8)&0xf);	
	*(U8 *)(AD1+9)=asicc((length>>4)&0xf);		//len
	*(U8 *)(AD1+10)=asicc((length>>0)&0xf);		
	
		
	aakj=CalcHe((U8 *)AD1+1,10);      //和校验，校验前的数求和
	a1=aakj&0xff;                   //只发最低位
	*(U8 *)(AD1+11)=asicc((a1>>4)&0xf);		//check
	*(U8 *)(AD1+12)=asicc((a1>>0)&0xf);	
	*(U8 *)(AD1+13)=0x03;			//END

	ptcom->send_length[0]=14;				//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址	
	ptcom->send_times=1;					//发送次数
			
			
	ptcom->return_length[0]=5+length*2;	//返回数据长度，有5个固定
	ptcom->return_start[0]=2;				//返回数据有效开始，第2个
	ptcom->return_length_available[0]=length*2;	//返回有效数据长度	
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->send_add[0]=ptcom->address;		//读的是这个地址的数据	
	ptcom->address=b1 * 16;				//发送真实数据地址	
	ptcom->register_length = length;
	ptcom->needShake=0;
}


void Read_Analog()				//读模拟量
{
	U16 aakj;
	int b;
	int a1;
	int length;
	int b3;
	
	b=ptcom->address;			    // 寄存器所给地址
	length=ptcom->register_length;  //所读的长度
	length = length *2;

	b3 = b * 2;
		

	*(U8 *)(AD1+0)=0x02;            //头ENQ，即02
	*(U8 *)(AD1+1)=0x72;  			//plc站地址，高位在前
	*(U8 *)(AD1+2)=0x4d;
	*(U8 *)(AD1+3)=asicc((b3>>4)&0xf);		//地址
	*(U8 *)(AD1+4)=asicc((b3>>0)&0xf);		
	
	*(U8 *)(AD1+5)=asicc((b3>>12)&0xf);		//地址
	*(U8 *)(AD1+6)=asicc((b3>>8)&0xf);
						

	
	*(U8 *)(AD1+7)=asicc((length>>12)&0xf);		//len
	*(U8 *)(AD1+8)=asicc((length>>8)&0xf);	
	*(U8 *)(AD1+9)=asicc((length>>4)&0xf);		//len
	*(U8 *)(AD1+10)=asicc((length>>0)&0xf);		
	
		
	aakj=CalcHe((U8 *)AD1+1,10);      //和校验，校验前的数求和
	a1=aakj&0xff;                   //只发最低位
	*(U8 *)(AD1+11)=asicc((a1>>4)&0xf);		//check
	*(U8 *)(AD1+12)=asicc((a1>>0)&0xf);	
	*(U8 *)(AD1+13)=0x03;			//END

	ptcom->send_length[0]=14;				//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址	
	ptcom->send_times=1;					//发送次数
			
	ptcom->return_length[0]=5+length*2;	//返回数据长度，有5个固定
	ptcom->return_start[0]=2;				//返回数据有效开始，第2个
	ptcom->return_length_available[0]=length*2;	//返回有效数据长度	
	ptcom->Current_Times=0;					//当前发送次数	
	ptcom->send_add[0]=ptcom->address;		//读的是这个地址的数据	
	
	ptcom->needShake=1;
}


void Read_Recipe()								//读取配方
{
	U16 aakj;
	int b;
	int a1;
	int i;
	int datalength;
	int p_start;
	int ps;
	int SendTimes;
	int LastTimeWord;							//最后一次发送长度
	int currentlength;
	int length;

	
		
	datalength=ptcom->register_length;		//发送总长度
	p_start=ptcom->address;					//开始地址
	length=ptcom->register_length;          //读的长度，多少个配方

	if(datalength>5000)                     //限制长度
		datalength=5000;

	if(datalength%16==0)                    //每次发16个配方，读的配方数刚好为16的倍数时
	{
		SendTimes=datalength/16;            //发送的次数            
		LastTimeWord=16;                    //最后一次发送的长度为16	
	}
	if(datalength%16!=0)                    //每次发16个配方，读的配方数不是16的倍数时
	{
		SendTimes=datalength/16+1;          //发送的次数
		LastTimeWord=datalength%16;         //最后一次发送的长度为除16的余数	
	}
		
	for (i=0;i<SendTimes;i++)               //小于发送次数继续发送
	{
		ps=14*i;                            //每次发19个长度，第二次发就是19开始
		b=p_start+i*16;                     //每次的开始地址
		b=b*2;

		
		*(U8 *)(AD1+0+ps)=0x02;            //头ENQ，即02
		*(U8 *)(AD1+1+ps)=0x72;  			//plc站地址，高位在前
		*(U8 *)(AD1+2+ps)=0x4d;
		*(U8 *)(AD1+3+ps)=asicc((b>>4)&0xf);		//地址
		*(U8 *)(AD1+4+ps)=asicc((b>>0)&0xf);		
		
		*(U8 *)(AD1+5+ps)=asicc((b>>12)&0xf);		//地址
		*(U8 *)(AD1+6+ps)=asicc((b>>8)&0xf);	
							
		if (i!=(SendTimes-1))	          //不是最后一次发送时
		{
			*(U8 *)(AD1+7+ps)=asicc((32>>12)&0xf);		//len
			*(U8 *)(AD1+8+ps)=asicc((32>>8)&0xf);	
			*(U8 *)(AD1+9+ps)=asicc((32>>4)&0xf);		//len
			*(U8 *)(AD1+10+ps)=asicc((32>>0)&0xf);		
			currentlength=16;             //发送的数据长度
		}
		if (i==(SendTimes-1))	          //最后一次发送时
		{
			*(U8 *)(AD1+7+ps)=asicc((LastTimeWord*2>>12)&0xf);		//len
			*(U8 *)(AD1+8+ps)=asicc((LastTimeWord*2>>8)&0xf);	
			*(U8 *)(AD1+9+ps)=asicc((LastTimeWord*2>>4)&0xf);		//len
			*(U8 *)(AD1+10+ps)=asicc((LastTimeWord*2>>0)&0xf);		
			currentlength=LastTimeWord;   //发送的数据长度
		}		
			
		aakj=CalcHe((U8 *)AD1+ps+1,10);      //和校验，校验前的数求和
		a1=aakj&0xff;                   //只发最低位
		*(U8 *)(AD1+11+ps)=asicc((a1>>4)&0xf);		//check
		*(U8 *)(AD1+12+ps)=asicc((a1>>0)&0xf);	
		*(U8 *)(AD1+13+ps)=0x03;			//END
	
		
		ptcom->send_length[i]=14;				//发送长度
		ptcom->send_staradd[i]=i*14;			//发送数据存储地址	
		ptcom->send_add[i]=p_start+i*16;		//读的是这个地址的数据	
		ptcom->send_data_length[i]=currentlength;//不是最后一次都是16个
				
		ptcom->return_length[i]=5+currentlength*4;	//返回数据长度，有11个固定
		ptcom->return_start[i]=2;				    //返回数据有效开始
		ptcom->return_length_available[i]=currentlength*4;	//返回有效数据长度			
	}

	ptcom->Current_Times=0;					        //当前发送次数
	ptcom->needShake=1;		
	ptcom->send_times=SendTimes;					//发送次数		

}

void Write_Analog()								//写模拟量
{
	U16 aakj;
	int b,i;
	int a1;
	int length;
	int plcadd;
	int c,c0,c1,c2,c3,c4;
	int b3;
	int iSend_Len;
			
	b=ptcom->address;			    //开始地址
	plcadd=ptcom->plc_address;	    //PLC站地址
	length=ptcom->register_length;  //写的配方数
	
	b3 = b * 2;
	
	*(U8 *)(AD1+0)=0x02;            //头STX，即02
	*(U8 *)(AD1+1)=0x77; 			//plc站地址，高位在前
	*(U8 *)(AD1+2)=0x4d;
	
	*(U8 *)(AD1+3)=asicc((b3>>4)&0xf);		//地址
	*(U8 *)(AD1+4)=asicc((b3>>0)&0xf);		
	
	*(U8 *)(AD1+5)=asicc((b3>>12)&0xf);		//地址
	*(U8 *)(AD1+6)=asicc((b3>>8)&0xf);	
	
	iSend_Len = length*2;
	*(U8 *)(AD1+7)=asicc((iSend_Len>>12)&0xf);		//len
	*(U8 *)(AD1+8)=asicc((iSend_Len>>8)&0xf);	
	*(U8 *)(AD1+9)=asicc((iSend_Len>>4)&0xf);		//len
	*(U8 *)(AD1+10)=asicc((iSend_Len>>0)&0xf);		
	
	for(i=0;i<length;i++)
	{			
		c0=ptcom->U8_Data[i*2];   //从D[]数组中要数据，对应的c0为高位，c为低位
		c=ptcom->U8_Data[i*2+1];
							
		c1=c&0xf;                //对所写数据作asicc处理，c为低位
		c2=(c>>4)&0xf;           //对所写数据作asicc处理，c为低位		
		c3=c0&0xf;               //对所写数据作asicc处理，c1为高位
		c4=(c0>>4)&0xf;          //对所写数据作asicc处理，c1为高位				
		
		*(U8 *)(AD1+11+i*4)=asicc(c4);        //数据是高位先发
		*(U8 *)(AD1+12+i*4)=asicc(c3);
		*(U8 *)(AD1+13+i*4)=asicc(c2);
		*(U8 *)(AD1+14+i*4)=asicc(c1);	
	}
	
	aakj=CalcHe((U8 *)AD1+1,10+i*4);      //和校验，校验前的数求和
	a1=aakj&0xff;                   //只发最低位
	*(U8 *)(AD1+11+length*4)=asicc((a1>>4)&0xf);		//check
	*(U8 *)(AD1+12+length*4)=asicc((a1>>0)&0xf);	
	*(U8 *)(AD1+13+length*4)=0x03;			//END

	ptcom->send_length[0]=14+length*4;		//发送长度
	ptcom->send_staradd[0]=0;				//发送数据存储地址	
	ptcom->send_times=1;					//发送次数
			
	ptcom->return_length[0]=5;				//返回数据长度
	ptcom->return_start[0]=0;				//返回数据有效开始
	ptcom->return_length_available[0]=0;	//返回有效数据长度	
	ptcom->Current_Times=0;					//当前发送次数	
}

void Write_Time()                                //从PLC写时间  
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
	int a1,a2,a3,a4;
	int b1,b2,b3,b4;
	int c1,c2,c3,c4;
	U16 aakj;
	int plcadd;
	int length;
	int addr;
		
	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));//数据长度
	staradd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//数据长度

	if(datalength%16==0)                           //写的配方数刚好是16的倍数时             
	{
		SendTimes=datalength/16;                   //发送的次数
		LastTimeWord=16;                           //最后一次发送的长度	
	}
	if(datalength%16!=0)                           //不是16的倍数时
	{
		SendTimes=datalength/16+1;                 //发送的次数
		LastTimeWord=datalength%16;                //最后一次发送的长度	
	}	
	
	ps=78;                                         //写16个配方时，发送的是83个长度
	for (i=0;i<SendTimes;i++)
	{
		addr = staradd + i*16;
		
		if (i!=(SendTimes-1))                      //不是最后一次发送时
		{	
			length=16;                             //发16个
		}
		else                                       //最后一次发送时
		{
			length=LastTimeWord;                   //发剩余的配方数             
		}

		*(U8 *)(AD1+0+ps*i)=0x02;            //头STX，即02
		*(U8 *)(AD1+1+ps*i)=0x77; 			//plc站地址，高位在前
		*(U8 *)(AD1+2+ps*i)=0x4d;
		
		*(U8 *)(AD1+3+ps*i)=asicc((addr*2>>4)&0xf);		//地址
		*(U8 *)(AD1+4+ps*i)=asicc((addr*2>>0)&0xf);		
		
		*(U8 *)(AD1+5+ps*i)=asicc((addr*2>>12)&0xf);		//地址
		*(U8 *)(AD1+6+ps*i)=asicc((addr*2>>8)&0xf);	
		

		*(U8 *)(AD1+7+ps*i)=asicc((length*2>>12)&0xf);		//len
		*(U8 *)(AD1+8+ps*i)=asicc((length*2>>8)&0xf);	
		*(U8 *)(AD1+9+ps*i)=asicc((length*2>>4)&0xf);		//len
		*(U8 *)(AD1+10+ps*i)=asicc((length*2>>0)&0xf);	
		
		for(j=0;j<length;j++)
		{
			b4=*(U8 *)(PE+9+i*32+j*2);         //从数组中取数据，B3是低位，B4是高位
			b3=*(U8 *)(PE+9+i*32+j*2+1);
				
			c1=b3&0xf;               //对所写数据作asicc处理
			c2=(b3>>4)&0xf;          //对所写数据作asicc处理		
			c3=b4&0xf;               //对所写数据作asicc处理
			c4=(b4>>4)&0xf;          //对所写数据作asicc处理				
		
			*(U8 *)(AD1+11+j*4+ps*i)=asicc(c4);   //高位先发
			*(U8 *)(AD1+12+j*4+ps*i)=asicc(c3);
			*(U8 *)(AD1+13+j*4+ps*i)=asicc(c2);
			*(U8 *)(AD1+14+j*4+ps*i)=asicc(c1);				
		}
		aakj=CalcHe((U8 *)AD1+1+ps*i,10+j*4);      //和校验，校验前的数求和
		a1=aakj&0xff;                   //只发最低位
		*(U8 *)(AD1+11+length*4+ps*i)=asicc((a1>>4)&0xf);		//check
		*(U8 *)(AD1+12+length*4+ps*i)=asicc((a1>>0)&0xf);	
		*(U8 *)(AD1+13+length*4+ps*i)=0x03;			//END

		ptcom->send_length[i]=14+length*4;				//发送长度
		ptcom->send_staradd[i]=i*ps;			//发送数据存储地址	
		ptcom->return_length[i]=5;				//返回数据长度
		ptcom->return_start[i]=0;				//返回数据有效开始
		ptcom->return_length_available[i]=0;	//返回有效数据长度	
			
	}
	ptcom->send_times=SendTimes;					//发送次数
	ptcom->Current_Times=0;					//当前发送次数		
}


void compxy(void)				//处理成标准存储格式
{
	int i;
	unsigned char a1,a2,a3,a4;
	int b,b1;
	if(ptcom->needShake==1)                                //KK==0时。对返回来的数高低位交换
	{                                  
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/4;i++)	//返回的有用长度/4就是需要的长度
 		 {		
  		    a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4);
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+1);
			a3=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+2);
			a4=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+3);	
			a1=bsicc(a1);
			a2=bsicc(a2);
			a3=bsicc(a3);
			a4=bsicc(a4);                       //ASC玛返回，所以要转为16进制
			b=(a1<<4)+a2;                       //返回的前两个
			b1=(a3<<4)+a4;                      //返回的后两
			*(U8 *)(COMad+i*2)=b1;				//重新存,从第0个开始存，交换高低位
			*(U8 *)(COMad+i*2+1)=b;	
		}
	}
	else                                      //对返回来的数高低位不变
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/4;i++)	//返回的有用长度/4就是需要的长度
		{
		a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4);
		a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+1);
		a3=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+2);
		a4=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+3);	
		a1=bsicc(a1);                           //ASC玛返回，所以要转为16进制
		a2=bsicc(a2);
		a3=bsicc(a3);
		a4=bsicc(a4);
		b=(a1<<4)+a2;                          //返回的前两个
		b1=(a3<<4)+a4;                         //返回的后两个
		*(U8 *)(COMad+i*2)=b;				   //重新存,从第0个开始存，高低位不变
		*(U8 *)(COMad+i*2+1)=b1;	
		}
	}			
	ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;	//长度减半	
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

int remark()				//返回来的数据计算校检码是否正确，LG返回的数没有校验，故不检验是否正确都return 1
{
	unsigned int aakj1,aakj2;
	unsigned int akj1;
	unsigned int a1,a2;	
	aakj1=*(U8 *)(COMad+ptcom->return_length[ptcom->Current_Times-1]-2)&0xff;	
	aakj2=*(U8 *)(COMad+ptcom->return_length[ptcom->Current_Times-1]-3)&0xff;
				
	akj1=CalcHe((U8 *)COMad+1,ptcom->return_length[ptcom->Current_Times-1]-2);
	a1=asicc((akj1>>4)&0xf);		//check
	a2=asicc((akj1>>0)&0xf);	
	if(aakj1==a1 && aakj2 == a2)
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
