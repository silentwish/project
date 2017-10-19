 

#include "stdio.h"
#include "def.h"
#include "smm.h"


	
struct Com_struct_D *ptcom;



void Enter_PlcDriver(void)
{

	ptcom=(struct Com_struct_D *)adcom;
		    
	switch (ptcom->R_W_Flag)
	{
	case PLC_READ_DATA:				        //进入驱动是读数据
	case PLC_READ_DIGITALALARM:			        //进入驱动是读数据,报警	
	case PLC_READ_TREND:			        //进入驱动是读数据,趋势图
	case PLC_READ_ANALOGALARM:			        //进入驱动是读数据,类比报警	
	case PLC_READ_CONTROL:			        //进入驱动是读PLC控制的数据	
		switch(ptcom->registerr)
		{
          case 'X':             //X--输入继电器
          case 'Y':             //Y--输出继电器
          case 'M':             //M--辅助继电器
          case 'H':             //S
          case 'K':             //SM--步进继电器
          case 'T':             //T  --计时接点
          case 'C':		        //C  --计数接点
          case 'J':             //T*--计时线圈
          case 'L':             //C*--计数线圈
                Read_Bool();     //进入驱动是读位数据       
                break;
          case 'D':             //D
          case 'R':             //R
          case 'N':             //SD
          case 't':             //TV
          case 'c':             //CV
          case 'v':             //CV32
          case 'd':             //D_Bit
          case 'r':             //R_Bit
               Read_Analog();    //进入驱动是读模拟数据 
               break;			
		}
		break;
	case PLC_WRITE_DATA:				
		switch(ptcom->registerr)
		{
          case 'Y':             //Y--输出继电器
          case 'M':             //M--辅助继电器
          case 'H':             //SM
          case 'K':             //S--步进继电器
          case 'T':             //T  --计时接点
          case 'C':             //C  --计数接点
          case 'J':             //T*--计时线圈
          case 'L':             //C*--计数线圈
          case 'd':             //D_Bit
          case 'r':             //R_Bit
                Set_Reset();    //进入驱动是强置置位和复位
                break;
          case 'D':             //D
          case 'R':             //R
          case 'N':             //SD
          case 't':             //TV
          case 'c':             //CV
          case 'v':             //CV32
               Write_Analog();	//进入驱动是写模拟数据	
                break;			
		}
		break;	
	case PLC_WRITE_TIME:				        //进入驱动是写时间到PLC
		switch(ptcom->registerr)
		{
          case 'D':
                Write_Time();		
                break;			
		}
		break;	
	case PLC_READ_TIME:				         //进入驱动是读取时间到PLC
		switch(ptcom->registerr)
		{
          case 'D':		
                Read_Time();		
                break;			
		}
		break;
	case PLC_WRITE_RECIPE:				         //进入驱动是写配方到PLC
		switch(*(U8 *)(PE+3))   //配方寄存器名称
		{
           case 'D':		
                Write_Recipe();		
                break;			
		}
		break;
	case PLC_READ_RECIPE:				         //进入驱动是从PLC读取配方
		switch(*(U8 *)(PE+3))   //配方寄存器名称
		{
          case 'D':		
               Read_Recipe();		
               break;			
		}
		break;							
	case PLC_CHECK_DATA:				         //进入驱动是数据处理
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
    
	b=ptcom->address;			 // 开始置位地址
	plcadd=ptcom->plc_address;	 //PLC站地址

	switch (ptcom->registerr)	 //根据寄存器类型获得偏移地址
	{
     case 'Y':             //Y--输出继电器
          orderNum=0x91;
          break;
     case 'M':             //M--辅助继电器
          orderNum=0x92;
          break;
     case 'H':             //S
         orderNum=0x93;
         break;
     case 'K':             //SM--步进继电器
         orderNum=0x94;
         break; 
     case 'T':                          //T :计时接点
         orderNum=0x99; 
         break;
     case 'C':                          //C:计数接点
         orderNum=0x9d; 
         break; 
     case 'J':                          //T* :计时线圈
         orderNum=0x98; 
         break; 
     case 'L':                          //C* :计数线圈
         orderNum=0x9c; 
         break;          
     case 'd':                          //D_Bit
         orderNum=0x95; 
         break; 
     case 'r':                          //R_Bit
         orderNum=0x97; 
         break; 
	}
	
     if(ptcom->registerr=='Y') //转换成8进制
     {
         a1=b&0x7;         //个位
         a2=(b>>3)&0x7;    //十位
         a3=(b>>6)&0x7;    //百位
         a4= 0;            //千位
         a5= 0;            //万位
         a6= 0;            //十万
         
         a2=(a2<<4)&0xf0;  //转成hex，高位在前，低位在后
         a2=a2|a1;
         
         a4=a3;
         a6=0;
     }
     else if(ptcom->registerr=='d' || ptcom->registerr=='r')
     {
       b1=b%16;
       b=b/16;
       aa1 = Asc_To_Hex6(b);               //地址转换，十进制12345.12转换成0x12345C
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
         if(b>=9000)     //上位从9000开始，实际发送的地址从0开始。
         {
           b=b-9000;
         }
         else
         {
           b=0;
         }
       }
       
       aa1 = Asc_To_Hex6(b);           //地址转换，十进制123456转换成0x123456
       
       aa1=(unsigned int)aa1;
       a2 = aa1;                       //0x56
       a4 = aa1>>8;                    //0x34
       a6 = aa1>>16;                   //0x12
     }

     *(U8 *)(AD1+0)=0x10;               //DLE
     *(U8 *)(AD1+1)=0x02;               //STX
     *(U8 *)(AD1+2)=plcadd;             //站号
     *(U8 *)(AD1+3)=0x09;               //长度L      
     *(U8 *)(AD1+4)=0x00;               //长度H
     *(U8 *)(AD1+5)=0x29;               //命令码，位元写入
     *(U8 *)(AD1+6)=orderNum;           //元件代码
     *(U8 *)(AD1+7)=a2;                 //起始地址L
     *(U8 *)(AD1+8)=a4;                 //起始地址M
     *(U8 *)(AD1+9)=a6;                 //起始地址H
     *(U8 *)(AD1+10)=0x01;              //寄存器长度L
     *(U8 *)(AD1+11)=0x00;              //寄存器长度H
     if (ptcom->writeValue==1)          //置位
     {
        *(U8 *)(AD1+12)=0x01;          //数据L
        *(U8 *)(AD1+13)=0x00;          //数据H       
     }
     if (ptcom->writeValue==0)         //复位
     {
         *(U8 *)(AD1+12)=0x00;         //数据L
         *(U8 *)(AD1+13)=0x00;         //数据H         
     } 

     *(U8 *)(AD1+14)=0x10;              //DLE
     *(U8 *)(AD1+15)=0x03;              //ETX
     
      aakj=CalcHe((U8 *)(AD1+2),12);    //校验和，高位在前，低位在后         
      a1=(aakj>>4)&0xf;
      a2=aakj&0xf;
      a1=asicc(a1);
      a2=asicc(a2);
      
      *(U8 *)(AD1+16)=a1;                //校验和H
      *(U8 *)(AD1+17)=a2;                //校验和L

     for(i=2;i<14;i++)                   //除去起始码结束码，及校验码外，为0X10的字节数目。
     {
       if(*(U8 *)(AD1+i)==0x10)
       {
         nLen++;
       }
     }
     for(i=2;i<14+nLen;i++)
     {
       if((*(U8 *)(AD1+i)==0x10)&&(*(U8 *)(AD1+i+1)!=0x03))          //找到为0X10的数据。
       {
         k++;
         for(j=17+k;j>i;j--)               //往后移
         {
          *(U8 *)(AD1+j)=*(U8 *)(AD1+j-1);
         }
         
         *(U8 *)(AD1+i+1)=0x10;            //增加一个0X10字节。
         i++;
       }
     }
      
      ptcom->send_length[0]=18+nLen;       //发送长度
      ptcom->send_staradd[0]=0;            //发送数据存储地址  
          
      ptcom->return_start[0]=0;            //返回数据有效开始
      ptcom->return_length[0]=10;          //返回数据长度，有10个固定，
      ptcom->return_length_available[0]=0; //返回有效数据长度    
      
      ptcom->send_add[0]=ptcom->address;                      //读的是这个地址的数据
      ptcom->send_times=1;                 //发送次数
      ptcom->Current_Times=0;              //当前发送次数  

     ptcom->send_staradd[99]=0;      
          
}



 void Read_Bool()				    //读取数字量的信息
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
	//U32 lw = *(U32*)(lw_word + 0);    /*调试信息*/

	
	plcadd=ptcom->plc_address;	        //PLC站地址	
	b=ptcom->address;					// 开始读取地址
	len=ptcom->register_length;		    // 开始读取长度
		
	switch (ptcom->registerr)	        //根据寄存器类型获得偏移地址
	{
     case 'X':                          //X--输入继电器
          orderNum=0x90;
          break;	
     case 'Y':                          //Y--输出继电器
          orderNum=0x91;
          break;
     case 'M':                          //M--辅助继电器
          orderNum=0x92;
          break;
     case 'H':                          //S
         orderNum=0x93;
         break;	
     case 'K':                          //SM--步进继电器
         orderNum=0x94;
         break; 
     case 'T':                          //T :计时接点
          orderNum=0x99; 
          break;
     case 'C':                          //C:计数接点
         orderNum=0x9d; 
         break;	
     case 'J':                          //T* :计时线圈
         orderNum=0x98; 
         break; 
     case 'L':                          //C* :计数线圈
         orderNum=0x9c; 
         break;          
	}
	
		
    if(ptcom->registerr=='X' || ptcom->registerr=='Y' ) //转换成8进制
    {
      a1=b&0x7;                          //个位
      a2=(b>>3)&0x7;                     //十位
      a3=(b>>6)&0x7;                     //百位
      a4= 0;                             //千位
      a5= 0;                             //万位
      a6= 0;                             //十万
      
      a2=(a2<<4)&0xf0;                   //转成hex，高位在前，低位在后
      a2=a2|a1;
      a4=a3;
      a6=0;
      
      sendNum=0x21;
    }
    else
    {    
       if(ptcom->registerr =='K')
       {
         if(b>=9000)     //上位从9000开始，实际发送的地址从0开始。
         {
           b=b-9000;
         }
         else
         {
           b=0;
         }
       }
       
      aa1 = Asc_To_Hex6(b);               //地址转换，十进制123456转换成0x123456

      aa1=(unsigned int)aa1;
      a2 = aa1;                           //0x56
      a4 = aa1>>8;                        //0x34
      a6 = aa1>>16;                       //0x12
      
      sendNum=0x21;
    }


    if(ptcom->registerr=='T' || ptcom->registerr=='J' ||ptcom->registerr=='K') 
    {
       if(b>=504)  //最长可以读取512，再长越界报错。
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
        
        if(b>=8184)  //最长可以读取8191，再长越界报错。
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
        
        if(b>=4088)  //最长可以读取4088，再长越界报错。
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
	*(U8 *)(AD1+2)=plcadd;                 //站号
	*(U8 *)(AD1+3)=0x07;   			       //长度L    	
	*(U8 *)(AD1+4)=0x00;			       //长度H
	*(U8 *)(AD1+5)=sendNum;   			       //命令码
	*(U8 *)(AD1+6)=orderNum;		       //元件代码
	*(U8 *)(AD1+7)=a2;		               //起始地址L
	*(U8 *)(AD1+8)=a4;		               //起始地址M
	*(U8 *)(AD1+9)=a6;	                   //起始地址H
	*(U8 *)(AD1+10)=regLen&0xff;           //寄存器长度L
	*(U8 *)(AD1+11)=(regLen>>8)&0xff;      //寄存器长度H
	*(U8 *)(AD1+12)=0x10;		           //DLE
	*(U8 *)(AD1+13)=0x03;	                //ETX
	
	aakj=CalcHe((U8 *)(AD1+2),10);         //校验和，高位在前，低位在后         
	a1=(aakj>>4)&0xf;
	a2=aakj&0xf;
    a1=asicc(a1);
    a2=asicc(a2);
    
	*(U8 *)(AD1+14)=a1;                     //校验和H
	*(U8 *)(AD1+15)=a2;                     //校验和L

    nLen = 0;                               //数据中0X10的个数
    for(i=2;i<12+nLen;i++)
    {
      if((*(U8 *)(AD1+i)==0x10)&&(*(U8 *)(AD1+i+1)!=0x03))          //找到为0X10的数据。
      {
        nLen++;
        for(j=15+nLen;j>i;j--)                //往后移
        {
         *(U8 *)(AD1+j)=*(U8 *)(AD1+j-1);
        }
        i++;
        *(U8 *)(AD1+i)=0x10;                 //增加一个0X10字节。
      }
    }

	
	ptcom->send_length[0]=16+nLen;		      //发送长度
	ptcom->send_staradd[0]=0;				  //发送数据存储地址	

	ptcom->return_start[0]=6;				  //返回数据开始		
	ptcom->return_length[0]=10+2*len;         //返回数据长度，有10个固定，
	ptcom->return_length_available[0]=len+1;  //返回有效数据长度	

	ptcom->send_times=1;					  //发送次数
	ptcom->Current_Times=0;					  //当前发送次数
	
    if(ptcom->registerr=='d' || ptcom->registerr=='r')
    {
        ptcom->send_staradd[99] = 7;  
    }
    else
    {
        ptcom->send_staradd[99] = 0;  
    }
}


 void Read_Analog()				          //读模拟量
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
   // U32 lw = *(U32*)(lw_word + 0);    /*调试信息*/
    
	plcadd=ptcom->plc_address;	  //PLC站地址	
	b=ptcom->address;			  //开始读取地址
	len=ptcom->register_length;	      // 开始读取长度

	switch (ptcom->registerr)	//根据寄存器类型获得偏移地址
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
       if(b>=9000)     //上位从9000开始，实际发送的地址从0开始。
       {
         b=b-9000;
       }
       else
       {
         b=0;
       }
	}

	
    aa1 = Asc_To_Hex6(b);             //地址转换，十进制123456转换成0x123456
    
    aa1=(unsigned int)aa1;
    a2 = aa1;                         //0x56
    a4 = aa1>>8;                      //0x34
    a6 = aa1>>16;                     //0x12
    
    regLen=len;
    
    *(U8 *)(AD1+0)=0x10;                      //DLE
    *(U8 *)(AD1+1)=0x02;                      //STX
    *(U8 *)(AD1+2)=plcadd;                    //站号
    *(U8 *)(AD1+3)=0x07;                      //长度L      
    *(U8 *)(AD1+4)=0x00;                      //长度H
    *(U8 *)(AD1+5)=0x20;                      //命令码
    *(U8 *)(AD1+6)=orderNum;                  //元件代码
    *(U8 *)(AD1+7)=a2;                        //起始地址L
    *(U8 *)(AD1+8)=a4;                        //起始地址M
    *(U8 *)(AD1+9)=a6;                        //起始地址H
    *(U8 *)(AD1+10)=regLen&0xff;              //元件数目L
    *(U8 *)(AD1+11)=(regLen>>8)&0xff;         //元件数目H
    *(U8 *)(AD1+12)=0x10;                     //DLE
    *(U8 *)(AD1+13)=0x03;                     //ETX
    
     aakj=CalcHe((U8 *)(AD1+2),10);           //校验和，高位在前，低位在后         
     a1=(aakj>>4)&0xf;
     a2=aakj&0xf;
     a1=asicc(a1);
     a2=asicc(a2);
     
     *(U8 *)(AD1+14)=a1;                        //校验和H
     *(U8 *)(AD1+15)=a2;                        //校验和L

     for(i=2;i<12+nLen;i++)
     {
       if((*(U8 *)(AD1+i)==0x10)&&(*(U8 *)(AD1+i+1)!=0x03))    //找到为0X10的数据。
       {
         nLen++;
         for(j=15+nLen;j>i;j--)                 //往后移
         {
          *(U8 *)(AD1+j)=*(U8 *)(AD1+j-1);
         }
         
         *(U8 *)(AD1+i+1)=0x10;                 //增加一个0X10字节。
         i++;
       }
     }

     if(ptcom->registerr=='v')
     {
         ptcom->return_start[0]=6;                   //返回数据有效开始
         ptcom->return_length[0]=-1;                 //返回数据长度，有10个固定，
         ptcom->return_length_available[0]=regLen*4; //返回有效数据长度  
         ptcom->send_staradd[99] = 7;
     }
     else if( (ptcom->registerr =='d') || (ptcom->registerr =='r') )
     {
         ptcom->return_start[0]=6;                   //返回数据有效开始
         ptcom->return_length[0]=-1;                 //返回数据长度，有10个固定，
         ptcom->return_length_available[0]=regLen*2; //返回有效数据长度  
         ptcom->send_staradd[99] = 0;

         ptcom->address=b*16;                        //上位传过来的是addr*16+bit。
         ptcom->register_length=len*2;               //两个字节。
     }
     else
     {
         ptcom->return_start[0]=6;                   //返回数据有效开始
         ptcom->return_length[0]=-1;                 //返回数据长度，有10个固定，
         ptcom->return_length_available[0]=regLen*2; //返回有效数据长度  
         ptcom->send_staradd[99] = 7;
     }

     ptcom->send_length[0]=16+nLen;                 //发送长度
     ptcom->send_staradd[0]=0;                      //发送数据存储地址  

     ptcom->send_times=1;                           //发送次数
     ptcom->Current_Times=0;                        //当前发送次数
}


 void Read_Recipe()						      //读取配方
{
	U16 aakj;
	int b,i;
	char a1,a2,a4,a6;
	int aa1;
	int plcadd,ps,ps1=0,staradd,SendTimes,LastTimeWord;
	int sendlength,datalength;
	int j=0,k=0;
	int nLen=0;
	
	plcadd=ptcom->plc_address;	              //PLC站地址	
	b=ptcom->address;						  // 开始读取地址
	datalength=ptcom->register_length;		  // 开始读取长度
	
	if(datalength%24==0)                       
	{
		SendTimes=datalength/24;               //发送的次数为datalength/24              
		LastTimeWord=24;                       //最后一次发送的长度为24个D
	}
	if(datalength%24!=0)                       //数据长度不是24的倍数时
	{
		SendTimes=datalength/24+1;             //发送的次数datalength/24+1
		LastTimeWord=datalength%24;            //最后一次发送的长度为除24的余数
	}

    
	ps=20;
	for (i=0;i<SendTimes;i++)
	{		
		staradd=b+i*24;                        //起始地址
		
		if (i!=(SendTimes-1))			       //不是最后一次
		{
			sendlength=24;
		}
		else
		{
			sendlength=LastTimeWord;
		}
		
        aa1 = Asc_To_Hex6(staradd);           //地址转换，十进制123456转换成0x123456
        
        aa1=(unsigned int)aa1;
        a2 = aa1;                             //0x56
        a4 = aa1>>8;                          //0x34
        a6 = aa1>>16;                         //0x12
				
        *(U8 *)(AD1+ps*i+ps1+0)=0x10;                   //DLE
        *(U8 *)(AD1+ps*i+ps1+1)=0x02;                   //STX
        *(U8 *)(AD1+ps*i+ps1+2)=plcadd;                 //站号
        *(U8 *)(AD1+ps*i+ps1+3)=0x07;                   //长度L      
        *(U8 *)(AD1+ps*i+ps1+4)=0x00;                   //长度H
        *(U8 *)(AD1+ps*i+ps1+5)=0x20;                   //命令码
        *(U8 *)(AD1+ps*i+ps1+6)=0xA0;                   //元件代码
        *(U8 *)(AD1+ps*i+ps1+7)=a2;                     //起始地址L
        *(U8 *)(AD1+ps*i+ps1+8)=a4;                     //起始地址M
        *(U8 *)(AD1+ps*i+ps1+9)=a6;                     //起始地址H
        *(U8 *)(AD1+ps*i+ps1+10)=sendlength&0xff;       //元件数目L
        *(U8 *)(AD1+ps*i+ps1+11)=(sendlength>>8)&0xff;  //元件数目H
        *(U8 *)(AD1+ps*i+ps1+12)=0x10;                  //DLE
        *(U8 *)(AD1+ps*i+ps1+13)=0x03;                  //ETX
						
         aakj=CalcHe((U8 *)(AD1+ps*i+ps1+2),10);        //校验和，高位在前，低位在后         
         a1=(aakj>>4)&0xf;
         a2=aakj&0xf;
         a1=asicc(a1);
         a2=asicc(a2);
         
         *(U8 *)(AD1+ps*i+ps1+14)=a1;                   //校验和H
         *(U8 *)(AD1+ps*i+ps1+15)=a2;                   //校验和L
         nLen=0;                                        //数据中0X10的个数

        for(k=2;k<12+nLen;k++)                          //找到0X10的数据，然后再后面增加一个0X10
        {
          if((*(U8 *)(AD1+ps*i+ps1+k)==0x10)&&(*(U8 *)(AD1+ps*i+ps1+k+1)!=0x03))            //找到为0X10的数据。
          {
            nLen++;
            for(j=15+nLen;j>k;j--)                        //往后移
            {
             *(U8 *)(AD1+ps*i+ps1+j)=*(U8 *)(AD1+ps*i+ps1+j-1);
            }
            k++;
            *(U8 *)(AD1+ps*i+ps1+k)=0x10;              //增加一个0X10字节。
          }
        }  
       

		ptcom->send_length[i]=16+nLen;				    //发送长度=原始长度+增加的0X10
		ptcom->send_staradd[i]=ps*i+ps1;			    //发送数据存储地址	
		ptcom->send_add[i]=b+i*24;		                //读的是这个地址的数据
		ptcom->send_data_length[i]=sendlength;	        //不是最后一次都是32个D

		ptcom->return_start[i]=6;				        //返回数据有效开始			
		ptcom->return_length[i]=-1;                     //返回数据长度，有10个固定，
		ptcom->return_length_available[i]=sendlength*2;	//返回有效数据长度	
		
        ps1+=nLen;
	}
	
	ptcom->send_times=SendTimes;					    //发送次数
	ptcom->Current_Times=0;					            //当前发送次数
	ptcom->send_staradd[99] = 7; 
}



 void Write_Analog()					      //写模拟量
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
    
	plcadd=ptcom->plc_address;	       //PLC站地址	
	b=ptcom->address;				   // 开始读取地址
	len=ptcom->register_length;		   // 开始读取长度
		
    switch (ptcom->registerr)          //根据寄存器类型获得偏移地址
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
       if(b>=9000)     //上位从9000开始，实际发送的地址从0开始。
       {
         b=b-9000;
       }
       else
       {
         b=0;
       }
	}
    
    aa1 = Asc_To_Hex6(b);             //地址转换，十进制123456转换成0x123456
    
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
    *(U8 *)(AD1+2)=plcadd;                //站号
    *(U8 *)(AD1+3)=dataLen&0xff;          //长度L      
    *(U8 *)(AD1+4)=(dataLen>>8)&0xff;     //长度H
    *(U8 *)(AD1+5)=0x28;                  //命令码
    *(U8 *)(AD1+6)=orderNum;              //元件代码
    *(U8 *)(AD1+7)=a2;                    //起始地址L
    *(U8 *)(AD1+8)=a4;                    //起始地址M
    *(U8 *)(AD1+9)=a6;                    //起始地址H
    *(U8 *)(AD1+10)=len&0xff;             //元件数目L
    *(U8 *)(AD1+11)=(len>>8)&0xff;        //元件数目H

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
        
        aakj=CalcHe((U8 *)(AD1+2),10+len*4);   //校验和，高位在前，低位在后         
        b1=(aakj>>4)&0xf;
        b2=aakj&0xf;
        b1=asicc(b1);
        b2=asicc(b2);
        
        *(U8 *)(AD1+18+(len-1)*4)=b1;          //校验和H
        *(U8 *)(AD1+19+(len-1)*4)=b2;          //校验和L
        
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
        
        aakj=CalcHe((U8 *)(AD1+2),10+len*2);   //校验和，高位在前，低位在后         
        b1=(aakj>>4)&0xf;
        b2=aakj&0xf;
        b1=asicc(b1);
        b2=asicc(b2);
        
        *(U8 *)(AD1+16+(len-1)*2)=b1;          //校验和H
        *(U8 *)(AD1+17+(len-1)*2)=b2;          //校验和L
        
        len2 = 15+len*2;
        len1 = 12+len*2;  

    }

    

    for(i=2;i<len1;i++)                     //除去起始码结束码，及校验码外，为0X10的字节数目。
    {  
      if(*(U8 *)(AD1+i)==0x10)
      {
        nLen++;                             //数据中为0X10的个数。
      }
    }

    for(i=2;i<len1;i++)                     //找到0X10的数据，然后再后面增加一个0X10
    {
        if((*(U8 *)(AD1+i)==0x10)&&(*(U8 *)(AD1+i+1)!=0x03))          //找到为0X10的数据。
      {
        k++;
        for(j=len2+k;j>i;j--)               //往后移
        {
         *(U8 *)(AD1+j)=*(U8 *)(AD1+j-1);
        }
        
        *(U8 *)(AD1+i+1)=0x10;              //增加一个0X10字节。
        i++;
      }
    }

     if(ptcom->registerr=='v')
     {
        ptcom->send_length[0]=16+len*4+nLen;    //发送长度
     }
     else
     {
        ptcom->send_length[0]=16+len*2+nLen;    //发送长度
     }
     
     ptcom->send_staradd[0]=0;               //发送数据存储地址  

     ptcom->return_start[0]=0;               //返回数据开始
     ptcom->return_length[0]=10;             //返回数据长度，有10个固定，
     ptcom->return_length_available[0]=0;    //返回有效数据长度  

     ptcom->send_times=1;                    //发送次数
     ptcom->Current_Times=0;                 //当前发送次数  
     ptcom->send_staradd[99] = 0;

}



 void Write_Time()                  //写时间到PLC
{
	Write_Analog();									
}


 void Read_Time()					 //从PLC读取时间
{
	Read_Analog();
}


 void Write_Recipe()			     //写配方到PLC
{
	int staradd;
	int SendTimes;                           //发送的次数
	int LastTimeWord;						 //最后一次发送长度
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
	
	
	datalength=((*(U8 *)(PE+0))<<8)+(*(U8 *)(PE+1));//数据长度
	b=((*(U8 *)(PE+5))<<24)+((*(U8 *)(PE+6))<<16)+((*(U8 *)(PE+7))<<8)+(*(U8 *)(PE+8));//数据开始地址
	plcadd=*(U8 *)(PE+4);	                          //PLC站地址
	
	if(datalength%24==0)                              
	{
		SendTimes=datalength/24;                      //发送的次数为datalength/24              
		LastTimeWord=24;                              //最后一次发送的长度为24个D
	}
	if(datalength%24!=0)                              //数据长度不是24D的倍数时
	{
		SendTimes=datalength/24+1;                    //发送的次数datalength/24+1
		LastTimeWord=datalength%24;                   //最后一次发送的长度为除24的余数
	}	
	
	ps=16;                                     
	

	for (i=0;i<SendTimes;i++)
	{   
		if (i!=(SendTimes-1))                         //不是最后一次发送时
		{	
			length=24;                                //发15个D
		}
		else                                          //最后一次发送时
		{
			length=LastTimeWord;                      //发剩余的长度             
		}
		
		staradd=b +i*24;                              //起始地址

        aa1 = Asc_To_Hex6(staradd);                   //地址转换，十进制123456转换成0x123456
        
        aa1=(unsigned int)aa1;
        a2 = aa1;                                     //0x56
        a4 = aa1>>8;                                  //0x34
        a6 = aa1>>16;                                 //0x12

        datalength = 7+length*2;                      //资料Byte数
		
        *(U8 *)(AD1+ps*i+ps1+0)=0x10;                 //DLE
        *(U8 *)(AD1+ps*i+ps1+1)=0x02;                 //STX
        *(U8 *)(AD1+ps*i+ps1+2)=plcadd;               //站号
        *(U8 *)(AD1+ps*i+ps1+3)=datalength&0xff;      //长度L      
        *(U8 *)(AD1+ps*i+ps1+4)=(datalength>>8)&0xff; //长度H
        *(U8 *)(AD1+ps*i+ps1+5)=0x28;                 //命令码
        *(U8 *)(AD1+ps*i+ps1+6)=0xA0;                 //元件代码
        *(U8 *)(AD1+ps*i+ps1+7)=a2;                   //起始地址L
        *(U8 *)(AD1+ps*i+ps1+8)=a4;                   //起始地址M
        *(U8 *)(AD1+ps*i+ps1+9)=a6;                   //起始地址H
        *(U8 *)(AD1+ps*i+ps1+10)=length&0xff;         //元件数目L
        *(U8 *)(AD1+ps*i+ps1+11)=(length>>8)&0xff;    //元件数目H		
						
		for(j=0;j<length;j++)                         //写入多个元件值                 
		{	
           k4=*(U8 *)(PE+9+i*48+j*2);                 //从存数据的寄存器开始地址PE+9取数据，k3为高位，k4为低位
           k3=*(U8 *)(PE+9+i*48+j*2+1);
           		
           *(U8 *)(AD1+ps*i+ps1+12+j*2)=k4;
           *(U8 *)(AD1+ps*i+ps1+12+j*2+1)=k3;      
		}

       *(U8 *)(AD1+ps*i+ps1+12+length*2)=0x10;        //DLE
       *(U8 *)(AD1+ps*i+ps1+13+length*2)=0x03;        //ETX

        aakj=CalcHe((U8 *)(AD1+ps*i+ps1+2),10+length*2); //校验和，高位在前，低位在后         
        a1=(aakj>>4)&0xf;
        a2=aakj&0xf;
        a1=asicc(a1);
        a2=asicc(a2);
        
        *(U8 *)(AD1+ps*i+ps1+14+length*2)=a1;         //校验和H
        *(U8 *)(AD1+ps*i+ps1+15+length*2)=a2;         //校验和L
        

      TTLen= 15+length*2;                             //总长度-1
      len1 = 12+length*2;
      nLen=0;
          
      for(t=2;t<len1+nLen;t++)                         //找到0X10的字节，再增加一个0X10字节
      {
        if( (*(U8 *)(AD1+ps*i+ps1+t)==0x10))
        {
          nLen+=1;
          for(j=TTLen+nLen;j>t;j--)                    //往后移
          {
           *(U8 *)(AD1+ps*i+ps1+j)=*(U8 *)(AD1+ps*i+ps1+j-1);
          }
          t++;
          *(U8 *)(AD1+ps*i+ps1+t)=0x10;               //增加一个0X10字节。
        }
      }  
      
		ptcom->send_length[i]=16+length*2+nLen;	      //发送长度
		ptcom->send_staradd[i]=i*ps+ps1;		      //发送数据存储地址	
		ptcom->return_length[i]=10;			          //返回数据长度
		ptcom->return_start[i]=0;			          //返回数据有效开始
		ptcom->return_length_available[i]=0;          //返回有效数据长度	

		ps1 += nLen;
		ps1 += length*2;
				
	}
	ptcom->send_times=SendTimes;			          //发送次数
	ptcom->Current_Times=0;					          //当前发送次数
	
}


