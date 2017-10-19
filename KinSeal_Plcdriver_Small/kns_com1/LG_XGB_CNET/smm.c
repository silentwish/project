/************************************************************************
*  File name:     XGB_CNET
*  Default parameters: 9600 8 ONE 1
*  Description:   该驱动包含如下寄存器：
*
*  名称		格式		范围		备注					对应代号
*  P		dddh		0~255f		位：输入/输出区域		Y
*  M		dddh		0~255f		位：辅助继电器			M
*  K		dddh		0~255f		位：保护继电器			K
*  L		dddh		0~255f		位：链接继电器			L
*  F		dddh		0~255f		位：特殊继电器			H
*  T		ddd			0~255		位：计时器继电器		T
*  C		ddd			0~255		位：计数器继电器		C
*  D		dddd		0~9999		字：数据寄存器			D
*  S		dddd		0~9999		字：数据寄存器			R
*  TV		ddd			0~255		字：计时器当前值		t
*  CV		ddd			0~255		字：计数器当前值		c
*--------------------------------------------------------------------------
*  Motify 1
*  Date:
*  reason:
*  Author:
*--------------------------------------------------------------------------
*****************************************************************************/
#include "stdio.h"
#include "def.h"
#include "smm.h"


	
struct Com_struct_D *ptcom;

/*************************************************************************
 * Funciton: 主程序进入驱动收集发送数据接口函数
 * Parameters: 无
 * Return: 无
**************************************************************************/
void Enter_PlcDriver(void)
{
	ptcom=(struct Com_struct_D *)adcom;	
	switch (ptcom->R_W_Flag)
	{
	case PLC_READ_DATA:							//进入驱动是读数据
	case PLC_READ_DIGITALALARM:						//进入驱动是读数据,报警	
	case PLC_READ_TREND:						//进入驱动是读数据,趋势图
	case PLC_READ_ANALOGALARM:						//进入驱动是读数据,类比报警	
	case PLC_READ_CONTROL:						//进入驱动是读PLC控制的数据	
		switch(ptcom->registerr)
		{
		case 'Y':					//P
		case 'M':					//M
		case 'K':					//K
		case 'L':					//L
		case 'H':					//F
		case 'T':					//T
		case 'C':					//C						
			Read_Bool();				//读数字量
			break;
		case 'D':					//D
		case 'R':					//S
		case 't':					//TV
		case 'c':					//CV	
			Read_Analog();			//读模拟量
			break;			
		}
		break;
	case PLC_WRITE_DATA:							//进入驱动是写数据
		switch(ptcom->registerr)
		{
		case 'Y':					//P
		case 'M':					//M
		case 'K':					//K
		case 'L':					//L
		//case 'H':					//F
		case 'T':					//T
		case 'C':					//C					
			Set_Reset();			//置位复位
			break;
		case 'D':					//D
		case 'R':					//S
		case 't':					//TV
		case 'c':					//CV
			Write_Analog();			//写模拟量		
			break;			
		}
		break;	
	case PLC_WRITE_TIME:							//进入驱动是写时间到PLC
		switch(ptcom->registerr)
		{
		case 'D':					//D
		case 'R':					//S
		case 't':					//TV
		case 'c':					//CV
			Write_Time();		
			break;			
		}
		break;	
	case PLC_READ_TIME:							//进入驱动是读取时间到PLC
		switch(ptcom->registerr)
		{
		case 'D':					//D
		case 'R':					//S
		case 't':					//TV
		case 'c':					//CV	
			Read_Time();		
			break;			
		}
		break;
	case PLC_WRITE_RECIPE:							//进入驱动是写配方到PLC
		switch(*(U8 *)(PE+3))		//配方寄存器名称
		{
		case 'D':					//D
		case 'R':					//S
			Write_Recipe();		
			break;			
		}
		break;
	case PLC_READ_RECIPE:							//进入驱动是从PLC读取配方
		switch(*(U8 *)(PE+3))		//配方寄存器名称
		{
		case 'D':					//D
		case 'R':					//S
			Read_Recipe();		
			break;			
		}
		break;							
	case PLC_CHECK_DATA:							//进入驱动是数据处理
		watchcom();
		break;				
	}	 
}

