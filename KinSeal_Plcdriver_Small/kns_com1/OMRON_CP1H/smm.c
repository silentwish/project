/***********************************************************************************************************************************************************************
 * OMRON
 * ˵��
 * DIO				ddd.dd	0~3189.15		λ��ӳ���ַ��Y								
 * W				ddd.dd	0~6143.15		λ��ӳ���ַ��H								
 * H				ddd.dd	0~511.15		λ��ӳ���ַ��M								
 * A				ddd.dd	0~959.15		λ��ӳ���ַ��K								
 * D				ddd		0~32767			λ��ӳ���ַ��D								
 * T				ddd		0~4096			�֣�ӳ���ַ��T	
 * C				ddd		0~4096			�֣�ӳ���ַ��C	
 * T*				ddd		0~4096			�֣�ӳ���ַ��t	
 * C*				ddd		0~4096			�֣�ӳ���ַ��c	
 * DIO_W			ddd		0~3189			�֣�ӳ���ַ��R								
 * H_W				ddd		0~511			�֣�ӳ���ַ��N								
***********************************************************************************************************************************************************************/
#include "stdio.h"
#include "def.h"
#include "smm.h"


		
struct Com_struct_D *ptcom;



/***********************************************************************************************************************************************************************
 * Function: �ײ���������ӿ�
 * Parameters: ��
 * Return: true: ��
***********************************************************************************************************************************************************************/
void Enter_PlcDriver(void)
{
	ptcom=(struct Com_struct_D *)adcom;	
	switch (ptcom->R_W_Flag)
	{
	case PLC_READ_DATA:						//���������Ƕ�����
	case PLC_READ_DIGITALALARM:					//���������Ƕ�����,����	
	case PLC_READ_TREND:					//���������Ƕ�����,����ͼ
	case PLC_READ_ANALOGALARM:					//���������Ƕ�����,��ȱ���	
	case PLC_READ_CONTROL:					//���������Ƕ�PLC���Ƶ�����	
		switch(ptcom->registerr)
		{
		case 'M':
		case 'Y':
		case 'H':
		case 'K':
		case 'T':
		case 'C':						
			Read_Bool();   		//���������Ƕ�λ����       
			break;
		case 'D':
		case 'R':
		case 't':
		case 'c':
		case 'N':	
			Read_Analog();  		//���������Ƕ�ģ������ 
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
			Set_Reset();      	//����������ǿ����λ�͸�λ
			break;
		case 'D':
		case 'R':
		case 't':
		case 'c':
		case 'N':
			Write_Analog();	  	//����������дģ������	
			break;	
		default:
			handshake();
			break;			
		}
		break;	
	case PLC_WRITE_TIME:						//����������дʱ�䵽PLC
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
	case PLC_READ_TIME:						//���������Ƕ�ȡʱ�䵽PLC
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
	case PLC_WRITE_RECIPE:						//����������д�䷽��PLC
		switch(*(U8 *)(PE+3))	//�䷽�Ĵ�������
		{
		case 'D':		
			Write_Recipe();		
			break; 
		default:
			handshake();
			break;				
		}
		break;
	case PLC_READ_RECIPE:						//���������Ǵ�PLC��ȡ�䷽
		switch(*(U8 *)(PE+3))	//�䷽�Ĵ�������
		{
		case 'D':		
			Read_Recipe();		
			break;
		default:
			handshake();
			break;				
		}
		break;	
	case 7:						//���������ǰ�PLC��״̬����Ϊ����
		handshake();
		break;									
	case PLC_CHECK_DATA:						//�������������ݴ���
		watchcom();
		break;
	default:
		handshake();
		break;				
	}	 
}

/***********************************************************************************************************************************************************************
 * Function: ���ֺ���
 * Parameters: ��
 * Return: true: ��
***********************************************************************************************************************************************************************/
void handshake()
{
	int plcadd;
	U16 aakj;
	int a1,a2;	
	plcadd=ptcom->plc_address;						//PLCվ��ַ
	
	*(U8 *)(AD1+0)=0x40;
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);  //plcվ��ַ����λ��ǰ
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);	
	*(U8 *)(AD1+3)=0x53;
	*(U8 *)(AD1+4)=0x43;
	*(U8 *)(AD1+5)=0x30;
	*(U8 *)(AD1+6)=0x31;
		
	aakj=CalFCS((U8 *)AD1,7);    					//FCSУ�飬��λ��ǰ����λ�ں�         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+7)=asicc(a1);    					//asicc����ʾ
	*(U8 *)(AD1+8)=asicc(a2);
	*(U8 *)(AD1+9)=0x2a;         					//������Ԫ2a,0d
	*(U8 *)(AD1+10)=0x0d;
		
	ptcom->send_length[0]=11;						//���ͳ���
	ptcom->send_staradd[0]=0;						//�������ݴ洢��ַ	
	ptcom->send_times=1;							//���ʹ���
		
	ptcom->return_length[0]=11;						//�������ݳ���
	ptcom->return_start[0]=0;						//����������Ч��ʼ
	ptcom->return_length_available[0]=0;			//������Ч���ݳ���	
	ptcom->Current_Times=0;							//��ǰ���ʹ���		
}

