/************************************************************************
*  File name:     XGB_CNET
*  Default parameters: 9600 8 ONE 1
*  Description:   �������������¼Ĵ�����
*
*  ����		��ʽ		��Χ		��ע					��Ӧ����
*  P		dddh		0~255f		λ������/�������		Y
*  M		dddh		0~255f		λ�������̵���			M
*  K		dddh		0~255f		λ�������̵���			K
*  L		dddh		0~255f		λ�����Ӽ̵���			L
*  F		dddh		0~255f		λ������̵���			H
*  T		ddd			0~255		λ����ʱ���̵���		T
*  C		ddd			0~255		λ���������̵���		C
*  D		dddd		0~9999		�֣����ݼĴ���			D
*  S		dddd		0~9999		�֣����ݼĴ���			R
*  TV		ddd			0~255		�֣���ʱ����ǰֵ		t
*  CV		ddd			0~255		�֣���������ǰֵ		c
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
 * Funciton: ��������������ռ��������ݽӿں���
 * Parameters: ��
 * Return: ��
**************************************************************************/
void Enter_PlcDriver(void)
{
	ptcom=(struct Com_struct_D *)adcom;	
	switch (ptcom->R_W_Flag)
	{
	case PLC_READ_DATA:							//���������Ƕ�����
	case PLC_READ_DIGITALALARM:						//���������Ƕ�����,����	
	case PLC_READ_TREND:						//���������Ƕ�����,����ͼ
	case PLC_READ_ANALOGALARM:						//���������Ƕ�����,��ȱ���	
	case PLC_READ_CONTROL:						//���������Ƕ�PLC���Ƶ�����	
		switch(ptcom->registerr)
		{
		case 'Y':					//P
		case 'M':					//M
		case 'K':					//K
		case 'L':					//L
		case 'H':					//F
		case 'T':					//T
		case 'C':					//C						
			Read_Bool();				//��������
			break;
		case 'D':					//D
		case 'R':					//S
		case 't':					//TV
		case 'c':					//CV	
			Read_Analog();			//��ģ����
			break;			
		}
		break;
	case PLC_WRITE_DATA:							//����������д����
		switch(ptcom->registerr)
		{
		case 'Y':					//P
		case 'M':					//M
		case 'K':					//K
		case 'L':					//L
		//case 'H':					//F
		case 'T':					//T
		case 'C':					//C					
			Set_Reset();			//��λ��λ
			break;
		case 'D':					//D
		case 'R':					//S
		case 't':					//TV
		case 'c':					//CV
			Write_Analog();			//дģ����		
			break;			
		}
		break;	
	case PLC_WRITE_TIME:							//����������дʱ�䵽PLC
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
	case PLC_READ_TIME:							//���������Ƕ�ȡʱ�䵽PLC
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
	case PLC_WRITE_RECIPE:							//����������д�䷽��PLC
		switch(*(U8 *)(PE+3))		//�䷽�Ĵ�������
		{
		case 'D':					//D
		case 'R':					//S
			Write_Recipe();		
			break;			
		}
		break;
	case PLC_READ_RECIPE:							//���������Ǵ�PLC��ȡ�䷽
		switch(*(U8 *)(PE+3))		//�䷽�Ĵ�������
		{
		case 'D':					//D
		case 'R':					//S
			Read_Recipe();		
			break;			
		}
		break;							
	case PLC_CHECK_DATA:							//�������������ݴ���
		watchcom();
		break;				
	}	 
}