/*************************************************************************
 * Funciton: 数字量写入
 * Parameters: 无
 * Return: 无
**************************************************************************/
void Set_Reset()
{
	int iSendAdd,iWord,iBit;
	int plcadd;	
	int iTemp1,iTemp2,iTemp3,iTemp4,iTemp5;

	iSendAdd=ptcom->address;								//寄存器给的开始地址
	plcadd=ptcom->plc_address;								//PLC站地址
	
	*(U8 *)(AD1+0)=0x05;        							//头ENQ，即05
	
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);  			//plc站地址，高位在前
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);
	
	*(U8 *)(AD1+3)=0x57;        							//W
	*(U8 *)(AD1+4)=0x53;        							//S
	*(U8 *)(AD1+5)=0x53;        							//S 
	
    *(U8 *)(AD1+6)=0x30;        							//BLOCK NO.
	*(U8 *)(AD1+7)=0x31;		

	*(U8 *)(AD1+8)=0x30;        							//地址字符和开始地址共9个长度       
	*(U8 *)(AD1+9)=0x39;
	
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
		
	case 'K':
		*(U8 *)(AD1+10)=0x25;       						//K区是%KX，即25 4C 58
		*(U8 *)(AD1+11)=0x4B;
		*(U8 *)(AD1+12)=0x58;
		break;
		
	case 'L':
		*(U8 *)(AD1+10)=0x25;       						//L区是%LX，即25 4B 58
		*(U8 *)(AD1+11)=0x4C;
		*(U8 *)(AD1+12)=0x58;
		break;
		
	case 'H':
		*(U8 *)(AD1+10)=0x25;       						//F区是%FX，即25 4B 58
		*(U8 *)(AD1+11)=0x46;
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
	
	if (ptcom->registerr == 'T' || ptcom->registerr == 'C')
	{
		*(U8 *)(AD1+13)=0x30;
	
		iTemp1=iSendAdd/10000;								//最后一个字节是十六进制发送，都要转成ASC码
		iTemp2=(iSendAdd%10000)/1000;
		iTemp3=(iSendAdd%1000)/100;
		iTemp4=(iSendAdd%100)/10;	
		iTemp5=iSendAdd%10;
        
    	*(U8 *)(AD1+14)=asicc(iTemp1);
		*(U8 *)(AD1+15)=asicc(iTemp2);  		
		*(U8 *)(AD1+16)=asicc(iTemp3);
		*(U8 *)(AD1+17)=asicc(iTemp4);		
		*(U8 *)(AD1+18)=asicc(iTemp5); 
	}
	else
	{
	    iWord = iSendAdd/16;
        iBit = iSendAdd%16;

		iTemp1=iWord/10000;									//最后一个字节是十六进制发送，都要转成ASC码
		iTemp2=(iWord%10000)/1000;
		iTemp3=(iWord%1000)/100;
		iTemp4=(iWord%100)/10;	
		iTemp5=iWord%10;
        
        *(U8 *)(AD1+13)=asicc(iTemp1);
		*(U8 *)(AD1+14)=asicc(iTemp2);  		
		*(U8 *)(AD1+15)=asicc(iTemp3);
		*(U8 *)(AD1+16)=asicc(iTemp4);		
		*(U8 *)(AD1+17)=asicc(iTemp5); 
		
		*(U8 *)(AD1+18)=asicc(iBit);
	}	

	if (ptcom->writeValue==1)								//置位时，发30 31
	{
		*(U8 *)(AD1+19)=0x30;
		*(U8 *)(AD1+20)=0x31;
	}
	if (ptcom->writeValue==0)								//复位时，发30 30
	{
		*(U8 *)(AD1+19)=0x30;
		*(U8 *)(AD1+20)=0x30;
	}
	
	*(U8 *)(AD1+21)=0x04;       							//结束码 04
	
	ptcom->send_length[0]=22;								//发送长度P为22个
	ptcom->send_staradd[0]=0;								//发送数据存储地址	
	ptcom->send_times=1;									//发送次数
		
	ptcom->return_length[0]=7;								//返回数据长度
	ptcom->return_start[0]=0;								//返回数据有效开始
	ptcom->return_length_available[0]=0;					//返回有效数据长度	
	ptcom->Current_Times=0;									//当前发送次数			
}

