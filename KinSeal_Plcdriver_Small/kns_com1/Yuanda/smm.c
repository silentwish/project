/******************************************************************************************
 * 2015-12-04
 * 远大通讯协议
******************************************************************************************/
#include "stdio.h"
#include "def.h"
#include "smm.h"

struct Com_struct_D *ptcom;
U8 ReadData[50];
int Datd_length;

void Enter_PlcDriver(void)
{	
	LB=*(U32 *)(LB_Address+0);
	LW=*(U32 *)(LW_Address+0);
	RWI=*(U32 *)(RWI_Address+0);
	
	ptcom=(struct Com_struct_D *)adcom;
	
//	sysprintf("enter plc driver %c,ptcom->R_W_Flag %d\n",ptcom->registerr,ptcom->R_W_Flag);		
		
	switch (ptcom->R_W_Flag)
	{
		case PLC_READ_DATA:				//进入驱动是读数据
		case PLC_READ_DIGITALALARM:		//进入驱动是读数据,报警	
		case PLC_READ_TREND:			//进入驱动是读数据,趋势图
		case PLC_READ_ANALOGALARM:		//进入驱动是读数据,类比报警	
		case PLC_READ_CONTROL:			//进入驱动是读PLC控制的数据
			switch(ptcom->registerr)
			{
				case 'x':		//参数查询 DA
				case 'y':		//部件查询	DB	
				case 'Y':		//部件查询	DB_Bit
				case 'l':		//工况查询 DC
				case 'm':		//能耗查询 DE
				case 'c':		//模式切换 DF
				case 'C':		//模式切换查询位
				case 'X':		//参数位查询 DA_bit
				case 'L':		//工况位
				//case 'M':		//能耗位
				//case 'A':		//参数设置
				//case 'D':		//部件设置
				//case 'R':		//模式设置
					Read_Analog();
					//Write_Analog();
					break;
				default:
					break;			
			}
			break;
		case PLC_WRITE_DATA:				//进入驱动是写数据
		case PLC_WRITE_TIME:				//进入驱动是写时间到PLC
		case PLC_WRITE_RECIPE:				//进入驱动是写配方到PLC
			switch(ptcom->registerr)
			{
				case 'x':		//参数设置DA
				case 'X':		//参数位设置DA_bit
				case 'D': 		//部件设置DH
				case 'R':		//模式设置 DI
				case 'y':		//开关位   DB_Bit
				case 'Y':		//开关位   DB_Bit
				case 'L':      // 工况位 DC_Bit
				case 'c':		//模式切换 DF
				case 'C':		//模式切换查询位
					Write_Analog();		
					break;
				default:
				ptcom->send_length[0]=0;				//发送长度
				ptcom->send_staradd[0]=0;				//发送数据存储地址	
				ptcom->send_times=0;					//发送次数
				ptcom->return_length[0]=0;				//返回数据长度
				ptcom->return_start[0]=0;				//返回数据有效开始
				ptcom->return_length_available[0]=0;	//返回有效数据长度	
				ptcom->Current_Times=0;	
					break;				
			}
			break;		
		case PLC_CHECK_DATA:				//进入驱动是数据处理
			watchcom();
			break;
		default:
			break;					
	}	 
}

