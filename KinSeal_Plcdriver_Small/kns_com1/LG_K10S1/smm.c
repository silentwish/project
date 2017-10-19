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
void Set_Reset()//��λ/��λ
{
	U16 aakj;
	int b,t;
	int a1,a2;
	int b1,b2;
	int plcadd;	
	int add;

	b=ptcom->address;								// �Ĵ������Ŀ�ʼ��ַ
	plcadd=ptcom->plc_address;						//PLCվ��ַ
	
	*(U8 *)(AD1+0)=0x05;        					//�����ź�0x05����ҪPLC����06

	*(U8 *)(AD1+1)=0x02;        					//�̶�02
	if (ptcom->writeValue==1)						//��λʱ����o����6f
	{
		*(U8 *)(AD1+2)=0x6f;
	}
	if (ptcom->writeValue==0)						//��λʱ����p����70
	{
		*(U8 *)(AD1+2)=0x70;
	}
	
	*(U8 *)(AD1+3)=0x46;							//�̶�FB 
	*(U8 *)(AD1+4)=0x42;

	switch (ptcom->registerr)						//���ݼĴ������ͻ�ȡƫ�Ƶ�ַ
	{
	case 'Y':
		t=0xa0;		
		break;
				
	case 'M':
		t=0x80;	
		break;	
		
	case 'L':
		t=0xb4;
		break;
		
	case 'K':
		t=0xa4;
		break;
		
	case 'T':
		t=0xe4;
		break;	
		
	case 'C':
		t=0xea;
		break;								
	}
	
	add=b/8;										//���ֽ�Ϊ��λ��λ���߸�λ
	add=add+t;
	a1=(add>>4)&0xf;
	a2=add&0xf;
	*(U8 *)(AD1+5)=asicc(a1);						//ת�����ַ��λ
	*(U8 *)(AD1+6)=asicc(a2);			

	if (ptcom->writeValue==1)						//���ֽ�Ϊ��λ��ȷ��Ҫ��λ�ĵ�ַ��8λ�е���һλ�����λΪ1����λΪ0
	{
		b1=1;
		b2=b%8;					
		b1=(b1<<b2)&0xff;
		a1=(b1>>4)&0xf;
		a2=b1&0xf;
		*(U8 *)(AD1+7)=asicc(a1);
		*(U8 *)(AD1+8)=asicc(a2);			
	}
	if (ptcom->writeValue==0)						//���ֽ�Ϊ��λ��ȷ��Ҫ��λ�ĵ�ַ��8λ�е���һ�������λΪ0����λΪ1
	{
		b1=1;
		b2=b%8;
		b1=~(b1<<b2)&0xff;
		a1=(b1>>4)&0xf;
		a2=b1&0xf;
		*(U8 *)(AD1+7)=asicc(a1);
		*(U8 *)(AD1+8)=asicc(a2);	
	}
		
	*(U8 *)(AD1+9)=0x04;       						//������ 04
	
	aakj=CalcHe((U8 *)(AD1+2),8);					//��У��
	a1=(aakj>>4)&0xf;                 
	a2=aakj&0xf;
	*(U8 *)(AD1+10)=asicc(a1);         
	*(U8 *)(AD1+11)=asicc(a2);        
	
	ptcom->send_length[0]=1;						//���ͳ���
	ptcom->send_staradd[0]=0;						//�������ݴ洢��ַ		
	ptcom->return_length[0]=1;						//�������ݳ���
	ptcom->return_start[0]=0;						//����������Ч��ʼ
	ptcom->return_length_available[0]=0;			//������Ч���ݳ���	
	
	ptcom->send_length[1]=11;						//���ͳ���
	ptcom->send_staradd[1]=1;						//�������ݴ洢��ַ		
	ptcom->return_length[1]=6;						//�������ݳ���
	ptcom->return_start[1]=0;						//����������Ч��ʼ
	ptcom->return_length_available[1]=0;			//������Ч���ݳ���
			
	ptcom->Current_Times=0;							//��ǰ���ʹ���			
	ptcom->send_times=2;							//���ʹ���	
	
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Read_Bool()//��ȡ����������Ϣ
{
	U16 aakj;
	int b,t;
	int add;
	int a1,a2;
	int length;
	int plcadd;
	
	b=ptcom->address;			    				// �Ĵ���������ַ
	plcadd=ptcom->plc_address;	    				//PLCվ��ַ
	length=ptcom->register_length;  				//�����ĳ���
	

	*(U8 *)(AD1+0)=0x05;        					//�����ź�0x05����ҪPLC����06

	*(U8 *)(AD1+1)=0x02;        					//�̶�
	*(U8 *)(AD1+2)=0x67;							//��λʱ��g����67
	*(U8 *)(AD1+3)=0x46;							//�̶�FB 
	*(U8 *)(AD1+4)=0x42;
	
	switch (ptcom->registerr)						//���ݼĴ������ͻ�ȡƫ�Ƶ�ַ
	{
	case 'Y':
		t=0xa0;		
		break;
				
	case 'M':
		t=0x80;	
		break;	
		
	case 'L':
		t=0xb4;
		break;
		
	case 'K':
		t=0xa4;
		break;
		
	case 'T':
		t=0xe4;
		break;	
		
	case 'C':
		t=0xea;
		break;								
	}
	
	add=b/8;										//���ֽ�Ϊ��λ��ȡ��1������Ϊ1���ֽ�
	add=add+t;
	a1=(add>>4)&0xf;
	a2=add&0xf;
	*(U8 *)(AD1+5)=asicc(a1);
	*(U8 *)(AD1+6)=asicc(a2);		
	
	a1=(length>>4)&0xf;								//����
	a2=length&0xf;
	
	*(U8 *)(AD1+7)=asicc(a1);
	*(U8 *)(AD1+8)=asicc(a2);	
	
	*(U8 *)(AD1+9)=0x04;       						//������ 04
	
	aakj=CalcHe((U8 *)(AD1+2),8);  					//��У��
	a1=(aakj>>4)&0xf;                 
	a2=aakj&0xf;
	*(U8 *)(AD1+10)=asicc(a1);        
	*(U8 *)(AD1+11)=asicc(a2);         
	
	ptcom->send_length[0]=1;						//���ͳ���
	ptcom->send_staradd[0]=0;						//�������ݴ洢��ַ	
	ptcom->return_length[0]=1;              		//�������ݳ���
	ptcom->return_start[0]=0;						//����������Ч��ʼ
	ptcom->return_length_available[0]=0 ;			//������Ч���ݳ���
		
	ptcom->send_length[1]=11;						//���ͳ���
	ptcom->send_staradd[1]=1;						//�������ݴ洢��ַ	
	ptcom->return_length[1]=5+length*2;     		//�������ݳ���                                                
	ptcom->return_start[1]=2;						//����������Ч��ʼ
	ptcom->return_length_available[1]=length*2 ;	//������Ч���ݳ���
		
	ptcom->send_times=2;							//���ʹ���	
	ptcom->send_add[0]=ptcom->address;				//�����������ַ������
	ptcom->send_add[1]=ptcom->address;				//�����������ַ������
	ptcom->Current_Times=0;							//��ǰ���ʹ���   	
	ptcom->Simens_Count=1; 							//ȫ�ֱ�����������ֵ���ش���ͬ
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Read_Analog()//��ģ����
{
	U16 aakj;
	int add;
	int a1,a2;
	int t;
	int plcadd;
	int length;
	
	plcadd=ptcom->plc_address;	    				//PLCվ��ַ
	add=ptcom->address;			    				// �Ĵ���������ַ
	length=ptcom->register_length;  				//���ĳ���
	     

	*(U8 *)(AD1+0)=0x05;        					//�����ź�0x05����ҪPLC����06

	*(U8 *)(AD1+1)=0x02;        					//�̶�
	*(U8 *)(AD1+2)=0x67;							//��ʱ��g����67
	*(U8 *)(AD1+3)=0x46; 							//�̶�FC
	*(U8 *)(AD1+4)=0x43;
	
	switch (ptcom->registerr)	    				//���ݼĴ������ͻ�ȡƫ�Ƶ�ַ
	{
	case 'D': 
		t=0x80;		
		break;	
			
	case 't':
		t=0x00;	 	                   
		break;
			
	case 'c': 
		t=0x60;	
		break;						
	}
	
	add=add*2+t;									//�Ĵ����ǰ��ֽ����У���ȡʱ���ֽڶ�ȡ���̶���ʱ��ַ��*2
	a1=(add>>4)&0xf;
	a2=add&0xf;
	*(U8 *)(AD1+5)=asicc(a1);						//��ȡ��ַ
	*(U8 *)(AD1+6)=asicc(a2);	
	
	a1=((length*2)>>4)&0xf;							//��ȡ����Ҳ��Ӧ*2
	a2=(length*2)&0xf;
	
	*(U8 *)(AD1+7)=asicc(a1);
	*(U8 *)(AD1+8)=asicc(a2);	
	
	*(U8 *)(AD1+9)=0x04;       						//������ 04
	
	aakj=CalcHe((U8 *)(AD1+2),8);  					//��У��
	a1=(aakj>>4)&0xf;               
	a2=aakj&0xf;
	*(U8 *)(AD1+10)=asicc(a1);         				
	*(U8 *)(AD1+11)=asicc(a2);         	

	ptcom->send_length[0]=1;						//���ͳ���
	ptcom->send_staradd[0]=0;						//�������ݴ洢��ַ	
	ptcom->return_length[0]=1;              		//�������ݳ���
	ptcom->return_start[0]=0;						//����������Ч��ʼ
	ptcom->return_length_available[0]=0 ;			//������Ч���ݳ���
		
	ptcom->send_length[1]=11;						//���ͳ���
	ptcom->send_staradd[1]=1;						//�������ݴ洢��ַ	
	ptcom->return_length[1]=5+length*4;     		//�������ݳ���                                                
	ptcom->return_start[1]=2;						//����������Ч��ʼ
	ptcom->return_length_available[1]=length*4 ;	//������Ч���ݳ���
		
	ptcom->send_times=2;							//���ʹ���	
	ptcom->send_add[0]=ptcom->address;				//�����������ַ������
	ptcom->send_add[1]=ptcom->address;				//�����������ַ������
	ptcom->Current_Times=0;							//��ǰ���ʹ���   	
	ptcom->Simens_Count=2; 							//ȫ�ֱ�����������ֵ���ش���ͬ
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Write_Analog()//дģ����
{
	U16 aakj;
	int add,i;
	int a1,a2;
	int length;
	int plcadd;
	int t;
	int c,c0,c1,c2,c3,c4;
			
	add=ptcom->address;			    				//��ʼ��ַ
	plcadd=ptcom->plc_address;	    				//PLCվ��ַ
	length=ptcom->register_length;  				//д���䷽��
	
	*(U8 *)(AD1+0)=0x05;        					//�����ź�0x05����ҪPLC����06

	*(U8 *)(AD1+1)=0x02;        					//�̶�
	*(U8 *)(AD1+2)=0x77;							//дʱ��w,��77
	*(U8 *)(AD1+3)=0x46; 
	*(U8 *)(AD1+4)=0x43;							//�̶�FC
	
	switch (ptcom->registerr)	    				//���ݼĴ������ͻ�ȡƫ�Ƶ�ַ
	{
	case 'D': 
		t=0x80;		
		break;	
			
	case 't':
		t=0x00;	 	                   
		break;
			
	case 'c': 
		t=0x60;	
		break;						
	}
	
	add=add*2+t;									//�Ĵ����ǰ��ֽ����У���ȡʱ���ֽڶ�ȡ���̶���ʱ��ַ��*2
	a1=(add>>4)&0xf;
	a2=add&0xf;
	*(U8 *)(AD1+5)=asicc(a1);
	*(U8 *)(AD1+6)=asicc(a2);	
	
	a1=((length*2)>>4)&0xf;							//��ȡ����Ҳ��Ӧ*2
	a2=(length*2)&0xf;
	
	*(U8 *)(AD1+7)=asicc(a1);
	*(U8 *)(AD1+8)=asicc(a2);
	
	for(i=0;i<length;i++)
	{			
		c=ptcom->U8_Data[i*2];   					//��D[]������Ҫ���ݣ���Ӧ��c0Ϊ��λ��cΪ��λ
		c0=ptcom->U8_Data[i*2+1];
							
		c1=c&0xf;                					//����д������asicc����cΪ��λ
		c2=(c>>4)&0xf;           					//����д������asicc����cΪ��λ		
		c3=c0&0xf;               					//����д������asicc����c1Ϊ��λ
		c4=(c0>>4)&0xf;          					//����д������asicc����c1Ϊ��λ				
		
		*(U8 *)(AD1+9+i*4)=asicc(c2);        		//�����ǵ�λ�ȷ�
		*(U8 *)(AD1+10+i*4)=asicc(c1);
		*(U8 *)(AD1+11+i*4)=asicc(c4);
		*(U8 *)(AD1+12+i*4)=asicc(c3);	
	}
	
	*(U8 *)(AD1+9+length*4)=0x04;        			//�����ַ�
	
	aakj=CalcHe((U8 *)(AD1+2),8+length*4);  		//��У��
	a1=(aakj>>4)&0xf;                 
	a2=aakj&0xf;
	*(U8 *)(AD1+10+length*4)=asicc(a1);         
	*(U8 *)(AD1+11+length*4)=asicc(a2);        

	ptcom->send_length[0]=1;						//���ͳ���
	ptcom->send_staradd[0]=0;						//�������ݴ洢��ַ	
	ptcom->return_length[0]=1;              		//�������ݳ���
	ptcom->return_start[0]=0;						//����������Ч��ʼ
	ptcom->return_length_available[0]=0 ;			//������Ч���ݳ���
		
	ptcom->send_length[1]=11+length*4;				//���ͳ���
	ptcom->send_staradd[1]=1;						//�������ݴ洢��ַ	
	ptcom->return_length[1]=6;     					//�������ݳ���                                                
	ptcom->return_start[1]=.0;						//����������Ч��ʼ
	ptcom->return_length_available[1]=0 ;			//������Ч���ݳ���
		
	ptcom->send_times=2;							//���ʹ���
	ptcom->Current_Times=0;							//��ǰ���ʹ���		
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Read_Recipe()//��ȡ�䷽
{
	U16 aakj;
	int add;
	int a1,a2;
	int i;
	int datalength;
	int p_start;
	int ps;
	int SendTimes;
	int LastTimeWord;								//���һ�η��ͳ���
	int currentlength;
	int plcadd;
	
		
	datalength=ptcom->register_length;				//�����ܳ���
	p_start=ptcom->address;							//��ʼ��ַ
	plcadd=ptcom->plc_address;	            		//PLCվ��ַ
	
	if(datalength>5000)                     		//���Ƴ���
		datalength=5000;

	if(datalength%16==0)                    		//ÿ�η�16���䷽�������䷽���պ�Ϊ16�ı���ʱ
	{
		SendTimes=datalength/16;            		//���͵Ĵ���            
		LastTimeWord=16;                    		//���һ�η��͵ĳ���Ϊ16	
	}
	if(datalength%16!=0)                    		//ÿ�η�16���䷽�������䷽������16�ı���ʱ
	{
		SendTimes=datalength/16+1;          		//���͵Ĵ���
		LastTimeWord=datalength%16;         		//���һ�η��͵ĳ���Ϊ��16������	
	}
	
	for (i=0;i<SendTimes;i++)               		//С�ڷ��ʹ�����������
	{
		ps=12*i;                            		//ÿ�η�12�����ȣ��ڶ��η�����12��ʼ
		add=p_start+i*16;                     		//ÿ�εĿ�ʼ��ַ

		*(U8 *)(AD1+0+ps)=0x05;        				//�����ź�0x05����ҪPLC����06

		*(U8 *)(AD1+1+ps)=0x02;        				//�̶�
		*(U8 *)(AD1+2+ps)=0x67;						//��ʱ��g����67
		*(U8 *)(AD1+3+ps)=0x46; 					//�̶�FC
		*(U8 *)(AD1+4+ps)=0x43;
			
		add=add*2+0x80;								//�Ĵ����ǰ��ֽ����У���ȡʱ���ֽڶ�ȡ���̶���ʱ��ַ��*2����+ƫ�Ƶ�ַ
		a1=(add>>4)&0xf;
		a2=add&0xf;
		*(U8 *)(AD1+5+ps)=asicc(a1);
		*(U8 *)(AD1+6+ps)=asicc(a2);	
		
		if (i!=(SendTimes-1))	          			//�������һ�η���ʱ
		{
			*(U8 *)(AD1+7+ps)=0x32;      			//�̶�����32���ֽڣ���0x20��ת��asicc
			*(U8 *)(AD1+8+ps)=0x30;
			currentlength=16;             			//���͵����ݳ���
		}
		if (i==(SendTimes-1))	          			//���һ�η���ʱ
		{
			currentlength=LastTimeWord;   			//���͵����ݳ���
			*(U8 *)(AD1+7+ps)=asicc(((currentlength*2)>>4)&0xf);//���͵ĳ���Ϊ��16������LastTimeWord
			*(U8 *)(AD1+8+ps)=asicc((currentlength*2)&0xf);
		}	
			
		*(U8 *)(AD1+9+ps)=0x04;          			//�����ַ�
		
		aakj=CalcHe((U8 *)(AD1+2+ps),8);  			//��У��
		a1=(aakj>>4)&0xf;                 
		a2=aakj&0xf;
		*(U8 *)(AD1+10+ps)=asicc(a1);        
		*(U8 *)(AD1+11+ps)=asicc(a2);         	
		
		ptcom->send_length[i*2]=1;					//���ͳ���
		ptcom->send_staradd[i*2]=ps;				//�������ݴ洢��ַ	
		ptcom->return_length[i*2]=1;              	//�������ݳ���
		ptcom->return_start[i*2]=0;					//����������Ч��ʼ
		ptcom->return_length_available[i*2]=0;		//������Ч���ݳ���
			
		ptcom->send_length[i*2+1]=11;				//���ͳ���
		ptcom->send_staradd[i*2+1]=1+ps;			//�������ݴ洢��ַ	
		ptcom->return_length[i*2+1]=5+currentlength*4;//�������ݳ���                                                
		ptcom->return_start[i*2+1]=2;				//����������Ч��ʼ
		ptcom->return_length_available[i*2+1]=currentlength*4;//������Ч���ݳ���
			
		ptcom->send_add[i*2+1]=ptcom->address+i*16;	//�����������ַ������
		ptcom->send_data_length[1+i*2]=currentlength;//�������һ�ζ���32��D  	
		ptcom->Simens_Count=2; 						//ȫ�ֱ�����������ֵ���ش���ͬ			
	}
	ptcom->send_times=SendTimes*2;					//���ʹ���
	ptcom->Current_Times=0;					        //��ǰ���ʹ���	
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Write_Recipe()//д�䷽��PLC
{
	int datalength;
	int staradd;
	int SendTimes;
	int LastTimeWord;								//���һ�η��ͳ���
	int i,j;
	int ps;
	int a1,a2;
	int b3,b4;
	int c1,c2,c3,c4;
	U16 aakj;
	int plcadd;
	int length;
	int add;
		
	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));//���ݳ���
	staradd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//���ݳ���
	plcadd=ptcom->plc_address;	                   	//PLCվ��ַ

	if(datalength%16==0)                           	//д���䷽���պ���16�ı���ʱ             
	{
		SendTimes=datalength/16;                   	//���͵Ĵ���
		LastTimeWord=16;                           	//���һ�η��͵ĳ���	
	}
	if(datalength%16!=0)                           	//����16�ı���ʱ
	{
		SendTimes=datalength/16+1;                 	//���͵Ĵ���
		LastTimeWord=datalength%16;                	//���һ�η��͵ĳ���	
	}	
	
	ps=76;                                         	//д16���䷽ʱ�����͵���76������
	for (i=0;i<SendTimes;i++)
	{
		if (i!=(SendTimes-1))                      	//�������һ�η���ʱ
		{	
			length=16;                             	//��16��
		}
		else                                       	//���һ�η���ʱ
		{
			length=LastTimeWord;                   	//��ʣ����䷽��             
		}

		*(U8 *)(AD1+0+ps*i)=0x05;        			//�����ź�0x05����ҪPLC����06

		*(U8 *)(AD1+1+ps*i)=0x02;        			//�̶�
		*(U8 *)(AD1+2+ps*i)=0x77;					//дʱ��w����77
		*(U8 *)(AD1+3+ps*i)=0x46;					//�̶�FC 
		*(U8 *)(AD1+4+ps*i)=0x43;
		
		add=staradd+i*16;							//�Ĵ����ǰ��ֽ����У���ȡʱ���ֽڶ�ȡ���̶���ʱ��ַ��*2����+ƫ�Ƶ�ַ    
		add=add*2+0x80;
		a1=(add>>4)&0xf;
		a2=add&0xf;
		*(U8 *)(AD1+5+ps*i)=asicc(a1);
		*(U8 *)(AD1+6+ps*i)=asicc(a2);	
		
		a1=((length*2)>>4)&0xf;						//����Ҳ��Ӧ��*2
		a2=(length*2)&0xf;
		
		*(U8 *)(AD1+7+ps*i)=asicc(a1);
		*(U8 *)(AD1+8+ps*i)=asicc(a2);
		
		for(j=0;j<length;j++)
		{
			b3=*(U8 *)(PE+9+i*32+j*2);         		//��������ȡ���ݣ�B3�ǵ�λ��B4�Ǹ�λ
			b4=*(U8 *)(PE+9+i*32+j*2+1);
				
			c1=b3&0xf;               				//����д������asicc����
			c2=(b3>>4)&0xf;          				//����д������asicc����		
			c3=b4&0xf;               				//����д������asicc����
			c4=(b4>>4)&0xf;          				//����д������asicc����				
		
			*(U8 *)(AD1+9+j*4+ps*i)=asicc(c2);   	//��λ�ȷ�
			*(U8 *)(AD1+10+j*4+ps*i)=asicc(c1);
			*(U8 *)(AD1+11+j*4+ps*i)=asicc(c4);
			*(U8 *)(AD1+12+j*4+ps*i)=asicc(c3);				
		}
		*(U8 *)(AD1+9+ps*i+length*4)=0x04;	     	//�����ַ�
		
		aakj=CalcHe((U8 *)(AD1+2+ps*i),8+length*4); //��У��
		a1=(aakj>>4)&0xf;                 
		a2=aakj&0xf;
		*(U8 *)(AD1+10+length*4+ps*i)=asicc(a1);         
		*(U8 *)(AD1+11+length*4+ps*i)=asicc(a2);         	

		ptcom->send_length[i*2]=1;					//���ͳ���
		ptcom->send_staradd[i*2]=ps*i;				//�������ݴ洢��ַ	
		ptcom->return_length[i*2]=1;              	//�������ݳ���
		ptcom->return_start[i*2]=0;					//����������Ч��ʼ
		ptcom->return_length_available[i*2]=0 ;		//������Ч���ݳ���
			
		ptcom->send_length[i*2+1]=11+length*4;		//���ͳ���
		ptcom->send_staradd[i*2+1]=1+ps*i;			//�������ݴ洢��ַ	
		ptcom->return_length[i*2+1]=6;     			//�������ݳ���                                                
		ptcom->return_start[i*2+1]=0;				//����������Ч��ʼ
		ptcom->return_length_available[i*2+1]=0 ;	//������Ч���ݳ���
			
	}
	ptcom->send_times=SendTimes*2;					//���ʹ���
	ptcom->Current_Times=0;							//��ǰ���ʹ���		
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
void compxy(void)//����ɱ�׼�洢��ʽ
{
	int i;
	unsigned char a1,a2,a3,a4;
	int b,b1;

	if(ptcom->Simens_Count==1)                               
	{                                 
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/2;i++)	//���ص����ó���/2������Ҫ�ĳ���
 		{		
  		    a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2);
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2+1);
			a1=bsicc(a1);
			a2=bsicc(a2);
			b=(a1<<4)+a2;                      
			*(U8 *)(COMad+i)=b;	
		}
	}
	if(ptcom->Simens_Count==2)                                   
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/4;i++)	//���ص����ó���/4������Ҫ�ĳ���
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4);
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+1);
			a3=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+2);
			a4=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4+3);	
			a1=bsicc(a1);                           							//ASC�귵�أ�����ҪתΪ16����
			a2=bsicc(a2);
			a3=bsicc(a3);
			a4=bsicc(a4);
			b=(a1<<4)+a2;                          
			b1=(a3<<4)+a4;                         
			*(U8 *)(COMad+i*2)=b1;				   								//���´�,�ӵ�0����ʼ�棬�ߵ�λ����
			*(U8 *)(COMad+i*2+1)=b;	
		}
	}			
	ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;	//���ȼ���	
	
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void watchcom(void)//�������У��
{
	unsigned int aakj=0;
	aakj=remark();
	if(aakj==1)										//У������ȷ
	{
		ptcom->IfResultCorrect=1;
		compxy();									//�������ݴ������
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
int remark()//�����������ݼ���У�����Ƿ���ȷ��LG���ص���û��У�飬�ʲ������Ƿ���ȷ��return 1
{
	if(ptcom->return_length[ptcom->Current_Times-1]==1)//��ǰ���ʹ�������1�Ļ������ؼ���ȡ��
	{
		return 0;
	}
	else 
	{
		return 1;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
U16 CalcHe(unsigned char *chData,U16 uNo)//�����У��
{
	int i;
	int ab=0;
	for(i=0;i<uNo;i++)
	{
		ab=ab+chData[i];
	}
	return (ab);
}