/*************************************************************************
 * Funciton: 数字量读取
 * Parameters: 无
 * Return: 无
**************************************************************************/
void Read_Bool()
{
	int b;
	int length;
	int plcadd;
	int b1,b2;
	int a1,a2,a3,a4,a5;
	int add;
	int i,j;
	int ps;
	
	b=ptcom->address;			    						//寄存器所给地址
	plcadd=ptcom->plc_address;	    						//PLC站地址
	length=ptcom->register_length;  						//所读的长度

	switch (ptcom->registerr)
	{
	case 'Y':
	case 'M':
	case 'K':
	case 'L': 
	case 'H':
		*(U8 *)(AD1+0)=0x05;            					//头ENQ，即05
		
		*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);  		//plc站地址，高位在前
		*(U8 *)(AD1+2)=asicc(plcadd&0xf);
		
		*(U8 *)(AD1+3)=0x52;            					//R
		*(U8 *)(AD1+4)=0x53;            					//S
		*(U8 *)(AD1+5)=0x42;            					//B 
		
		*(U8 *)(AD1+6)=0x30;            					//2个长度发送的是地址字符长度 
		*(U8 *)(AD1+7)=0x38;		
				
		switch (ptcom->registerr)	    					//根据寄存器类型有不同的地址字符
		{
			case 'Y': 
				*(U8 *)(AD1+8)=0x25;        				//下3个长度发送的是地址字符%PW       
				*(U8 *)(AD1+9)=0x50; 
			    *(U8 *)(AD1+10)=0x57;   
				break;	
					
			case 'M':		
				*(U8 *)(AD1+8)=0x25;        				//下3个长度发送的是地址字符%MW       
				*(U8 *)(AD1+9)=0x4D; 
			    *(U8 *)(AD1+10)=0x57;         
				break;
					
			case 'K': 
				*(U8 *)(AD1+8)=0x25;        				//下3个长度发送的是地址字符%KW       
				*(U8 *)(AD1+9)=0x4B; 
			    *(U8 *)(AD1+10)=0x57;
				break;
				
			case 'L': 
				*(U8 *)(AD1+8)=0x25;        				//下3个长度发送的是地址字符%LW       
				*(U8 *)(AD1+9)=0x4C; 
			    *(U8 *)(AD1+10)=0x57;
				break;		
				
			case 'H': 
				*(U8 *)(AD1+8)=0x25;        				//下3个长度发送的是地址字符%FW       
				*(U8 *)(AD1+9)=0x46; 
			    *(U8 *)(AD1+10)=0x57;
				break;						
		}

		b=b/16;												//地址转换，地址占5个字节，由高到低，前4个字节是十进制发送 
		a1=b/10000;											//最后一个字节是十六进制发送，都要转成ASC码
		a2=(b%10000)/1000;
		a3=(b%1000)/100;
		a4=(b%100)/10;
		a5=b%10;	
			
		*(U8 *)(AD1+11)=asicc(a1);    
		*(U8 *)(AD1+12)=asicc(a2);   	
		*(U8 *)(AD1+13)=asicc(a3);    
		*(U8 *)(AD1+14)=asicc(a4);  
		*(U8 *)(AD1+15)=asicc(a5);
			
		if (length%2 == 0)
		{
			length = length/2;
		}					
		else
		{
			length = length/2 + 1;
		}
		b1=(length>>4)&0xf;             					//读的长度处理，b1高位，b2低位
		b2=length&0xf;
		*(U8 *)(AD1+16)=asicc(b1);      					//读的长度，asicc显示
		*(U8 *)(AD1+17)=asicc(b2);
			
		*(U8 *)(AD1+18)=0x04;           					//结束字符

		ptcom->send_length[0]=19;							//发送长度
		ptcom->send_staradd[0]=0;							//发送数据存储地址	
							
		ptcom->return_length[0]=11+length*4;				//返回数据长度，有11个固定
		ptcom->return_start[0]=10;							//返回数据有效开始，第11个
		ptcom->return_length_available[0]=length*4;			//返回有效数据长度	
		
		ptcom->address = ptcom->address/16*16;	    		//PLC地址
		ptcom->register_length = length*2;  				//所读的长度			
		ptcom->send_add[0]=ptcom->address;					//读的是这个地址的数据		
		
		ptcom->Current_Times=0;								//当前发送次数
		ptcom->send_times=1;								//发送次数
		
		ptcom->Simens_Count=1; 								//全局变量，根据其值0或1对返回处理不同
		break;
	case 'T':
	case 'C':
		for (i=0;i<length;i++)
		{
			ps=81*i;	
			*(U8 *)(AD1+0+ps)=0x05;            				//头ENQ，即05
			
			*(U8 *)(AD1+1+ps)=asicc(((plcadd&0xf0)>>4)&0xf);//plc站地址，高位在前
			*(U8 *)(AD1+2+ps)=asicc(plcadd&0xf);
			
			*(U8 *)(AD1+3+ps)=0x52;            				//R
			*(U8 *)(AD1+4+ps)=0x53;            				//S
			*(U8 *)(AD1+5+ps)=0x53;            				//S 
			
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
			
				add=b+8*i+j;							//每块地址+1，就是连续的读8块
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
					
			ptcom->send_length[i]=81;						//发送长度
			ptcom->send_staradd[i]=ps;						//发送数据存储地址	
									
			ptcom->return_length[i]=41;						//返回数据长度
			ptcom->return_start[i]=11;						//返回数据有效开始
			ptcom->return_length_available[i]=32;			//返回有效数据长度				

			ptcom->register_length = 1;		
		}
		ptcom->send_times=length;							//发送次数
		ptcom->Current_Times=0;								//当前发送次数
		ptcom->Simens_Count=2; 								//全局变量，根据其值0或1对返回处理不同
		break; 	
	}
}

