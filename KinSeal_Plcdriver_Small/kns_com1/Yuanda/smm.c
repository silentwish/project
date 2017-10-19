/******************************************************************************************
 * 2015-12-04
 * Զ��ͨѶЭ��
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
		case PLC_READ_DATA:				//���������Ƕ�����
		case PLC_READ_DIGITALALARM:		//���������Ƕ�����,����	
		case PLC_READ_TREND:			//���������Ƕ�����,����ͼ
		case PLC_READ_ANALOGALARM:		//���������Ƕ�����,��ȱ���	
		case PLC_READ_CONTROL:			//���������Ƕ�PLC���Ƶ�����
			switch(ptcom->registerr)
			{
				case 'x':		//������ѯ DA
				case 'y':		//������ѯ	DB	
				case 'Y':		//������ѯ	DB_Bit
				case 'l':		//������ѯ DC
				case 'm':		//�ܺĲ�ѯ DE
				case 'c':		//ģʽ�л� DF
				case 'C':		//ģʽ�л���ѯλ
				case 'X':		//����λ��ѯ DA_bit
				case 'L':		//����λ
				//case 'M':		//�ܺ�λ
				//case 'A':		//��������
				//case 'D':		//��������
				//case 'R':		//ģʽ����
					Read_Analog();
					//Write_Analog();
					break;
				default:
					break;			
			}
			break;
		case PLC_WRITE_DATA:				//����������д����
		case PLC_WRITE_TIME:				//����������дʱ�䵽PLC
		case PLC_WRITE_RECIPE:				//����������д�䷽��PLC
			switch(ptcom->registerr)
			{
				case 'x':		//��������DA
				case 'X':		//����λ����DA_bit
				case 'D': 		//��������DH
				case 'R':		//ģʽ���� DI
				case 'y':		//����λ   DB_Bit
				case 'Y':		//����λ   DB_Bit
				case 'L':      // ����λ DC_Bit
				case 'c':		//ģʽ�л� DF
				case 'C':		//ģʽ�л���ѯλ
					Write_Analog();		
					break;
				default:
				ptcom->send_length[0]=0;				//���ͳ���
				ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
				ptcom->send_times=0;					//���ʹ���
				ptcom->return_length[0]=0;				//�������ݳ���
				ptcom->return_start[0]=0;				//����������Ч��ʼ
				ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
				ptcom->Current_Times=0;	
					break;				
			}
			break;		
		case PLC_CHECK_DATA:				//�������������ݴ���
			watchcom();
			break;
		default:
			break;					
	}	 
}

void Read_Analog()				//�ؼ��Ƕ����ԣ����Ͳ�ѯ֡
{
	int b;
	U32 addr,newaddr;
	U8 type_code,newtype; //�����
	U8 num_m;    //������
	int plcadd;
	int length;
	int t;
	int i;
	U8 sw ,sw1 ; //����
	U8 reg;  //�Ĵ�����־
	U8 arg1;
	U8 size_c; //����֡�ֽ���
	reg = ptcom->registerr; //�õ��Ĵ����±�
	addr = ptcom->address;			      //���������Ѿ�ת�����öεĿ�ʼ��ַ
	 //�Ĵ�����ͬ,����addr�Ĵ���ʽҲ��ͬ

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

	switch (ptcom->registerr)	//���ݼĴ������ͻ�ȡ��Ӧ�Ĺ�����
	{
		case 'x':		//������ѯ DA
		case 'X':      //����λ DA_Bit
			t=0x02;
			switch(type_code)
			{
				case 2: //�̹�,�Ȳ�ѯ�·������������¶Ȳ��ܲ�ѯ�̹�
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
				case 8: // ���
				case 12://��ʪ��
					ptcom->return_length[0] = 20;
					ptcom->return_length_available[0] = 10;
					break;
				case 10:// �·��
					ptcom->return_length[0] = 35;
					ptcom->return_length_available[0] = 25;
					break;
				case 11://��ʪ��
					t = 0x01;
					ptcom->return_length[0] = 19;
					ptcom->return_length_available[0] = 9;
					break;
				default:
					break;
			}
			break;

		case 'y':		//������ѯ  DB
		case 'Y':      //�������� DB_Bit
			t=0x04;
			ptcom->return_length[0] = 11;
			ptcom->return_length_available[0] = 1;
			break;
		case 'l':		//������ѯ DC
		case 'L':      //����λ DC_Bit
			t=0x01;
			switch(type_code)
			{
				case 2: //�̹�,�Ȳ�ѯ�·������������¶Ȳ��ܲ�ѯ�̹�
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
				case 8: // ���
				case 12://��ʪ��
					ptcom->return_length[0] = 20;
					ptcom->return_length_available[0] = 10;
					break;
				case 10:// �·��
					ptcom->return_length[0] = 34;
					ptcom->return_length_available[0] = 24;
					break;
				case 11://��ʪ��
					ptcom->return_length[0] = 19;
					ptcom->return_length_available[0] = 9;
					break;
				default:
					break;
			}
			break;
		case 'm':		//�ܺĲ�ѯ DE
			if(type_code == 10) //�·��
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
		case 'c':		//ģʽ�л���ѯ DF
		case 'C':     //ģʽ����DF_Bit
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
//	b=b+add;					      //����ƫ�Ƶ�ַ

	type_code = ((addr>>8)&0x0f);
	num_m = addr>>12;

	newaddr=(num_m<<12)+(type_code<<8)+7;  //Զ��Э�����⣬��ʼ��ַ����07��һ���Զ���
	if(ptcom->registerr == 'X' || ptcom->registerr == 'L' || ptcom->registerr == 'Y' || ptcom->registerr == 'C')
	{
		newaddr = newaddr<<4;
	}
	ptcom->address=newaddr;
	ptcom->register_length=ptcom->return_length_available[0];
//	sysprintf("newaddr 0x%x\n",newaddr);

	if((type_code==2)&&((ptcom->registerr=='x')||(ptcom->registerr=='X')))	//�̹�,�Ȳ�ѯ�·��
		newtype=10;
	else
		newtype=type_code;
//	



	if((type_code==2)&&((ptcom->registerr=='x')||(ptcom->registerr=='X')))	//�̹�,�Ȳ�ѯ�·��
	{
		if(ptcom->Simens_Count != 100)
		{
			*(U8 *)(AD1+0)=0xF7;		//��ʼ���λ
			*(U8 *)(AD1+1)=0xF8;		//��ʼ���λ
			*(U8 *)(AD1+2)=0x05;			//�ֽ���
			*(U8 *)(AD1+3)=newtype;       //�����
			*(U8 *)(AD1+4)=0x00;      	//��ַ���λ
			*(U8 *)(AD1+5)=num_m;     	//��ַ���λ
			*(U8 *)(AD1+6)=t;     		//������

			b= AddSum((U8 *)(AD1+2),5);  //У��

			*(U8 *)(AD1+7)=b&0xff;     //У����
			*(U8 *)(AD1+8)=0xfd;     	//������

			ptcom->send_length[0]=9;				//���ͳ���
			ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
			ptcom->send_times=1;					//���ʹ���
			
			ptcom->return_start[0]=7;				//����������Ч��ʼ,����Ӧ��֡����Ч��ʼ���Ǵ�7��ʼ
			ptcom->Current_Times=0;				//��ǰ���ʹ���	
			ptcom->send_add[0]=ptcom->address;		//�����������ַ������		

			ptcom->send_staradd[99]=1;	
		
			*(U8 *)(AD1+9)=0xF7;		//��ʼ���λ
			*(U8 *)(AD1+10)=0xF8;		//��ʼ���λ
			*(U8 *)(AD1+11)=0x06;			//�ֽ���
			*(U8 *)(AD1+12)=type_code;       //�����
			*(U8 *)(AD1+13)=0x00;      	//��ַ���λ
			*(U8 *)(AD1+14)=num_m;     	//��ַ���λ
			*(U8 *)(AD1+15)=t;     		//������
			ptcom->send_length[1]=10;				//���ͳ���
			ptcom->send_staradd[1]=9;				//�������ݴ洢��ַ	
			ptcom->send_times=2;					//���ʹ���
			
			ptcom->return_start[1]=7;				//����������Ч��ʼ,����Ӧ��֡����Ч��ʼ���Ǵ�7��ʼ
			ptcom->Current_Times=0;				//��ǰ���ʹ���	
			ptcom->send_add[1]=ptcom->address;		//�����������ַ������		

			ptcom->send_staradd[99]=1;	
			
		}
		else if(ptcom->Simens_Count == 100)
		{
			*(U8 *)(AD1+0)=0xF7;		//��ʼ���λ
			*(U8 *)(AD1+1)=0xF8;		//��ʼ���λ
			*(U8 *)(AD1+2)=0x06;			//�ֽ���
			*(U8 *)(AD1+3)=type_code;       //�����
			*(U8 *)(AD1+4)=0x00;      	//��ַ���λ
			*(U8 *)(AD1+5)=num_m;     	//��ַ���λ
			*(U8 *)(AD1+6)=t;     		//������
			*(U8 *)(AD1+7)=0xFF;      //������
			b= AddSum((U8 *)(AD1+2),6);  //У��

			*(U8 *)(AD1+8)=b&0xff;     //У����
			*(U8 *)(AD1+9)=0xfd;     	//������
		
			ptcom->send_length[0]=10;				//���ͳ���
			ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
			ptcom->send_times=1;					//���ʹ���
		
			ptcom->return_start[0]=7;				//����������Ч��ʼ,����Ӧ��֡����Ч��ʼ���Ǵ�7��ʼ
			ptcom->Current_Times=0;				//��ǰ���ʹ���	
			ptcom->send_add[0]=ptcom->address;		//�����������ַ������		

			ptcom->send_staradd[99]=1;	
		}
	}
	else
	{
		*(U8 *)(AD1+0)=0xF7;		//��ʼ���λ
		*(U8 *)(AD1+1)=0xF8;		//��ʼ���λ
		*(U8 *)(AD1+2)=0x05;			//�ֽ���
		*(U8 *)(AD1+3)=newtype;       //�����
		*(U8 *)(AD1+4)=0x00;      	//��ַ���λ
		*(U8 *)(AD1+5)=num_m;     	//��ַ���λ
		*(U8 *)(AD1+6)=t;     		//������

		b= AddSum((U8 *)(AD1+2),5);  //У��

		*(U8 *)(AD1+7)=b&0xff;     //У����
		*(U8 *)(AD1+8)=0xfd;     	//������

		ptcom->send_length[0]=9;				//���ͳ���
		ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
		ptcom->send_times=1;					//���ʹ���
			
		ptcom->return_start[0]=7;				//����������Ч��ʼ,����Ӧ��֡����Ч��ʼ���Ǵ�7��ʼ
		ptcom->Current_Times=0;				//��ǰ���ʹ���	
		ptcom->send_add[0]=ptcom->address;		//�����������ַ������		

		ptcom->send_staradd[99]=1;	
	}
}

void Write_Analog()				//дģ����
{
	int b;  
	int i;
	int length;
	int plcadd;		
	int add;
	U8 t;
	U32 addr,newaddr;
	U8 reg;
	U8 type_code,newtype; //�����
	U8 num_m;    //������
	U8 para_code; //������, �ɵ�ַ�õ���Ҫ���õĲ���λ��
	U8 sw_code;   //����(С��λ)
	U8 b1;
	U8 size_c; //����֡�ֽ���
	
	reg = ptcom->registerr; //�õ��Ĵ����±�
	addr = ptcom->address;			      //���������Ѿ�ת�����öεĿ�ʼ��ַ
	if(ptcom->registerr == 'X' || ptcom->registerr == 'L' || ptcom->registerr == 'Y' || ptcom->registerr == 'C')
	{
		type_code = (U8)((addr>>12)&0X0f);
	}
	else
	{
		type_code = (U8)((addr>>8)&0X0f);
	}
//	sysprintf("Write_Analog*******************ptcom->Simens_Count %d,ptcom->registerr %c,type_code %d \n",ptcom->Simens_Count,ptcom->registerr,type_code);
	if((ptcom->Simens_Count == 100)&&(type_code==10))  //�̹ܣ���д�·��
	{
		ptcom->Current_Times=1000;	
		return;
	}
	switch (ptcom->registerr)	//���ݼĴ������ͻ�ȡ��Ӧ�Ĺ�����
	{
		case 'x':		//������ѯ DA
		case 'X':      //����λ DA_Bit
			switch(type_code)
			{
				case 10:// �·��
					t=0x02;
					ptcom->return_length[0] = 35; // ������ѯ�ķ������ݳ���34
					ptcom->return_length_available[0] = 25; //������Ч���ݳ���
					break;
				
				case 2: //�̹�,�Ȳ�ѯ�·������������¶Ȳ��ܲ�ѯ�̹�
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
				case 8: // ���
				case 12://��ʪ��
					ptcom->return_length[0] = 20;
					ptcom->return_length_available[0] = 10;
					break;
				case 11://��ʪ��
					t = 0x01;
					ptcom->return_length[0] = 19;
					ptcom->return_length_available[0] = 9;
					break;
				default:
					break;
			}
			break;
		case 'y':		//������ѯ  DB
		case 'Y':      //�������� DB_Bit
			t=0x04;
			ptcom->return_length[0] = 11;
			ptcom->return_length_available[0] = 1;
			break;

		case 'm':		//�ܺĲ�ѯ DE
			t=0x03;
			if(type_code == 10) //�·��
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
		case 'C':		//ģʽ�л���ѯ DF
		case 'c':     // ģʽ�л�����DF_Bit
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

	if((type_code==2)&&((ptcom->registerr=='x')||(ptcom->registerr=='X')))	//�̹�,�Ȳ�ѯ�·��
		newtype=10;
	else
		newtype=type_code;
	if((type_code==2)&&((ptcom->registerr=='x')||(ptcom->registerr=='X')))	//�̹�,�Ȳ�ѯ�·��
	{
		if(ptcom->Simens_Count != 100)
		{
			*(U8 *)(AD1+0)=0xF7;		//��ʼ���λ
			*(U8 *)(AD1+1)=0xF8;		//��ʼ���λ
			*(U8 *)(AD1+2)=0x05;			//�ֽ���
			*(U8 *)(AD1+3)=newtype;       //�����
			*(U8 *)(AD1+4)=0x00;      	//��ַ���λ
			*(U8 *)(AD1+5)=num_m;     	//��ַ���λ
			*(U8 *)(AD1+6)=t;     		//������

			b= AddSum((U8 *)(AD1+2),5);  //У��

			*(U8 *)(AD1+7)=b&0xff;     //У����
			*(U8 *)(AD1+8)=0xfd;     	//������

			ptcom->send_length[0]=9;				//���ͳ���
			ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
		//ptcom->send_times=1;					//���ʹ���
			
			ptcom->return_start[0]=7;				//����������Ч��ʼ,����Ӧ��֡����Ч��ʼ���Ǵ�7��ʼ
		//ptcom->Current_Times=0;				//��ǰ���ʹ���	
		//ptcom->send_add[0]=ptcom->address;		//�����������ַ������		

			ptcom->send_staradd[99]=1;	

			*(U8 *)(AD1+9)=0xF7;		//��ʼ���λ
			*(U8 *)(AD1+10)=0xF8;		//��ʼ���λ
			*(U8 *)(AD1+11)=0x06;			//�ֽ���
			*(U8 *)(AD1+12)=type_code;       //�����
			*(U8 *)(AD1+13)=0x00;      	//��ַ���λ
			*(U8 *)(AD1+14)=num_m;     	//��ַ���λ
			*(U8 *)(AD1+15)=t;     		//������
			ptcom->send_length[1]=10;				//���ͳ���
			ptcom->send_staradd[1]=9;				//�������ݴ洢��ַ	
			ptcom->send_times=2;					//���ʹ���
			
			ptcom->return_start[1]=7;				//����������Ч��ʼ,����Ӧ��֡����Ч��ʼ���Ǵ�7��ʼ
			ptcom->Current_Times=0;				//��ǰ���ʹ���	
			ptcom->send_add[1]=ptcom->address;		//�����������ַ������		

			ptcom->send_staradd[99]=1;	
			
		}
		else if(ptcom->Simens_Count == 100)
		{
			*(U8 *)(AD1+0)=0xF7;		//��ʼ���λ
			*(U8 *)(AD1+1)=0xF8;		//��ʼ���λ
			*(U8 *)(AD1+2)=0x06;			//�ֽ���
			*(U8 *)(AD1+3)=type_code;       //�����
			*(U8 *)(AD1+4)=0x00;      	//��ַ���λ
			*(U8 *)(AD1+5)=num_m;     	//��ַ���λ
			*(U8 *)(AD1+6)=t;     		//������
			*(U8 *)(AD1+7)=0xFF;      //������
			b= AddSum((U8 *)(AD1+2),6);  //У��

			*(U8 *)(AD1+8)=b&0xff;     //У����
			*(U8 *)(AD1+9)=0xfd;     	//������
		
			ptcom->send_length[0]=10;				//���ͳ���
			ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
			ptcom->send_times=2;					//���ʹ���
		
			ptcom->return_start[0]=7;				//����������Ч��ʼ,����Ӧ��֡����Ч��ʼ���Ǵ�7��ʼ
			ptcom->Current_Times=0;				//��ǰ���ʹ���	
			ptcom->send_add[0]=ptcom->address;		//�����������ַ������		

			ptcom->send_staradd[99]=1;	
		}
	}
	else
	{
		*(U8 *)(AD1+0)=0xF7;		//��ʼ���λ
		*(U8 *)(AD1+1)=0xF8;		//��ʼ���λ
		*(U8 *)(AD1+2)=0x05;			//�ֽ���
		*(U8 *)(AD1+3)=newtype;       //�����
		*(U8 *)(AD1+4)=0x00;      	//��ַ���λ
		*(U8 *)(AD1+5)=num_m;     	//��ַ���λ
		*(U8 *)(AD1+6)=t;     		//������

		b= AddSum((U8 *)(AD1+2),5);  //У��

		*(U8 *)(AD1+7)=b&0xff;     //У����
		*(U8 *)(AD1+8)=0xfd;     	//������

		ptcom->send_length[0]=9;				//���ͳ���
		ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
		//ptcom->send_times=1;					//���ʹ���
			
		ptcom->return_start[0]=7;				//����������Ч��ʼ,����Ӧ��֡����Ч��ʼ���Ǵ�7��ʼ
		//ptcom->Current_Times=0;				//��ǰ���ʹ���	
		//ptcom->send_add[0]=ptcom->address;		//�����������ַ������		

		ptcom->send_staradd[99]=1;	
	
	}
	
	switch (ptcom->registerr)	//���ݼĴ������ͻ�ȡ��Ӧ�������
	{
		case 'x':				//���� ����  DA
		case 'X':				 //����λ DA_Bit
			t=0x81;
			type_code = ((addr>>8)&0x0f);
			num_m = addr>>12;
			para_code = (U8) addr; // ǿ��ת���ĵ�����λ, ��ַ
			switch(type_code)  
			{
				case 10: //�·�,0a
					size_c = 0x1E;
					ptcom->send_length[1] = 34;
					ptcom->return_length[1] = 35;
					ptcom->return_length_available[1] = 25;
					break;
				case 2://�̹�,02
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
				case 11://��ʪ��,0b
				case 8://���,08
					size_c = 0x09;
					ptcom->send_length[1] = 13;
					ptcom->return_length[1] = 13;
					ptcom->return_length_available[1] = 4;
					break;
				default:
					break;
			}
			break;
//		case 'D':				//������ѯ ����	
		case 'y':		//������ѯ  DB
		case 'Y':      //�������� DB_Bit
			t=0x05;
			type_code = ((addr>>8)&0x0f);
			num_m = addr>>12;
			para_code = (U8) addr; // ǿ��ת���ĵ�����λ, ��ַ
			switch(type_code)  
			{
				case 10: //�·��
					size_c = 0x07;
					ptcom->send_length[1] = 11;
					ptcom->return_length[1] = 11;
					ptcom->return_length_available[1] = 2;
					break;
				case 2://�̹�
				case 11://��ʪ��
					size_c = 0x06;
					ptcom->send_length[1] = 10;
					ptcom->return_length[1] = 10;
					ptcom->return_length_available[1] = 1;
					break;
				default:
					break;
			}
			break;
		case 'c':				//ģʽ�л� ����	DF
		case 'C':              //ģʽ�л�����λDF_Bit
			t=0x07;
			type_code = ((addr>>8)&0x0f);
			num_m = addr>>12;
			para_code = (U8) addr; // ǿ��ת���ĵ�����λ, ��ַ
			size_c = 0x06;
			ptcom->send_length[1] = 10;
			ptcom->return_length[1] = 11;
			ptcom->return_length_available[1] = 1;
			break;
		case 'L':    //����λDC_Bit
			break;
		default:
			break;		
	}
	//sysprintf("function %x \n", t);
	if((type_code==2)&&((ptcom->registerr=='x')||(ptcom->registerr=='X')))	//�̹�,�Ȳ�ѯ�·��
	{
//�����ǵ���������
		if(ptcom->Simens_Count != 100)
		{
			*(U8 *)(AD1+19)=0xF7;		//��ʼ���λ
			*(U8 *)(AD1+20)=0xF8;		//��ʼ���λ
			*(U8 *)(AD1+21)=size_c;			//�ֽ���
			*(U8 *)(AD1+22)=type_code;       //�����
			*(U8 *)(AD1+23)=0x00;      	//��ַ���λ
			*(U8 *)(AD1+24)=num_m;     	//��ַ���λ
			*(U8 *)(AD1+25)=t;     		//������

			ptcom->send_staradd[2]=19;				//�������ݴ洢��ַ	
			ptcom->send_times=3;					//���ʹ���
			
			ptcom->return_start[2]=7;				//����������Ч��ʼ
			ptcom->Current_Times=0;			
			ptcom->send_staradd[99]= 1;//�ȶ���д
		}
		else if(ptcom->Simens_Count == 100)
		{
			*(U8 *)(AD1+10)=0xF7;		//��ʼ���λ
			*(U8 *)(AD1+11)=0xF8;		//��ʼ���λ
			*(U8 *)(AD1+12)=size_c;			//�ֽ���
			*(U8 *)(AD1+13)=type_code;       //�����
			*(U8 *)(AD1+14)=0x00;      	//��ַ���λ
			*(U8 *)(AD1+15)=num_m;     	//��ַ���λ
			*(U8 *)(AD1+16)=t;     		//������

			ptcom->send_staradd[1]=10;				//�������ݴ洢��ַ	
			ptcom->send_times=2;					//���ʹ���
			
			ptcom->return_start[1]=7;				//����������Ч��ʼ
			ptcom->Current_Times=0;			
			ptcom->send_staradd[99]= 1;//�ȶ���д
		}

	}
	else
	{
//�����ǵڶ�֡���ݣ��Ȳ�ѯ��������
		*(U8 *)(AD1+9)=0xF7;		//��ʼ���λ
		*(U8 *)(AD1+10)=0xF8;		//��ʼ���λ
		*(U8 *)(AD1+11)=size_c;			//�ֽ���
		*(U8 *)(AD1+12)=type_code;       //�����
		*(U8 *)(AD1+13)=0x00;      	//��ַ���λ
		*(U8 *)(AD1+14)=num_m;     	//��ַ���λ
		*(U8 *)(AD1+15)=t;     		//������

		ptcom->send_staradd[1]=9;				//�������ݴ洢��ַ	
		ptcom->send_times=2;					//���ʹ���
		
		ptcom->return_start[1]=7;				//����������Ч��ʼ
		ptcom->Current_Times=0;			
		ptcom->send_staradd[99]= 1;//�ȶ���д
	}
	//sysprintf("ptcom->send_length[1]  %d,ptcom->send_staradd[1] %d \n",ptcom->send_length[1],ptcom->send_staradd[1] );
}

void setalarm(U32 addr)
{
	char value;
	U8 para_code;
	U8 type_code; //�����
	int k;
	
	type_code = ((addr>>8)&0x0f);
	para_code = (U8)(addr);
	//sysprintf("addr**********************%x\n", addr);
/*
	value=*((U8 *)LW + 2000 + para_code);
	value=value>>k;				//�����趨�ı����
	value=value&0x01;	
	*/
	//*(U32 *)(LB_Address+1000)=1;
	//for(k = 17; k<27 ; k++)
	//sysprintf("finall LocalWord[%d] = 0x%x\n",2000+k, *(U8 *)(LW+2000+k));
	
	switch(type_code)
	{
		case 10: //�·���Ĺ���λ  LB1000-LB1020 21��
			value=*(U8 *)(LW + 2000 + 17); // 1A18��8������λ			
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
			value=*(U8 *)(LW + 2000 + 18); // 1A19��8������λ
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
			value=*(U8 *)(LW + 2000 + 19); // 1A1A��5������λ
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
			value=*(U8 *)(LW + 2000 + 5); // 120C��5������λ
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
		case 11: //��ʪ������λLB1026-LB1031
			value=*(U8 *)(LW + 2000 + 5); // 1B0C��6������λ
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
		case 8: //��׵ĵ�һ������λ��5���ʼ�1027��������ַ��LB1032��ʼ��LB1034
			value=*(U8 *)(LW + 2000 + 3); // 180A��3������λ
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
		case 12: //��ʪ�� LB1035-LB1039
			value=*(U8 *)(LW + 2000 + 6); // 1B0C��5������λ
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

void Process_DoubleCoil()	//�·��+�̹ܵĴ���
{
		U8 new_code,para_code; //������, �ɵ�ַ�õ���Ҫ���õĲ���λ��
	char high,low,funnum,b;
	short i,data,data1;
	int addr,k;

	addr=ptcom->address;			      //���������Ѿ�ת�����öεĿ�ʼ��ַ
//	sysprintf("Process_Coil,ptcom->send_times %d\n",ptcom->send_times);
//	sysprintf("Process_Coil,ptcom->Current_Times %d\n",ptcom->Current_Times);

	if(ptcom->send_times==2) //˵���ǲ�ѯ�·���Ժ�Ĳ�ѯ�̹�
	{
/*******************
		*(U8 *)(AD1+9)=0xF7;		//��ʼ���λ
		*(U8 *)(AD1+10)=0xF8;		//��ʼ���λ
		*(U8 *)(AD1+11)=0x06;			//�ֽ���
		*(U8 *)(AD1+12)=type_code;       //�����
		*(U8 *)(AD1+13)=0x00;      	//��ַ���λ
		*(U8 *)(AD1+14)=num_m;     	//��ַ���λ
		*(U8 *)(AD1+15)=t;     		//������

*******************/
		if(ptcom->Current_Times==1) //��ѯ�·��
		{
			low= *(U8 *)(LW + 2000 + 7-7);  //D8D7 10��16���������¶�
			high= *(U8 *)(LW + 2000 +8-7);
		//	sysprintf("Read_Analog*******************00 high %d low %d\n",high,low);
			
			data=(short)(high<<8|low)-450;
			data1=data/10;	//ʵ�ʷ��͹�����,2��16����
		//	sysprintf("Read_Analog*******************00 data %d data1 %d\n",data,data1);

			funnum=data1*2;
		//	sysprintf("Read_Analog*******************11 data %d funnum %d\n",data1,funnum);
			*(U8 *)(AD1+16)=funnum;
			b= AddSum((U8 *)(AD1+11),6);     //У��

			*(U8 *)(AD1+17)=b&0xff;    		//У����
			*(U8 *)(AD1+18)=0xfd;     			//������
		}
	
	}
	else if(ptcom->send_times==3)  //�Ȳ��·�����ٲ��̹ܣ�������
	{
		if(ptcom->Current_Times==1) //��ѯ�·��
		{
/*******************
		*(U8 *)(AD1+9)=0xF7;		//��ʼ���λ
		*(U8 *)(AD1+10)=0xF8;		//��ʼ���λ
		*(U8 *)(AD1+11)=0x06;			//�ֽ���
		*(U8 *)(AD1+12)=type_code;       //�����
		*(U8 *)(AD1+13)=0x00;      	//��ַ���λ
		*(U8 *)(AD1+14)=num_m;     	//��ַ���λ
		*(U8 *)(AD1+15)=t;     		//������

*******************/
				low= *(U8 *)(LW + 2000 + 7-7);  //D8D7 10��16���������¶�
				high= *(U8 *)(LW + 2000 + 8-7);
				data=high<<8|low;
				funnum=data/5;	//ʵ�ʷ��͹�����,2��16����
				*(U8 *)(AD1+16)=funnum;
				b= AddSum((U8 *)(AD1+11),6);     //У��

				*(U8 *)(AD1+17)=b&0xff;    		//У����
				*(U8 *)(AD1+18)=0xfd;     			//������

		}
		else if(ptcom->Current_Times==2) //��ѯ�̹�
		{
/***************************************
��ѯ�̹���ȷ�Ժ�,
					D7,�趨�¶�
					D8,�����¶�
					D9,�յ�����ˮ�¶�
D10	B0	�̹ܹ�	1-��Ч     0-��Ч	
	B1	�̹ܿ�	1-��Ч     0-��Ч	
	B3B2	/	/	Ԥ��
	B5B4	�¶ȵ�λ	01-F       10-C	
	B6	�յ���	1-��     0-��	
	B7	/	/	Ԥ��
D11	B0	����ģʽ	1-��Ч     0-��Ч	
	B1	����ģʽ	1-��Ч     0-��Ч	
	B2	�����Զ�ת	0-��Ч     1-��Ч	
	B3	������	0-��Ч     1-��Ч	
	B4	�Զ�ģʽ�յ�ģʽ	1-����     0-����	
	B5	�¶ȿ�������	0-��ͨ�̹�     
							1-˫���̹�	 
	B6	�̹ܷ���ǿ�Ʊ���	1-��Ч     0-��Ч	 
	B7	/	/	Ԥ��
D12	B0	�����¶ȴ������쳣	1-��Ч     0-��Ч	
	B1	�յ�ˮ�����̽����	1-��Ч     0-��Ч	���̹���̽
	B2	�յ�ˮ������̽����	1-��Ч     0-��Ч	
	B3	���������	1-��Ч     0-��Ч	
	B4	�̹�ˮ�¹��ͱ���	1-��Ч     0-��Ч	 
	B5-B7	/	/	Ԥ��
D17D 16	�յ�������ʱ��
�����̹ܲ�������:
	D7	�趨�¶�
	D11D10 	�յ�������ʱ��
	D8	B0	�̹ܹ�	1-��Ч     0-��Ч	
		B1	�̹ܿ�	1-��Ч     0-��Ч	
		B2	/	/	Ԥ��
		B3	�ܺ�����	1-��Ч     0-��Ч	
		B5B4	�¶ȵ�λ	01-F       10-C	�㲥����
		B6	���ϸ�λ	1-��Ч     0-��Ч	�㲥����
		B7	��ʷ��������	1-��Ч     0-��Ч	
	D9	B0	����ģʽ	1-��Ч     0-��Ч	�ֶ�
		B1	����ģʽ	1-��Ч     0-��Ч	�ֶ�
		B2	�����Զ�ת	1-��Ч     0-��Ч	
		B3	������	1-��Ч     0-��Ч	
		B4 	�Զ�ģʽ�յ�ģʽ	1-����     0-����	
		B5	�¶ȿ�������	0-��ͨ�̹�     
								1-˫���̹�	
		B6	�̹ܷ���ǿ�Ʊ���	1-��Ч     0-��Ч	 
		B7	/	/	Ԥ��

***************************************/
//19Ϊǰ��֡�������ݵĳ���
				*(U8 *)(AD1 + 19+7) = *(U8 *)(LW + 2000 + 7-7);		//������ѯD7��Ӧ���ò���D7
				*(U8 *)(AD1 + 19+8)= *(U8 *)(LW + 2000 + 10-7);		//������ѯD10��Ӧ��������D8
				*(U8 *)(AD1 + 19+9) = *(U8 *)(LW + 2000 + 11-7);		//������ѯD11��Ӧ��������D9
				*(U8 *)(AD1 + 19+10)= *(U8 *)(LW + 2000 + 16-7);		//������ѯD16��Ӧ��������D10
				*(U8 *)(AD1 + 19+11) = *(U8 *)(LW + 2000 + 17-7);		//������ѯD17��Ӧ��������D11
				if(ptcom->registerr == 'X' )		//����λ
				{
					k = addr%8;
					addr = addr/16;
				}
				new_code = (U8) addr; // ǿ��ת���ĵ�����λ, ��ַ

				switch(new_code)		//����ѯ��ַת���ɲ������õ�ַ
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

				if(ptcom->registerr == 'X')		//����λ
				{
			//	sysprintf("0 *(U8 *)(AD1 +10+ para_code) %x\n",*(U8 *)(AD1 +10+ para_code));
					if(new_code == 10)
					{
						if(ptcom->U8_Data[0] == 1)
						{
							*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (1 << k)|(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code)); //������ֵ�����Ĵ���
							if(k==0) //�̹ܹ���Ч��ʹ�ܿ���Ч
							{
								*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(1 << 1))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code));
							}
							else if(k==1) //�̹ܿ���Ч��ʹ�ܹ���Ч
							{
								*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(1 << 0))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code));
							}
								
						}
						else if(ptcom->U8_Data[0] == 0)
						{
							*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(1 << k))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code)); //������ֵ�����Ĵ���
						}
					}
					else if(new_code == 11)
					{
						if(ptcom->U8_Data[0] == 1)
						{
							*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (1 << k)|(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code)); //������ֵ�����Ĵ���
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
							*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(1 << k))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code)); //������ֵ�����Ĵ���
						}
					}
		//		sysprintf("1 *(U8 *)(AD1 +10+ para_code) %x\n",*(U8 *)(AD1 +10+ para_code));

				}			
				else if(ptcom->registerr == 'x')		//������
				{
					if(para_code == 8)// ���� D8�¶ȵ�λ ���⴦��
					{
						*(U8 *)(AD1 +19+ para_code) = ((ptcom->U8_Data[0]<<4)|((*(U8 *)(AD1 +19+ para_code))&0x0f));
					}
					else if( (para_code == 10) )		//D11D 10	�յ�������ʱ��
					{
						*(U8 *)(AD1 +19+ para_code) = ptcom->U8_Data[0]; //������ֵ�����Ĵ���
						*(U8 *)(AD1 +20+ para_code) = ptcom->U8_Data[1]; //������ֵ�����Ĵ���
					}
					else 
					{
						*(U8 *)(AD1 +10+ para_code) = ptcom->U8_Data[0]; //������ֵ�����Ĵ���
					}

				}
				b= AddSum((U8 *)(AD1+21),ptcom->send_length[1] - 4);     //У��

				*(U8 *)(AD1+19+ptcom->send_length[1]-2)=b&0xff;    		//У����
				*(U8 *)(AD1+19+ptcom->send_length[1]-1)=0xfd;     			//������
			
		}
	}
}