/***********************************************************************************************************************************************************************
 * Function: ��λ�͸�λ
 * Parameters: ��
 * Return: true: ��
***********************************************************************************************************************************************************************/
void Set_Reset()                
{
	U16 aakj;
	int b,b1,b2,b3;
	int a1,a2,a3,a4;
	int plcadd;
	int t;	

	b=ptcom->address;									// ��ʼ��λ��ַ
	plcadd=ptcom->plc_address;							//PLCվ��ַ
	
	switch (ptcom->registerr)							//���ݲ�ͬ�ļĴ���������ʼ��ַ
	{
	case 'Y':
	case 'M':
	case 'H':
	case 'K':
		t=b/16;                						//DIO WR AR HR �ĵ�ַ��ʽ����xxxx.xx����ʼ��ַ��xxxxxx���ֳ�100ȡ��������		
		a1=(t>>12)&0xf;         						//ȡ�������ֵ�ǧλ��
		a2=(t>>8)&0xf;          						//ȡ�������ֵİ�λ��
		a3=(t>>4)&0xf;          						//ȡ�������ֵ�ʮλ��
		a4=t&0xf;               						//ȡ�������ֵĸ�λ��
		
		b1=b%16;             						//ȡС�����֣�С������00-15
		b2=(b1>>4)&0xf;        						//ȡС�����ֵ�ʮ��λ
		b3=(b1)&0xf;          	 						//ȡС�����ֵİٷ�λ
		break;
	case 'T':	
	case 'C':
		a1=(b/1000)&0xf;                      			//ȡ�������ֵ�ǧλ��
		a2=((b-a1*1000)/100)&0xf;             			//ȡ�������ֵİ�λ��
		a3=((b-a1*1000-a2*100)/10)&0xf;       			//ȡ�������ֵ�ʮλ��
		a4=((b-a1*1000-a2*100-a3*10)/1)&0xf;  			//ȡ�������ֵĸ�λ��
		break;		
	}
	
	switch (ptcom->registerr)							//���ݲ�ͬ�ļĴ��������Ͳ�ͬ�Ĵ���
	{
	case 'Y':                   						//DIO WR HR AR����FINS ��ʽ���ʹ���
	case 'M':
	case 'H':
	case 'K':
		*(U8 *)(AD1+0)=0x40;                        	//OMRON��ʼ��Ԫ"@",��40
		*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);	//plcվ��ַ����λ��ǰ
		*(U8 *)(AD1+2)=asicc(plcadd&0xf);
		
		*(U8 *)(AD1+3)=0x46;                        	//header code FA����46 41                    
		*(U8 *)(AD1+4)=0x41;  
		
		*(U8 *)(AD1+5)=0x30;                          	//response wait time
		         
		*(U8 *)(AD1+6)=0x30;                          	//ICF �̶�30 30    	
		*(U8 *)(AD1+7)=0x30;    	
		*(U8 *)(AD1+8)=0x30;                          	//DA2 �̶�30 30          
		*(U8 *)(AD1+9)=0x30;    	
		*(U8 *)(AD1+10)=0x30;                         	//SA2 �̶�30 30   	
		*(U8 *)(AD1+11)=0x30;         
		*(U8 *)(AD1+12)=0x30;                         	//SID �̶�30 30     	
		*(U8 *)(AD1+13)=0x30;
		 	
		*(U8 *)(AD1+14)=0x30;                         	//FINS command code �̶�30 31 30 32  		
		*(U8 *)(AD1+15)=0x31; 	
		*(U8 *)(AD1+16)=0x30;         
		*(U8 *)(AD1+17)=0x32;	
		
		switch (ptcom->registerr)	                  	//���ݼĴ����Ĳ�ͬ���в�ͬ�Ĳ�����
		{
		case 'Y': 
			*(U8 *)(AD1+18)=0x33;                     	//DIO��	
			*(U8 *)(AD1+19)=0x30;
			break;
		case 'H': 
			*(U8 *)(AD1+18)=0x33;                     	//WR��	
			*(U8 *)(AD1+19)=0x31;
			break;
		case 'M': 
			*(U8 *)(AD1+18)=0x33;                     	//HR��	
			*(U8 *)(AD1+19)=0x32;
			break;
		case 'K':
			*(U8 *)(AD1+18)=0x33;                     	//AR�� 	
			*(U8 *)(AD1+19)=0x33;
			break;
		}	
		
		*(U8 *)(AD1+20)=asicc(a1);                    	//��ʼ��ַ�������֣��ɸߵ���
		*(U8 *)(AD1+21)=asicc(a2);
		*(U8 *)(AD1+22)=asicc(a3);
		*(U8 *)(AD1+23)=asicc(a4);	
		
		*(U8 *)(AD1+24)=asicc(b2);                    	//��ʼ��ַ��������                    
		*(U8 *)(AD1+25)=asicc(b3);	
		
		*(U8 *)(AD1+26)=0x30;                         	//FINS response code �̶�30 30 30 31         
		*(U8 *)(AD1+27)=0x30;    	
		*(U8 *)(AD1+28)=0x30; 	
		*(U8 *)(AD1+29)=0x31;
		
		if (ptcom->writeValue==1)	                  	//��λ����
		{
			*(U8 *)(AD1+30)=0x30;                     	//��1
			*(U8 *)(AD1+31)=0x31;
		}
		
		if (ptcom->writeValue==0)	                  	//��λ����
		{
			*(U8 *)(AD1+30)=0x30;                     	//��0
			*(U8 *)(AD1+31)=0x30;
		}	
		
		aakj=CalFCS((U8 *)AD1,32);                    	//FCSУ�飬��λ��ǰ����λ�ں�         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+32)=asicc(a1);                    	//asicc����ʾ
		*(U8 *)(AD1+33)=asicc(a2);
		*(U8 *)(AD1+34)=0x2a;                         	//������Ԫ2a,0d
		*(U8 *)(AD1+35)=0x0d;	
		
		ptcom->send_length[0]=36;				      	//���ͳ���
		ptcom->send_staradd[0]=0;				      	//�������ݴ洢��ַ	
		ptcom->send_times=1;					      	//���ʹ���
			
		ptcom->return_length[0]=27;                   	//�������ݳ���
		ptcom->return_start[0]=0;				      	//����������Ч��ʼ
		ptcom->return_length_available[0]=0 ;	      	//������Ч���ݳ���	
		ptcom->Current_Times=0;					      	//��ǰ���ʹ���
		
		ptcom->Simens_Count=0;					      	//λ���أ���ͬ�ķ����в�ͬ�����ݴ���ʽ
		break;
			
	case 'T':                                         	//T��C�Ƚ����⣬��HOST LINKS��ʽ���ʹ��� 	
	case 'C':
		*(U8 *)(AD1+0)=0x40;                          	//OMRON��ʼ��Ԫ"@",��40
		*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf); 	//plcվ��ַ����λ��ǰ
		*(U8 *)(AD1+2)=asicc(plcadd&0xf);
		
		if (ptcom->writeValue==1)	                  	//��λ����
		{
			*(U8 *)(AD1+3)=0x4b;                      	//������KS��ǿ����λ��
			*(U8 *)(AD1+4)=0x53;
		}
		
		if (ptcom->writeValue==0)	                  	//��λ����
		{
			*(U8 *)(AD1+3)=0x4b;                      	//������KR��ǿ�ø�λ��
			*(U8 *)(AD1+4)=0x52;
		}
		
		switch (ptcom->registerr)	                  	//���ݼĴ����Ĳ�ͬ���в�ͬ�Ĳ�����
		{
		case 'T':
			*(U8 *)(AD1+5)=0x54;                      	//��T����������ΪT I M �ո�
			*(U8 *)(AD1+6)=0x49;
			*(U8 *)(AD1+7)=0x4d;
			*(U8 *)(AD1+8)=0x20;
			break;
		case 'C':
			*(U8 *)(AD1+5)=0x43;                      	//��C����������ΪC N T �ո�
			*(U8 *)(AD1+6)=0x4e;
			*(U8 *)(AD1+7)=0x54;
			*(U8 *)(AD1+8)=0x20;
			break;
		}
		
		*(U8 *)(AD1+9)=asicc(a1);                     	//��ʼ��ַ�����δӸߵ��ͣ�Ҫת��asicc��
		*(U8 *)(AD1+10)=asicc(a2);
		*(U8 *)(AD1+11)=asicc(a3);
		*(U8 *)(AD1+12)=asicc(a4);
		*(U8 *)(AD1+13)=0x30;                         	//С��λ���ɸߵ��� 
		*(U8 *)(AD1+14)=0x30;
		
		aakj=CalFCS((U8 *)AD1,15);                    	//FCSУ�飬��λ��ǰ����λ�ں�         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+15)=asicc(a1);                    	//asicc����ʾ
		*(U8 *)(AD1+16)=asicc(a2);
		*(U8 *)(AD1+17)=0x2a;                         	//������Ԫ2a,0d
		*(U8 *)(AD1+18)=0x0d;
		
		ptcom->send_length[0]=19;				      	//���ͳ���
		ptcom->send_staradd[0]=0;				      	//�������ݴ洢��ַ	
		ptcom->send_times=1;					      	//���ʹ���
			
		ptcom->return_length[0]=11;				      	//�������ݳ���
		ptcom->return_start[0]=0;				      	//����������Ч��ʼ
		ptcom->return_length_available[0]=0;	      	//������Ч���ݳ���	
		ptcom->Current_Times=0;					      	//��ǰ���ʹ���
		
		ptcom->Simens_Count=0;					      	//λ���أ���ͬ�ķ����в�ͬ�����ݴ���ʽ
		break;
	}			
}

