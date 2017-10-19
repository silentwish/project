/***********************************************************************************************************************************************************************
 * FX485BD
 * 说明
 * X				ooo		0~377			位：映射地址：X								
 * Y				ooo 	0~377			位：映射地址：Y								
 * M				dddd	0~7999			位：映射地址：M	
 * SM				dddd	8000~8999		位：映射地址：H	
 * S				ddd		0~999			位：映射地址：K	
 * T				ddd		0~511			位：映射地址：T	
 * C				ddd		0~255			位：映射地址：C								
 * D_Bit			dddd.dd	0.00~7999.15	位：映射地址：L							
 * D				dddd	0~7999			字：映射地址：D								
 * SD				dddd	8000~8999		字：映射地址：N	
 * R				ddddd	0-32766			字：映射地址：R	
 * TV				ddd		0~511			字：映射地址：t	
 * CV				ddd		0~199			字：映射地址：c	
 * 32CV				ddd		200~255			双字：映射地址：c 地址偏移1000，如选择地址200，再加上1000进行偏移								
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
//	sysprintf("enter plc driver %c,ptcom->R_W_Flag %d\n",ptcom->registerr,ptcom->R_W_Flag);		

	switch (ptcom->R_W_Flag)
	{
	case PLC_READ_DATA:						//进入驱动是读数据
	case PLC_READ_DIGITALALARM:					//进入驱动是读数据,报警	
	case PLC_READ_TREND:					//进入驱动是读数据,趋势图
	case PLC_READ_ANALOGALARM:					//进入驱动是读数据,类比报警	
	case PLC_READ_CONTROL:					//进入驱动是读PLC控制的数据	
		switch(ptcom->registerr)
		{
		case 'X':
		case 'Y':
		case 'M':
		case 'H':
		case 'K':						
			Read_Bool();   		//进入驱动是读位数据       
			break;
		case 'T':		
		case 'C':
			Read_Bool_TC();		//进入驱动是读位数据  
			break;
		case 'D':
		case 'N':		
		case 't':					
			Read_Analog();  		//进入驱动是读模拟数据 
			break;
		case 'c':
			{
				if(ptcom->address > 1000)
				{
					Read_Analog_CV32();  		//进入驱动是读模拟数据 
				}
				else
				{
					Read_Analog();  		//进入驱动是读模拟数据 
				}
				break;
			}
		case 'R':
			Read_Analog_R();		//进入驱动是读模拟数据 
			break;				
		}
		break;
	case PLC_WRITE_DATA:				
		switch(ptcom->registerr)
		{		
		case 'Y':
		case 'M':
		case 'H':
		case 'K':
		case 'T':
		case 'C':				
			Set_Reset();      	//进入驱动是强置置位和复位
			break;
		case 'D':
		case 'N':
		case 'R':
		case 't':		
			Write_Analog();	  	//进入驱动是写模拟数据	
			break;	
		case 'c':
			{
				if(ptcom->address > 1000)
				{
					Write_Analog_CV32();  		//进入驱动是写模拟数据
				}
				else
				{
					Write_Analog();  		//进入驱动是写模拟数据 
				}
				break;
			}	
		}
		break;	
	case PLC_WRITE_TIME:						//进入驱动是写时间到PLC
		switch(ptcom->registerr)
		{
		case 'D':
			Write_Time();		
			break;			
		}
		break;	
	case PLC_READ_TIME:						//进入驱动是读取时间到PLC
		switch(ptcom->registerr)
		{
		case 'D':		
			Read_Time();		
			break;			
		}
		break;
	case PLC_WRITE_RECIPE:						//进入驱动是写配方到PLC
		switch(*(U8 *)(PE+3))	//配方寄存器名称
		{
		case 'D':		
			Write_Recipe();		
			break; 		
		}
		break;
	case PLC_READ_RECIPE:						//进入驱动是从PLC读取配方
		switch(*(U8 *)(PE+3))	//配方寄存器名称
		{
		case 'D':		
			Read_Recipe();		
			break;			
		}
		break;	
	case 7:						//进入驱动是把PLC的状态设置为监视
		//handshake();
		break;									
	case PLC_CHECK_DATA:						//进入驱动是数据处理
		watchcom();
		break;
	default:
		//handshake();
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
	
	
	*(U8 *)(AD1+0)=0x05;                              	//开始符
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);     	//plc站地址，高位在前
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);
	
	*(U8 *)(AD1+3)='F';                          		                   
	*(U8 *)(AD1+4)='F';  		
	*(U8 *)(AD1+5)='B';
	*(U8 *)(AD1+6)='W';                          		 	
	*(U8 *)(AD1+7)='0';
	/*
	if (ptcom->registerr == 'Y')
	{
		a1=b/512;
		a2=(b%512)/64;                                	
		a3=(b%64)/8;                                	
		a4=b%8;                                     	
	}
	else
	*/
	{
		a1=b/1000;
		a2=(b%1000)/100;                                	
		a3=(b%100)/10;                                	
		a4=b%10; 
	}
	
	*(U8 *)(AD1+9)=asicc(a1);    	
	*(U8 *)(AD1+10)=asicc(a2);                         	  	
	*(U8 *)(AD1+11)=asicc(a3);         
	*(U8 *)(AD1+12)=asicc(a4);  
	
	switch (ptcom->registerr)							//根据不同的寄存器，处理开始地址
	{
	case 'Y':
		*(U8 *)(AD1+8)='Y';                        	
		break;		
	case 'M':
		*(U8 *)(AD1+8)='M';   	
		break;
	case 'H':
		*(U8 *)(AD1+8)='M';   	
		break;
	case 'K':
		*(U8 *)(AD1+8)='S';   	
		break;
	case 'T':
		*(U8 *)(AD1+8)='T'; 
		*(U8 *)(AD1+9)='S';   	
		break;	
	case 'C':
		*(U8 *)(AD1+8)='C'; 
		*(U8 *)(AD1+9)='S';   	
		break;		
	}
	   	
	  	
	*(U8 *)(AD1+13)=0x30;		 	
	*(U8 *)(AD1+14)=0x31; 
	
	if (ptcom->writeValue==1)	                  	//置位命令
	{
		*(U8 *)(AD1+15)=0x31;                     	//置1
	}		
	else 
	{
		*(U8 *)(AD1+15)=0x30;                     	//置0		
	}
		                        	 		
	aakj=CalFCS((U8 *)(AD1+1),15);                	//除去开始符05计算和校验         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	
	*(U8 *)(AD1+16)=asicc(a1); 	
	*(U8 *)(AD1+17)=asicc(a2); 
	
	ptcom->send_length[0]=18;				  		//发送长度
	ptcom->send_staradd[0]=0;				  		//发送数据存储地址	
	ptcom->send_times=1;					  		//发送次数
			
	ptcom->return_length[0]=5;           			//返回数据长度
	ptcom->return_start[0]=0;				  		//返回数据有效开始
	ptcom->return_length_available[0]=0;	  		//返回有效数据长度	
	ptcom->Current_Times=0;					  		//当前发送次数	
	
	ptcom->Simens_Count=0;					  		//位返回，不同的返回有不同的数据处理方式		
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

	*(U8 *)(AD1+0)=0x05;                              	//开始符
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);     	//plc站地址，高位在前
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);
	
	*(U8 *)(AD1+3)='F';                          		                   
	*(U8 *)(AD1+4)='F';  		
	*(U8 *)(AD1+5)='W';
	*(U8 *)(AD1+6)='R';                          		 	
	*(U8 *)(AD1+7)='0';  
	
	switch (ptcom->registerr)
	{
	case 'X':
		*(U8 *)(AD1+8)='X';
		break;
	case 'Y':
		*(U8 *)(AD1+8)='Y';
		break;
	case 'M':
	case 'H':
		*(U8 *)(AD1+8)='M';
		break;
	case 'K':
		*(U8 *)(AD1+8)='S';
		break;
	}  	