/*************************************************************************
 * Funciton: ������д��
 * Parameters: ��
 * Return: ��
**************************************************************************/
void Set_Reset()
{
	int iSendAdd,iWord,iBit;
	int plcadd;	
	int iTemp1,iTemp2,iTemp3,iTemp4,iTemp5;

	iSendAdd=ptcom->address;								//�Ĵ������Ŀ�ʼ��ַ
	plcadd=ptcom->plc_address;								//PLCվ��ַ
	
	*(U8 *)(AD1+0)=0x05;        							//ͷENQ����05
	
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);  			//plcվ��ַ����λ��ǰ
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);
	
	*(U8 *)(AD1+3)=0x57;        							//W
	*(U8 *)(AD1+4)=0x53;        							//S
	*(U8 *)(AD1+5)=0x53;        							//S 
	
    *(U8 *)(AD1+6)=0x30;        							//BLOCK NO.
	*(U8 *)(AD1+7)=0x31;		

	*(U8 *)(AD1+8)=0x30;        							//��ַ�ַ��Ϳ�ʼ��ַ��9������       
	*(U8 *)(AD1+9)=0x39;
	
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
		
	case 'K':
		*(U8 *)(AD1+10)=0x25;       						//K����%KX����25 4C 58
		*(U8 *)(AD1+11)=0x4B;
		*(U8 *)(AD1+12)=0x58;
		break;
		
	case 'L':
		*(U8 *)(AD1+10)=0x25;       						//L����%LX����25 4B 58
		*(U8 *)(AD1+11)=0x4C;
		*(U8 *)(AD1+12)=0x58;
		break;
		
	case 'H':
		*(U8 *)(AD1+10)=0x25;       						//F����%FX����25 4B 58
		*(U8 *)(AD1+11)=0x46;
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
	
	if (ptcom->registerr == 'T' || ptcom->registerr == 'C')
	{
		*(U8 *)(AD1+13)=0x30;
	
		iTemp1=iSendAdd/10000;								//���һ���ֽ���ʮ�����Ʒ��ͣ���Ҫת��ASC��
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

		iTemp1=iWord/10000;									//���һ���ֽ���ʮ�����Ʒ��ͣ���Ҫת��ASC��
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

	if (ptcom->writeValue==1)								//��λʱ����30 31
	{
		*(U8 *)(AD1+19)=0x30;
		*(U8 *)(AD1+20)=0x31;
	}
	if (ptcom->writeValue==0)								//��λʱ����30 30
	{
		*(U8 *)(AD1+19)=0x30;
		*(U8 *)(AD1+20)=0x30;
	}
	
	*(U8 *)(AD1+21)=0x04;       							//������ 04
	
	ptcom->send_length[0]=22;								//���ͳ���PΪ22��
	ptcom->send_staradd[0]=0;								//�������ݴ洢��ַ	
	ptcom->send_times=1;									//���ʹ���
		
	ptcom->return_length[0]=7;								//�������ݳ���
	ptcom->return_start[0]=0;								//����������Ч��ʼ
	ptcom->return_length_available[0]=0;					//������Ч���ݳ���	
	ptcom->Current_Times=0;									//��ǰ���ʹ���			
}