/***********************************************************************************************************************************************************************
 * Function: ��ȡ����������Ϣ
 * Parameters: ��
 * Return: true: ��
***********************************************************************************************************************************************************************/
void Read_Bool()				                          
{
	U16 aakj;
	int b,t,c;
	int a1,a2,a3,a4;
	int plcadd;	
	int c1,c2,c3,c4;	
		
	b=ptcom->address;			                      	//��ʼ��ַ
	plcadd=ptcom->plc_address;	                      	//PLCվ��ַ

	
	switch (ptcom->registerr)	                      	//���ݲ�ͬ�ļĴ���������ʼ��ַ
	{
	case 'Y':
	case 'M':
	case 'K':
	case 'H':				
		//b=(b+8)/100;				                  	//�����ٿ�£
		//t=b;
		//ptcom->address=t*100;                         	//��ַ��д
		b=ptcom->address/16;
		if(ptcom->R_W_Flag)
		{
			ptcom->address=b*16;                         	//��ַ��д
		}
		
		a1=(b>>12)&0xf;                               	//ȡ�������ֵ�ǧλ��
		a2=(b>>8)&0xf;                                	//ȡ�������ֵİ�λ��
		a3=(b>>4)&0xf;                                	//ȡ�������ֵ�ʮλ��
		a4=b&0xf;                                     	//ȡ�������ֵĸ�λ��
		break;
	case 'T':
		a1=(b>>12)&0xf;                               	//ȡ�������ֵ�ǧλ��
		a2=(b>>8)&0xf;                                	//ȡ�������ֵİ�λ��
		a3=(b>>4)&0xf;                                	//ȡ�������ֵ�ʮλ��
		a4=b&0xf;                                     	//ȡ�������ֵĸ�λ��
		break;	
	case 'C':
		b=b+0x8000;
		a1=(b>>12)&0xf;                               	//ȡ�������ֵ�ǧλ��
		a2=(b>>8)&0xf;                                	//ȡ�������ֵİ�λ��
		a3=(b>>4)&0xf;                                	//ȡ�������ֵ�ʮλ��
		a4=b&0xf;                                     	//ȡ�������ֵĸ�λ��
		break;		
	}

	*(U8 *)(AD1+0)=0x40;                              	//OMRON��ʼ��Ԫ"@",��40
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);     	//plcվ��ַ����λ��ǰ
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);
	
	*(U8 *)(AD1+3)=0x46;                          		//header code FA����46 41                    
	*(U8 *)(AD1+4)=0x41;  
		
	*(U8 *)(AD1+5)=0x30;                          		//response wait time
		         
	*(U8 *)(AD1+6)=0x30;                          		//ICF �̶�30 30    	
	*(U8 *)(AD1+7)=0x30;    	
	*(U8 *)(AD1+8)=0x30;                          		//DA2 �̶�30 30          
	*(U8 *)(AD1+9)=0x30;    	
	*(U8 *)(AD1+10)=0x30;                         		//SA2 �̶�30 30   	
	*(U8 *)(AD1+11)=0x30;         
	*(U8 *)(AD1+12)=0x30;                         		//SID �̶�30 30     	
	*(U8 *)(AD1+13)=0x30;
		 	
	*(U8 *)(AD1+14)=0x30;                         		//FINS command code �̶�30 31 30 31  			
	*(U8 *)(AD1+15)=0x31; 	
	*(U8 *)(AD1+16)=0x30;         
	*(U8 *)(AD1+17)=0x31; 
	   
	switch (ptcom->registerr)	                  		//���ݼĴ����Ĳ�ͬ���в�ͬ�Ĳ�����
	{	
	case 'Y':
		*(U8 *)(AD1+18)=0x42;                     		//DIO��	
		*(U8 *)(AD1+19)=0x30;
		break;
	case 'H':
		*(U8 *)(AD1+18)=0x42;                     		//WR��	 	 	
		*(U8 *)(AD1+19)=0x31;
		break;
	case 'M': 	
		*(U8 *)(AD1+18)=0x42;                     		//HR��	 	
		*(U8 *)(AD1+19)=0x32;
		break;
	case 'K': 	
		*(U8 *)(AD1+18)=0x42;                     		//AR��	 	
		*(U8 *)(AD1+19)=0x33;
		break;
	case 'T': 	
	case 'C':
		*(U8 *)(AD1+18)=0x30;                     		//T/C��	 	 	
		*(U8 *)(AD1+19)=0x39;
		break;
	}
	
	switch (ptcom->registerr)	                  		//���ݼĴ����Ĳ�ͬ�����Ͳ�ͬ�Ĵ���
	{
	case 'Y':
	case 'H':
	case 'M':
	case 'K':
		*(U8 *)(AD1+20)=asicc(a1);                		//��ʼ��ַ
		*(U8 *)(AD1+21)=asicc(a2);
		*(U8 *)(AD1+22)=asicc(a3);
		*(U8 *)(AD1+23)=asicc(a4);	
		
		*(U8 *)(AD1+24)=0x30;                     		//ÿ�ζ�16λ 	
		*(U8 *)(AD1+25)=0x30;
		
		c=ptcom->register_length;                 		//�����ٸ�Ԫ��
		c=(c/2)+(c%2);                            		//���1���ֽ�
		//c1=(c/1000)&0xf;                         	 	//ȡǧλ��
		//c2=((c-c1*1000)/100)&0xf;                 		//ȡ��λ��
		//c3=((c-c1*1000-c2*100)/10)&0xf;           		//ȡʮλ��
		//c4=(c-c1*1000-c2*100-c3*10)&0xf;          		//ȡ��λ�� 
		c1=(c>>12)&0xf;                           		//ȡǧλ��
		c2=(c>>8)&0xf;                            		//ȡ��λ��
		c3=(c>>4)&0xf;                            		//ȡʮλ��
		c4=c&0xf;
		*(U8 *)(AD1+26)=asicc(c1);	              		//����Ԫ����������asicc�뷢�ͣ����δӸߵ���
		*(U8 *)(AD1+27)=asicc(c2);
		*(U8 *)(AD1+28)=asicc(c3);
		*(U8 *)(AD1+29)=asicc(c4);
		
		aakj=CalFCS((U8 *)AD1,30);                		//FCSУ�飬��λ��ǰ����λ�ں�         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+30)=asicc(a1);                		//asicc����ʾ
		*(U8 *)(AD1+31)=asicc(a2);
		*(U8 *)(AD1+32)=0x2a;                     		//������Ԫ2a,0d
		*(U8 *)(AD1+33)=0x0d;		
		
		ptcom->send_length[0]=34;				  		//���ͳ���
		ptcom->send_staradd[0]=0;				  		//�������ݴ洢��ַ	
		ptcom->send_times=1;					  		//���ʹ���
			
		ptcom->return_length[0]=27+c*4;           		//�������ݳ��ȣ���27���̶�
		ptcom->return_start[0]=23;				  		//����������Ч��ʼ
		ptcom->return_length_available[0]=c*4 ;	  		//������Ч���ݳ���	
		ptcom->Current_Times=0;					  		//��ǰ���ʹ���	
		ptcom->send_add[0]=ptcom->address;		  		//�����������ַ������
   	
		ptcom->register_length=c*2;				  		//����16λ����1���֣�CΪ�ֽڣ���*2
		ptcom->Simens_Count=1;					  		//λ���أ���ͬ�ķ����в�ͬ�����ݴ���ʽ	
		break;
	case 'T':
	case 'C':
		*(U8 *)(AD1+20)=asicc(a1);
		*(U8 *)(AD1+21)=asicc(a2);
		*(U8 *)(AD1+22)=asicc(a3);
		*(U8 *)(AD1+23)=asicc(a4);	
		
		*(U8 *)(AD1+24)=0x30; 	
		*(U8 *)(AD1+25)=0x30;
		
		c=ptcom->register_length;                 		//�����ٸ�Ԫ��
		
		c=c*8;                                    		//T/CҪ�󷢵���λ��ϵͳ���������ֽڣ�*8
		
		c1=(c>>12)&0xf;                           		//ȡǧλ��
		c2=(c>>8)&0xf;                            		//ȡ��λ��
		c3=(c>>4)&0xf;                            		//ȡʮλ��
		c4=c&0xf;                                 		//ȡ��λ�� 
		*(U8 *)(AD1+26)=asicc(c1);	              		//����Ԫ����������asicc�뷢�ͣ����δӸߵ���
		*(U8 *)(AD1+27)=asicc(c2);
		*(U8 *)(AD1+28)=asicc(c3);
		*(U8 *)(AD1+29)=asicc(c4);
		
		aakj=CalFCS((U8 *)AD1,30);                		//FCSУ�飬��λ��ǰ����λ�ں�         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+30)=asicc(a1);                		//asicc����ʾ
		*(U8 *)(AD1+31)=asicc(a2);
		*(U8 *)(AD1+32)=0x2a;                     		//������Ԫ2a,0d
		*(U8 *)(AD1+33)=0x0d;		
		
		ptcom->send_length[0]=34;				  		//���ͳ���
		ptcom->send_staradd[0]=0;				  		//�������ݴ洢��ַ	
		ptcom->send_times=1;					  		//���ʹ���
			 
		ptcom->return_length[0]=27+c*2;           		//�������ݳ��ȣ���27���̶�
		ptcom->return_start[0]=23;				  		//����������Ч��ʼ
		ptcom->return_length_available[0]=c*2 ;	  		//������Ч���ݳ���	
		ptcom->Current_Times=0;					  		//��ǰ���ʹ���	
		ptcom->send_add[0]=ptcom->address;		  		//�����������ַ������  	
		ptcom->Simens_Count=2;					  		//λ���أ���ͬ�ķ����в�ͬ�����ݴ���ʽ
		break;
	}
}

