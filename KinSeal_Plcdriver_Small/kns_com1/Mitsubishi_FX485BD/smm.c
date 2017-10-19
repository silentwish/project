/***********************************************************************************************************************************************************************
 * FX485BD
 * ˵��
 * X				ooo		0~377			λ��ӳ���ַ��X								
 * Y				ooo 	0~377			λ��ӳ���ַ��Y								
 * M				dddd	0~7999			λ��ӳ���ַ��M	
 * SM				dddd	8000~8999		λ��ӳ���ַ��H	
 * S				ddd		0~999			λ��ӳ���ַ��K	
 * T				ddd		0~511			λ��ӳ���ַ��T	
 * C				ddd		0~255			λ��ӳ���ַ��C								
 * D_Bit			dddd.dd	0.00~7999.15	λ��ӳ���ַ��L							
 * D				dddd	0~7999			�֣�ӳ���ַ��D								
 * SD				dddd	8000~8999		�֣�ӳ���ַ��N	
 * R				ddddd	0-32766			�֣�ӳ���ַ��R	
 * TV				ddd		0~511			�֣�ӳ���ַ��t	
 * CV				ddd		0~199			�֣�ӳ���ַ��c	
 * 32CV				ddd		200~255			˫�֣�ӳ���ַ��c ��ַƫ��1000����ѡ���ַ200���ټ���1000����ƫ��								
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
//	sysprintf("enter plc driver %c,ptcom->R_W_Flag %d\n",ptcom->registerr,ptcom->R_W_Flag);		

	switch (ptcom->R_W_Flag)
	{
	case PLC_READ_DATA:						//���������Ƕ�����
	case PLC_READ_DIGITALALARM:					//���������Ƕ�����,����	
	case PLC_READ_TREND:					//���������Ƕ�����,����ͼ
	case PLC_READ_ANALOGALARM:					//���������Ƕ�����,��ȱ���	
	case PLC_READ_CONTROL:					//���������Ƕ�PLC���Ƶ�����	
		switch(ptcom->registerr)
		{
		case 'X':
		case 'Y':
		case 'M':
		case 'H':
		case 'K':						
			Read_Bool();   		//���������Ƕ�λ����       
			break;
		case 'T':		
		case 'C':
			Read_Bool_TC();		//���������Ƕ�λ����  
			break;
		case 'D':
		case 'N':		
		case 't':					
			Read_Analog();  		//���������Ƕ�ģ������ 
			break;
		case 'c':
			{
				if(ptcom->address > 1000)
				{
					Read_Analog_CV32();  		//���������Ƕ�ģ������ 
				}
				else
				{
					Read_Analog();  		//���������Ƕ�ģ������ 
				}
				break;
			}
		case 'R':
			Read_Analog_R();		//���������Ƕ�ģ������ 
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
			Set_Reset();      	//����������ǿ����λ�͸�λ
			break;
		case 'D':
		case 'N':
		case 'R':
		case 't':		
			Write_Analog();	  	//����������дģ������	
			break;	
		case 'c':
			{
				if(ptcom->address > 1000)
				{
					Write_Analog_CV32();  		//����������дģ������
				}
				else
				{
					Write_Analog();  		//����������дģ������ 
				}
				break;
			}	
		}
		break;	
	case PLC_WRITE_TIME:						//����������дʱ�䵽PLC
		switch(ptcom->registerr)
		{
		case 'D':
			Write_Time();		
			break;			
		}
		break;	
	case PLC_READ_TIME:						//���������Ƕ�ȡʱ�䵽PLC
		switch(ptcom->registerr)
		{
		case 'D':		
			Read_Time();		
			break;			
		}
		break;
	case PLC_WRITE_RECIPE:						//����������д�䷽��PLC
		switch(*(U8 *)(PE+3))	//�䷽�Ĵ�������
		{
		case 'D':		
			Write_Recipe();		
			break; 		
		}
		break;
	case PLC_READ_RECIPE:						//���������Ǵ�PLC��ȡ�䷽
		switch(*(U8 *)(PE+3))	//�䷽�Ĵ�������
		{
		case 'D':		
			Read_Recipe();		
			break;			
		}
		break;	
	case 7:						//���������ǰ�PLC��״̬����Ϊ����
		//handshake();
		break;									
	case PLC_CHECK_DATA:						//�������������ݴ���
		watchcom();
		break;
	default:
		//handshake();
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
	
	
	*(U8 *)(AD1+0)=0x05;                              	//��ʼ��
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);     	//plcվ��ַ����λ��ǰ
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
	
	switch (ptcom->registerr)							//���ݲ�ͬ�ļĴ���������ʼ��ַ
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
	
	if (ptcom->writeValue==1)	                  	//��λ����
	{
		*(U8 *)(AD1+15)=0x31;                     	//��1
	}		
	else 
	{
		*(U8 *)(AD1+15)=0x30;                     	//��0		
	}
		                        	 		
	aakj=CalFCS((U8 *)(AD1+1),15);                	//��ȥ��ʼ��05�����У��         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	
	*(U8 *)(AD1+16)=asicc(a1); 	
	*(U8 *)(AD1+17)=asicc(a2); 
	
	ptcom->send_length[0]=18;				  		//���ͳ���
	ptcom->send_staradd[0]=0;				  		//�������ݴ洢��ַ	
	ptcom->send_times=1;					  		//���ʹ���
			
	ptcom->return_length[0]=5;           			//�������ݳ���
	ptcom->return_start[0]=0;				  		//����������Ч��ʼ
	ptcom->return_length_available[0]=0;	  		//������Ч���ݳ���	
	ptcom->Current_Times=0;					  		//��ǰ���ʹ���	
	
	ptcom->Simens_Count=0;					  		//λ���أ���ͬ�ķ����в�ͬ�����ݴ���ʽ		
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

	*(U8 *)(AD1+0)=0x05;                              	//��ʼ��
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);     	//plcվ��ַ����λ��ǰ
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
	
	c=ptcom->register_length;                 		//�����ٸ�Ԫ��
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
	                     				
	aakj=CalFCS((U8 *)(AD1+1),14);                	//��ȥ��ʼ��05�����У��         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	
	*(U8 *)(AD1+15)=asicc(a1); 	
	*(U8 *)(AD1+16)=asicc(a2); 
	
	ptcom->send_length[0]=17;				  		//���ͳ���
	ptcom->send_staradd[0]=0;				  		//�������ݴ洢��ַ	
	ptcom->send_times=1;					  		//���ʹ���
			
	ptcom->return_length[0]=8+c1*4;           		//�������ݳ���
	ptcom->return_start[0]=5;				  		//����������Ч��ʼ
	ptcom->return_length_available[0]=c1*4;	  		//������Ч���ݳ���	
	ptcom->Current_Times=0;					  		//��ǰ���ʹ���	
	ptcom->send_add[0]=ptcom->address;		  		//�����������ַ������
   	
	ptcom->Simens_Count=1;					  		//λ���أ���ͬ�ķ����в�ͬ�����ݴ���ʽ	
			
}

/***********************************************************************************************************************************************************************
 * Function: ��ȡ����������Ϣ
 * Parameters: ��
 * Return: true: ��
***********************************************************************************************************************************************************************/
void Read_Bool_TC()				                          
{
	U16 aakj;
	int b,t,c;
	int a1,a2,a3,a4;
	int plcadd;	
	int c1,c2,c3,c4;	
		
	b=ptcom->address;			                      	//��ʼ��ַ
	plcadd=ptcom->plc_address;	                      	//PLCվ��ַ

	*(U8 *)(AD1+0)=0x05;                              	//��ʼ��
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);     	//plcվ��ַ����λ��ǰ
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
	
	c=ptcom->register_length;                 		//�����ٸ�Ԫ��
	c1=c*8;   
	c2=(c1>>4)&0xf;
	c3=c1&0xf;                		   	
	*(U8 *)(AD1+13)=asicc(c2);		 	
	*(U8 *)(AD1+14)=asicc(c3);    
	                     				
	aakj=CalFCS((U8 *)(AD1+1),14);                	//��ȥ��ʼ��05�����У��         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	
	*(U8 *)(AD1+15)=asicc(a1); 	
	*(U8 *)(AD1+16)=asicc(a2); 
	
	ptcom->send_length[0]=17;				  		//���ͳ���
	ptcom->send_staradd[0]=0;				  		//�������ݴ洢��ַ	
	ptcom->send_times=1;					  		//���ʹ���
			
	ptcom->return_length[0]=8+c1;           		//�������ݳ���
	ptcom->return_start[0]=5;				  		//����������Ч��ʼ
	ptcom->return_length_available[0]=c1;	  		//������Ч���ݳ���	
	ptcom->Current_Times=0;					  		//��ǰ���ʹ���	
	ptcom->send_add[0]=ptcom->address;		  		//�����������ַ������
   	
	ptcom->Simens_Count=2;					  		//λ���أ���ͬ�ķ����в�ͬ�����ݴ���ʽ	
			
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
	
	
	*(U8 *)(AD1+0)=0x05;                          //��ʼ��ENQ
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf); //plcվ��ַ����λ��ǰ
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
 	
 	switch (ptcom->registerr)	                  //���ݲ�ͬ�ļĴ���������ʼ��ַ
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
	
	c=ptcom->register_length;                      //�����ٸ�Ԫ��
	
	c1=(c>>4)&0xf;                                
	c2=c&0xf;                                 
	    	
	*(U8 *)(AD1+13)=asicc(c1);	 	
	*(U8 *)(AD1+14)=asicc(c2);	
	
	aakj=CalFCS((U8 *)(AD1+1),14);                	//��ȥ��ʼ��05�����У��         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	
	*(U8 *)(AD1+15)=asicc(a1); 	
	*(U8 *)(AD1+16)=asicc(a2); 
	
	ptcom->send_length[0]=17;				  		//���ͳ���
	ptcom->send_staradd[0]=0;				  		//�������ݴ洢��ַ	
	ptcom->send_times=1;					  		//���ʹ���
			
	ptcom->return_length[0]=8+c*4;           		//�������ݳ���
	ptcom->return_start[0]=5;				  		//����������Ч��ʼ
	ptcom->return_length_available[0]=c*4;	  		//������Ч���ݳ���	
	ptcom->Current_Times=0;					  		//��ǰ���ʹ���	
	ptcom->send_add[0]=ptcom->address;		  		//�����������ַ������
		
	ptcom->Simens_Count=5;                         //�ַ��أ����ݲ�ͬ��ֵ��ѡ��ͬ�����ݴ���ʽ
}

