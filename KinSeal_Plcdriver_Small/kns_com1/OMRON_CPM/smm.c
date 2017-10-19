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
		case 'M':
		case 'Y':
		case 'H':
		case 'L':
		case 'T':
		case 'C':						
			Read_Bool();   //���������Ƕ�λ����       
			break;
		case 'D':
		case 'R':
		case 't':
		case 'c':	
			Read_Analog();  //���������Ƕ�ģ������ 
			break;			
		}
		break;
	case PLC_WRITE_DATA:				
		switch(ptcom->registerr)
		{
		case 'L':
		case 'Y':
		case 'H':
		case 'T':
		case 'C':				
			Set_Reset();      //����������ǿ����λ�͸�λ
			break;
		case 'D':
		case 'R':
		case 't':
		case 'c':
			Write_Analog();	  //����������дģ������	
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
	case 7:				//���������ǰ�PLC��״̬����Ϊ����
	/*	ptcom->R_W_Flag=0;
		ptcom->address=0;
		ptcom->registerr='M';
		ptcom->register_length=1;
		Read_Bool();	*/
		handshake();
		break;									
	case PLC_CHECK_DATA:				//�������������ݴ���
		watchcom();
		break;				
	}	 
}

void handshake()		//����PLC״̬
{
	int plcadd;
	U16 aakj;
	int a1,a2;		
	
	plcadd=ptcom->plc_address;	//PLCվ��ַ
	*(U8 *)(AD1+0)=0x40;
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);  //plcվ��ַ����λ��ǰ
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);	
	*(U8 *)(AD1+3)=0x53;
	*(U8 *)(AD1+4)=0x43;
	*(U8 *)(AD1+5)=0x30;
	*(U8 *)(AD1+6)=0x32;	
	aakj=CalFCS((U8 *)AD1,7);    //FCSУ�飬��λ��ǰ����λ�ں�         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+7)=asicc(a1);    //asicc����ʾ
	*(U8 *)(AD1+8)=asicc(a2);
	*(U8 *)(AD1+9)=0x2a;         //������Ԫ2a,0d
	*(U8 *)(AD1+10)=0x0d;	
	ptcom->send_length[0]=11;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
	ptcom->send_times=1;					//���ʹ���
		
	ptcom->return_length[0]=11;				//�������ݳ���
	ptcom->return_start[0]=0;				//����������Ч��ʼ
	ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
	ptcom->Current_Times=0;					//��ǰ���ʹ���			
	ptcom->Simens_Count=0;
}