/***********************************************************************************************************************************************************************
 * Function: ��ģ����
 * Parameters: ��
 * Return: true: ��
***********************************************************************************************************************************************************************/
void Read_Analog()				                  
{
	U16 aakj;
	int b,c;
	int a1,a2,a3,a4;
	int c1,c2,c3,c4;
	int plcadd;
	
	plcadd=ptcom->plc_address;	                  //PLCվ��ַ
	b=ptcom->address;			                  //��ʼ��ַ
	
	switch (ptcom->registerr)	                  //���ݲ�ͬ�ļĴ���������ʼ��ַ
	{	
	case 'D':
	case 'R':
	case 't':
	case 'N':
		b=ptcom->address;			              //��ʼ��ַ
		break;
	case 'c':
		b=b+0x8000;
		break;
	}

	a1=(b>>12)&0xf;                               //ȡǧλ��
	a2=(b>>8)&0xf;                                //ȡ��λ��
	a3=(b>>4)&0xf;                                //ȡʮλ��
	a4=b&0xf;                                     //ȡ��λ��	

	*(U8 *)(AD1+0)=0x40;                          //OMRON��ʼ��Ԫ"@",��40
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf); //plcվ��ַ����λ��ǰ
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
	
	switch (ptcom->registerr)	                   //���ݼĴ����Ĳ�ͬ���в�ͬ�Ĳ�����
	{	
	case 'D':                                      //D�� 
		*(U8 *)(AD1+18)=0x38; 	
		*(U8 *)(AD1+19)=0x32;
		break;
	case 'R':                                      //CIO�� 
		*(U8 *)(AD1+18)=0x42; 	
		*(U8 *)(AD1+19)=0x30;
		break;
	case 'N':                                      //H�� 
		*(U8 *)(AD1+18)=0x42; 	
		*(U8 *)(AD1+19)=0x32;
		break;
	case 't':                                      //T/C��
	case 'c': 	
		*(U8 *)(AD1+18)=0x38; 	
		*(U8 *)(AD1+19)=0x39;
		break;
	}
	
	*(U8 *)(AD1+20)=asicc(a1);            	       //��ʼ��ַ
	*(U8 *)(AD1+21)=asicc(a2);
	*(U8 *)(AD1+22)=asicc(a3);
	*(U8 *)(AD1+23)=asicc(a4);	
		
	*(U8 *)(AD1+24)=0x30;                    	   //ģ����û��С��λ���ʶ�Ϊ30 30 	
	*(U8 *)(AD1+25)=0x30;
	 
	c=ptcom->register_length;                      //�����ٸ�Ԫ��
	
	c1=(c>>12)&0xf;                                //ȡǧλ��
	c2=(c>>8)&0xf;                                 //ȡ��λ��
	c3=(c>>4)&0xf;                                 //ȡʮλ��
	c4=c&0xf;                                      //ȡ��λ��

	*(U8 *)(AD1+26)=asicc(c1);	                   //����Ԫ����������asicc�뷢�ͣ����δӸߵ���
	*(U8 *)(AD1+27)=asicc(c2);
	*(U8 *)(AD1+28)=asicc(c3);
	*(U8 *)(AD1+29)=asicc(c4);

	aakj=CalFCS((U8 *)AD1,30);                     //FCSУ�飬��λ��ǰ����λ�ں�         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+30)=asicc(a1);                     //asicc����ʾ
	*(U8 *)(AD1+31)=asicc(a2);
	*(U8 *)(AD1+32)=0x2a;                          //������Ԫ2a,0d
	*(U8 *)(AD1+33)=0x0d;	
	 
	ptcom->send_length[0]=34;				       //���ͳ���
	ptcom->send_staradd[0]=0;				       //�������ݴ洢��ַ	
	ptcom->send_times=1;						   //���ʹ���
		
	ptcom->return_length[0]=27+c*4;                //�������ݳ��ȣ���27���̶�
	ptcom->return_start[0]=23;				       //����������Ч��ʼ
	ptcom->return_length_available[0]=c*4;	       //������Ч���ݳ���	
	ptcom->Current_Times=0;					       //��ǰ���ʹ���	
	ptcom->send_add[0]=ptcom->address;		       //�����������ַ������
		
	ptcom->Simens_Count=5;                         //�ַ��أ����ݲ�ͬ��ֵ��ѡ��ͬ�����ݴ���ʽ
}

