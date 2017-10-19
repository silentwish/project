#include "stdio.h"
#include "def.h"
#include "smm.h"


struct Com_struct_D *ptcom;

void Enter_PlcDriver(void)
{
	int result;
	int IfOK;	
	LB=*(U32 *)(LB_Address+0);
	LW=*(U32 *)(LW_Address+0);
	RWI=*(U32 *)(RWI_Address+0);	
	ptcom=(struct Com_struct_D *)adcom;
		
	result=checkCorrect();		//��������Ƿ���ȷ	
//	sysprintf("result %d \n",result);
	if (result==1)		//������ȷ
	{	
		IfOK=senddata();
		if (IfOK==1)		//�ɹ���ȷ
		{
			ptcom->IfResultCorrect=1;
		}
	}	
	else if (result==8)		//CRC����
	{
		errorsend(*(U8 *)(COMad+1),8);		//������
		ptcom->IfResultCorrect=1;
	}
}

int checkCorrect()		//��������Ƿ���ȷ	
{	
	unsigned char crc_high;
	unsigned char crc_low;
	unsigned char crc_high_return;
	unsigned char crc_low_return;
	U16 aakj;	

//	sysprintf("ptcom->IQ %d \n",ptcom->IQ);	
	aakj=CalcCrc((U8 *)COMad,ptcom->IQ-2);//����У���Ƿ���ȷ
//	sysprintf("aakj 0x%x \n",aakj);	

	crc_high=aakj/0x100;
	crc_low=aakj-crc_high*0x100;
	crc_high_return=*(U8 *)(COMad+ptcom->IQ-1);
	crc_low_return=*(U8 *)(COMad+ptcom->IQ-2);		
//	sysprintf("crc_high 0x%x,crc_low 0x%x,crc_high_return 0x%x,crc_low_return 0x%x\n",crc_high,crc_low,crc_high_return,crc_low_return);	

	if (crc_high==crc_high_return && crc_low==crc_low_return)	//У����ȷ
	{
//	sysprintf("here \n");	

		if (ptcom->R_W_Flag==*(U8 *)(COMad+0) || *(U8 *)(COMad+0)==0)	//��HMIվ��ַһ��,���߹㲥ģʽ
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}	
	else		//CRCУ�����
	{

//	sysprintf("here error\n");	
		if (ptcom->R_W_Flag!=*(U8 *)(COMad+0) || *(U8 *)(COMad+0)==0)	//վ��ַΪ0ʱ����������
		{
			return 0;
		}
		else
		{
			return 8;				//CRCErrorSend()
		}
	}
	return 0;	
}

/*********************************
���մ�����
**********************************/
void errorsend(int func,int type)
{
	unsigned char crc_high;
	unsigned char crc_low;
	U16 aakj;
	
	*(U8 *)(AD1+0)=ptcom->R_W_Flag;		//HMI��ַ
	switch (type)
	{
	case 8:		//crc error
		*(U8 *)(AD1+1)=0x80+func;		//���ش�������
		*(U8 *)(AD1+2)=0x08;			//crc error		
		break;
	case 1:		//function error
		*(U8 *)(AD1+1)=0x80+func;		//���ش�������
		*(U8 *)(AD1+2)=0x01;			//function error			
		break;	
	case 2:		//address error
		*(U8 *)(AD1+1)=0x80+func;		//���ش�������
		*(U8 *)(AD1+2)=0x02;			//address error		
		break;
	case 3:		//data error
		*(U8 *)(AD1+1)=0x80+func;		//���ش�������
		*(U8 *)(AD1+2)=0x03;			//data error			
		break;		
	default:// error
		*(U8 *)(AD1+1)=0x80+func;		//���ش�������
		*(U8 *)(AD1+2)=0x04;			// error		
		break;				
	}
	aakj=CalcCrc((U8 *)AD1,3);		//����У��
	crc_high=(aakj>>8)&0xff;
	crc_low=aakj&0xff;
	*(U8 *)(AD1+3)=crc_low;		//CRCУ����ֽ�
	*(U8 *)(AD1+4)=crc_high;		//CRCУ����ֽ�	
	ptcom->send_length[0]=5;		//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
	ptcom->send_times=1;					//���ʹ���
			
	ptcom->return_length[0]=0;				//�������ݳ���
	ptcom->return_start[0]=0;				//����������Ч��ʼ
	ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
	ptcom->Current_Times=0;					//��ǰ���ʹ���			
}