void Set_Reset()               //��λ�͸�λ
{
	U16 aakj;
	int b,b1,b2,b3;
	int a1,a2,a3,a4;
	int plcadd;
	int t,c;	
	int c1,c2,c3,c4;
	int sendlength;
	
	
	sendlength=17;
	b=ptcom->address;			// ��ʼ��λ��ַ
	plcadd=ptcom->plc_address;	//PLCվ��ַ
	
	b=(b+8)/100;						//����
//�ȶ�	////////////////////////////////////
	switch (ptcom->registerr)	//�����������ݼĴ����Ĳ�ͬ���в�ͬ�Ĺ�����
	{
	case 'Y':
	case 'M':
	case 'L':
	case 'H':
		a1=(b/100);             //ȡ��λ��
		a2=((b-a1*100)/10);     //ȡʮλ��
		a3=(b-a1*100-a2*10);    //ȡ��λ��
		break;
	case 'T':
	case 'C':
		a1=(b/100);             //ȡ��λ��
		a2=((b-a1*100)/10);     //ȡʮλ��
		a3=(b-a1*100-a2*10);    //ȡ��λ��
		break;
		
	}

	*(U8 *)(AD1+0)=0x40;        //OMRON��ʼ��Ԫ"@",��40
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf); //plcվ��ַ����λ��ǰ
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);
	
	switch (ptcom->registerr)	//�����������ݼĴ����Ĳ�ͬ���в�ͬ�Ĺ�����
	{
	case 'Y':
		*(U8 *)(AD1+3)=0x52;      //��IR/SR����������ΪRR����52 52
		*(U8 *)(AD1+4)=0x52;
		break;
	case 'M':
		*(U8 *)(AD1+3)=0x52;      //��AR����������ΪRJ����52 48
		*(U8 *)(AD1+4)=0x4A;
		break;	
	case 'T':
		*(U8 *)(AD1+3)=0x52;      //��T����������ΪRG����52 47
		*(U8 *)(AD1+4)=0x47;
		break;
	case 'C':
		*(U8 *)(AD1+3)=0x52;      //��C����������ΪRG����52 47
		*(U8 *)(AD1+4)=0x47;
		break;	
	case 'L':
		*(U8 *)(AD1+3)=0x52;      //��LR����������ΪRL����52 4C
		*(U8 *)(AD1+4)=0x4C;
		break;	
	case 'H':
		*(U8 *)(AD1+3)=0x52;      //��HR����������ΪRH����52 48
		*(U8 *)(AD1+4)=0x48;
		break;							
	}

	*(U8 *)(AD1+5)=0x30;         //��ʼ��ַ�����δӸߵ��ͣ�Ҫת��asicc��
	*(U8 *)(AD1+6)=asicc(a1);
	*(U8 *)(AD1+7)=asicc(a2);
	*(U8 *)(AD1+8)=asicc(a3);
	
	
	switch (ptcom->registerr)	//�����������ݼĴ����Ĳ�ͬ���в�ͬ�Ĺ�����
	{
	case 'Y':
	case 'M':
	case 'L':
	case 'H':
		c=1;   								//��1��Ԫ��
		c1=0;                    //ȡǧλ��
		c2=0;           //ȡ��λ��
		c3=0;     //ȡʮλ��
		c4=1;    //ȡ��λ�� 
		*(U8 *)(AD1+9)=asicc(c1);	//����Ԫ����������asicc�뷢�ͣ����δӸߵ���
		*(U8 *)(AD1+10)=asicc(c2);
		*(U8 *)(AD1+11)=asicc(c3);
		*(U8 *)(AD1+12)=asicc(c4);
		aakj=CalFCS((U8 *)AD1,13);    //FCSУ�飬��λ��ǰ����λ�ں�         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13)=asicc(a1);    //asicc����ʾ
		*(U8 *)(AD1+14)=asicc(a2);
		*(U8 *)(AD1+15)=0x2a;         //������Ԫ2a,0d
		*(U8 *)(AD1+16)=0x0d;	
		
		ptcom->Simens_Count=1;
		
		ptcom->register_length=c*2;				//�����ֽ���
		ptcom->send_length[0]=17;				//���ͳ���
		ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
			
		ptcom->return_length[0]=11+4;
		                                          
		ptcom->return_start[0]=7;				//����������Ч��ʼ
		ptcom->return_length_available[0]=4 ;	//������Ч���ݳ���	
		ptcom->Current_Times=0;					//��ǰ���ʹ���	
		ptcom->send_add[0]=b*100;		//�����������ַ������             
		break;
	case 'T':
	case 'C':
		c=1;   								//�����ٸ�Ԫ��
		c1=0;                    //ȡǧλ��
		c2=0;           //ȡ��λ��
		c3=0;     //ȡʮλ��
		c4=1;    //ȡ��λ�� 
		
		ptcom->Simens_Count=1;
		
		*(U8 *)(AD1+9)=asicc(c1);	//����Ԫ����������asicc�뷢�ͣ����δӸߵ���
		*(U8 *)(AD1+10)=asicc(c2);
		*(U8 *)(AD1+11)=asicc(c3);
		*(U8 *)(AD1+12)=asicc(c4);

		aakj=CalFCS((U8 *)AD1,13);    //FCSУ�飬��λ��ǰ����λ�ں�         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13)=asicc(a1);    //asicc����ʾ
		*(U8 *)(AD1+14)=asicc(a2);
		*(U8 *)(AD1+15)=0x2a;         //������Ԫ2a,0d
		*(U8 *)(AD1+16)=0x0d;	
		
		ptcom->send_length[0]=17;				//���ͳ���
		ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
			
		ptcom->return_length[0]=15;				//�������ݳ���
		                                                   
		ptcom->return_start[0]=7;				//����������Ч��ʼ
		ptcom->return_length_available[0]=4;	//������Ч���ݳ���	
		ptcom->Current_Times=0;					//��ǰ���ʹ���	
		ptcom->send_add[0]=b*16;					//�����������ַ������	
	break;
		
	}	
	