/***********************************************************************************************************************************************************************
 * Function: ��ȡ�䷽
 * Parameters: ��
 * Return: true: ��
***********************************************************************************************************************************************************************/
void Read_Recipe()							       
{
	U16 aakj;
	int b;
	int c,c1,c2,c3,c4;
	int a1,a2,a3,a4;
	int i;
	int datalength;                                //���ݳ���
	int p_start;                                   //���ݿ�ʼ��ַ
	int ps;
	int SendTimes;                                 //���ʹ���
	int LastTimeWord;						       //���һ�η��ͳ���
	int currentlength;
	int plcadd;                                    //PLCվ��ַ
	
	datalength=ptcom->register_length;		       //�����ܳ���
	p_start=ptcom->address;					       //��ʼ��ַ
	plcadd=ptcom->plc_address;	                   //PLCվ��ַ
	
	if(datalength>5000)                            //���Ƴ���
		datalength=5000;

	if(datalength%28==0)                           //����ܷ�28��D�����ݸպ���28D�ı���
	{
		SendTimes=datalength/28;                   //���ʹ���
		LastTimeWord=28;                           //���һ�η��͵ĳ���Ϊ28D	
	}
	if(datalength%28!=0)                           //����ܷ�28��D�����ݲ���28D�ı��� 
	{
		SendTimes=datalength/28+1;                 //���͵Ĵ���
		LastTimeWord=datalength%28;                //���һ�η��͵ĳ���Ϊ��28������	
	}
	
	for (i=0;i<SendTimes;i++)
	{
		ps=i*34;                                   //ÿ�η�34������
		b=p_start+i*28;                            //��ʼ��ַ
		a1=(b>>12)&0xf;                            //ȡǧλ��
		a2=(b>>8)&0xf;                             //ȡ��λ��
		a3=(b>>4)&0xf;                             //ȡʮλ��
		a4=b&0xf;                                  //ȡ��λ��	

		*(U8 *)(AD1+0+ps)=0x40;                    //OMRON��ʼ��Ԫ"@",��40
		*(U8 *)(AD1+1+ps)=asicc(((plcadd&0xf0)>>4)&0xf); //plcվ��ַ����λ��ǰ
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
	
		*(U8 *)(AD1+18+ps)=0x38;                    //������	
		*(U8 *)(AD1+19+ps)=0x32;
		
		*(U8 *)(AD1+20+ps)=asicc(a1);               //��ʼ��ַ���ɸߵ��ͷ���                
		*(U8 *)(AD1+21+ps)=asicc(a2);
		*(U8 *)(AD1+22+ps)=asicc(a3);
		*(U8 *)(AD1+23+ps)=asicc(a4);	
		
		*(U8 *)(AD1+24+ps)=0x30;                    //ģ����û��С��λ���̶�30 30 	
		*(U8 *)(AD1+25+ps)=0x30;
		
		if (i!=(SendTimes-1))	                    //�������һ�η���ʱ
		{
			*(U8 *)(AD1+26+ps)=0x30;                //�̶�����28��D����56�ֽڣ���λ��asicc����ʾ
			*(U8 *)(AD1+27+ps)=0x30;
			*(U8 *)(AD1+28+ps)=0x31;                //�̶�����28��D����56�ֽڣ���λ��asicc����ʾ
			*(U8 *)(AD1+29+ps)=0x43;
			currentlength=28;                       //�̶�����28��D
		}
		if (i==(SendTimes-1))	                    //���һ�η���ʱ
		{
			c=LastTimeWord;                         //�����ٸ�Ԫ��
			c1=(c>>12)&0xf;                         //ȡǧλ��
			c2=(c>>8)&0xf;                          //ȡ��λ��
			c3=(c>>4)&0xf;                          //ȡʮλ��
			c4=c&0xf;                               //ȡ��λ��
			*(U8 *)(AD1+26+ps)=asicc(c1);           //���͵ĳ��ȣ����δӸߵ��ͣ�Ҫת��asicc��
			*(U8 *)(AD1+27+ps)=asicc(c2);
			*(U8 *)(AD1+28+ps)=asicc(c3);
			*(U8 *)(AD1+29+ps)=asicc(c4);
			currentlength=LastTimeWord;             //ʣ��LastTimeWord��D
		}

		aakj=CalFCS((U8 *)(AD1+ps),30);             //FCSУ�飬��λ��ǰ����λ�ں�         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+30+ps)=asicc(a1);               //asicc����ʾ
		*(U8 *)(AD1+31+ps)=asicc(a2);
		*(U8 *)(AD1+32+ps)=0x2a;                    //������Ԫ2a,0d
		*(U8 *)(AD1+33+ps)=0x0d;
		
		ptcom->send_length[i]=34;				    //���ͳ���
		ptcom->send_staradd[i]=i*34;			    //�������ݴ洢��ַ	
		ptcom->send_add[i]=p_start+i*28;		    //�����������ַ������	
		ptcom->send_data_length[i]=currentlength;	//�������һ�ζ���28��D
				
		ptcom->return_length[i]=27+currentlength*4; //�������ݳ��ȣ���27���̶�
		ptcom->return_start[i]=23;				    //����������Ч��ʼ
		ptcom->return_length_available[i]=currentlength*4;	//������Ч���ݳ���	
		
	}
	ptcom->send_times=SendTimes;					//���ʹ���
	ptcom->Current_Times=0;		
	ptcom->Simens_Count=5;	                        //�ַ��أ����ݲ�ͬ��ֵ��ѡ��ͬ�����ݴ���ʽ
}

