/***********************************************************************************************************************************************************************
 * OMRON
 * 说明
 * DIO				ddd.dd	0~3189.15		位：映射地址：Y								
 * W				ddd.dd	0~6143.15		位：映射地址：H								
 * H				ddd.dd	0~511.15		位：映射地址：M								
 * A				ddd.dd	0~959.15		位：映射地址：K								
 * D				ddd		0~32767			位：映射地址：D								
 * T				ddd		0~4096			字：映射地址：T	
 * C				ddd		0~4096			字：映射地址：C	
 * T*				ddd		0~4096			字：映射地址：t	
 * C*				ddd		0~4096			字：映射地址：c	
 * DIO_W			ddd		0~3189			字：映射地址：R								
 * H_W				ddd		0~511			字：映射地址：N								
***********************************************************************************************************************************************************************/
#include "stdio.h"
#include "def.h"
#include "smm.h"


		
struct Com_struct_D *ptcom;



/***********************************************************************************************************************************************************************
 * Function: 底层进入驱动接口
 * Parameters: 无
 * Return: true: 无
***********************************************************************************************************************************************************************/
void Enter_PlcDriver(void)
{
	ptcom=(struct Com_struct_D *)adcom;	
	switch (ptcom->R_W_Flag)
	{
	case PLC_READ_DATA:						//进入驱动是读数据
	case PLC_READ_DIGITALALARM:					//进入驱动是读数据,报警	
	case PLC_READ_TREND:					//进入驱动是读数据,趋势图
	case PLC_READ_ANALOGALARM:					//进入驱动是读数据,类比报警	
	case PLC_READ_CONTROL:					//进入驱动是读PLC控制的数据	
		switch(ptcom->registerr)
		{
		case 'M':
		case 'Y':
		case 'H':
		case 'K':
		case 'T':
		case 'C':						
			Read_Bool();   		//进入驱动是读位数据       
			break;
		case 'D':
		case 'R':
		case 't':
		case 'c':
		case 'N':	
			Read_Analog();  		//进入驱动是读模拟数据 
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
		case 'H':
		case 'T':
		case 'C':				
			Set_Reset();      	//进入驱动是强置置位和复位
			break;
		case 'D':
		case 'R':
		case 't':
		case 'c':
		case 'N':
			Write_Analog();	  	//进入驱动是写模拟数据	
			break;	
		default:
			handshake();
			break;			
		}
		break;	
	case PLC_WRITE_TIME:						//进入驱动是写时间到PLC
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
	case PLC_READ_TIME:						//进入驱动是读取时间到PLC
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
	case PLC_WRITE_RECIPE:						//进入驱动是写配方到PLC
		switch(*(U8 *)(PE+3))	//配方寄存器名称
		{
		case 'D':		
			Write_Recipe();		
			break; 
		default:
			handshake();
			break;				
		}
		break;
	case PLC_READ_RECIPE:						//进入驱动是从PLC读取配方
		switch(*(U8 *)(PE+3))	//配方寄存器名称
		{
		case 'D':		
			Read_Recipe();		
			break;
		default:
			handshake();
			break;				
		}
		break;	
	case 7:						//进入驱动是把PLC的状态设置为监视
		handshake();
		break;									
	case PLC_CHECK_DATA:						//进入驱动是数据处理
		watchcom();
		break;
	default:
		handshake();
		break;				
	}	 
}

/***********************************************************************************************************************************************************************
 * Function: 握手函数
 * Parameters: 无
 * Return: true: 无
***********************************************************************************************************************************************************************/
void handshake()
{
	int plcadd;
	U16 aakj;
	int a1,a2;	
	plcadd=ptcom->plc_address;						//PLC站地址
	
	*(U8 *)(AD1+0)=0x40;
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);  //plc站地址，高位在前
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);	
	*(U8 *)(AD1+3)=0x53;
	*(U8 *)(AD1+4)=0x43;
	*(U8 *)(AD1+5)=0x30;
	*(U8 *)(AD1+6)=0x31;
		
	aakj=CalFCS((U8 *)AD1,7);    					//FCS校验，高位在前，低位在后         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+7)=asicc(a1);    					//asicc码显示
	*(U8 *)(AD1+8)=asicc(a2);
	*(U8 *)(AD1+9)=0x2a;         					//结束字元2a,0d
	*(U8 *)(AD1+10)=0x0d;
		
	ptcom->send_length[0]=11;						//发送长度
	ptcom->send_staradd[0]=0;						//发送数据存储地址	
	ptcom->send_times=1;							//发送次数
		
	ptcom->return_length[0]=11;						//返回数据长度
	ptcom->return_start[0]=0;						//返回数据有效开始
	ptcom->return_length_available[0]=0;			//返回有效数据长度	
	ptcom->Current_Times=0;							//当前发送次数		
}