//��д	//////////////////////////////////////

	switch (ptcom->registerr)	//�����������ݼĴ����Ĳ�ͬ���в�ͬ�Ĳ�����
	{
	case 'Y':
	case 'L':
	case 'H':	
		t=b;                   		 //ָ����ĵ�ַ��10���Ƶģ�����*100����/100ȡ��
		a1=(t/100)&0xf;             //ȡ��λ��
		a2=((t-a1*100)/10)&0xf;     //ȡʮλ��
		a3=(t-a1*100-a2*10)&0xf;    //ȡ��λ��
		break;
	case 'T':
	case 'C':
		a1=(b/100)&0xf;             //ȡ��λ��
		a2=((b-a1*100)/10)&0xf;     //ȡʮλ��
		a3=(b-a1*100-a2*10)&0xf;    //ȡ��λ��
		break;		
	}
		
	*(U8 *)(AD1+0+sendlength)=0x40;        //OMRON��ʼ��Ԫ"@",��40
	*(U8 *)(AD1+1+sendlength)=asicc(((plcadd&0xf0)>>4)&0xf);  //plcվ��ַ����λ��ǰ
	*(U8 *)(AD1+2+sendlength)=asicc(plcadd&0xf);
	
	
	switch (ptcom->registerr)	//�����������ݼĴ����Ĳ�ͬ���в�ͬ�Ĳ�����
	{
	case 'Y':
		*(U8 *)(AD1+3+sendlength)='W';      //��IR/SR����������ΪC I O �ո�
		*(U8 *)(AD1+4+sendlength)='R';
		break;
	case 'L':
		*(U8 *)(AD1+3+sendlength)='W';      //��IR/SR����������ΪC I O �ո�
		*(U8 *)(AD1+4+sendlength)='L';
		break;	
	case 'T':
		*(U8 *)(AD1+3+sendlength)='W';      //��IR/SR����������ΪC I O �ո�
		*(U8 *)(AD1+4+sendlength)='G';
		break;
	case 'C':
		*(U8 *)(AD1+3+sendlength)='W';      //��IR/SR����������ΪC I O �ո�
		*(U8 *)(AD1+4+sendlength)='G';
		break;
	case 'H':
		*(U8 *)(AD1+3+sendlength)='W';      //��IR/SR����������ΪC I O �ո�
		*(U8 *)(AD1+4+sendlength)='H';
		break;					
	}	
	
	*(U8 *)(AD1+5+sendlength)=0x30;					//��ַ
	*(U8 *)(AD1+6+sendlength)=asicc(a1);
	*(U8 *)(AD1+7+sendlength)=asicc(a2);
	*(U8 *)(AD1+8+sendlength)=asicc(a3);
	
	*(U8 *)(AD1+9+sendlength)=0x30;					//д�����ݱ���	
	*(U8 *)(AD1+10+sendlength)=0x31;
	*(U8 *)(AD1+11+sendlength)=0x32;
	*(U8 *)(AD1+12+sendlength)=0x33;	
	
			
	aakj=CalFCS((U8 *)AD1+sendlength,13);    //FCSУ�飬��λ��ǰ����λ�ں�         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+13+sendlength)=asicc(a1);    //asicc����ʾ
	*(U8 *)(AD1+14+sendlength)=asicc(a2);
	*(U8 *)(AD1+15+sendlength)=0x2a;         //������Ԫ2a,0d
	*(U8 *)(AD1+16+sendlength)=0x0d;
	
	ptcom->send_length[1]=17;				//���ͳ���
	ptcom->send_staradd[1]=17;				//�������ݴ洢��ַ	
		
	ptcom->return_length[1]=11;				//�������ݳ���
	ptcom->return_start[1]=0;				//����������Ч��ʼ
	ptcom->return_length_available[1]=0;	//������Ч���ݳ���	
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	
	ptcom->send_times=2;					//���ʹ���	
	
	if (ptcom->writeValue==1)		//ְλ
	{
		ptcom->Simens_Count=100;
	}	
	if (ptcom->writeValue==0)		//��λ
	{
		ptcom->Simens_Count=200;
	}	
}