/***********************************************************************************************************************************************************************
 * Function: ��ģ����
 * Parameters: ��
 * Return: true: ��
***********************************************************************************************************************************************************************/
void Read_Analog_CV32()				                  
{
	U16 aakj;
	int b,c;
	int a1,a2,a3,a4;
	int c1,c2,c3,c4;
	int plcadd;
	
	plcadd=ptcom->plc_address;	                  //PLCվ��ַ
	b=(ptcom->address - 1000)/2;			      //��ʼ��ַ��1000�ٳ���2����ƫ��
	
	
	*(U8 *)(AD1+0)=0x05;                          //��ʼ��ENQ
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf); //plcվ��ַ����λ��ǰ
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
	
	/*�жϳ��ȳ��ޣ���Ϊÿ�������ĵ�ַ�ͱ�ʾ32λ�����ϵ�ʱ�����ֳ�����������*/
    if (b + ptcom->register_length > 256)
    {
        ptcom->register_length = 256 - b;
    }
    
    c=ptcom->register_length;                   	//�����ٸ�Ԫ�� 
		
	c1=(c>>4)&0xf;                                
	c2=c&0xf;                                 
	    	
	*(U8 *)(AD1+13)=asicc(c1);	 	
	*(U8 *)(AD1+14)=asicc(c2);	
	
	aakj=CalFCS((U8 *)(AD1+1),14);                	//��ȥ��ʼ��05�����У��         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	
	*(U8 *)(AD1+15)=asicc(a1); 	
	*(U8 *)(AD1+16)=asicc(a2); 
	
	ptcom->send_length[0]=17;				  		//���ͳ���
	ptcom->send_staradd[0]=0;				  		//�������ݴ洢��ַ	
	ptcom->send_times=1;					  		//���ʹ���
			
	ptcom->return_length[0]=8+c*8;           		//�������ݳ���
	ptcom->return_start[0]=5;				  		//����������Ч��ʼ
	ptcom->return_length_available[0]=c*8;	  		//������Ч���ݳ���	
	ptcom->Current_Times=0;					  		//��ǰ���ʹ���	
	ptcom->send_add[0]=ptcom->address;		  		//�����������ַ������
		
	ptcom->Simens_Count=6;                         //˫�ַ��أ����ݲ�ͬ��ֵ��ѡ��ͬ�����ݴ���ʽ
}