/***********************************************************************************************************************************************************************
 * Function: 置位和复位
 * Parameters: 无
 * Return: true: 无
***********************************************************************************************************************************************************************/
void Set_Reset()                
{
	U16 aakj;
	int b,b1,b2,b3;
	int a1,a2,a3,a4;
	int plcadd;
	int t;	

	b=ptcom->address;									// 开始置位地址
	plcadd=ptcom->plc_address;							//PLC站地址
	
	switch (ptcom->registerr)							//根据不同的寄存器，处理开始地址
	{
	case 'Y':
	case 'M':
	case 'H':
	case 'K':
		t=b/16;                						//DIO WR AR HR 的地址格式都是xxxx.xx，开始地址是xxxxxx，现除100取整数部分		
		a1=(t>>12)&0xf;         						//取整数部分的千位数
		a2=(t>>8)&0xf;          						//取整数部分的百位数
		a3=(t>>4)&0xf;          						//取整数部分的十位数
		a4=t&0xf;               						//取整数部分的个位数
		
		b1=b%16;             						//取小数部分，小数部分00-15
		b2=(b1>>4)&0xf;        						//取小数部分的十分位
		b3=(b1)&0xf;          	 						//取小数部分的百分位
		break;
	case 'T':	
	case 'C':
		a1=(b/1000)&0xf;                      			//取整数部分的千位数
		a2=((b-a1*1000)/100)&0xf;             			//取整数部分的百位数
		a3=((b-a1*1000-a2*100)/10)&0xf;       			//取整数部分的十位数
		a4=((b-a1*1000-a2*100-a3*10)/1)&0xf;  			//取整数部分的个位数
		break;		
	}
	
	switch (ptcom->registerr)							//根据不同的寄存器，发送不同的代码
	{
	case 'Y':                   						//DIO WR HR AR区用FINS 格式发送代码
	case 'M':
	case 'H':
	case 'K':
		*(U8 *)(AD1+0)=0x40;                        	//OMRON开始字元"@",即40
		*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);	//plc站地址，高位在前
		*(U8 *)(AD1+2)=asicc(plcadd&0xf);
		
		*(U8 *)(AD1+3)=0x46;                        	//header code FA，即46 41                    
		*(U8 *)(AD1+4)=0x41;  
		
		*(U8 *)(AD1+5)=0x30;                          	//response wait time
		         
		*(U8 *)(AD1+6)=0x30;                          	//ICF 固定30 30    	
		*(U8 *)(AD1+7)=0x30;    	
		*(U8 *)(AD1+8)=0x30;                          	//DA2 固定30 30          
		*(U8 *)(AD1+9)=0x30;    	
		*(U8 *)(AD1+10)=0x30;                         	//SA2 固定30 30   	
		*(U8 *)(AD1+11)=0x30;         
		*(U8 *)(AD1+12)=0x30;                         	//SID 固定30 30     	
		*(U8 *)(AD1+13)=0x30;
		 	
		*(U8 *)(AD1+14)=0x30;                         	//FINS command code 固定30 31 30 32  		
		*(U8 *)(AD1+15)=0x31; 	
		*(U8 *)(AD1+16)=0x30;         
		*(U8 *)(AD1+17)=0x32;	
		
		switch (ptcom->registerr)	                  	//根据寄存器的不同，有不同的操作数
		{
		case 'Y': 
			*(U8 *)(AD1+18)=0x33;                     	//DIO区	
			*(U8 *)(AD1+19)=0x30;
			break;
		case 'H': 
			*(U8 *)(AD1+18)=0x33;                     	//WR区	
			*(U8 *)(AD1+19)=0x31;
			break;
		case 'M': 
			*(U8 *)(AD1+18)=0x33;                     	//HR区	
			*(U8 *)(AD1+19)=0x32;
			break;
		case 'K':
			*(U8 *)(AD1+18)=0x33;                     	//AR区 	
			*(U8 *)(AD1+19)=0x33;
			break;
		}	
		
		*(U8 *)(AD1+20)=asicc(a1);                    	//开始地址整数部分，由高到低
		*(U8 *)(AD1+21)=asicc(a2);
		*(U8 *)(AD1+22)=asicc(a3);
		*(U8 *)(AD1+23)=asicc(a4);	
		
		*(U8 *)(AD1+24)=asicc(b2);                    	//开始地址整数部分                    
		*(U8 *)(AD1+25)=asicc(b3);	
		
		*(U8 *)(AD1+26)=0x30;                         	//FINS response code 固定30 30 30 31         
		*(U8 *)(AD1+27)=0x30;    	
		*(U8 *)(AD1+28)=0x30; 	
		*(U8 *)(AD1+29)=0x31;
		
		if (ptcom->writeValue==1)	                  	//置位命令
		{
			*(U8 *)(AD1+30)=0x30;                     	//置1
			*(U8 *)(AD1+31)=0x31;
		}
		
		if (ptcom->writeValue==0)	                  	//复位命令
		{
			*(U8 *)(AD1+30)=0x30;                     	//置0
			*(U8 *)(AD1+31)=0x30;
		}	
		
		aakj=CalFCS((U8 *)AD1,32);                    	//FCS校验，高位在前，低位在后         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+32)=asicc(a1);                    	//asicc码显示
		*(U8 *)(AD1+33)=asicc(a2);
		*(U8 *)(AD1+34)=0x2a;                         	//结束字元2a,0d
		*(U8 *)(AD1+35)=0x0d;	
		
		ptcom->send_length[0]=36;				      	//发送长度
		ptcom->send_staradd[0]=0;				      	//发送数据存储地址	
		ptcom->send_times=1;					      	//发送次数
			
		ptcom->return_length[0]=27;                   	//返回数据长度
		ptcom->return_start[0]=0;				      	//返回数据有效开始
		ptcom->return_length_available[0]=0 ;	      	//返回有效数据长度	
		ptcom->Current_Times=0;					      	//当前发送次数
		
		ptcom->Simens_Count=0;					      	//位返回，不同的返回有不同的数据处理方式
		break;
			
	case 'T':                                         	//T和C比较特殊，用HOST LINKS格式发送代码 	
	case 'C':
		*(U8 *)(AD1+0)=0x40;                          	//OMRON开始字元"@",即40
		*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf); 	//plc站地址，高位在前
		*(U8 *)(AD1+2)=asicc(plcadd&0xf);
		
		if (ptcom->writeValue==1)	                  	//置位命令
		{
			*(U8 *)(AD1+3)=0x4b;                      	//功能码KS（强置置位）
			*(U8 *)(AD1+4)=0x53;
		}
		
		if (ptcom->writeValue==0)	                  	//复位命令
		{
			*(U8 *)(AD1+3)=0x4b;                      	//功能码KR（强置复位）
			*(U8 *)(AD1+4)=0x52;
		}
		
		switch (ptcom->registerr)	                  	//根据寄存器的不同，有不同的操作数
		{
		case 'T':
			*(U8 *)(AD1+5)=0x54;                      	//对T区，操作数为T I M 空格
			*(U8 *)(AD1+6)=0x49;
			*(U8 *)(AD1+7)=0x4d;
			*(U8 *)(AD1+8)=0x20;
			break;
		case 'C':
			*(U8 *)(AD1+5)=0x43;                      	//对C区，操作数为C N T 空格
			*(U8 *)(AD1+6)=0x4e;
			*(U8 *)(AD1+7)=0x54;
			*(U8 *)(AD1+8)=0x20;
			break;
		}
		
		*(U8 *)(AD1+9)=asicc(a1);                     	//开始地址，依次从高到低，要转成asicc码
		*(U8 *)(AD1+10)=asicc(a2);
		*(U8 *)(AD1+11)=asicc(a3);
		*(U8 *)(AD1+12)=asicc(a4);
		*(U8 *)(AD1+13)=0x30;                         	//小数位，由高到低 
		*(U8 *)(AD1+14)=0x30;
		
		aakj=CalFCS((U8 *)AD1,15);                    	//FCS校验，高位在前，低位在后         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+15)=asicc(a1);                    	//asicc码显示
		*(U8 *)(AD1+16)=asicc(a2);
		*(U8 *)(AD1+17)=0x2a;                         	//结束字元2a,0d
		*(U8 *)(AD1+18)=0x0d;
		
		ptcom->send_length[0]=19;				      	//发送长度
		ptcom->send_staradd[0]=0;				      	//发送数据存储地址	
		ptcom->send_times=1;					      	//发送次数
			
		ptcom->return_length[0]=11;				      	//返回数据长度
		ptcom->return_start[0]=0;				      	//返回数据有效开始
		ptcom->return_length_available[0]=0;	      	//返回有效数据长度	
		ptcom->Current_Times=0;					      	//当前发送次数
		
		ptcom->Simens_Count=0;					      	//位返回，不同的返回有不同的数据处理方式
		break;
	}			
}