void Read_Bool()				//��ȡ����������Ϣ
{
	U16 aakj;
	int b,t,c;
	int a0,a1,a2,a3;
	int b1,b2,b3;
	int plcadd;	
	int c1,c2,c3,c4;
		
	b=ptcom->address;			//���������Ѿ�ת�����öεĿ�ʼ��ַ
	plcadd=ptcom->plc_address;	//PLCվ��ַ
	
	switch (ptcom->registerr)	//�����������ݼĴ����Ĳ�ͬ���в�ͬ�Ĺ�����
	{
	case 'Y':
	case 'M':
	case 'L':
	case 'H':
	
		b=(b+8)/100;				//�����ٿ�£
		t=b;
		ptcom->address=t*100;

		a0=(t/1000)&0xf;
		a1=((t-a0*1000)/100)&0xf;             //ȡ��λ��
		a2=((t-a1*100)/10)&0xf;     //ȡʮλ��
		a3=(t-a1*100-a2*10)&0xf;    //ȡ��λ��
		break;
	case 'T':
	case 'C':
		a0=(t/1000)&0xf;
		a1=((t-a0*1000)/100)&0xf;             //ȡ��λ��
		a2=((b-a1*100)/10)&0xf;     //ȡʮλ��
		a3=(b-a1*100-a2*10)&0xf;    //ȡ��λ��
		break;
		
	}

	*(U8 *)(AD1+0)=0x40;        //OMRON��ʼ��Ԫ"@",��40
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf); //plcվ��ַ����λ��ǰ
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);
	
	switch (ptcom->registerr)	//�����������ݼĴ����Ĳ�ͬ���в�ͬ�Ĺ�����
	{
	case 'Y':
		*(U8 *)(AD1+3)=0x52;      //��IR/SR����������ΪRR����52 52
		*(U8 *)(AD1+4)=0x52;
		break;
	case 'M':
		*(U8 *)(AD1+3)=0x52;      //��AR����������ΪRJ����52 48
		*(U8 *)(AD1+4)=0x4A;
		break;	
	case 'T':
		*(U8 *)(AD1+3)=0x52;      //��T����������ΪRG����52 47
		*(U8 *)(AD1+4)=0x47;
		break;
	case 'C':
		*(U8 *)(AD1+3)=0x52;      //��C����������ΪRG����52 47
		*(U8 *)(AD1+4)=0x47;
		break;	
	case 'L':
		*(U8 *)(AD1+3)=0x52;      //��LR����������ΪRL����52 4C
		*(U8 *)(AD1+4)=0x4C;
		break;	
	case 'H':
		*(U8 *)(AD1+3)=0x52;      //��HR����������ΪRH����52 48
		*(U8 *)(AD1+4)=0x48;
		break;							
	}

	*(U8 *)(AD1+5)=asicc(a0);         //��ʼ��ַ�����δӸߵ��ͣ�Ҫת��asicc��
	*(U8 *)(AD1+6)=asicc(a1);
	*(U8 *)(AD1+7)=asicc(a2);
	*(U8 *)(AD1+8)=asicc(a3);
	
	
	switch (ptcom->registerr)	//�����������ݼĴ����Ĳ�ͬ���в�ͬ�Ĺ�����
	{
	case 'Y':
	case 'M':
	case 'L':
	case 'H':
		c=ptcom->register_length;   //�����ٸ�Ԫ��
		c=(c/2)+(c%2);
		c1=(c/1000)&0xf;                    //ȡǧλ��
		c2=((c-c1*1000)/100)&0xf;           //ȡ��λ��
		c3=((c-c1*1000-c2*100)/10)&0xf;     //ȡʮλ��
		c4=(c-c1*1000-c2*100-c3*10)&0xf;    //ȡ��λ�� 
		*(U8 *)(AD1+9)=asicc(c1);	//����Ԫ����������asicc�뷢�ͣ����δӸߵ���
		*(U8 *)(AD1+10)=asicc(c2);
		*(U8 *)(AD1+11)=asicc(c3);
		*(U8 *)(AD1+12)=asicc(c4);
		aakj=CalFCS((U8 *)AD1,13);    //FCSУ�飬��λ��ǰ����λ�ں�         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13)=asicc(a1);    //asicc����ʾ
		*(U8 *)(AD1+14)=asicc(a2);
		*(U8 *)(AD1+15)=0x2a;         //������Ԫ2a,0d
		*(U8 *)(AD1+16)=0x0d;	
		
		ptcom->register_length=c*2;				//�����ֽ���
		ptcom->send_length[0]=17;				//���ͳ���
		ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
		ptcom->send_times=1;					//���ʹ���
			
		ptcom->return_length[0]=11+c*4;//�������ݳ��ȣ���11���̶���40��PLC��ַ2�����ȣ�������2�����ȣ�
		                                                    //������2�����ȣ�У��2�����ȣ�0d��0a
		ptcom->return_start[0]=7;				//����������Ч��ʼ
		ptcom->return_length_available[0]=c*4 ;	//������Ч���ݳ���	
		ptcom->Current_Times=0;					//��ǰ���ʹ���	
		ptcom->send_add[0]=ptcom->address;		//�����������ַ������
		
		ptcom->Simens_Count=1;	                    //��������ʱ�ߵ�˳��             
		break;
	case 'T':
	case 'C':
		c=ptcom->register_length;   //�����ٸ�Ԫ��
		c=c*8;
		c1=(c/1000)&0xf;                    //ȡǧλ��
		c2=((c-c1*1000)/100)&0xf;           //ȡ��λ��
		c3=((c-c1*1000-c2*100)/10)&0xf;     //ȡʮλ��
		c4=(c-c1*1000-c2*100-c3*10)&0xf;    //ȡ��λ��
		
		ptcom->Simens_Count=1;
		
		*(U8 *)(AD1+9)=asicc(c1);	//����Ԫ����������asicc�뷢�ͣ����δӸߵ���
		*(U8 *)(AD1+10)=asicc(c2);
		*(U8 *)(AD1+11)=asicc(c3);
		*(U8 *)(AD1+12)=asicc(c4);

		aakj=CalFCS((U8 *)AD1,13);    //FCSУ�飬��λ��ǰ����λ�ں�         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13)=asicc(a1);    //asicc����ʾ
		*(U8 *)(AD1+14)=asicc(a2);
		*(U8 *)(AD1+15)=0x2a;         //������Ԫ2a,0d
		*(U8 *)(AD1+16)=0x0d;	
		
		ptcom->send_length[0]=17;				//���ͳ���
		ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
		ptcom->send_times=1;					//���ʹ���
			
		ptcom->return_length[0]=11+c;//�������ݳ��ȣ���11���̶���40��PLC��ַ2�����ȣ�������2�����ȣ�
		                                                    //������2�����ȣ�У��2�����ȣ�0d��0a
		ptcom->return_start[0]=7;				//����������Ч��ʼ
		ptcom->return_length_available[0]=c;	//������Ч���ݳ���	
		ptcom->Current_Times=0;					//��ǰ���ʹ���	
		ptcom->send_add[0]=ptcom->address;		//�����������ַ������
		ptcom->Simens_Count=1;	
	break;
		
	}	
}