void Read_Analog()				//控件是读属性，发送查询帧
{
	int b;
	U32 addr,newaddr;
	U8 type_code,newtype; //类别码
	U8 num_m;    //机器数
	int plcadd;
	int length;
	int t;
	int i;
	U8 sw ,sw1 ; //开关
	U8 reg;  //寄存器标志
	U8 arg1;
	U8 size_c; //数据帧字节数
	reg = ptcom->registerr; //得到寄存器下标
	addr = ptcom->address;			      //在主程序已经转换到该段的开始地址
	 //寄存器不同,对于addr的处理方式也不同

	//sysprintf("Read_Analog*******************\n");
	if(ptcom->registerr == 'X' || ptcom->registerr == 'L' || ptcom->registerr == 'Y' || ptcom->registerr == 'V')
	{
		type_code = (U8)((addr>>12)&0X0f);
	}
	else
	{
		type_code = (U8)((addr>>8)&0X0f);
	}
//	sysprintf("Read_Analog*******************ptcom->registerr %c,type_code %d \n",ptcom->registerr,type_code);

	switch (ptcom->registerr)	//根据寄存器类型获取对应的功能码
	{
		case 'x':		//参数查询 DA
		case 'X':      //参数位 DA_Bit
			t=0x02;
			switch(type_code)
			{
				case 2: //盘管,先查询新风机，获得室内温度才能查询盘管
					t = 0x01;
					if(ptcom->Simens_Count != 100)
					{
						ptcom->return_length[0] = 35;
						ptcom->return_length_available[0] = 25;

						ptcom->return_length[1] = 21;
						ptcom->return_length_available[1] = 11;
					}
					else if(ptcom->Simens_Count == 100)
					{
						ptcom->return_length[0] = 21;
						ptcom->return_length_available[0] = 11;
					}
					break;
				case 8: // 风咀
				case 12://除湿机
					ptcom->return_length[0] = 20;
					ptcom->return_length_available[0] = 10;
					break;
				case 10:// 新风机
					ptcom->return_length[0] = 35;
					ptcom->return_length_available[0] = 25;
					break;
				case 11://加湿器
					t = 0x01;
					ptcom->return_length[0] = 19;
					ptcom->return_length_available[0] = 9;
					break;
				default:
					break;
			}
			break;

		case 'y':		//部件查询  DB
		case 'Y':      //部件设置 DB_Bit
			t=0x04;
			ptcom->return_length[0] = 11;
			ptcom->return_length_available[0] = 1;
			break;
		case 'l':		//工况查询 DC
		case 'L':      //工况位 DC_Bit
			t=0x01;
			switch(type_code)
			{
				case 2: //盘管,先查询新风机，获得室内温度才能查询盘管
					if(ptcom->Simens_Count != 100)
					{
						ptcom->return_length[0] = 35;
						ptcom->return_length_available[0] = 25;

						ptcom->return_length[1] = 21;
						ptcom->return_length_available[1] = 11;
					}
					else if(ptcom->Simens_Count == 100)
					{
						ptcom->return_length[0] = 21;
						ptcom->return_length_available[0] = 11;
					}
					break;
				case 8: // 风咀
				case 12://除湿机
					ptcom->return_length[0] = 20;
					ptcom->return_length_available[0] = 10;
					break;
				case 10:// 新风机
					ptcom->return_length[0] = 34;
					ptcom->return_length_available[0] = 24;
					break;
				case 11://加湿器
					ptcom->return_length[0] = 19;
					ptcom->return_length_available[0] = 9;
					break;
				default:
					break;
			}
			break;
		case 'm':		//能耗查询 DE
			if(type_code == 10) //新风机
			{
				t=0x03;
				ptcom->return_length[0] = 24;
				ptcom->return_length_available[0] = 14;
			}
			else if(type_code == 2)
			{
				//sysprintf("Read_Analog pangguan\n");
				t=0x02;
				ptcom->return_length[0] = 40;
				ptcom->return_length_available[0] = 30;
			}
			break;
		case 'c':		//模式切换查询 DF
		case 'C':     //模式设置DF_Bit
			t=0x06;
			ptcom->return_length[0] = 11;
			ptcom->return_length_available[0] = 1;
			break;
		default:
			break;		
	}
	if(ptcom->registerr == 'X' || ptcom->registerr == 'L' || ptcom->registerr == 'Y' || ptcom->registerr == 'V')
	{
		addr = addr/16;
	}
//	b=b+add;					      //加上偏移地址

	type_code = ((addr>>8)&0x0f);
	num_m = addr>>12;

	newaddr=(num_m<<12)+(type_code<<8)+7;  //远大协议特殊，开始地址都是07，一次性读完
	if(ptcom->registerr == 'X' || ptcom->registerr == 'L' || ptcom->registerr == 'Y' || ptcom->registerr == 'C')
	{
		newaddr = newaddr<<4;
	}
	ptcom->address=newaddr;
	ptcom->register_length=ptcom->return_length_available[0];
//	sysprintf("newaddr 0x%x\n",newaddr);

	if((type_code==2)&&((ptcom->registerr=='x')||(ptcom->registerr=='X')))	//盘管,先查询新风机
		newtype=10;
	else
		newtype=type_code;
//	



	if((type_code==2)&&((ptcom->registerr=='x')||(ptcom->registerr=='X')))	//盘管,先查询新风机
	{
		if(ptcom->Simens_Count != 100)
		{
			*(U8 *)(AD1+0)=0xF7;		//开始码高位
			*(U8 *)(AD1+1)=0xF8;		//开始码地位
			*(U8 *)(AD1+2)=0x05;			//字节数
			*(U8 *)(AD1+3)=newtype;       //类别码
			*(U8 *)(AD1+4)=0x00;      	//地址码高位
			*(U8 *)(AD1+5)=num_m;     	//地址码低位
			*(U8 *)(AD1+6)=t;     		//功能码

			b= AddSum((U8 *)(AD1+2),5);  //校验

			*(U8 *)(AD1+7)=b&0xff;     //校验码
			*(U8 *)(AD1+8)=0xfd;     	//结束码

			ptcom->send_length[0]=9;				//发送长度
			ptcom->send_staradd[0]=0;				//发送数据存储地址	
			ptcom->send_times=1;					//发送次数
			
			ptcom->return_start[0]=7;				//返回数据有效开始,所有应答帧的有效开始都是从7开始
			ptcom->Current_Times=0;				//当前发送次数	
			ptcom->send_add[0]=ptcom->address;		//读的是这个地址的数据		

			ptcom->send_staradd[99]=1;	
		
			*(U8 *)(AD1+9)=0xF7;		//开始码高位
			*(U8 *)(AD1+10)=0xF8;		//开始码地位
			*(U8 *)(AD1+11)=0x06;			//字节数
			*(U8 *)(AD1+12)=type_code;       //类别码
			*(U8 *)(AD1+13)=0x00;      	//地址码高位
			*(U8 *)(AD1+14)=num_m;     	//地址码低位
			*(U8 *)(AD1+15)=t;     		//功能码
			ptcom->send_length[1]=10;				//发送长度
			ptcom->send_staradd[1]=9;				//发送数据存储地址	
			ptcom->send_times=2;					//发送次数
			
			ptcom->return_start[1]=7;				//返回数据有效开始,所有应答帧的有效开始都是从7开始
			ptcom->Current_Times=0;				//当前发送次数	
			ptcom->send_add[1]=ptcom->address;		//读的是这个地址的数据		

			ptcom->send_staradd[99]=1;	
			
		}
		else if(ptcom->Simens_Count == 100)
		{
			*(U8 *)(AD1+0)=0xF7;		//开始码高位
			*(U8 *)(AD1+1)=0xF8;		//开始码地位
			*(U8 *)(AD1+2)=0x06;			//字节数
			*(U8 *)(AD1+3)=type_code;       //类别码
			*(U8 *)(AD1+4)=0x00;      	//地址码高位
			*(U8 *)(AD1+5)=num_m;     	//地址码低位
			*(U8 *)(AD1+6)=t;     		//功能码
			*(U8 *)(AD1+7)=0xFF;      //数据码
			b= AddSum((U8 *)(AD1+2),6);  //校验

			*(U8 *)(AD1+8)=b&0xff;     //校验码
			*(U8 *)(AD1+9)=0xfd;     	//结束码
		
			ptcom->send_length[0]=10;				//发送长度
			ptcom->send_staradd[0]=0;				//发送数据存储地址	
			ptcom->send_times=1;					//发送次数
		
			ptcom->return_start[0]=7;				//返回数据有效开始,所有应答帧的有效开始都是从7开始
			ptcom->Current_Times=0;				//当前发送次数	
			ptcom->send_add[0]=ptcom->address;		//读的是这个地址的数据		

			ptcom->send_staradd[99]=1;	
		}
	}
	else
	{
		*(U8 *)(AD1+0)=0xF7;		//开始码高位
		*(U8 *)(AD1+1)=0xF8;		//开始码地位
		*(U8 *)(AD1+2)=0x05;			//字节数
		*(U8 *)(AD1+3)=newtype;       //类别码
		*(U8 *)(AD1+4)=0x00;      	//地址码高位
		*(U8 *)(AD1+5)=num_m;     	//地址码低位
		*(U8 *)(AD1+6)=t;     		//功能码

		b= AddSum((U8 *)(AD1+2),5);  //校验

		*(U8 *)(AD1+7)=b&0xff;     //校验码
		*(U8 *)(AD1+8)=0xfd;     	//结束码

		ptcom->send_length[0]=9;				//发送长度
		ptcom->send_staradd[0]=0;				//发送数据存储地址	
		ptcom->send_times=1;					//发送次数
			
		ptcom->return_start[0]=7;				//返回数据有效开始,所有应答帧的有效开始都是从7开始
		ptcom->Current_Times=0;				//当前发送次数	
		ptcom->send_add[0]=ptcom->address;		//读的是这个地址的数据		

		ptcom->send_staradd[99]=1;	
	}
}