/***********************************************************************************************************************************************************************
 * Function: 读取数字量的信息
 * Parameters: 无
 * Return: true: 无
***********************************************************************************************************************************************************************/
void Read_Bool()				                          
{
	U16 aakj;
	int b,t,c;
	int a1,a2,a3,a4;
	int plcadd;	
	int c1,c2,c3,c4;	
		
	b=ptcom->address;			                      	//开始地址
	plcadd=ptcom->plc_address;	                      	//PLC站地址

	
	switch (ptcom->registerr)	                      	//根据不同的寄存器，处理开始地址
	{
	case 'Y':
	case 'M':
	case 'K':
	case 'H':				
		//b=(b+8)/100;				                  	//向整百靠拢
		//t=b;
		//ptcom->address=t*100;                         	//地址重写
		b=ptcom->address/16;
		if(ptcom->R_W_Flag)
		{
			ptcom->address=b*16;                         	//地址重写
		}
		
		a1=(b>>12)&0xf;                               	//取整数部分的千位数
		a2=(b>>8)&0xf;                                	//取整数部分的百位数
		a3=(b>>4)&0xf;                                	//取整数部分的十位数
		a4=b&0xf;                                     	//取整数部分的个位数
		break;
	case 'T':
		a1=(b>>12)&0xf;                               	//取整数部分的千位数
		a2=(b>>8)&0xf;                                	//取整数部分的百位数
		a3=(b>>4)&0xf;                                	//取整数部分的十位数
		a4=b&0xf;                                     	//取整数部分的个位数
		break;	
	case 'C':
		b=b+0x8000;
		a1=(b>>12)&0xf;                               	//取整数部分的千位数
		a2=(b>>8)&0xf;                                	//取整数部分的百位数
		a3=(b>>4)&0xf;                                	//取整数部分的十位数
		a4=b&0xf;                                     	//取整数部分的个位数
		break;		
	}

	*(U8 *)(AD1+0)=0x40;                              	//OMRON开始字元"@",既40
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);     	//plc站地址，高位在前
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);
	
	*(U8 *)(AD1+3)=0x46;                          		//header code FA，即46 41                    
	*(U8 *)(AD1+4)=0x41;  
		
	*(U8 *)(AD1+5)=0x30;                          		//response wait time
		         
	*(U8 *)(AD1+6)=0x30;                          		//ICF 固定30 30    	
	*(U8 *)(AD1+7)=0x30;    	
	*(U8 *)(AD1+8)=0x30;                          		//DA2 固定30 30          
	*(U8 *)(AD1+9)=0x30;    	
	*(U8 *)(AD1+10)=0x30;                         		//SA2 固定30 30   	
	*(U8 *)(AD1+11)=0x30;         
	*(U8 *)(AD1+12)=0x30;                         		//SID 固定30 30     	
	*(U8 *)(AD1+13)=0x30;
		 	
	*(U8 *)(AD1+14)=0x30;                         		//FINS command code 固定30 31 30 31  			
	*(U8 *)(AD1+15)=0x31; 	
	*(U8 *)(AD1+16)=0x30;         
	*(U8 *)(AD1+17)=0x31; 
	   
	switch (ptcom->registerr)	                  		//根据寄存器的不同，有不同的操作数
	{	
	case 'Y':
		*(U8 *)(AD1+18)=0x42;                     		//DIO区	
		*(U8 *)(AD1+19)=0x30;
		break;
	case 'H':
		*(U8 *)(AD1+18)=0x42;                     		//WR区	 	 	
		*(U8 *)(AD1+19)=0x31;
		break;
	case 'M': 	
		*(U8 *)(AD1+18)=0x42;                     		//HR区	 	
		*(U8 *)(AD1+19)=0x32;
		break;
	case 'K': 	
		*(U8 *)(AD1+18)=0x42;                     		//AR区	 	
		*(U8 *)(AD1+19)=0x33;
		break;
	case 'T': 	
	case 'C':
		*(U8 *)(AD1+18)=0x30;                     		//T/C区	 	 	
		*(U8 *)(AD1+19)=0x39;
		break;
	}
	
	switch (ptcom->registerr)	                  		//根据寄存器的不同，发送不同的代码
	{
	case 'Y':
	case 'H':
	case 'M':
	case 'K':
		*(U8 *)(AD1+20)=asicc(a1);                		//开始地址
		*(U8 *)(AD1+21)=asicc(a2);
		*(U8 *)(AD1+22)=asicc(a3);
		*(U8 *)(AD1+23)=asicc(a4);	
		
		*(U8 *)(AD1+24)=0x30;                     		//每次读16位 	
		*(U8 *)(AD1+25)=0x30;
		
		c=ptcom->register_length;                 		//读多少个元件
		c=(c/2)+(c%2);                            		//多读1个字节
		//c1=(c/1000)&0xf;                         	 	//取千位数
		//c2=((c-c1*1000)/100)&0xf;                 		//取百位数
		//c3=((c-c1*1000-c2*100)/10)&0xf;           		//取十位数
		//c4=(c-c1*1000-c2*100-c3*10)&0xf;          		//取个位数 
		c1=(c>>12)&0xf;                           		//取千位数
		c2=(c>>8)&0xf;                            		//取百位数
		c3=(c>>4)&0xf;                            		//取十位数
		c4=c&0xf;
		*(U8 *)(AD1+26)=asicc(c1);	              		//发送元件个数，以asicc码发送，依次从高到低
		*(U8 *)(AD1+27)=asicc(c2);
		*(U8 *)(AD1+28)=asicc(c3);
		*(U8 *)(AD1+29)=asicc(c4);
		
		aakj=CalFCS((U8 *)AD1,30);                		//FCS校验，高位在前，低位在后         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+30)=asicc(a1);                		//asicc码显示
		*(U8 *)(AD1+31)=asicc(a2);
		*(U8 *)(AD1+32)=0x2a;                     		//结束字元2a,0d
		*(U8 *)(AD1+33)=0x0d;		
		
		ptcom->send_length[0]=34;				  		//发送长度
		ptcom->send_staradd[0]=0;				  		//发送数据存储地址	
		ptcom->send_times=1;					  		//发送次数
			
		ptcom->return_length[0]=27+c*4;           		//返回数据长度，有27个固定
		ptcom->return_start[0]=23;				  		//返回数据有效开始
		ptcom->return_length_available[0]=c*4 ;	  		//返回有效数据长度	
		ptcom->Current_Times=0;					  		//当前发送次数	
		ptcom->send_add[0]=ptcom->address;		  		//读的是这个地址的数据
   	
		ptcom->register_length=c*2;				  		//返回16位，共1个字，C为字节，故*2
		ptcom->Simens_Count=1;					  		//位返回，不同的返回有不同的数据处理方式	
		break;
	case 'T':
	case 'C':
		*(U8 *)(AD1+20)=asicc(a1);
		*(U8 *)(AD1+21)=asicc(a2);
		*(U8 *)(AD1+22)=asicc(a3);
		*(U8 *)(AD1+23)=asicc(a4);	
		
		*(U8 *)(AD1+24)=0x30; 	
		*(U8 *)(AD1+25)=0x30;
		
		c=ptcom->register_length;                 		//读多少个元件
		
		c=c*8;                                    		//T/C要求发的是位，系统传过来是字节，*8
		
		c1=(c>>12)&0xf;                           		//取千位数
		c2=(c>>8)&0xf;                            		//取百位数
		c3=(c>>4)&0xf;                            		//取十位数
		c4=c&0xf;                                 		//取个位数 
		*(U8 *)(AD1+26)=asicc(c1);	              		//发送元件个数，以asicc码发送，依次从高到低
		*(U8 *)(AD1+27)=asicc(c2);
		*(U8 *)(AD1+28)=asicc(c3);
		*(U8 *)(AD1+29)=asicc(c4);
		
		aakj=CalFCS((U8 *)AD1,30);                		//FCS校验，高位在前，低位在后         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+30)=asicc(a1);                		//asicc码显示
		*(U8 *)(AD1+31)=asicc(a2);
		*(U8 *)(AD1+32)=0x2a;                     		//结束字元2a,0d
		*(U8 *)(AD1+33)=0x0d;		
		
		ptcom->send_length[0]=34;				  		//发送长度
		ptcom->send_staradd[0]=0;				  		//发送数据存储地址	
		ptcom->send_times=1;					  		//发送次数
			 
		ptcom->return_length[0]=27+c*2;           		//返回数据长度，有27个固定
		ptcom->return_start[0]=23;				  		//返回数据有效开始
		ptcom->return_length_available[0]=c*2 ;	  		//返回有效数据长度	
		ptcom->Current_Times=0;					  		//当前发送次数	
		ptcom->send_add[0]=ptcom->address;		  		//读的是这个地址的数据  	
		ptcom->Simens_Count=2;					  		//位返回，不同的返回有不同的数据处理方式
		break;
	}
}