void Read_Analog()				//��ģ����
{
	U16 aakj;
	int b,c;
	int a1,a2,a3,a4;
	int c1,c2,c3,c4;
	int plcadd;
				
	b=ptcom->address;			//��ʼ��ַ
	plcadd=ptcom->plc_address;	//PLCվ��ַ
	
	a1=(b/1000)&0xf;                    //ȡ10����ǧλ��
	a2=((b-a1*1000)/100)&0xf;           //ȡ10���ư�λ��
	a3=((b-a1*1000-a2*100)/10)&0xf;     //ȡ10����ʮλ��
	a4=(b-a1*1000-a2*100-a3*10)&0xf;    //ȡ10���Ƹ�λ��	

	*(U8 *)(AD1+0)=0x40;          //OMRON��ʼ��Ԫ"@",��40
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf);        //plcվ��ַ����λ��ǰ
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);
	
	switch (ptcom->registerr)	  //�����������ݼĴ����Ĳ�ͬ���в�ͬ�Ĺ�����
	{
	case 'D':
		*(U8 *)(AD1+3)=0x52;      //��IR/SR����������ΪRD����52 44
		*(U8 *)(AD1+4)=0x44;
		break;
	case 'R':
		*(U8 *)(AD1+3)=0x52;      //��IR/SR����������ΪRD����52 52
		*(U8 *)(AD1+4)=0x52;
		break;
	case 't':
		*(U8 *)(AD1+3)=0x52;      //��T����������ΪRC����52 43
		*(U8 *)(AD1+4)=0x43;
		break;
	case 'c':
		*(U8 *)(AD1+3)=0x52;      //��C����������ΪRC����52 43
		*(U8 *)(AD1+4)=0x43;
		break;					
	}	
	
	*(U8 *)(AD1+5)=asicc(a1);     //��ʼ��ַ�����δӸߵ��ͣ�Ҫת��asicc��
	*(U8 *)(AD1+6)=asicc(a2);
	*(U8 *)(AD1+7)=asicc(a3);
	*(U8 *)(AD1+8)=asicc(a4);
	
	c=ptcom->register_length;     //�����ٸ�Ԫ��
 
	c1=(c/1000)&0xf;                    //ȡǧλ��
	c2=((c-c1*1000)/100)&0xf;           //ȡ��λ��
	c3=((c-c1*1000-c2*100)/10)&0xf;     //ȡʮλ��
	c4=(c-c1*1000-c2*100-c3*10)&0xf;    //ȡ��λ��

	*(U8 *)(AD1+9)=asicc(c1);	 //����Ԫ����������asicc�뷢�ͣ����δӸߵ���
	*(U8 *)(AD1+10)=asicc(c2);
	*(U8 *)(AD1+11)=asicc(c3);
	*(U8 *)(AD1+12)=asicc(c4);

	aakj=CalFCS((U8 *)AD1,13);    //FCSУ�飬��λ��ǰ����λ�ں�         
	a1=(aakj&0xf0)>>4;
	a1=a1&0xf;
	a2=aakj&0xf;
	*(U8 *)(AD1+13)=asicc(a1);    //asicc����ʾ
	*(U8 *)(AD1+14)=asicc(a2);
	*(U8 *)(AD1+15)=0x2a;         //������Ԫ2a,0d
	*(U8 *)(AD1+16)=0x0d;	
	
	ptcom->send_length[0]=17;				//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
	ptcom->send_times=1;					//���ʹ���
		
	ptcom->return_length[0]=11+ptcom->register_length*4;//�������ݳ��ȣ���11���̶���40��PLC��ַ2�����ȣ�������2�����ȣ�
	                                                    //������2�����ȣ�У��2�����ȣ�2a��0d
	ptcom->return_start[0]=7;				//����������Ч��ʼ
	ptcom->return_length_available[0]=ptcom->register_length*4;	//������Ч���ݳ���	
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	ptcom->send_add[0]=ptcom->address;		//�����������ַ������

	ptcom->Simens_Count=5;
}