void Process_SingleCoil()	//�̹ܵ����Ĵ���
{
		U8 new_code,para_code; //������, �ɵ�ַ�õ���Ҫ���õĲ���λ��
	char high,low,funnum,b;
	short i,data,data1;
	int addr,k;

	addr=ptcom->address;			      //���������Ѿ�ת�����öεĿ�ʼ��ַ
//	sysprintf("Process_Coil,ptcom->send_times %d\n",ptcom->send_times);
//	sysprintf("Process_Coil,ptcom->Current_Times %d\n",ptcom->Current_Times);

	if(ptcom->send_times==1) //˵���ǲ�ѯ�̹ܣ��Ѿ������
	{
			return;
	}
	else if(ptcom->send_times==2)  //�Ȳ��·�����ٲ��̹ܣ�������
	{
		if(ptcom->Current_Times==1) //��ѯ�̹�
		{
/***************************************
��ѯ�̹���ȷ�Ժ�,
					D7,�趨�¶�
					D8,�����¶�
					D9,�յ�����ˮ�¶�
D10	B0	�̹ܹ�	1-��Ч     0-��Ч	
	B1	�̹ܿ�	1-��Ч     0-��Ч	
	B3B2	/	/	Ԥ��
	B5B4	�¶ȵ�λ	01-F       10-C	
	B6	�յ���	1-��     0-��	
	B7	/	/	Ԥ��
D11	B0	����ģʽ	1-��Ч     0-��Ч	
	B1	����ģʽ	1-��Ч     0-��Ч	
	B2	�����Զ�ת	0-��Ч     1-��Ч	
	B3	������	0-��Ч     1-��Ч	
	B4	�Զ�ģʽ�յ�ģʽ	1-����     0-����	
	B5	�¶ȿ�������	0-��ͨ�̹�     
							1-˫���̹�	 
	B6	�̹ܷ���ǿ�Ʊ���	1-��Ч     0-��Ч	 
	B7	/	/	Ԥ��
D12	B0	�����¶ȴ������쳣	1-��Ч     0-��Ч	
	B1	�յ�ˮ�����̽����	1-��Ч     0-��Ч	���̹���̽
	B2	�յ�ˮ������̽����	1-��Ч     0-��Ч	
	B3	���������	1-��Ч     0-��Ч	
	B4	�̹�ˮ�¹��ͱ���	1-��Ч     0-��Ч	 
	B5-B7	/	/	Ԥ��
D17D 16	�յ�������ʱ��
�����̹ܲ�������:
	D7	�趨�¶�
	D11D10 	�յ�������ʱ��
	D8	B0	�̹ܹ�	1-��Ч     0-��Ч	
		B1	�̹ܿ�	1-��Ч     0-��Ч	
		B2	/	/	Ԥ��
		B3	�ܺ�����	1-��Ч     0-��Ч	
		B5B4	�¶ȵ�λ	01-F       10-C	�㲥����
		B6	���ϸ�λ	1-��Ч     0-��Ч	�㲥����
		B7	��ʷ��������	1-��Ч     0-��Ч	
	D9	B0	����ģʽ	1-��Ч     0-��Ч	�ֶ�
		B1	����ģʽ	1-��Ч     0-��Ч	�ֶ�
		B2	�����Զ�ת	1-��Ч     0-��Ч	
		B3	������	1-��Ч     0-��Ч	
		B4 	�Զ�ģʽ�յ�ģʽ	1-����     0-����	
		B5	�¶ȿ�������	0-��ͨ�̹�     
								1-˫���̹�	
		B6	�̹ܷ���ǿ�Ʊ���	1-��Ч     0-��Ч	 
		B7	/	/	Ԥ��

***************************************/
//10Ϊ��һ֡�������ݵĳ���
				*(U8 *)(AD1 + ptcom->send_length[0]+7) = *(U8 *)(LW + 2000 + 7-7);		//������ѯD7��Ӧ���ò���D7
				*(U8 *)(AD1 + ptcom->send_length[0]+8)= *(U8 *)(LW + 2000 + 10-7);		//������ѯD10��Ӧ��������D8
				*(U8 *)(AD1 + ptcom->send_length[0]+9) = *(U8 *)(LW + 2000 + 11-7);		//������ѯD11��Ӧ��������D9
				*(U8 *)(AD1 + ptcom->send_length[0]+10)= *(U8 *)(LW + 2000 + 16-7);		//������ѯD16��Ӧ��������D10
				*(U8 *)(AD1 + ptcom->send_length[0]+11) = *(U8 *)(LW + 2000 + 17-7);		//������ѯD17��Ӧ��������D11
				if(ptcom->registerr == 'X' )		//����λ
				{
					k = addr%8;
					addr = addr/16;
				}
				new_code = (U8) addr; // ǿ��ת���ĵ�����λ, ��ַ

				switch(new_code)		//����ѯ��ַת���ɲ������õ�ַ
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

				if(ptcom->registerr == 'X')		//����λ
				{
			//	sysprintf("0 *(U8 *)(AD1 +10+ para_code) %x\n",*(U8 *)(AD1 +10+ para_code));
					if(new_code == 10)
					{
						if(ptcom->U8_Data[0] == 1)
						{
							*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (1 << k)|(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code)); //������ֵ�����Ĵ���
							if(k==0) //�̹ܹ���Ч��ʹ�ܿ���Ч
							{
								*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(1 << 1))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code));
							}
							else if(k==1) //�̹ܿ���Ч��ʹ�ܹ���Ч
							{
								*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(1 << 0))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code));
							}
								
						}
						else if(ptcom->U8_Data[0] == 0)
						{
							*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(1 << k))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code)); //������ֵ�����Ĵ���
						}
					}
					else if(new_code == 11)
					{
						if(ptcom->U8_Data[0] == 1)
						{
							*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (1 << k)|(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code)); //������ֵ�����Ĵ���
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
							*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(1 << k))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code)); //������ֵ�����Ĵ���
						}
					}
					
		//		sysprintf("1 *(U8 *)(AD1 +10+ para_code) %x\n",*(U8 *)(AD1 +10+ para_code));

				}			
				else if(ptcom->registerr == 'x')		//������
				{
					if(para_code == 8)// ���� D8�¶ȵ�λ ���⴦��
					{
						*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = ((ptcom->U8_Data[0]<<4)|((*(U8 *)(AD1 +ptcom->send_length[0]+ para_code))&0x0f));
					}
					else if( (para_code == 10) )		//D11D 10	�յ�������ʱ��
					{
						*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = ptcom->U8_Data[0]; //������ֵ�����Ĵ���
						*(U8 *)(AD1 +ptcom->send_length[0]+1+ para_code) = ptcom->U8_Data[1]; //������ֵ�����Ĵ���
					}
					else 
					{
						*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = ptcom->U8_Data[0]; //������ֵ�����Ĵ���
					}

				}
				b= AddSum((U8 *)(AD1+ptcom->send_length[0]+2),ptcom->send_length[1] - 4);     //У��

				*(U8 *)(AD1+ptcom->send_length[0]+ptcom->send_length[1]-2)=b&0xff;    		//У����
				*(U8 *)(AD1+ptcom->send_length[0]+ptcom->send_length[1]-1)=0xfd;     			//������
			
		}
	}
}

