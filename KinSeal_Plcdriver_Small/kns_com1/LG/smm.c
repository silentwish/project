
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
void Set_Reset()//置位复位函数
{
	U16 aakj;
	int b,t;
	int plcadd;	
	int a1,a2,a3,a4,a5;
	int b1;

	t = 0;

	b=ptcom->address;										// 寄存器给的开始地址
	plcadd=ptcom->plc_address;								//PLC站地址
	
	*(U8 *)(AD1+0)=0x05;        							//头ENQ，即05
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);  			//plc站地址，高位在前
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);
	*(U8 *)(AD1+3)=0x77;        							//置位和复位是对P,M,L,K写，就是w
	*(U8 *)(AD1+4)=0x53;        							//下两个长度发送的是命令类型SS(单个)，即53 53
	*(U8 *)(AD1+5)=0x53; 
    *(U8 *)(AD1+6)=0x30;        							//下两个长度写单个P,M,L,K，地址长度为1 
	*(U8 *)(AD1+7)=0x31;		

	*(U8 *)(AD1+8)=0x30;        							//地址字符和开始地址共8个长度       
	*(U8 *)(AD1+9)=0x38;
	
	switch (ptcom->registerr)								//根据寄存器类型发送不同的命令字符
	{
	case 'Y':
		*(U8 *)(AD1+10)=0x25;       						//P区是%PX，即25 50 58
		*(U8 *)(AD1+11)=0x50;
		*(U8 *)(AD1+12)=0x58;     
		break;
				
	case 'M':
		*(U8 *)(AD1+10)=0x25;       						//M区是%MX，即25 4D 58
		*(U8 *)(AD1+11)=0x4D;
		*(U8 *)(AD1+12)=0x58;
		break;	
		
	case 'L':
		*(U8 *)(AD1+10)=0x25;       						//K区是%LX，即25 4C 58
		*(U8 *)(AD1+11)=0x4C;
		*(U8 *)(AD1+12)=0x58;
		break;
		
	case 'K':
		*(U8 *)(AD1+10)=0x25;       						//L区是%KX，即25 4B 58
		*(U8 *)(AD1+11)=0x4B;
		*(U8 *)(AD1+12)=0x58;
		break;
		
	case 'T':
		*(U8 *)(AD1+10)=0x25;       						//T区是%TX，即25 4B 58
		*(U8 *)(AD1+11)=0x54;
		*(U8 *)(AD1+12)=0x58;
		break;	
		
	case 'C':
		*(U8 *)(AD1+10)=0x25;       						//C区是%CX，即25 4B 58
		*(U8 *)(AD1+11)=0x43;
		*(U8 *)(AD1+12)=0x58;
		break;								
	}	
	switch (ptcom->registerr)								//根据寄存器类型发送不同的命令字符
	{
		case 'Y':
		case 'M':
		case 'L':
		case 'K':	
			b1=b/16;												//地址转换，地址占5个字节，由高到低，前4个字节是十进制发送
			a1=b1/1000;												//最后一个字节是十六进制发送，都要转成ASC码
			a2=(b1-a1*1000)/100;
			a3=(b1-a1*1000-a2*100)/10;
			a4=(b1-a1*1000-a2*100-a3*10);	
			
			*(U8 *)(AD1+13)=asicc(a1);
			*(U8 *)(AD1+14)=asicc(a2);  		
			*(U8 *)(AD1+15)=asicc(a3);
			*(U8 *)(AD1+16)=asicc(a4);		
			*(U8 *)(AD1+17)=asicc(b&0xf); 
			break;
		case 'T':
		case 'C':
			a1=b/10000;												//最后一个字节是十六进制发送，都要转成ASC码
			a2=(b-a1*10000)/1000;
			a3=(b-a1*10000-a2*1000)/100;
			a4=(b-a1*10000-a2*1000-a3*100)/10;
			a5=(b-a1*10000-a2*1000-a3*100-a4*10);	
			
			*(U8 *)(AD1+13)=asicc(a1);
			*(U8 *)(AD1+14)=asicc(a2);  		
			*(U8 *)(AD1+15)=asicc(a3);
			*(U8 *)(AD1+16)=asicc(a4);		
			*(U8 *)(AD1+17)=asicc(a5);
			break;					
	}		

	if (ptcom->writeValue==1)								//置位时，发30 31
	{
		*(U8 *)(AD1+18+t)=0x30;
		*(U8 *)(AD1+19+t)=0x31;
	}
	if (ptcom->writeValue==0)								//复位时，发30 30
	{
		*(U8 *)(AD1+18+t)=0x30;
		*(U8 *)(AD1+19+t)=0x30;
	}
	
	*(U8 *)(AD1+20+t)=0x04;       							//结束码 04

	aakj=CalcHe((U8 *)AD1,21);  							//和校验，校验前的数求和
	a1=aakj&0xff;                 							//只发最低位
	*(U8 *)(AD1+21+t)=asicc((a1 >> 4) & 0xf);        							 	//16进制数，不用转成asicc嘛
	*(U8 *)(AD1+22+t)=asicc((a1 >> 0) & 0xf);        							 	//16进制数，不用转成asicc嘛
	
	ptcom->send_length[0]=23;								//发送长度P为22个
	ptcom->send_staradd[0]=0;								//发送数据存储地址	
	ptcom->send_times=1;									//发送次数
		
	ptcom->return_length[0]=9;								//返回数据长度
	ptcom->return_start[0]=0;								//返回数据有效开始
	ptcom->return_length_available[0]=0;					//返回有效数据长度	
	ptcom->Current_Times=0;									//当前发送次数			
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Read_Bool()//读取数字量的信息
{
	U16 aakj;
	int b;
	int length;
	int plcadd;
	int b1,b2;
	int a1,a2,a3,a4;
	int add;
	int i,j;
	int ps;
	
	b=ptcom->address;			    						// 寄存器所给地址
	plcadd=ptcom->plc_address;	    						//PLC站地址
	length=ptcom->register_length;  						//所读的长度


	switch (ptcom->registerr)
	{
	case 'Y':
	case 'M':
	case 'K':
	case 'L': 
		*(U8 *)(AD1+0)=0x05;            					//头ENQ，即05
		*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);  		//plc站地址，高位在前
		*(U8 *)(AD1+2)=asicc(plcadd&0xf);
		*(U8 *)(AD1+3)=0x72;            					//读数字量是对P,M,L,K读，就是r
		*(U8 *)(AD1+4)=0x53;            					//下两个长度发送的是命令类型SB(连续)，即53 42
		*(U8 *)(AD1+5)=0x42; 
		*(U8 *)(AD1+6)=0x30;            					//下2个长度发送的是地址字符长度 
		*(U8 *)(AD1+7)=0x37;		
				
		switch (ptcom->registerr)	    					//根据寄存器类型有不同的地址字符
		{
			case 'Y': 
				*(U8 *)(AD1+8)=0x25;        				//下3个长度发送的是地址字符%PB       
				*(U8 *)(AD1+9)=0x50; 
			    *(U8 *)(AD1+10)=0x57;   
				break;	
					
			case 'M':		
				*(U8 *)(AD1+8)=0x25;        				//下3个长度发送的是地址字符%MB       
				*(U8 *)(AD1+9)=0x4D; 
			    *(U8 *)(AD1+10)=0x57;         
				break;
					
			case 'K': 
				*(U8 *)(AD1+8)=0x25;        				//下3个长度发送的是地址字符%KB       
				*(U8 *)(AD1+9)=0x4B; 
			    *(U8 *)(AD1+10)=0x57;
				break;
				
			case 'L': 
				*(U8 *)(AD1+8)=0x25;        				//下3个长度发送的是地址字符%LB       
				*(U8 *)(AD1+9)=0x4C; 
			    *(U8 *)(AD1+10)=0x57;
				break;							
		}

		b=b/16;												//地址转换，地址占5个字节，由高到低，前4个字节是十进制发送 
		a1=b/1000;											//最后一个字节是十六进制发送，都要转成ASC码
		a2=(b-a1*1000)/100;
		a3=(b-a1*1000-a2*100)/10;
		a4=(b-a1*1000-a2*100-a3*10);		
		*(U8 *)(AD1+11)=asicc(a1);    
		*(U8 *)(AD1+12)=asicc(a2);   	
		*(U8 *)(AD1+13)=asicc(a3);    
		*(U8 *)(AD1+14)=asicc(a4);  
			
		length = length/2 + length%2;				
		b1=(length>>4)&0xf;             					//读的长度处理，b1高位，b2低位
		b2=length&0xf;
		*(U8 *)(AD1+15)=asicc(b1);      					//读的长度，asicc显示
		*(U8 *)(AD1+16)=asicc(b2);
			
		*(U8 *)(AD1+17)=0x04;           					//结束字符
				
		aakj=CalcHe((U8 *)AD1,18);      					//和校验，校验前的数求和
		a1=aakj&0xff;                   					//只发最低位
		//*(U8 *)(AD1+18)=a1;             					//LG的校验不用转asicc
		*(U8 *)(AD1+18)=asicc((a1 >> 4) & 0xf);        							 	//16进制数，不用转成asicc嘛
		*(U8 *)(AD1+19)=asicc((a1 >> 0) & 0xf);        							 	//16进制数，不用转成asicc嘛

		ptcom->send_length[0]=20;							//发送长度
		ptcom->send_staradd[0]=0;							//发送数据存储地址	
		ptcom->send_times=1;								//发送次数
					
		ptcom->return_length[0]=13+length*4;				//返回数据长度，有11个固定
		ptcom->return_start[0]=10;							//返回数据有效开始，第11个
		ptcom->return_length_available[0]=length*4;			//返回有效数据长度	
		ptcom->Current_Times=0;								//当前发送次数			
		ptcom->send_add[0]=ptcom->address;					//读的是这个地址的数据
		ptcom->register_length=length*2;				  	//返回16位，共1个字，C为字节，故*2
		ptcom->Simens_Count=1; 								//全局变量，根据其值0或1对返回处理不同
		break;
	case 'T':
	case 'C':
		for (i=0;i<length;i++)
		{
			ps=83*i;	
			*(U8 *)(AD1+0+ps)=0x05;            				//头ENQ，即05
			*(U8 *)(AD1+1+ps)=asicc(((plcadd&0xf0)>>4)&0xf);//plc站地址，高位在前
			*(U8 *)(AD1+2+ps)=asicc(plcadd&0xf);
			*(U8 *)(AD1+3+ps)=0x52;            				//读数字量是对P,M,L,K读，就是R
			*(U8 *)(AD1+4+ps)=0x53;            				//下两个长度发送的是命令类型SS(单独)，即53 53
			*(U8 *)(AD1+5+ps)=0x53; 
			*(U8 *)(AD1+6+ps)=0x30;            				//8个块 
			*(U8 *)(AD1+7+ps)=0x38;
			
			for (j=0;j<8;j++)
			{			
				*(U8 *)(AD1+8+j*9+ps)=0x30;           		//下2个长度发送的是地址字符长度 
				*(U8 *)(AD1+9+j*9+ps)=0x37;
				
				*(U8 *)(AD1+10+j*9+ps)=0x25;
				switch (ptcom->registerr)
				{
					case 'T':
						*(U8 *)(AD1+11+j*9+ps)=0x54;
						break;
					case 'C':
						*(U8 *)(AD1+11+j*9+ps)=0x43;
						break;
				}
				*(U8 *)(AD1+12+j*9+ps)=0x58;
			
				add=b+length*i+j;							//每块地址+1，就是连续的读8块
				a1=add/1000;
				a2=(add-a1*1000)/100;
				a3=(add-a1*1000-a2*100)/10;
				a4=(add-a1*1000-a2*100-a3*10);
			    *(U8 *)(AD1+13+j*9+ps)=asicc(a1);    
				*(U8 *)(AD1+14+j*9+ps)=asicc(a2);   	
			    *(U8 *)(AD1+15+j*9+ps)=asicc(a3);    
				*(U8 *)(AD1+16+j*9+ps)=asicc(a4);
			}
			 				
			*(U8 *)(AD1+80+ps)=0x04;           				//结束字符		
		
			aakj=CalcHe((U8 *)(AD1+ps),81);      			//和校验，校验前的数求和
			a1=aakj&0xff;                   				//只发最低位
			*(U8 *)(AD1+81+ps)=asicc((a1 >> 4) & 0xf);             				//LG的校验不用转asicc
			*(U8 *)(AD1+82+ps)=asicc((a1 >> 0) & 0xf);             				//LG的校验不用转asicc
						
			ptcom->send_length[i]=83;						//发送长度
			ptcom->send_staradd[i]=ps;						//发送数据存储地址	
									
			ptcom->return_length[i]=41;						//返回数据长度
			ptcom->return_start[i]=8;						//返回数据有效开始
			ptcom->return_length_available[i]=32;			//返回有效数据长度				
			ptcom->send_add[i]=ptcom->address+8*i;			//读的是这个地址的数据
			ptcom->address=ptcom->address+8*i;					
		}
		ptcom->send_times=length;							//发送次数
		ptcom->Current_Times=0;								//当前发送次数
		ptcom->Simens_Count=2; 								//全局变量，根据其值0或1对返回处理不同
		break; 	
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Read_Analog()//读模拟量
{
	U16 aakj;
	int b;
	int a1,a2,a3,a4;
	int plcadd;
	int length;
	int b1,b2;
	
	plcadd=ptcom->plc_address;	    						//PLC站地址
	b=ptcom->address;			    						// 寄存器所给地址
	length=ptcom->register_length;  						//读的长度
	     

	*(U8 *)(AD1+0)=0x05;            						//头ENQ，即05
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);        	//plc站地址，高位在前
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);	
	*(U8 *)(AD1+3)=0x72;            						//读模拟量，就是R
	*(U8 *)(AD1+4)=0x53;            						//下两个长度发送的是命令类型SB(连续)，即53 42
	*(U8 *)(AD1+5)=0x42; 
	*(U8 *)(AD1+6)=0x30;            						//下2个长度发送的是地址字符和开始地址共7个长度 
	*(U8 *)(AD1+7)=0x37; 
	
	switch (ptcom->registerr)	    						//根据寄存器类型有不同的地址字符
	{
	case 'D': 
		*(U8 *)(AD1+8)=0x25;        						//下3个长度发送的是地址字符%DW       
		*(U8 *)(AD1+9)=0x44; 
	    *(U8 *)(AD1+10)=0x57;  
		break;	
			
	case 't':
		*(U8 *)(AD1+8)=0x25;        						//下3个长度发送的是地址字符%TW       
		*(U8 *)(AD1+9)=0x54; 
	    *(U8 *)(AD1+10)=0x57; 	                   
		break;
			
	case 'c': 
		*(U8 *)(AD1+8)=0x25;        						//下3个长度发送的是地址字符%CW       
		*(U8 *)(AD1+9)=0x43; 
	    *(U8 *)(AD1+10)=0x57;
		break;						
	}
	
	a1=(b/1000)&0xf;                       					//取开始地址的千位数
	a2=((b-a1*1000)/100)&0xf;              					//取          百位数
	a3=((b-a1*1000-a2*100)/10)&0xf;        					//取          十位数
	a4=(b-a1*1000-a2*100-a3*10)&0xf;       					//取          个位数
	
	*(U8 *)(AD1+11)=asicc(a1);  							//开始的地址，asicc显示，高位先发
	*(U8 *)(AD1+12)=asicc(a2);	
	*(U8 *)(AD1+13)=asicc(a3);          
	*(U8 *)(AD1+14)=asicc(a4);
	
	b1=(length>>4)&0xf;                 					//读的长度处理，b1为高位,b2为低位
	b2=length&0xf;
		
	*(U8 *)(AD1+15)=asicc(b1);          					//读的长度，asicc显示
	*(U8 *)(AD1+16)=asicc(b2);
	
	*(U8 *)(AD1+17)=0x04;               					//结束字符
		
	aakj=CalcHe((U8 *)AD1,18);   							//和校验，校验前的数求和
	a1=aakj&0xff;                  							//只发最低位
	//*(U8 *)(AD1+18)=a1;
	*(U8 *)(AD1+18)=asicc((a1 >> 4) & 0xf);        			
	*(U8 *)(AD1+19)=asicc((a1 >> 0) & 0xf);  
	
	ptcom->send_length[0]=20;								//发送长度
	ptcom->send_staradd[0]=0;								//发送数据存储地址	
	ptcom->send_times=1;									//发送次数
		
	ptcom->return_length[0]=13+length*4;					//返回数据长度，有11个固定
	ptcom->return_start[0]=10;								//返回数据有效开始
	ptcom->return_length_available[0]=length*4;				//返回有效数据长度	
	ptcom->Current_Times=0;									//当前发送次数	
	ptcom->send_add[0]=ptcom->address;						//读的是这个地址的数据
	ptcom->Simens_Count=3; 
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Read_Recipe()//读取配方
{
	U16 aakj;
	int b;
	int a1,a2,a3,a4;
	int i;
	int datalength;
	int p_start;
	int ps;
	int SendTimes;
	int LastTimeWord;										//最后一次发送长度
	int currentlength;
	int length;
	int plcadd;
	
		
	datalength=ptcom->register_length;						//发送总长度
	p_start=ptcom->address;									//开始地址
	length=ptcom->register_length;          				//读的长度，多少个配方
	plcadd=ptcom->plc_address;	            				//PLC站地址
	
	if(datalength>5000)                     				//限制长度
		datalength=5000;

	if(datalength%64==0)                    				//每次发16个配方，读的配方数刚好为16的倍数时
	{
		SendTimes=datalength/64;            				//发送的次数            
		LastTimeWord=64;                    				//最后一次发送的长度为16	
	}
	if(datalength%64!=0)                    				//每次发16个配方，读的配方数不是16的倍数时
	{
		SendTimes=datalength/64+1;          				//发送的次数
		LastTimeWord=datalength%64;         				//最后一次发送的长度为除16的余数	
	}
	
	for (i=0;i<SendTimes;i++)               				//小于发送次数继续发送
	{
		ps=20*i;                            				//每次发19个长度，第二次发就是19开始
		b=p_start+i*64;                     				//每次的开始地址
		
		a1=(b/1000)&0xf;                    				//取千位数
		a2=((b-a1*1000)/100)&0xf;           				//取百位数
		a3=((b-a1*1000-a2*100)/10)&0xf;     				//取十位数
		a4=(b-a1*1000-a2*100-a3*10)&0xf;    				//取个位数

		*(U8 *)(AD1+0+ps)=0x05;             				//头ENQ，即05
		*(U8 *)(AD1+1+ps)=asicc(((plcadd&0xf0)>>4)&0xf); 	//plc站地址，高位在前
		*(U8 *)(AD1+2+ps)=asicc(plcadd&0xf);
		*(U8 *)(AD1+3+ps)=0x72;            					//读配方，就是R
		*(U8 *)(AD1+4+ps)=0x53;            					//下两个长度发送的是命令类型SB(连续)，即53 42
		*(U8 *)(AD1+5+ps)=0x42; 
		*(U8 *)(AD1+6+ps)=0x30;            					//下2个长度发送的是地址字符和开始地址共7个长度 
		*(U8 *)(AD1+7+ps)=0x37; 
		*(U8 *)(AD1+8+ps)=0x25;            					//下3个长度发送的是地址字符%DW       
		*(U8 *)(AD1+9+ps)=0x44; 
	    *(U8 *)(AD1+10+ps)=0x57;
			
		*(U8 *)(AD1+11+ps)=asicc(a1);      					//开始的地址，asicc显示，高位先发
		*(U8 *)(AD1+12+ps)=asicc(a2);	
		*(U8 *)(AD1+13+ps)=asicc(a3);      
		*(U8 *)(AD1+14+ps)=asicc(a4);
		
		if (i!=(SendTimes-1))	          					//不是最后一次发送时
		{
			*(U8 *)(AD1+15+ps)=0x34;      					//固定长度16个，即0x10，转成asicc
			*(U8 *)(AD1+16+ps)=0x30;
			currentlength=64;             					//发送的数据长度
		}
		if (i==(SendTimes-1))	          					//最后一次发送时
		{
			*(U8 *)(AD1+15+ps)=asicc((LastTimeWord>>4)&0xf);//发送的长度为除16的余数LastTimeWord，高位先发
			*(U8 *)(AD1+16+ps)=asicc(LastTimeWord&0xf);
			currentlength=LastTimeWord;   					//发送的数据长度
		}	
			
		*(U8 *)(AD1+17+ps)=0x04;          					//结束字符
		
		aakj=CalcHe((U8 *)(AD1 + ps),18);        					//和校验，校验前的数求和
		a1=aakj&0xff;                     					//只发最低位
		//*(U8 *)(AD1+18+ps)=a1;               					//LG的校验发的是和校验的最低位，16进制
		*(U8 *)(AD1+18+ps)=asicc((a1 >> 4) & 0xf);        						
		*(U8 *)(AD1+19+ps)=asicc((a1 >> 0) & 0xf); 
		
		
		ptcom->send_length[i]=20;							//发送长度
		ptcom->send_staradd[i]=i*20;						//发送数据存储地址	
		ptcom->send_add[i]=p_start+i*64;					//读的是这个地址的数据	
		ptcom->send_data_length[i]=currentlength;			//不是最后一次都是16个
				
		ptcom->return_length[i]=13+currentlength*4;			//返回数据长度，有11个固定
		ptcom->return_start[i]=10;				    		//返回数据有效开始
		ptcom->return_length_available[i]=currentlength*4;	//返回有效数据长度		
	}
	ptcom->send_times=SendTimes;							//发送次数
	ptcom->Current_Times=0;					        		//当前发送次数	
	ptcom->Simens_Count=3;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Write_Analog()//写模拟量
{
	U16 aakj;
	int b,i;
	int a1,a2,a3,a4;
	int b1,b2;
	int length;
	int plcadd;
	int c,c0,c1,c2,c3,c4;
			
	b=ptcom->address;			    						//开始地址
	plcadd=ptcom->plc_address;	    						//PLC站地址
	length=ptcom->register_length;  						//写的配方数
	
	*(U8 *)(AD1+0)=0x05;            						//头ENQ，即05
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf); 			//plc站地址，高位在前
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);
	
	*(U8 *)(AD1+3)=0x77;            						//写配方，就是W
	*(U8 *)(AD1+4)=0x53;            						//下两个长度发送的是命令类型SB(连续)，即53 42
	*(U8 *)(AD1+5)=0x42; 
	*(U8 *)(AD1+6)=0x30;            						//下2个长度发送的是地址字符和开始地址共7个长度 
	*(U8 *)(AD1+7)=0x37; 
	
	switch (ptcom->registerr)	    						//根据寄存器类型有不同的地址字符
	{
	case 'D': 
		*(U8 *)(AD1+8)=0x25;        						//下3个长度发送的是地址字符%DW       
		*(U8 *)(AD1+9)=0x44; 
	    *(U8 *)(AD1+10)=0x57;       
		break;	
			
	case 't':
		*(U8 *)(AD1+8)=0x25;        						//下3个长度发送的是地址字符%TW       
		*(U8 *)(AD1+9)=0x54; 
	    *(U8 *)(AD1+10)=0x57;         
		break;
			
	case 'c': 
		*(U8 *)(AD1+8)=0x25;        						//下3个长度发送的是地址字符%CW       
		*(U8 *)(AD1+9)=0x43; 
	    *(U8 *)(AD1+10)=0x57;
		break;						
	}
	a1=(b/1000)&0xf;                       					//取千位数
	a2=((b-a1*1000)/100)&0xf;              					//取百位数
	a3=((b-a1*1000-a2*100)/10)&0xf;        					//取十位数
	a4=(b-a1*1000-a2*100-a3*10)&0xf;       					//取个位数
	
	*(U8 *)(AD1+11)=asicc(a1);  							//开始的地址，asicc显示，高位先发
	*(U8 *)(AD1+12)=asicc(a2);	
	*(U8 *)(AD1+13)=asicc(a3);    
	*(U8 *)(AD1+14)=asicc(a4);
	
	b1=(length>>4)&0xf;             						//写的配方数处理，高位为b1，低位为b2
	b2=length&0xf;
		
	*(U8 *)(AD1+15)=asicc(b1);       						//读的长度，asicc显示
	*(U8 *)(AD1+16)=asicc(b2);
	
	for(i=0;i<length;i++)
	{			
	c=ptcom->U8_Data[i*2];   								//从D[]数组中要数据，对应的c0为高位，c为低位
	c0=ptcom->U8_Data[i*2+1];
						
	c1=c&0xf;                								//对所写数据作asicc处理，c为低位
	c2=(c>>4)&0xf;           								//对所写数据作asicc处理，c为低位		
	c3=c0&0xf;               								//对所写数据作asicc处理，c1为高位
	c4=(c0>>4)&0xf;          								//对所写数据作asicc处理，c1为高位				
	
	*(U8 *)(AD1+17+i*4)=asicc(c4);        					//数据是高位先发
	*(U8 *)(AD1+18+i*4)=asicc(c3);
	*(U8 *)(AD1+19+i*4)=asicc(c2);
	*(U8 *)(AD1+20+i*4)=asicc(c1);	
	}
	
	*(U8 *)(AD1+17+length*4)=0x04;        					//结束字符
	
	aakj=CalcHe((U8 *)AD1,18+length*4);   					//和校验，校验前的数求和
	a1=aakj&0xff;                         					//只发最低位
	//*(U8 *)(AD1+18+length*4)=a1;          					//LG的校验发16进制	
	*(U8 *)(AD1+18+length*4)=asicc((a1 >> 4) & 0xf);          					//LG的校验发16进制	
	*(U8 *)(AD1+19+length*4)=asicc((a1 >> 0) & 0xf);          					//LG的校验发16进制	

	ptcom->send_length[0]=20+length*4;						//发送长度
	ptcom->send_staradd[0]=0;								//发送数据存储地址	
	ptcom->send_times=1;									//发送次数
			
	ptcom->return_length[0]=9;								//返回数据长度
	ptcom->return_start[0]=0;								//返回数据有效开始
	ptcom->return_length_available[0]=0;					//返回有效数据长度	
	ptcom->Current_Times=0;									//当前发送次数
	ptcom->Simens_Count=0;	
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
void Write_Recipe()//写配方到PLC
{
	int datalength;
	int staradd;
	int SendTimes;
	int LastTimeWord;										//最后一次发送长度
	int i,j;
	int ps;
	int b;
	int a1,a2,a3,a4;
	int b1,b2,b3,b4;
	int c1,c2,c3,c4;
	U16 aakj;
	int plcadd;
	int length;
		
	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));		//数据长度
	staradd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//数据长度
	plcadd=ptcom->plc_address;	                   			//PLC站地址

	if(datalength%64==0)                           			//写的配方数刚好是16的倍数时             
	{
		SendTimes=datalength/64;                   			//发送的次数
		LastTimeWord=64;                           			//最后一次发送的长度	
	}
	if(datalength%64!=0)                           			//不是16的倍数时
	{
		SendTimes=datalength/64+1;                 			//发送的次数
		LastTimeWord=datalength%64;                			//最后一次发送的长度	
	}	
	
	ps=276;                                         			//写16个配方时，发送的是83个长度
	for (i=0;i<SendTimes;i++)
	{
		if (i!=(SendTimes-1))                      			//不是最后一次发送时
		{	
			length=64;                             			//发16个
		}
		else                                       			//最后一次发送时
		{
			length=LastTimeWord;                   			//发剩余的配方数             
		}

		*(U8 *)(AD1+0+ps*i)=0x05;            				//头ENQ，即05
		*(U8 *)(AD1+1+ps*i)=asicc(((plcadd&0xf0)>>4)&0xf);	//plc站地址，高位在前
		*(U8 *)(AD1+2+ps*i)=asicc(plcadd&0xf);
		*(U8 *)(AD1+3+ps*i)=0x77;            				//写配方，就是W
		*(U8 *)(AD1+4+ps*i)=0x53;            				//下两个长度发送的是命令类型SB(连续)，即53 42
		*(U8 *)(AD1+5+ps*i)=0x42; 
		*(U8 *)(AD1+6+ps*i)=0x30;            				//下2个长度发送的是地址字符和开始地址共7个长度 
		*(U8 *)(AD1+7+ps*i)=0x37; 
		*(U8 *)(AD1+8+ps*i)=0x25;            				//下3个长度发送的是地址字符%DW       
		*(U8 *)(AD1+9+ps*i)=0x44; 
	    *(U8 *)(AD1+10+ps*i)=0x57;
	    
		b=staradd+i*64;                        				//每次偏移16
		a1=(b/1000)&0xf;                       				//取千位数
		a2=((b-a1*1000)/100)&0xf;             				//取百位数
		a3=((b-a1*1000-a2*100)/10)&0xf;        				//取十位数
		a4=(b-a1*1000-a2*100-a3*10)&0xf;       				//取个位数
			    
		*(U8 *)(AD1+11+ps*i)=asicc(a1);        				//开始的地址，asicc显示，高位先发
		*(U8 *)(AD1+12+ps*i)=asicc(a2);	
		*(U8 *)(AD1+13+ps*i)=asicc(a3);     
		*(U8 *)(AD1+14+ps*i)=asicc(a4);
		
		b1=(length>>4)&0xf;                    				//写的个数处理，b1是高位，b2是低位
		b2=length&0xf;
		
		*(U8 *)(AD1+15+ps*i)=asicc(b1);        				//长度，asicc显示
		*(U8 *)(AD1+16+ps*i)=asicc(b2);
		
		for(j=0;j<length;j++)
		{
			b3=*(U8 *)(PE+9+i*128+j*2);         				//从数组中取数据，B3是低位，B4是高位
			b4=*(U8 *)(PE+9+i*128+j*2+1);
				
			c1=b3&0xf;               						//对所写数据作asicc处理
			c2=(b3>>4)&0xf;          						//对所写数据作asicc处理		
			c3=b4&0xf;               						//对所写数据作asicc处理
			c4=(b4>>4)&0xf;          						//对所写数据作asicc处理				
		
			*(U8 *)(AD1+17+j*4+ps*i)=asicc(c4);   			//高位先发
			*(U8 *)(AD1+18+j*4+ps*i)=asicc(c3);
			*(U8 *)(AD1+19+j*4+ps*i)=asicc(c2);
			*(U8 *)(AD1+20+j*4+ps*i)=asicc(c1);				
		}
		*(U8 *)(AD1+17+ps*i+length*4)=0x04;	     			//结束字符
		
		aakj=CalcHe((U8 *)(AD1 + ps*i),18+length*4);   			//和校验，校验前的数求和
		a1=aakj&0xff;                  						//只发最低位
		//*(U8 *)(AD1+18+length*4+ps*i)=a1;					//16进制
		*(U8 *)(AD1+18+length*4+ps*i)=asicc((a1 >> 4) & 0xf);					//16进制
		*(U8 *)(AD1+19+length*4+ps*i)=asicc((a1 >> 0) & 0xf);					//16进制

		ptcom->send_length[i]=20+length*4;					//发送长度
		ptcom->send_staradd[i]=i*ps;						//发送数据存储地址	
		ptcom->return_length[i]=9;							//返回数据长度
		ptcom->return_start[i]=0;							//返回数据有效开始
		ptcom->return_length_available[i]=0;				//返回有效数据长度	
			
	}
	ptcom->send_times=SendTimes;							//发送次数
	ptcom->Current_Times=0;									//当前发送次数
	ptcom->Simens_Count=0;		
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void compxy(void)//处理成标准存储格式
{
	int i;
	unsigned char a1,a2,a3,a4;
	int b,b1;
	int t[8];
	if(ptcom->Simens_Count==1)                               //==1时。对返回来的数高低位交换
	{                                  
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/4;i++)	//返回的有用长度/4就是需要的长度
 		 {		
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4);
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+1);
			a3=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+2);
			a4=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+3);	
			a1=bsicc(a1);                           			//ASC玛返回，所以要转为16进制
			a2=bsicc(a2);
			a3=bsicc(a3);
			a4=bsicc(a4);
			b=(a1<<4)+a2;                          				//返回的前两个
			b1=(a3<<4)+a4;                         				//返回的后两个
			*(U8 *)(COMad+i*2)=b1;				   				//重新存,从第0个开始存，高低位不变
			*(U8 *)(COMad+i*2+1)=b;
		}
	}
	else if (ptcom->Simens_Count==2)
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/4;i++)	//返回的有用长度/4就是需要的长度
 		 {		
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+2);
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+3);	
			a1=bsicc(a1);
			a2=bsicc(a2);                       
			t[i]=(a1<<4)+a2;                     
		}
		b=(t[7]<<7)+(t[6]<<6)+(t[5]<<5)+(t[4]<<4)+(t[3]<<3)+(t[2]<<2)+(t[1]<<1)+t[0];
		*(U8 *)(COMad+0)=b;									//重新存,从第0个开始存，交换高低位			
	}
	else if (ptcom->Simens_Count==3)                        //对返回来的数高低位不变
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/4;i++)	//返回的有用长度/4就是需要的长度
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4);
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+1);
			a3=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+2);
			a4=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+3);	
			a1=bsicc(a1);                           			//ASC玛返回，所以要转为16进制
			a2=bsicc(a2);
			a3=bsicc(a3);
			a4=bsicc(a4);
			b=(a1<<4)+a2;                          				//返回的前两个
			b1=(a3<<4)+a4;                         				//返回的后两个
			*(U8 *)(COMad+i*2)=b;				   				//重新存,从第0个开始存，高低位不变
			*(U8 *)(COMad+i*2+1)=b1;	
		}
	}			
	ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;//长度减半	
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void watchcom(void)//检查数据校检
{
	unsigned int aakj=0;
	aakj=remark();
	if(aakj==1)												//校检玛正确
	{
		ptcom->IfResultCorrect=1;
		compxy();											//进入数据处理程序
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
int remark()				//返回来的数据计算校检码是否正确，LG返回的数没有校验，故不检验是否正确都return 1
{
	return 1;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