void Read_Recipe()							//��ȡ�䷽
{
	U16 aakj;
	int b;
	int c,c1,c2,c3,c4;
	int a1,a2,a3,a4;
	int i;
	int datalength;                         //���ݳ���
	int p_start;                            //���ݿ�ʼ��ַ
	int ps;
	int SendTimes;                          //���ʹ���
	int LastTimeWord;						//���һ�η��ͳ���
	int currentlength;
	int plcadd;                             //PLCվ��ַ
	
	datalength=ptcom->register_length;		//�����ܳ���
	p_start=ptcom->address;					//��ʼ��ַ
	plcadd=ptcom->plc_address;	            //PLCվ��ַ
	
	if(datalength>5000)                     //���Ƴ���
		datalength=5000;

	if(datalength%28==0)                    //̨������ܷ�28��D�����ݸպ���28D�ı���
	{
		SendTimes=datalength/28;            //���ʹ���
		LastTimeWord=28;                    //���һ�η��͵ĳ���Ϊ28D	
	}
	if(datalength%28!=0)                    //̨������ܷ�28��D�����ݲ���28D�ı��� 
	{
		SendTimes=datalength/28+1;          //���͵Ĵ���
		LastTimeWord=datalength%28;         //���һ�η��͵ĳ���Ϊ��28������	
	}
	
	for (i=0;i<SendTimes;i++)
	{
		ps=i*17;                            //ÿ�η�17������
		b=p_start+i*28;                     //��ʼ��ַ
		a1=(b/1000)&0xf;                    //ȡǧλ��
		a2=((b-a1*1000)/100)&0xf;           //ȡ��λ��
		a3=((b-a1*1000-a2*100)/10)&0xf;     //ȡʮλ��
		a4=(b-a1*1000-a2*100-a3*10)&0xf;    //ȡ��λ��
		
		*(U8 *)(AD1+0+ps)=0x40;             //OMRON��ʼ��Ԫ"@",��40
		*(U8 *)(AD1+1+ps)=asicc(((plcadd&0xf0)>>4)&0xf);        //plcվ��ַ����λ��ǰ
		*(U8 *)(AD1+2+ps)=asicc(plcadd&0xf);
		

		*(U8 *)(AD1+3+ps)=0x52;      //��DM����������ΪRD����52 44
		*(U8 *)(AD1+4+ps)=0x44;

		*(U8 *)(AD1+5+ps)=asicc(a1);         //��ʼ��ַ�����δӸߵ��ͣ�Ҫת��asicc��
		*(U8 *)(AD1+6+ps)=asicc(a2);
		*(U8 *)(AD1+7+ps)=asicc(a3);
		*(U8 *)(AD1+8+ps)=asicc(a4);
		
		if (i!=(SendTimes-1))	     //�������һ�η���ʱ
		{
			*(U8 *)(AD1+9+ps)=0x30;             //�̶�����28��D����56�ֽڣ���λ��asicc����ʾ
			*(U8 *)(AD1+10+ps)=0x30;
			*(U8 *)(AD1+11+ps)=0x32;            //�̶�����28��D����56�ֽڣ���λ��asicc����ʾ
			*(U8 *)(AD1+12+ps)=0x38;
			currentlength=28;                   //�̶�����28��D
		}
		if (i==(SendTimes-1))	     //���һ�η���ʱ
		{
			c=LastTimeWord;                     //�����ٸ�Ԫ��
			c1=(c/1000)&0xf;                    //ȡǧλ��
			c2=((c-c1*1000)/100)&0xf;           //ȡ��λ��
			c3=((c-c1*1000-c2*100)/10)&0xf;     //ȡʮλ��
			c4=(c-c1*1000-c2*100-c3*10)&0xf;    //ȡ��λ��
			*(U8 *)(AD1+9+ps)=asicc(c1);         //���͵ĳ��ȣ����δӸߵ��ͣ�Ҫת��asicc��
			*(U8 *)(AD1+10+ps)=asicc(c2);
			*(U8 *)(AD1+11+ps)=asicc(c3);
			*(U8 *)(AD1+12+ps)=asicc(c4);
			currentlength=LastTimeWord;         //ʣ��LastTimeWord��D
		}

		aakj=CalFCS((U8 *)(AD1+ps),13);    //FCSУ�飬��λ��ǰ����λ�ں�         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13+ps)=asicc(a1);    //asicc����ʾ
		*(U8 *)(AD1+14+ps)=asicc(a2);
		*(U8 *)(AD1+15+ps)=0x2a;         //������Ԫ2a,0d
		*(U8 *)(AD1+16+ps)=0x0d;
		
		ptcom->send_length[i]=17;				    //���ͳ���
		ptcom->send_staradd[i]=i*17;			    //�������ݴ洢��ַ	
		ptcom->send_add[i]=p_start+i*28;		    //�����������ַ������	
		ptcom->send_data_length[i]=currentlength;	//�������һ�ζ���28��D
				
		ptcom->return_length[i]=11+currentlength*4; //�������ݳ��ȣ���11���̶���40��PLC��ַ2�����ȣ�������2�����ȣ�
	                                                //������2�����ȣ�У��2�����ȣ�0d��0a
		ptcom->return_start[i]=7;				    //����������Ч��ʼ
		ptcom->return_length_available[i]=currentlength*4;	//������Ч���ݳ���	
		
	}
	ptcom->send_times=SendTimes;					//���ʹ���
	ptcom->Current_Times=0;		
	ptcom->Simens_Count=5;	
}

void Write_Analog()								    //дģ����
{
	U16 aakj;
	int b;
	int a1,a2,a3,a4;
	int i;
	int length;
	int c0,c01,c02,c03,c04;
	int c1;
	int plcadd;
	
	b=ptcom->address;			//��ʼ��ַ
	plcadd=ptcom->plc_address;	//PLCվ��ַ
	length=ptcom->register_length;//����
	
	a1=(b/1000)&0xf;                    //ȡǧλ��
	a2=((b-a1*1000)/100)&0xf;           //ȡ��λ��
	a3=((b-a1*1000-a2*100)/10)&0xf;     //ȡʮλ��
	a4=(b-a1*1000-a2*100-a3*10)&0xf;    //ȡ��λ��	
	
	*(U8 *)(AD1+0)=0x40;        //OMRON��ʼ��Ԫ"@",��40
	*(U8 *)(AD1+1)=asicc(((plcadd&0xf0)>>4)&0xf); //plcվ��ַ����λ��ǰ
	*(U8 *)(AD1+2)=asicc(plcadd&0xf);

	switch (ptcom->registerr)	  //�����������ݼĴ����Ĳ�ͬ���в�ͬ�Ĺ�����
	{
	case 'D':
		*(U8 *)(AD1+3)=0x57;      //��DM����������ΪWD����57 44
		*(U8 *)(AD1+4)=0x44;
		break;
	case 'R':
		*(U8 *)(AD1+3)=0x57;      //��DM����������ΪWD����57 52
		*(U8 *)(AD1+4)=0x52;
		break;
	case 't':
		*(U8 *)(AD1+3)=0x57;      //��T����������ΪWC����57 43
		*(U8 *)(AD1+4)=0x43;
		break;
	case 'c':
		*(U8 *)(AD1+3)=0x57;      //��C����������ΪWC����57 43
		*(U8 *)(AD1+4)=0x43;
		break;					
	}	
	
	*(U8 *)(AD1+5)=asicc(a1);   //��ʼ��ַ�����δӸߵ��ͣ�Ҫת��asicc��
	*(U8 *)(AD1+6)=asicc(a2);
	*(U8 *)(AD1+7)=asicc(a3);
	*(U8 *)(AD1+8)=asicc(a4);
	
	for (i=0;i<length;i++)        //д����Ԫ��ֵ
	{				
		c0=ptcom->U8_Data[i*2];   //��D[]������Ҫ���ݣ���Ӧ��c1Ϊ��λ��c0Ϊ��λ
		c1=ptcom->U8_Data[i*2+1];
						
		c01=c0&0xf;               //����д������asicc����c0Ϊ��λ
		c02=(c0>>4)&0xf;          //����д������asicc����c0Ϊ��λ		
		c03=c1&0xf;               //����д������asicc����c1Ϊ��λ
		c04=(c1>>4)&0xf;          //����д������asicc����c0Ϊ��λ				
	
		*(U8 *)(AD1+9+i*4)=asicc(c04);
		*(U8 *)(AD1+10+i*4)=asicc(c03);
		*(U8 *)(AD1+11+i*4)=asicc(c02);
		*(U8 *)(AD1+12+i*4)=asicc(c01);	
	} 
		
		aakj=CalFCS((U8 *)AD1,9+length*4);      //FCSУ�飬��λ��ǰ����λ�ں�         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+9+length*4)=asicc(a1);      //asicc����ʾ
		*(U8 *)(AD1+10+length*4)=asicc(a2);
		*(U8 *)(AD1+11+length*4)=0x2a;          //������Ԫ2a,0d
		*(U8 *)(AD1+12+length*4)=0x0d;

		
		ptcom->send_length[0]=13+length*4;		//���ͳ���
		ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
		ptcom->send_times=1;					//���ʹ���
				
		ptcom->return_length[0]=11;				//�������ݳ���
		ptcom->return_start[0]=0;				//����������Ч��ʼ
		ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
		ptcom->Current_Times=0;	
}