/***********************************************************************************************************************************************************************
 * Function: ��ģ����
 * Parameters: ��
 * Return: true: ��
***********************************************************************************************************************************************************************/
void Read_Analog_R()				                  
{
	U16 aakj;
	int b,c;
	int a1,a2,a3,a4,a5,a6;
	int c1,c2,c3,c4;
	int plcadd;
	
	plcadd=ptcom->plc_address;	                  //PLCվ��ַ
	b=ptcom->address;			                  //��ʼ��ַ
	
	
	*(U8 *)(AD1+0)=0x05;                          //��ʼ��ENQ
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf); //plcվ��ַ����λ��ǰ
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
		
	c=ptcom->register_length;                      //�����ٸ�Ԫ��
	
	c1=(c>>4)&0xf;                                
	c2=c&0xf;                                 
	    	
	*(U8 *)(AD1+15)=asicc(c1);	 	
	*(U8 *)(AD1+16)=asicc(c2);	
	
	aakj=CalFCS((U8 *)(AD1+1),16);                	//��ȥ��ʼ��05�����У��         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	
	*(U8 *)(AD1+17)=asicc(a1); 	
	*(U8 *)(AD1+18)=asicc(a2); 
	
	ptcom->send_length[0]=19;				  		//���ͳ���
	ptcom->send_staradd[0]=0;				  		//�������ݴ洢��ַ	
	ptcom->send_times=1;					  		//���ʹ���
			
	ptcom->return_length[0]=8+c*4;           		//�������ݳ���
	ptcom->return_start[0]=5;				  		//����������Ч��ʼ
	ptcom->return_length_available[0]=c*4;	  		//������Ч���ݳ���	
	ptcom->Current_Times=0;					  		//��ǰ���ʹ���	
	ptcom->send_add[0]=ptcom->address;		  		//�����������ַ������
		
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

	if(datalength%30==0)                           //����ܷ�30��D�����ݸպ���30D�ı���
	{
		SendTimes=datalength/30;                   //���ʹ���
		LastTimeWord=30;                           //���һ�η��͵ĳ���Ϊ30D	
	}
	if(datalength%30!=0)                           //����ܷ�30��D�����ݲ���30D�ı��� 
	{
		SendTimes=datalength/30+1;                 //���͵Ĵ���
		LastTimeWord=datalength%30;                //���һ�η��͵ĳ���Ϊ��30������	
	}
	
	for (i=0;i<SendTimes;i++)
	{
		ps=i*17;                                   //ÿ�η�17������
		b=p_start+i*30;                            //��ʼ��ַ
		
		a1=b/1000;                               
		a2=(b%1000)/100;                               
		a3=(b%100)/10;                                
		a4=b%10; 
		
		*(U8 *)(AD1+0+ps)=0x05;                          //��ʼ��ENQ
		*(U8 *)(AD1+1+ps)=asicc(((plcadd&0xf0)>>4)&0xf); //plcվ��ַ����λ��ǰ
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
					
		c=ptcom->register_length;                      //�����ٸ�Ԫ��
		
		c1=(c>>4)&0xf;                                
		c2=c&0xf;                                 
		
		if (i!=(SendTimes-1))	                    //�������һ�η���ʱ
		{
			c=30;                      				//�����ٸ�Ԫ��
			c1=(c>>4)&0xf;                                
			c2=c&0xf; 
			
			currentlength=30;                       //�̶�����30��D
		}
		else
		{
			c=LastTimeWord;                         //�����ٸ�Ԫ��
			c1=(c>>4)&0xf;                                
			c2=c&0xf; 
			
			currentlength=LastTimeWord;             //ʣ��LastTimeWord��D
		}
		    	
		*(U8 *)(AD1+13+ps)=asicc(c1);	 	
		*(U8 *)(AD1+14+ps)=asicc(c2);	
		
		aakj=CalFCS((U8 *)(AD1+1+ps),14);                	//��ȥ��ʼ��05�����У��         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		
		*(U8 *)(AD1+15+ps)=asicc(a1); 	
		*(U8 *)(AD1+16+ps)=asicc(a2); 
		
		ptcom->send_length[i]=17;				    //���ͳ���
		ptcom->send_staradd[i]=i*17;			    //�������ݴ洢��ַ	
		ptcom->send_add[i]=p_start+i*30;		    //�����������ַ������	
		ptcom->send_data_length[i]=currentlength;	//�������һ�ζ���28��D
				
		ptcom->return_length[i]=8+currentlength*4; //�������ݳ��ȣ���27���̶�
		ptcom->return_start[i]=5;				    //����������Ч��ʼ
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
	
	plcadd=ptcom->plc_address;	                  //PLCվ��ַ
	b=ptcom->address;			                  //��ʼ��ַ
	
	
	*(U8 *)(AD1+0)=0x05;                          //��ʼ��ENQ
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf); //plcվ��ַ����λ��ǰ
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

	switch (ptcom->registerr)	                  //���ݲ�ͬ�ļĴ���������ʼ��ַ
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
	
	c=ptcom->register_length;                      //д���ٸ�Ԫ��
	
	c1=(c>>4)&0xf;                                
	c2=c&0xf;                                 
	    	
	*(U8 *)(AD1+13)=asicc(c1);	 	
	*(U8 *)(AD1+14)=asicc(c2);	
	    	
	for (i=0;i<c;i++)                                  //д����Ԫ��ֵ
	{				
		c0=ptcom->U8_Data[i*2];                        //��D[]������Ҫ���ݣ���Ӧ��c1Ϊ��λ��c0Ϊ��λ
		c1=ptcom->U8_Data[i*2+1];
						
		c01=c0&0xf;                                    //����д������asicc����c0Ϊ��λ
		c02=(c0>>4)&0xf;                               //����д������asicc����c0Ϊ��λ		
		c03=c1&0xf;                                    //����д������asicc����c1Ϊ��λ
		c04=(c1>>4)&0xf;                               //����д������asicc����c1Ϊ��λ				
	
		*(U8 *)(AD1+15+i*4)=asicc(c04);
		*(U8 *)(AD1+16+i*4)=asicc(c03);
		*(U8 *)(AD1+17+i*4)=asicc(c02);
		*(U8 *)(AD1+18+i*4)=asicc(c01);	
	}
	
	aakj=CalFCS((U8 *)(AD1+1),14+c*4);           	    //��ȥ��ʼ��05�����У��         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+15+c*4)=asicc(a1);          	     //asicc����ʾ
	*(U8 *)(AD1+16+c*4)=asicc(a2); 
			
	ptcom->send_length[0]=17+c*4;		     	     //���ͳ���
	ptcom->send_staradd[0]=0;				     	 //�������ݴ洢��ַ	
	ptcom->send_times=1;					     	 //���ʹ���
			
	ptcom->return_length[0]=5;				     	 //�������ݳ���
	ptcom->return_start[0]=0;				     	 //����������Ч��ʼ
	ptcom->return_length_available[0]=0;	     	 //������Ч���ݳ���		
	ptcom->Current_Times=0;	
	
	ptcom->Simens_Count=0;	                         //�ַ��أ����ݲ�ͬ��ֵ��ѡ��ͬ�����ݴ���ʽ
}