/*************************************************************************
 * Funciton: 模拟量读取
 * Parameters: 无
 * Return: 无
**************************************************************************/
void Read_Analog()
{
	int b;
	int a1,a2,a3,a4,a5;
	int plcadd;
	int length;
	int b1,b2;
	
	plcadd=ptcom->plc_address;	    						//PLC站地址
	b=ptcom->address;			    						//寄存器所给地址
	length=ptcom->register_length;  						//读的长度
	     

	*(U8 *)(AD1+0)=0x05;            						//头ENQ，即05
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);        	//plc站地址，高位在前
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);	
	*(U8 *)(AD1+3)=0x52;            						//R
	*(U8 *)(AD1+4)=0x53;            						//S
	*(U8 *)(AD1+5)=0x42;            						//B 
	*(U8 *)(AD1+6)=0x30;            						//下2个长度发送的是地址字符和开始地址共8个长度 
	*(U8 *)(AD1+7)=0x38; 
	
	switch (ptcom->registerr)	    						//根据寄存器类型有不同的地址字符
	{
	case 'D': 
		*(U8 *)(AD1+8)=0x25;        						//下3个长度发送的是地址字符%DW       
		*(U8 *)(AD1+9)=0x44; 
	    *(U8 *)(AD1+10)=0x57;  
		break;	
		
	case 'R': 
		*(U8 *)(AD1+8)=0x25;        						//下3个长度发送的是地址字符%SW       
		*(U8 *)(AD1+9)=0x53; 
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
	
	a1=b/10000;												//最后一个字节是十六进制发送，都要转成ASC码
	a2=(b%10000)/1000;
	a3=(b%1000)/100;
	a4=(b%100)/10;	
	a5=b%10;	
	
	*(U8 *)(AD1+11)=asicc(a1);  							//开始的地址，asicc显示，高位先发
	*(U8 *)(AD1+12)=asicc(a2);	
	*(U8 *)(AD1+13)=asicc(a3);          
	*(U8 *)(AD1+14)=asicc(a4);
	*(U8 *)(AD1+15)=asicc(a5);
	
	b1=(length>>4)&0xf;                 					//读的长度处理，b1为高位,b2为低位
	b2=length&0xf;
		
	*(U8 *)(AD1+16)=asicc(b1);          					//读的长度，asicc显示
	*(U8 *)(AD1+17)=asicc(b2);
	
	*(U8 *)(AD1+18)=0x04;               					//结束字符
	
	ptcom->send_length[0]=19;								//发送长度
	ptcom->send_staradd[0]=0;								//发送数据存储地址	
	ptcom->send_times=1;									//发送次数
		
	ptcom->return_length[0]=11+length*4;					//返回数据长度，有11个固定
	ptcom->return_start[0]=10;								//返回数据有效开始
	ptcom->return_length_available[0]=length*4;				//返回有效数据长度	
	ptcom->Current_Times=0;									//当前发送次数	
	ptcom->send_add[0]=ptcom->address;						//读的是这个地址的数据
	ptcom->Simens_Count=3; 
}