void Write_Time()                                //дʱ�䵽PLC
{
	Write_Analog();									
}


void Read_Time()									//��PLC��ȡʱ��
{
	Read_Analog();
}


void Write_Recipe()								//д�䷽��PLC
{
	int datalength;
	int staradd;
	int SendTimes;                              //���͵Ĵ���
	int LastTimeWord;							//���һ�η��ͳ���
	int i,j;
	int ps;
	int b;
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

    ps=125;                                    //��������28��D��ʱ��ǰ��ÿ�η�125��

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
		
		b=staradd+i*28;                        //ÿ��ƫ��28��D
		a1=(b/1000)&0xf;                       //ȡǧλ��
		a2=((b-a1*1000)/100)&0xf;              //ȡ��λ��
		a3=((b-a1*1000-a2*100)/10)&0xf;        //ȡʮλ��
		a4=(b-a1*1000-a2*100-a3*10)&0xf;       //ȡ��λ��

		*(U8 *)(AD1+0+ps*i)=0x40;              //OMRON��ʼ��Ԫ"@",��40
		*(U8 *)(AD1+1+ps*i)=asicc(((plcadd&0xf0)>>4)&0xf); //plcվ��ַ����λ��ǰ
		*(U8 *)(AD1+2+ps*i)=asicc(plcadd&0xf);

		*(U8 *)(AD1+3+ps*i)=0x57;      //��DM����������ΪWD����57 44
		*(U8 *)(AD1+4+ps*i)=0x44;

		*(U8 *)(AD1+5+ps*i)=asicc(a1);         //��ʼ��ַ�����δӸߵ��ͣ�Ҫת��asicc��
		*(U8 *)(AD1+6+ps*i)=asicc(a2);
		*(U8 *)(AD1+7+ps*i)=asicc(a3);
		*(U8 *)(AD1+8+ps*i)=asicc(a4);

		for(j=0;j<length;j++)                  //д����Ԫ��ֵ                 
		{	
			c0=*(U8 *)(PE+9+i*56+j*2);         //�Ӵ����ݵļĴ�����ʼ��ַPE+9ȡ���ݣ�k3Ϊ��λ��k4Ϊ��λ
			c1=*(U8 *)(PE+9+i*56+j*2+1);
							
			c01=c0&0xf;               //����д������asicc����c0Ϊ��λ
			c02=(c0>>4)&0xf;          //����д������asicc����c0Ϊ��λ		
			c03=c1&0xf;               //����д������asicc����c1Ϊ��λ
			c04=(c1>>4)&0xf;          //����д������asicc����c0Ϊ��λ				
		;
			*(U8 *)(AD1+9+j*4+ps*i)=asicc(c04);
			*(U8 *)(AD1+10+j*4+ps*i)=asicc(c03);
			*(U8 *)(AD1+11+j*4+ps*i)=asicc(c02);
			*(U8 *)(AD1+12+j*4+ps*i)=asicc(c01);							
		}

		aakj=CalFCS((U8 *)(AD1+ps*i),9+length*4);      //FCSУ�飬��λ��ǰ����λ�ں�         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13+(length-1)*4+ps*i)=asicc(a1);      //asicc����ʾ
		*(U8 *)(AD1+14+(length-1)*4+ps*i)=asicc(a2);
		*(U8 *)(AD1+15+(length-1)*4+ps*i)=0x2a;          //������Ԫ2a,0d
		*(U8 *)(AD1+16+(length-1)*4+ps*i)=0x0d;
		
		ptcom->send_length[i]=13+length*4;	   //���ͳ���
		ptcom->send_staradd[i]=i*ps;		   //�������ݴ洢��ַ	
		
		ptcom->return_length[i]=11;			   //�������ݳ���
		ptcom->return_start[i]=0;			   //����������Ч��ʼ
		ptcom->return_length_available[i]=0;   //������Ч���ݳ���	
    }
	ptcom->send_times=SendTimes;			   //���ʹ���
	ptcom->Current_Times=0;				
}