/***********************************************************************************************************************************************************************
 * Function: дģ����
 * Parameters: ��
 * Return: true: ��
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
	
	plcadd=ptcom->plc_address;	                  //PLCվ��ַ
	b=(ptcom->address - 1000)/2;			      //��ʼ��ַ��1000�ٳ���2����ƫ��
	
	
	*(U8 *)(AD1+0)=0x05;                          //��ʼ��ENQ
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf); //plcվ��ַ����λ��ǰ
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
	
	c=ptcom->register_length/2;                      //д���ٸ�Ԫ��
	if (c == 0)
	{
		c = 1;
	}
	c1=(c>>4)&0xf;                                
	c2=c&0xf;                                 
	    	
	*(U8 *)(AD1+13)=asicc(c1);	 	
	*(U8 *)(AD1+14)=asicc(c2);	
	    	
	for (i=0;i<c;i++)                                  //д����Ԫ��ֵ
	{				
		c0=ptcom->U8_Data[i*4];                        //��D[]������Ҫ���ݣ���Ӧ��c1Ϊ��λ��c0Ϊ��λ
		c1=ptcom->U8_Data[i*4+1];
		c2=ptcom->U8_Data[i*4+2];
		c3=ptcom->U8_Data[i*4+3];
						
		c01=c0&0xf;                                    //����д������asicc����c0Ϊ��λ
		c02=(c0>>4)&0xf;                               //����д������asicc����c0Ϊ��λ		
		c03=c1&0xf;                                    //����д������asicc����c1Ϊ��λ
		c04=(c1>>4)&0xf;                               //����д������asicc����c1Ϊ��λ				
		c05=c2&0xf;                                    //����д������asicc����c2Ϊ��λ
		c06=(c2>>4)&0xf;                               //����д������asicc����c2Ϊ��λ		
		c07=c3&0xf;                                    //����д������asicc����c3Ϊ��λ
		c08=(c3>>4)&0xf;                               //����д������asicc����c3Ϊ��λ
		
		*(U8 *)(AD1+15+i*8)=asicc(c08);
		*(U8 *)(AD1+16+i*8)=asicc(c07);
		*(U8 *)(AD1+17+i*8)=asicc(c06);
		*(U8 *)(AD1+18+i*8)=asicc(c05);	
		*(U8 *)(AD1+19+i*8)=asicc(c04);
		*(U8 *)(AD1+20+i*8)=asicc(c03);
		*(U8 *)(AD1+21+i*8)=asicc(c02);
		*(U8 *)(AD1+22+i*8)=asicc(c01);	
	}
	
	aakj=CalFCS((U8 *)(AD1+1),14+c*8);           	    //��ȥ��ʼ��05�����У��         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+15+c*8)=asicc(a1);          	     //asicc����ʾ
	*(U8 *)(AD1+16+c*8)=asicc(a2); 
			
	ptcom->send_length[0]=17+c*8;		     	     //���ͳ���
	ptcom->send_staradd[0]=0;				     	 //�������ݴ洢��ַ	
	ptcom->send_times=1;					     	 //���ʹ���
			
	ptcom->return_length[0]=5;				     	 //�������ݳ���
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

	//U32 lw = *(U32*)(lw_word + 0);    /*������Ϣ*/
	
	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));//���ݳ���
	staradd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//���ݿ�ʼ��ַ
	plcadd=*(U8 *)(PE+4);

	if(datalength%30==0)                       //����ܷ�30��D�����ݳ��ȸպ���30�ı���ʱ
	{
		SendTimes=datalength/30;               //���͵Ĵ���Ϊdatalength/30               
		LastTimeWord=30;                       //���һ�η��͵ĳ���Ϊ30��D
	}
	if(datalength%30!=0)                       //���ݳ��Ȳ���30D�ı���ʱ
	{
		SendTimes=datalength/30+1;             //���͵Ĵ���datalength/30+1
		LastTimeWord=datalength%30;            //���һ�η��͵ĳ���Ϊ��30������
	}

    ps=137;                                    //��������30��D��ʱ��ǰ��ÿ�η�137��


   // *(U16*)((U16*)lw + 20) =datalength;    /*������Ϣ*/

	for (i=0;i<SendTimes;i++)
	{   
		if (i!=(SendTimes-1))                  //�������һ�η���ʱ
		{	
			length=30;                         //��30��D
		}
		else                                   //���һ�η���ʱ
		{
			length=LastTimeWord;               //��ʣ��ĳ���             
		}
		
		b=staradd+i*30;                      	   //ÿ��ƫ��30��D
		
		a1=b/1000;                               
		a2=(b%1000)/100;                               
		a3=(b%100)/10;                                
		a4=b%10; 
		
		*(U8 *)(AD1+0+ps*i)=0x05;                          //��ʼ��ENQ
		*(U8 *)(AD1+1+ps*i)=asicc(((plcadd&0xf0)>>4)&0xf); //plcվ��ַ����λ��ǰ
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
		
		c=length;                      //д���ٸ�Ԫ��
		
		c1=(c>>4)&0xf;                                
		c2=c&0xf;                                 
		    	
		*(U8 *)(AD1+13+ps*i)=asicc(c1);	 	
		*(U8 *)(AD1+14+ps*i)=asicc(c2);	
		    	
		for (j=0;j<c;j++)                                  //д����Ԫ��ֵ
		{				
			c0=*(U8 *)(PE+9+i*60+j*2);         //�Ӵ����ݵļĴ�����ʼ��ַPE+9ȡ���ݣ�k3Ϊ��λ��k4Ϊ��λ
			c1=*(U8 *)(PE+9+i*60+j*2+1);
							
			c01=c0&0xf;                                    //����д������asicc����c0Ϊ��λ
			c02=(c0>>4)&0xf;                               //����д������asicc����c0Ϊ��λ		
			c03=c1&0xf;                                    //����д������asicc����c1Ϊ��λ
			c04=(c1>>4)&0xf;                               //����д������asicc����c1Ϊ��λ				
		
			*(U8 *)(AD1+15+j*4+ps*i)=asicc(c04);
			*(U8 *)(AD1+16+j*4+ps*i)=asicc(c03);
			*(U8 *)(AD1+17+j*4+ps*i)=asicc(c02);
			*(U8 *)(AD1+18+j*4+ps*i)=asicc(c01);	
		}
		
		aakj=CalFCS((U8 *)(AD1+1+ps*i),14+c*4);           	    //��ȥ��ʼ��05�����У��         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+15+c*4+ps*i)=asicc(a1);          	     //asicc����ʾ
		*(U8 *)(AD1+16+c*4+ps*i)=asicc(a2); 
				
		
		ptcom->send_length[i]=17+c*4;	   		//���ͳ���
		ptcom->send_staradd[i]=i*ps;		   //�������ݴ洢��ַ	
		
		ptcom->return_length[i]=5;			   //�������ݳ���
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
	int b1,b2,b3,b4;

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
	else if(ptcom->Simens_Count==2)           //T/C�������ݷ��ش���
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
	
			*(U8 *)(COMad+i*2)=b2;					        
			*(U8 *)(COMad+i*2+1)=b1;							     		
		}
		ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;	//���ȼ���	
	} 
	else if(ptcom->Simens_Count==6)
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/8;i++)	//ASC�귵�أ�����ҪתΪ16���ƣ�8��asicc�뻻��4��16������
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
	unsigned int a1,a2,a3;
	unsigned int aakj1;
	unsigned int aakj2;
	unsigned int akj1;
	unsigned int akj2;
	if(ptcom->return_length[ptcom->Current_Times-1] == 5)	//д����ֻ����5������
	{
		a1=*(U8 *)(COMad)&0xff;
		a2=*(U8 *)(COMad+3)&0xff;
		a1=*(U8 *)(COMad+4)&0xff;
		if (a1 == 0x06 && a2 == 0x46 && a3 == 0x46)//����06����ȷ��
        {
            return 2;
        }
        return 0;
	}
	
	aakj2=*(U8 *)(COMad+ptcom->return_length[ptcom->Current_Times-1]-1)&0xff;		//�ڷ������Current_Times++����ʱҪ--
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
 * Function: У�麯��
 * Parameters: ��
 * Return: true: ��
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