void compxy(void)				              //处理成标准存储格式,重新排列
{
	int i;
	unsigned short a1,a2;
   // U32 lw = *(U32*)(lw_word + 0);    /*调试信息*/

    if (ptcom->send_staradd[99] == 7)
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/2;i++)	
		{
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2);
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2+1);
			*(U8 *)(COMad+i*2)=a2;					//重新存,从第0个开始存
			*(U8 *)(COMad+i*2+1)=a1;
		}
		
		ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;	//长度减半		
	}
	else
	{
		for(i=0;i<ptcom->return_length_available[ptcom->Current_Times-1]/2;i++)	
		{
		
			a1=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2);
			a2=*(U8 *)(COMad+ptcom->return_start[ptcom->Current_Times-1]+i*2+1);
			*(U8 *)(COMad+i*2)=a1;					//重新存,从第0个开始存
			*(U8 *)(COMad+i*2+1)=a2;
			//*(U16*)((U16*)lw + 0) =*(U8 *)(COMad+0);    /*调试信息*/
			//*(U16*)((U16*)lw + 1) =*(U8 *)(COMad+1);    /*调试信息*/
			
		}
	
		ptcom->return_length_available[ptcom->Current_Times-1]=ptcom->return_length_available[ptcom->Current_Times-1]/2;	//长度减半		
	}
}