void Write_Analog()				//写模拟量
{
	int b;  
	int i;
	int length;
	int plcadd;		
	int add;
	U8 t;
	U32 addr,newaddr;
	U8 reg;
	U8 type_code,newtype; //类别码
	U8 num_m;    //机器数
	U8 para_code; //参数码, 由地址得到需要设置的参数位置
	U8 sw_code;   //开关(小数位)
	U8 b1;
	U8 size_c; //数据帧字节数
	
	reg = ptcom->registerr; //得到寄存器下标
	addr = ptcom->address;			      //在主程序已经转换到该段的开始地址
	if(ptcom->registerr == 'X' || ptcom->registerr == 'L' || ptcom->registerr == 'Y' || ptcom->registerr == 'C')
	{
		type_code = (U8)((addr>>12)&0X0f);
	}
	else
	{
		type_code = (U8)((addr>>8)&0X0f);
	}
//	sysprintf("Write_Analog*******************ptcom->Simens_Count %d,ptcom->registerr %c,type_code %d \n",ptcom->Simens_Count,ptcom->registerr,type_code);
	if((ptcom->Simens_Count == 100)&&(type_code==10))  //盘管，不写新风机
	{
		ptcom->Current_Times=1000;	
		return;
	}
	switch (ptcom->registerr)	//根据寄存器类型获取对应的功能码
	{
		case 'x':		//参数查询 DA
		case 'X':      //参数位 DA_Bit
			switch(type_code)
			{
				case 10:// 新风机
					t=0x02;
					ptcom->return_length[0] = 35; // 参数查询的返回数据长度34
					ptcom->return_length_available[0] = 25; //返回有效数据长度
					break;
				
				case 2: //盘管,先查询新风机，获得室内温度才能查询盘管
					t = 0x01;
					if(ptcom->Simens_Count != 100)
					{
						ptcom->return_length[0] = 35;
						ptcom->return_length_available[0] = 25;

						ptcom->return_length[1] = 21;
						ptcom->return_length_available[1] = 11;
					}
					else if(ptcom->Simens_Count == 100)
					{
						ptcom->return_length[0] = 21;
						ptcom->return_length_available[0] = 11;
					}
					break;
				case 8: // 风咀
				case 12://除湿机
					ptcom->return_length[0] = 20;
					ptcom->return_length_available[0] = 10;
					break;
				case 11://加湿器
					t = 0x01;
					ptcom->return_length[0] = 19;
					ptcom->return_length_available[0] = 9;
					break;
				default:
					break;
			}
			break;
		case 'y':		//部件查询  DB
		case 'Y':      //部件设置 DB_Bit
			t=0x04;
			ptcom->return_length[0] = 11;
			ptcom->return_length_available[0] = 1;
			break;

		case 'm':		//能耗查询 DE
			t=0x03;
			if(type_code == 10) //新风机
			{
				ptcom->return_length[0] = 24;
				ptcom->return_length_available[0] = 14;
			}
			else if(type_code == 2)
			{
				ptcom->return_length[0] = 40;
				ptcom->return_length_available[0] = 30;
			}
			break;
		case 'C':		//模式切换查询 DF
		case 'c':     // 模式切换设置DF_Bit
			t=0x06;
			ptcom->return_length[0] = 11;
			ptcom->return_length_available[0] = 1;
			break;
		default:
			break;		
	}
	if(ptcom->registerr == 'X' || ptcom->registerr == 'L' || ptcom->registerr == 'Y' || ptcom->registerr == 'C')
	{
		addr = addr/16;
	}

	type_code = ((addr>>8)&0x0f);
	num_m = addr>>12;

	if((type_code==2)&&((ptcom->registerr=='x')||(ptcom->registerr=='X')))	//盘管,先查询新风机
		newtype=10;
	else
		newtype=type_code;
	if((type_code==2)&&((ptcom->registerr=='x')||(ptcom->registerr=='X')))	//盘管,先查询新风机
	{
		if(ptcom->Simens_Count != 100)
		{
			*(U8 *)(AD1+0)=0xF7;		//开始码高位
			*(U8 *)(AD1+1)=0xF8;		//开始码地位
			*(U8 *)(AD1+2)=0x05;			//字节数
			*(U8 *)(AD1+3)=newtype;       //类别码
			*(U8 *)(AD1+4)=0x00;      	//地址码高位
			*(U8 *)(AD1+5)=num_m;     	//地址码低位
			*(U8 *)(AD1+6)=t;     		//功能码

			b= AddSum((U8 *)(AD1+2),5);  //校验

			*(U8 *)(AD1+7)=b&0xff;     //校验码
			*(U8 *)(AD1+8)=0xfd;     	//结束码

			ptcom->send_length[0]=9;				//发送长度
			ptcom->send_staradd[0]=0;				//发送数据存储地址	
		//ptcom->send_times=1;					//发送次数
			
			ptcom->return_start[0]=7;				//返回数据有效开始,所有应答帧的有效开始都是从7开始
		//ptcom->Current_Times=0;				//当前发送次数	
		//ptcom->send_add[0]=ptcom->address;		//读的是这个地址的数据		

			ptcom->send_staradd[99]=1;	

			*(U8 *)(AD1+9)=0xF7;		//开始码高位
			*(U8 *)(AD1+10)=0xF8;		//开始码地位
			*(U8 *)(AD1+11)=0x06;			//字节数
			*(U8 *)(AD1+12)=type_code;       //类别码
			*(U8 *)(AD1+13)=0x00;      	//地址码高位
			*(U8 *)(AD1+14)=num_m;     	//地址码低位
			*(U8 *)(AD1+15)=t;     		//功能码
			ptcom->send_length[1]=10;				//发送长度
			ptcom->send_staradd[1]=9;				//发送数据存储地址	
			ptcom->send_times=2;					//发送次数
			
			ptcom->return_start[1]=7;				//返回数据有效开始,所有应答帧的有效开始都是从7开始
			ptcom->Current_Times=0;				//当前发送次数	
			ptcom->send_add[1]=ptcom->address;		//读的是这个地址的数据		

			ptcom->send_staradd[99]=1;	
			
		}
		else if(ptcom->Simens_Count == 100)
		{
			*(U8 *)(AD1+0)=0xF7;		//开始码高位
			*(U8 *)(AD1+1)=0xF8;		//开始码地位
			*(U8 *)(AD1+2)=0x06;			//字节数
			*(U8 *)(AD1+3)=type_code;       //类别码
			*(U8 *)(AD1+4)=0x00;      	//地址码高位
			*(U8 *)(AD1+5)=num_m;     	//地址码低位
			*(U8 *)(AD1+6)=t;     		//功能码
			*(U8 *)(AD1+7)=0xFF;      //数据码
			b= AddSum((U8 *)(AD1+2),6);  //校验

			*(U8 *)(AD1+8)=b&0xff;     //校验码
			*(U8 *)(AD1+9)=0xfd;     	//结束码
		
			ptcom->send_length[0]=10;				//发送长度
			ptcom->send_staradd[0]=0;				//发送数据存储地址	
			ptcom->send_times=2;					//发送次数
		
			ptcom->return_start[0]=7;				//返回数据有效开始,所有应答帧的有效开始都是从7开始
			ptcom->Current_Times=0;				//当前发送次数	
			ptcom->send_add[0]=ptcom->address;		//读的是这个地址的数据		

			ptcom->send_staradd[99]=1;	
		}
	}
	else
	{
		*(U8 *)(AD1+0)=0xF7;		//开始码高位
		*(U8 *)(AD1+1)=0xF8;		//开始码地位
		*(U8 *)(AD1+2)=0x05;			//字节数
		*(U8 *)(AD1+3)=newtype;       //类别码
		*(U8 *)(AD1+4)=0x00;      	//地址码高位
		*(U8 *)(AD1+5)=num_m;     	//地址码低位
		*(U8 *)(AD1+6)=t;     		//功能码

		b= AddSum((U8 *)(AD1+2),5);  //校验

		*(U8 *)(AD1+7)=b&0xff;     //校验码
		*(U8 *)(AD1+8)=0xfd;     	//结束码

		ptcom->send_length[0]=9;				//发送长度
		ptcom->send_staradd[0]=0;				//发送数据存储地址	
		//ptcom->send_times=1;					//发送次数
			
		ptcom->return_start[0]=7;				//返回数据有效开始,所有应答帧的有效开始都是从7开始
		//ptcom->Current_Times=0;				//当前发送次数	
		//ptcom->send_add[0]=ptcom->address;		//读的是这个地址的数据		

		ptcom->send_staradd[99]=1;	
	
	}
	
	switch (ptcom->registerr)	//根据寄存器类型获取对应的类别码
	{
		case 'x':				//参数 设置  DA
		case 'X':				 //参数位 DA_Bit
			t=0x81;
			type_code = ((addr>>8)&0x0f);
			num_m = addr>>12;
			para_code = (U8) addr; // 强制转换的到后两位, 地址
			switch(type_code)  
			{
				case 10: //新风,0a
					size_c = 0x1E;
					ptcom->send_length[1] = 34;
					ptcom->return_length[1] = 35;
					ptcom->return_length_available[1] = 25;
					break;
				case 2://盘管,02
					size_c = 0x0A;
					if(ptcom->Simens_Count != 100)
					{
						ptcom->send_length[2] = 14;
						ptcom->return_length[2] = 14;
						ptcom->return_length_available[2] = 5;
					}
					else if(ptcom->Simens_Count == 100)
					{
						ptcom->send_length[1] = 14;
						ptcom->return_length[1] = 14;
						ptcom->return_length_available[1] = 5;
					}
					
					break;
				case 11://加湿器,0b
				case 8://风咀,08
					size_c = 0x09;
					ptcom->send_length[1] = 13;
					ptcom->return_length[1] = 13;
					ptcom->return_length_available[1] = 4;
					break;
				default:
					break;
			}
			break;
//		case 'D':				//部件查询 设置	
		case 'y':		//部件查询  DB
		case 'Y':      //部件设置 DB_Bit
			t=0x05;
			type_code = ((addr>>8)&0x0f);
			num_m = addr>>12;
			para_code = (U8) addr; // 强制转换的到后两位, 地址
			switch(type_code)  
			{
				case 10: //新风机
					size_c = 0x07;
					ptcom->send_length[1] = 11;
					ptcom->return_length[1] = 11;
					ptcom->return_length_available[1] = 2;
					break;
				case 2://盘管
				case 11://加湿器
					size_c = 0x06;
					ptcom->send_length[1] = 10;
					ptcom->return_length[1] = 10;
					ptcom->return_length_available[1] = 1;
					break;
				default:
					break;
			}
			break;
		case 'c':				//模式切换 设置	DF
		case 'C':              //模式切换设置位DF_Bit
			t=0x07;
			type_code = ((addr>>8)&0x0f);
			num_m = addr>>12;
			para_code = (U8) addr; // 强制转换的到后两位, 地址
			size_c = 0x06;
			ptcom->send_length[1] = 10;
			ptcom->return_length[1] = 11;
			ptcom->return_length_available[1] = 1;
			break;
		case 'L':    //工况位DC_Bit
			break;
		default:
			break;		
	}
	//sysprintf("function %x \n", t);
	if((type_code==2)&&((ptcom->registerr=='x')||(ptcom->registerr=='X')))	//盘管,先查询新风机
	{
//下面是第三桢数据
		if(ptcom->Simens_Count != 100)
		{
			*(U8 *)(AD1+19)=0xF7;		//开始码高位
			*(U8 *)(AD1+20)=0xF8;		//开始码地位
			*(U8 *)(AD1+21)=size_c;			//字节数
			*(U8 *)(AD1+22)=type_code;       //类别码
			*(U8 *)(AD1+23)=0x00;      	//地址码高位
			*(U8 *)(AD1+24)=num_m;     	//地址码低位
			*(U8 *)(AD1+25)=t;     		//功能码

			ptcom->send_staradd[2]=19;				//发送数据存储地址	
			ptcom->send_times=3;					//发送次数
			
			ptcom->return_start[2]=7;				//返回数据有效开始
			ptcom->Current_Times=0;			
			ptcom->send_staradd[99]= 1;//先读后写
		}
		else if(ptcom->Simens_Count == 100)
		{
			*(U8 *)(AD1+10)=0xF7;		//开始码高位
			*(U8 *)(AD1+11)=0xF8;		//开始码地位
			*(U8 *)(AD1+12)=size_c;			//字节数
			*(U8 *)(AD1+13)=type_code;       //类别码
			*(U8 *)(AD1+14)=0x00;      	//地址码高位
			*(U8 *)(AD1+15)=num_m;     	//地址码低位
			*(U8 *)(AD1+16)=t;     		//功能码

			ptcom->send_staradd[1]=10;				//发送数据存储地址	
			ptcom->send_times=2;					//发送次数
			
			ptcom->return_start[1]=7;				//返回数据有效开始
			ptcom->Current_Times=0;			
			ptcom->send_staradd[99]= 1;//先读后写
		}

	}
	else
	{
//下面是第二帧数据，先查询，再设置
		*(U8 *)(AD1+9)=0xF7;		//开始码高位
		*(U8 *)(AD1+10)=0xF8;		//开始码地位
		*(U8 *)(AD1+11)=size_c;			//字节数
		*(U8 *)(AD1+12)=type_code;       //类别码
		*(U8 *)(AD1+13)=0x00;      	//地址码高位
		*(U8 *)(AD1+14)=num_m;     	//地址码低位
		*(U8 *)(AD1+15)=t;     		//功能码

		ptcom->send_staradd[1]=9;				//发送数据存储地址	
		ptcom->send_times=2;					//发送次数
		
		ptcom->return_start[1]=7;				//返回数据有效开始
		ptcom->Current_Times=0;			
		ptcom->send_staradd[99]= 1;//先读后写
	}
	//sysprintf("ptcom->send_length[1]  %d,ptcom->send_staradd[1] %d \n",ptcom->send_length[1],ptcom->send_staradd[1] );
}