int senddata()	//�������ݵ�����
{
	int result=0;
	switch (*(U8 *)(COMad+1))		//������
	{
	case 01:						//��ȡLB������
	case 02:						//��ȡLB������	
		result=slave_send_LB();	
		break;
	case 03:						//��ȡLW������
	case 04:						//��ȡLW������
		result=slave_send_LW();		
		break;	
	case 05:						//����LB	
		result=slave_write_LB();
		break;
	case 0x06:						//д�뵥��LW	
	case 0x10:						//д������LW		
		result=slave_write_LW();
		break;	
	default:
		errorsend(*(U8 *)(COMad+1),1);		//�����𲻴��ڴ�����
		result=1;												
	}
	if (*(U8 *)(COMad+0)==0)		//�㲥ģʽ
	{
		result=0;					//�㲥ģʽ�²����ش���
	}
	return result;
}

int slave_send_LB()					//����LB�����ݵ���վ
{
	U8 staradd_low;
	U8 staradd_high;	
	U8 length_low;
	U8 length_high;
	U16 length;
	U16 staradd;
	int i,j,k;
	U8 charGroup[255];
	unsigned char crc_high;
	unsigned char crc_low;
	U16 aakj;
	
	staradd_low=*(U8 *)(COMad+3);		//��ַ
	staradd_high=*(U8 *)(COMad+2);	
	staradd=(staradd_high<<8)+staradd_low;
	length_low=*(U8 *)(COMad+5);		//����,��λ��λ
	length_high=*(U8 *)(COMad+4);	
	length=(length_high<<8)+length_low;
	
	if ((staradd+length)>=4096)//��ַ������Χ
	{
		errorsend(*(U8 *)(COMad+1),2);		//������
		return 1;
	}
	if (length%8==0)		//�պ�����
	{
		length=length/8;
	}
	else if (length%8!=0)		//������
	{
		length=length/8+1;
	}	
	for (i=0;i<length;i++)	//����
	{
		charGroup[i]=0;		//��λ
		for (j=0;j<8;j++)
		{
			charGroup[i]=(((*(U8 *)(LB+staradd+i*8+j))&0x01)<<j)+charGroup[i];
		}
	}
//׼����������
	*(U8 *)(AD1+0)=ptcom->R_W_Flag;		//HMI��ַ
	*(U8 *)(AD1+1)=*(U8 *)(COMad+1);		//������
	*(U8 *)(AD1+2)=length&0xff;		//���ͳ���
	for (i=0;i<length;i++)
	{
		*(U8 *)(AD1+3+i)=charGroup[i];		//���ͳ���
	}	
	aakj=CalcCrc((U8 *)AD1,3+length);		//����У��
	crc_high=(aakj>>8)&0xff;
	crc_low=aakj&0xff;
	*(U8 *)(AD1+3+length)=crc_low;		//CRCУ����ֽ�
	*(U8 *)(AD1+4+length)=crc_high;		//CRCУ����ֽ�	
//׼����������
	k=4+length+1;
	ptcom->send_length[0]=4+length+1;		//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
	ptcom->send_times=1;					//���ʹ���
			
	ptcom->return_length[0]=0;				//�������ݳ���
	ptcom->return_start[0]=0;				//����������Ч��ʼ
	ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	return 1;
}