/***********************************************************************************************************************************************************************
 * Function: 读模拟量
 * Parameters: 无
 * Return: true: 无
***********************************************************************************************************************************************************************/
void Read_Analog()				                  
{
	U16 aakj;
	int b,c;
	int a1,a2,a3,a4;
	int c1,c2,c3,c4;
	int plcadd;
	
	plcadd=ptcom->plc_address;	                  //PLC站地址
	b=ptcom->address;			                  //开始地址
	
	switch (ptcom->registerr)	                  //根据不同的寄存器，处理开始地址
	{	
	case 'D':
	case 'R':
	case 't':
	case 'N':
		b=ptcom->address;			              //开始地址
		break;
	case 'c':
		b=b+0x8000;
		break;
	}

	a1=(b>>12)&0xf;                               //取千位数
	a2=(b>>8)&0xf;                                //取百位数
	a3=(b>>4)&0xf;                                //取十位数
	a4=b&0xf;                                     //取个位数	

	*(U8 *)(AD1+0)=0x40;                          //OMRON开始字元"@",既40
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf); //plc站地址，高位在前
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);
	*(U8 *)(AD1+3)=0x46;     
	*(U8 *)(AD1+4)=0x41;

	*(U8 *)(AD1+5)=0x30;
	         
	*(U8 *)(AD1+6)=0x30;    	
	*(U8 *)(AD1+7)=0x30;    	
	*(U8 *)(AD1+8)=0x30;         
	*(U8 *)(AD1+9)=0x30;    	
	*(U8 *)(AD1+10)=0x30; 	
	*(U8 *)(AD1+11)=0x30;         
	*(U8 *)(AD1+12)=0x30;    	
	*(U8 *)(AD1+13)=0x30;
	 	
	*(U8 *)(AD1+14)=0x30;		
	*(U8 *)(AD1+15)=0x31; 	
	*(U8 *)(AD1+16)=0x30;         
	*(U8 *)(AD1+17)=0x31;
	
	switch (ptcom->registerr)	                   //根据寄存器的不同，有不同的操作数
	{	
	case 'D':                                      //D区 
		*(U8 *)(AD1+18)=0x38; 	
		*(U8 *)(AD1+19)=0x32;
		break;
	case 'R':                                      //CIO区 
		*(U8 *)(AD1+18)=0x42; 	
		*(U8 *)(AD1+19)=0x30;
		break;
	case 'N':                                      //H区 
		*(U8 *)(AD1+18)=0x42; 	
		*(U8 *)(AD1+19)=0x32;
		break;
	case 't':                                      //T/C区
	case 'c': 	
		*(U8 *)(AD1+18)=0x38; 	
		*(U8 *)(AD1+19)=0x39;
		break;
	}
	
	*(U8 *)(AD1+20)=asicc(a1);            	       //开始地址
	*(U8 *)(AD1+21)=asicc(a2);
	*(U8 *)(AD1+22)=asicc(a3);
	*(U8 *)(AD1+23)=asicc(a4);	
		
	*(U8 *)(AD1+24)=0x30;                    	   //模拟量没有小数位，故都为30 30 	
	*(U8 *)(AD1+25)=0x30;
	 
	c=ptcom->register_length;                      //读多少个元件
	
	c1=(c>>12)&0xf;                                //取千位数
	c2=(c>>8)&0xf;                                 //取百位数
	c3=(c>>4)&0xf;                                 //取十位数
	c4=c&0xf;                                      //取个位数

	*(U8 *)(AD1+26)=asicc(c1);	                   //发送元件个数，以asicc码发送，依次从高到低
	*(U8 *)(AD1+27)=asicc(c2);
	*(U8 *)(AD1+28)=asicc(c3);
	*(U8 *)(AD1+29)=asicc(c4);

	aakj=CalFCS((U8 *)AD1,30);                     //FCS校验，高位在前，低位在后         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+30)=asicc(a1);                     //asicc码显示
	*(U8 *)(AD1+31)=asicc(a2);
	*(U8 *)(AD1+32)=0x2a;                          //结束字元2a,0d
	*(U8 *)(AD1+33)=0x0d;	
	 
	ptcom->send_length[0]=34;				       //发送长度
	ptcom->send_staradd[0]=0;				       //发送数据存储地址	
	ptcom->send_times=1;						   //发送次数
		
	ptcom->return_length[0]=27+c*4;                //返回数据长度，有27个固定
	ptcom->return_start[0]=23;				       //返回数据有效开始
	ptcom->return_length_available[0]=c*4;	       //返回有效数据长度	
	ptcom->Current_Times=0;					       //当前发送次数	
	ptcom->send_add[0]=ptcom->address;		       //读的是这个地址的数据
		
	ptcom->Simens_Count=5;                         //字返回，根据不同的值来选择不同的数据处理方式
}