void Process_Coil()	//�̹ܵĴ���
{

	if(ptcom->Simens_Count != 100)
		Process_DoubleCoil();
	else 
		Process_SingleCoil();

}

void Process_Humi()	//��ʪ���Ĵ���
{
		U8 new_code,para_code; //������, �ɵ�ַ�õ���Ҫ���õĲ���λ��
	char high,low,funnum,b;
	short i,data,data1;
	int addr,k;

	addr=ptcom->address;			      //���������Ѿ�ת�����öεĿ�ʼ��ַ

	if(ptcom->send_times==1) //˵���ǲ�ѯ��ʪ�����Ѿ������
	{
			return;
	}
	else if(ptcom->send_times==2)  //�Ȳ��ʪ����������
	{
		if(ptcom->Current_Times==1) //��ѯ��ʪ��
		{
/***************************************
��ѯ��ʪ����ȷ�Ժ�,
D7	Ŀ��ʪ��	1%	100����16����	24	10-100	
D8	����ʪ��	1%	100����16����	24	1-100	
D9	��ʪ����ʱ��	3��	��3��16����	6	3-750 	
D10	��ʪ����ʱ��	1����	16����	4	1-10 	
D13	����汾  ��	1��	16����		00-99	
D14	����汾 ��	1��	16����		00-99	
D15	����汾 ��	1��	16����		00-99	
��������
D11	
	B0	��ʪ����	1����Ч     0����Ч	
	B1	��ʪ����	1����Ч     0����Ч	
	B2	��ʪ��	1����Ч     0����Ч	
	B3	��ʪ�� 	1����Ч     0����Ч	
	B5B4	Ŀ��ʪ��	10���� 01���� 00����	
	B6	��������ѡ��	0-��ŷ�  1-�綯�� 	
	B7	/	/	Ԥ��
D12	
	B0	����ʪ�ȴ���������	1����Ч     0����Ч	
	B1	��ʪ������	1����Ч     0����Ч	
	B2	��ˮ����	1����Ч     0����Ч	
	B3	��ѹ�ù���	1����Ч     0����Ч	
	B4	ˮѹ�쳣����	1����Ч     0����Ч	
	B5	��ѹ��ͨѶ����	1����Ч     0����Ч 	
	B6-B7	/	/	Ԥ��

���ü�ʪ����������:
D7	Ŀ��ʪ��	1%	100����16����	24	10-100	
D8	��ʪ����ʱ��	3��	����3��16����	5	3-720 	
D9	��ʪ����ʱ��	1����	16����	4	1-10 	
��������
D10	
	B0	��ʪ����	1����Ч     0����Ч	
	B1	��ʪ����	1����Ч     0����Ч	
	B3B2	Ŀ��ʪ��	10���� 01���� 00����	
	B4	��������ѡ��	0-��ŷ�  1-�綯�� 	
	B5	/	/	Ԥ��
	B6	���ϸ�λ	1����Ч     0����Ч	�㲥����
	B7	��ʷ��������	1����Ч     0����Ч	


***************************************/
				*(U8 *)(AD1 + ptcom->send_length[0]+7) = *(U8 *)(LW + 2000 + 7-7);		//������ѯD7��Ӧ���ò���D7
				*(U8 *)(AD1 + ptcom->send_length[0]+8)= *(U8 *)(LW + 2000 + 9-7);		//������ѯD9��Ӧ��������D8
				*(U8 *)(AD1 + ptcom->send_length[0]+9) = *(U8 *)(LW + 2000 + 10-7);		//������ѯD10��Ӧ��������D9
				*(U8 *)(AD1 + ptcom->send_length[0]+10)= *(U8 *)(LW + 2000 + 11-7);		//������ѯD11��Ӧ��������D10
				if(ptcom->registerr == 'X' )		//����λ
				{
					k = addr%8;
					addr = addr/16;
				}
				new_code = (U8) addr; // ǿ��ת���ĵ�����λ, ��ַ

				switch(new_code)		//����ѯ��ַת���ɲ������õ�ַ
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

				if(ptcom->registerr == 'X')		//����λ
				{
				sysprintf("0 *(U8 *)(AD1 +10+ para_code) %x\n",*(U8 *)(AD1 +10+ para_code));
					if(ptcom->U8_Data[0] == 1)
					{
						if(k==6)  //��ѯB6��Ӧ����B4
							*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) =  (1 << 4)|(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code)); //������ֵ�����Ĵ���
						else
							*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (1 << k)|(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code)); //������ֵ�����Ĵ���
						if(k==0) //��ʪ������Ч��ʹ�ܿ���Ч
						{
							*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(1 << 1))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code));
						}
						else if(k==1) //��ʪ������Ч��ʹ�ܹ���Ч
						{
							*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(1 << 0))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code));
						}
							
					}
					else if(ptcom->U8_Data[0] == 0)
					{
						if(k==6)  //��ѯB6��Ӧ����B4
								*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(1 << 4))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code)); //������ֵ�����Ĵ���
						else
							*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(1 << k))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code)); //������ֵ�����Ĵ���
					}
		//		sysprintf("1 *(U8 *)(AD1 +10+ para_code) %x\n",*(U8 *)(AD1 +10+ para_code));

				}			
				else if(ptcom->registerr == 'x')		//������
				{
					sysprintf("ptcom->U8_Data[0] = %d\n", ptcom->U8_Data[0]);
					if(para_code == 10)// ���� D10 Ŀ��ʪ�����⴦��
					{
						 //��ѯB6��Ӧ����B4����Ҫ�����ŷ��綯������
						 if( (~(1 << 6))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code)))
						 	*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) =  (1 << 4)|(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code)); //������ֵ�����Ĵ���
						 else
							*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = (~(1 << 4))&(*(U8 *)(AD1 +ptcom->send_length[0]+ para_code)); //������ֵ�����Ĵ���
					 	
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
						*(U8 *)(AD1 +ptcom->send_length[0]+ para_code) = ptcom->U8_Data[0]; //������ֵ�����Ĵ���
					}

				}
				b= AddSum((U8 *)(AD1+ptcom->send_length[0]+2),ptcom->send_length[1] - 4);     //У��

				*(U8 *)(AD1+ptcom->send_length[0]+ptcom->send_length[1]-2)=b&0xff;    		//У����
				*(U8 *)(AD1+ptcom->send_length[0]+ptcom->send_length[1]-1)=0xfd;     			//������
			
		}
	}
}
void compxy(void)			//����ɱ�׼�洢��ʽ
{
	int i;
	unsigned char a[100];
	int a1,a2;
	int nBitPos = 0;
	int addr;
	int b;
	U8 type_code; //�����
	U8 num_m;    //������
	U8 para_code; //������, �ɵ�ַ�õ���Ҫ���õĲ���λ��
	U8 sw_code;   //����(С��λ)
	U8 k;
	U8 num;
	U32 addr1;
	addr=ptcom->address;			      //���������Ѿ�ת�����öεĿ�ʼ��ַ

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
		
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1];i++)						//ASC�귵�أ�����ҪתΪ16����
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
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1];i++)		//ASC�귵�أ�����ҪתΪ16����
		{
			a1 = *(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i);
			*(U8 *)(COMad + i) = a1; // д����յ���ֵ
			*(U8 *)(LW + 2000 + i) = a1;
			//sysprintf(" LocalWord[%d] = 0x%x ",2000+i, *(U8 *)(LW+2000+i));
		}
	//	sysprintf("*(U8 *)(COMad + 21) = 0x%x\n", *(U8 *)(COMad + 21));
		
		if(ptcom->registerr == 'L' || ptcom->registerr == 'l' || ptcom->registerr == 'x' || ptcom->registerr == 'X') //������ѯ ���뱨��
		{
			if(ptcom->registerr == 'L' || ptcom->registerr == 'X') //ֻȡǰ��λ
			{
				addr1 = addr/16;
			}
			else
			{
				addr1 = addr;
			}
			setalarm(addr1);
		}
		if((type_code==2)&&((ptcom->registerr=='x')||(ptcom->registerr=='X')))	//�̹ܲ�ѯ��,�Ȳ�ѯ�·��
		{
			Process_Coil();
			return;
		}
		else if((type_code==11)&&((ptcom->registerr=='x')||(ptcom->registerr=='X')))	//��ʪ���Ĵ���
		{
			Process_Humi();
			return;
		}

		if(ptcom->send_times==2) //˵���ǲ�ѯ�������
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
			para_code = (U8) addr; // ǿ��ת���ĵ�����λ, ��ַ
			if(ptcom->registerr == 'X' || ptcom->registerr == 'L' || ptcom->registerr == 'Y' || ptcom->registerr == 'C')
			{
				if((para_code == 29) && (ptcom->registerr == 'X'))
				{
					if(ptcom->U8_Data[0] == 1)
					{
						*(U8 *)(AD1 +9+ para_code) = (1 << k)|(*(U8 *)(AD1 +9+ para_code)); //������ֵ�����Ĵ���
					}
					else if(ptcom->U8_Data[0] == 0)
					{
						*(U8 *)(AD1 +9+ para_code) = (~(1 << k))&(*(U8 *)(AD1 +9+ para_code)); //������ֵ�����Ĵ���
					}
					else
					{

					}
				}
				else if(ptcom->registerr == 'Y')
				{
					if(ptcom->U8_Data[0] == 1)
					{
						*(U8 *)(AD1 +9+ para_code) = (1 << k)|(*(U8 *)(AD1 +9+ para_code)); //������ֵ�����Ĵ���
					}
					else if(ptcom->U8_Data[0] == 0)
					{
						*(U8 *)(AD1 +9+ para_code) = (~(1 << k))&(*(U8 *)(AD1 +9+ para_code)); //������ֵ�����Ĵ���
					}
					else
					{

					}
				}
				else
				{
					*(U8 *)(AD1 +9+ para_code) = (ptcom->U8_Data[0] << k); //������ֵ�����Ĵ���
				}
				//sysprintf("*(U8 *)(AD1 +9+ %d) = %d\n",para_code, *(U8 *)(AD1 +9+ para_code));
				
			}
			
			else if(ptcom->registerr == 'x')
			{
				if((type_code == 10)) // ������·�������⴦��
				{
					if(para_code == 29)// ���� DA1A29 ���⴦��
					{
						*(U8 *)(AD1 +9+ para_code) = ((ptcom->U8_Data[0]<<4)|((*(U8 *)(AD1 +9+ para_code))&0x0f));
					}
					else if((para_code == 16) || (para_code == 18) || (para_code == 24) )
					{
						*(U8 *)(AD1 +9+ para_code) = ptcom->U8_Data[0]; //������ֵ�����Ĵ���
						*(U8 *)(AD1 +10+ para_code) = ptcom->U8_Data[1]; //������ֵ�����Ĵ���
					}
					else
					{
						*(U8 *)(AD1 +9+ para_code) = ptcom->U8_Data[0]; //������ֵ�����Ĵ���
					}
				}
		
				else 
				{
						*(U8 *)(AD1 +9+ para_code) = ptcom->U8_Data[0]; //������ֵ�����Ĵ���
				}
			}
			else if(ptcom->registerr == 'c')
			{
				*(U8 *)(AD1 +9+ para_code) = 1<<ptcom->U8_Data[0];
			}
			else
			{
				*(U8 *)(AD1 +9+ para_code) = ptcom->U8_Data[0]; //������ֵ�����Ĵ���
			}
			
			b= AddSum((U8 *)(AD1+11),ptcom->send_length[1] - 4);     //У��

			*(U8 *)(AD1+9+ptcom->send_length[1]-2)=b&0xff;    		//У����
			*(U8 *)(AD1+9+ptcom->send_length[1]-1)=0xfd;     			//������
		}
	}
}

void watchcom(void)		//�������У��
{
	unsigned int aakj=0;
	aakj=remark();		
	if(aakj==1)			//У������ȷ
	{
		ptcom->IfResultCorrect=1;
		compxy();		//�������ݴ������
	}
	else
	{
		ptcom->IfResultCorrect=0;
	}
}

int remark()				//�����������ݼ���У�����Ƿ���ȷ
{
	unsigned int aakj1;
	unsigned int aakj2;
	aakj1=(*(U8 *)(COMad+ptcom->return_length[ptcom->Current_Times-1]-2))&0xff;		//�ڷ������Current_Times++����ʱҪ--
	aakj2=AddSum((U8 *)(COMad+2),ptcom->return_length[ptcom->Current_Times-1]-4)&0xff;
	//sysprintf("aakj1=0x%x  aakj2=0x%x\n",aakj1,aakj2);
	if(aakj1==aakj2)
		return 1;
	else
		return 0;
}
U16 AddSum(unsigned char* data,unsigned short Len)//����У���
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