/*
	if (ptcom->registerr == 'X' || ptcom->registerr == 'Y')
	{
//	sysprintf("Read_Bool b %d!!!!!!\n",b);		

		a1=b/512;
		a2=(b%512)/64;                                	
		a3=(b%64)/8;                                	
		a4=b%8;                                     	
	}
	else
*/
	{
		a1=b/1000;
		a2=(b%1000)/100;                                	
		a3=(b%100)/10;                                	
		a4=b%10; 
	}
	       
	*(U8 *)(AD1+9)=asicc(a1);    	
	*(U8 *)(AD1+10)=asicc(a2);                         			
	*(U8 *)(AD1+11)=asicc(a3);         
	*(U8 *)(AD1+12)=asicc(a4);   
	
	c=ptcom->register_length;                 		//读多少个元件
	if (c % 2 == 0)
	{
		c1=c/2;
	}   
	else
	{
		c1=c/2+1;
	}   
	c2=(c1>>4)&0xf;
	c3=c1&0xf;                		   	
	*(U8 *)(AD1+13)=asicc(c2);		 	
	*(U8 *)(AD1+14)=asicc(c3);    
	                     				
	aakj=CalFCS((U8 *)(AD1+1),14);                	//除去开始符05计算和校验         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	
	*(U8 *)(AD1+15)=asicc(a1); 	
	*(U8 *)(AD1+16)=asicc(a2); 
	
	ptcom->send_length[0]=17;				  		//发送长度
	ptcom->send_staradd[0]=0;				  		//发送数据存储地址	
	ptcom->send_times=1;					  		//发送次数
			
	ptcom->return_length[0]=8+c1*4;           		//返回数据长度
	ptcom->return_start[0]=5;				  		//返回数据有效开始
	ptcom->return_length_available[0]=c1*4;	  		//返回有效数据长度	
	ptcom->Current_Times=0;					  		//当前发送次数	
	ptcom->send_add[0]=ptcom->address;		  		//读的是这个地址的数据
   	
	ptcom->Simens_Count=1;					  		//位返回，不同的返回有不同的数据处理方式	
			
}