void setalarm(U32 addr)
{
	char value;
	U8 para_code;
	U8 type_code; //类别码
	int k;
	
	type_code = ((addr>>8)&0x0f);
	para_code = (U8)(addr);
	//sysprintf("addr**********************%x\n", addr);
/*
	value=*((U8 *)LW + 2000 + para_code);
	value=value>>k;				//右移设定的编号数
	value=value&0x01;	
	*/
	//*(U32 *)(LB_Address+1000)=1;
	//for(k = 17; k<27 ; k++)
	//sysprintf("finall LocalWord[%d] = 0x%x\n",2000+k, *(U8 *)(LW+2000+k));
	
	switch(type_code)
	{
		case 10: //新风机的故障位  LB1000-LB1020 21个
			value=*(U8 *)(LW + 2000 + 17); // 1A18的8个故障位			
			for(k = 0; k<8;k++)  // 5bian8
			{
				if(((value>>k)&0x01) == 1 )
				{
					*(U8 *)(LB+1500+k)= 1;
				}
				else
				{
					*(U8 *)(LB+1500+k) = 0;
				}
			}
			value=*(U8 *)(LW + 2000 + 18); // 1A19的8个故障位
			for(k = 0; k<8;k++)
			{
				if(((value>>k)&0x01) == 1 )
				{
					*(U8 *)(LB+1508+k)=1;
				}
				else
				{
					*(U8 *)(LB+1508+k)=0;
				}
			}
			value=*(U8 *)(LW + 2000 + 19); // 1A1A的5个故障位
			for(k = 0; k<5;k++)
			{
				if(((value>>k)&0x01) == 1 )
				{
					*(U8 *)(LB+1516+k)=1;
				}
				else
				{
					*(U8 *)(LB+1516+k)=0;
				}
			}
			break;
		case 2:
			value=*(U8 *)(LW + 2000 + 5); // 120C的5个故障位
			for(k = 0; k<5;k++)
			{
				if(((value>>k)&0x01) == 1 )
				{
					*(U8 *)(LB+1521+k)=1;
				}
				else
				{
					*(U8 *)(LB+1521+k)=0;
				}
			}
			break;
		case 11: //加湿器故障位LB1026-LB1031
			value=*(U8 *)(LW + 2000 + 5); // 1B0C的6个故障位
			for(k = 0; k<6;k++)
			{
				if(((value>>k)&0x01) == 1 )
				{
					*(U8 *)(LB+1526+k)=1;
				}
				else
				{
					*(U8 *)(LB+1526+k)=0;
				}
			}
			break;
		case 8: //风咀的第一个报警位是5，故加1027，报警地址从LB1032开始到LB1034
			value=*(U8 *)(LW + 2000 + 3); // 180A的3个故障位
			for(k = 5; k<8;k++)
			{
				if(((value>>k)&0x01) == 1 )
				{
					*(U8 *)(LB+1527+k)=1;
				}
				else 
				{
					*(U8 *)(LB+1527+k)=0;
				}
			}
			break;
		case 12: //除湿机 LB1035-LB1039
			value=*(U8 *)(LW + 2000 + 6); // 1B0C的5个故障位
			for(k = 0; k<5;k++)
			{
				if(((value>>k)&0x01) == 1 )
				{
					*(U8 *)(LB+1535+k)=1;
				}
				else
				{
					*(U8 *)(LB+1535+k)=0;
				}
			}
			break;
		default:
			break;
	}
	//for(k = 0; k<10 ; k++)
	//sysprintf("finall LocalBit[%d] = %d\n",1500+k, *(U8 *)(LB+1500+k));
}