/*************************
Э��涨LW��Modbus��ַ��0-4095
RWI�ĵ�ַ��4096-8191
*************************/
int slave_send_LW()					//����LW�����ݵ���վ
{
	U8 staradd_low;
	U8 staradd_high;	
	U8 length_low;
	U8 length_high;
	U16 length;
	U16 staradd;
	int i,k;
	U8 charGroup[512];
	unsigned char crc_high;
	unsigned char crc_low;
	U16 aakj;
	
	staradd_low=*(U8 *)(COMad+3);		//��ַ
	staradd_high=*(U8 *)(COMad+2);	
	staradd=(staradd_high<<8)+staradd_low;
	length_low=*(U8 *)(COMad+5);		//����,��λ����
	length_high=*(U8 *)(COMad+4);	
	length=(length_high<<8)+length_low;
	
	if ((staradd+length)>8191)//��ַ������Χ
	{
		errorsend(*(U8 *)(COMad+1),2);		//������
		return 1;
	}	
	if (staradd<4096)		//������LW������
	{
		for (i=0;i<length;i++)	//����
		{
			charGroup[i*2+0]=((*(U16 *)(LW+(staradd+i)*2))>>8)&0xff;		//��λ
			charGroup[i*2+1]=((*(U16 *)(LW+(staradd+i)*2)))&0xff;		//��λ		
		}	
	}
	else if (staradd>=4096)	//������RWI������
	{
		staradd=staradd-4096;
		for (i=0;i<length;i++)	//����
		{
			charGroup[i*2+0]=((*(U16 *)(RWI+(staradd+i)*2))>>8)&0xff;		//��λ
			charGroup[i*2+1]=((*(U16 *)(RWI+(staradd+i)*2)))&0xff;		//��λ		
		}
	}
	

//׼����������
	*(U8 *)(AD1+0)=ptcom->R_W_Flag;		//HMI��ַ
	*(U8 *)(AD1+1)=*(U8 *)(COMad+1);		//������
	*(U8 *)(AD1+2)=(length*2)&0xff;		//���ͳ���,��λ���ֽ�
	for (i=0;i<length*2;i++)
	{
		*(U8 *)(AD1+3+i)=charGroup[i];		//���ͳ���
	}	
	aakj=CalcCrc((U8 *)AD1,3+length*2);		//����У��
	crc_high=(aakj>>8)&0xff;
	crc_low=aakj&0xff;
	*(U8 *)(AD1+3+length*2)=crc_low;		//CRCУ����ֽ�
	*(U8 *)(AD1+4+length*2)=crc_high;		//CRCУ����ֽ�	
		
	k=5+length*2;							//���͵����ڵ��ֽ���
//׼����������
	ptcom->send_length[0]=k;			//���ͳ���
	ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
	ptcom->send_times=1;					//���ʹ���
			
	ptcom->return_length[0]=0;				//�������ݳ���
	ptcom->return_start[0]=0;				//����������Ч��ʼ
	ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
	ptcom->Current_Times=0;					//��ǰ���ʹ���	
	return 1;
}

int slave_write_LB()					//����LB����ֵ
{
	U8 staradd_low;
	U8 staradd_high;	
	U8 V1;
	U8 V2;
	U16 staradd;
	int i,k;
	int Finish=0;
	
	staradd_low=*(U8 *)(COMad+3);		//��ַ
	staradd_high=*(U8 *)(COMad+2);	
	staradd=(staradd_high<<8)+staradd_low;
	V1=*(U8 *)(COMad+4);		//��λ���Ǹ�λ
	V2=*(U8 *)(COMad+5);
	if (staradd>=4096)//��ַ������Χ
	{
		errorsend(*(U8 *)(COMad+1),2);		//������
		return 1;
	}
	if (V1==0xff && V2==0)	//��λ
	{
		*(U8 *)(LB+staradd)=1;
		Finish=1;
	}
	else if (V1==0 && V2==0)			//��λ
	{
		*(U8 *)(LB+staradd)=0;
		Finish=1;
	}
	else
	{
		errorsend(*(U8 *)(COMad+1),3);		//������
		return 1;
	}
//׼����������
	if (Finish==1)						//д�����ֵ����ȷ��
	{
		for (i=0;i<8;i++)
		{
			*(U8 *)(AD1+i)=*(U8 *)(COMad+i);
		}
		
		k=8;							//���͵����ڵ��ֽ���
//׼����������
		ptcom->send_length[0]=k;			//���ͳ���
		ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
		ptcom->send_times=1;					//���ʹ���
				
		ptcom->return_length[0]=0;				//�������ݳ���
		ptcom->return_start[0]=0;				//����������Ч��ʼ
		ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
		ptcom->Current_Times=0;					//��ǰ���ʹ���	
		return 1;
	}
	else
		return 0;

}