/***********************************************************************************************************************************************************************
 * Function: дģ����
 * Parameters: ��
 * Return: true: ��
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
	
	plcadd=ptcom->plc_address;	                    //PLCվ��ַ
	
	switch (ptcom->registerr)	                    //���ݲ�ͬ�ļĴ���������ʼ��ַ
	{	
	case 'D':
	case 'R':
	case 't':	
	case 'N':				
		b=ptcom->address;			                //��ʼ��ַ
		break;
	case 'c':
		b=ptcom->address;			                    //��ʼ��ַ
		b=b+0x8000;
		break; 
	}

	a1=(b>>12)&0xf;                                 //ȡǧλ��
	a2=(b>>8)&0xf;                                  //ȡ��λ��
	a3=(b>>4)&0xf;                                  //ȡʮλ��
	a4=b&0xf;                                       //ȡ��λ��	

	*(U8 *)(AD1+0)=0x40;                            //OMRON��ʼ��Ԫ"@",��40
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);   //plcվ��ַ����λ��ǰ
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
	
	switch (ptcom->registerr)	                     //���ݼĴ����Ĳ�ͬ���в�ͬ�Ĺ�����
	{	
	case 'D':
		*(U8 *)(AD1+18)=0x38; 	
		*(U8 *)(AD1+19)=0x32;
		break;
	case 'R':                                      //CIO�� 
		*(U8 *)(AD1+18)=0x42; 	
		*(U8 *)(AD1+19)=0x30;
		break;
	case 'N':                                      //H�� 
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
	
	*(U8 *)(AD1+20)=asicc(a1);                        //��ʼ��ַ
	*(U8 *)(AD1+21)=asicc(a2);
	*(U8 *)(AD1+22)=asicc(a3);
	*(U8 *)(AD1+23)=asicc(a4);	
		
	*(U8 *)(AD1+24)=0x30; 	                          //ģ����ûС��λ
	*(U8 *)(AD1+25)=0x30;
	 
	
	c=ptcom->register_length;                         //�����ٸ�Ԫ��

	c1=(c>>12)&0xf;                                   //ȡǧλ��
	c2=(c>>8)&0xf;                                    //ȡ��λ��
	c3=(c>>4)&0xf;                                    //ȡʮλ��
	c4=c&0xf;                                         //ȡ��λ��

	*(U8 *)(AD1+26)=asicc(c1);	                      //����Ԫ����������asicc�뷢�ͣ����δӸߵ���
	*(U8 *)(AD1+27)=asicc(c2);
	*(U8 *)(AD1+28)=asicc(c3);
	*(U8 *)(AD1+29)=asicc(c4);
	
	for (i=0;i<c;i++)                                  //д����Ԫ��ֵ
	{				
		c0=ptcom->U8_Data[i*2];                        //��D[]������Ҫ���ݣ���Ӧ��c1Ϊ��λ��c0Ϊ��λ
		c1=ptcom->U8_Data[i*2+1];
						
		c01=c0&0xf;                                    //����д������asicc����c0Ϊ��λ
		c02=(c0>>4)&0xf;                               //����д������asicc����c0Ϊ��λ		
		c03=c1&0xf;                                    //����д������asicc����c1Ϊ��λ
		c04=(c1>>4)&0xf;                               //����д������asicc����c0Ϊ��λ				
	
		*(U8 *)(AD1+30+i*4)=asicc(c04);
		*(U8 *)(AD1+31+i*4)=asicc(c03);
		*(U8 *)(AD1+32+i*4)=asicc(c02);
		*(U8 *)(AD1+33+i*4)=asicc(c01);	
	}
		
	aakj=CalFCS((U8 *)AD1,30+c*4);           	     //FCSУ�飬��λ��ǰ����λ�ں�         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+30+c*4)=asicc(a1);          	     //asicc����ʾ
	*(U8 *)(AD1+31+c*4)=asicc(a2); 
	*(U8 *)(AD1+32+c*4)=0x2a;               	     //������Ԫ2a,0d
	*(U8 *)(AD1+33+c*4)=0x0d;

		
	ptcom->send_length[0]=34+c*4;		     	     //���ͳ���
	ptcom->send_staradd[0]=0;				     	 //�������ݴ洢��ַ	
	ptcom->send_times=1;					     	 //���ʹ���
			
	ptcom->return_length[0]=27;				     	 //�������ݳ���
	ptcom->return_start[0]=0;				     	 //����������Ч��ʼ
	ptcom->return_length_available[0]=0;	     	 //������Ч���ݳ���		
	ptcom->Current_Times=0;	
	
	ptcom->Simens_Count=0;	                         //�ַ��أ����ݲ�ͬ��ֵ��ѡ��ͬ�����ݴ���ʽ
}

/***********************************************************************************************************************************************************************
 * Function: д�䷽��PLC
 * Parameters: ��
 * Return: true: ��
***********************************************************************************************************************************************************************/
void Write_Recipe()								    
{
	int datalength;
	int staradd;
	int SendTimes;                                  //���͵Ĵ���
	int LastTimeWord;							    //���һ�η��ͳ���
	int i,j;
	int ps;
	int b;
	int c,c2,c3,c4;
	int c0,c1,c01,c02,c03,c04;
	int a1,a2,a3,a4;
	U16 aakj;
	int length;
	int plcadd;	
	
	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));//���ݳ���
	staradd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//���ݿ�ʼ��ַ
	plcadd=*(U8 *)(PE+4);

	if(datalength%28==0)                       //OMRON����ܷ�28��D�����ݳ��ȸպ���28�ı���ʱ
	{
		SendTimes=datalength/28;               //���͵Ĵ���Ϊdatalength/28               
		LastTimeWord=28;                       //���һ�η��͵ĳ���Ϊ28��D
	}
	if(datalength%28!=0)                       //���ݳ��Ȳ���28D�ı���ʱ
	{
		SendTimes=datalength/28+1;             //���͵Ĵ���datalength/28+1
		LastTimeWord=datalength%28;            //���һ�η��͵ĳ���Ϊ��28������
	}

    ps=146;                                    //��������28��D��ʱ��ǰ��ÿ�η�146��

	for (i=0;i<SendTimes;i++)
	{   
		if (i!=(SendTimes-1))                  //�������һ�η���ʱ
		{	
			length=28;                         //��28��D
		}
		else                                   //���һ�η���ʱ
		{
			length=LastTimeWord;               //��ʣ��ĳ���             
		}
		
	b=staradd+i*28;                      	   //ÿ��ƫ��28��D
		
	a1=(b>>12)&0xf;                       	   //ȡǧλ��
	a2=(b>>8)&0xf;                             //ȡ��λ��
	a3=(b>>4)&0xf;                             //ȡʮλ��
	a4=b&0xf;                                  //ȡ��λ��	

	*(U8 *)(AD1+0+ps*i)=0x40;                  //OMRON��ʼ��Ԫ"@",��40
	*(U8 *)(AD1+1+ps*i)=asicc(((plcadd&0xf0)>>4)&0xf); //plcվ��ַ����λ��ǰ
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
	 
	
	c=length;                        //�����ٸ�Ԫ��

	c1=(c>>12)&0xf;                  //ȡǧλ��
	c2=(c>>8)&0xf;                   //ȡ��λ��
	c3=(c>>4)&0xf;                   //ȡʮλ��
	c4=c&0xf;                        //ȡ��λ��

	*(U8 *)(AD1+26+ps*i)=asicc(c1);	 //����Ԫ����������asicc�뷢�ͣ����δӸߵ���
	*(U8 *)(AD1+27+ps*i)=asicc(c2);
	*(U8 *)(AD1+28+ps*i)=asicc(c3);
	*(U8 *)(AD1+29+ps*i)=asicc(c4);

		for(j=0;j<length;j++)        //д����Ԫ��ֵ                 
		{	
			c0=*(U8 *)(PE+9+i*56+j*2);//�Ӵ����ݵļĴ�����ʼ��ַPE+9ȡ���ݣ�k3Ϊ��λ��k4Ϊ��λ
			c1=*(U8 *)(PE+9+i*56+j*2+1);
							
			c01=c0&0xf;               //����д������asicc����c0Ϊ��λ
			c02=(c0>>4)&0xf;          //����д������asicc����c0Ϊ��λ		
			c03=c1&0xf;               //����д������asicc����c1Ϊ��λ
			c04=(c1>>4)&0xf;          //����д������asicc����c0Ϊ��λ				
	
			*(U8 *)(AD1+30+j*4+ps*i)=asicc(c04);
			*(U8 *)(AD1+31+j*4+ps*i)=asicc(c03);
			*(U8 *)(AD1+32+j*4+ps*i)=asicc(c02);
			*(U8 *)(AD1+33+j*4+ps*i)=asicc(c01);							
		}

		aakj=CalFCS((U8 *)(AD1+ps*i),30+length*4);      //FCSУ�飬��λ��ǰ����λ�ں�         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+34+(length-1)*4+ps*i)=asicc(a1);      //asicc����ʾ
		*(U8 *)(AD1+35+(length-1)*4+ps*i)=asicc(a2);
		*(U8 *)(AD1+36+(length-1)*4+ps*i)=0x2a;          //������Ԫ2a,0d
		*(U8 *)(AD1+37+(length-1)*4+ps*i)=0x0d;
		
		ptcom->send_length[i]=34+length*4;	   //���ͳ���
		ptcom->send_staradd[i]=i*ps;		   //�������ݴ洢��ַ	
		
		ptcom->return_length[i]=27;			   //�������ݳ���
		ptcom->return_start[i]=0;			   //����������Ч��ʼ
		ptcom->return_length_available[i]=0;   //������Ч���ݳ���	
    }
	ptcom->send_times=SendTimes;			   //���ʹ���
	ptcom->Current_Times=0;
	ptcom->Simens_Count=0;					  //�ַ���				
}

