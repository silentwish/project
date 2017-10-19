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
		case 'Y':
		case 'M':
		case 'K':
		case 'L':
		case 'T':
		case 'C':						
			Read_Bool();//读数字量
			break;
		case 'D':
		case 't':
		case 'c':	
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
		case 'T':
		case 'C':				
			Set_Reset();//置位复位
			break;
		case 'D':
		case 't':
		case 'c':
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
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Set_Reset()//置位/复位
{
	U16 aakj;
	int b,t;
	int a1,a2;
	int b1,b2;
	int plcadd;	
	int add;

	b=ptcom->address;								// 寄存器给的开始地址
	plcadd=ptcom->plc_address;						//PLC站地址
	
	*(U8 *)(AD1+0)=0x05;        					//请求信号0x05，需要PLC返回06

	*(U8 *)(AD1+1)=0x02;        					//固定02
	if (ptcom->writeValue==1)						//置位时，发o，即6f
	{
		*(U8 *)(AD1+2)=0x6f;
	}
	if (ptcom->writeValue==0)						//复位时，发p，即70
	{
		*(U8 *)(AD1+2)=0x70;
	}
	
	*(U8 *)(AD1+3)=0x46;							//固定FB 
	*(U8 *)(AD1+4)=0x42;

	switch (ptcom->registerr)						//根据寄存器类型获取偏移地址
	{
	case 'Y':
		t=0xa0;		
		break;
				
	case 'M':
		t=0x80;	
		break;	
		
	case 'L':
		t=0xb4;
		break;
		
	case 'K':
		t=0xa4;
		break;
		
	case 'T':
		t=0xe4;
		break;	
		
	case 'C':
		t=0xea;
		break;								
	}
	
	add=b/8;										//以字节为单位置位或者复位
	add=add+t;
	a1=(add>>4)&0xf;
	a2=add&0xf;
	*(U8 *)(AD1+5)=asicc(a1);						//转化后地址高位
	*(U8 *)(AD1+6)=asicc(a2);			

	if (ptcom->writeValue==1)						//以字节为单位后，确定要置位的地址在8位中的哪一位。则该位为1其余位为0
	{
		b1=1;
		b2=b%8;					
		b1=(b1<<b2)&0xff;
		a1=(b1>>4)&0xf;
		a2=b1&0xf;
		*(U8 *)(AD1+7)=asicc(a1);
		*(U8 *)(AD1+8)=asicc(a2);			
	}
	if (ptcom->writeValue==0)						//以字节为单位后，确定要复位的地址在8位中的哪一个。则该位为0其余位为1
	{
		b1=1;
		b2=b%8;
		b1=~(b1<<b2)&0xff;
		a1=(b1>>4)&0xf;
		a2=b1&0xf;
		*(U8 *)(AD1+7)=asicc(a1);
		*(U8 *)(AD1+8)=asicc(a2);	
	}
		
	*(U8 *)(AD1+9)=0x04;       						//结束码 04
	
	aakj=CalcHe((U8 *)(AD1+2),8);					//和校验
	a1=(aakj>>4)&0xf;                 
	a2=aakj&0xf;
	*(U8 *)(AD1+10)=asicc(a1);         
	*(U8 *)(AD1+11)=asicc(a2);        
	
	ptcom->send_length[0]=1;						//发送长度
	ptcom->send_staradd[0]=0;						//发送数据存储地址		
	ptcom->return_length[0]=1;						//返回数据长度
	ptcom->return_start[0]=0;						//返回数据有效开始
	ptcom->return_length_available[0]=0;			//返回有效数据长度	
	
	ptcom->send_length[1]=11;						//发送长度
	ptcom->send_staradd[1]=1;						//发送数据存储地址		
	ptcom->return_length[1]=6;						//返回数据长度
	ptcom->return_start[1]=0;						//返回数据有效开始
	ptcom->return_length_available[1]=0;			//返回有效数据长度
			
	ptcom->Current_Times=0;							//当前发送次数			
	ptcom->send_times=2;							//发送次数	
	
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Read_Bool()//读取数字量的信息
{
	U16 aakj;
	int b,t;
	int add;
	int a1,a2;
	int length;
	int plcadd;
	
	b=ptcom->address;			    				// 寄存器所给地址
	plcadd=ptcom->plc_address;	    				//PLC站地址
	length=ptcom->register_length;  				//所读的长度
	

	*(U8 *)(AD1+0)=0x05;        					//请求信号0x05，需要PLC返回06

	*(U8 *)(AD1+1)=0x02;        					//固定
	*(U8 *)(AD1+2)=0x67;							//读位时发g，即67
	*(U8 *)(AD1+3)=0x46;							//固定FB 
	*(U8 *)(AD1+4)=0x42;
	
	switch (ptcom->registerr)						//根据寄存器类型获取偏移地址
	{
	case 'Y':
		t=0xa0;		
		break;
				
	case 'M':
		t=0x80;	
		break;	
		
	case 'L':
		t=0xb4;
		break;
		
	case 'K':
		t=0xa4;
		break;
		
	case 'T':
		t=0xe4;
		break;	
		
	case 'C':
		t=0xea;
		break;								
	}
	
	add=b/8;										//按字节为单位读取，1个长度为1个字节
	add=add+t;
	a1=(add>>4)&0xf;
	a2=add&0xf;
	*(U8 *)(AD1+5)=asicc(a1);
	*(U8 *)(AD1+6)=asicc(a2);		
	
	a1=(length>>4)&0xf;								//长度
	a2=length&0xf;
	
	*(U8 *)(AD1+7)=asicc(a1);
	*(U8 *)(AD1+8)=asicc(a2);	
	
	*(U8 *)(AD1+9)=0x04;       						//结束码 04
	
	aakj=CalcHe((U8 *)(AD1+2),8);  					//和校验
	a1=(aakj>>4)&0xf;                 
	a2=aakj&0xf;
	*(U8 *)(AD1+10)=asicc(a1);        
	*(U8 *)(AD1+11)=asicc(a2);         
	
	ptcom->send_length[0]=1;						//发送长度
	ptcom->send_staradd[0]=0;						//发送数据存储地址	
	ptcom->return_length[0]=1;              		//返回数据长度
	ptcom->return_start[0]=0;						//返回数据有效开始
	ptcom->return_length_available[0]=0 ;			//返回有效数据长度
		
	ptcom->send_length[1]=11;						//发送长度
	ptcom->send_staradd[1]=1;						//发送数据存储地址	
	ptcom->return_length[1]=5+length*2;     		//返回数据长度                                                
	ptcom->return_start[1]=2;						//返回数据有效开始
	ptcom->return_length_available[1]=length*2 ;	//返回有效数据长度
		
	ptcom->send_times=2;							//发送次数	
	ptcom->send_add[0]=ptcom->address;				//读的是这个地址的数据
	ptcom->send_add[1]=ptcom->address;				//读的是这个地址的数据
	ptcom->Current_Times=0;							//当前发送次数   	
	ptcom->Simens_Count=1; 							//全局变量，根据其值返回处理不同
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Read_Analog()//读模拟量
{
	U16 aakj;
	int add;
	int a1,a2;
	int t;
	int plcadd;
	int length;
	
	plcadd=ptcom->plc_address;	    				//PLC站地址
	add=ptcom->address;			    				// 寄存器所给地址
	length=ptcom->register_length;  				//读的长度
	     

	*(U8 *)(AD1+0)=0x05;        					//请求信号0x05，需要PLC返回06

	*(U8 *)(AD1+1)=0x02;        					//固定
	*(U8 *)(AD1+2)=0x67;							//读时发g，即67
	*(U8 *)(AD1+3)=0x46; 							//固定FC
	*(U8 *)(AD1+4)=0x43;
	
	switch (ptcom->registerr)	    				//根据寄存器类型获取偏移地址
	{
	case 'D': 
		t=0x80;		
		break;	
			
	case 't':
		t=0x00;	 	                   
		break;
			
	case 'c': 
		t=0x60;	
		break;						
	}
	
	add=add*2+t;									//寄存器是按字节排列，读取时按字节读取，固读字时地址需*2
	a1=(add>>4)&0xf;
	a2=add&0xf;
	*(U8 *)(AD1+5)=asicc(a1);						//读取地址
	*(U8 *)(AD1+6)=asicc(a2);	
	
	a1=((length*2)>>4)&0xf;							//读取长度也相应*2
	a2=(length*2)&0xf;
	
	*(U8 *)(AD1+7)=asicc(a1);
	*(U8 *)(AD1+8)=asicc(a2);	
	
	*(U8 *)(AD1+9)=0x04;       						//结束码 04
	
	aakj=CalcHe((U8 *)(AD1+2),8);  					//和校验
	a1=(aakj>>4)&0xf;               
	a2=aakj&0xf;
	*(U8 *)(AD1+10)=asicc(a1);         				
	*(U8 *)(AD1+11)=asicc(a2);         	

	ptcom->send_length[0]=1;						//发送长度
	ptcom->send_staradd[0]=0;						//发送数据存储地址	
	ptcom->return_length[0]=1;              		//返回数据长度
	ptcom->return_start[0]=0;						//返回数据有效开始
	ptcom->return_length_available[0]=0 ;			//返回有效数据长度
		
	ptcom->send_length[1]=11;						//发送长度
	ptcom->send_staradd[1]=1;						//发送数据存储地址	
	ptcom->return_length[1]=5+length*4;     		//返回数据长度                                                
	ptcom->return_start[1]=2;						//返回数据有效开始
	ptcom->return_length_available[1]=length*4 ;	//返回有效数据长度
		
	ptcom->send_times=2;							//发送次数	
	ptcom->send_add[0]=ptcom->address;				//读的是这个地址的数据
	ptcom->send_add[1]=ptcom->address;				//读的是这个地址的数据
	ptcom->Current_Times=0;							//当前发送次数   	
	ptcom->Simens_Count=2; 							//全局变量，根据其值返回处理不同
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Write_Analog()//写模拟量
{
	U16 aakj;
	int add,i;
	int a1,a2;
	int length;
	int plcadd;
	int t;
	int c,c0,c1,c2,c3,c4;
			
	add=ptcom->address;			    				//开始地址
	plcadd=ptcom->plc_address;	    				//PLC站地址
	length=ptcom->register_length;  				//写的配方数
	
	*(U8 *)(AD1+0)=0x05;        					//请求信号0x05，需要PLC返回06

	*(U8 *)(AD1+1)=0x02;        					//固定
	*(U8 *)(AD1+2)=0x77;							//写时发w,即77
	*(U8 *)(AD1+3)=0x46; 
	*(U8 *)(AD1+4)=0x43;							//固定FC
	
	switch (ptcom->registerr)	    				//根据寄存器类型获取偏移地址
	{
	case 'D': 
		t=0x80;		
		break;	
			
	case 't':
		t=0x00;	 	                   
		break;
			
	case 'c': 
		t=0x60;	
		break;						
	}
	
	add=add*2+t;									//寄存器是按字节排列，读取时按字节读取，固读字时地址需*2
	a1=(add>>4)&0xf;
	a2=add&0xf;
	*(U8 *)(AD1+5)=asicc(a1);
	*(U8 *)(AD1+6)=asicc(a2);	
	
	a1=((length*2)>>4)&0xf;							//读取长度也相应*2
	a2=(length*2)&0xf;
	
	*(U8 *)(AD1+7)=asicc(a1);
	*(U8 *)(AD1+8)=asicc(a2);
	
	for(i=0;i<length;i++)
	{			
		c=ptcom->U8_Data[i*2];   					//从D[]数组中要数据，对应的c0为高位，c为低位
		c0=ptcom->U8_Data[i*2+1];
							
		c1=c&0xf;                					//对所写数据作asicc处理，c为低位
		c2=(c>>4)&0xf;           					//对所写数据作asicc处理，c为低位		
		c3=c0&0xf;               					//对所写数据作asicc处理，c1为高位
		c4=(c0>>4)&0xf;          					//对所写数据作asicc处理，c1为高位				
		
		*(U8 *)(AD1+9+i*4)=asicc(c2);        		//数据是低位先发
		*(U8 *)(AD1+10+i*4)=asicc(c1);
		*(U8 *)(AD1+11+i*4)=asicc(c4);
		*(U8 *)(AD1+12+i*4)=asicc(c3);	
	}
	
	*(U8 *)(AD1+9+length*4)=0x04;        			//结束字符
	
	aakj=CalcHe((U8 *)(AD1+2),8+length*4);  		//和校验
	a1=(aakj>>4)&0xf;                 
	a2=aakj&0xf;
	*(U8 *)(AD1+10+length*4)=asicc(a1);         
	*(U8 *)(AD1+11+length*4)=asicc(a2);        

	ptcom->send_length[0]=1;						//发送长度
	ptcom->send_staradd[0]=0;						//发送数据存储地址	
	ptcom->return_length[0]=1;              		//返回数据长度
	ptcom->return_start[0]=0;						//返回数据有效开始
	ptcom->return_length_available[0]=0 ;			//返回有效数据长度
		
	ptcom->send_length[1]=11+length*4;				//发送长度
	ptcom->send_staradd[1]=1;						//发送数据存储地址	
	ptcom->return_length[1]=6;     					//返回数据长度                                                
	ptcom->return_start[1]=.0;						//返回数据有效开始
	ptcom->return_length_available[1]=0 ;			//返回有效数据长度
		
	ptcom->send_times=2;							//发送次数
	ptcom->Current_Times=0;							//当前发送次数		
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Read_Recipe()//读取配方
{
	U16 aakj;
	int add;
	int a1,a2;
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
	plcadd=ptcom->plc_address;	            		//PLC站地址
	
	if(datalength>5000)                     		//限制长度
		datalength=5000;

	if(datalength%16==0)                    		//每次发16个配方，读的配方数刚好为16的倍数时
	{
		SendTimes=datalength/16;            		//发送的次数            
		LastTimeWord=16;                    		//最后一次发送的长度为16	
	}
	if(datalength%16!=0)                    		//每次发16个配方，读的配方数不是16的倍数时
	{
		SendTimes=datalength/16+1;          		//发送的次数
		LastTimeWord=datalength%16;         		//最后一次发送的长度为除16的余数	
	}
	
	for (i=0;i<SendTimes;i++)               		//小于发送次数继续发送
	{
		ps=12*i;                            		//每次发12个长度，第二次发就是12开始
		add=p_start+i*16;                     		//每次的开始地址

		*(U8 *)(AD1+0+ps)=0x05;        				//请求信号0x05，需要PLC返回06

		*(U8 *)(AD1+1+ps)=0x02;        				//固定
		*(U8 *)(AD1+2+ps)=0x67;						//读时发g，即67
		*(U8 *)(AD1+3+ps)=0x46; 					//固定FC
		*(U8 *)(AD1+4+ps)=0x43;
			
		add=add*2+0x80;								//寄存器是按字节排列，读取时按字节读取，固读字时地址需*2，并+偏移地址
		a1=(add>>4)&0xf;
		a2=add&0xf;
		*(U8 *)(AD1+5+ps)=asicc(a1);
		*(U8 *)(AD1+6+ps)=asicc(a2);	
		
		if (i!=(SendTimes-1))	          			//不是最后一次发送时
		{
			*(U8 *)(AD1+7+ps)=0x32;      			//固定长度32个字节，即0x20，转成asicc
			*(U8 *)(AD1+8+ps)=0x30;
			currentlength=16;             			//发送的数据长度
		}
		if (i==(SendTimes-1))	          			//最后一次发送时
		{
			currentlength=LastTimeWord;   			//发送的数据长度
			*(U8 *)(AD1+7+ps)=asicc(((currentlength*2)>>4)&0xf);//发送的长度为除16的余数LastTimeWord
			*(U8 *)(AD1+8+ps)=asicc((currentlength*2)&0xf);
		}	
			
		*(U8 *)(AD1+9+ps)=0x04;          			//结束字符
		
		aakj=CalcHe((U8 *)(AD1+2+ps),8);  			//和校验
		a1=(aakj>>4)&0xf;                 
		a2=aakj&0xf;
		*(U8 *)(AD1+10+ps)=asicc(a1);        
		*(U8 *)(AD1+11+ps)=asicc(a2);         	
		
		ptcom->send_length[i*2]=1;					//发送长度
		ptcom->send_staradd[i*2]=ps;				//发送数据存储地址	
		ptcom->return_length[i*2]=1;              	//返回数据长度
		ptcom->return_start[i*2]=0;					//返回数据有效开始
		ptcom->return_length_available[i*2]=0;		//返回有效数据长度
			
		ptcom->send_length[i*2+1]=11;				//发送长度
		ptcom->send_staradd[i*2+1]=1+ps;			//发送数据存储地址	
		ptcom->return_length[i*2+1]=5+currentlength*4;//返回数据长度                                                
		ptcom->return_start[i*2+1]=2;				//返回数据有效开始
		ptcom->return_length_available[i*2+1]=currentlength*4;//返回有效数据长度
			
		ptcom->send_add[i*2+1]=ptcom->address+i*16;	//读的是这个地址的数据
		ptcom->send_data_length[1+i*2]=currentlength;//不是最后一次都是32个D  	
		ptcom->Simens_Count=2; 						//全局变量，根据其值返回处理不同			
	}
	ptcom->send_times=SendTimes*2;					//发送次数
	ptcom->Current_Times=0;					        //当前发送次数	
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Write_Recipe()//写配方到PLC
{
	int datalength;
	int staradd;
	int SendTimes;
	int LastTimeWord;								//最后一次发送长度
	int i,j;
	int ps;
	int a1,a2;
	int b3,b4;
	int c1,c2,c3,c4;
	U16 aakj;
	int plcadd;
	int length;
	int add;
		
	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));//数据长度
	staradd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//数据长度
	plcadd=ptcom->plc_address;	                   	//PLC站地址

	if(datalength%16==0)                           	//写的配方数刚好是16的倍数时             
	{
		SendTimes=datalength/16;                   	//发送的次数
		LastTimeWord=16;                           	//最后一次发送的长度	
	}
	if(datalength%16!=0)                           	//不是16的倍数时
	{
		SendTimes=datalength/16+1;                 	//发送的次数
		LastTimeWord=datalength%16;                	//最后一次发送的长度	
	}	
	
	ps=76;                                         	//写16个配方时，发送的是76个长度
	for (i=0;i<SendTimes;i++)
	{
		if (i!=(SendTimes-1))                      	//不是最后一次发送时
		{	
			length=16;                             	//发16个
		}
		else                                       	//最后一次发送时
		{
			length=LastTimeWord;                   	//发剩余的配方数             
		}

		*(U8 *)(AD1+0+ps*i)=0x05;        			//请求信号0x05，需要PLC返回06

		*(U8 *)(AD1+1+ps*i)=0x02;        			//固定
		*(U8 *)(AD1+2+ps*i)=0x77;					//写时发w，即77
		*(U8 *)(AD1+3+ps*i)=0x46;					//固定FC 
		*(U8 *)(AD1+4+ps*i)=0x43;
		
		add=staradd+i*16;							//寄存器是按字节排列，读取时按字节读取，固读字时地址需*2，并+偏移地址    
		add=add*2+0x80;
		a1=(add>>4)&0xf;
		a2=add&0xf;
		*(U8 *)(AD1+5+ps*i)=asicc(a1);
		*(U8 *)(AD1+6+ps*i)=asicc(a2);	
		
		a1=((length*2)>>4)&0xf;						//长度也相应的*2
		a2=(length*2)&0xf;
		
		*(U8 *)(AD1+7+ps*i)=asicc(a1);
		*(U8 *)(AD1+8+ps*i)=asicc(a2);
		
		for(j=0;j<length;j++)
		{
			b3=*(U8 *)(PE+9+i*32+j*2);         		//从数组中取数据，B3是低位，B4是高位
			b4=*(U8 *)(PE+9+i*32+j*2+1);
				
			c1=b3&0xf;               				//对所写数据作asicc处理
			c2=(b3>>4)&0xf;          				//对所写数据作asicc处理		
			c3=b4&0xf;               				//对所写数据作asicc处理
			c4=(b4>>4)&0xf;          				//对所写数据作asicc处理				
		
			*(U8 *)(AD1+9+j*4+ps*i)=asicc(c2);   	//高位先发
			*(U8 *)(AD1+10+j*4+ps*i)=asicc(c1);
			*(U8 *)(AD1+11+j*4+ps*i)=asicc(c4);
			*(U8 *)(AD1+12+j*4+ps*i)=asicc(c3);				
		}
		*(U8 *)(AD1+9+ps*i+length*4)=0x04;	     	//结束字符
		
		aakj=CalcHe((U8 *)(AD1+2+ps*i),8+length*4); //和校验
		a1=(aakj>>4)&0xf;                 
		a2=aakj&0xf;
		*(U8 *)(AD1+10+length*4+ps*i)=asicc(a1);         
		*(U8 *)(AD1+11+length*4+ps*i)=asicc(a2);         	

		ptcom->send_length[i*2]=1;					//发送长度
		ptcom->send_staradd[i*2]=ps*i;				//发送数据存储地址	
		ptcom->return_length[i*2]=1;              	//返回数据长度
		ptcom->return_start[i*2]=0;					//返回数据有效开始
		ptcom->return_length_available[i*2]=0 ;		//返回有效数据长度
			
		ptcom->send_length[i*2+1]=11+length*4;		//发送长度
		ptcom->send_staradd[i*2+1]=1+ps*i;			//发送数据存储地址	
		ptcom->return_length[i*2+1]=6;     			//返回数据长度                                                
		ptcom->return_start[i*2+1]=0;				//返回数据有效开始
		ptcom->return_length_available[i*2+1]=0 ;	//返回有效数据长度
			
	}
	ptcom->send_times=SendTimes*2;					//发送次数
	ptcom->Current_Times=0;							//当前发送次数		
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Write_Time()//从PLC写时间  
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
	unsigned char a1,a2,a3,a4;
	int b,b1;

	if(ptcom->Simens_Count==1)                               
	{                                 
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/2;i++)	//返回的有用长度/2就是需要的长度
 		{		
  		    a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2);
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2+1);
			a1=bsicc(a1);
			a2=bsicc(a2);
			b=(a1<<4)+a2;                      
			*(U8 *)(COMad+i)=b;	
		}
	}
	if(ptcom->Simens_Count==2)                                   
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/4;i++)	//返回的有用长度/4就是需要的长度
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4);
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+1);
			a3=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+2);
			a4=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+3);	
			a1=bsicc(a1);                           							//ASC玛返回，所以要转为16进制
			a2=bsicc(a2);
			a3=bsicc(a3);
			a4=bsicc(a4);
			b=(a1<<4)+a2;                          
			b1=(a3<<4)+a4;                         
			*(U8 *)(COMad+i*2)=b1;				   								//重新存,从第0个开始存，高低位不变
			*(U8 *)(COMad+i*2+1)=b;	
		}
	}			
	ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;	//长度减半	
	
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void watchcom(void)//检查数据校检
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
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int remark()//返回来的数据计算校检码是否正确，LG返回的数没有校验，故不检验是否正确都return 1
{
	if(ptcom->return_length[ptcom->Current_Times-1]==1)//当前发送次数等于1的话，返回继续取数
	{
		return 0;
	}
	else 
	{
		return 1;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
U16 CalcHe(unsigned char *chData,U16 uNo)//计算和校检
{
	int i;
	int ab=0;
	for(i=0;i<uNo;i++)
	{
		ab=ab+chData[i];
	}
	return (ab);
}