void Process_DoubleCoil()	//新风机+盘管的处理
{
		U8 new_code,para_code; //参数码, 由地址得到需要设置的参数位置
	char high,low,funnum,b;
	short i,data,data1;
	int addr,k;

	addr=ptcom->address;			      //在主程序已经转换到该段的开始地址
//	sysprintf("Process_Coil,ptcom->send_times %d\n",ptcom->send_times);
//	sysprintf("Process_Coil,ptcom->Current_Times %d\n",ptcom->Current_Times);

	if(ptcom->send_times==2) //说明是查询新风机以后的查询盘管
	{
/*******************
		*(U8 *)(AD1+9)=0xF7;		//开始码高位
		*(U8 *)(AD1+10)=0xF8;		//开始码地位
		*(U8 *)(AD1+11)=0x06;			//字节数
		*(U8 *)(AD1+12)=type_code;       //类别码
		*(U8 *)(AD1+13)=0x00;      	//地址码高位
		*(U8 *)(AD1+14)=num_m;     	//地址码低位
		*(U8 *)(AD1+15)=t;     		//功能码

*******************/
		if(ptcom->Current_Times==1) //查询新风机
		{
			low= *(U8 *)(LW + 2000 + 7-7);  //D8D7 10倍16进制室内温度
			high= *(U8 *)(LW + 2000 +8-7);
		//	sysprintf("Read_Analog*******************00 high %d low %d\n",high,low);
			
			data=(short)(high<<8|low)-450;
			data1=data/10;	//实际发送功能码,2倍16进制
		//	sysprintf("Read_Analog*******************00 data %d data1 %d\n",data,data1);

			funnum=data1*2;
		//	sysprintf("Read_Analog*******************11 data %d funnum %d\n",data1,funnum);
			*(U8 *)(AD1+16)=funnum;
			b= AddSum((U8 *)(AD1+11),6);     //校验

			*(U8 *)(AD1+17)=b&0xff;    		//校验码
			*(U8 *)(AD1+18)=0xfd;     			//结束码
		}
	
	}
	else if(ptcom->send_times==3)  //先查新风机，再查盘管，再设置
	{
		if(ptcom->Current_Times==1) //查询新风机
		{
/*******************
		*(U8 *)(AD1+9)=0xF7;		//开始码高位
		*(U8 *)(AD1+10)=0xF8;		//开始码地位
		*(U8 *)(AD1+11)=0x06;			//字节数
		*(U8 *)(AD1+12)=type_code;       //类别码
		*(U8 *)(AD1+13)=0x00;      	//地址码高位
		*(U8 *)(AD1+14)=num_m;     	//地址码低位
		*(U8 *)(AD1+15)=t;     		//功能码

*******************/
				low= *(U8 *)(LW + 2000 + 7-7);  //D8D7 10倍16进制室内温度
				high= *(U8 *)(LW + 2000 + 8-7);
				data=high<<8|low;
				funnum=data/5;	//实际发送功能码,2倍16进制
				*(U8 *)(AD1+16)=funnum;
				b= AddSum((U8 *)(AD1+11),6);     //校验

				*(U8 *)(AD1+17)=b&0xff;    		//校验码
				*(U8 *)(AD1+18)=0xfd;     			//结束码

		}
		else if(ptcom->Current_Times==2) //查询盘管
		{
/***************************************
查询盘管正确以后,
					D7,设定温度
					D8,室内温度
					D9,空调口入水温度
D10	B0	盘管关	1-有效     0-无效	
	B1	盘管开	1-有效     0-无效	
	B3B2	/	/	预留
	B5B4	温度单位	01-F       10-C	
	B6	空调阀	1-开     0-关	
	B7	/	/	预留
D11	B0	制冷模式	1-有效     0-无效	
	B1	制热模式	1-有效     0-无效	
	B2	冷热自动转	0-无效     1-有效	
	B3	热量表	0-无效     1-有效	
	B4	自动模式空调模式	1-制热     0-制冷	
	B5	温度控制类型	0-普通盘管     
							1-双温盘管	 
	B6	盘管防冻强制保护	1-有效     0-无效	 
	B7	/	/	预留
D12	B0	室内温度传感器异常	1-有效     0-无效	
	B1	空调水入口温探故障	1-有效     0-无效	内盘管温探
	B2	空调水出口温探故障	1-有效     0-无效	
	B3	热量表故障	1-有效     0-无效	
	B4	盘管水温过低报警	1-有效     0-无效	 
	B5-B7	/	/	预留
D17D 16	空调阀开关时间
设置盘管参数如下:
	D7	设定温度
	D11D10 	空调阀开关时间
	D8	B0	盘管关	1-有效     0-无效	
		B1	盘管开	1-有效     0-无效	
		B2	/	/	预留
		B3	能耗清零	1-有效     0-无效	
		B5B4	温度单位	01-F       10-C	广播命令
		B6	故障复位	1-有效     0-无效	广播命令
		B7	历史故障清零	1-有效     0-无效	
	D9	B0	制冷模式	1-有效     0-无效	手动
		B1	制热模式	1-有效     0-无效	手动
		B2	冷热自动转	1-有效     0-无效	
		B3	热量表	1-有效     0-无效	
		B4 	自动模式空调模式	1-制热     0-制冷	
		B5	温度控制类型	0-普通盘管     
								1-双温盘管	
		B6	盘管防冻强制保护	1-有效     0-无效	 
		B7	/	/	预留

***************************************/
//19为前两帧发送数据的长度
				*(U8 *)(AD1 + 19+7) = *(U8 *)(LW + 2000 + 7-7);		//工况查询D7对应设置参数D7
				*(U8 *)(AD1 + 19+8)= *(U8 *)(LW + 2000 + 10-7);		//工况查询D10对应参数设置D8
				*(U8 *)(AD1 + 19+9) = *(U8 *)(LW + 2000 + 11-7);		//工况查询D11对应参数设置D9
				*(U8 *)(AD1 + 19+10)= *(U8 *)(LW + 2000 + 16-7);		//工况查询D16对应参数设置D10
				*(U8 *)(AD1 + 19+11) = *(U8 *)(LW + 2000 + 17-7);		//工况查询D17对应参数设置D11
				if(ptcom->registerr == 'X' )		//参数位
				{
					k = addr%8;
					addr = addr/16;
				}
				new_code = (U8) addr; // 强制转换的到后两位, 地址

				switch(new_code)		//将查询地址转换成参数设置地址
				{
						case	7:
								para_code=7;
								break;
						case 10:
								para_code=8;
								break;
						case 11:
								para_code=9;
								break;
					case 16:
								para_code=10;
								break;
				}
		//		sysprintf("Process_Coil new_code %d,para_code %d,ptcom->U8_Data[0] %d,k %d\n",new_code,para_code,ptcom->U8_Data[0],k);

				if(ptcom->registerr == 'X')		//参数位
				{
			//	sysprintf("0 *(U8 *)(AD1 +10+ para_code) %x\n",*(U8 *)(AD1 +10+ para_code));
					if(new_code == 10)
					{
						if(ptcom->U8_Data[0] == 1)
						{
							*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (1 << k)|(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code)); //将输入值传给寄存器
							if(k==0) //盘管关有效，使能开无效
							{
								*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(1 << 1))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code));
							}
							else if(k==1) //盘管开有效，使能关无效
							{
								*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(1 << 0))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code));
							}
								
						}
						else if(ptcom->U8_Data[0] == 0)
						{
							*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(1 << k))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code)); //将输入值传给寄存器
						}
					}
					else if(new_code == 11)
					{
						if(ptcom->U8_Data[0] == 1)
						{
							*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (1 << k)|(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code)); //将输入值传给寄存器
							if(k==0)
							{
								*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(6 << 0))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code));
							}
							else if(k==1)
							{
								*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(5 << 0))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code));
							}
							else if(k==2)
							{
								*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(3 << 0))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code));
							}
							else
							{

							}
						}
						else if(ptcom->U8_Data[0] == 0)
						{
							*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(1 << k))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code)); //将输入值传给寄存器
						}
					}
		//		sysprintf("1 *(U8 *)(AD1 +10+ para_code) %x\n",*(U8 *)(AD1 +10+ para_code));

				}			
				else if(ptcom->registerr == 'x')		//参数字
				{
					if(para_code == 8)// 满足 D8温度单位 特殊处理
					{
						*(U8 *)(AD1 +19+ para_code) = ((ptcom->U8_Data[0]<<4)|((*(U8 *)(AD1 +19+ para_code))&0x0f));
					}
					else if( (para_code == 10) )		//D11D 10	空调阀开关时间
					{
						*(U8 *)(AD1 +19+ para_code) = ptcom->U8_Data[0]; //将输入值传给寄存器
						*(U8 *)(AD1 +20+ para_code) = ptcom->U8_Data[1]; //将输入值传给寄存器
					}
					else 
					{
						*(U8 *)(AD1 +10+ para_code) = ptcom->U8_Data[0]; //将输入值传给寄存器
					}

				}
				b= AddSum((U8 *)(AD1+21),ptcom->send_length[1] - 4);     //校验

				*(U8 *)(AD1+19+ptcom->send_length[1]-2)=b&0xff;    		//校验码
				*(U8 *)(AD1+19+ptcom->send_length[1]-1)=0xfd;     			//结束码
			
		}
	}
}

