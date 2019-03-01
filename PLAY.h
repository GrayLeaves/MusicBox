#ifndef __PLAY_H_
#define __PLAY_H_

#include "playvideo.h"
#include "Global.h"
#include "lcd.h"

//---�ض���ؼ���---//
#ifndef uchar
#define uchar unsigned char
#endif
#ifndef uint 
#define uint unsigned int
#endif
#ifndef ulong
#define ulong unsigned long

#define MaxBuf 33   				 //����

uchar wBuf[MaxBuf+2];        //�������������,���綪ʧ
uchar Offset=0,Old=1;        //�洢�ĵ�ǰλ��
//****************�����б�*************************************************
void Erase(void );           //�����ͳ�ʼ��wBuf[]�������
void Play(uchar *Sound,uchar Signature,uchar Octachord,uint Speed);
/***************************************************************************
* �洢��������Memory processing
*****************************************************************/
void Erase(void )            //�����ͳ�ʼ��wBuf[]��rBuf[]�������
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
	 for(k=0;k<=200;k++)            //����,���������
	 {
		  BeepIO = !BeepIO;
		  delayus(187);
	 }
}
/*void Replay(void )              //��������wBuf[]��¼������
{
	 uchar s;
	 if(Offset>=8)                  //���ϲ���Ҫ��,OffsetΪ�������
	  {
     for(s=0;s<Offset;s+=2)
			{
        ALL=Buf[s];
				TR0=1;                        
			  delayms(200);             //��0.2s
		    TR0=0;                    //�رն�ʱ
				delayms(187);             //���0.187s
				ALL=0;
      }
		}
}*/
//simple music
void Play(uchar *Sound,uchar Signature,uchar Octachord,uint Speed)
{
	uint NewFreTab[12];		//�µ�Ƶ�ʱ�
	uint Point,LDiv,LDiv0,LDiv1,LDiv2,LDiv4,CurrentFre,Temp_T,SoundLength;
	uchar Tone,Length,SL,SH,SM,SLen,XG,FD;
	
	IsMusic=1;
	BeepIO=0;
	ET1=0; 
	for(i=0;i<12;i++) 				// ���ݵ��ż������˶��������µ�Ƶ�ʱ� 
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
	while((Sound[SoundLength]!=0xff)&&(Sound[SoundLength]!=0x00))	//�����������
	{
		SoundLength+=2;
	}
	Point = 0;
	Tone   = Sound[Point];	
	Length = Sound[Point+1]; 	      // ������һ����������ʱʱֵ
	LDiv0 = 12000/Speed;				    // ���1�������ĳ���(����10ms) 	
	LDiv4 = LDiv0/4; 				        // ���4�������ĳ��� 
	LDiv4 = LDiv4-LDiv4*SOUND_SPACE;// ��ͨ��������׼ 
	TR0	  = 0;
	TR1   = 1;
	while(Point < SoundLength)
	{
		SL=Tone%10; 								//��������� 
		LED=Tab[Tone%8];            //LED������ʾ
		SM=Tone/10%10; 						  //������ߵ��� 
		SH=Tone/100; 								//������Ƿ����� 
		CurrentFre = NewFreTab[SignTab[SL-1]+SH]; 	//�����Ӧ������Ƶ�� 	
		if(SL!=0)
		{
			if (SM==1) CurrentFre >>= 2; 		//���� 
			if (SM==3) CurrentFre <<= 2; 		//����
			Temp_T = 65536-(50000/CurrentFre)*10/(12000000/OSC);//�����������ֵ
			Sound_Temp_TH0 = Temp_T/256; 
			Sound_Temp_TL0 = Temp_T%256; 
			TH0 = Sound_Temp_TH0;  
			TL0 = Sound_Temp_TL0 + 12; //��12�Ƕ��ж���ʱ�Ĳ��� 
		}
		SLen=LengthTab[Length%10];   //����Ǽ�������
		XG=Length/10%10; 			       //�����������(0��ͨ1����2����) 
		FD=Length/100;
		LDiv=LDiv0/SLen; 			       //���������������ĳ���(���ٸ�10ms)
		if (FD==1) 
			LDiv=LDiv+LDiv/2;
		if(XG!=1)	
			if(XG==0) 				         //�����ͨ���������೤�� 
				if (SLen<=4)	
					LDiv1=LDiv-LDiv4;
				else
					LDiv1=LDiv*SOUND_SPACE;
			else
				LDiv1=LDiv/2; 		      //������������೤�� 
		else 
			LDiv1=LDiv;
		if(SL==0) LDiv1=0;
			LDiv2=LDiv-LDiv1; 		    //����������ĳ��� 
	  	if (SL!=0)
		{
			TR0=1;
			for(i=LDiv1;i>0;i--) 	    //���涨���ȵ��� 
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
			for(i=LDiv2;i>0;i--) 	//������ļ��
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
	IsMusic=0;                         //����T0�Ÿ��ѡ����
}
#endif