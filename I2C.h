#ifndef __I2C_h__
#define __I2C_h__

#include <intrins.h>	
#include "Global.h"

#define  PCF8591 0x90    //PCF8591 地址
//**********************************************************************
uchar AD_which=0;      //AD/DA选通
uint AD[5];
uchar Out[8];
//*****************************函数声明************************************
void Start_I2c(void );    //启动总线函数  
void Stop_I2c(void );     //结束总线函数 
void Ack_I2c(bit a);      //应答子函数
void  SendByte(uchar c);  //字节数据发送函数
uchar RcvByte(void );     //字节数据接收函数 
bit DACconversion(uchar sla,uchar c,uchar Val);//DAC变换/转化函数 
bit ISendByte(uchar sla,uchar c);//ADC发送字节[命令]数据函数
uchar IRcvByte(uchar sla);//ADC读字节数据函数 
//void Init_timer2(void );  //定时器2初始化
/*************************此部分为I2C总线的驱动程序***********************/
sbit SCL=P2^0;       //I2C  时钟 
sbit SDA=P2^1;       //I2C  数据 
bit ack;             /*应答标志位*/
//**********************总AD/DA转换处理函数******************************/
void ADpro(void )
{  
// Init_timer2();
  while(IsAD)
   {
/********************以下AD-DA处理***************************************/  
		 switch(AD_which)
			{
				case 0: ISendByte(PCF8591,0x41);
								AD[0]=IRcvByte(PCF8591)*2;   //ADC0模数转换1
								break;  
				case 1: ISendByte(PCF8591,0x42);
								AD[1]=IRcvByte(PCF8591)*2;   //ADC1模数转换2
								break;  
				case 2: ISendByte(PCF8591,0x43);
								AD[2]=IRcvByte(PCF8591)*2;   //ADC2模数转换3
								break;  
				case 3: ISendByte(PCF8591,0x40);
								AD[3]=IRcvByte(PCF8591)*2;   //ADC3模数转换4
								break;  

				case 4: DACconversion(PCF8591,0x40,AD[4]/2);    //DAC数模转换
								break;
			}
			AD[4]=500;//数字--->>模拟输出 输出4.56V  		  
			if(AD_which==4)AD_which=0;
			//温度
			Out[0]=AD[0]%10000/1000;
			Out[1]=AD[0]%1000/100;
			Out[2]=AD[0]%100/10;
			Out[3]=AD[0]%10;
			//亮度
			Out[4]=AD[1]%10000/1000;
			Out[5]=AD[1]%1000/100;
			Out[6]=AD[1]%100/10;
			Out[7]=AD[1]%10;
			for(k=1;k<8;k++)
			 {
	       Disp(k,Out[k]);//第1位数码管显示	
         delayms(100);				 
			 }
  }  
}
/*******************************************************************
                     起动总线函数               
函数原型: void  Start_I2c();  
功能:     启动I2C总线,即发送I2C起始条件.  
********************************************************************/
void Start_I2c(void )
{
	SDA=1;         /*发送起始条件的数据信号*/
	_nop_();
	SCL=1;
	_nop_();        /*起始条件建立时间大于4.7us,延时*/
	_nop_();
	_nop_();
	_nop_();
	_nop_();    
	SDA=0;         /*发送起始信号*/
	_nop_();        /* 起始条件锁定时间大于4μs*/
	_nop_();
	_nop_();
	_nop_();
	_nop_();      
	SCL=0;       /*钳住I2C总线，准备发送或接收数据 */
	_nop_();
	_nop_();
}
/*******************************************************************
                      结束总线函数               
函数原型: void  Stop_I2c();  
功能:     结束I2C总线,即发送I2C结束条件.  
********************************************************************/
void Stop_I2c(void )
{
	SDA=0;      /*发送结束条件的数据信号*/
	_nop_();       /*发送结束条件的时钟信号*/
	SCL=1;      /*结束条件建立时间大于4μs*/
	_nop_();
	_nop_();
	_nop_();
	_nop_(); 
	_nop_();
	SDA=1;      /*发送I2C总线结束信号*/
	_nop_();
	_nop_();
	_nop_();
	_nop_(); 
}
/*******************************************************************
                 字节数据发送函数               
函数原型: void  SendByte(UCHAR c);
功能:     将数据c发送出去,可以是地址,也可以是数据,发完后等待应答,并对
          此状态位进行操作.(不应答或非应答都使ack=0)     
           发送数据正常，ack=1; ack=0表示被控器无应答或损坏。
********************************************************************/
void SendByte(uchar c)
{
   uchar  BitCnt;

   for(BitCnt=0;BitCnt<8;BitCnt++)  /*要传送的数据长度为8位*/
   {
	 if((c<<BitCnt)&0x80)SDA=1;   /*判断发送位*/
     else  SDA=0;                
	_nop_();
	SCL=1;               /*置时钟线为高，通知被控器开始接收数据位*/
	_nop_(); 
	_nop_();             /*保证时钟高电平周期大于4μs*/
	_nop_();
	_nop_();
	_nop_();
	 SCL=0; 
   }

	_nop_();
	_nop_();
	SDA=1;                /*8位发送完后释放数据线，准备接收应答位*/
	_nop_();
	_nop_();   
	SCL=1;
	_nop_();
	_nop_();
	_nop_();
	if(SDA==1)ack=0;     
	   else ack=1;        /*判断是否接收到应答信号*/
	SCL=0;
	_nop_();
	_nop_();
}
/*******************************************************************
                 字节数据接收函数               
函数原型: UCHAR  RcvByte();
功能:        用来接收从器件传来的数据,并判断总线错误(不发应答信号)，
          发完后请用应答函数应答从机。  
********************************************************************/    
uchar RcvByte(void )
{
	uchar  retc;
	uchar  BitCnt;

	retc=0; 
	SDA=1;                     /*置数据线为输入方式*/
	for(BitCnt=0;BitCnt<8;BitCnt++)
	{
	_nop_();           
	SCL=0;                  /*置时钟线为低，准备接收数据位*/
	_nop_();
	_nop_();                 /*时钟低电平周期大于4.7μs*/
	_nop_();
	_nop_();
	_nop_();
	SCL=1;                  /*置时钟线为高使数据线上数据有效*/
	_nop_();
	_nop_();
	retc=retc<<1;
	if(SDA==1)retc=retc+1;  /*读数据位,接收的数据位放入retc中 */
	_nop_();
	_nop_(); 
	}
	SCL=0;    
	_nop_();
	_nop_();
	return(retc);
}
/********************************************************************
                     应答子函数
函数原型:  void Ack_I2c(bit a);
功能:      主控器进行应答信号(可以是应答或非应答信号，由位参数a决定)
********************************************************************/
void Ack_I2c(bit a)
{
	if(a==0)SDA=0;              /*在此发出应答或非应答信号 */
	else SDA=1;
	_nop_();
	_nop_();
	_nop_();      
	SCL=1;
	_nop_();
	_nop_();                   /*时钟低电平周期大于4μs*/
	_nop_();
	_nop_();
	_nop_();  
	SCL=0;                     /*清时钟线，钳住I2C总线以便继续接收*/
	_nop_();
	_nop_();    
}
/*******************************************************************
DAC 变换, 转化函数               
*******************************************************************/
bit DACconversion(uchar sla,uchar c,uchar Val)
{
   Start_I2c();              //启动总线
   SendByte(sla);            //发送器件地址
   if(ack==0)return(0);
   SendByte(c);              //发送控制字节
   if(ack==0)return(0);
   SendByte(Val);            //发送DAC的数值  
   if(ack==0)return(0);
   Stop_I2c();               //结束总线
   return(1);
}
/*******************************************************************
ADC发送字节[命令]数据函数               
*******************************************************************/
bit ISendByte(uchar sla,uchar c)
{
   Start_I2c();              //启动总线
   SendByte(sla);            //发送器件地址
   if(ack==0)return(0);
   SendByte(c);              //发送数据
   if(ack==0)return(0);
   Stop_I2c();               //结束总线
   return(1);
}
/*******************************************************************
ADC读字节数据函数               
*******************************************************************/
uchar IRcvByte(uchar sla)
{  
	 uchar c;
   Start_I2c();          //启动总线
   SendByte(sla+1);      //发送器件地址
   if(ack==0)return(0);
   c=RcvByte();          //读取数据0
   Ack_I2c(1);           //发送非就答位
   Stop_I2c();           //结束总线
   return(c);
}
/*//定时器2初始化
void Init_timer2(void )
{
	 RCAP2H=(65536-15000)/256;//赋T2初始值0x0bdc，溢出时间为50ms
	 RCAP2L=(65536-15000)%256;
	 TR2=1;	     //启动定时器2
	 ET2=1;		 //打开定时器2中断
	 EA=1;		 //打开总中断
}
//定时器中断函数
void Timer2(void ) interrupt 5	  //定时器2是5号中断
{	
	 TF2=0;
}*/
#endif