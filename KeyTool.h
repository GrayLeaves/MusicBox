#ifndef __KEYTOOL_h_
#define __KEYTOOL_h_

#include "playvideo.h"
#include "PLAY.h"
#include "lcd.h"
#include "AT24C02.h"
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

#define GPIO_KEY P1
uchar JiePai=0x02;
//****************全局变量******************************************************
uchar KeyValue=0;           			//用来存放读取到的键值
bit   KeyState=0;                 //用来存放按键状态
bit   WrOut=0;                    //用来判断是否存储到外部存储器
//****************函数列表******************************************************
void Test(void );
void Beat(void );                 //节拍生成函数
void Record(void );               //音乐数据逐个记录到数组wBuf[]里
void Replay(void );               //播放数组wBuf[]记录的内容
void Coplay(void );               //取出外部存储器的音乐数据并播放
void Key4(void );                 //功能选择键
void Matrix(void );               //矩阵式处理数值函数
void KeyDown(void );              //矩阵键盘扫描
void Irpros(void );               //遥控器处理函数

/********************************************************************************
**函 数 名 : Beat
* 函数功能 : 根据按键生成节拍延时
*********************************************************************************/
void Beat(void )
{
	if(REflag&&(Offset>=2))
	{
		switch(KeyValue)    //节拍赋值规则,忽略分隔符和默认节拍,修改默认参数
			 {
/*			case 12:{//升半阶,可重复加,溢出自动%256
									 wBuf[Offset-1]+=100;
									 break;
								 }*/
				 case 13:{//延音-1，可重复减
					         if(wBuf[Offset-1]<2)wBuf[Offset-1]=0x00;
				           else wBuf[Offset-1]-=1;
					         break;
				         }
				 case 14:{//浮点+100,不可重复加
					         if(Offset!=Old)        //放过首次
									 {
                     wBuf[Offset-1]+=100;
									 }
									 break;
				         }
				 case 15:{//连音线+10,不可重复加
									 if(Offset!=Old)
									 {
                     wBuf[Offset-1]+=10;
									 }
									 break;
				         }
				 case 16:{//休止符+20,不可重复加
									 if(Offset!=Old)
									 {
                     wBuf[Offset-1]+=20;
									 }
									 break;
				         }
				 default:;
			 }
			 Old=Offset;                        //记录旧值
			 if(WrOut)
				 {
				   At24c02Write(Offset-1,wBuf[Offset-1]);   //在地址Offset-1内写入数据wBuf[Offset-1],记录修改后的节拍
				 }
	}
}
/********************************************************************************
**函 数 名 : Record
* 函数功能 : 记录
*********************************************************************************/
void Record(void )               //音乐数据逐个记录到数组wBuf[]里
{
  if(REflag)	                   //检测是否存储
	 { 
		if(Offset<MaxBuf)
			{
			  switch(EXT)              //修改音调
				 {
					case  0:{wBuf[Offset]=Diao+10;break;}
					case  7:{wBuf[Offset]=Diao+20;break;}
					case 14:{wBuf[Offset]=Diao+30;break;}
				 }
				 wBuf[Offset+1]=JiePai;    //默认节拍
				 if(Offset<=14)
				 LcdShowStr(Offset,0,YF[ALL]);
				 else
				 {
				   if(Offset<=30)
				   LcdShowStr((Offset-16),1,YF[ALL]);
         }
				 if(Wrout)
				 {
				   At24c02Write(Offset,wBuf[Offset]);   //在地址Offset内写入数据wBuf[Offset],记录音符
					 At24c02Write(Offset+1,wBuf[Offset+1]);   //在地址Offset+1内写入数据wBuf[Offset+1],记录默认节拍
					 At24c02Write(MaxBuf,Offset);         //在地址MaxBuf内写入数据Offset,记录存储的个数(时刻更新)
				 }
				 Offset+=2;            //右移两位
      }
		else
		  {
				REflag=0;
				LED=0xff;
			  for(k=0;k<=7;k++)      //报警,表充满数据
					 {
						 LED=Tab[k];
						 delayms(125);
					 }             
      }
	 }
}
/*******************************************************************************
* 函 数 名 : Replay
* 函数功能 : 功能选择键
*******************************************************************************/
void Replay(void )               //播放数组wBuf[]记录的内容
{
	if(Offset>=6)                 //符合播放要求,Offset为储存个数
	 {
		 if(wBuf[Offset]!=0xff)
		 { //结尾处为0xff 0xff
			 	wBuf[Offset]=0xff;
			  wBuf[Offset+1]=0xff;
			  Play(wBuf,0,3,360);        //默认就好
		    delayms(200);	
		 }
	 }
}
void Coplay(void )
{
	uchar xdata Cache[MaxBuf+2];
	uchar MAX=0;
	
  MAX=At24c02Read(MaxBuf);
	if(MAX>=6)
	{//结尾处为0xff 0xff
		 wBuf[MAX+1]=0xff;
		 wBuf[MAX+2]=0xff;
		 for(k=0;(Cache[k]!=0xff)&&(Cache[k]!=0);k+=2)
			 {
				 Cache[k]=At24c02Read(k);
				 i=Cache[k]%10+7*(Cache[k]/10-1);
				 if(k<=14)
				 LcdShowStr(k,0,YF[i]);
				 else
				 {
					 if(k<=30)
					 LcdShowStr((k-16),1,YF[i]);
				 }
			 }
		Play(Cache,0,3,360);
	}
}
/*******************************************************************************
* 函 数 名 : Key4
* 函数功能 : 功能选择键
*******************************************************************************/
void Key4(void )
{
	if(K1==0)			//启动音乐
	{
		delayms(10);
		if(K1==0)
		{
			while(!K1);
		  Initial();                          //音乐盒
		  Playvideo(Diao,0,3,300+60*EXT/7);
		  delayms(500);	
		}
	}
	
	if(K2==0)      //功能转换键
	{
		delayms(10);
		if(K2==0)
		 {
			 while(!K2);
			 WrOut=!WrOut;
		 }
		delayms(500);
	}
}
/*******************************************************************************
* 函 数 名  : Test
* 函数功能	: 初始化
*******************************************************************************/
void Test(void )                //初始化和启动提示
{
  LED=0x00;
  delayms(300);
	LED=0xe7;                     //中音标志
	//电子琴初始化
	TMOD|= 0x11;
	ET0 = 1;                      //使能定时器T0溢出中断源
	TR0 = 0;                      //关闭定时器T0
	EA  = 1;                      //CPU开中断
  InitLcd1602();                //LCD1602初始化
  Erase();
	IsMusic=0;
}
/*******************************************************************************
* 函 数 名   : KeyDown
* 函数功能	 : 矩阵键盘扫描检测有按键按下并读取键值
*******************************************************************************/
void KeyDown(void )
{
	uint keep=0;
	GPIO_KEY=0x0f;
	if(GPIO_KEY!=0x0f)
	{
		delayms(10);
		if(GPIO_KEY!=0x0f)
		{
			KeyState=1;               //检测到按键输入
//***********************测试列********************************
			GPIO_KEY=0x0f;
      delayms(10);
			switch(GPIO_KEY)
			{
				case(0x07):	KeyValue=1;break;
				case(0x0b):	KeyValue=2;break;
				case(0x0d): KeyValue=3;break;
				case(0x0e):	KeyValue=4;break;
           default: KeyValue=0;	           //检测出错
			}
//**********************测试行*********************************
			GPIO_KEY=0xf0;
			delayms(10);
			switch(GPIO_KEY)
			{
				case(0x70):	KeyValue=KeyValue;break;
				case(0xb0):	KeyValue=KeyValue+4;break;
				case(0xd0): KeyValue=KeyValue+8;break;
				case(0xe0):	KeyValue=KeyValue+12;break;
           default:	KeyValue=0;            //检测出错
			}
			while((keep<100)&&(GPIO_KEY!=0xf0))	 //按键松手检测
			{
				delayms(10);
				keep++;
			}
			keep=0;
		}
	}
	GPIO_KEY=0xff;
}
//**************************红外处理函数***********************************
void Irpros(void )
{
	if(IrValue[2]!=0x00)        //有按键按下
	{
   KeyState=1;
	 switch(IrValue[2])
	   {//遥控器处理函数,与矩阵键盘相容
      case(0x0d): KeyValue= 9; break; //record control
			case(0x5e): KeyValue=10; break; //replay
			case(0x5a): KeyValue=11; break; //erase
      case(0x4a): KeyValue=12; break; //#ultrasonic wave
			 
			case(0x16): KeyValue=13; break; //延时
			case(0x0c): KeyValue=14; break; //连音线
      case(0x08): KeyValue=15; break; //浮点/升半阶
			case(0x42): KeyValue=16; break; //*休止符
			
			case(0x43): KeyValue= 8; break; //上
      case(0x44): KeyValue=17; break; //下
			 
			case(0x46): KeyValue= 1; break; //DO
			case(0x40): KeyValue= 2; break; //RE
      case(0x15): KeyValue= 3; break; //MI
			case(0x19): KeyValue= 4; break; //FA
			case(0x18): KeyValue= 5; break; //SO
      case(0x1c): KeyValue= 6; break; //LA
			case(0x52): KeyValue= 7; break; //SI
         default: KeyValue= 0;       //检测出错				
    }
		IrValue[2]=0x00;              //处理完后清零,防止再度起作用
		delayms(10);
	}
}
//*********************************************************
void Matrix(void )
{
	KeyDown();                     //矩阵按键检测函数
	Irpros();                      //红外按键处理函数
	
	if(KeyState&&(KeyValue!=0))    //状态或赋值非零
	  {
		 KeyState=0;
		 if((KeyValue==8)||(KeyValue==17))
			{
				if(KeyValue==8)
				  {
					  EXT+=7;              //EXT必须默认为7!
				    if(EXT==21)EXT=0;
          }
			  else
				  {
				    if(EXT==0)EXT=14;
						else EXT-=7;         //EXT必须默认为7!
          }
			  switch(EXT)              //修改音调和记录默认节拍
				 {
					case  0:LED=0xfc;break;
					case  7:LED=0xe7;break;
					case 14:LED=0x3f;break;
				 }
				delayms(500);
			}
		 else
			{
				if(KeyValue<=7)                  //音调转换
					{
//*****************八音总播放器**************************************************
						Diao=KeyValue;           //音阶赋值
						ALL = Diao+EXT;          //总值
						Record();                //记录
						TR0=1;                   //启动发音
						delayms(250);			       //发音时间
						TR0=0;                   //关闭定时
						Diao=0;                  //防止重复触发
//*******************************************************************************							
					}
				 else
					{
						 if(KeyValue>=12)
							{
								if(REflag)
								{
								  Beat();                         //更新值
									if((KeyValue==12)&&(Offset>=6)) //删除前一个记录(已记录三个以上的数据)
									 {
								     Offset-=2;                   //左移
									   wBuf[Offset]+=0xff;          //注释掉数据
					           wBuf[Offset+1]+=0xff;
										 if(Offset<=14)                //显示空,即表示删除该数据
										    LcdShowStr(Offset,0,YF[0]);
										 else
										  {
											  if(Offset<=30)
											  LcdShowStr((Offset-16),1,YF[0]);
										  }
										 delayms(500);
									 }
								}
								else
								{
   								//==在非记录音乐的模式下启动超声波测距,改变发声属性==
			            if(KeyValue==12)
										{
											Ulsign=1;         //启动超声波测距模块
                      delayms(500);
										}
										
									switch(KeyValue)      //修改节拍默认值    
									  {
										  case 13:JiePai=0x01;break;//慢速
										  case 14:JiePai=0x02;break;//常速
										  case 15:JiePai=0x03;break;//中速
										  default:JiePai=0x02;break;
									  }
			            if(KeyValue==16)     //播放存储在AT24C02的音乐
										{
											Coplay();
                      delayms(500);
										}										
										
									delayms(300);
                }
							}
						 else
							{		//==记录控制==
									if(KeyValue==9)      //是否要记录
										{
											REflag=!REflag;
											if(REflag)       //记录,全亮
											{
													LcdShowStr(0,0,Null);  //重新记录,擦屏
													delayms(10);
													LcdShowStr(0,1,Null);
													delayms(10);
													if(Offset>1)           //不是重新记录,打印出已记录的内容
													{
														for(k=0;k<Offset;k+=2)
															{
																 i=wBuf[k]%10+7*(wBuf[k]/10-1);
																 if(k<=14)
																 LcdShowStr(k,0,YF[i]);
																 else
																 {
																	 if(k<=30)
																	 LcdShowStr((k-16),1,YF[i]);
																 }
															}
													}
												 LED=0x00;
											}
											else							//舍弃,全灭
											{
												LED=0xff;
											}
											delayms(500);
										}
									//==播放存储在数组的音乐01==
									if(KeyValue==10)
										{
											Replay();
										}
									//==擦除音乐数据==//数组和外部存储器都清零
									if(KeyValue==11)
										{
											Erase();      //按复位键擦除数组数据,不影响外部存储器的数据
										}
							}
	          }
       }
    }
}//end void
#endif