/***********************************************************************************************************************************************************************
 * Function: 读取数字量的信息
 * Parameters: 无
 * Return: true: 无
***********************************************************************************************************************************************************************/
void Read_Bool_TC()				                          
{
	U16 aakj;
	int b,t,c;
	int a1,a2,a3,a4;
	int plcadd;	
	int c1,c2,c3,c4;	
		
	b=ptcom->address;			                      	//开始地址
	plcadd=ptcom->plc_address;	                      	//PLC站地址

	*(U8 *)(AD1+0)=0x05;                              	//开始符
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);     	//plc站地址，高位在前
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);
	
	*(U8 *)(AD1+3)='F';                          		                   
	*(U8 *)(AD1+4)='F';  		
	*(U8 *)(AD1+5)='B';
	*(U8 *)(AD1+6)='R';                          		 	
	*(U8 *)(AD1+7)='0';  
	
	switch (ptcom->registerr)
	{
	case 'T':
		*(U8 *)(AD1+8)='T';
		*(U8 *)(AD1+9)='S';
		break;
	case 'C':
		*(U8 *)(AD1+8)='C';
		*(U8 *)(AD1+9)='S';
		break;	
	}  	
	
	a1=(b%1000)/100;
	a2=(b%100)/10;                                	
	a3=b%10; 	       
	  	
	*(U8 *)(AD1+10)=asicc(a1);                         			
	*(U8 *)(AD1+11)=asicc(a2);         
	*(U8 *)(AD1+12)=asicc(a3);   
	
	c=ptcom->register_length;                 		//读多少个元件
	c1=c*8;   
	c2=(c1>>4)&0xf;
	c3=c1&0xf;                		   	
	*(U8 *)(AD1+13)=asicc(c2);		 	
	*(U8 *)(AD1+14)=asicc(c3);    
	                     				
	aakj=CalFCS((U8 *)(AD1+1),14);                	//除去开始符05计算和校验         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	
	*(U8 *)(AD1+15)=asicc(a1); 	
	*(U8 *)(AD1+16)=asicc(a2); 
	
	ptcom->send_length[0]=17;				  		//发送长度
	ptcom->send_staradd[0]=0;				  		//发送数据存储地址	
	ptcom->send_times=1;					  		//发送次数
			
	ptcom->return_length[0]=8+c1;           		//返回数据长度
	ptcom->return_start[0]=5;				  		//返回数据有效开始
	ptcom->return_length_available[0]=c1;	  		//返回有效数据长度	
	ptcom->Current_Times=0;					  		//当前发送次数	
	ptcom->send_add[0]=ptcom->address;		  		//读的是这个地址的数据
   	
	ptcom->Simens_Count=2;					  		//位返回，不同的返回有不同的数据处理方式	
			
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
	
	
	*(U8 *)(AD1+0)=0x05;                          //开始符ENQ
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf); //plc站地址，高位在前
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);
	
	*(U8 *)(AD1+3)='F';     
	*(U8 *)(AD1+4)='F';
	*(U8 *)(AD1+5)='W';	         
	*(U8 *)(AD1+6)='R';    	
	*(U8 *)(AD1+7)='0';  
	
	a1=b/1000;                               
	a2=(b%1000)/100;                               
	a3=(b%100)/10;                                
	a4=b%10;                                     
 	
 	switch (ptcom->registerr)	                  //根据不同的寄存器，处理开始地址
	{
	case 'D':
	case 'N':
		*(U8 *)(AD1+8)='D';         
		*(U8 *)(AD1+9)=asicc(a1);    	
		*(U8 *)(AD1+10)=asicc(a2); 	
		*(U8 *)(AD1+11)=asicc(a3);         
		*(U8 *)(AD1+12)=asicc(a4);
		break;
	case 't':
		*(U8 *)(AD1+8)='T';         
		*(U8 *)(AD1+9)='N';    	
		*(U8 *)(AD1+10)=asicc(a2); 	
		*(U8 *)(AD1+11)=asicc(a3);         
		*(U8 *)(AD1+12)=asicc(a4);
		break;
	case 'c':
		*(U8 *)(AD1+8)='C';         
		*(U8 *)(AD1+9)='N';    	
		*(U8 *)(AD1+10)=asicc(a2); 	
		*(U8 *)(AD1+11)=asicc(a3);         
		*(U8 *)(AD1+12)=asicc(a4);
		break;
	}
	
	c=ptcom->register_length;                      //读多少个元件
	
	c1=(c>>4)&0xf;                                
	c2=c&0xf;                                 
	    	
	*(U8 *)(AD1+13)=asicc(c1);	 	
	*(U8 *)(AD1+14)=asicc(c2);	
	
	aakj=CalFCS((U8 *)(AD1+1),14);                	//除去开始符05计算和校验         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	
	*(U8 *)(AD1+15)=asicc(a1); 	
	*(U8 *)(AD1+16)=asicc(a2); 
	
	ptcom->send_length[0]=17;				  		//发送长度
	ptcom->send_staradd[0]=0;				  		//发送数据存储地址	
	ptcom->send_times=1;					  		//发送次数
			
	ptcom->return_length[0]=8+c*4;           		//返回数据长度
	ptcom->return_start[0]=5;				  		//返回数据有效开始
	ptcom->return_length_available[0]=c*4;	  		//返回有效数据长度	
	ptcom->Current_Times=0;					  		//当前发送次数	
	ptcom->send_add[0]=ptcom->address;		  		//读的是这个地址的数据
		
	ptcom->Simens_Count=5;                         //字返回，根据不同的值来选择不同的数据处理方式
}

