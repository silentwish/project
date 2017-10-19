 

#include "stdio.h"
#include "def.h"
#include "smm.h"


	
struct Com_struct_D *ptcom;



void Enter_PlcDriver(void)
{

	ptcom=(struct Com_struct_D *)adcom;
		    
	switch (ptcom->R_W_Flag)
	{
	case PLC_READ_DATA:				        //���������Ƕ�����
	case PLC_READ_DIGITALALARM:			        //���������Ƕ�����,����	
	case PLC_READ_TREND:			        //���������Ƕ�����,����ͼ
	case PLC_READ_ANALOGALARM:			        //���������Ƕ�����,��ȱ���	
	case PLC_READ_CONTROL:			        //���������Ƕ�PLC���Ƶ�����	
		switch(ptcom->registerr)
		{
          case 'X':             //X--����̵���
          case 'Y':             //Y--����̵���
          case 'M':             //M--�����̵���
          case 'H':             //S
          case 'K':             //SM--�����̵���
          case 'T':             //T  --��ʱ�ӵ�
          case 'C':		        //C  --�����ӵ�
          case 'J':             //T*--��ʱ��Ȧ
          case 'L':             //C*--������Ȧ
                Read_Bool();     //���������Ƕ�λ����       
                break;
          case 'D':             //D
          case 'R':             //R
          case 'N':             //SD
          case 't':             //TV
          case 'c':             //CV
          case 'v':             //CV32
          case 'd':             //D_Bit
          case 'r':             //R_Bit
               Read_Analog();    //���������Ƕ�ģ������ 
               break;			
		}
		break;
	case PLC_WRITE_DATA:				
		switch(ptcom->registerr)
		{
          case 'Y':             //Y--����̵���
          case 'M':             //M--�����̵���
          case 'H':             //SM
          case 'K':             //S--�����̵���
          case 'T':             //T  --��ʱ�ӵ�
          case 'C':             //C  --�����ӵ�
          case 'J':             //T*--��ʱ��Ȧ
          case 'L':             //C*--������Ȧ
          case 'd':             //D_Bit
          case 'r':             //R_Bit
                Set_Reset();    //����������ǿ����λ�͸�λ
                break;
          case 'D':             //D
          case 'R':             //R
          case 'N':             //SD
          case 't':             //TV
          case 'c':             //CV
          case 'v':             //CV32
               Write_Analog();	//����������дģ������	
                break;			
		}
		break;	
	case PLC_WRITE_TIME:				        //����������дʱ�䵽PLC
		switch(ptcom->registerr)
		{
          case 'D':
                Write_Time();		
                break;			
		}
		break;	
	case PLC_READ_TIME:				         //���������Ƕ�ȡʱ�䵽PLC
		switch(ptcom->registerr)
		{
          case 'D':		
                Read_Time();		
                break;			
		}
		break;
	case PLC_WRITE_RECIPE:				         //����������д�䷽��PLC
		switch(*(U8 *)(PE+3))   //�䷽�Ĵ�������
		{
           case 'D':		
                Write_Recipe();		
                break;			
		}
		break;
	case PLC_READ_RECIPE:				         //���������Ǵ�PLC��ȡ�䷽
		switch(*(U8 *)(PE+3))   //�䷽�Ĵ�������
		{
          case 'D':		
               Read_Recipe();		
               break;			
		}
		break;							
	case PLC_CHECK_DATA:				         //�������������ݴ���
		watchcom();
		break;				
	}	 
}



 void Set_Reset()
{
	U16 aakj;
	int b,b1;
	char a1,a2,a3,a4,a5,a6;
	int aa1;
	int plcadd,orderNum;
    int i=0,j=0,k=0,nLen=0;
    
	b=ptcom->address;			 // ��ʼ��λ��ַ
	plcadd=ptcom->plc_address;	 //PLCվ��ַ

	switch (ptcom->registerr)	 //���ݼĴ������ͻ��ƫ�Ƶ�ַ
	{
     case 'Y':             //Y--����̵���
          orderNum=0x91;
          break;
     case 'M':             //M--�����̵���
          orderNum=0x92;
          break;
     case 'H':             //S
         orderNum=0x93;
         break;
     case 'K':             //SM--�����̵���
         orderNum=0x94;
         break; 
     case 'T':                          //T :��ʱ�ӵ�
         orderNum=0x99; 
         break;
     case 'C':                          //C:�����ӵ�
         orderNum=0x9d; 
         break; 
     case 'J':                          //T* :��ʱ��Ȧ
         orderNum=0x98; 
         break; 
     case 'L':                          //C* :������Ȧ
         orderNum=0x9c; 
         break;          
     case 'd':                          //D_Bit
         orderNum=0x95; 
         break; 
     case 'r':                          //R_Bit
         orderNum=0x97; 
         break; 
	}
	
     if(ptcom->registerr=='Y') //ת����8����
     {
         a1=b&0x7;         //��λ
         a2=(b>>3)&0x7;    //ʮλ
         a3=(b>>6)&0x7;    //��λ
         a4= 0;            //ǧλ
         a5= 0;            //��λ
         a6= 0;            //ʮ��
         
         a2=(a2<<4)&0xf0;  //ת��hex����λ��ǰ����λ�ں�
         a2=a2|a1;
         
         a4=a3;
         a6=0;
     }
     else if(ptcom->registerr=='d' || ptcom->registerr=='r')
     {
       b1=b%16;
       b=b/16;
       aa1 = Asc_To_Hex6(b);               //��ַת����ʮ����12345.12ת����0x12345C
       aa1=(unsigned int)aa1;
       
       a2 = (aa1<<4)&0xf0;              //0x5
       a2 |=(b1&0xf) ;                  //0x5c
       
       a4 = aa1>>4;                     //0x34
       a6 = aa1>>12;                     //0x12
     }
     else
     {

       if(ptcom->registerr =='K')
       {
         if(b>=9000)     //��λ��9000��ʼ��ʵ�ʷ��͵ĵ�ַ��0��ʼ��
         {
           b=b-9000;
         }
         else
         {
           b=0;
         }
       }
       
       aa1 = Asc_To_Hex6(b);           //��ַת����ʮ����123456ת����0x123456
       
       aa1=(unsigned int)aa1;
       a2 = aa1;                       //0x56
       a4 = aa1>>8;                    //0x34
       a6 = aa1>>16;                   //0x12
     }

     *(U8 *)(AD1+0)=0x10;               //DLE
     *(U8 *)(AD1+1)=0x02;               //STX
     *(U8 *)(AD1+2)=plcadd;             //վ��
     *(U8 *)(AD1+3)=0x09;               //����L      
     *(U8 *)(AD1+4)=0x00;               //����H
     *(U8 *)(AD1+5)=0x29;               //�����룬λԪд��
     *(U8 *)(AD1+6)=orderNum;           //Ԫ������
     *(U8 *)(AD1+7)=a2;                 //��ʼ��ַL
     *(U8 *)(AD1+8)=a4;                 //��ʼ��ַM
     *(U8 *)(AD1+9)=a6;                 //��ʼ��ַH
     *(U8 *)(AD1+10)=0x01;              //�Ĵ�������L
     *(U8 *)(AD1+11)=0x00;              //�Ĵ�������H
     if (ptcom->writeValue==1)          //��λ
     {
        *(U8 *)(AD1+12)=0x01;          //����L
        *(U8 *)(AD1+13)=0x00;          //����H       
     }
     if (ptcom->writeValue==0)         //��λ
     {
         *(U8 *)(AD1+12)=0x00;         //����L
         *(U8 *)(AD1+13)=0x00;         //����H         
     } 

     *(U8 *)(AD1+14)=0x10;              //DLE
     *(U8 *)(AD1+15)=0x03;              //ETX
     
      aakj=CalcHe((U8 *)(AD1+2),12);    //У��ͣ���λ��ǰ����λ�ں�         
      a1=(aakj>>4)&0xf;
      a2=aakj&0xf;
      a1=asicc(a1);
      a2=asicc(a2);
      
      *(U8 *)(AD1+16)=a1;                //У���H
      *(U8 *)(AD1+17)=a2;                //У���L

     for(i=2;i<14;i++)                   //��ȥ��ʼ������룬��У�����⣬Ϊ0X10���ֽ���Ŀ��
     {
       if(*(U8 *)(AD1+i)==0x10)
       {
         nLen++;
       }
     }
     for(i=2;i<14+nLen;i++)
     {
       if((*(U8 *)(AD1+i)==0x10)&&(*(U8 *)(AD1+i+1)!=0x03))          //�ҵ�Ϊ0X10�����ݡ�
       {
         k++;
         for(j=17+k;j>i;j--)               //������
         {
          *(U8 *)(AD1+j)=*(U8 *)(AD1+j-1);
         }
         
         *(U8 *)(AD1+i+1)=0x10;            //����һ��0X10�ֽڡ�
         i++;
       }
     }
      
      ptcom->send_length[0]=18+nLen;       //���ͳ���
      ptcom->send_staradd[0]=0;            //�������ݴ洢��ַ  
          
      ptcom->return_start[0]=0;            //����������Ч��ʼ
      ptcom->return_length[0]=10;          //�������ݳ��ȣ���10���̶���
      ptcom->return_length_available[0]=0; //������Ч���ݳ���    
      
      ptcom->send_add[0]=ptcom->address;                      //�����������ַ������
      ptcom->send_times=1;                 //���ʹ���
      ptcom->Current_Times=0;              //��ǰ���ʹ���  

     ptcom->send_staradd[99]=0;      
          
}



 void Read_Bool()				    //��ȡ����������Ϣ
{
	U16 aakj;
	int b;
	char a1,a2,a3,a4,a5,a6;
    int aa1;
	int plcadd;
	int len;
	int regLen;
    int orderNum;
    int i=0,j=0,nLen=0;
    int sendNum=0;
	//U32 lw = *(U32*)(lw_word + 0);    /*������Ϣ*/

	
	plcadd=ptcom->plc_address;	        //PLCվ��ַ	
	b=ptcom->address;					// ��ʼ��ȡ��ַ
	len=ptcom->register_length;		    // ��ʼ��ȡ����
		
	switch (ptcom->registerr)	        //���ݼĴ������ͻ��ƫ�Ƶ�ַ
	{
     case 'X':                          //X--����̵���
          orderNum=0x90;
          break;	
     case 'Y':                          //Y--����̵���
          orderNum=0x91;
          break;
     case 'M':                          //M--�����̵���
          orderNum=0x92;
          break;
     case 'H':                          //S
         orderNum=0x93;
         break;	
     case 'K':                          //SM--�����̵���
         orderNum=0x94;
         break; 
     case 'T':                          //T :��ʱ�ӵ�
          orderNum=0x99; 
          break;
     case 'C':                          //C:�����ӵ�
         orderNum=0x9d; 
         break;	
     case 'J':                          //T* :��ʱ��Ȧ
         orderNum=0x98; 
         break; 
     case 'L':                          //C* :������Ȧ
         orderNum=0x9c; 
         break;          
	}
	
		
    if(ptcom->registerr=='X' || ptcom->registerr=='Y' ) //ת����8����
    {
      a1=b&0x7;                          //��λ
      a2=(b>>3)&0x7;                     //ʮλ
      a3=(b>>6)&0x7;                     //��λ
      a4= 0;                             //ǧλ
      a5= 0;                             //��λ
      a6= 0;                             //ʮ��
      
      a2=(a2<<4)&0xf0;                   //ת��hex����λ��ǰ����λ�ں�
      a2=a2|a1;
      a4=a3;
      a6=0;
      
      sendNum=0x21;
    }
    else
    {    
       if(ptcom->registerr =='K')
       {
         if(b>=9000)     //��λ��9000��ʼ��ʵ�ʷ��͵ĵ�ַ��0��ʼ��
         {
           b=b-9000;
         }
         else
         {
           b=0;
         }
       }
       
      aa1 = Asc_To_Hex6(b);               //��ַת����ʮ����123456ת����0x123456

      aa1=(unsigned int)aa1;
      a2 = aa1;                           //0x56
      a4 = aa1>>8;                        //0x34
      a6 = aa1>>16;                       //0x12
      
      sendNum=0x21;
    }


    if(ptcom->registerr=='T' || ptcom->registerr=='J' ||ptcom->registerr=='K') 
    {
       if(b>=504)  //����Զ�ȡ512���ٳ�Խ�籨��
       {
           regLen=8;
       }
       else
       {
           regLen=(len+1)*8;
       }
    }
    else if(ptcom->registerr=='M')
    {
        
        if(b>=8184)  //����Զ�ȡ8191���ٳ�Խ�籨��
        {
            regLen=8;
        }
        else
        {
            regLen=(len+1)*8;
        }
    }
    else if(ptcom->registerr=='H')
    {
        
        if(b>=4088)  //����Զ�ȡ4088���ٳ�Խ�籨��
        {
            regLen=8;
        }
        else
        {
            regLen=(len+1)*8;
        }
    }
    else
    {
        if(len<2)
        {
            regLen=(len+1)*8;
        }
        else
        {
            regLen=len*8;
        }
    }

    
    

	*(U8 *)(AD1+0)=0x10;        	       //DLE
	*(U8 *)(AD1+1)=0x02;                   //STX
	*(U8 *)(AD1+2)=plcadd;                 //վ��
	*(U8 *)(AD1+3)=0x07;   			       //����L    	
	*(U8 *)(AD1+4)=0x00;			       //����H
	*(U8 *)(AD1+5)=sendNum;   			       //������
	*(U8 *)(AD1+6)=orderNum;		       //Ԫ������
	*(U8 *)(AD1+7)=a2;		               //��ʼ��ַL
	*(U8 *)(AD1+8)=a4;		               //��ʼ��ַM
	*(U8 *)(AD1+9)=a6;	                   //��ʼ��ַH
	*(U8 *)(AD1+10)=regLen&0xff;           //�Ĵ�������L
	*(U8 *)(AD1+11)=(regLen>>8)&0xff;      //�Ĵ�������H
	*(U8 *)(AD1+12)=0x10;		           //DLE
	*(U8 *)(AD1+13)=0x03;	                //ETX
	
	aakj=CalcHe((U8 *)(AD1+2),10);         //У��ͣ���λ��ǰ����λ�ں�         
	a1=(aakj>>4)&0xf;
	a2=aakj&0xf;
    a1=asicc(a1);
    a2=asicc(a2);
    
	*(U8 *)(AD1+14)=a1;                     //У���H
	*(U8 *)(AD1+15)=a2;                     //У���L

    nLen = 0;                               //������0X10�ĸ���
    for(i=2;i<12+nLen;i++)
    {
      if((*(U8 *)(AD1+i)==0x10)&&(*(U8 *)(AD1+i+1)!=0x03))          //�ҵ�Ϊ0X10�����ݡ�
      {
        nLen++;
        for(j=15+nLen;j>i;j--)                //������
        {
         *(U8 *)(AD1+j)=*(U8 *)(AD1+j-1);
        }
        i++;
        *(U8 *)(AD1+i)=0x10;                 //����һ��0X10�ֽڡ�
      }
    }

	
	ptcom->send_length[0]=16+nLen;		      //���ͳ���
	ptcom->send_staradd[0]=0;				  //�������ݴ洢��ַ	

	ptcom->return_start[0]=6;				  //�������ݿ�ʼ		
	ptcom->return_length[0]=10+2*len;         //�������ݳ��ȣ���10���̶���
	ptcom->return_length_available[0]=len+1;  //������Ч���ݳ���	

	ptcom->send_times=1;					  //���ʹ���
	ptcom->Current_Times=0;					  //��ǰ���ʹ���
	
    if(ptcom->registerr=='d' || ptcom->registerr=='r')
    {
        ptcom->send_staradd[99] = 7;  
    }
    else
    {
        ptcom->send_staradd[99] = 0;  
    }
}


 void Read_Analog()				          //��ģ����
{
	U16 aakj;
	int b;
	char a1,a2,a4,a6;
	int aa1;
	int plcadd;
	int len;
    int orderNum;
    int i=0,j=0,regLen=0;
    int nLen=0;
   // U32 lw = *(U32*)(lw_word + 0);    /*������Ϣ*/
    
	plcadd=ptcom->plc_address;	  //PLCվ��ַ	
	b=ptcom->address;			  //��ʼ��ȡ��ַ
	len=ptcom->register_length;	      // ��ʼ��ȡ����

	switch (ptcom->registerr)	//���ݼĴ������ͻ��ƫ�Ƶ�ַ
	{
	 case 'D':              //D
           orderNum=0xA0;
		   break;
     case 'R':              //R
           orderNum=0xA2;
           break;  
     case 'N':              //SD
           orderNum=0xA1;
           break;   
	 case 't':			    //TV
           orderNum=0xA8;
		   break;
	 case 'c':			    //CV
            orderNum=0xAC;
		    break;
     case 'v':             //CV32
          orderNum=0xAD;
            break;
    case 'd':             //D_Bit
         orderNum=0xA0; 
         break; 
    case 'r':            //R_Bit
         orderNum=0xA2; 
         break;


	}
	if( (ptcom->registerr =='d') || (ptcom->registerr =='r') )
	{
      b/=16;
	}
	else if(ptcom->registerr =='N')
	{
       if(b>=9000)     //��λ��9000��ʼ��ʵ�ʷ��͵ĵ�ַ��0��ʼ��
       {
         b=b-9000;
       }
       else
       {
         b=0;
       }
	}

	
    aa1 = Asc_To_Hex6(b);             //��ַת����ʮ����123456ת����0x123456
    
    aa1=(unsigned int)aa1;
    a2 = aa1;                         //0x56
    a4 = aa1>>8;                      //0x34
    a6 = aa1>>16;                     //0x12
    
    regLen=len;
    
    *(U8 *)(AD1+0)=0x10;                      //DLE
    *(U8 *)(AD1+1)=0x02;                      //STX
    *(U8 *)(AD1+2)=plcadd;                    //վ��
    *(U8 *)(AD1+3)=0x07;                      //����L      
    *(U8 *)(AD1+4)=0x00;                      //����H
    *(U8 *)(AD1+5)=0x20;                      //������
    *(U8 *)(AD1+6)=orderNum;                  //Ԫ������
    *(U8 *)(AD1+7)=a2;                        //��ʼ��ַL
    *(U8 *)(AD1+8)=a4;                        //��ʼ��ַM
    *(U8 *)(AD1+9)=a6;                        //��ʼ��ַH
    *(U8 *)(AD1+10)=regLen&0xff;              //Ԫ����ĿL
    *(U8 *)(AD1+11)=(regLen>>8)&0xff;         //Ԫ����ĿH
    *(U8 *)(AD1+12)=0x10;                     //DLE
    *(U8 *)(AD1+13)=0x03;                     //ETX
    
     aakj=CalcHe((U8 *)(AD1+2),10);           //У��ͣ���λ��ǰ����λ�ں�         
     a1=(aakj>>4)&0xf;
     a2=aakj&0xf;
     a1=asicc(a1);
     a2=asicc(a2);
     
     *(U8 *)(AD1+14)=a1;                        //У���H
     *(U8 *)(AD1+15)=a2;                        //У���L

     for(i=2;i<12+nLen;i++)
     {
       if((*(U8 *)(AD1+i)==0x10)&&(*(U8 *)(AD1+i+1)!=0x03))    //�ҵ�Ϊ0X10�����ݡ�
       {
         nLen++;
         for(j=15+nLen;j>i;j--)                 //������
         {
          *(U8 *)(AD1+j)=*(U8 *)(AD1+j-1);
         }
         
         *(U8 *)(AD1+i+1)=0x10;                 //����һ��0X10�ֽڡ�
         i++;
       }
     }

     if(ptcom->registerr=='v')
     {
         ptcom->return_start[0]=6;                   //����������Ч��ʼ
         ptcom->return_length[0]=-1;                 //�������ݳ��ȣ���10���̶���
         ptcom->return_length_available[0]=regLen*4; //������Ч���ݳ���  
         ptcom->send_staradd[99] = 7;
     }
     else if( (ptcom->registerr =='d') || (ptcom->registerr =='r') )
     {
         ptcom->return_start[0]=6;                   //����������Ч��ʼ
         ptcom->return_length[0]=-1;                 //�������ݳ��ȣ���10���̶���
         ptcom->return_length_available[0]=regLen*2; //������Ч���ݳ���  
         ptcom->send_staradd[99] = 0;

         ptcom->address=b*16;                        //��λ����������addr*16+bit��
         ptcom->register_length=len*2;               //�����ֽڡ�
     }
     else
     {
         ptcom->return_start[0]=6;                   //����������Ч��ʼ
         ptcom->return_length[0]=-1;                 //�������ݳ��ȣ���10���̶���
         ptcom->return_length_available[0]=regLen*2; //������Ч���ݳ���  
         ptcom->send_staradd[99] = 7;
     }

     ptcom->send_length[0]=16+nLen;                 //���ͳ���
     ptcom->send_staradd[0]=0;                      //�������ݴ洢��ַ  

     ptcom->send_times=1;                           //���ʹ���
     ptcom->Current_Times=0;                        //��ǰ���ʹ���
}


 void Read_Recipe()						      //��ȡ�䷽
{
	U16 aakj;
	int b,i;
	char a1,a2,a4,a6;
	int aa1;
	int plcadd,ps,ps1=0,staradd,SendTimes,LastTimeWord;
	int sendlength,datalength;
	int j=0,k=0;
	int nLen=0;
	
	plcadd=ptcom->plc_address;	              //PLCվ��ַ	
	b=ptcom->address;						  // ��ʼ��ȡ��ַ
	datalength=ptcom->register_length;		  // ��ʼ��ȡ����
	
	if(datalength%24==0)                       
	{
		SendTimes=datalength/24;               //���͵Ĵ���Ϊdatalength/24              
		LastTimeWord=24;                       //���һ�η��͵ĳ���Ϊ24��D
	}
	if(datalength%24!=0)                       //���ݳ��Ȳ���24�ı���ʱ
	{
		SendTimes=datalength/24+1;             //���͵Ĵ���datalength/24+1
		LastTimeWord=datalength%24;            //���һ�η��͵ĳ���Ϊ��24������
	}

    
	ps=20;
	for (i=0;i<SendTimes;i++)
	{		
		staradd=b+i*24;                        //��ʼ��ַ
		
		if (i!=(SendTimes-1))			       //�������һ��
		{
			sendlength=24;
		}
		else
		{
			sendlength=LastTimeWord;
		}
		
        aa1 = Asc_To_Hex6(staradd);           //��ַת����ʮ����123456ת����0x123456
        
        aa1=(unsigned int)aa1;
        a2 = aa1;                             //0x56
        a4 = aa1>>8;                          //0x34
        a6 = aa1>>16;                         //0x12
				
        *(U8 *)(AD1+ps*i+ps1+0)=0x10;                   //DLE
        *(U8 *)(AD1+ps*i+ps1+1)=0x02;                   //STX
        *(U8 *)(AD1+ps*i+ps1+2)=plcadd;                 //վ��
        *(U8 *)(AD1+ps*i+ps1+3)=0x07;                   //����L      
        *(U8 *)(AD1+ps*i+ps1+4)=0x00;                   //����H
        *(U8 *)(AD1+ps*i+ps1+5)=0x20;                   //������
        *(U8 *)(AD1+ps*i+ps1+6)=0xA0;                   //Ԫ������
        *(U8 *)(AD1+ps*i+ps1+7)=a2;                     //��ʼ��ַL
        *(U8 *)(AD1+ps*i+ps1+8)=a4;                     //��ʼ��ַM
        *(U8 *)(AD1+ps*i+ps1+9)=a6;                     //��ʼ��ַH
        *(U8 *)(AD1+ps*i+ps1+10)=sendlength&0xff;       //Ԫ����ĿL
        *(U8 *)(AD1+ps*i+ps1+11)=(sendlength>>8)&0xff;  //Ԫ����ĿH
        *(U8 *)(AD1+ps*i+ps1+12)=0x10;                  //DLE
        *(U8 *)(AD1+ps*i+ps1+13)=0x03;                  //ETX
						
         aakj=CalcHe((U8 *)(AD1+ps*i+ps1+2),10);        //У��ͣ���λ��ǰ����λ�ں�         
         a1=(aakj>>4)&0xf;
         a2=aakj&0xf;
         a1=asicc(a1);
         a2=asicc(a2);
         
         *(U8 *)(AD1+ps*i+ps1+14)=a1;                   //У���H
         *(U8 *)(AD1+ps*i+ps1+15)=a2;                   //У���L
         nLen=0;                                        //������0X10�ĸ���

        for(k=2;k<12+nLen;k++)                          //�ҵ�0X10�����ݣ�Ȼ���ٺ�������һ��0X10
        {
          if((*(U8 *)(AD1+ps*i+ps1+k)==0x10)&&(*(U8 *)(AD1+ps*i+ps1+k+1)!=0x03))            //�ҵ�Ϊ0X10�����ݡ�
          {
            nLen++;
            for(j=15+nLen;j>k;j--)                        //������
            {
             *(U8 *)(AD1+ps*i+ps1+j)=*(U8 *)(AD1+ps*i+ps1+j-1);
            }
            k++;
            *(U8 *)(AD1+ps*i+ps1+k)=0x10;              //����һ��0X10�ֽڡ�
          }
        }  
       

		ptcom->send_length[i]=16+nLen;				    //���ͳ���=ԭʼ����+���ӵ�0X10
		ptcom->send_staradd[i]=ps*i+ps1;			    //�������ݴ洢��ַ	
		ptcom->send_add[i]=b+i*24;		                //�����������ַ������
		ptcom->send_data_length[i]=sendlength;	        //�������һ�ζ���32��D

		ptcom->return_start[i]=6;				        //����������Ч��ʼ			
		ptcom->return_length[i]=-1;                     //�������ݳ��ȣ���10���̶���
		ptcom->return_length_available[i]=sendlength*2;	//������Ч���ݳ���	
		
        ps1+=nLen;
	}
	
	ptcom->send_times=SendTimes;					    //���ʹ���
	ptcom->Current_Times=0;					            //��ǰ���ʹ���
	ptcom->send_staradd[99] = 7; 
}



 void Write_Analog()					      //дģ����
{
	U16 aakj;
	int b,i,k=0;
	char a2,a4,a6;
    int aa1;
	int plcadd;
	int len,len1=0,len2;
	int orderNum=0,j=0;
	int nLen=0;
	U8 c1=0,c2=0,c3=0,c4=0;
	int b1=0,b2=0;
    int dataLen;
    
	plcadd=ptcom->plc_address;	       //PLCվ��ַ	
	b=ptcom->address;				   // ��ʼ��ȡ��ַ
	len=ptcom->register_length;		   // ��ʼ��ȡ����
		
    switch (ptcom->registerr)          //���ݼĴ������ͻ��ƫ�Ƶ�ַ
    {
      case 'D':              //D
            orderNum=0xA0;
            break;
      case 'R':              //R
            orderNum=0xA2;
            break;  
      case 'N':              //SD
            orderNum=0xA1;
            break;   
      case 't':              //TV
            orderNum=0xA8;
            break;
      case 'c':              //CV
             orderNum=0xAC;
             break;
      case 'v':             //CV32
           orderNum=0xAD;
             break;
    }



    if(ptcom->registerr =='N')
	{
       if(b>=9000)     //��λ��9000��ʼ��ʵ�ʷ��͵ĵ�ַ��0��ʼ��
       {
         b=b-9000;
       }
       else
       {
         b=0;
       }
	}
    
    aa1 = Asc_To_Hex6(b);             //��ַת����ʮ����123456ת����0x123456
    
    aa1=(unsigned int)aa1;
    a2 = aa1;                         //0x56
    a4 = aa1>>8;                      //0x34
    a6 = aa1>>16;                     //0x12
    if(ptcom->registerr=='v')
    {
      dataLen=7+len*4;
    }
    else
    {
      dataLen=7+len*2;
    }

    *(U8 *)(AD1+0)=0x10;                  //DLE
    *(U8 *)(AD1+1)=0x02;                  //STX
    *(U8 *)(AD1+2)=plcadd;                //վ��
    *(U8 *)(AD1+3)=dataLen&0xff;          //����L      
    *(U8 *)(AD1+4)=(dataLen>>8)&0xff;     //����H
    *(U8 *)(AD1+5)=0x28;                  //������
    *(U8 *)(AD1+6)=orderNum;              //Ԫ������
    *(U8 *)(AD1+7)=a2;                    //��ʼ��ַL
    *(U8 *)(AD1+8)=a4;                    //��ʼ��ַM
    *(U8 *)(AD1+9)=a6;                    //��ʼ��ַH
    *(U8 *)(AD1+10)=len&0xff;             //Ԫ����ĿL
    *(U8 *)(AD1+11)=(len>>8)&0xff;        //Ԫ����ĿH

    if(ptcom->registerr=='v')
    {
        for(i=0;i<len;i++)
        {
          c1=ptcom->U8_Data[i*4];
          c2=ptcom->U8_Data[i*4+1];
          c3=ptcom->U8_Data[i*4+2];
          c4=ptcom->U8_Data[i*4+3];
          
          *(U8 *)(AD1+12+i*4)=c1;
          *(U8 *)(AD1+13+i*4)=c2;           
          *(U8 *)(AD1+14+i*4)=c3;
          *(U8 *)(AD1+15+i*4)=c4; 
        }
        
        *(U8 *)(AD1+16+(len-1)*4)=0x10;        //DLE
        *(U8 *)(AD1+17+(len-1)*4)=0x03;        //ETX
        
        aakj=CalcHe((U8 *)(AD1+2),10+len*4);   //У��ͣ���λ��ǰ����λ�ں�         
        b1=(aakj>>4)&0xf;
        b2=aakj&0xf;
        b1=asicc(b1);
        b2=asicc(b2);
        
        *(U8 *)(AD1+18+(len-1)*4)=b1;          //У���H
        *(U8 *)(AD1+19+(len-1)*4)=b2;          //У���L
        
        len2 = 15+len*4;
        len1 = 12+len*4;  

    }
    else
    {
        for(i=0;i<len;i++)
        {
          c1=ptcom->U8_Data[i*2];
          c2=ptcom->U8_Data[i*2+1];
          *(U8 *)(AD1+12+i*2)=c1;
          *(U8 *)(AD1+13+i*2)=c2; 
        }
        
        *(U8 *)(AD1+14+(len-1)*2)=0x10;        //DLE
        *(U8 *)(AD1+15+(len-1)*2)=0x03;        //ETX
        
        aakj=CalcHe((U8 *)(AD1+2),10+len*2);   //У��ͣ���λ��ǰ����λ�ں�         
        b1=(aakj>>4)&0xf;
        b2=aakj&0xf;
        b1=asicc(b1);
        b2=asicc(b2);
        
        *(U8 *)(AD1+16+(len-1)*2)=b1;          //У���H
        *(U8 *)(AD1+17+(len-1)*2)=b2;          //У���L
        
        len2 = 15+len*2;
        len1 = 12+len*2;  

    }

    

    for(i=2;i<len1;i++)                     //��ȥ��ʼ������룬��У�����⣬Ϊ0X10���ֽ���Ŀ��
    {  
      if(*(U8 *)(AD1+i)==0x10)
      {
        nLen++;                             //������Ϊ0X10�ĸ�����
      }
    }

    for(i=2;i<len1;i++)                     //�ҵ�0X10�����ݣ�Ȼ���ٺ�������һ��0X10
    {
        if((*(U8 *)(AD1+i)==0x10)&&(*(U8 *)(AD1+i+1)!=0x03))          //�ҵ�Ϊ0X10�����ݡ�
      {
        k++;
        for(j=len2+k;j>i;j--)               //������
        {
         *(U8 *)(AD1+j)=*(U8 *)(AD1+j-1);
        }
        
        *(U8 *)(AD1+i+1)=0x10;              //����һ��0X10�ֽڡ�
        i++;
      }
    }

     if(ptcom->registerr=='v')
     {
        ptcom->send_length[0]=16+len*4+nLen;    //���ͳ���
     }
     else
     {
        ptcom->send_length[0]=16+len*2+nLen;    //���ͳ���
     }
     
     ptcom->send_staradd[0]=0;               //�������ݴ洢��ַ  

     ptcom->return_start[0]=0;               //�������ݿ�ʼ
     ptcom->return_length[0]=10;             //�������ݳ��ȣ���10���̶���
     ptcom->return_length_available[0]=0;    //������Ч���ݳ���  

     ptcom->send_times=1;                    //���ʹ���
     ptcom->Current_Times=0;                 //��ǰ���ʹ���  
     ptcom->send_staradd[99] = 0;

}



 void Write_Time()                  //дʱ�䵽PLC
{
	Write_Analog();									
}


 void Read_Time()					 //��PLC��ȡʱ��
{
	Read_Analog();
}


 void Write_Recipe()			     //д�䷽��PLC
{
	int staradd;
	int SendTimes;                           //���͵Ĵ���
	int LastTimeWord;						 //���һ�η��ͳ���
	int i,j,t=0;
	int ps;
	int b;
	char a1,a2,a4,a6;
	int aa1;
	U16 aakj;
	int length;
	int plcadd;	
	int k3,k4;
	int nLen=0,ps1=0;
	int datalength=0;
	int TTLen=0,len1=0;
	
	
	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));//���ݳ���
	b=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//���ݿ�ʼ��ַ
	plcadd=*(U8 *)(PE+4);	                          //PLCվ��ַ
	
	if(datalength%24==0)                              
	{
		SendTimes=datalength/24;                      //���͵Ĵ���Ϊdatalength/24              
		LastTimeWord=24;                              //���һ�η��͵ĳ���Ϊ24��D
	}
	if(datalength%24!=0)                              //���ݳ��Ȳ���24D�ı���ʱ
	{
		SendTimes=datalength/24+1;                    //���͵Ĵ���datalength/24+1
		LastTimeWord=datalength%24;                   //���һ�η��͵ĳ���Ϊ��24������
	}	
	
	ps=16;                                     
	

	for (i=0;i<SendTimes;i++)
	{   
		if (i!=(SendTimes-1))                         //�������һ�η���ʱ
		{	
			length=24;                                //��15��D
		}
		else                                          //���һ�η���ʱ
		{
			length=LastTimeWord;                      //��ʣ��ĳ���             
		}
		
		staradd=b +i*24;                              //��ʼ��ַ

        aa1 = Asc_To_Hex6(staradd);                   //��ַת����ʮ����123456ת����0x123456
        
        aa1=(unsigned int)aa1;
        a2 = aa1;                                     //0x56
        a4 = aa1>>8;                                  //0x34
        a6 = aa1>>16;                                 //0x12

        datalength = 7+length*2;                      //����Byte��
		
        *(U8 *)(AD1+ps*i+ps1+0)=0x10;                 //DLE
        *(U8 *)(AD1+ps*i+ps1+1)=0x02;                 //STX
        *(U8 *)(AD1+ps*i+ps1+2)=plcadd;               //վ��
        *(U8 *)(AD1+ps*i+ps1+3)=datalength&0xff;      //����L      
        *(U8 *)(AD1+ps*i+ps1+4)=(datalength>>8)&0xff; //����H
        *(U8 *)(AD1+ps*i+ps1+5)=0x28;                 //������
        *(U8 *)(AD1+ps*i+ps1+6)=0xA0;                 //Ԫ������
        *(U8 *)(AD1+ps*i+ps1+7)=a2;                   //��ʼ��ַL
        *(U8 *)(AD1+ps*i+ps1+8)=a4;                   //��ʼ��ַM
        *(U8 *)(AD1+ps*i+ps1+9)=a6;                   //��ʼ��ַH
        *(U8 *)(AD1+ps*i+ps1+10)=length&0xff;         //Ԫ����ĿL
        *(U8 *)(AD1+ps*i+ps1+11)=(length>>8)&0xff;    //Ԫ����ĿH		
						
		for(j=0;j<length;j++)                         //д����Ԫ��ֵ                 
		{	
           k4=*(U8 *)(PE+9+i*48+j*2);                 //�Ӵ����ݵļĴ�����ʼ��ַPE+9ȡ���ݣ�k3Ϊ��λ��k4Ϊ��λ
           k3=*(U8 *)(PE+9+i*48+j*2+1);
           		
           *(U8 *)(AD1+ps*i+ps1+12+j*2)=k4;
           *(U8 *)(AD1+ps*i+ps1+12+j*2+1)=k3;      
		}

       *(U8 *)(AD1+ps*i+ps1+12+length*2)=0x10;        //DLE
       *(U8 *)(AD1+ps*i+ps1+13+length*2)=0x03;        //ETX

        aakj=CalcHe((U8 *)(AD1+ps*i+ps1+2),10+length*2); //У��ͣ���λ��ǰ����λ�ں�         
        a1=(aakj>>4)&0xf;
        a2=aakj&0xf;
        a1=asicc(a1);
        a2=asicc(a2);
        
        *(U8 *)(AD1+ps*i+ps1+14+length*2)=a1;         //У���H
        *(U8 *)(AD1+ps*i+ps1+15+length*2)=a2;         //У���L
        

      TTLen= 15+length*2;                             //�ܳ���-1
      len1 = 12+length*2;
      nLen=0;
          
      for(t=2;t<len1+nLen;t++)                         //�ҵ�0X10���ֽڣ�������һ��0X10�ֽ�
      {
        if( (*(U8 *)(AD1+ps*i+ps1+t)==0x10))
        {
          nLen+=1;
          for(j=TTLen+nLen;j>t;j--)                    //������
          {
           *(U8 *)(AD1+ps*i+ps1+j)=*(U8 *)(AD1+ps*i+ps1+j-1);
          }
          t++;
          *(U8 *)(AD1+ps*i+ps1+t)=0x10;               //����һ��0X10�ֽڡ�
        }
      }  
      
		ptcom->send_length[i]=16+length*2+nLen;	      //���ͳ���
		ptcom->send_staradd[i]=i*ps+ps1;		      //�������ݴ洢��ַ	
		ptcom->return_length[i]=10;			          //�������ݳ���
		ptcom->return_start[i]=0;			          //����������Ч��ʼ
		ptcom->return_length_available[i]=0;          //������Ч���ݳ���	

		ps1 += nLen;
		ps1 += length*2;
				
	}
	ptcom->send_times=SendTimes;			          //���ʹ���
	ptcom->Current_Times=0;					          //��ǰ���ʹ���
	
}