/***********************************************************************************************************************************************************************
 * Function: 读取配方
 * Parameters: 无
 * Return: true: 无
***********************************************************************************************************************************************************************/
void Read_Recipe()							       
{
	U16 aakj;
	int b;
	int c,c1,c2,c3,c4;
	int a1,a2,a3,a4;
	int i;
	int datalength;                                //数据长度
	int p_start;                                   //数据开始地址
	int ps;
	int SendTimes;                                 //发送次数
	int LastTimeWord;						       //最后一次发送长度
	int currentlength;
	int plcadd;                                    //PLC站地址
	
	datalength=ptcom->register_length;		       //发送总长度
	p_start=ptcom->address;					       //开始地址
	plcadd=ptcom->plc_address;	                   //PLC站地址
	
	if(datalength>5000)                            //限制长度
		datalength=5000;

	if(datalength%28==0)                           //最多能发28个D，数据刚好是28D的倍数
	{
		SendTimes=datalength/28;                   //发送次数
		LastTimeWord=28;                           //最后一次发送的长度为28D	
	}
	if(datalength%28!=0)                           //最多能发28个D，数据不是28D的倍数 
	{
		SendTimes=datalength/28+1;                 //发送的次数
		LastTimeWord=datalength%28;                //最后一次发送的长度为除28的余数	
	}
	
	for (i=0;i<SendTimes;i++)
	{
		ps=i*34;                                   //每次发34个长度
		b=p_start+i*28;                            //起始地址
		a1=(b>>12)&0xf;                            //取千位数
		a2=(b>>8)&0xf;                             //取百位数
		a3=(b>>4)&0xf;                             //取十位数
		a4=b&0xf;                                  //取个位数	

		*(U8 *)(AD1+0+ps)=0x40;                    //OMRON开始字元"@",既40
		*(U8 *)(AD1+1+ps)=asicc(((plcadd&0xf0)>>4)&0xf); //plc站地址，高位在前
		*(U8 *)(AD1+2+ps)=asicc(plcadd&0xf);
		*(U8 *)(AD1+3+ps)=0x46;     
		*(U8 *)(AD1+4+ps)=0x41;

		*(U8 *)(AD1+5+ps)=0x30;
		         
		*(U8 *)(AD1+6+ps)=0x30;    	
		*(U8 *)(AD1+7+ps)=0x30;    	
		*(U8 *)(AD1+8+ps)=0x30;         
		*(U8 *)(AD1+9+ps)=0x30;    	
		*(U8 *)(AD1+10+ps)=0x30; 	
		*(U8 *)(AD1+11+ps)=0x30;         
		*(U8 *)(AD1+12+ps)=0x30;    	
		*(U8 *)(AD1+13+ps)=0x30;
		 	
		*(U8 *)(AD1+14+ps)=0x30;		
		*(U8 *)(AD1+15+ps)=0x31; 	
		*(U8 *)(AD1+16+ps)=0x30;         
		*(U8 *)(AD1+17+ps)=0x31;
	
		*(U8 *)(AD1+18+ps)=0x38;                    //操作数	
		*(U8 *)(AD1+19+ps)=0x32;
		
		*(U8 *)(AD1+20+ps)=asicc(a1);               //开始地址，由高到低发送                
		*(U8 *)(AD1+21+ps)=asicc(a2);
		*(U8 *)(AD1+22+ps)=asicc(a3);
		*(U8 *)(AD1+23+ps)=asicc(a4);	
		
		*(U8 *)(AD1+24+ps)=0x30;                    //模拟量没有小数位，固定30 30 	
		*(U8 *)(AD1+25+ps)=0x30;
		
		if (i!=(SendTimes-1))	                    //不是最后一次发送时
		{
			*(U8 *)(AD1+26+ps)=0x30;                //固定长度28个D，即56字节，高位，asicc码显示
			*(U8 *)(AD1+27+ps)=0x30;
			*(U8 *)(AD1+28+ps)=0x31;                //固定长度28个D，即56字节，低位，asicc码显示
			*(U8 *)(AD1+29+ps)=0x43;
			currentlength=28;                       //固定长度28个D
		}
		if (i==(SendTimes-1))	                    //最后一次发送时
		{
			c=LastTimeWord;                         //读多少个元件
			c1=(c>>12)&0xf;                         //取千位数
			c2=(c>>8)&0xf;                          //取百位数
			c3=(c>>4)&0xf;                          //取十位数
			c4=c&0xf;                               //取个位数
			*(U8 *)(AD1+26+ps)=asicc(c1);           //发送的长度，依次从高到低，要转成asicc码
			*(U8 *)(AD1+27+ps)=asicc(c2);
			*(U8 *)(AD1+28+ps)=asicc(c3);
			*(U8 *)(AD1+29+ps)=asicc(c4);
			currentlength=LastTimeWord;             //剩余LastTimeWord个D
		}

		aakj=CalFCS((U8 *)(AD1+ps),30);             //FCS校验，高位在前，低位在后         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+30+ps)=asicc(a1);               //asicc码显示
		*(U8 *)(AD1+31+ps)=asicc(a2);
		*(U8 *)(AD1+32+ps)=0x2a;                    //结束字元2a,0d
		*(U8 *)(AD1+33+ps)=0x0d;
		
		ptcom->send_length[i]=34;				    //发送长度
		ptcom->send_staradd[i]=i*34;			    //发送数据存储地址	
		ptcom->send_add[i]=p_start+i*28;		    //读的是这个地址的数据	
		ptcom->send_data_length[i]=currentlength;	//不是最后一次都是28个D
				
		ptcom->return_length[i]=27+currentlength*4; //返回数据长度，有27个固定
		ptcom->return_start[i]=23;				    //返回数据有效开始
		ptcom->return_length_available[i]=currentlength*4;	//返回有效数据长度	
		
	}
	ptcom->send_times=SendTimes;					//发送次数
	ptcom->Current_Times=0;		
	ptcom->Simens_Count=5;	                        //字返回，根据不同的值来选择不同的数据处理方式
}