void Process_SingleCoil()	//盘管单独的处理
{
		U8 new_code,para_code; //参数码, 由地址得到需要设置的参数位置
	char high,low,funnum,b;
	short i,data,data1;
	int addr,k;

	addr=ptcom->address;			      //在主程序已经转换到该段的开始地址
//	sysprintf("Process_Coil,ptcom->send_times %d\n",ptcom->send_times);
//	sysprintf("Process_Coil,ptcom->Current_Times %d\n",ptcom->Current_Times);

	if(ptcom->send_times==1) //说明是查询盘管，已经处理好
	{
			return;
	}
	else if(ptcom->send_times==2)  //先查新风机，再查盘管，再设置
	{
		if(ptcom->Current_Times==1) //查询盘管
		{
/***************************************
查询盘管正确以后,
					D7,设定温度
					D8,室内温度
					D9,空调口入水温度
D10	B0	盘管关	1-有效     0-无效	
	B1	盘管开	1-有效     0-无效	
	B3B2	/	/	预留
	B5B4	温度单位	01-F       10-C	
	B6	空调阀	1-开     0-关	
	B7	/	/	预留
D11	B0	制冷模式	1-有效     0-无效	
	B1	制热模式	1-有效     0-无效	
	B2	冷热自动转	0-无效     1-有效	
	B3	热量表	0-无效     1-有效	
	B4	自动模式空调模式	1-制热     0-制冷	
	B5	温度控制类型	0-普通盘管     
							1-双温盘管	 
	B6	盘管防冻强制保护	1-有效     0-无效	 
	B7	/	/	预留
D12	B0	室内温度传感器异常	1-有效     0-无效	
	B1	空调水入口温探故障	1-有效     0-无效	内盘管温探
	B2	空调水出口温探故障	1-有效     0-无效	
	B3	热量表故障	1-有效     0-无效	
	B4	盘管水温过低报警	1-有效     0-无效	 
	B5-B7	/	/	预留
D17D 16	空调阀开关时间
设置盘管参数如下:
	D7	设定温度
	D11D10 	空调阀开关时间
	D8	B0	盘管关	1-有效     0-无效	
		B1	盘管开	1-有效     0-无效	
		B2	/	/	预留
		B3	能耗清零	1-有效     0-无效	
		B5B4	温度单位	01-F       10-C	广播命令
		B6	故障复位	1-有效     0-无效	广播命令
		B7	历史故障清零	1-有效     0-无效	
	D9	B0	制冷模式	1-有效     0-无效	手动
		B1	制热模式	1-有效     0-无效	手动
		B2	冷热自动转	1-有效     0-无效	
		B3	热量表	1-有效     0-无效	
		B4 	自动模式空调模式	1-制热     0-制冷	
		B5	温度控制类型	0-普通盘管     
								1-双温盘管	
		B6	盘管防冻强制保护	1-有效     0-无效	 
		B7	/	/	预留

***************************************/
//10为第一帧发送数据的长度
				*(U8 *)(AD1 + ptcom->send_length[0]+7) = *(U8 *)(LW + 2000 + 7-7);		//工况查询D7对应设置参数D7
				*(U8 *)(AD1 + ptcom->send_length[0]+8)= *(U8 *)(LW + 2000 + 10-7);		//工况查询D10对应参数设置D8
				*(U8 *)(AD1 + ptcom->send_length[0]+9) = *(U8 *)(LW + 2000 + 11-7);		//工况查询D11对应参数设置D9
				*(U8 *)(AD1 + ptcom->send_length[0]+10)= *(U8 *)(LW + 2000 + 16-7);		//工况查询D16对应参数设置D10
				*(U8 *)(AD1 + ptcom->send_length[0]+11) = *(U8 *)(LW + 2000 + 17-7);		//工况查询D17对应参数设置D11
				if(ptcom->registerr == 'X' )		//参数位
				{
					k = addr%8;
					addr = addr/16;
				}
				new_code = (U8) addr; // 强制转换的到后两位, 地址

				switch(new_code)		//将查询地址转换成参数设置地址
				{
						case	7:
								para_code=7;
								break;
						case 10:
								para_code=8;
								break;
						case 11:
								para_code=9;
								break;
					case 16:
								para_code=10;
								break;
				}
		//		sysprintf("Process_Coil new_code %d,para_code %d,ptcom->U8_Data[0] %d,k %d\n",new_code,para_code,ptcom->U8_Data[0],k);

				if(ptcom->registerr == 'X')		//参数位
				{
			//	sysprintf("0 *(U8 *)(AD1 +10+ para_code) %x\n",*(U8 *)(AD1 +10+ para_code));
					if(new_code == 10)
					{
						if(ptcom->U8_Data[0] == 1)
						{
							*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (1 << k)|(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code)); //将输入值传给寄存器
							if(k==0) //盘管关有效，使能开无效
							{
								*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(1 << 1))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code));
							}
							else if(k==1) //盘管开有效，使能关无效
							{
								*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(1 << 0))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code));
							}
								
						}
						else if(ptcom->U8_Data[0] == 0)
						{
							*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(1 << k))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code)); //将输入值传给寄存器
						}
					}
					else if(new_code == 11)
					{
						if(ptcom->U8_Data[0] == 1)
						{
							*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (1 << k)|(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code)); //将输入值传给寄存器
							if(k==0)
							{
								*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(6 << 0))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code));
							}
							else if(k==1)
							{
								*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(5 << 0))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code));
							}
							else if(k==2)
							{
								*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(3 << 0))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code));
							}
							else
							{

							}
						}
						else if(ptcom->U8_Data[0] == 0)
						{
							*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(1 << k))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code)); //将输入值传给寄存器
						}
					}
					
		//		sysprintf("1 *(U8 *)(AD1 +10+ para_code) %x\n",*(U8 *)(AD1 +10+ para_code));

				}			
				else if(ptcom->registerr == 'x')		//参数字
				{
					if(para_code == 8)// 满足 D8温度单位 特殊处理
					{
						*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = ((ptcom->U8_Data[0]<<4)|((*(U8 *)(AD1 +ptcom->send_length[0]+ para_code))&0x0f));
					}
					else if( (para_code == 10) )		//D11D 10	空调阀开关时间
					{
						*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = ptcom->U8_Data[0]; //将输入值传给寄存器
						*(U8 *)(AD1 +ptcom->send_length[0]+1+ para_code) = ptcom->U8_Data[1]; //将输入值传给寄存器
					}
					else 
					{
						*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = ptcom->U8_Data[0]; //将输入值传给寄存器
					}

				}
				b= AddSum((U8 *)(AD1+ptcom->send_length[0]+2),ptcom->send_length[1] - 4);     //校验

				*(U8 *)(AD1+ptcom->send_length[0]+ptcom->send_length[1]-2)=b&0xff;    		//校验码
				*(U8 *)(AD1+ptcom->send_length[0]+ptcom->send_length[1]-1)=0xfd;     			//结束码
			
		}
	}
}

