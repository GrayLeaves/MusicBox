#ifndef __PLAY_H_
#define __PLAY_H_

#include "playvideo.h"
#include "Global.h"
#include "lcd.h"

//---重定义关键词---//
#ifndef uchar
#define uchar unsigned char
#endif
#ifndef uint 
#define uint unsigned int
#endif
#ifndef ulong
#define ulong unsigned long

#define MaxBuf 33   				 //奇数

uchar wBuf[MaxBuf+2];        //把声调存进数组,掉电丢失
uchar Offset=0,Old=1;        //存储的当前位置
//****************函数列表*************************************************
void Erase(void );           //擦除和初始化wBuf[]里的内容
void Play(uchar *Sound,uchar Signature,uchar Octachord,uint Speed);
/***************************************************************************
* 存储处理函数：Memory processing
*****************************************************************/
void Erase(void )            //擦除和初始化wBuf[]和rBuf[]里的内容
{
   for(k=0;k<(MaxBuf+2);k++)
	 {
	    wBuf[k]=0x00;
	 }
	 REflag=0;
	 Offset=0;
	 LcdShowStr(0,0,welcome);
	 delayms(10);
	 LcdShowStr(0,1,piano);
	 for(k=0;k<=200;k++)            //报警,表擦除数据
	 {
		  BeepIO = !BeepIO;
		  delayus(187);
	 }
}
/*void Replay(void )              //播放数组wBuf[]记录的内容
{
	 uchar s;
	 if(Offset>=8)                  //符合播放要求,Offset为储存个数
	  {
     for(s=0;s<Offset;s+=2)
			{
        ALL=Buf[s];
				TR0=1;                        
			  delayms(200);             //响0.2s
		    TR0=0;                    //关闭定时
				delayms(187);             //间隔0.187s
				ALL=0;
      }
		}
}*/
//simple music
void Play(uchar *Sound,uchar Signature,uchar Octachord,uint Speed)
{
	uint NewFreTab[12];		//新的频率表
	uint Point,LDiv,LDiv0,LDiv1,LDiv2,LDiv4,CurrentFre,Temp_T,SoundLength;
	uchar Tone,Length,SL,SH,SM,SLen,XG,FD;
	
	IsMusic=1;
	BeepIO=0;
	ET1=0; 
	for(i=0;i<12;i++) 				// 根据调号及升降八度来生成新的频率表 
	{
		j = i + Signature;
		if(j > 11)
		{
			j = j-12;
			NewFreTab[i] = FreTab[j]*2;
		}
		else
			NewFreTab[i] = FreTab[j];
		if(Octachord == 1)
			NewFreTab[i]>>=2;
		else if(Octachord == 3)
			NewFreTab[i]<<=2;
	}									
	SoundLength = 0;
	while((Sound[SoundLength]!=0xff)&&(Sound[SoundLength]!=0x00))	//计算歌曲长度
	{
		SoundLength+=2;
	}
	Point = 0;
	Tone   = Sound[Point];	
	Length = Sound[Point+1]; 	      // 读出第一个音符和它时时值
	LDiv0 = 12000/Speed;				    // 算出1分音符的长度(几个10ms) 	
	LDiv4 = LDiv0/4; 				        // 算出4分音符的长度 
	LDiv4 = LDiv4-LDiv4*SOUND_SPACE;// 普通音最长间隔标准 
	TR0	  = 0;
	TR1   = 1;
	while(Point < SoundLength)
	{
		SL=Tone%10; 								//计算出音符 
		LED=Tab[Tone%8];            //LED背景显示
		SM=Tone/10%10; 						  //计算出高低音 
		SH=Tone/100; 								//计算出是否升半 
		CurrentFre = NewFreTab[SignTab[SL-1]+SH]; 	//查出对应音符的频率 	
		if(SL!=0)
		{
			if (SM==1) CurrentFre >>= 2; 		//低音 
			if (SM==3) CurrentFre <<= 2; 		//高音
			Temp_T = 65536-(50000/CurrentFre)*10/(12000000/OSC);//计算计数器初值
			Sound_Temp_TH0 = Temp_T/256; 
			Sound_Temp_TL0 = Temp_T%256; 
			TH0 = Sound_Temp_TH0;  
			TL0 = Sound_Temp_TL0 + 12; //加12是对中断延时的补偿 
		}
		SLen=LengthTab[Length%10];   //算出是几分音符
		XG=Length/10%10; 			       //算出音符类型(0普通1连音2顿音) 
		FD=Length/100;
		LDiv=LDiv0/SLen; 			       //算出连音音符演奏的长度(多少个10ms)
		if (FD==1) 
			LDiv=LDiv+LDiv/2;
		if(XG!=1)	
			if(XG==0) 				         //算出普通音符的演奏长度 
				if (SLen<=4)	
					LDiv1=LDiv-LDiv4;
				else
					LDiv1=LDiv*SOUND_SPACE;
			else
				LDiv1=LDiv/2; 		      //算出顿音的演奏长度 
		else 
			LDiv1=LDiv;
		if(SL==0) LDiv1=0;
			LDiv2=LDiv-LDiv1; 		    //算出不发音的长度 
	  	if (SL!=0)
		{
			TR0=1;
			for(i=LDiv1;i>0;i--) 	    //发规定长度的音 
			{
				while(TF1==0);
				TH1 = Sound_Temp_TH1;
				TL1 = Sound_Temp_TL1;
				TF1=0;
			}
		}
		if(LDiv2!=0)
		{
			TR0=0; BeepIO=0;
			for(i=LDiv2;i>0;i--) 	//音符间的间隔
			{
				while(TF1==0);
				TH1 = Sound_Temp_TH1;
				TL1 = Sound_Temp_TL1;
				TF1=0;
			}
		}
		Point+=2; 
		Tone=Sound[Point];
		Length=Sound[Point+1];
	}
	BeepIO = 0;
	IsMusic=0;                         //控制T0放歌的选择项
}
#endif