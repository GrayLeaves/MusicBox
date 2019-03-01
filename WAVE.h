#ifndef __WAVE_h_
#define __WAVE_h_

#include "Global.h"
#include "lcd.h"
#include "Temp.h"

//---�ض���ؼ���---//
#ifndef uchar
#define uchar unsigned char
#endif
#ifndef uint 
#define uint unsigned int
#endif
#ifndef ulong
#define ulong unsigned long

//******************���������******************************
sbit Trig = P3^5;
sbit Echo = P3^4;			  
uint  tc = 0;
ulong S = 0;
int  Temp;                   //�¶�ֵ
bit flag=0;                  //�����Ƿ񳬹�6m
uchar Out[7];                //�¶ȺͲ������ʾ�������
//**********************************************************
void Tempros(void );              //�¶ȴ�����
void Count(void );                //�������
void Show(void );                 //��ʾ�¶�,�����
void StartModule(void ); 		      //����ģ��
void Ultrasonic(void );           //�������
/*******************************************************************************
* ���ģ�� : Ultrasonic ranging
*******************************************************************************/
//*************************�¶ȴ�����*********************************
void Tempros(void )
{
	 float tp;                  //��ʱ����
	 int  temp;             //�¶ȱ���
//***************************�¶Ƚ���*****************************************
	 temp=Ds18b20ReadTemp();
	 	//���ݴ�����С����,���¶ȸ���һ�������ͱ���
		//����¶���������ô,��ô������ԭ����ǲ���������
	 if(temp>=0)		 //���¶�ֵΪ�Ǹ���
	   {
			 tp=(float)temp;
		   Temp=tp*0.0625*100+0.5;	//ת��Ϊint�ͱ���
	   }
	 else
	   {   
	  //��Ϊ��ȡ���¶���ʵ���¶ȵĲ���,���Լ�1,��ȡ�����ԭ��
			 temp=temp-1;
			 temp=!temp;
			 tp=(float)temp;
			 Temp=tp*6.25+0.5;	
		//ע����ֵ���¶�ֵ��100��,���ھ�ȷ���ٷ�λ
  	 }
		//��ʾ��ʱ�¶�
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
	//**********************������봦��****************************************
		//��ʾ��ʱ�����������Χ
		 if(flag==1)
		 {
			 flag=0;
			 LcdShowStr(0,0,failstr);
		 }
		 else
		 {
			 S=tc*(331.45+0.61*Temp/100)/20000;     
/*���������CMΪ��λ������,���¶Ȳ�������,���������٣�v=331.45+0.61*Temp m/s */
			 Out[4]=S%1000/100;
			 Out[5]=S%1000%100/10;
			 Out[6]=S%1000%10 %10;		 		 
       if((S<200)&&Ulsign)     //�����2m��,�������򷢳���������
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
		LcdShowStr(0,0,tempstr); //��ʾ��ǰ�¶����
		LcdShowStr(0,1,lenstr);  //��ʾ��ǰ��������
		//��ʾ�¶�0
		DisplayOneChar( 8, 0, ASCII[Out[0]]);
		DisplayOneChar( 9, 0, ASCII[Out[1]]);	
		DisplayOneChar(11, 0, ASCII[Out[2]]);
		DisplayOneChar(12, 0, ASCII[Out[3]]);
		//��ʾ�����1
		DisplayOneChar( 9, 1, ASCII[Out[4]]);
		DisplayOneChar(11, 1, ASCII[Out[5]]);	
		DisplayOneChar(12, 1, ASCII[Out[6]]);
}
void StartModule(void ) 		         //����ģ��
{
	  Trig=1;			                     //����һ��ģ��
	  _nop_(); _nop_();  _nop_(); _nop_(); _nop_(); _nop_(); _nop_();
    _nop_(); _nop_();  _nop_(); _nop_(); _nop_(); _nop_(); _nop_();
	  _nop_(); _nop_();  _nop_(); _nop_(); _nop_(); _nop_(); _nop_();
	  Trig=0;
}

void Ultrasonic(void )
{
	 TH1=0;
	 TL1=0;          
	 flag=0;           //�������־
	 StartModule();
	 while(!Echo);		//��RXΪ��ʱ�ȴ�
	 TR1=1;			      //��������
	 while(Echo);			//��RXΪ1�������ȴ�
	 TR1=0;				    //�رռ���
	 if(TF1)          //�����жϵķ���
		 {
			 TF1=0;flag=1;//��������೬����Χ��־
		 }
	 Tempros();    //�����¶�ֵ
	 Count();			 //��������
	 Show();       //��ʾ�������
}

#endif