/*************************************************************************
 * Funciton: 读取配方
 * Parameters: 无
 * Return: 无
**************************************************************************/
void Read_Recipe()
{
	int b;
	int a1,a2,a3,a4,a5;
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

	if(datalength%16==0)                    				//每次发16个配方，读的配方数刚好为16的倍数时
	{
		SendTimes=datalength/16;            				//发送的次数            
		LastTimeWord=16;                    				//最后一次发送的长度为16	
	}
	if(datalength%16!=0)                    				//每次发16个配方，读的配方数不是16的倍数时
	{
		SendTimes=datalength/16+1;          				//发送的次数
		LastTimeWord=datalength%16;         				//最后一次发送的长度为除16的余数	
	}
	
	for (i=0;i<SendTimes;i++)               				//小于发送次数继续发送
	{
		ps=19*i;                            				//每次发19个长度，第二次发就是19开始
		b=p_start+i*16;                     				//每次的开始地址
		
		a1=b/10000;											//最后一个字节是十六进制发送，都要转成ASC码
		a2=(b%10000)/1000;
		a3=(b%1000)/100;
		a4=(b%100)/10;	
		a5=b%10;	

		*(U8 *)(AD1+0+ps)=0x05;             				//头ENQ，即05
		*(U8 *)(AD1+1+ps)=asicc(((plcadd&0xf0)>>4)&0xf); 	//plc站地址，高位在前
		*(U8 *)(AD1+2+ps)=asicc(plcadd&0xf);
		*(U8 *)(AD1+3+ps)=0x52;            					//读配方，就是R
		*(U8 *)(AD1+4+ps)=0x53;            					//下两个长度发送的是命令类型SB(连续)，即53 42
		*(U8 *)(AD1+5+ps)=0x42; 
		*(U8 *)(AD1+6+ps)=0x30;            					//下2个长度发送的是地址字符和开始地址共7个长度 
		*(U8 *)(AD1+7+ps)=0x38; 
		*(U8 *)(AD1+8+ps)=0x25;            					//下3个长度发送的是地址字符%DW       
		*(U8 *)(AD1+9+ps)=0x44; 
	    *(U8 *)(AD1+10+ps)=0x57;
			
		*(U8 *)(AD1+11+ps)=asicc(a1);      					//开始的地址，asicc显示，高位先发
		*(U8 *)(AD1+12+ps)=asicc(a2);	
		*(U8 *)(AD1+13+ps)=asicc(a3);      
		*(U8 *)(AD1+14+ps)=asicc(a4);
		*(U8 *)(AD1+15+ps)=asicc(a5);
		
		if (i!=(SendTimes-1))	          					//不是最后一次发送时
		{
			*(U8 *)(AD1+16+ps)=0x31;      					//固定长度16个，即0x10，转成asicc
			*(U8 *)(AD1+17+ps)=0x30;
			currentlength=16;             					//发送的数据长度
		}
		if (i==(SendTimes-1))	          					//最后一次发送时
		{
			*(U8 *)(AD1+16+ps)=asicc((LastTimeWord>>4)&0xf);//发送的长度为除16的余数LastTimeWord，高位先发
			*(U8 *)(AD1+17+ps)=asicc(LastTimeWord&0xf);
			currentlength=LastTimeWord;   					//发送的数据长度
		}	
			
		*(U8 *)(AD1+18+ps)=0x04;          					//结束字符
		
		ptcom->send_length[i]=19;							//发送长度
		ptcom->send_staradd[i]=i*19;						//发送数据存储地址	
		ptcom->send_add[i]=p_start+i*16;					//读的是这个地址的数据	
		ptcom->send_data_length[i]=currentlength;			//不是最后一次都是16个
				
		ptcom->return_length[i]=11+currentlength*4;			//返回数据长度，有11个固定
		ptcom->return_start[i]=10;				    		//返回数据有效开始
		ptcom->return_length_available[i]=currentlength*4;	//返回有效数据长度		
	}
	ptcom->send_times=SendTimes;							//发送次数
	ptcom->Current_Times=0;					        		//当前发送次数	
	ptcom->Simens_Count=3;
}