void watchcom(void)		 //检查数据校检
{
	unsigned int aakj=0;

    aakj=remark();
    if(aakj==1)                  //校检玛正确
    {
       ptcom->IfResultCorrect=1;
       compxy();                //进入数据处理程序
    }
    else
    {
       ptcom->IfResultCorrect=0;
    }
}



 int remark()				//返回来的数据计算校检码是否正确
{
	unsigned int aakj1;
	unsigned int aakj2;
	unsigned int akj1,akj2;	
	int i,j,retLen=0;
	int k=0;

  
  if( (*(U8 *)(COMad+0)==0x10 ) && (*(U8 *)(COMad+1)==0x06) && (*(U8 *)(COMad+5)==0x00) ) //字符串开始标志，并且通信标志为0X00--表示通信成功
  {
      for(i=0;i<200;i++)
      {
        if(*(U8 *)(COMad+i)==0x10&&*(U8 *)(COMad+i+1)==0x03) //假如遇到0X10 0X03 --字符串结束标志
        {
          retLen=i;                                          //返回长度增加，用于计算返回长度。
          break;
        }
        else if( (*(U8 *)(COMad+i)==0x10) && (*(U8 *)(COMad+i+1)==0x10) )
        {
         k++;
        }
     }
     
     if(retLen>=8)                                                       //根据返回长度做处理
     {
        for(i=2;i<retLen-k-1;i++)
        {
          if( (*(U8 *)(COMad+i)==0x10) && (*(U8 *)(COMad+i+1)==0x10) )  //找到相邻字节都是0x10的字节，去掉一个。
          {
             for(j=i; j<retLen+4;j++)
             {
               *(U8 *)(COMad+j) = *(U8 *)(COMad+j+1);                     //后续字节往前移，把0X10覆盖掉。
             }
          }
        }
        
        if(k!=0)                                                         //数据包里面有0X10，剔除后重新计算长度
        {
            for(i=0;i<200;i++)
            {
              if(*(U8 *)(COMad+i)==0x10&&*(U8 *)(COMad+i+1)==0x03)       //假如遇到0X10 0X03 --字符串结束标志
              {
                retLen=i;                                                //返回长度增加，用于计算返回长度。
                break;
              }
            }
        }
      }

      aakj1=(*(U8 *)(COMad+retLen+3))&0xff;    //校验码低字节
      aakj2=(*(U8 *)(COMad+retLen+2))&0xff;    //校验码高字节
      
      akj1=CalcHe((U8 *)(COMad+2),retLen-2);   //对收到的数据进行校验
      
      akj2=(akj1>>4)&0xf;                      //校验和的低字节     
      akj1=akj1&0xf;                           //校验和的高字节
      
      akj1=asicc(akj1);                        //转换成ASCII码
      akj2=asicc(akj2);                        //转换成ASCII码

      if((akj1==aakj1)&&(akj2==aakj2)||(akj1==aakj1)&&(akj2==aakj2+1))       //比较校验码，相等完毕，不相等继续取数据进行校验
      {
         return 1;
      }
      else
      {
        return 0;	
      }

   }
   else                                //不是以0X 01  0X06 开头并且第五个字节不是0X00，返回错误
   {
      return 0;
   }  
}

 int asicc(int a)			//转为Asc码
{
	int bl;
	if(a<10)
		bl=a+0x30;
	if(a>9)
		bl=a-10+0x41;
	return bl;	
}

 unsigned short CalcHe(unsigned char *chData,unsigned short uNo)		//计算和校检
{
	int i;
	int ab=0;
	for(i=1;i<uNo;i++)
	{
		ab=ab+chData[i];
	}
	return (ab);
}


  int Asc_To_Hex6( int addr)   //将十进制123456转换成0x123456
{
   int a1,a2,a3,a4,a5,a6;
   int retAdd=0;

   a1= addr%10;           //个位
   a2=(addr/10)%10;       //十位
   a3=(addr/100)%10;      //百位
   a4=(addr/1000)%10;     //千位
   a5=(addr/10000)%10;    //万位
   a6=(addr/100000)%10;   //十万
       
   a2=(a2<<4)&0xf0;       //转成hex，高位在前，低位在后
   a2=a2|a1;
   
   a4=(a4<<4)&0xf0;       //转成hex，高位在前，低位在后
   a4=a4|a3;
   
   a6=(a6<<4)&0xf0;       //转成hex，高位在前，低位在后
   a6=a6|a5; 
   
   retAdd =a2;
   retAdd |=(a4<<8)&0xff00 ; 
   retAdd |=(a6<<16)&0xff0000 ;
   
   return retAdd;
  
  
}