/***********************************************************************************************************************************************************************
 * Function: 写模拟量
 * Parameters: 无
 * Return: true: 无
***********************************************************************************************************************************************************************/
void Write_Analog()								    
{
	U16 aakj;
	int b;
	int a1,a2,a3,a4;
	int i;
	int c0,c01,c02,c03,c04;
	int c,c1,c2,c3,c4;
	int plcadd;
	
	plcadd=ptcom->plc_address;	                    //PLC站地址
	
	switch (ptcom->registerr)	                    //根据不同的寄存器，处理开始地址
	{	
	case 'D':
	case 'R':
	case 't':	
	case 'N':				
		b=ptcom->address;			                //开始地址
		break;
	case 'c':
		b=ptcom->address;			                    //开始地址
		b=b+0x8000;
		break; 
	}

	a1=(b>>12)&0xf;                                 //取千位数
	a2=(b>>8)&0xf;                                  //取百位数
	a3=(b>>4)&0xf;                                  //取十位数
	a4=b&0xf;                                       //取个位数	

	*(U8 *)(AD1+0)=0x40;                            //OMRON开始字元"@",既40
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);   //plc站地址，高位在前
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);
	*(U8 *)(AD1+3)=0x46;     
	*(U8 *)(AD1+4)=0x41;

	*(U8 *)(AD1+5)=0x30;
	         
	*(U8 *)(AD1+6)=0x30;    	
	*(U8 *)(AD1+7)=0x30;    	
	*(U8 *)(AD1+8)=0x30;         
	*(U8 *)(AD1+9)=0x30;    	
	*(U8 *)(AD1+10)=0x30; 	
	*(U8 *)(AD1+11)=0x30;         
	*(U8 *)(AD1+12)=0x30;    	
	*(U8 *)(AD1+13)=0x30;
	 	
	*(U8 *)(AD1+14)=0x30;		
	*(U8 *)(AD1+15)=0x31; 	
	*(U8 *)(AD1+16)=0x30;         
	*(U8 *)(AD1+17)=0x32;
	
	switch (ptcom->registerr)	                     //根据寄存器的不同，有不同的功能码
	{	
	case 'D':
		*(U8 *)(AD1+18)=0x38; 	
		*(U8 *)(AD1+19)=0x32;
		break;
	case 'R':                                      //CIO区 
		*(U8 *)(AD1+18)=0x42; 	
		*(U8 *)(AD1+19)=0x30;
		break;
	case 'N':                                      //H区 
		*(U8 *)(AD1+18)=0x42; 	
		*(U8 *)(AD1+19)=0x32;
		break;
	case 't':
		*(U8 *)(AD1+18)=0x38; 	 	
		*(U8 *)(AD1+19)=0x39;
		break;
	case 'c': 	
		*(U8 *)(AD1+18)=0x38; 	
		*(U8 *)(AD1+19)=0x39;
		break;
	}
	
	*(U8 *)(AD1+20)=asicc(a1);                        //开始地址
	*(U8 *)(AD1+21)=asicc(a2);
	*(U8 *)(AD1+22)=asicc(a3);
	*(U8 *)(AD1+23)=asicc(a4);	
		
	*(U8 *)(AD1+24)=0x30; 	                          //模拟量没小数位
	*(U8 *)(AD1+25)=0x30;
	 
	
	c=ptcom->register_length;                         //读多少个元件

	c1=(c>>12)&0xf;                                   //取千位数
	c2=(c>>8)&0xf;                                    //取百位数
	c3=(c>>4)&0xf;                                    //取十位数
	c4=c&0xf;                                         //取个位数

	*(U8 *)(AD1+26)=asicc(c1);	                      //发送元件个数，以asicc码发送，依次从高到低
	*(U8 *)(AD1+27)=asicc(c2);
	*(U8 *)(AD1+28)=asicc(c3);
	*(U8 *)(AD1+29)=asicc(c4);
	
	for (i=0;i<c;i++)                                  //写入多个元件值
	{				
		c0=ptcom->U8_Data[i*2];                        //从D[]数组中要数据，对应的c1为高位，c0为低位
		c1=ptcom->U8_Data[i*2+1];
						
		c01=c0&0xf;                                    //对所写数据作asicc处理，c0为低位
		c02=(c0>>4)&0xf;                               //对所写数据作asicc处理，c0为低位		
		c03=c1&0xf;                                    //对所写数据作asicc处理，c1为高位
		c04=(c1>>4)&0xf;                               //对所写数据作asicc处理，c0为低位				
	
		*(U8 *)(AD1+30+i*4)=asicc(c04);
		*(U8 *)(AD1+31+i*4)=asicc(c03);
		*(U8 *)(AD1+32+i*4)=asicc(c02);
		*(U8 *)(AD1+33+i*4)=asicc(c01);	
	}
		
	aakj=CalFCS((U8 *)AD1,30+c*4);           	     //FCS校验，高位在前，低位在后         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+30+c*4)=asicc(a1);          	     //asicc码显示
	*(U8 *)(AD1+31+c*4)=asicc(a2); 
	*(U8 *)(AD1+32+c*4)=0x2a;               	     //结束字元2a,0d
	*(U8 *)(AD1+33+c*4)=0x0d;

		
	ptcom->send_length[0]=34+c*4;		     	     //发送长度
	ptcom->send_staradd[0]=0;				     	 //发送数据存储地址	
	ptcom->send_times=1;					     	 //发送次数
			
	ptcom->return_length[0]=27;				     	 //返回数据长度
	ptcom->return_start[0]=0;				     	 //返回数据有效开始
	ptcom->return_length_available[0]=0;	     	 //返回有效数据长度		
	ptcom->Current_Times=0;	
	
	ptcom->Simens_Count=0;	                         //字返回，根据不同的值来选择不同的数据处理方式
}