/*************************************************************************
 * Funciton: ��������ȡ
 * Parameters: ��
 * Return: ��
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
	
	b=ptcom->address;			    						//�Ĵ���������ַ
	plcadd=ptcom->plc_address;	    						//PLCվ��ַ
	length=ptcom->register_length;  						//�����ĳ���

	switch (ptcom->registerr)
	{
	case 'Y':
	case 'M':
	case 'K':
	case 'L': 
	case 'H':
		*(U8 *)(AD1+0)=0x05;            					//ͷENQ����05
		
		*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);  		//plcվ��ַ����λ��ǰ
		*(U8 *)(AD1+2)=asicc(plcadd&0xf);
		
		*(U8 *)(AD1+3)=0x52;            					//R
		*(U8 *)(AD1+4)=0x53;            					//S
		*(U8 *)(AD1+5)=0x42;            					//B 
		
		*(U8 *)(AD1+6)=0x30;            					//2�����ȷ��͵��ǵ�ַ�ַ����� 
		*(U8 *)(AD1+7)=0x38;		
				
		switch (ptcom->registerr)	    					//���ݼĴ��������в�ͬ�ĵ�ַ�ַ�
		{
			case 'Y': 
				*(U8 *)(AD1+8)=0x25;        				//��3�����ȷ��͵��ǵ�ַ�ַ�%PW       
				*(U8 *)(AD1+9)=0x50; 
			    *(U8 *)(AD1+10)=0x57;   
				break;	
					
			case 'M':		
				*(U8 *)(AD1+8)=0x25;        				//��3�����ȷ��͵��ǵ�ַ�ַ�%MW       
				*(U8 *)(AD1+9)=0x4D; 
			    *(U8 *)(AD1+10)=0x57;         
				break;
					
			case 'K': 
				*(U8 *)(AD1+8)=0x25;        				//��3�����ȷ��͵��ǵ�ַ�ַ�%KW       
				*(U8 *)(AD1+9)=0x4B; 
			    *(U8 *)(AD1+10)=0x57;
				break;
				
			case 'L': 
				*(U8 *)(AD1+8)=0x25;        				//��3�����ȷ��͵��ǵ�ַ�ַ�%LW       
				*(U8 *)(AD1+9)=0x4C; 
			    *(U8 *)(AD1+10)=0x57;
				break;		
				
			case 'H': 
				*(U8 *)(AD1+8)=0x25;        				//��3�����ȷ��͵��ǵ�ַ�ַ�%FW       
				*(U8 *)(AD1+9)=0x46; 
			    *(U8 *)(AD1+10)=0x57;
				break;						
		}

		b=b/16;												//��ַת������ַռ5���ֽڣ��ɸߵ��ͣ�ǰ4���ֽ���ʮ���Ʒ��� 
		a1=b/10000;											//���һ���ֽ���ʮ�����Ʒ��ͣ���Ҫת��ASC��
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
		b1=(length>>4)&0xf;             					//���ĳ��ȴ���b1��λ��b2��λ
		b2=length&0xf;
		*(U8 *)(AD1+16)=asicc(b1);      					//���ĳ��ȣ�asicc��ʾ
		*(U8 *)(AD1+17)=asicc(b2);
			
		*(U8 *)(AD1+18)=0x04;           					//�����ַ�

		ptcom->send_length[0]=19;							//���ͳ���
		ptcom->send_staradd[0]=0;							//�������ݴ洢��ַ	
							
		ptcom->return_length[0]=11+length*4;				//�������ݳ��ȣ���11���̶�
		ptcom->return_start[0]=10;							//����������Ч��ʼ����11��
		ptcom->return_length_available[0]=length*4;			//������Ч���ݳ���	
		
		ptcom->address = ptcom->address/16*16;	    		//PLC��ַ
		ptcom->register_length = length*2;  				//�����ĳ���			
		ptcom->send_add[0]=ptcom->address;					//�����������ַ������		
		
		ptcom->Current_Times=0;								//��ǰ���ʹ���
		ptcom->send_times=1;								//���ʹ���
		
		ptcom->Simens_Count=1; 								//ȫ�ֱ�����������ֵ0��1�Է��ش���ͬ
		break;
	case 'T':
	case 'C':
		for (i=0;i<length;i++)
		{
			ps=81*i;	
			*(U8 *)(AD1+0+ps)=0x05;            				//ͷENQ����05
			
			*(U8 *)(AD1+1+ps)=asicc(((plcadd&0xf0)>>4)&0xf);//plcվ��ַ����λ��ǰ
			*(U8 *)(AD1+2+ps)=asicc(plcadd&0xf);
			
			*(U8 *)(AD1+3+ps)=0x52;            				//R
			*(U8 *)(AD1+4+ps)=0x53;            				//S
			*(U8 *)(AD1+5+ps)=0x53;            				//S 
			
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
			
				add=b+8*i+j;							//ÿ���ַ+1�����������Ķ�8��
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
					
			ptcom->send_length[i]=81;						//���ͳ���
			ptcom->send_staradd[i]=ps;						//�������ݴ洢��ַ	
									
			ptcom->return_length[i]=41;						//�������ݳ���
			ptcom->return_start[i]=11;						//����������Ч��ʼ
			ptcom->return_length_available[i]=32;			//������Ч���ݳ���				

			ptcom->register_length = 1;		
		}
		ptcom->send_times=length;							//���ʹ���
		ptcom->Current_Times=0;								//��ǰ���ʹ���
		ptcom->Simens_Count=2; 								//ȫ�ֱ�����������ֵ0��1�Է��ش���ͬ
		break; 	
	}
}

/*************************************************************************
 * Funciton: ģ������ȡ
 * Parameters: ��
 * Return: ��
**************************************************************************/
void Read_Analog()
{
	int b;
	int a1,a2,a3,a4,a5;
	int plcadd;
	int length;
	int b1,b2;
	
	plcadd=ptcom->plc_address;	    						//PLCվ��ַ
	b=ptcom->address;			    						//�Ĵ���������ַ
	length=ptcom->register_length;  						//���ĳ���
	     

	*(U8 *)(AD1+0)=0x05;            						//ͷENQ����05
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);        	//plcվ��ַ����λ��ǰ
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);	
	*(U8 *)(AD1+3)=0x52;            						//R
	*(U8 *)(AD1+4)=0x53;            						//S
	*(U8 *)(AD1+5)=0x42;            						//B 
	*(U8 *)(AD1+6)=0x30;            						//��2�����ȷ��͵��ǵ�ַ�ַ��Ϳ�ʼ��ַ��8������ 
	*(U8 *)(AD1+7)=0x38; 
	
	switch (ptcom->registerr)	    						//���ݼĴ��������в�ͬ�ĵ�ַ�ַ�
	{
	case 'D': 
		*(U8 *)(AD1+8)=0x25;        						//��3�����ȷ��͵��ǵ�ַ�ַ�%DW       
		*(U8 *)(AD1+9)=0x44; 
	    *(U8 *)(AD1+10)=0x57;  
		break;	
		
	case 'R': 
		*(U8 *)(AD1+8)=0x25;        						//��3�����ȷ��͵��ǵ�ַ�ַ�%SW       
		*(U8 *)(AD1+9)=0x53; 
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
	
	a1=b/10000;												//���һ���ֽ���ʮ�����Ʒ��ͣ���Ҫת��ASC��
	a2=(b%10000)/1000;
	a3=(b%1000)/100;
	a4=(b%100)/10;	
	a5=b%10;	
	
	*(U8 *)(AD1+11)=asicc(a1);  							//��ʼ�ĵ�ַ��asicc��ʾ����λ�ȷ�
	*(U8 *)(AD1+12)=asicc(a2);	
	*(U8 *)(AD1+13)=asicc(a3);          
	*(U8 *)(AD1+14)=asicc(a4);
	*(U8 *)(AD1+15)=asicc(a5);
	
	b1=(length>>4)&0xf;                 					//���ĳ��ȴ���b1Ϊ��λ,b2Ϊ��λ
	b2=length&0xf;
		
	*(U8 *)(AD1+16)=asicc(b1);          					//���ĳ��ȣ�asicc��ʾ
	*(U8 *)(AD1+17)=asicc(b2);
	
	*(U8 *)(AD1+18)=0x04;               					//�����ַ�
	
	ptcom->send_length[0]=19;								//���ͳ���
	ptcom->send_staradd[0]=0;								//�������ݴ洢��ַ	
	ptcom->send_times=1;									//���ʹ���
		
	ptcom->return_length[0]=11+length*4;					//�������ݳ��ȣ���11���̶�
	ptcom->return_start[0]=10;								//����������Ч��ʼ
	ptcom->return_length_available[0]=length*4;				//������Ч���ݳ���	
	ptcom->Current_Times=0;									//��ǰ���ʹ���	
	ptcom->send_add[0]=ptcom->address;						//�����������ַ������
	ptcom->Simens_Count=3; 
}

