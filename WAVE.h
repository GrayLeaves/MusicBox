#ifndef __WAVE_h_
#define __WAVE_h_

#include "Global.h"
#include "lcd.h"
#include "Temp.h"

//---重定义关键词---//
#ifndef uchar
#define uchar unsigned char
#endif
#ifndef uint 
#define uint unsigned int
#endif
#ifndef ulong
#define ulong unsigned long

//******************超声波测距******************************
sbit Trig = P3^5;
sbit Echo = P3^4;			  
uint  tc = 0;
ulong S = 0;
int  Temp;                   //温度值
bit flag=0;                  //距离是否超过6m
uchar Out[7];                //温度和测距结果显示存放数组
//**********************************************************
void Tempros(void );              //温度处理函数
void Count(void );                //计算距离
void Show(void );                 //显示温度,测距结果
void StartModule(void ); 		      //启动模块
void Ultrasonic(void );           //启动测距
/*******************************************************************************
* 测距模块 : Ultrasonic ranging
*******************************************************************************/
//*************************温度处理函数*********************************
void Tempros(void )
{
	 float tp;                  //临时变量
	 int  temp;             //温度变量
//***************************温度解码*****************************************
	 temp=Ds18b20ReadTemp();
	 	//数据处理有小数点,将温度赋给一个浮点型变量
		//如果温度是正的那么,那么正数的原码就是补码它本身
	 if(temp>=0)		 //当温度值为非负数
	   {
			 tp=(float)temp;
		   Temp=tp*0.0625*100+0.5;	//转化为int型变量
	   }
	 else
	   {   
	  //因为读取的温度是实际温度的补码,所以减1,再取反求出原码
			 temp=temp-1;
			 temp=!temp;
			 tp=(float)temp;
			 Temp=tp*6.25+0.5;	
		//注意数值是温度值的100倍,用于精确到百分位
  	 }
		//显示此时温度
		 Out[0]=Temp%10000/1000;
		 Out[1]=Temp%1000/100;
		 Out[2]=Temp%100/10;
		 Out[3]=Temp%10;			 
}
void Count(void )
{
	 tc=TH1*256+TL1;
	 TH1=0;
	 TL1=0;
	//**********************测距结果与处理****************************************
		//显示此时测距结果超出范围
		 if(flag==1)
		 {
			 flag=0;
			 LcdShowStr(0,0,failstr);
		 }
		 else
		 {
			 S=tc*(331.45+0.61*Temp/100)/20000;     
/*算出来是以CM为单位的整数,含温度补偿功能,超声波声速：v=331.45+0.61*Temp m/s */
			 Out[4]=S%1000/100;
			 Out[5]=S%1000%100/10;
			 Out[6]=S%1000%10 %10;		 		 
       if((S<200)&&Ulsign)     //测距在2m内,且启用则发出音符警报
				 {
					 ALL=S/10+1;
					 TR0=1;
					 delayms(100);
					 TR0=0;
					 delayms(200);
				 }
			 }
}
void Show(void )
{
		LcdShowStr(0,0,tempstr); //显示当前温度面板
		LcdShowStr(0,1,lenstr);  //显示当前测距结果面板
		//显示温度0
		DisplayOneChar( 8, 0, ASCII[Out[0]]);
		DisplayOneChar( 9, 0, ASCII[Out[1]]);	
		DisplayOneChar(11, 0, ASCII[Out[2]]);
		DisplayOneChar(12, 0, ASCII[Out[3]]);
		//显示测距结果1
		DisplayOneChar( 9, 1, ASCII[Out[4]]);
		DisplayOneChar(11, 1, ASCII[Out[5]]);	
		DisplayOneChar(12, 1, ASCII[Out[6]]);
}
void StartModule(void ) 		         //启动模块
{
	  Trig=1;			                     //启动一次模块
	  _nop_(); _nop_();  _nop_(); _nop_(); _nop_(); _nop_(); _nop_();
    _nop_(); _nop_();  _nop_(); _nop_(); _nop_(); _nop_(); _nop_();
	  _nop_(); _nop_();  _nop_(); _nop_(); _nop_(); _nop_(); _nop_();
	  Trig=0;
}

void Ultrasonic(void )
{
	 TH1=0;
	 TL1=0;          
	 flag=0;           //清溢出标志
	 StartModule();
	 while(!Echo);		//当RX为零时等待
	 TR1=1;			      //开启计数
	 while(Echo);			//当RX为1计数并等待
	 TR1=0;				    //关闭计数
	 if(TF1)          //不用中断的方法
		 {
			 TF1=0;flag=1;//超声波测距超出范围标志
		 }
	 Tempros();    //计算温度值
	 Count();			 //计算测距结果
	 Show();       //显示上述结果
}

#endif