/*************************
Э��涨LW��Modbus��ַ��0-4095
RWI�ĵ�ַ��4096-8191
*************************/
int slave_write_LW()					//����LW����ֵ
{
	U8 staradd_low;
	U8 staradd_high;
	U8 L1,L2;	
	U16 length;
	U16 staradd;
	int i,k;
	U8 V1,V2;
	unsigned char crc_high;
	unsigned char crc_low;
	U16 aakj;
	
	
	
	if (*(U8 *)(COMad+1)==0x10)				//д���ֽ�
	{
		L1=(*(U8 *)(COMad+4)<<8)+*(U8 *)(COMad+5);	//modbusЭ��涨4��5���ֵ�λ��6���ֽڵ�λ
		L2=*(U8 *)(COMad+6);
		if ((L1*2)!=L2)						//�ֽڵ�λ���ֵ�λ��ƥ��
		{
			errorsend(*(U8 *)(COMad+1),3);		//������
			return 1;
		}
		if (ptcom->IQ!=L2+9)
		{
			errorsend(*(U8 *)(COMad+1),3);		//���յ������ݳ�����ʵ�ʲ�һ��
			return 1;			
		}
	}
	
	if (*(U8 *)(COMad+1)==0x06)
	{
		if (ptcom->IQ!=8)
		{
			errorsend(*(U8 *)(COMad+1),3);		//���յ������ݳ�����ʵ�ʲ�һ��
			return 1;			
		}		
	}

	staradd_low=*(U8 *)(COMad+3);		//��ַ
	staradd_high=*(U8 *)(COMad+2);	
	staradd=(staradd_high<<8)+staradd_low;
	
	if (*(U8 *)(COMad+1)==0x06)			//д�뵥���Ĵ���
	{
		length=1;
		k=4;
	}
	else
	{
		length=L1;
		k=7;
	}
	if((staradd+length)>=8192)					//��ַ������Χ
	{
		errorsend(*(U8 *)(COMad+1),2);		//������
		return 1;	
	}	
	if(staradd<4096)			//LW
	{
		for (i=0;i<length;i++)
		{
			V1=*(U8 *)(COMad+k+i*2+0);
			V2=*(U8 *)(COMad+k+i*2+1);			
			*(U16 *)(LW+(staradd+i)*2)=(V1<<8)+V2;
		}		
	}
	else if(staradd>=4096)			//RWI
	{
		staradd=staradd-4096;
		for (i=0;i<length;i++)
		{
			V1=*(U8 *)(COMad+k+i*2+0);
			V2=*(U8 *)(COMad+k+i*2+1);			
			*(U16 *)(RWI+(staradd+i)*2)=(V1<<8)+V2;
		}
	}	
	if (*(U8 *)(COMad+1)==0x06)		//�����������0x06,�򷵻غͷ��ʹ���һ���Ķ���
	{
		for (i=0;i<8;i++)
		{
			*(U8 *)(AD1+i)=*(U8 *)(COMad+i);
		}
		
		k=8;							//���͵����ڵ��ֽ���
//׼����������
		ptcom->send_length[0]=k;			//���ͳ���
		ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
		ptcom->send_times=1;					//���ʹ���
				
		ptcom->return_length[0]=0;				//�������ݳ���
		ptcom->return_start[0]=0;				//����������Ч��ʼ
		ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
		ptcom->Current_Times=0;					//��ǰ���ʹ���	
		return 1;
	}
	else if (*(U8 *)(COMad+1)==0x10)		//��������0x10ʱ��������������
	{
		*(U8 *)(AD1+0)=*(U8 *)(COMad+0);
		*(U8 *)(AD1+1)=*(U8 *)(COMad+1);		
		*(U8 *)(AD1+2)=*(U8 *)(COMad+2);		
		*(U8 *)(AD1+3)=*(U8 *)(COMad+3);		
		*(U8 *)(AD1+4)=*(U8 *)(COMad+4);		
		*(U8 *)(AD1+5)=*(U8 *)(COMad+5);	
		aakj=CalcCrc((U8 *)AD1,6);		//����У��
		crc_high=(aakj>>8)&0xff;
		crc_low=aakj&0xff;
		*(U8 *)(AD1+6)=crc_low;		//CRCУ����ֽ�
		*(U8 *)(AD1+7)=crc_high;		//CRCУ����ֽ�					
	
		k=8;					//���͵����ڵ��ֽ���
//׼����������
		ptcom->send_length[0]=k;			//���ͳ���
		ptcom->send_staradd[0]=0;				//�������ݴ洢��ַ	
		ptcom->send_times=1;					//���ʹ���
				
		ptcom->return_length[0]=0;				//�������ݳ���
		ptcom->return_start[0]=0;				//����������Ч��ʼ
		ptcom->return_length_available[0]=0;	//������Ч���ݳ���	
		ptcom->Current_Times=0;					//��ǰ���ʹ���	
		return 1;	
	}
	return 0;
}

U16 CalcCrc(unsigned char *chData,unsigned short uNo)
{
	U16 crc=0xffff;
	U16 i,j;
	for(i=0;i<uNo;i++)
	{
	  crc^=chData[i];
	  for(j=0;j<8;j++)
	  {
	    if(crc&1)
		{
	     crc>>=1;
		 crc^=0xA001;
		}
	    else
		 crc>>=1;
	  }
	}
	return (crc);
}