/***********************************************************************************************************************************************************************
 * Function: 读模拟量
 * Parameters: 无
 * Return: true: 无
***********************************************************************************************************************************************************************/
void Read_Analog_CV32()				                  
{
	U16 aakj;
	int b,c;
	int a1,a2,a3,a4;
	int c1,c2,c3,c4;
	int plcadd;
	
	plcadd=ptcom->plc_address;	                  //PLC站地址
	b=(ptcom->address - 1000)/2;			      //开始地址减1000再除以2进行偏移
	
	
	*(U8 *)(AD1+0)=0x05;                          //开始符ENQ
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf); //plc站地址，高位在前
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);
	
	*(U8 *)(AD1+3)='F';     
	*(U8 *)(AD1+4)='F';
	*(U8 *)(AD1+5)='W';	         
	*(U8 *)(AD1+6)='R';    	
	*(U8 *)(AD1+7)='0';  
	
	*(U8 *)(AD1+8)='C';         
	*(U8 *)(AD1+9)='N'; 
	
	a1=b/1000;                               
	a2=(b%1000)/100;                               
	a3=(b%100)/10;                                
	a4=b%10;                                     
 	
 	*(U8 *)(AD1+10)=asicc(a2); 	
	*(U8 *)(AD1+11)=asicc(a3);         
	*(U8 *)(AD1+12)=asicc(a4);
	
	/*判断长度超限，因为每个单独的地址就表示32位，整合的时候会出现超容量的现象*/
    if (b + ptcom->register_length > 256)
    {
        ptcom->register_length = 256 - b;
    }
    
    c=ptcom->register_length;                   	//读多少个元件 
		
	c1=(c>>4)&0xf;                                
	c2=c&0xf;                                 
	    	
	*(U8 *)(AD1+13)=asicc(c1);	 	
	*(U8 *)(AD1+14)=asicc(c2);	
	
	aakj=CalFCS((U8 *)(AD1+1),14);                	//除去开始符05计算和校验         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	
	*(U8 *)(AD1+15)=asicc(a1); 	
	*(U8 *)(AD1+16)=asicc(a2); 
	
	ptcom->send_length[0]=17;				  		//发送长度
	ptcom->send_staradd[0]=0;				  		//发送数据存储地址	
	ptcom->send_times=1;					  		//发送次数
			
	ptcom->return_length[0]=8+c*8;           		//返回数据长度
	ptcom->return_start[0]=5;				  		//返回数据有效开始
	ptcom->return_length_available[0]=c*8;	  		//返回有效数据长度	
	ptcom->Current_Times=0;					  		//当前发送次数	
	ptcom->send_add[0]=ptcom->address;		  		//读的是这个地址的数据
		
	ptcom->Simens_Count=6;                         //双字返回，根据不同的值来选择不同的数据处理方式
}