/*************************************************************************
 * Funciton: 写模拟量
 * Parameters: 无
 * Return: 无
**************************************************************************/
void Write_Analog()
{
	int b,i;
	int a1,a2,a3,a4,a5;
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
	
	*(U8 *)(AD1+3)=0x57;            						//写配方，就是W
	*(U8 *)(AD1+4)=0x53;            						//下两个长度发送的是命令类型SB(连续)，即53 42
	*(U8 *)(AD1+5)=0x42; 
	*(U8 *)(AD1+6)=0x30;            						//下2个长度发送的是地址字符和开始地址共7个长度 
	*(U8 *)(AD1+7)=0x38; 
	
	switch (ptcom->registerr)	    						//根据寄存器类型有不同的地址字符
	{
	case 'D': 
		*(U8 *)(AD1+8)=0x25;        						//下3个长度发送的是地址字符%DW       
		*(U8 *)(AD1+9)=0x44; 
	    *(U8 *)(AD1+10)=0x57;       
		break;	
		
	case 'R': 
		*(U8 *)(AD1+8)=0x25;        						//下3个长度发送的是地址字符%SW       
		*(U8 *)(AD1+9)=0x53; 
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
	
	a1=b/10000;												//最后一个字节是十六进制发送，都要转成ASC码
	a2=(b%10000)/1000;
	a3=(b%1000)/100;
	a4=(b%100)/10;	
	a5=b%10;	
	
	*(U8 *)(AD1+11)=asicc(a1);  							//开始的地址，asicc显示，高位先发
	*(U8 *)(AD1+12)=asicc(a2);	
	*(U8 *)(AD1+13)=asicc(a3);    
	*(U8 *)(AD1+14)=asicc(a4);
	*(U8 *)(AD1+15)=asicc(a5);
	
	b1=(length>>4)&0xf;             						//写的配方数处理，高位为b1，低位为b2
	b2=length&0xf;
		
	*(U8 *)(AD1+16)=asicc(b1);       						//读的长度，asicc显示
	*(U8 *)(AD1+17)=asicc(b2);
	
	for(i=0;i<length;i++)
	{			
	c=ptcom->U8_Data[i*2];   								//从D[]数组中要数据，对应的c0为高位，c为低位
	c0=ptcom->U8_Data[i*2+1];
						
	c1=c&0xf;                								//对所写数据作asicc处理，c为低位
	c2=(c>>4)&0xf;           								//对所写数据作asicc处理，c为低位		
	c3=c0&0xf;               								//对所写数据作asicc处理，c1为高位
	c4=(c0>>4)&0xf;          								//对所写数据作asicc处理，c1为高位				
	
	*(U8 *)(AD1+18+i*4)=asicc(c4);        					//数据是高位先发
	*(U8 *)(AD1+19+i*4)=asicc(c3);
	*(U8 *)(AD1+20+i*4)=asicc(c2);
	*(U8 *)(AD1+21+i*4)=asicc(c1);	
	}
	
	*(U8 *)(AD1+18+length*4)=0x04;        					//结束字符

	ptcom->send_length[0]=19+length*4;						//发送长度
	ptcom->send_staradd[0]=0;								//发送数据存储地址	
	ptcom->send_times=1;									//发送次数
			
	ptcom->return_length[0]=7;								//返回数据长度
	ptcom->return_start[0]=0;								//返回数据有效开始
	ptcom->return_length_available[0]=0;					//返回有效数据长度	
	ptcom->Current_Times=0;									//当前发送次数
	ptcom->Simens_Count=0;	
}

/*************************************************************************
 * Funciton: 写配方到PLC
 * Parameters: 无
 * Return: 无
**************************************************************************/
void Write_Recipe()
{
	int datalength;
	int staradd;
	int SendTimes;
	int LastTimeWord;										//最后一次发送长度
	int i,j;
	int ps;
	int b;
	int a1,a2,a3,a4,a5;
	int b1,b2,b3,b4;
	int c1,c2,c3,c4;
	int plcadd;
	int length;
		
	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));		//数据长度
	staradd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//数据长度
	plcadd=ptcom->plc_address;	                   			//PLC站地址

	if(datalength%16==0)                           			//写的配方数刚好是16的倍数时             
	{
		SendTimes=datalength/16;                   			//发送的次数
		LastTimeWord=16;                           			//最后一次发送的长度	
	}
	if(datalength%16!=0)                           			//不是16的倍数时
	{
		SendTimes=datalength/16+1;                 			//发送的次数
		LastTimeWord=datalength%16;                			//最后一次发送的长度	
	}	
	
	ps=83;                                         			//写16个配方时，发送的是83个长度
	for (i=0;i<SendTimes;i++)
	{
		if (i!=(SendTimes-1))                      			//不是最后一次发送时
		{	
			length=16;                             			//发16个
		}
		else                                       			//最后一次发送时
		{
			length=LastTimeWord;                   			//发剩余的配方数             
		}

		*(U8 *)(AD1+0+ps*i)=0x05;            				//头ENQ，即05
		*(U8 *)(AD1+1+ps*i)=asicc(((plcadd&0xf0)>>4)&0xf);	//plc站地址，高位在前
		*(U8 *)(AD1+2+ps*i)=asicc(plcadd&0xf);
		*(U8 *)(AD1+3+ps*i)=0x57;            				//写配方，就是W
		*(U8 *)(AD1+4+ps*i)=0x53;            				//下两个长度发送的是命令类型SB(连续)，即53 42
		*(U8 *)(AD1+5+ps*i)=0x42; 
		*(U8 *)(AD1+6+ps*i)=0x30;            				//下2个长度发送的是地址字符和开始地址共7个长度 
		*(U8 *)(AD1+7+ps*i)=0x38; 
		*(U8 *)(AD1+8+ps*i)=0x25;            				//下3个长度发送的是地址字符%DW       
		*(U8 *)(AD1+9+ps*i)=0x44; 
	    *(U8 *)(AD1+10+ps*i)=0x57;
	    
		b=staradd+i*16;                        				//每次偏移16
		a1=b/10000;											//最后一个字节是十六进制发送，都要转成ASC码
		a2=(b%10000)/1000;
		a3=(b%1000)/100;
		a4=(b%100)/10;	
		a5=b%10;	
			    
		*(U8 *)(AD1+11+ps*i)=asicc(a1);        				//开始的地址，asicc显示，高位先发
		*(U8 *)(AD1+12+ps*i)=asicc(a2);	
		*(U8 *)(AD1+13+ps*i)=asicc(a3);     
		*(U8 *)(AD1+14+ps*i)=asicc(a4);
		*(U8 *)(AD1+15+ps*i)=asicc(a5);
		
		b1=(length>>4)&0xf;                    				//写的个数处理，b1是高位，b2是低位
		b2=length&0xf;
		
		*(U8 *)(AD1+16+ps*i)=asicc(b1);        				//长度，asicc显示
		*(U8 *)(AD1+17+ps*i)=asicc(b2);
		
		for(j=0;j<length;j++)
		{
			b3=*(U8 *)(PE+9+i*32+j*2);         				//从数组中取数据，B3是低位，B4是高位
			b4=*(U8 *)(PE+9+i*32+j*2+1);
				
			c1=b3&0xf;               						//对所写数据作asicc处理
			c2=(b3>>4)&0xf;          						//对所写数据作asicc处理		
			c3=b4&0xf;               						//对所写数据作asicc处理
			c4=(b4>>4)&0xf;          						//对所写数据作asicc处理				
		
			*(U8 *)(AD1+18+j*4+ps*i)=asicc(c4);   			//高位先发
			*(U8 *)(AD1+19+j*4+ps*i)=asicc(c3);
			*(U8 *)(AD1+20+j*4+ps*i)=asicc(c2);
			*(U8 *)(AD1+21+j*4+ps*i)=asicc(c1);				
		}
		*(U8 *)(AD1+18+ps*i+length*4)=0x04;	     			//结束字符

		ptcom->send_length[i]=19+length*4;					//发送长度
		ptcom->send_staradd[i]=i*ps;						//发送数据存储地址	
		ptcom->return_length[i]=7;							//返回数据长度
		ptcom->return_start[i]=0;							//返回数据有效开始
		ptcom->return_length_available[i]=0;				//返回有效数据长度	
			
	}
	ptcom->send_times=SendTimes;							//发送次数
	ptcom->Current_Times=0;									//当前发送次数
	ptcom->Simens_Count=0;		
}