/***********************************************************************************************************************************************************************
 * Function: 写配方到PLC
 * Parameters: 无
 * Return: true: 无
***********************************************************************************************************************************************************************/
void Write_Recipe()								    
{
	int datalength;
	int staradd;
	int SendTimes;                                  //发送的次数
	int LastTimeWord;							    //最后一次发送长度
	int i,j;
	int ps;
	int b;
	int c,c2,c3,c4;
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

    ps=146;                                    //当发超过28个D的时候，前面每次发146个

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
		
	b=staradd+i*28;                      	   //每次偏移28个D
		
	a1=(b>>12)&0xf;                       	   //取千位数
	a2=(b>>8)&0xf;                             //取百位数
	a3=(b>>4)&0xf;                             //取十位数
	a4=b&0xf;                                  //取个位数	

	*(U8 *)(AD1+0+ps*i)=0x40;                  //OMRON开始字元"@",既40
	*(U8 *)(AD1+1+ps*i)=asicc(((plcadd&0xf0)>>4)&0xf); //plc站地址，高位在前
	*(U8 *)(AD1+2+ps*i)=asicc(plcadd&0xf);
	*(U8 *)(AD1+3+ps*i)=0x46;     
	*(U8 *)(AD1+4+ps*i)=0x41;

	*(U8 *)(AD1+5+ps*i)=0x30;
	         
	*(U8 *)(AD1+6+ps*i)=0x30;    	
	*(U8 *)(AD1+7+ps*i)=0x30;    	
	*(U8 *)(AD1+8+ps*i)=0x30;         
	*(U8 *)(AD1+9+ps*i)=0x30;    	
	*(U8 *)(AD1+10+ps*i)=0x30; 	
	*(U8 *)(AD1+11+ps*i)=0x30;         
	*(U8 *)(AD1+12+ps*i)=0x30;    	
	*(U8 *)(AD1+13+ps*i)=0x30;
	 	
	*(U8 *)(AD1+14+ps*i)=0x30;		
	*(U8 *)(AD1+15+ps*i)=0x31; 	
	*(U8 *)(AD1+16+ps*i)=0x30;         
	*(U8 *)(AD1+17+ps*i)=0x32;
	
	*(U8 *)(AD1+18+ps*i)=0x38; 	
	*(U8 *)(AD1+19+ps*i)=0x32;
		
	*(U8 *)(AD1+20+ps*i)=asicc(a1);
	*(U8 *)(AD1+21+ps*i)=asicc(a2);
	*(U8 *)(AD1+22+ps*i)=asicc(a3);
	*(U8 *)(AD1+23+ps*i)=asicc(a4);	
		
	*(U8 *)(AD1+24+ps*i)=0x30; 	
	*(U8 *)(AD1+25+ps*i)=0x30;
	 
	
	c=length;                        //读多少个元件

	c1=(c>>12)&0xf;                  //取千位数
	c2=(c>>8)&0xf;                   //取百位数
	c3=(c>>4)&0xf;                   //取十位数
	c4=c&0xf;                        //取个位数

	*(U8 *)(AD1+26+ps*i)=asicc(c1);	 //发送元件个数，以asicc码发送，依次从高到低
	*(U8 *)(AD1+27+ps*i)=asicc(c2);
	*(U8 *)(AD1+28+ps*i)=asicc(c3);
	*(U8 *)(AD1+29+ps*i)=asicc(c4);

		for(j=0;j<length;j++)        //写入多个元件值                 
		{	
			c0=*(U8 *)(PE+9+i*56+j*2);//从存数据的寄存器开始地址PE+9取数据，k3为高位，k4为低位
			c1=*(U8 *)(PE+9+i*56+j*2+1);
							
			c01=c0&0xf;               //对所写数据作asicc处理，c0为低位
			c02=(c0>>4)&0xf;          //对所写数据作asicc处理，c0为低位		
			c03=c1&0xf;               //对所写数据作asicc处理，c1为高位
			c04=(c1>>4)&0xf;          //对所写数据作asicc处理，c0为低位				
	
			*(U8 *)(AD1+30+j*4+ps*i)=asicc(c04);
			*(U8 *)(AD1+31+j*4+ps*i)=asicc(c03);
			*(U8 *)(AD1+32+j*4+ps*i)=asicc(c02);
			*(U8 *)(AD1+33+j*4+ps*i)=asicc(c01);							
		}

		aakj=CalFCS((U8 *)(AD1+ps*i),30+length*4);      //FCS校验，高位在前，低位在后         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+34+(length-1)*4+ps*i)=asicc(a1);      //asicc码显示
		*(U8 *)(AD1+35+(length-1)*4+ps*i)=asicc(a2);
		*(U8 *)(AD1+36+(length-1)*4+ps*i)=0x2a;          //结束字元2a,0d
		*(U8 *)(AD1+37+(length-1)*4+ps*i)=0x0d;
		
		ptcom->send_length[i]=34+length*4;	   //发送长度
		ptcom->send_staradd[i]=i*ps;		   //发送数据存储地址	
		
		ptcom->return_length[i]=27;			   //返回数据长度
		ptcom->return_start[i]=0;			   //返回数据有效开始
		ptcom->return_length_available[i]=0;   //返回有效数据长度	
    }
	ptcom->send_times=SendTimes;			   //发送次数
	ptcom->Current_Times=0;
	ptcom->Simens_Count=0;					  //字返回				
}

