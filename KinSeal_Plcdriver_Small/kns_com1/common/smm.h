#ifndef _SMM_H
#define _SMM_H

/*********************************************
COM1�ṹ��
registerr-----�Ĵ�������				plc_address---------PLCվ��ַ
address-----�Ĵ�����ַ					register_length---Ҫ���ļĴ�������
IfWriteBuffer--Ҫ����Ҫд��				IfWrite-----------����д
IfRead---------���ڶ�					R_W_Flag---------������д
send_length[100]---���͵ĳ���			send_staradd[100]---���Ϳ�ʼ��ַ
send_times----���͵Ĵ���				return_length[100]----���ص����ݳ���
return_start[100]---��Ч���ݿ�ʼ��ַ
return_length_available[100]--���ص���Ч���ݳ���		
com_status---ͨ�������쳣��־
Control_ID------Ҫ������д�Ŀؼ���ID	Circle_Control_ID---��ǰѭ�������ĵڼ����ؼ�
bl_ID----------�ؼ��ĵڼ�������			IQ-------------���ռ���
startSend------��ʼ���ͱ�־				Current_Times--��ǰ���͵��ڼ���
receiveFinish---������ɱ�־			com_wait_time--���ͺ�ȴ�PLC��Ӧ��ʱ��
com_times-------����ͬһ��ָ����˵ڼ���	writeValue--д�����ֵ
IfResultCorrect---���ص������Ƿ���ȷ	U8 U8_Data[64]--���Ͷ����ݵ�CHAR��
send_add---------ÿһ�εĶ�/д�����ݵĵ�ַ
send_data_length-�����˶�д���ٸ�D������
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

extern U32 LB_Address; 			//���ָ��LB�ṹ��ĵ�ַ
extern U32 LW_Address; 			//���ָ��LW�ṹ��ĵ�ַ
extern U32 RWI_Address; 			//���ָ��RWI�ṹ��ĵ�ַ

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