/*************************************************************************
 * Funciton: ��ȡ�䷽
 * Parameters: ��
 * Return: ��
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

	if(datalength%16==0)                    				//ÿ�η�16���䷽�������䷽���պ�Ϊ16�ı���ʱ
	{
		SendTimes=datalength/16;            				//���͵Ĵ���            
		LastTimeWord=16;                    				//���һ�η��͵ĳ���Ϊ16	
	}
	if(datalength%16!=0)                    				//ÿ�η�16���䷽�������䷽������16�ı���ʱ
	{
		SendTimes=datalength/16+1;          				//���͵Ĵ���
		LastTimeWord=datalength%16;         				//���һ�η��͵ĳ���Ϊ��16������	
	}
	
	for (i=0;i<SendTimes;i++)               				//С�ڷ��ʹ�����������
	{
		ps=19*i;                            				//ÿ�η�19�����ȣ��ڶ��η�����19��ʼ
		b=p_start+i*16;                     				//ÿ�εĿ�ʼ��ַ
		
		a1=b/10000;											//���һ���ֽ���ʮ�����Ʒ��ͣ���Ҫת��ASC��
		a2=(b%10000)/1000;
		a3=(b%1000)/100;
		a4=(b%100)/10;	
		a5=b%10;	

		*(U8 *)(AD1+0+ps)=0x05;             				//ͷENQ����05
		*(U8 *)(AD1+1+ps)=asicc(((plcadd&0xf0)>>4)&0xf); 	//plcվ��ַ����λ��ǰ
		*(U8 *)(AD1+2+ps)=asicc(plcadd&0xf);
		*(U8 *)(AD1+3+ps)=0x52;            					//���䷽������R
		*(U8 *)(AD1+4+ps)=0x53;            					//���������ȷ��͵�����������SB(����)����53 42
		*(U8 *)(AD1+5+ps)=0x42; 
		*(U8 *)(AD1+6+ps)=0x30;            					//��2�����ȷ��͵��ǵ�ַ�ַ��Ϳ�ʼ��ַ��7������ 
		*(U8 *)(AD1+7+ps)=0x38; 
		*(U8 *)(AD1+8+ps)=0x25;            					//��3�����ȷ��͵��ǵ�ַ�ַ�%DW       
		*(U8 *)(AD1+9+ps)=0x44; 
	    *(U8 *)(AD1+10+ps)=0x57;
			
		*(U8 *)(AD1+11+ps)=asicc(a1);      					//��ʼ�ĵ�ַ��asicc��ʾ����λ�ȷ�
		*(U8 *)(AD1+12+ps)=asicc(a2);	
		*(U8 *)(AD1+13+ps)=asicc(a3);      
		*(U8 *)(AD1+14+ps)=asicc(a4);
		*(U8 *)(AD1+15+ps)=asicc(a5);
		
		if (i!=(SendTimes-1))	          					//�������һ�η���ʱ
		{
			*(U8 *)(AD1+16+ps)=0x31;      					//�̶�����16������0x10��ת��asicc
			*(U8 *)(AD1+17+ps)=0x30;
			currentlength=16;             					//���͵����ݳ���
		}
		if (i==(SendTimes-1))	          					//���һ�η���ʱ
		{
			*(U8 *)(AD1+16+ps)=asicc((LastTimeWord>>4)&0xf);//���͵ĳ���Ϊ��16������LastTimeWord����λ�ȷ�
			*(U8 *)(AD1+17+ps)=asicc(LastTimeWord&0xf);
			currentlength=LastTimeWord;   					//���͵����ݳ���
		}	
			
		*(U8 *)(AD1+18+ps)=0x04;          					//�����ַ�
		
		ptcom->send_length[i]=19;							//���ͳ���
		ptcom->send_staradd[i]=i*19;						//�������ݴ洢��ַ	
		ptcom->send_add[i]=p_start+i*16;					//�����������ַ������	
		ptcom->send_data_length[i]=currentlength;			//�������һ�ζ���16��
				
		ptcom->return_length[i]=11+currentlength*4;			//�������ݳ��ȣ���11���̶�
		ptcom->return_start[i]=10;				    		//����������Ч��ʼ
		ptcom->return_length_available[i]=currentlength*4;	//������Ч���ݳ���		
	}
	ptcom->send_times=SendTimes;							//���ʹ���
	ptcom->Current_Times=0;					        		//��ǰ���ʹ���	
	ptcom->Simens_Count=3;
}

/*************************************************************************
 * Funciton: дģ����
 * Parameters: ��
 * Return: ��
**************************************************************************/
void Write_Analog()
{
	int b,i;
	int a1,a2,a3,a4,a5;
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
	
	*(U8 *)(AD1+3)=0x57;            						//д�䷽������W
	*(U8 *)(AD1+4)=0x53;            						//���������ȷ��͵�����������SB(����)����53 42
	*(U8 *)(AD1+5)=0x42; 
	*(U8 *)(AD1+6)=0x30;            						//��2�����ȷ��͵��ǵ�ַ�ַ��Ϳ�ʼ��ַ��7������ 
	*(U8 *)(AD1+7)=0x38; 
	
	switch (ptcom->registerr)	    						//���ݼĴ��������в�ͬ�ĵ�ַ�ַ�
	{
	case 'D': 
		*(U8 *)(AD1+8)=0x25;        						//��3�����ȷ��͵��ǵ�ַ�ַ�%DW       
		*(U8 *)(AD1+9)=0x44; 
	    *(U8 *)(AD1+10)=0x57;       
		break;	
		
	case 'R': 
		*(U8 *)(AD1+8)=0x25;        						//��3�����ȷ��͵��ǵ�ַ�ַ�%SW       
		*(U8 *)(AD1+9)=0x53; 
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
	
	a1=b/10000;												//���һ���ֽ���ʮ�����Ʒ��ͣ���Ҫת��ASC��
	a2=(b%10000)/1000;
	a3=(b%1000)/100;
	a4=(b%100)/10;	
	a5=b%10;	
	
	*(U8 *)(AD1+11)=asicc(a1);  							//��ʼ�ĵ�ַ��asicc��ʾ����λ�ȷ�
	*(U8 *)(AD1+12)=asicc(a2);	
	*(U8 *)(AD1+13)=asicc(a3);    
	*(U8 *)(AD1+14)=asicc(a4);
	*(U8 *)(AD1+15)=asicc(a5);
	
	b1=(length>>4)&0xf;             						//д���䷽��������λΪb1����λΪb2
	b2=length&0xf;
		
	*(U8 *)(AD1+16)=asicc(b1);       						//���ĳ��ȣ�asicc��ʾ
	*(U8 *)(AD1+17)=asicc(b2);
	
	for(i=0;i<length;i++)
	{			
	c=ptcom->U8_Data[i*2];   								//��D[]������Ҫ���ݣ���Ӧ��c0Ϊ��λ��cΪ��λ
	c0=ptcom->U8_Data[i*2+1];
						
	c1=c&0xf;                								//����д������asicc����cΪ��λ
	c2=(c>>4)&0xf;           								//����д������asicc����cΪ��λ		
	c3=c0&0xf;               								//����д������asicc����c1Ϊ��λ
	c4=(c0>>4)&0xf;          								//����д������asicc����c1Ϊ��λ				
	
	*(U8 *)(AD1+18+i*4)=asicc(c4);        					//�����Ǹ�λ�ȷ�
	*(U8 *)(AD1+19+i*4)=asicc(c3);
	*(U8 *)(AD1+20+i*4)=asicc(c2);
	*(U8 *)(AD1+21+i*4)=asicc(c1);	
	}
	
	*(U8 *)(AD1+18+length*4)=0x04;        					//�����ַ�

	ptcom->send_length[0]=19+length*4;						//���ͳ���
	ptcom->send_staradd[0]=0;								//�������ݴ洢��ַ	
	ptcom->send_times=1;									//���ʹ���
			
	ptcom->return_length[0]=7;								//�������ݳ���
	ptcom->return_start[0]=0;								//����������Ч��ʼ
	ptcom->return_length_available[0]=0;					//������Ч���ݳ���	
	ptcom->Current_Times=0;									//��ǰ���ʹ���
	ptcom->Simens_Count=0;	
}

/*************************************************************************
 * Funciton: д�䷽��PLC
 * Parameters: ��
 * Return: ��
**************************************************************************/
void Write_Recipe()
{
	int datalength;
	int staradd;
	int SendTimes;
	int LastTimeWord;										//���һ�η��ͳ���
	int i,j;
	int ps;
	int b;
	int a1,a2,a3,a4,a5;
	int b1,b2,b3,b4;
	int c1,c2,c3,c4;
	int plcadd;
	int length;
		
	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));		//���ݳ���
	staradd=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//���ݳ���
	plcadd=ptcom->plc_address;	                   			//PLCվ��ַ

	if(datalength%16==0)                           			//д���䷽���պ���16�ı���ʱ             
	{
		SendTimes=datalength/16;                   			//���͵Ĵ���
		LastTimeWord=16;                           			//���һ�η��͵ĳ���	
	}
	if(datalength%16!=0)                           			//����16�ı���ʱ
	{
		SendTimes=datalength/16+1;                 			//���͵Ĵ���
		LastTimeWord=datalength%16;                			//���һ�η��͵ĳ���	
	}	
	
	ps=83;                                         			//д16���䷽ʱ�����͵���83������
	for (i=0;i<SendTimes;i++)
	{
		if (i!=(SendTimes-1))                      			//�������һ�η���ʱ
		{	
			length=16;                             			//��16��
		}
		else                                       			//���һ�η���ʱ
		{
			length=LastTimeWord;                   			//��ʣ����䷽��             
		}

		*(U8 *)(AD1+0+ps*i)=0x05;            				//ͷENQ����05
		*(U8 *)(AD1+1+ps*i)=asicc(((plcadd&0xf0)>>4)&0xf);	//plcվ��ַ����λ��ǰ
		*(U8 *)(AD1+2+ps*i)=asicc(plcadd&0xf);
		*(U8 *)(AD1+3+ps*i)=0x57;            				//д�䷽������W
		*(U8 *)(AD1+4+ps*i)=0x53;            				//���������ȷ��͵�����������SB(����)����53 42
		*(U8 *)(AD1+5+ps*i)=0x42; 
		*(U8 *)(AD1+6+ps*i)=0x30;            				//��2�����ȷ��͵��ǵ�ַ�ַ��Ϳ�ʼ��ַ��7������ 
		*(U8 *)(AD1+7+ps*i)=0x38; 
		*(U8 *)(AD1+8+ps*i)=0x25;            				//��3�����ȷ��͵��ǵ�ַ�ַ�%DW       
		*(U8 *)(AD1+9+ps*i)=0x44; 
	    *(U8 *)(AD1+10+ps*i)=0x57;
	    
		b=staradd+i*16;                        				//ÿ��ƫ��16
		a1=b/10000;											//���һ���ֽ���ʮ�����Ʒ��ͣ���Ҫת��ASC��
		a2=(b%10000)/1000;
		a3=(b%1000)/100;
		a4=(b%100)/10;	
		a5=b%10;	
			    
		*(U8 *)(AD1+11+ps*i)=asicc(a1);        				//��ʼ�ĵ�ַ��asicc��ʾ����λ�ȷ�
		*(U8 *)(AD1+12+ps*i)=asicc(a2);	
		*(U8 *)(AD1+13+ps*i)=asicc(a3);     
		*(U8 *)(AD1+14+ps*i)=asicc(a4);
		*(U8 *)(AD1+15+ps*i)=asicc(a5);
		
		b1=(length>>4)&0xf;                    				//д�ĸ�������b1�Ǹ�λ��b2�ǵ�λ
		b2=length&0xf;
		
		*(U8 *)(AD1+16+ps*i)=asicc(b1);        				//���ȣ�asicc��ʾ
		*(U8 *)(AD1+17+ps*i)=asicc(b2);
		
		for(j=0;j<length;j++)
		{
			b3=*(U8 *)(PE+9+i*32+j*2);         				//��������ȡ���ݣ�B3�ǵ�λ��B4�Ǹ�λ
			b4=*(U8 *)(PE+9+i*32+j*2+1);
				
			c1=b3&0xf;               						//����д������asicc����
			c2=(b3>>4)&0xf;          						//����д������asicc����		
			c3=b4&0xf;               						//����д������asicc����
			c4=(b4>>4)&0xf;          						//����д������asicc����				
		
			*(U8 *)(AD1+18+j*4+ps*i)=asicc(c4);   			//��λ�ȷ�
			*(U8 *)(AD1+19+j*4+ps*i)=asicc(c3);
			*(U8 *)(AD1+20+j*4+ps*i)=asicc(c2);
			*(U8 *)(AD1+21+j*4+ps*i)=asicc(c1);				
		}
		*(U8 *)(AD1+18+ps*i+length*4)=0x04;	     			//�����ַ�

		ptcom->send_length[i]=19+length*4;					//���ͳ���
		ptcom->send_staradd[i]=i*ps;						//�������ݴ洢��ַ	
		ptcom->return_length[i]=7;							//�������ݳ���
		ptcom->return_start[i]=0;							//����������Ч��ʼ
		ptcom->return_length_available[i]=0;				//������Ч���ݳ���	
			
	}
	ptcom->send_times=SendTimes;							//���ʹ���
	ptcom->Current_Times=0;									//��ǰ���ʹ���
	ptcom->Simens_Count=0;		
}