/***********************************************************************************************************************************************************************
 * Function: 写时间到PLC
 * Parameters: 无
 * Return: true: 无
***********************************************************************************************************************************************************************/
void Write_Time()                                     
{
	Write_Analog();									
}

/***********************************************************************************************************************************************************************
 * Function: 从PLC读取时间
 * Parameters: 无
 * Return: true: 无
***********************************************************************************************************************************************************************/
void Read_Time()									     
{
	Read_Analog();
}

/***********************************************************************************************************************************************************************
 * Function: 处理成标准存储格式,重新排列
 * Parameters: 无
 * Return: true: 无
***********************************************************************************************************************************************************************/
void compxy(void)				              
{
	int i;
	unsigned char a1,a2,a3,a4,a5,a6,a7,a8;
	int b1,b2;
	if(ptcom->Simens_Count==1)                //位返回的处理，数据颠倒存储
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
		}
		ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;
	}
	else if(ptcom->Simens_Count==2)           //T/C读的数据返回处理，返回30 31表示开通，30 30表示断开
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/16;i++)
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*16+1);
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*16+3);
			a3=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*16+5);
			a4=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*16+7);
			a5=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*16+9);
			a6=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*16+11);
			a7=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*16+13);
			a8=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*16+15);
			a1=bsicc(a1);
			a2=bsicc(a2);
			a3=bsicc(a3);
			a4=bsicc(a4);
			a5=bsicc(a5);
			a6=bsicc(a6);
			a7=bsicc(a7);
			a8=bsicc(a8);
			b1=(a8<<7)+(a7<<6)+(a6<<5)+(a5<<4)+(a4<<3)+(a3<<2)+(a2<<1)+a1;
			
			*(U8 *)(COMad+i*2+0)=b1;
		}
		ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;
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
	//		if(ptcom->Simens_Count==1)	                         //读数字量的时候为1，倒顺序
	//		{
	//			*(U8 *)(COMad+i*2)=b2;					         //重新存,从第0个开始存
	//			*(U8 *)(COMad+i*2+1)=b1;					     //重新存,从第0个开始存
	//		}		
	//		else
	//		{ 
				*(U8 *)(COMad+i*2)=b1;					         //重新存,从第0个开始存
				*(U8 *)(COMad+i*2+1)=b2;					     //重新存,从第0个开始存		
	//		}	
		}
		ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;	//长度减半	
	} 
	else
	{
		ptcom->IfResultCorrect=0;
	}
                             
}

/***********************************************************************************************************************************************************************
 * Function: 检查数据校检
 * Parameters: 无
 * Return: true: 无
***********************************************************************************************************************************************************************/
void watchcom(void)		
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

/***********************************************************************************************************************************************************************
 * Function: 转为Asc码
 * Parameters: 无
 * Return: true: 无
***********************************************************************************************************************************************************************/
int asicc(int a)			
{
	int bl;
	if(a<10)
		bl=a+0x30;
	if(a>9)
		bl=a-10+0x41;
	return bl;	
}

/***********************************************************************************************************************************************************************
 * Function: Asc转为数字
 * Parameters: 无
 * Return: true: 无
***********************************************************************************************************************************************************************/
int bsicc(int a)			
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

/***********************************************************************************************************************************************************************
 * Function: 返回来的数据计算校检码是否正确
 * Parameters: 无
 * Return: true: 无
***********************************************************************************************************************************************************************/
int remark()				
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
	{
		return 1;
	}
	else
	{
		return 0;
	}	
}

/***********************************************************************************************************************************************************************
 * Function: 校验函数
 * Parameters: 无
 * Return: true: 无
***********************************************************************************************************************************************************************/
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