/*************************************************************************
 * Funciton: 从PLC写时间  
 * Parameters: 无
 * Return: 无
**************************************************************************/
void Write_Time()
{
	Write_Analog();									
}

/*************************************************************************
 * Funciton: 从PLC读取时间 
 * Parameters: 无
 * Return: 无
**************************************************************************/
void Read_Time()
{
	Read_Analog();	
}

/*************************************************************************
 * Funciton: 处理成标准存储格式
 * Parameters: 无
 * Return: 无
**************************************************************************/
void compxy(void)
{
	int i;
	unsigned char a1,a2,a3,a4;
	int b,b1;
	int t[8];
	if(ptcom->Simens_Count==1)//==1时。位返回处理，位以字为单位读取，返回主程序以字节为单位且低在前
	{                                  
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/4;i++)//以字为单位，ASC码返回，4个字节为1字
 		 {		
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4);
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+1);
			a3=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+2);
			a4=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+3);	
			a1=bsicc(a1);                           			
			a2=bsicc(a2);
			a3=bsicc(a3);
			a4=bsicc(a4);                      				
			b=(a1<<4)+a2;                          				
			b1=(a3<<4)+a4;                         				
			*(U8 *)(COMad+i*2)=b1;//接收的第二个字节为0-7bit				   				
			*(U8 *)(COMad+i*2+1)=b;
		}
	}
	else if (ptcom->Simens_Count==2)//==2时。TC返回处理，TC以一个字节为单位读取，返回代码中一个BIT占4位为30 31 30 xx，其中xx表示状态，xx=0x31为ON，xx=0x30为OFF
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/4;i++)
 		 {		
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4);	
			t[i]=bsicc(a1);                                           
		}
		b=(t[7]<<7)+(t[6]<<6)+(t[5]<<5)+(t[4]<<4)+(t[3]<<3)+(t[2]<<2)+(t[1]<<1)+t[0];//以1个字节为单位，返回数据组合成1个字节回给主程序
		*(U8 *)(COMad+0)=b;									
		
		ptcom->address=ptcom->address+8*(ptcom->Current_Times-1);		
	}
	else if (ptcom->Simens_Count==3)//==3时。字返回处理，依次由高到低，如0x04d2返回数据为30 34 44 32
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
		b=(a1<<4)+a2;                          				
		b1=(a3<<4)+a4;                         				
		*(U8 *)(COMad+i*2)=b;				   			
		*(U8 *)(COMad+i*2+1)=b1;	
		}
	}			
	ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;//长度减半	
}

/*************************************************************************
 * Funciton: 检查数据校检
 * Parameters: 无
 * Return: 无
**************************************************************************/
void watchcom(void)
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

/*************************************************************************
 * Funciton: 转为Asc码
 * Parameters: a需转化的实数
 * Return: 无
**************************************************************************/
int asicc(int a)
{
	int bl;
	if(a<10)
		bl=a+0x30;
	if(a>9)
		bl=a-10+0x41;
	return bl;	
}

/*************************************************************************
 * Funciton: Asc转为数字
 * Parameters: a需转化的ASC码
 * Return: 无
**************************************************************************/
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

/*************************************************************************
 * Funciton: 检测数据正确性
 * Parameters: 无
 * Return: 无
**************************************************************************/
int remark()
{
	return 1;
}

/*************************************************************************
 * Funciton: 计算和校检
 * Parameters: chData：校验字节组指针。uNo：个数
 * Return: 无
**************************************************************************/
U16 CalcHe(unsigned char *chData,U16 uNo)		
{
	int i;
	int ab=0;
	for(i=0;i<uNo;i++)
	{
		ab=ab+chData[i];
	}
	return (ab);
}
