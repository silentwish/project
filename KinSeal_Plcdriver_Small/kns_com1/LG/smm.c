
#include "stdio.h"
#include "def.h"
#include "smm.h"


	
struct Com_struct_D *ptcom;





void Enter_PlcDriver(void)
{
	ptcom=(struct Com_struct_D *)adcom;	
	switch (ptcom->R_W_Flag)
	{
	case PLC_READ_DATA:				//���������Ƕ�����
	case PLC_READ_DIGITALALARM:			//���������Ƕ�����,����	
	case PLC_READ_TREND:			//���������Ƕ�����,����ͼ
	case PLC_READ_ANALOGALARM:			//���������Ƕ�����,��ȱ���	
	case PLC_READ_CONTROL:			//���������Ƕ�PLC���Ƶ�����	
		switch(ptcom->registerr)
		{
		case 'Y':
		case 'M':
		case 'K':
		case 'L':
		case 'T':
		case 'C':						
			Read_Bool();//��������
			break;
		case 'D':
		case 't':
		case 'c':	
			Read_Analog();//��ģ����
			break;			
		}
		break;
	case PLC_WRITE_DATA:				//����������д����
		switch(ptcom->registerr)
		{
		case 'M':
		case 'Y':
		case 'K':
		case 'L':
		case 'T':
		case 'C':				
			Set_Reset();//��λ��λ
			break;
		case 'D':
		case 't':
		case 'c':
			Write_Analog();//дģ����		
			break;			
		}
		break;	
	case PLC_WRITE_TIME:				//����������дʱ�䵽PLC
		switch(ptcom->registerr)
		{
		case 'D':
			
			Write_Time();		
			break;			
		}
		break;	
	case PLC_READ_TIME:				//���������Ƕ�ȡʱ�䵽PLC
		switch(ptcom->registerr)
		{
		case 'D':		
			Read_Time();		
			break;			
		}
		break;
	case PLC_WRITE_RECIPE:				//����������д�䷽��PLC
		switch(*(U8 *)(PE+3))//�䷽�Ĵ�������
		{
		case 'D':		
			Write_Recipe();		
			break;			
		}
		break;
	case PLC_READ_RECIPE:				//���������Ǵ�PLC��ȡ�䷽
		switch(*(U8 *)(PE+3))//�䷽�Ĵ�������
		{
		case 'D':		
			Read_Recipe();		
			break;			
		}
		break;							
	case PLC_CHECK_DATA:				//�������������ݴ���
		watchcom();
		break;				
	}	 
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Set_Reset()//��λ��λ����
{
	U16 aakj;
	int b,t;
	int plcadd;	
	int a1,a2,a3,a4,a5;
	int b1;

	t = 0;

	b=ptcom->address;										// �Ĵ������Ŀ�ʼ��ַ
	plcadd=ptcom->plc_address;								//PLCվ��ַ
	
	*(U8 *)(AD1+0)=0x05;        							//ͷENQ����05
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);  			//plcվ��ַ����λ��ǰ
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);
	*(U8 *)(AD1+3)=0x77;        							//��λ�͸�λ�Ƕ�P,M,L,Kд������w
	*(U8 *)(AD1+4)=0x53;        							//���������ȷ��͵�����������SS(����)����53 53
	*(U8 *)(AD1+5)=0x53; 
    *(U8 *)(AD1+6)=0x30;        							//����������д����P,M,L,K����ַ����Ϊ1 
	*(U8 *)(AD1+7)=0x31;		

	*(U8 *)(AD1+8)=0x30;        							//��ַ�ַ��Ϳ�ʼ��ַ��8������       
	*(U8 *)(AD1+9)=0x38;
	
	switch (ptcom->registerr)								//���ݼĴ������ͷ��Ͳ�ͬ�������ַ�
	{
	case 'Y':
		*(U8 *)(AD1+10)=0x25;       						//P����%PX����25 50 58
		*(U8 *)(AD1+11)=0x50;
		*(U8 *)(AD1+12)=0x58;     
		break;
				
	case 'M':
		*(U8 *)(AD1+10)=0x25;       						//M����%MX����25 4D 58
		*(U8 *)(AD1+11)=0x4D;
		*(U8 *)(AD1+12)=0x58;
		break;	
		
	case 'L':
		*(U8 *)(AD1+10)=0x25;       						//K����%LX����25 4C 58
		*(U8 *)(AD1+11)=0x4C;
		*(U8 *)(AD1+12)=0x58;
		break;
		
	case 'K':
		*(U8 *)(AD1+10)=0x25;       						//L����%KX����25 4B 58
		*(U8 *)(AD1+11)=0x4B;
		*(U8 *)(AD1+12)=0x58;
		break;
		
	case 'T':
		*(U8 *)(AD1+10)=0x25;       						//T����%TX����25 4B 58
		*(U8 *)(AD1+11)=0x54;
		*(U8 *)(AD1+12)=0x58;
		break;	
		
	case 'C':
		*(U8 *)(AD1+10)=0x25;       						//C����%CX����25 4B 58
		*(U8 *)(AD1+11)=0x43;
		*(U8 *)(AD1+12)=0x58;
		break;								
	}	
	switch (ptcom->registerr)								//���ݼĴ������ͷ��Ͳ�ͬ�������ַ�
	{
		case 'Y':
		case 'M':
		case 'L':
		case 'K':	
			b1=b/16;												//��ַת������ַռ5���ֽڣ��ɸߵ��ͣ�ǰ4���ֽ���ʮ���Ʒ���
			a1=b1/1000;												//���һ���ֽ���ʮ�����Ʒ��ͣ���Ҫת��ASC��
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
			a1=b/10000;												//���һ���ֽ���ʮ�����Ʒ��ͣ���Ҫת��ASC��
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

	if (ptcom->writeValue==1)								//��λʱ����30 31
	{
		*(U8 *)(AD1+18+t)=0x30;
		*(U8 *)(AD1+19+t)=0x31;
	}
	if (ptcom->writeValue==0)								//��λʱ����30 30
	{
		*(U8 *)(AD1+18+t)=0x30;
		*(U8 *)(AD1+19+t)=0x30;
	}
	
	*(U8 *)(AD1+20+t)=0x04;       							//������ 04

	aakj=CalcHe((U8 *)AD1,21);  							//��У�飬У��ǰ�������
	a1=aakj&0xff;                 							//ֻ�����λ
	*(U8 *)(AD1+21+t)=asicc((a1 >> 4) & 0xf);        							 	//16������������ת��asicc��
	*(U8 *)(AD1+22+t)=asicc((a1 >> 0) & 0xf);        							 	//16������������ת��asicc��
	
	ptcom->send_length[0]=23;								//���ͳ���PΪ22��
	ptcom->send_staradd[0]=0;								//�������ݴ洢��ַ	
	ptcom->send_times=1;									//���ʹ���
		
	ptcom->return_length[0]=9;								//�������ݳ���
	ptcom->return_start[0]=0;								//����������Ч��ʼ
	ptcom->return_length_available[0]=0;					//������Ч���ݳ���	
	ptcom->Current_Times=0;									//��ǰ���ʹ���			
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Read_Bool()//��ȡ����������Ϣ
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
	
	b=ptcom->address;			    						// �Ĵ���������ַ
	plcadd=ptcom->plc_address;	    						//PLCվ��ַ
	length=ptcom->register_length;  						//�����ĳ���


	switch (ptcom->registerr)
	{
	case 'Y':
	case 'M':
	case 'K':
	case 'L': 
		*(U8 *)(AD1+0)=0x05;            					//ͷENQ����05
		*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);  		//plcվ��ַ����λ��ǰ
		*(U8 *)(AD1+2)=asicc(plcadd&0xf);
		*(U8 *)(AD1+3)=0x72;            					//���������Ƕ�P,M,L,K��������r
		*(U8 *)(AD1+4)=0x53;            					//���������ȷ��͵�����������SB(����)����53 42
		*(U8 *)(AD1+5)=0x42; 
		*(U8 *)(AD1+6)=0x30;            					//��2�����ȷ��͵��ǵ�ַ�ַ����� 
		*(U8 *)(AD1+7)=0x37;		
				
		switch (ptcom->registerr)	    					//���ݼĴ��������в�ͬ�ĵ�ַ�ַ�
		{
			case 'Y': 
				*(U8 *)(AD1+8)=0x25;        				//��3�����ȷ��͵��ǵ�ַ�ַ�%PB       
				*(U8 *)(AD1+9)=0x50; 
			    *(U8 *)(AD1+10)=0x57;   
				break;	
					
			case 'M':		
				*(U8 *)(AD1+8)=0x25;        				//��3�����ȷ��͵��ǵ�ַ�ַ�%MB       
				*(U8 *)(AD1+9)=0x4D; 
			    *(U8 *)(AD1+10)=0x57;         
				break;
					
			case 'K': 
				*(U8 *)(AD1+8)=0x25;        				//��3�����ȷ��͵��ǵ�ַ�ַ�%KB       
				*(U8 *)(AD1+9)=0x4B; 
			    *(U8 *)(AD1+10)=0x57;
				break;
				
			case 'L': 
				*(U8 *)(AD1+8)=0x25;        				//��3�����ȷ��͵��ǵ�ַ�ַ�%LB       
				*(U8 *)(AD1+9)=0x4C; 
			    *(U8 *)(AD1+10)=0x57;
				break;							
		}

		b=b/16;												//��ַת������ַռ5���ֽڣ��ɸߵ��ͣ�ǰ4���ֽ���ʮ���Ʒ��� 
		a1=b/1000;											//���һ���ֽ���ʮ�����Ʒ��ͣ���Ҫת��ASC��
		a2=(b-a1*1000)/100;
		a3=(b-a1*1000-a2*100)/10;
		a4=(b-a1*1000-a2*100-a3*10);		
		*(U8 *)(AD1+11)=asicc(a1);    
		*(U8 *)(AD1+12)=asicc(a2);   	
		*(U8 *)(AD1+13)=asicc(a3);    
		*(U8 *)(AD1+14)=asicc(a4);  
			
		length = length/2 + length%2;				
		b1=(length>>4)&0xf;             					//���ĳ��ȴ���b1��λ��b2��λ
		b2=length&0xf;
		*(U8 *)(AD1+15)=asicc(b1);      					//���ĳ��ȣ�asicc��ʾ
		*(U8 *)(AD1+16)=asicc(b2);
			
		*(U8 *)(AD1+17)=0x04;           					//�����ַ�
				
		aakj=CalcHe((U8 *)AD1,18);      					//��У�飬У��ǰ�������
		a1=aakj&0xff;                   					//ֻ�����λ
		//*(U8 *)(AD1+18)=a1;             					//LG��У�鲻��תasicc
		*(U8 *)(AD1+18)=asicc((a1 >> 4) & 0xf);        							 	//16������������ת��asicc��
		*(U8 *)(AD1+19)=asicc((a1 >> 0) & 0xf);        							 	//16������������ת��asicc��

		ptcom->send_length[0]=20;							//���ͳ���
		ptcom->send_staradd[0]=0;							//�������ݴ洢��ַ	
		ptcom->send_times=1;								//���ʹ���
					
		ptcom->return_length[0]=13+length*4;				//�������ݳ��ȣ���11���̶�
		ptcom->return_start[0]=10;							//����������Ч��ʼ����11��
		ptcom->return_length_available[0]=length*4;			//������Ч���ݳ���	
		ptcom->Current_Times=0;								//��ǰ���ʹ���			
		ptcom->send_add[0]=ptcom->address;					//�����������ַ������
		ptcom->register_length=length*2;				  	//����16λ����1���֣�CΪ�ֽڣ���*2
		ptcom->Simens_Count=1; 								//ȫ�ֱ�����������ֵ0��1�Է��ش���ͬ
		break;
	case 'T':
	case 'C':
		for (i=0;i<length;i++)
		{
			ps=83*i;	
			*(U8 *)(AD1+0+ps)=0x05;            				//ͷENQ����05
			*(U8 *)(AD1+1+ps)=asicc(((plcadd&0xf0)>>4)&0xf);//plcվ��ַ����λ��ǰ
			*(U8 *)(AD1+2+ps)=asicc(plcadd&0xf);
			*(U8 *)(AD1+3+ps)=0x52;            				//���������Ƕ�P,M,L,K��������R
			*(U8 *)(AD1+4+ps)=0x53;            				//���������ȷ��͵�����������SS(����)����53 53
			*(U8 *)(AD1+5+ps)=0x53; 
			*(U8 *)(AD1+6+ps)=0x30;            				//8���� 
			*(U8 *)(AD1+7+ps)=0x38;
			
			for (j=0;j<8;j++)
			{			
				*(U8 *)(AD1+8+j*9+ps)=0x30;           		//��2�����ȷ��͵��ǵ�ַ�ַ����� 
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
			
				add=b+length*i+j;							//ÿ���ַ+1�����������Ķ�8��
				a1=add/1000;
				a2=(add-a1*1000)/100;
				a3=(add-a1*1000-a2*100)/10;
				a4=(add-a1*1000-a2*100-a3*10);
			    *(U8 *)(AD1+13+j*9+ps)=asicc(a1);    
				*(U8 *)(AD1+14+j*9+ps)=asicc(a2);   	
			    *(U8 *)(AD1+15+j*9+ps)=asicc(a3);    
				*(U8 *)(AD1+16+j*9+ps)=asicc(a4);
			}
			 				
			*(U8 *)(AD1+80+ps)=0x04;           				//�����ַ�		
		
			aakj=CalcHe((U8 *)(AD1+ps),81);      			//��У�飬У��ǰ�������
			a1=aakj&0xff;                   				//ֻ�����λ
			*(U8 *)(AD1+81+ps)=asicc((a1 >> 4) & 0xf);             				//LG��У�鲻��תasicc
			*(U8 *)(AD1+82+ps)=asicc((a1 >> 0) & 0xf);             				//LG��У�鲻��תasicc
						
			ptcom->send_length[i]=83;						//���ͳ���
			ptcom->send_staradd[i]=ps;						//�������ݴ洢��ַ	
									
			ptcom->return_length[i]=41;						//�������ݳ���
			ptcom->return_start[i]=8;						//����������Ч��ʼ
			ptcom->return_length_available[i]=32;			//������Ч���ݳ���				
			ptcom->send_add[i]=ptcom->address+8*i;			//�����������ַ������
			ptcom->address=ptcom->address+8*i;					
		}
		ptcom->send_times=length;							//���ʹ���
		ptcom->Current_Times=0;								//��ǰ���ʹ���
		ptcom->Simens_Count=2; 								//ȫ�ֱ�����������ֵ0��1�Է��ش���ͬ
		break; 	
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Read_Analog()//��ģ����
{
	U16 aakj;
	int b;
	int a1,a2,a3,a4;
	int plcadd;
	int length;
	int b1,b2;
	
	plcadd=ptcom->plc_address;	    						//PLCվ��ַ
	b=ptcom->address;			    						// �Ĵ���������ַ
	length=ptcom->register_length;  						//���ĳ���
	     

	*(U8 *)(AD1+0)=0x05;            						//ͷENQ����05
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);        	//plcվ��ַ����λ��ǰ
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);	
	*(U8 *)(AD1+3)=0x72;            						//��ģ����������R
	*(U8 *)(AD1+4)=0x53;            						//���������ȷ��͵�����������SB(����)����53 42
	*(U8 *)(AD1+5)=0x42; 
	*(U8 *)(AD1+6)=0x30;            						//��2�����ȷ��͵��ǵ�ַ�ַ��Ϳ�ʼ��ַ��7������ 
	*(U8 *)(AD1+7)=0x37; 
	
	switch (ptcom->registerr)	    						//���ݼĴ��������в�ͬ�ĵ�ַ�ַ�
	{
	case 'D': 
		*(U8 *)(AD1+8)=0x25;        						//��3�����ȷ��͵��ǵ�ַ�ַ�%DW       
		*(U8 *)(AD1+9)=0x44; 
	    *(U8 *)(AD1+10)=0x57;  
		break;	
			
	case 't':
		*(U8 *)(AD1+8)=0x25;        						//��3�����ȷ��͵��ǵ�ַ�ַ�%TW       
		*(U8 *)(AD1+9)=0x54; 
	    *(U8 *)(AD1+10)=0x57; 	                   
		break;
			
	case 'c': 
		*(U8 *)(AD1+8)=0x25;        						//��3�����ȷ��͵��ǵ�ַ�ַ�%CW       
		*(U8 *)(AD1+9)=0x43; 
	    *(U8 *)(AD1+10)=0x57;
		break;						
	}
	
	a1=(b/1000)&0xf;                       					//ȡ��ʼ��ַ��ǧλ��
	a2=((b-a1*1000)/100)&0xf;              					//ȡ          ��λ��
	a3=((b-a1*1000-a2*100)/10)&0xf;        					//ȡ          ʮλ��
	a4=(b-a1*1000-a2*100-a3*10)&0xf;       					//ȡ          ��λ��
	
	*(U8 *)(AD1+11)=asicc(a1);  							//��ʼ�ĵ�ַ��asicc��ʾ����λ�ȷ�
	*(U8 *)(AD1+12)=asicc(a2);	
	*(U8 *)(AD1+13)=asicc(a3);          
	*(U8 *)(AD1+14)=asicc(a4);
	
	b1=(length>>4)&0xf;                 					//���ĳ��ȴ���b1Ϊ��λ,b2Ϊ��λ
	b2=length&0xf;
		
	*(U8 *)(AD1+15)=asicc(b1);          					//���ĳ��ȣ�asicc��ʾ
	*(U8 *)(AD1+16)=asicc(b2);
	
	*(U8 *)(AD1+17)=0x04;               					//�����ַ�
		
	aakj=CalcHe((U8 *)AD1,18);   							//��У�飬У��ǰ�������
	a1=aakj&0xff;                  							//ֻ�����λ
	//*(U8 *)(AD1+18)=a1;
	*(U8 *)(AD1+18)=asicc((a1 >> 4) & 0xf);        			
	*(U8 *)(AD1+19)=asicc((a1 >> 0) & 0xf);  
	
	ptcom->send_length[0]=20;								//���ͳ���
	ptcom->send_staradd[0]=0;								//�������ݴ洢��ַ	
	ptcom->send_times=1;									//���ʹ���
		
	ptcom->return_length[0]=13+length*4;					//�������ݳ��ȣ���11���̶�
	ptcom->return_start[0]=10;								//����������Ч��ʼ
	ptcom->return_length_available[0]=length*4;				//������Ч���ݳ���	
	ptcom->Current_Times=0;									//��ǰ���ʹ���	
	ptcom->send_add[0]=ptcom->address;						//�����������ַ������
	ptcom->Simens_Count=3; 
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Read_Recipe()//��ȡ�䷽
{
	U16 aakj;
	int b;
	int a1,a2,a3,a4;
	int i;
	int datalength;
	int p_start;
	int ps;
	int SendTimes;
	int LastTimeWord;										//���һ�η��ͳ���
	int currentlength;
	int length;
	int plcadd;
	
		
	datalength=ptcom->register_length;						//�����ܳ���
	p_start=ptcom->address;									//��ʼ��ַ
	length=ptcom->register_length;          				//���ĳ��ȣ����ٸ��䷽
	plcadd=ptcom->plc_address;	            				//PLCվ��ַ
	
	if(datalength>5000)                     				//���Ƴ���
		datalength=5000;

	if(datalength%64==0)                    				//ÿ�η�16���䷽�������䷽���պ�Ϊ16�ı���ʱ
	{
		SendTimes=datalength/64;            				//���͵Ĵ���            
		LastTimeWord=64;                    				//���һ�η��͵ĳ���Ϊ16	
	}
	if(datalength%64!=0)                    				//ÿ�η�16���䷽�������䷽������16�ı���ʱ
	{
		SendTimes=datalength/64+1;          				//���͵Ĵ���
		LastTimeWord=datalength%64;         				//���һ�η��͵ĳ���Ϊ��16������	
	}
	
	for (i=0;i<SendTimes;i++)               				//С�ڷ��ʹ�����������
	{
		ps=20*i;                            				//ÿ�η�19�����ȣ��ڶ��η�����19��ʼ
		b=p_start+i*64;                     				//ÿ�εĿ�ʼ��ַ
		
		a1=(b/1000)&0xf;                    				//ȡǧλ��
		a2=((b-a1*1000)/100)&0xf;           				//ȡ��λ��
		a3=((b-a1*1000-a2*100)/10)&0xf;     				//ȡʮλ��
		a4=(b-a1*1000-a2*100-a3*10)&0xf;    				//ȡ��λ��

		*(U8 *)(AD1+0+ps)=0x05;             				//ͷENQ����05
		*(U8 *)(AD1+1+ps)=asicc(((plcadd&0xf0)>>4)&0xf); 	//plcվ��ַ����λ��ǰ
		*(U8 *)(AD1+2+ps)=asicc(plcadd&0xf);
		*(U8 *)(AD1+3+ps)=0x72;            					//���䷽������R
		*(U8 *)(AD1+4+ps)=0x53;            					//���������ȷ��͵�����������SB(����)����53 42
		*(U8 *)(AD1+5+ps)=0x42; 
		*(U8 *)(AD1+6+ps)=0x30;            					//��2�����ȷ��͵��ǵ�ַ�ַ��Ϳ�ʼ��ַ��7������ 
		*(U8 *)(AD1+7+ps)=0x37; 
		*(U8 *)(AD1+8+ps)=0x25;            					//��3�����ȷ��͵��ǵ�ַ�ַ�%DW       
		*(U8 *)(AD1+9+ps)=0x44; 
	    *(U8 *)(AD1+10+ps)=0x57;
			
		*(U8 *)(AD1+11+ps)=asicc(a1);      					//��ʼ�ĵ�ַ��asicc��ʾ����λ�ȷ�
		*(U8 *)(AD1+12+ps)=asicc(a2);	
		*(U8 *)(AD1+13+ps)=asicc(a3);      
		*(U8 *)(AD1+14+ps)=asicc(a4);
		
		if (i!=(SendTimes-1))	          					//�������һ�η���ʱ
		{
			*(U8 *)(AD1+15+ps)=0x34;      					//�̶�����16������0x10��ת��asicc
			*(U8 *)(AD1+16+ps)=0x30;
			currentlength=64;             					//���͵����ݳ���
		}
		if (i==(SendTimes-1))	          					//���һ�η���ʱ
		{
			*(U8 *)(AD1+15+ps)=asicc((LastTimeWord>>4)&0xf);//���͵ĳ���Ϊ��16������LastTimeWord����λ�ȷ�
			*(U8 *)(AD1+16+ps)=asicc(LastTimeWord&0xf);
			currentlength=LastTimeWord;   					//���͵����ݳ���
		}	
			
		*(U8 *)(AD1+17+ps)=0x04;          					//�����ַ�
		
		aakj=CalcHe((U8 *)(AD1 + ps),18);        					//��У�飬У��ǰ�������
		a1=aakj&0xff;                     					//ֻ�����λ
		//*(U8 *)(AD1+18+ps)=a1;               					//LG��У�鷢���Ǻ�У������λ��16����
		*(U8 *)(AD1+18+ps)=asicc((a1 >> 4) & 0xf);        						
		*(U8 *)(AD1+19+ps)=asicc((a1 >> 0) & 0xf); 
		
		
		ptcom->send_length[i]=20;							//���ͳ���
		ptcom->send_staradd[i]=i*20;						//�������ݴ洢��ַ	
		ptcom->send_add[i]=p_start+i*64;					//�����������ַ������	
		ptcom->send_data_length[i]=currentlength;			//�������һ�ζ���16��
				
		ptcom->return_length[i]=13+currentlength*4;			//�������ݳ��ȣ���11���̶�
		ptcom->return_start[i]=10;				    		//����������Ч��ʼ
		ptcom->return_length_available[i]=currentlength*4;	//������Ч���ݳ���		
	}
	ptcom->send_times=SendTimes;							//���ʹ���
	ptcom->Current_Times=0;					        		//��ǰ���ʹ���	
	ptcom->Simens_Count=3;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Write_Analog()//дģ����
{
	U16 aakj;
	int b,i;
	int a1,a2,a3,a4;
	int b1,b2;
	int length;
	int plcadd;
	int c,c0,c1,c2,c3,c4;
			
	b=ptcom->address;			    						//��ʼ��ַ
	plcadd=ptcom->plc_address;	    						//PLCվ��ַ
	length=ptcom->register_length;  						//д���䷽��
	
	*(U8 *)(AD1+0)=0x05;            						//ͷENQ����05
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf); 			//plcվ��ַ����λ��ǰ
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);
	
	*(U8 *)(AD1+3)=0x77;            						//д�䷽������W
	*(U8 *)(AD1+4)=0x53;            						//���������ȷ��͵�����������SB(����)����53 42
	*(U8 *)(AD1+5)=0x42; 
	*(U8 *)(AD1+6)=0x30;            						//��2�����ȷ��͵��ǵ�ַ�ַ��Ϳ�ʼ��ַ��7������ 
	*(U8 *)(AD1+7)=0x37; 
	
	switch (ptcom->registerr)	    						//���ݼĴ��������в�ͬ�ĵ�ַ�ַ�
	{
	case 'D': 
		*(U8 *)(AD1+8)=0x25;        						//��3�����ȷ��͵��ǵ�ַ�ַ�%DW       
		*(U8 *)(AD1+9)=0x44; 
	    *(U8 *)(AD1+10)=0x57;       
		break;	
			
	case 't':
		*(U8 *)(AD1+8)=0x25;        						//��3�����ȷ��͵��ǵ�ַ�ַ�%TW       
		*(U8 *)(AD1+9)=0x54; 
	    *(U8 *)(AD1+10)=0x57;         
		break;
			
	case 'c': 
		*(U8 *)(AD1+8)=0x25;        						//��3�����ȷ��͵��ǵ�ַ�ַ�%CW       
		*(U8 *)(AD1+9)=0x43; 
	    *(U8 *)(AD1+10)=0x57;
		break;						
	}
	a1=(b/1000)&0xf;                       					//ȡǧλ��
	a2=((b-a1*1000)/100)&0xf;              					//ȡ��λ��
	a3=((b-a1*1000-a2*100)/10)&0xf;        					//ȡʮλ��
	a4=(b-a1*1000-a2*100-a3*10)&0xf;       					//ȡ��λ��
	
	*(U8 *)(AD1+11)=asicc(a1);  							//��ʼ�ĵ�ַ��asicc��ʾ����λ�ȷ�
	*(U8 *)(AD1+12)=asicc(a2);	
	*(U8 *)(AD1+13)=asicc(a3);    
	*(U8 *)(AD1+14)=asicc(a4);
	
	b1=(length>>4)&0xf;             						//д���䷽��������λΪb1����λΪb2
	b2=length&0xf;
		
	*(U8 *)(AD1+15)=asicc(b1);       						//���ĳ��ȣ�asicc��ʾ
	*(U8 *)(AD1+16)=asicc(b2);
	
	for(i=0;i<length;i++)
	{			
	c=ptcom->U8_Data[i*2];   								//��D[]������Ҫ���ݣ���Ӧ��c0Ϊ��λ��cΪ��λ
	c0=ptcom->U8_Data[i*2+1];
						
	c1=c&0xf;                								//����д������asicc����cΪ��λ
	c2=(c>>4)&0xf;           								//����д������asicc����cΪ��λ		
	c3=c0&0xf;               								//����д������asicc����c1Ϊ��λ
	c4=(c0>>4)&0xf;          								//����д������asicc����c1Ϊ��λ				
	
	*(U8 *)(AD1+17+i*4)=asicc(c4);        					//�����Ǹ�λ�ȷ�
	*(U8 *)(AD1+18+i*4)=asicc(c3);
	*(U8 *)(AD1+19+i*4)=asicc(c2);
	*(U8 *)(AD1+20+i*4)=asicc(c1);	
	}
	
	*(U8 *)(AD1+17+length*4)=0x04;        					//�����ַ�
	
	aakj=CalcHe((U8 *)AD1,18+length*4);   					//��У�飬У��ǰ�������
	a1=aakj&0xff;                         					//ֻ�����λ
	//*(U8 *)(AD1+18+length*4)=a1;          					//LG��У�鷢16����	
	*(U8 *)(AD1+18+length*4)=asicc((a1 >> 4) & 0xf);          					//LG��У�鷢16����	
	*(U8 *)(AD1+19+length*4)=asicc((a1 >> 0) & 0xf);          					//LG��У�鷢16����	

	ptcom->send_length[0]=20+length*4;						//���ͳ���
	ptcom->send_staradd[0]=0;								//�������ݴ洢��ַ	
	ptcom->send_times=1;									//���ʹ���
			
	ptcom->return_length[0]=9;								//�������ݳ���
	ptcom->return_start[0]=0;								//����������Ч��ʼ
	ptcom->return_length_available[0]=0;					//������Ч���ݳ���	
	ptcom->Current_Times=0;									//��ǰ���ʹ���
	ptcom->Simens_Count=0;	
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Write_Time()//��PLCдʱ��  
{
	Write_Analog();									
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Read_Time()//��PLC��ȡʱ��
{
	Read_Analog();	
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Write_Recipe()//д�䷽��PLC
{
	int datalength;
	int staradd;
	int SendTimes;
	int LastTimeWord;										//���һ�η��ͳ���
	int i,j;
	int ps;
	int b;
	int a1,a2,a3,a4;
	int b1,b2,b3,b4;
	int c1,c2,c3,c4;
	U16 aakj;
	int plcadd;
	int length;
		
	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));		//���ݳ���
	staradd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//���ݳ���
	plcadd=ptcom->plc_address;	                   			//PLCվ��ַ

	if(datalength%64==0)                           			//д���䷽���պ���16�ı���ʱ             
	{
		SendTimes=datalength/64;                   			//���͵Ĵ���
		LastTimeWord=64;                           			//���һ�η��͵ĳ���	
	}
	if(datalength%64!=0)                           			//����16�ı���ʱ
	{
		SendTimes=datalength/64+1;                 			//���͵Ĵ���
		LastTimeWord=datalength%64;                			//���һ�η��͵ĳ���	
	}	
	
	ps=276;                                         			//д16���䷽ʱ�����͵���83������
	for (i=0;i<SendTimes;i++)
	{
		if (i!=(SendTimes-1))                      			//�������һ�η���ʱ
		{	
			length=64;                             			//��16��
		}
		else                                       			//���һ�η���ʱ
		{
			length=LastTimeWord;                   			//��ʣ����䷽��             
		}

		*(U8 *)(AD1+0+ps*i)=0x05;            				//ͷENQ����05
		*(U8 *)(AD1+1+ps*i)=asicc(((plcadd&0xf0)>>4)&0xf);	//plcվ��ַ����λ��ǰ
		*(U8 *)(AD1+2+ps*i)=asicc(plcadd&0xf);
		*(U8 *)(AD1+3+ps*i)=0x77;            				//д�䷽������W
		*(U8 *)(AD1+4+ps*i)=0x53;            				//���������ȷ��͵�����������SB(����)����53 42
		*(U8 *)(AD1+5+ps*i)=0x42; 
		*(U8 *)(AD1+6+ps*i)=0x30;            				//��2�����ȷ��͵��ǵ�ַ�ַ��Ϳ�ʼ��ַ��7������ 
		*(U8 *)(AD1+7+ps*i)=0x37; 
		*(U8 *)(AD1+8+ps*i)=0x25;            				//��3�����ȷ��͵��ǵ�ַ�ַ�%DW       
		*(U8 *)(AD1+9+ps*i)=0x44; 
	    *(U8 *)(AD1+10+ps*i)=0x57;
	    
		b=staradd+i*64;                        				//ÿ��ƫ��16
		a1=(b/1000)&0xf;                       				//ȡǧλ��
		a2=((b-a1*1000)/100)&0xf;             				//ȡ��λ��
		a3=((b-a1*1000-a2*100)/10)&0xf;        				//ȡʮλ��
		a4=(b-a1*1000-a2*100-a3*10)&0xf;       				//ȡ��λ��
			    
		*(U8 *)(AD1+11+ps*i)=asicc(a1);        				//��ʼ�ĵ�ַ��asicc��ʾ����λ�ȷ�
		*(U8 *)(AD1+12+ps*i)=asicc(a2);	
		*(U8 *)(AD1+13+ps*i)=asicc(a3);     
		*(U8 *)(AD1+14+ps*i)=asicc(a4);
		
		b1=(length>>4)&0xf;                    				//д�ĸ�������b1�Ǹ�λ��b2�ǵ�λ
		b2=length&0xf;
		
		*(U8 *)(AD1+15+ps*i)=asicc(b1);        				//���ȣ�asicc��ʾ
		*(U8 *)(AD1+16+ps*i)=asicc(b2);
		
		for(j=0;j<length;j++)
		{
			b3=*(U8 *)(PE+9+i*128+j*2);         				//��������ȡ���ݣ�B3�ǵ�λ��B4�Ǹ�λ
			b4=*(U8 *)(PE+9+i*128+j*2+1);
				
			c1=b3&0xf;               						//����д������asicc����
			c2=(b3>>4)&0xf;          						//����д������asicc����		
			c3=b4&0xf;               						//����д������asicc����
			c4=(b4>>4)&0xf;          						//����д������asicc����				
		
			*(U8 *)(AD1+17+j*4+ps*i)=asicc(c4);   			//��λ�ȷ�
			*(U8 *)(AD1+18+j*4+ps*i)=asicc(c3);
			*(U8 *)(AD1+19+j*4+ps*i)=asicc(c2);
			*(U8 *)(AD1+20+j*4+ps*i)=asicc(c1);				
		}
		*(U8 *)(AD1+17+ps*i+length*4)=0x04;	     			//�����ַ�
		
		aakj=CalcHe((U8 *)(AD1 + ps*i),18+length*4);   			//��У�飬У��ǰ�������
		a1=aakj&0xff;                  						//ֻ�����λ
		//*(U8 *)(AD1+18+length*4+ps*i)=a1;					//16����
		*(U8 *)(AD1+18+length*4+ps*i)=asicc((a1 >> 4) & 0xf);					//16����
		*(U8 *)(AD1+19+length*4+ps*i)=asicc((a1 >> 0) & 0xf);					//16����

		ptcom->send_length[i]=20+length*4;					//���ͳ���
		ptcom->send_staradd[i]=i*ps;						//�������ݴ洢��ַ	
		ptcom->return_length[i]=9;							//�������ݳ���
		ptcom->return_start[i]=0;							//����������Ч��ʼ
		ptcom->return_length_available[i]=0;				//������Ч���ݳ���	
			
	}
	ptcom->send_times=SendTimes;							//���ʹ���
	ptcom->Current_Times=0;									//��ǰ���ʹ���
	ptcom->Simens_Count=0;		
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void compxy(void)//����ɱ�׼�洢��ʽ
{
	int i;
	unsigned char a1,a2,a3,a4;
	int b,b1;
	int t[8];
	if(ptcom->Simens_Count==1)                               //==1ʱ���Է����������ߵ�λ����
	{                                  
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/4;i++)	//���ص����ó���/4������Ҫ�ĳ���
 		 {		
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4);
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+1);
			a3=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+2);
			a4=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+3);	
			a1=bsicc(a1);                           			//ASC�귵�أ�����ҪתΪ16����
			a2=bsicc(a2);
			a3=bsicc(a3);
			a4=bsicc(a4);
			b=(a1<<4)+a2;                          				//���ص�ǰ����
			b1=(a3<<4)+a4;                         				//���صĺ�����
			*(U8 *)(COMad+i*2)=b1;				   				//���´�,�ӵ�0����ʼ�棬�ߵ�λ����
			*(U8 *)(COMad+i*2+1)=b;
		}
	}
	else if (ptcom->Simens_Count==2)
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/4;i++)	//���ص����ó���/4������Ҫ�ĳ���
 		 {		
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+2);
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+3);	
			a1=bsicc(a1);
			a2=bsicc(a2);                       
			t[i]=(a1<<4)+a2;                     
		}
		b=(t[7]<<7)+(t[6]<<6)+(t[5]<<5)+(t[4]<<4)+(t[3]<<3)+(t[2]<<2)+(t[1]<<1)+t[0];
		*(U8 *)(COMad+0)=b;									//���´�,�ӵ�0����ʼ�棬�����ߵ�λ			
	}
	else if (ptcom->Simens_Count==3)                        //�Է����������ߵ�λ����
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/4;i++)	//���ص����ó���/4������Ҫ�ĳ���
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4);
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+1);
			a3=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+2);
			a4=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+3);	
			a1=bsicc(a1);                           			//ASC�귵�أ�����ҪתΪ16����
			a2=bsicc(a2);
			a3=bsicc(a3);
			a4=bsicc(a4);
			b=(a1<<4)+a2;                          				//���ص�ǰ����
			b1=(a3<<4)+a4;                         				//���صĺ�����
			*(U8 *)(COMad+i*2)=b;				   				//���´�,�ӵ�0����ʼ�棬�ߵ�λ����
			*(U8 *)(COMad+i*2+1)=b1;	
		}
	}			
	ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;//���ȼ���	
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void watchcom(void)//�������У��
{
	unsigned int aakj=0;
	aakj=remark();
	if(aakj==1)												//У������ȷ
	{
		ptcom->IfResultCorrect=1;
		compxy();											//�������ݴ������
	}
	else
	{
		ptcom->IfResultCorrect=0;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int asicc(int a)//תΪAsc��
{
	int bl;
	if(a<10)
		bl=a+0x30;
	if(a>9)
		bl=a-10+0x41;
	return bl;	
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int bsicc(int a)//AscתΪ����
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
int remark()				//�����������ݼ���У�����Ƿ���ȷ��LG���ص���û��У�飬�ʲ������Ƿ���ȷ��return 1
{
	return 1;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
U16 CalcHe(unsigned char *chData,U16 uNo)		//�����У��
{
	int i;
	int ab=0;
	for(i=0;i<uNo;i++)
	{
		ab=ab+chData[i];
	}
	return (ab);
}