/***********************************************************************************************************************************************************************
 * Function: 读模拟量
 * Parameters: 无
 * Return: true: 无
***********************************************************************************************************************************************************************/
void Read_Analog_R()				                  
{
	U16 aakj;
	int b,c;
	int a1,a2,a3,a4,a5,a6;
	int c1,c2,c3,c4;
	int plcadd;
	
	plcadd=ptcom->plc_address;	                  //PLC站地址
	b=ptcom->address;			                  //开始地址
	
	
	*(U8 *)(AD1+0)=0x05;                          //开始符ENQ
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf); //plc站地址，高位在前
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);
	
	*(U8 *)(AD1+3)='F';     
	*(U8 *)(AD1+4)='F';
	*(U8 *)(AD1+5)='Q';	         
	*(U8 *)(AD1+6)='R';    	
	*(U8 *)(AD1+7)='0';  
	
	*(U8 *)(AD1+8)='R';         
		
	a1=b/100000;                               
	a2=(b%100000)/10000;                               
	a3=(b%10000)/1000;
	a4=(b%1000)/100;                               
	a5=(b%100)/10;                                 
	a6=b%10;                                     
 	
 	*(U8 *)(AD1+9)=asicc(a1);    	
	*(U8 *)(AD1+10)=asicc(a2); 	
	*(U8 *)(AD1+11)=asicc(a3);         
	*(U8 *)(AD1+12)=asicc(a4);
	*(U8 *)(AD1+13)=asicc(a5);         
	*(U8 *)(AD1+14)=asicc(a6);
		
	c=ptcom->register_length;                      //读多少个元件
	
	c1=(c>>4)&0xf;                                
	c2=c&0xf;                                 
	    	
	*(U8 *)(AD1+15)=asicc(c1);	 	
	*(U8 *)(AD1+16)=asicc(c2);	
	
	aakj=CalFCS((U8 *)(AD1+1),16);                	//除去开始符05计算和校验         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	
	*(U8 *)(AD1+17)=asicc(a1); 	
	*(U8 *)(AD1+18)=asicc(a2); 
	
	ptcom->send_length[0]=19;				  		//发送长度
	ptcom->send_staradd[0]=0;				  		//发送数据存储地址	
	ptcom->send_times=1;					  		//发送次数
			
	ptcom->return_length[0]=8+c*4;           		//返回数据长度
	ptcom->return_start[0]=5;				  		//返回数据有效开始
	ptcom->return_length_available[0]=c*4;	  		//返回有效数据长度	
	ptcom->Current_Times=0;					  		//当前发送次数	
	ptcom->send_add[0]=ptcom->address;		  		//读的是这个地址的数据
		
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

	if(datalength%30==0)                           //最多能发30个D，数据刚好是30D的倍数
	{
		SendTimes=datalength/30;                   //发送次数
		LastTimeWord=30;                           //最后一次发送的长度为30D	
	}
	if(datalength%30!=0)                           //最多能发30个D，数据不是30D的倍数 
	{
		SendTimes=datalength/30+1;                 //发送的次数
		LastTimeWord=datalength%30;                //最后一次发送的长度为除30的余数	
	}
	
	for (i=0;i<SendTimes;i++)
	{
		ps=i*17;                                   //每次发17个长度
		b=p_start+i*30;                            //起始地址
		
		a1=b/1000;                               
		a2=(b%1000)/100;                               
		a3=(b%100)/10;                                
		a4=b%10; 
		
		*(U8 *)(AD1+0+ps)=0x05;                          //开始符ENQ
		*(U8 *)(AD1+1+ps)=asicc(((plcadd&0xf0)>>4)&0xf); //plc站地址，高位在前
		*(U8 *)(AD1+2+ps)=asicc(plcadd&0xf);
		
		*(U8 *)(AD1+3+ps)='F';     
		*(U8 *)(AD1+4+ps)='F';
		*(U8 *)(AD1+5+ps)='W';	         
		*(U8 *)(AD1+6+ps)='R';    	
		*(U8 *)(AD1+7+ps)='0';  		                                    
	 	
	 	*(U8 *)(AD1+8+ps)='D';         
		*(U8 *)(AD1+9+ps)=asicc(a1);    	
		*(U8 *)(AD1+10+ps)=asicc(a2); 	
		*(U8 *)(AD1+11+ps)=asicc(a3);         
		*(U8 *)(AD1+12+ps)=asicc(a4);
					
		c=ptcom->register_length;                      //读多少个元件
		
		c1=(c>>4)&0xf;                                
		c2=c&0xf;                                 
		
		if (i!=(SendTimes-1))	                    //不是最后一次发送时
		{
			c=30;                      				//读多少个元件
			c1=(c>>4)&0xf;                                
			c2=c&0xf; 
			
			currentlength=30;                       //固定长度30个D
		}
		else
		{
			c=LastTimeWord;                         //读多少个元件
			c1=(c>>4)&0xf;                                
			c2=c&0xf; 
			
			currentlength=LastTimeWord;             //剩余LastTimeWord个D
		}
		    	
		*(U8 *)(AD1+13+ps)=asicc(c1);	 	
		*(U8 *)(AD1+14+ps)=asicc(c2);	
		
		aakj=CalFCS((U8 *)(AD1+1+ps),14);                	//除去开始符05计算和校验         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		
		*(U8 *)(AD1+15+ps)=asicc(a1); 	
		*(U8 *)(AD1+16+ps)=asicc(a2); 
		
		ptcom->send_length[i]=17;				    //发送长度
		ptcom->send_staradd[i]=i*17;			    //发送数据存储地址	
		ptcom->send_add[i]=p_start+i*30;		    //读的是这个地址的数据	
		ptcom->send_data_length[i]=currentlength;	//不是最后一次都是28个D
				
		ptcom->return_length[i]=8+currentlength*4; //返回数据长度，有27个固定
		ptcom->return_start[i]=5;				    //返回数据有效开始
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
	
	plcadd=ptcom->plc_address;	                  //PLC站地址
	b=ptcom->address;			                  //开始地址
	
	
	*(U8 *)(AD1+0)=0x05;                          //开始符ENQ
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf); //plc站地址，高位在前
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);
	
	*(U8 *)(AD1+3)='F';     
	*(U8 *)(AD1+4)='F';
	*(U8 *)(AD1+5)='W';	         
	*(U8 *)(AD1+6)='W';    	
	*(U8 *)(AD1+7)='0';  

	
	a1=b/1000;                               
	a2=(b%1000)/100;                               
	a3=(b%100)/10;                                
	a4=b%10;     

	switch (ptcom->registerr)	                  //根据不同的寄存器，处理开始地址
	{
	case 'D':
	case 'N':
		*(U8 *)(AD1+8)='D';         
		*(U8 *)(AD1+9)=asicc(a1);    	
		*(U8 *)(AD1+10)=asicc(a2); 	
		*(U8 *)(AD1+11)=asicc(a3);         
		*(U8 *)(AD1+12)=asicc(a4);
		break;
	case 't':
		*(U8 *)(AD1+8)='T';         
		*(U8 *)(AD1+9)='N';    	
		*(U8 *)(AD1+10)=asicc(a2); 	
		*(U8 *)(AD1+11)=asicc(a3);         
		*(U8 *)(AD1+12)=asicc(a4);
		break;
	case 'c':
		*(U8 *)(AD1+8)='C';         
		*(U8 *)(AD1+9)='N';    	
		*(U8 *)(AD1+10)=asicc(a2); 	
		*(U8 *)(AD1+11)=asicc(a3);         
		*(U8 *)(AD1+12)=asicc(a4);
		break;
	}
	
	c=ptcom->register_length;                      //写多少个元件
	
	c1=(c>>4)&0xf;                                
	c2=c&0xf;                                 
	    	
	*(U8 *)(AD1+13)=asicc(c1);	 	
	*(U8 *)(AD1+14)=asicc(c2);	
	    	
	for (i=0;i<c;i++)                                  //写入多个元件值
	{				
		c0=ptcom->U8_Data[i*2];                        //从D[]数组中要数据，对应的c1为高位，c0为低位
		c1=ptcom->U8_Data[i*2+1];
						
		c01=c0&0xf;                                    //对所写数据作asicc处理，c0为低位
		c02=(c0>>4)&0xf;                               //对所写数据作asicc处理，c0为高位		
		c03=c1&0xf;                                    //对所写数据作asicc处理，c1为低位
		c04=(c1>>4)&0xf;                               //对所写数据作asicc处理，c1为高位				
	
		*(U8 *)(AD1+15+i*4)=asicc(c04);
		*(U8 *)(AD1+16+i*4)=asicc(c03);
		*(U8 *)(AD1+17+i*4)=asicc(c02);
		*(U8 *)(AD1+18+i*4)=asicc(c01);	
	}
	
	aakj=CalFCS((U8 *)(AD1+1),14+c*4);           	    //除去开始符05计算和校验         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+15+c*4)=asicc(a1);          	     //asicc码显示
	*(U8 *)(AD1+16+c*4)=asicc(a2); 
			
	ptcom->send_length[0]=17+c*4;		     	     //发送长度
	ptcom->send_staradd[0]=0;				     	 //发送数据存储地址	
	ptcom->send_times=1;					     	 //发送次数
			
	ptcom->return_length[0]=5;				     	 //返回数据长度
	ptcom->return_start[0]=0;				     	 //返回数据有效开始
	ptcom->return_length_available[0]=0;	     	 //返回有效数据长度		
	ptcom->Current_Times=0;	
	
	ptcom->Simens_Count=0;	                         //字返回，根据不同的值来选择不同的数据处理方式
}