/*************************************************************************
 * Funciton: ��PLCдʱ��  
 * Parameters: ��
 * Return: ��
**************************************************************************/
void Write_Time()
{
	Write_Analog();									
}

/*************************************************************************
 * Funciton: ��PLC��ȡʱ�� 
 * Parameters: ��
 * Return: ��
**************************************************************************/
void Read_Time()
{
	Read_Analog();	
}

/*************************************************************************
 * Funciton: ����ɱ�׼�洢��ʽ
 * Parameters: ��
 * Return: ��
**************************************************************************/
void compxy(void)
{
	int i;
	unsigned char a1,a2,a3,a4;
	int b,b1;
	int t[8];
	if(ptcom->Simens_Count==1)//==1ʱ��λ���ش���λ����Ϊ��λ��ȡ���������������ֽ�Ϊ��λ�ҵ���ǰ
	{                                  
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/4;i++)//����Ϊ��λ��ASC�뷵�أ�4���ֽ�Ϊ1��
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
			*(U8 *)(COMad+i*2)=b1;//���յĵڶ����ֽ�Ϊ0-7bit				   				
			*(U8 *)(COMad+i*2+1)=b;
		}
	}
	else if (ptcom->Simens_Count==2)//==2ʱ��TC���ش���TC��һ���ֽ�Ϊ��λ��ȡ�����ش�����һ��BITռ4λΪ30 31 30 xx������xx��ʾ״̬��xx=0x31ΪON��xx=0x30ΪOFF
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/4;i++)
 		 {		
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*4);	
			t[i]=bsicc(a1);                                           
		}
		b=(t[7]<<7)+(t[6]<<6)+(t[5]<<5)+(t[4]<<4)+(t[3]<<3)+(t[2]<<2)+(t[1]<<1)+t[0];//��1���ֽ�Ϊ��λ������������ϳ�1���ֽڻظ�������
		*(U8 *)(COMad+0)=b;									
		
		ptcom->address=ptcom->address+8*(ptcom->Current_Times-1);		
	}
	else if (ptcom->Simens_Count==3)//==3ʱ���ַ��ش��������ɸߵ��ͣ���0x04d2��������Ϊ30 34 44 32
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
	ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;//���ȼ���	
}

/*************************************************************************
 * Funciton: �������У��
 * Parameters: ��
 * Return: ��
**************************************************************************/
void watchcom(void)
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

/*************************************************************************
 * Funciton: תΪAsc��
 * Parameters: a��ת����ʵ��
 * Return: ��
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
 * Funciton: AscתΪ����
 * Parameters: a��ת����ASC��
 * Return: ��
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
 * Funciton: ���������ȷ��
 * Parameters: ��
 * Return: ��
**************************************************************************/
int remark()
{
	return 1;
}

/*************************************************************************
 * Funciton: �����У��
 * Parameters: chData��У���ֽ���ָ�롣uNo������
 * Return: ��
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
