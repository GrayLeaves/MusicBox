#ifndef __ISD_h__
#define __ISD_h__

#include "Global.h"

//---重定义关键词---//
#ifndef uchar
#define uchar unsigned char
#endif
#ifndef uint 
#define uint unsigned int
#endif
#ifndef ulong
#define ulong unsigned long

sbit ISD_SS   = P1^0;              	//ISD4002片选
sbit ISD_SCLK = P1^1;              	//ISD4002时钟
sbit ISD_MOSI = P1^2;              	//ISD4002数据输入
sbit ISD_MISO = P1^3;              	//ISD4002数据输出
sbit ISD_INT  =	P3^3;             	//ISD4002溢出或放音结束中断
sbit PR   =  P1^4;              	  //PR=1录音   PR=0放音
sbit RUN  =  P1^5;              	  //执行录音或放音
sbit STOP =  P1^6;               	  //复位
sbit LED  =  P1^7;              	  //指示灯
//**************  函数声明   ********************************************* 
void ISD_WRITE(uchar isdx); 			  //SPI串行发送一个字节子程序
void ISD_STOP(void);					      //发送stop指令程序
void ISD_PU(void);						      //发送上电指令程序
void ISD_PD(void);					      	//发送省电指令程序
void ISD_PLAY(void);					      //发送play指令程序
void ISD_REC(void);						      //发送rec指令程序
void ISD_SETPLAY(uchar adl,uchar adh);	//发送setplay指令程序
void ISD_SETREC(uchar adl,uchar adh);	  //发送setrec指令程序
uchar READ_ISD_OVF(void);				    //读溢出标志OVF程序
void ISD_OVF(void);					      	//LED闪烁程序
//************************************************************************
//                            主程序
//功能：1.录音时,按住RUN键,LED点亮开始录音,松开RUN即可停止录音
//        再次按下RUN键,LED点亮,开始录第二段音,依次类推,直到芯片溢出。
//      2.放音时,按一下RUN键,即播放一段语音,按stop键芯片复位。
//************************************************************************
void ISDpro(void)
{
	P1=0xff;  					//初始化
	while(Isd)
	{	
		while(RUN==1)     				//等待RUN键按下
		{
			if(RUN==0)              //按键防抖动
			{
				delayms(20);
			}
		}
		ISD_PU();        				//RUN键按下为0，ISD上电并延迟50ms
		ISD_PD();       				//停止本次操作并进入省电状态
		ISD_PU();						    //SD上电并延迟50ms
		if(PR==1)     					//如果PR=1则转入录音部分
		{
			delayms(500);     		//延迟录音 指定地址录音
			ISD_SETREC(0x00,0x00);//发送0x0000h地址的setplay指令 
			do
			{ 
				ISD_REC();    			//发送rec指令 下一个地址开始录音
				while(RUN==0)    		//键未松开 等待录音完毕 
				{
					if (ISD_INT==0)   //如果芯片溢出
					ISD_OVF();    		//进行LED闪烁提示
				}
				if(ISD_INT==0)break;//录音溢出，按键RUN释放 则退出录音。则芯片复位
				LED=1;         			//按键释放，未满，录音完毕，LED熄灭
				ISD_STOP();     		//发送停止命令
				while(RUN==1)       //如果RUN未按下 查询 RUN和STOP键
				{
					if(STOP==0)    		//如果按下STOP按键，则芯片复位
					break;
					if (RUN==0)			  //如果RUN再次按下，开始录制下一段语音
					delayms(500);
				}
			}while(RUN==0);
		}
		else             				//如果PR==0则转入放音部分
		{
			while(RUN==0);			//未释放按键时等待
			ISD_SETPLAY(0x00,0x00);     //发送setplay指令，从0x0000地址开始放音
			do
			{ 
				ISD_PLAY();               //发送放音指令
				delayus(20);
				while(ISD_INT==1);        //等待放音完毕的EOM中断信号
				LED=1;					          //LED熄灭
				ISD_STOP();         	    //放音完毕，发送stop指令
				if (READ_ISD_OVF()==1)   	//检查芯片是否溢出 ，如溢出则停止放音，芯片复位
				break;
				while(RUN==1)     		    //等待RUN键再次按下
				{
					if (STOP==0)  		      //如果按下STOP按键，则芯片复位
					break;
					if(RUN==0)			        //如果RUN再次按下，开始播放下一段语音
					delayms(20);
				}
			}while(RUN==0);     		    //RUN键再次按下，播放下一段语音
		}
		ISD_STOP();       	          //发送stop指令
		ISD_PD();						          //停止本次操作并进入省电状态
	}
}
//************************************************************************
//ISD4002 SPI串行发送一个字节子程序，8位数据
//************************************************************************
void ISD_WRITE(uchar isdx) 	//SPI串行发送一个字节子程序
{   
	ISD_SS=0;     						//片选使能,打开SPI通信端
	ISD_SCLK=0;							  //时钟置0
	for(i=0;i<8;i++) 					//8位依次发送
	{ 
		if ((isdx&0x01)==1)			//取最低位，为1则数据线置1
		{
			ISD_MOSI=1;
		}
		else							    //为0则数据线置0
		{
			ISD_MOSI=0;
		}		
		isdx=isdx>>1;					//先发低位再发高位，依次发送
		ISD_SCLK=1;						//时钟产生上升沿 锁存输入的数据
		delayus(2);						  //延时使数据稳定
		ISD_SCLK=0;						//回复时钟低电平
		delayus(2);
	}
}
//***********************************************************************
//发送stop指令
//***********************************************************************
void ISD_STOP(void)					//发送stop指令程序
{   
	delayus(10);
	ISD_WRITE(0x30);					//停止录音或放音操作，指令代码0X11 0
	ISD_SS=1;
	delayms(50);
}
//***********************************************************************
//发送上电指令，并延迟50ms
//***********************************************************************
void ISD_PU(void)						//发送上电指令程序
{  
	delayus(10);
	ISD_SS=0;
	ISD_WRITE(0x20);					//器件上电，指令代码0010 0
	ISD_SS=1;
	delayms(50);
}
//**********************************************************************
//发送省电指令，并延迟50ms
//************************************************************************
void ISD_PD(void)						//发送省电指令程序
{ 
	delayus(10);
  ISD_WRITE(0x10);				//停止本次操作并进入省电状态，指令代码0X01 X
	ISD_SS=1;
	delayms(50);
}
//************************************************************************
//发送play指令
//**************************************************************************
void ISD_PLAY(void)					//发送play指令程序
{
	LED=0;								    //指示灯亮
	ISD_WRITE(0xf0);					//在下一个地址开始放音，指令代码1111 1/0 
	ISD_SS=1;
}
//*************************************************************************
//发送rec指令
//*************************************************************************
void ISD_REC(void)					//发送rec指令程序
{
	LED=0;								    //指示灯亮
	ISD_WRITE(0xb0);					//从下一个地址开始录音
	ISD_SS=1;
}
//**************************************************************************
//发送setplay指令
//**************************************************************************
void ISD_SETPLAY(uchar adl,uchar adh)  	//发送setplay指令程序
{
	ISD_WRITE(adl);    					//发送放音起始地址低位
	adh=adh||0xe0;    					//从地址<A9~A0>开始放音
	ISD_WRITE(adh);   					//发送放音起始地址高位
	ISD_SS=1;
}
//****************************************************************************
//发送setrec指令
//****************************************************************************
void ISD_SETREC(uchar adl,uchar adh)	//发送setrec指令程序
{
	ISD_WRITE(adl);    					//发送录音起始地址低位
	adh=adh||0xa0;   					  //从地址<A9~A0>开始录音
	ISD_WRITE(adh);    					//发送录音起始地址高位
	ISD_SS=1;
}
//*****************************************************************************
//检查芯片是否溢出(读OVF,并返回OVF值)
//*****************************************************************************
uchar READ_ISD_OVF(void)		//读溢出标志OVF程序
{
	ISD_SS=0; 							  //片选有效
	delayus(2);
	ISD_SCLK=0;							  //先产生时钟上升沿
	delayus(2);
	ISD_SCLK=1;
	ISD_SCLK=0;   						//时钟信号下降沿时输出数据
	delayus(2);
	if (ISD_MISO==1) 					//只读第1位数据
	{ 
		ISD_SS =1;     					//关闭SPI通信端
		ISD_STOP();     				//发送stop指令
		return 1;     					//OVF为1，返回1
	}
	else
	{ 
		ISD_SS =1;         			//关闭SPI通信端
		ISD_STOP();     				//发送stop指令
		return 0;     					//OVF为0，返回0
	}
}
//***************************************************************************
//芯片溢出，LED闪烁提醒停止录音
//***************************************************************************
void ISD_OVF(void)					//LED闪烁程序
{
	while(RUN==0)						  //为0 表示键在按下 还未释放 
	{
		LED=1;   						    //指示灯灭
		delayms(300);					  //延时指示灯闪烁
		LED=0;   						    //指示灯亮
		delayms(300);
	}
}
#endif