void compxy(void)				              //����ɱ�׼�洢��ʽ,��������
{
	int i;
	unsigned char a1,a2,a3,a4;
	int b1,b2;
	U16 temp;
	int j;
	U16 aakj;
	
	if(ptcom->Simens_Count==1)
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
			ptcom->Simens_Count=0;	
		}
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
		//	if(ptcom->Simens_Count==1)	                         //����������ʱ��Ϊ1����˳��
		//	{
		//		*(U8 *)(COMad+i*2)=b2;					     //���´�,�ӵ�0����ʼ��
		//		*(U8 *)(COMad+i*2+1)=b1;					 //���´�,�ӵ�0����ʼ��
		//	}		
		//	else
		//	{ 
				*(U8 *)(COMad+i*2)=b1;					     //���´�,�ӵ�0����ʼ��
				*(U8 *)(COMad+i*2+1)=b2;					 //���´�,�ӵ�0����ʼ��		
		//	}	
		}
		ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;	//���ȼ���	
	} 
	else if (ptcom->Simens_Count==100)			//��λ
	{
		a1=*(U8 *)(COMad+7);
		a2=*(U8 *)(COMad+8);
		a3=*(U8 *)(COMad+9);
		a4=*(U8 *)(COMad+10);
		a1=bsicc(a1);
		a2=bsicc(a2);
		a3=bsicc(a3);
		a4=bsicc(a4);
		b1=(a1<<4)+a2;
		b2=(a3<<4)+a4;	
		b1=(b1<<8)+b2;
				
		j=ptcom->address-ptcom->send_add[0];			//ƫ�ƶ���λ

		temp=1<<j;
		
		temp=b1|temp;
		
		a1=(temp>>12)&0xf;
		a2=(temp>>8)&0xf;		
		a3=(temp>>4)&0xf;		
		a4=(temp>>0)&0xf;		

		//����
		*(U8 *)(AD1+9+17)=asicc(a1);					//д�����ݱ���	
		*(U8 *)(AD1+10+17)=asicc(a2);
		*(U8 *)(AD1+11+17)=asicc(a3);
		*(U8 *)(AD1+12+17)=asicc(a4);			
		
		aakj=CalFCS((U8 *)AD1+17,13);    //FCSУ�飬��λ��ǰ����λ�ں�         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13+17)=asicc(a1);    //asicc����ʾ
		*(U8 *)(AD1+14+17)=asicc(a2);
			
		ptcom->Simens_Count=0;	
				
	}
	else if (ptcom->Simens_Count==200)			//��λ
	{
		a1=*(U8 *)(COMad+7);
		a2=*(U8 *)(COMad+8);
		a3=*(U8 *)(COMad+9);
		a4=*(U8 *)(COMad+10);
		a1=bsicc(a1);
		a2=bsicc(a2);
		a3=bsicc(a3);
		a4=bsicc(a4);
		b1=(a1<<4)+a2;
		b2=(a3<<4)+a4;	
		b1=(b1<<8)+b2;
				
		j=ptcom->address-ptcom->send_add[0];			//ƫ�ƶ���λ

		temp=1<<j;
		temp=~temp;
		
		temp=b1&temp;
		
		a1=(temp>>12)&0xf;
		a2=(temp>>8)&0xf;		
		a3=(temp>>4)&0xf;		
		a4=(temp>>0)&0xf;		

		//����
		*(U8 *)(AD1+9+17)=asicc(a1);					//д�����ݱ���	
		*(U8 *)(AD1+10+17)=asicc(a2);
		*(U8 *)(AD1+11+17)=asicc(a3);
		*(U8 *)(AD1+12+17)=asicc(a4);			
		
		aakj=CalFCS((U8 *)AD1+17,13);    //FCSУ�飬��λ��ǰ����λ�ں�         
		a1=(aakj&0xf0)>>4;
		a1=a1&0xf;
		a2=aakj&0xf;
		*(U8 *)(AD1+13+17)=asicc(a1);    //asicc����ʾ
		*(U8 *)(AD1+14+17)=asicc(a2);
			
		ptcom->Simens_Count=0;			
	}	
	else
	{
		ptcom->IfResultCorrect=0;
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



int asicc(int a)			//תΪAsc��
{
	int bl;
	if(a<10)
		bl=a+0x30;
	if(a>9)
		bl=a-10+0x41;
	return bl;	
}

int bsicc(int a)			//AscתΪ����
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

int remark()				//�����������ݼ���У�����Ƿ���ȷ
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
		return 1;
	else
		return 0;
}


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