void compxy(void)				              //����ɱ�׼�洢��ʽ,��������
{
	int i;
	unsigned short a1,a2;
   // U32 lw = *(U32*)(lw_word + 0);    /*������Ϣ*/

    if (ptcom->send_staradd[99] == 7)
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/2;i++)	
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2);
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2+1);
			*(U8 *)(COMad+i*2)=a2;					//���´�,�ӵ�0����ʼ��
			*(U8 *)(COMad+i*2+1)=a1;
		}
		
		ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;	//���ȼ���		
	}
	else
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/2;i++)	
		{
		
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2);
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2+1);
			*(U8 *)(COMad+i*2)=a1;					//���´�,�ӵ�0����ʼ��
			*(U8 *)(COMad+i*2+1)=a2;
			//*(U16*)((U16*)lw + 0) =*(U8 *)(COMad+0);    /*������Ϣ*/
			//*(U16*)((U16*)lw + 1) =*(U8 *)(COMad+1);    /*������Ϣ*/
			
		}
	
		ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;	//���ȼ���		
	}
}


void watchcom(void)		 //�������У��
{
	unsigned int aakj=0;

    aakj=remark();
    if(aakj==1)                  //У������ȷ
    {
       ptcom->IfResultCorrect=1;
       compxy();                //�������ݴ������
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
	unsigned int akj1,akj2;	
	int i,j,retLen=0;
	int k=0;

  
  if( (*(U8 *)(COMad+0)==0x10 ) && (*(U8 *)(COMad+1)==0x06) && (*(U8 *)(COMad+5)==0x00) ) //�ַ�����ʼ��־������ͨ�ű�־Ϊ0X00--��ʾͨ�ųɹ�
  {
      for(i=0;i<200;i++)
      {
        if(*(U8 *)(COMad+i)==0x10&&*(U8 *)(COMad+i+1)==0x03) //��������0X10 0X03 --�ַ���������־
        {
          retLen=i;                                          //���س������ӣ����ڼ��㷵�س��ȡ�
          break;
        }
        else if( (*(U8 *)(COMad+i)==0x10) && (*(U8 *)(COMad+i+1)==0x10) )
        {
         k++;
        }
     }
     
     if(retLen>=8)                                                       //���ݷ��س���������
     {
        for(i=2;i<retLen-k-1;i++)
        {
          if( (*(U8 *)(COMad+i)==0x10) && (*(U8 *)(COMad+i+1)==0x10) )  //�ҵ������ֽڶ���0x10���ֽڣ�ȥ��һ����
          {
             for(j=i; j<retLen+4;j++)
             {
               *(U8 *)(COMad+j) = *(U8 *)(COMad+j+1);                     //�����ֽ���ǰ�ƣ���0X10���ǵ���
             }
          }
        }
        
        if(k!=0)                                                         //���ݰ�������0X10���޳������¼��㳤��
        {
            for(i=0;i<200;i++)
            {
              if(*(U8 *)(COMad+i)==0x10&&*(U8 *)(COMad+i+1)==0x03)       //��������0X10 0X03 --�ַ���������־
              {
                retLen=i;                                                //���س������ӣ����ڼ��㷵�س��ȡ�
                break;
              }
            }
        }
      }

      aakj1=(*(U8 *)(COMad+retLen+3))&0xff;    //У������ֽ�
      aakj2=(*(U8 *)(COMad+retLen+2))&0xff;    //У������ֽ�
      
      akj1=CalcHe((U8 *)(COMad+2),retLen-2);   //���յ������ݽ���У��
      
      akj2=(akj1>>4)&0xf;                      //У��͵ĵ��ֽ�     
      akj1=akj1&0xf;                           //У��͵ĸ��ֽ�
      
      akj1=asicc(akj1);                        //ת����ASCII��
      akj2=asicc(akj2);                        //ת����ASCII��

      if((akj1==aakj1)&&(akj2==aakj2)||(akj1==aakj1)&&(akj2==aakj2+1))       //�Ƚ�У���룬�����ϣ�����ȼ���ȡ���ݽ���У��
      {
         return 1;
      }
      else
      {
        return 0;	
      }

   }
   else                                //������0X 01  0X06 ��ͷ���ҵ�����ֽڲ���0X00�����ش���
   {
      return 0;
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

 unsigned short CalcHe(unsigned char *chData,unsigned short uNo)		//�����У��
{
	int i;
	int ab=0;
	for(i=1;i<uNo;i++)
	{
		ab=ab+chData[i];
	}
	return (ab);
}


  int Asc_To_Hex6( int addr)   //��ʮ����123456ת����0x123456
{
   int a1,a2,a3,a4,a5,a6;
   int retAdd=0;

   a1= addr%10;           //��λ
   a2=(addr/10)%10;       //ʮλ
   a3=(addr/100)%10;      //��λ
   a4=(addr/1000)%10;     //ǧλ
   a5=(addr/10000)%10;    //��λ
   a6=(addr/100000)%10;   //ʮ��
       
   a2=(a2<<4)&0xf0;       //ת��hex����λ��ǰ����λ�ں�
   a2=a2|a1;
   
   a4=(a4<<4)&0xf0;       //ת��hex����λ��ǰ����λ�ں�
   a4=a4|a3;
   
   a6=(a6<<4)&0xf0;       //ת��hex����λ��ǰ����λ�ں�
   a6=a6|a5; 
   
   retAdd =a2;
   retAdd |=(a4<<8)&0xff00 ; 
   retAdd |=(a6<<16)&0xff0000 ;
   
   return retAdd;
  
  
}