/***********************************************************************************************************************************************************************
 * Function: 写模拟量
 * Parameters: 无
 * Return: true: 无
***********************************************************************************************************************************************************************/
void Write_Analog_CV32()								    
{
	U16 aakj;
	int b;
	int a1,a2,a3,a4;
	int i;
	int c01,c02,c03,c04,c05,c06,c07,c08;
	int c,c0,c1,c2,c3,c4;
	int plcadd;
	
	plcadd=ptcom->plc_address;	                  //PLC站地址
	b=(ptcom->address - 1000)/2;			      //开始地址减1000再除以2进行偏移
	
	
	*(U8 *)(AD1+0)=0x05;                          //开始符ENQ
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf); //plc站地址，高位在前
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);
	
	*(U8 *)(AD1+3)='F';     
	*(U8 *)(AD1+4)='F';
	*(U8 *)(AD1+5)='W';	         
	*(U8 *)(AD1+6)='W';    	
	*(U8 *)(AD1+7)='0';  
	
	a1=b/1000;                               
	a2=(b%1000)/100;                               
	a3=(b%100)/10;                                
	a4=b%10;     

	*(U8 *)(AD1+8)='C';         
	*(U8 *)(AD1+9)='N';    	
	*(U8 *)(AD1+10)=asicc(a2); 	
	*(U8 *)(AD1+11)=asicc(a3);         
	*(U8 *)(AD1+12)=asicc(a4);
	
	c=ptcom->register_length/2;                      //写多少个元件
	if (c == 0)
	{
		c = 1;
	}
	c1=(c>>4)&0xf;                                
	c2=c&0xf;                                 
	    	
	*(U8 *)(AD1+13)=asicc(c1);	 	
	*(U8 *)(AD1+14)=asicc(c2);	
	    	
	for (i=0;i<c;i++)                                  //写入多个元件值
	{				
		c0=ptcom->U8_Data[i*4];                        //从D[]数组中要数据，对应的c1为高位，c0为低位
		c1=ptcom->U8_Data[i*4+1];
		c2=ptcom->U8_Data[i*4+2];
		c3=ptcom->U8_Data[i*4+3];
						
		c01=c0&0xf;                                    //对所写数据作asicc处理，c0为低位
		c02=(c0>>4)&0xf;                               //对所写数据作asicc处理，c0为高位		
		c03=c1&0xf;                                    //对所写数据作asicc处理，c1为低位
		c04=(c1>>4)&0xf;                               //对所写数据作asicc处理，c1为高位				
		c05=c2&0xf;                                    //对所写数据作asicc处理，c2为低位
		c06=(c2>>4)&0xf;                               //对所写数据作asicc处理，c2为高位		
		c07=c3&0xf;                                    //对所写数据作asicc处理，c3为低位
		c08=(c3>>4)&0xf;                               //对所写数据作asicc处理，c3为高位
		
		*(U8 *)(AD1+15+i*8)=asicc(c08);
		*(U8 *)(AD1+16+i*8)=asicc(c07);
		*(U8 *)(AD1+17+i*8)=asicc(c06);
		*(U8 *)(AD1+18+i*8)=asicc(c05);	
		*(U8 *)(AD1+19+i*8)=asicc(c04);
		*(U8 *)(AD1+20+i*8)=asicc(c03);
		*(U8 *)(AD1+21+i*8)=asicc(c02);
		*(U8 *)(AD1+22+i*8)=asicc(c01);	
	}
	
	aakj=CalFCS((U8 *)(AD1+1),14+c*8);           	    //除去开始符05计算和校验         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+15+c*8)=asicc(a1);          	     //asicc码显示
	*(U8 *)(AD1+16+c*8)=asicc(a2); 
			
	ptcom->send_length[0]=17+c*8;		     	     //发送长度
	ptcom->send_staradd[0]=0;				     	 //发送数据存储地址	
	ptcom->send_times=1;					     	 //发送次数
			
	ptcom->return_length[0]=5;				     	 //返回数据长度
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

	//U32 lw = *(U32*)(lw_word + 0);    /*调试信息*/
	
	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));//数据长度
	staradd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//数据开始地址
	plcadd=*(U8 *)(PE+4);

	if(datalength%30==0)                       //最多能发30个D，数据长度刚好是30的倍数时
	{
		SendTimes=datalength/30;               //发送的次数为datalength/30               
		LastTimeWord=30;                       //最后一次发送的长度为30个D
	}
	if(datalength%30!=0)                       //数据长度不是30D的倍数时
	{
		SendTimes=datalength/30+1;             //发送的次数datalength/30+1
		LastTimeWord=datalength%30;            //最后一次发送的长度为除30的余数
	}

    ps=137;                                    //当发超过30个D的时候，前面每次发137个


   // *(U16*)((U16*)lw + 20) =datalength;    /*调试信息*/

	for (i=0;i<SendTimes;i++)
	{   
		if (i!=(SendTimes-1))                  //不是最后一次发送时
		{	
			length=30;                         //发30个D
		}
		else                                   //最后一次发送时
		{
			length=LastTimeWord;               //发剩余的长度             
		}
		
		b=staradd+i*30;                      	   //每次偏移30个D
		
		a1=b/1000;                               
		a2=(b%1000)/100;                               
		a3=(b%100)/10;                                
		a4=b%10; 
		
		*(U8 *)(AD1+0+ps*i)=0x05;                          //开始符ENQ
		*(U8 *)(AD1+1+ps*i)=asicc(((plcadd&0xf0)>>4)&0xf); //plc站地址，高位在前
		*(U8 *)(AD1+2+ps*i)=asicc(plcadd&0xf);
		
		*(U8 *)(AD1+3+ps*i)='F';     
		*(U8 *)(AD1+4+ps*i)='F';
		*(U8 *)(AD1+5+ps*i)='W';	         
		*(U8 *)(AD1+6+ps*i)='W';    	
		*(U8 *)(AD1+7+ps*i)='0';  
		
		*(U8 *)(AD1+8+ps*i)='D';         
		*(U8 *)(AD1+9+ps*i)=asicc(a1);    	
		*(U8 *)(AD1+10+ps*i)=asicc(a2); 	
		*(U8 *)(AD1+11+ps*i)=asicc(a3);         
		*(U8 *)(AD1+12+ps*i)=asicc(a4);   
		
		c=length;                      //写多少个元件
		
		c1=(c>>4)&0xf;                                
		c2=c&0xf;                                 
		    	
		*(U8 *)(AD1+13+ps*i)=asicc(c1);	 	
		*(U8 *)(AD1+14+ps*i)=asicc(c2);	
		    	
		for (j=0;j<c;j++)                                  //写入多个元件值
		{				
			c0=*(U8 *)(PE+9+i*60+j*2);         //从存数据的寄存器开始地址PE+9取数据，k3为高位，k4为低位
			c1=*(U8 *)(PE+9+i*60+j*2+1);
							
			c01=c0&0xf;                                    //对所写数据作asicc处理，c0为低位
			c02=(c0>>4)&0xf;                               //对所写数据作asicc处理，c0为高位		
			c03=c1&0xf;                                    //对所写数据作asicc处理，c1为低位
			c04=(c1>>4)&0xf;                               //对所写数据作asicc处理，c1为高位				
		
			*(U8 *)(AD1+15+j*4+ps*i)=asicc(c04);
			*(U8 *)(AD1+16+j*4+ps*i)=asicc(c03);
			*(U8 *)(AD1+17+j*4+ps*i)=asicc(c02);
			*(U8 *)(AD1+18+j*4+ps*i)=asicc(c01);	
		}
		
		aakj=CalFCS((U8 *)(AD1+1+ps*i),14+c*4);           	    //除去开始符05计算和校验         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+15+c*4+ps*i)=asicc(a1);          	     //asicc码显示
		*(U8 *)(AD1+16+c*4+ps*i)=asicc(a2); 
				
		
		ptcom->send_length[i]=17+c*4;	   		//发送长度
		ptcom->send_staradd[i]=i*ps;		   //发送数据存储地址	
		
		ptcom->return_length[i]=5;			   //返回数据长度
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
	int b1,b2,b3,b4;

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
	else if(ptcom->Simens_Count==2)           //T/C读的数据返回处理
	{	
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/8;i++)
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
			b1=(a8<<7)+(a7<<6)+(a6<<5)+(a5<<4)+(a4<<3)+(a3<<2)+(a2<<1)+a1;
			
			*(U8 *)(COMad+i)=b1;
			
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
	
			*(U8 *)(COMad+i*2)=b2;					        
			*(U8 *)(COMad+i*2+1)=b1;							     		
		}
		ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;	//长度减半	
	} 
	else if(ptcom->Simens_Count==6)
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/8;i++)	//ASC玛返回，所以要转为16进制，8个asicc码换成4个16进制数
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
			
			b1=(a1<<4)+a2;
			b2=(a3<<4)+a4;
			b3=(a5<<4)+a6;
			b4=(a7<<4)+a8;
			
			*(U8 *)(COMad+i*4)=b4;					         
			*(U8 *)(COMad+i*4+1)=b3;					     
			*(U8 *)(COMad+i*4+2)=b2;					     
			*(U8 *)(COMad+i*4+3)=b1;		
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
	unsigned int a1,a2,a3;
	unsigned int aakj1;
	unsigned int aakj2;
	unsigned int akj1;
	unsigned int akj2;
	if(ptcom->return_length[ptcom->Current_Times-1] == 5)	//写功能只返回5个代码
	{
		a1=*(U8 *)(COMad)&0xff;
		a2=*(U8 *)(COMad+3)&0xff;
		a1=*(U8 *)(COMad+4)&0xff;
		if (a1 == 0x06 && a2 == 0x46 && a3 == 0x46)//返回06是正确的
        {
            return 2;
        }
        return 0;
	}
	
	aakj2=*(U8 *)(COMad+ptcom->return_length[ptcom->Current_Times-1]-1)&0xff;		//在发送完后Current_Times++，此时要--
	aakj1=*(U8 *)(COMad+ptcom->return_length[ptcom->Current_Times-1]-2)&0xff;
	akj1=CalFCS((U8 *)(COMad+1),ptcom->return_length[ptcom->Current_Times-1]-3);
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
	int i;
    int SUM=0;

    for(i=0;i<uNo;i++)
    {
        SUM=SUM+chData[i];
    }
    return (SUM);
}