void Process_Coil()	//盘管的处理
{

	if(ptcom->Simens_Count != 100)
		Process_DoubleCoil();
	else 
		Process_SingleCoil();

}

void Process_Humi()	//加湿器的处理
{
		U8 new_code,para_code; //参数码, 由地址得到需要设置的参数位置
	char high,low,funnum,b;
	short i,data,data1;
	int addr,k;

	addr=ptcom->address;			      //在主程序已经转换到该段的开始地址

	if(ptcom->send_times==1) //说明是查询加湿器，已经处理好
	{
			return;
	}
	else if(ptcom->send_times==2)  //先查加湿器，再设置
	{
		if(ptcom->Current_Times==1) //查询加湿器
		{
/***************************************
查询加湿器正确以后,
D7	目标湿度	1%	100倍后16进制	24	10-100	
D8	室内湿度	1%	100倍后16进制	24	1-100	
D9	加湿阀开时间	3秒	除3后16进制	6	3-750 	
D10	加湿阀关时间	1分钟	16进制	4	1-10 	
D13	程序版本  年	1年	16进制		00-99	
D14	程序版本 月	1月	16进制		00-99	
D15	程序版本 日	1日	16进制		00-99	
数字量：
D11	
	B0	加湿器关	1—有效     0—无效	
	B1	加湿器开	1—有效     0—无效	
	B2	加湿关	1—有效     0—无效	
	B3	加湿开 	1—有效     0—无效	
	B5B4	目标湿度	10—高 01—中 00—低	
	B6	阀门类型选择	0-电磁阀  1-电动阀 	
	B7	/	/	预留
D12	
	B0	室内湿度传感器故障	1—有效     0—无效	
	B1	加湿阀故障	1—有效     0—无效	
	B2	无水报警	1—有效     0—无效	
	B3	高压泵故障	1—有效     0—无效	
	B4	水压异常故障	1—有效     0—无效	
	B5	高压端通讯故障	1—有效     0—无效 	
	B6-B7	/	/	预留

设置加湿器参数如下:
D7	目标湿度	1%	100倍后16进制	24	10-100	
D8	加湿阀开时间	3秒	除以3后16进制	5	3-720 	
D9	加湿阀关时间	1分钟	16进制	4	1-10 	
数字量：
D10	
	B0	加湿器关	1—有效     0—无效	
	B1	加湿器开	1—有效     0—无效	
	B3B2	目标湿度	10—高 01—中 00—低	
	B4	阀门类型选择	0-电磁阀  1-电动阀 	
	B5	/	/	预留
	B6	故障复位	1—有效     0—无效	广播命令
	B7	历史故障清零	1—有效     0—无效	


***************************************/
				*(U8 *)(AD1 + ptcom->send_length[0]+7) = *(U8 *)(LW + 2000 + 7-7);		//工况查询D7对应设置参数D7
				*(U8 *)(AD1 + ptcom->send_length[0]+8)= *(U8 *)(LW + 2000 + 9-7);		//工况查询D9对应参数设置D8
				*(U8 *)(AD1 + ptcom->send_length[0]+9) = *(U8 *)(LW + 2000 + 10-7);		//工况查询D10对应参数设置D9
				*(U8 *)(AD1 + ptcom->send_length[0]+10)= *(U8 *)(LW + 2000 + 11-7);		//工况查询D11对应参数设置D10
				if(ptcom->registerr == 'X' )		//参数位
				{
					k = addr%8;
					addr = addr/16;
				}
				new_code = (U8) addr; // 强制转换的到后两位, 地址

				switch(new_code)		//将查询地址转换成参数设置地址
				{
						case	7:
								para_code=7;
								break;
						case 9:
								para_code=8;
								break;
						case 10:
								para_code=9;
								break;
						case 11:
								para_code=10;
								break;
				}
				sysprintf("Process_Coil new_code %d,para_code %d,ptcom->U8_Data[0] %d,k %d\n",new_code,para_code,ptcom->U8_Data[0],k);

				if(ptcom->registerr == 'X')		//参数位
				{
				sysprintf("0 *(U8 *)(AD1 +10+ para_code) %x\n",*(U8 *)(AD1 +10+ para_code));
					if(ptcom->U8_Data[0] == 1)
					{
						if(k==6)  //查询B6对应设置B4
							*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) =  (1 << 4)|(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code)); //将输入值传给寄存器
						else
							*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (1 << k)|(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code)); //将输入值传给寄存器
						if(k==0) //加湿器关有效，使能开无效
						{
							*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(1 << 1))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code));
						}
						else if(k==1) //加湿器开有效，使能关无效
						{
							*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(1 << 0))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code));
						}
							
					}
					else if(ptcom->U8_Data[0] == 0)
					{
						if(k==6)  //查询B6对应设置B4
								*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(1 << 4))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code)); //将输入值传给寄存器
						else
							*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(1 << k))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code)); //将输入值传给寄存器
					}
		//		sysprintf("1 *(U8 *)(AD1 +10+ para_code) %x\n",*(U8 *)(AD1 +10+ para_code));

				}			
				else if(ptcom->registerr == 'x')		//参数字
				{
					sysprintf("ptcom->U8_Data[0] = %d\n", ptcom->U8_Data[0]);
					if(para_code == 10)// 满足 D10 目标湿度特殊处理
					{
						 //查询B6对应设置B4，不要冲掉电磁阀电动阀处理
						 if( (~(1 << 6))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code)))
						 	*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) =  (1 << 4)|(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code)); //将输入值传给寄存器
						 else
							*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(1 << 4))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code)); //将输入值传给寄存器
					 	
						*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = ((ptcom->U8_Data[0]<<2)|((*(U8 *)(AD1 +ptcom->send_length[0]+ para_code))&0xf3));
					}
					else if(para_code == 7)
					{
						low=  ptcom->U8_Data[0];  
						high=  ptcom->U8_Data[1];
						data=high<<8|low;
						funnum=data/100;	//
						*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) =funnum; 
					//	sysprintf("Read_Analog*******************00 high %d low %d\n",high,low);			
					//	sysprintf("Read_Analog*******************11 data %d funnum %d\n",data,funnum);
					}
					else 
					{
						*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = ptcom->U8_Data[0]; //将输入值传给寄存器
					}

				}
				b= AddSum((U8 *)(AD1+ptcom->send_length[0]+2),ptcom->send_length[1] - 4);     //校验

				*(U8 *)(AD1+ptcom->send_length[0]+ptcom->send_length[1]-2)=b&0xff;    		//校验码
				*(U8 *)(AD1+ptcom->send_length[0]+ptcom->send_length[1]-1)=0xfd;     			//结束码
			
		}
	}
}
void compxy(void)			//处理成标准存储格式
{
	int i;
	unsigned char a[100];
	int a1,a2;
	int nBitPos = 0;
	int addr;
	int b;
	U8 type_code; //类别码
	U8 num_m;    //机器数
	U8 para_code; //参数码, 由地址得到需要设置的参数位置
	U8 sw_code;   //开关(小数位)
	U8 k;
	U8 num;
	U32 addr1;
	addr=ptcom->address;			      //在主程序已经转换到该段的开始地址

	if(ptcom->registerr == 'X' || ptcom->registerr == 'L' || ptcom->registerr == 'Y' || ptcom->registerr == 'V')
	{
		type_code = ((addr>>12)&0x0f);
	}
	else
	{
		type_code = ((addr>>8)&0x0f);
	}
	
	if(ptcom->send_staradd[99] == 99)// DA_bit DG_bit DE_bit DC_bit
	{
		
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1];i++)						//ASC玛返回，所以要转为16进制
		{
			a1 = *(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2 + 1);
			a2 = *(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2 + 0);
			*(U8 *)(COMad + i*2 + 0)=a1;
			*(U8 *)(COMad + i*2 + 1)=a2;		

			*(U8 *)(LW + 2000 + 2*i + 0) = a1;
			*(U8 *)(LW + 2000 + 2*i + 0) = a2;
		}
	}
	else if(ptcom->send_staradd[99] == 1)
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1];i++)		//ASC玛返回，所以要转为16进制
		{
			a1 = *(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i);
			*(U8 *)(COMad + i) = a1; // 写入接收到的值
			*(U8 *)(LW + 2000 + i) = a1;
			//sysprintf(" LocalWord[%d] = 0x%x ",2000+i, *(U8 *)(LW+2000+i));
		}
	//	sysprintf("*(U8 *)(COMad + 21) = 0x%x\n", *(U8 *)(COMad + 21));
		
		if(ptcom->registerr == 'L' || ptcom->registerr == 'l' || ptcom->registerr == 'x' || ptcom->registerr == 'X') //工况查询 进入报警
		{
			if(ptcom->registerr == 'L' || ptcom->registerr == 'X') //只取前四位
			{
				addr1 = addr/16;
			}
			else
			{
				addr1 = addr;
			}
			setalarm(addr1);
		}
		if((type_code==2)&&((ptcom->registerr=='x')||(ptcom->registerr=='X')))	//盘管查询，,先查询新风机
		{
			Process_Coil();
			return;
		}
		else if((type_code==11)&&((ptcom->registerr=='x')||(ptcom->registerr=='X')))	//加湿器的处理
		{
			Process_Humi();
			return;
		}

		if(ptcom->send_times==2) //说明是查询后的设置
		{
			if(ptcom->registerr == 'X' || ptcom->registerr == 'L' || ptcom->registerr == 'Y' || ptcom->registerr == 'C')
			{
				k = addr%8;
				addr = addr/16;
			}
			for(i = 0;i < ptcom->send_length[1] - 9; i++)  //9+7
			{
				*(U8 *)(AD1 + 16 + i) = *(U8 *)(LW + 2000 + i);
			}	
			para_code = (U8) addr; // 强制转换的到后两位, 地址
			if(ptcom->registerr == 'X' || ptcom->registerr == 'L' || ptcom->registerr == 'Y' || ptcom->registerr == 'C')
			{
				if((para_code == 29) && (ptcom->registerr == 'X'))
				{
					if(ptcom->U8_Data[0] == 1)
					{
						*(U8 *)(AD1 +9+ para_code) = (1 << k)|(*(U8 *)(AD1 +9+ para_code)); //将输入值传给寄存器
					}
					else if(ptcom->U8_Data[0] == 0)
					{
						*(U8 *)(AD1 +9+ para_code) = (~(1 << k))&(*(U8 *)(AD1 +9+ para_code)); //将输入值传给寄存器
					}
					else
					{

					}
				}
				else if(ptcom->registerr == 'Y')
				{
					if(ptcom->U8_Data[0] == 1)
					{
						*(U8 *)(AD1 +9+ para_code) = (1 << k)|(*(U8 *)(AD1 +9+ para_code)); //将输入值传给寄存器
					}
					else if(ptcom->U8_Data[0] == 0)
					{
						*(U8 *)(AD1 +9+ para_code) = (~(1 << k))&(*(U8 *)(AD1 +9+ para_code)); //将输入值传给寄存器
					}
					else
					{

					}
				}
				else
				{
					*(U8 *)(AD1 +9+ para_code) = (ptcom->U8_Data[0] << k); //将输入值传给寄存器
				}
				//sysprintf("*(U8 *)(AD1 +9+ %d) = %d\n",para_code, *(U8 *)(AD1 +9+ para_code));
				
			}
			
			else if(ptcom->registerr == 'x')
			{
				if((type_code == 10)) // 满足对新风机的特殊处理
				{
					if(para_code == 29)// 满足 DA1A29 特殊处理
					{
						*(U8 *)(AD1 +9+ para_code) = ((ptcom->U8_Data[0]<<4)|((*(U8 *)(AD1 +9+ para_code))&0x0f));
					}
					else if((para_code == 16) || (para_code == 18) || (para_code == 24) )
					{
						*(U8 *)(AD1 +9+ para_code) = ptcom->U8_Data[0]; //将输入值传给寄存器
						*(U8 *)(AD1 +10+ para_code) = ptcom->U8_Data[1]; //将输入值传给寄存器
					}
					else
					{
						*(U8 *)(AD1 +9+ para_code) = ptcom->U8_Data[0]; //将输入值传给寄存器
					}
				}
		
				else 
				{
						*(U8 *)(AD1 +9+ para_code) = ptcom->U8_Data[0]; //将输入值传给寄存器
				}
			}
			else if(ptcom->registerr == 'c')
			{
				*(U8 *)(AD1 +9+ para_code) = 1<<ptcom->U8_Data[0];
			}
			else
			{
				*(U8 *)(AD1 +9+ para_code) = ptcom->U8_Data[0]; //将输入值传给寄存器
			}
			
			b= AddSum((U8 *)(AD1+11),ptcom->send_length[1] - 4);     //校验

			*(U8 *)(AD1+9+ptcom->send_length[1]-2)=b&0xff;    		//校验码
			*(U8 *)(AD1+9+ptcom->send_length[1]-1)=0xfd;     			//结束码
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
	aakj1=(*(U8 *)(COMad+ptcom->return_length[ptcom->Current_Times-1]-2))&0xff;		//在发送完后Current_Times++，此时要--
	aakj2=AddSum((U8 *)(COMad+2),ptcom->return_length[ptcom->Current_Times-1]-4)&0xff;
	//sysprintf("aakj1=0x%x  aakj2=0x%x\n",aakj1,aakj2);
	if(aakj1==aakj2)
		return 1;
	else
		return 0;
}
U16 AddSum(unsigned char* data,unsigned short Len)//计算校验和
{
	int i;
	short result=0;
	if(!data)
	{
	   return 0;
	}	
	for(i=0;i<Len;i++)
	{
		result=result+data[i];
	}
	return (result);
}


