#ifndef __KEYTOOL_h_
#define __KEYTOOL_h_

#include "playvideo.h"
#include "PLAY.h"
#include "lcd.h"
#include "AT24C02.h"
#include "Global.h"

//---�ض���ؼ���---//
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
//****************ȫ�ֱ���******************************************************
uchar KeyValue=0;           			//������Ŷ�ȡ���ļ�ֵ
bit   KeyState=0;                 //������Ű���״̬
bit   WrOut=0;                    //�����ж��Ƿ�洢���ⲿ�洢��
//****************�����б�******************************************************
void Test(void );
void Beat(void );                 //�������ɺ���
void Record(void );               //�������������¼������wBuf[]��
void Replay(void );               //��������wBuf[]��¼������
void Coplay(void );               //ȡ���ⲿ�洢�����������ݲ�����
void Key4(void );                 //����ѡ���
void Matrix(void );               //����ʽ������ֵ����
void KeyDown(void );              //�������ɨ��
void Irpros(void );               //ң����������

/********************************************************************************
**�� �� �� : Beat
* �������� : ���ݰ������ɽ�����ʱ
*********************************************************************************/
void Beat(void )
{
	if(REflag&&(Offset>=2))
	{
		switch(KeyValue)    //���ĸ�ֵ����,���Էָ�����Ĭ�Ͻ���,�޸�Ĭ�ϲ���
			 {
/*			case 12:{//�����,���ظ���,����Զ�%256
									 wBuf[Offset-1]+=100;
									 break;
								 }*/
				 case 13:{//����-1�����ظ���
					         if(wBuf[Offset-1]<2)wBuf[Offset-1]=0x00;
				           else wBuf[Offset-1]-=1;
					         break;
				         }
				 case 14:{//����+100,�����ظ���
					         if(Offset!=Old)        //�Ź��״�
									 {
                     wBuf[Offset-1]+=100;
									 }
									 break;
				         }
				 case 15:{//������+10,�����ظ���
									 if(Offset!=Old)
									 {
                     wBuf[Offset-1]+=10;
									 }
									 break;
				         }
				 case 16:{//��ֹ��+20,�����ظ���
									 if(Offset!=Old)
									 {
                     wBuf[Offset-1]+=20;
									 }
									 break;
				         }
				 default:;
			 }
			 Old=Offset;                        //��¼��ֵ
			 if(WrOut)
				 {
				   At24c02Write(Offset-1,wBuf[Offset-1]);   //�ڵ�ַOffset-1��д������wBuf[Offset-1],��¼�޸ĺ�Ľ���
				 }
	}
}
/********************************************************************************
**�� �� �� : Record
* �������� : ��¼
*********************************************************************************/
void Record(void )               //�������������¼������wBuf[]��
{
  if(REflag)	                   //����Ƿ�洢
	 { 
		if(Offset<MaxBuf)
			{
			  switch(EXT)              //�޸�����
				 {
					case  0:{wBuf[Offset]=Diao+10;break;}
					case  7:{wBuf[Offset]=Diao+20;break;}
					case 14:{wBuf[Offset]=Diao+30;break;}
				 }
				 wBuf[Offset+1]=JiePai;    //Ĭ�Ͻ���
				 if(Offset<=14)
				 LcdShowStr(Offset,0,YF[ALL]);
				 else
				 {
				   if(Offset<=30)
				   LcdShowStr((Offset-16),1,YF[ALL]);
         }
				 if(Wrout)
				 {
				   At24c02Write(Offset,wBuf[Offset]);   //�ڵ�ַOffset��д������wBuf[Offset],��¼����
					 At24c02Write(Offset+1,wBuf[Offset+1]);   //�ڵ�ַOffset+1��д������wBuf[Offset+1],��¼Ĭ�Ͻ���
					 At24c02Write(MaxBuf,Offset);         //�ڵ�ַMaxBuf��д������Offset,��¼�洢�ĸ���(ʱ�̸���)
				 }
				 Offset+=2;            //������λ
      }
		else
		  {
				REflag=0;
				LED=0xff;
			  for(k=0;k<=7;k++)      //����,���������
					 {
						 LED=Tab[k];
						 delayms(125);
					 }             
      }
	 }
}
/*******************************************************************************
* �� �� �� : Replay
* �������� : ����ѡ���
*******************************************************************************/
void Replay(void )               //��������wBuf[]��¼������
{
	if(Offset>=6)                 //���ϲ���Ҫ��,OffsetΪ�������
	 {
		 if(wBuf[Offset]!=0xff)
		 { //��β��Ϊ0xff 0xff
			 	wBuf[Offset]=0xff;
			  wBuf[Offset+1]=0xff;
			  Play(wBuf,0,3,360);        //Ĭ�Ͼͺ�
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
	{//��β��Ϊ0xff 0xff
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
* �� �� �� : Key4
* �������� : ����ѡ���
*******************************************************************************/
void Key4(void )
{
	if(K1==0)			//��������
	{
		delayms(10);
		if(K1==0)
		{
			while(!K1);
		  Initial();                          //���ֺ�
		  Playvideo(Diao,0,3,300+60*EXT/7);
		  delayms(500);	
		}
	}
	
	if(K2==0)      //����ת����
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
* �� �� ��  : Test
* ��������	: ��ʼ��
*******************************************************************************/
void Test(void )                //��ʼ����������ʾ
{
  LED=0x00;
  delayms(300);
	LED=0xe7;                     //������־
	//�����ٳ�ʼ��
	TMOD|= 0x11;
	ET0 = 1;                      //ʹ�ܶ�ʱ��T0����ж�Դ
	TR0 = 0;                      //�رն�ʱ��T0
	EA  = 1;                      //CPU���ж�
  InitLcd1602();                //LCD1602��ʼ��
  Erase();
	IsMusic=0;
}
/*******************************************************************************
* �� �� ��   : KeyDown
* ��������	 : �������ɨ�����а������²���ȡ��ֵ
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
			KeyState=1;               //��⵽��������
//***********************������********************************
			GPIO_KEY=0x0f;
      delayms(10);
			switch(GPIO_KEY)
			{
				case(0x07):	KeyValue=1;break;
				case(0x0b):	KeyValue=2;break;
				case(0x0d): KeyValue=3;break;
				case(0x0e):	KeyValue=4;break;
           default: KeyValue=0;	           //������
			}
//**********************������*********************************
			GPIO_KEY=0xf0;
			delayms(10);
			switch(GPIO_KEY)
			{
				case(0x70):	KeyValue=KeyValue;break;
				case(0xb0):	KeyValue=KeyValue+4;break;
				case(0xd0): KeyValue=KeyValue+8;break;
				case(0xe0):	KeyValue=KeyValue+12;break;
           default:	KeyValue=0;            //������
			}
			while((keep<100)&&(GPIO_KEY!=0xf0))	 //�������ּ��
			{
				delayms(10);
				keep++;
			}
			keep=0;
		}
	}
	GPIO_KEY=0xff;
}
//**************************���⴦����***********************************
void Irpros(void )
{
	if(IrValue[2]!=0x00)        //�а�������
	{
   KeyState=1;
	 switch(IrValue[2])
	   {//ң����������,������������
      case(0x0d): KeyValue= 9; break; //record control
			case(0x5e): KeyValue=10; break; //replay
			case(0x5a): KeyValue=11; break; //erase
      case(0x4a): KeyValue=12; break; //#ultrasonic wave
			 
			case(0x16): KeyValue=13; break; //��ʱ
			case(0x0c): KeyValue=14; break; //������
      case(0x08): KeyValue=15; break; //����/�����
			case(0x42): KeyValue=16; break; //*��ֹ��
			
			case(0x43): KeyValue= 8; break; //��
      case(0x44): KeyValue=17; break; //��
			 
			case(0x46): KeyValue= 1; break; //DO
			case(0x40): KeyValue= 2; break; //RE
      case(0x15): KeyValue= 3; break; //MI
			case(0x19): KeyValue= 4; break; //FA
			case(0x18): KeyValue= 5; break; //SO
      case(0x1c): KeyValue= 6; break; //LA
			case(0x52): KeyValue= 7; break; //SI
         default: KeyValue= 0;       //������				
    }
		IrValue[2]=0x00;              //�����������,��ֹ�ٶ�������
		delayms(10);
	}
}
//*********************************************************
void Matrix(void )
{
	KeyDown();                     //���󰴼���⺯��
	Irpros();                      //���ⰴ��������
	
	if(KeyState&&(KeyValue!=0))    //״̬��ֵ����
	  {
		 KeyState=0;
		 if((KeyValue==8)||(KeyValue==17))
			{
				if(KeyValue==8)
				  {
					  EXT+=7;              //EXT����Ĭ��Ϊ7!
				    if(EXT==21)EXT=0;
          }
			  else
				  {
				    if(EXT==0)EXT=14;
						else EXT-=7;         //EXT����Ĭ��Ϊ7!
          }
			  switch(EXT)              //�޸������ͼ�¼Ĭ�Ͻ���
				 {
					case  0:LED=0xfc;break;
					case  7:LED=0xe7;break;
					case 14:LED=0x3f;break;
				 }
				delayms(500);
			}
		 else
			{
				if(KeyValue<=7)                  //����ת��
					{
//*****************�����ܲ�����**************************************************
						Diao=KeyValue;           //���׸�ֵ
						ALL = Diao+EXT;          //��ֵ
						Record();                //��¼
						TR0=1;                   //��������
						delayms(250);			       //����ʱ��
						TR0=0;                   //�رն�ʱ
						Diao=0;                  //��ֹ�ظ�����
//*******************************************************************************							
					}
				 else
					{
						 if(KeyValue>=12)
							{
								if(REflag)
								{
								  Beat();                         //����ֵ
									if((KeyValue==12)&&(Offset>=6)) //ɾ��ǰһ����¼(�Ѽ�¼�������ϵ�����)
									 {
								     Offset-=2;                   //����
									   wBuf[Offset]+=0xff;          //ע�͵�����
					           wBuf[Offset+1]+=0xff;
										 if(Offset<=14)                //��ʾ��,����ʾɾ��������
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
   								//==�ڷǼ�¼���ֵ�ģʽ���������������,�ı䷢������==
			            if(KeyValue==12)
										{
											Ulsign=1;         //�������������ģ��
                      delayms(500);
										}
										
									switch(KeyValue)      //�޸Ľ���Ĭ��ֵ    
									  {
										  case 13:JiePai=0x01;break;//����
										  case 14:JiePai=0x02;break;//����
										  case 15:JiePai=0x03;break;//����
										  default:JiePai=0x02;break;
									  }
			            if(KeyValue==16)     //���Ŵ洢��AT24C02������
										{
											Coplay();
                      delayms(500);
										}										
										
									delayms(300);
                }
							}
						 else
							{		//==��¼����==
									if(KeyValue==9)      //�Ƿ�Ҫ��¼
										{
											REflag=!REflag;
											if(REflag)       //��¼,ȫ��
											{
													LcdShowStr(0,0,Null);  //���¼�¼,����
													delayms(10);
													LcdShowStr(0,1,Null);
													delayms(10);
													if(Offset>1)           //�������¼�¼,��ӡ���Ѽ�¼������
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
											else							//����,ȫ��
											{
												LED=0xff;
											}
											delayms(500);
										}
									//==���Ŵ洢�����������01==
									if(KeyValue==10)
										{
											Replay();
										}
									//==������������==//������ⲿ�洢��������
									if(KeyValue==11)
										{
											Erase();      //����λ��������������,��Ӱ���ⲿ�洢��������
										}
							}
	          }
       }
    }
}//end void
#endif