#ifndef _SMM_H
#define _SMM_H

/*********************************************
COM1结构体
registerr-----寄存器名称				plc_address---------PLC站地址
address-----寄存器地址					register_length---要读的寄存器长度
IfWriteBuffer--要数据要写入				IfWrite-----------正在写
IfRead---------正在读					R_W_Flag---------读还是写
send_length[100]---发送的长度			send_staradd[100]---发送开始地址
send_times----发送的次数				return_length[100]----返回的数据长度
return_start[100]---有效数据开始地址
return_length_available[100]--返回的有效数据长度		
com_status---通信正常异常标志
Control_ID------要读或者写的控件的ID	Circle_Control_ID---当前循环读到的第几个控件
bl_ID----------控件的第几个变量			IQ-------------接收计数
startSend------开始发送标志				Current_Times--当前发送到第几次
receiveFinish---接收完成标志			com_wait_time--发送后等待PLC相应的时间
com_times-------发送同一种指令发送了第几次	writeValue--写入的数值
IfResultCorrect---返回的数据是否正确	U8 U8_Data[64]--发送对数据的CHAR型
send_add---------每一次的读/写的数据的地址
send_data_length-发送了读写多少个D的数据
*********************************************/
typedef struct Com_struct_D{
	U8 registerr;
	U8 plc_address;
	U32 address;
	U16 register_length;
	U8 IfWriteBuffer; 
	U8 IfWrite;
	U8 IfRead; 
	U8 R_W_Flag;
	U16 send_length[100];
	U16 send_staradd[100];
	U32 send_add[20];
	U8 send_data_length[20];
	U16 send_times;
	U16 return_length[100];
	U16 return_start[100];
	U16 return_length_available[100];
	U8 com_status;
	U8 Control_ID;
	U16 Circle_Control_ID;
	U8 bl_ID;
	U16 IQ;
	U8 startSend;
	U8 Current_Times;
	U8 receiveFinish;
	U16 com_wait_time;
	U8 com_times;
	double writeValue;
	U8 IfResultCorrect;
	U8 U8_Data[64];
	U8 needShake;
	U16 Simens_Count;
}Com_struct_D;

extern U32 AD1;   
extern U32 adcom; 
extern U32 COMad; 
extern U32 PE; 

extern U32 LB_Address; 			//获得指向LB结构体的地址
extern U32 LW_Address; 			//获得指向LW结构体的地址
extern U32 RWI_Address; 			//获得指向RWI结构体的地址

extern U32 LB;
extern U32 LW;
extern U32 RWI;	

#define KINSEAL_DEBUG	1

typedef enum
{
	PLC_READ_DATA=0,
	PLC_WRITE_DATA=1,
	PLC_CHECK_DATA=2,
	PLC_WRITE_TIME=3,
	PLC_READ_TIME=4,
	PLC_WRITE_RECIPE=5,
	PLC_READ_RECIPE=6,
	PLC_READ_DIGITALALARM=10,
	PLC_READ_TREND=11,
	PLC_READ_ANALOGALARM=12,
	PLC_READ_CONTROL=13,
}PLC_CMD;


unsigned short CalcCrc(unsigned char *chData,unsigned short uNo);
unsigned short CalLRC(unsigned char *chData,unsigned short uNo);
unsigned short CalcCrcr(unsigned char *chData,unsigned short uNo);
unsigned short CalcBCC(unsigned char *chData,unsigned short uNo);
unsigned short AddSum(unsigned char* data,unsigned short Len);

void SJ_TO_PLC(int b);
void PLC_TO_SJ(int b);


void float_to_char(void);
void char_to_float(void);
void SJ_TO_PLCC(int nb,float dd);
void writeDTA(void);
void writeDT(void);
void Read_Bool_TC(void);
void Read_Analog_CV32(void);
void Read_Analog_R(void);
void Write_Analog_CV32(void);
unsigned short CalFCS(unsigned char *chData,unsigned short uNo);
void Read_Bool_continous(void);
void Read_Bool_discontinous(void);
void Read_Analog_D(void);
void Read_Analog_tc(void);
void Write_Analog_D(void);
void Write_Analog_t(void);
void Write_Analog_c(void);
void R_Write_Recipe(void);
void R_Read_Recipe(void);

void Enter_PlcDriver(void);
void Set_Reset(void);
void Read_Bool(void);
int asicc(int a);
int bsicc(int a);
void setreset(void);
void Read_Recipe(void);
void Read_Bool(void);


void Read_Analog(void);

void Write_Analog(void);

void compxy(void);
void watchcom(void);
unsigned short CalcHe(unsigned char *chData,unsigned short uNo);
unsigned short CalXor(unsigned char *chData,unsigned short uNo);

unsigned short Second_Send_CalcHe(unsigned char *chData,unsigned short uNo);
void writetc(void);
int remark(void);
void Write_Time(void);
void Read_Time(void);
void Write_Recipe(void);	
void Read_Recipe(void);
void handshake(void);
void Write_Wordbit();
void errorsend(int func,int type);
#endif