/***********************************************************************************************************************************************************************
 * Function: дʱ�䵽PLC
 * Parameters: ��
 * Return: true: ��
***********************************************************************************************************************************************************************/
void Write_Time()                                     
{
	Write_Analog();									
}

/***********************************************************************************************************************************************************************
 * Function: ��PLC��ȡʱ��
 * Parameters: ��
 * Return: true: ��
***********************************************************************************************************************************************************************/
void Read_Time()									     
{
	Read_Analog();
}

/***********************************************************************************************************************************************************************
 * Function: ����ɱ�׼�洢��ʽ,��������
 * Parameters: ��
 * Return: true: ��
***********************************************************************************************************************************************************************/
void compxy(void)				              
{
	int i;
	unsigned char a1,a2,a3,a4,a5,a6,a7,a8;
	int b1,b2;
	if(ptcom->Simens_Count==1)                //λ���صĴ������ݵߵ��洢
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
	else if(ptcom->Simens_Count==2)           //T/C�������ݷ��ش�������30 31��ʾ��ͨ��30 30��ʾ�Ͽ�
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
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/4;i++)	//ASC�귵�أ�����ҪתΪ16���ƣ�4��asicc�뻻��2��16������
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
	//		if(ptcom->Simens_Count==1)	                         //����������ʱ��Ϊ1����˳��
	//		{
	//			*(U8 *)(COMad+i*2)=b2;					         //���´�,�ӵ�0����ʼ��
	//			*(U8 *)(COMad+i*2+1)=b1;					     //���´�,�ӵ�0����ʼ��
	//		}		
	//		else
	//		{ 
				*(U8 *)(COMad+i*2)=b1;					         //���´�,�ӵ�0����ʼ��
				*(U8 *)(COMad+i*2+1)=b2;					     //���´�,�ӵ�0����ʼ��		
	//		}	
		}
		ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;	//���ȼ���	
	} 
	else
	{
		ptcom->IfResultCorrect=0;
	}
                             
}

/***********************************************************************************************************************************************************************
 * Function: �������У��
 * Parameters: ��
 * Return: true: ��
***********************************************************************************************************************************************************************/
void watchcom(void)		
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

/***********************************************************************************************************************************************************************
 * Function: תΪAsc��
 * Parameters: ��
 * Return: true: ��
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
 * Function: AscתΪ����
 * Parameters: ��
 * Return: true: ��
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
 * Function: �����������ݼ���У�����Ƿ���ȷ
 * Parameters: ��
 * Return: true: ��
***********************************************************************************************************************************************************************/
int remark()				
{
	unsigned int aakj1;
	unsigned int aakj2;
	unsigned int akj1;
	unsigned int akj2;
	aakj2=*(U8 *)(COMad+ptcom->return_length[ptcom->Current_Times-1]-3)&0xff;		//�ڷ������Current_Times++����ʱҪ--
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
 * Function: У�麯��
 * Parameters: ��
 * Return: true: ��
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

