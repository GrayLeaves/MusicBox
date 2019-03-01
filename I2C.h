#ifndef __I2C_h__
#define __I2C_h__

#include <intrins.h>	
#include "Global.h"

#define  PCF8591 0x90    //PCF8591 ��ַ
//**********************************************************************
uchar AD_which=0;      //AD/DAѡͨ
uint AD[5];
uchar Out[8];
//*****************************��������************************************
void Start_I2c(void );    //�������ߺ���  
void Stop_I2c(void );     //�������ߺ��� 
void Ack_I2c(bit a);      //Ӧ���Ӻ���
void  SendByte(uchar c);  //�ֽ����ݷ��ͺ���
uchar RcvByte(void );     //�ֽ����ݽ��պ��� 
bit DACconversion(uchar sla,uchar c,uchar Val);//DAC�任/ת������ 
bit ISendByte(uchar sla,uchar c);//ADC�����ֽ�[����]���ݺ���
uchar IRcvByte(uchar sla);//ADC���ֽ����ݺ��� 
//void Init_timer2(void );  //��ʱ��2��ʼ��
/*************************�˲���ΪI2C���ߵ���������***********************/
sbit SCL=P2^0;       //I2C  ʱ�� 
sbit SDA=P2^1;       //I2C  ���� 
bit ack;             /*Ӧ���־λ*/
//**********************��AD/DAת����������******************************/
void ADpro(void )
{  
// Init_timer2();
  while(IsAD)
   {
/********************����AD-DA����***************************************/  
		 switch(AD_which)
			{
				case 0: ISendByte(PCF8591,0x41);
								AD[0]=IRcvByte(PCF8591)*2;   //ADC0ģ��ת��1
								break;  
				case 1: ISendByte(PCF8591,0x42);
								AD[1]=IRcvByte(PCF8591)*2;   //ADC1ģ��ת��2
								break;  
				case 2: ISendByte(PCF8591,0x43);
								AD[2]=IRcvByte(PCF8591)*2;   //ADC2ģ��ת��3
								break;  
				case 3: ISendByte(PCF8591,0x40);
								AD[3]=IRcvByte(PCF8591)*2;   //ADC3ģ��ת��4
								break;  

				case 4: DACconversion(PCF8591,0x40,AD[4]/2);    //DAC��ģת��
								break;
			}
			AD[4]=500;//����--->>ģ����� ���4.56V  		  
			if(AD_which==4)AD_which=0;
			//�¶�
			Out[0]=AD[0]%10000/1000;
			Out[1]=AD[0]%1000/100;
			Out[2]=AD[0]%100/10;
			Out[3]=AD[0]%10;
			//����
			Out[4]=AD[1]%10000/1000;
			Out[5]=AD[1]%1000/100;
			Out[6]=AD[1]%100/10;
			Out[7]=AD[1]%10;
			for(k=1;k<8;k++)
			 {
	       Disp(k,Out[k]);//��1λ�������ʾ	
         delayms(100);				 
			 }
  }  
}
/*******************************************************************
                     �����ߺ���               
����ԭ��: void  Start_I2c();  
����:     ����I2C����,������I2C��ʼ����.  
********************************************************************/
void Start_I2c(void )
{
	SDA=1;         /*������ʼ�����������ź�*/
	_nop_();
	SCL=1;
	_nop_();        /*��ʼ��������ʱ�����4.7us,��ʱ*/
	_nop_();
	_nop_();
	_nop_();
	_nop_();    
	SDA=0;         /*������ʼ�ź�*/
	_nop_();        /* ��ʼ��������ʱ�����4��s*/
	_nop_();
	_nop_();
	_nop_();
	_nop_();      
	SCL=0;       /*ǯסI2C���ߣ�׼�����ͻ�������� */
	_nop_();
	_nop_();
}
/*******************************************************************
                      �������ߺ���               
����ԭ��: void  Stop_I2c();  
����:     ����I2C����,������I2C��������.  
********************************************************************/
void Stop_I2c(void )
{
	SDA=0;      /*���ͽ��������������ź�*/
	_nop_();       /*���ͽ���������ʱ���ź�*/
	SCL=1;      /*������������ʱ�����4��s*/
	_nop_();
	_nop_();
	_nop_();
	_nop_(); 
	_nop_();
	SDA=1;      /*����I2C���߽����ź�*/
	_nop_();
	_nop_();
	_nop_();
	_nop_(); 
}
/*******************************************************************
                 �ֽ����ݷ��ͺ���               
����ԭ��: void  SendByte(UCHAR c);
����:     ������c���ͳ�ȥ,�����ǵ�ַ,Ҳ����������,�����ȴ�Ӧ��,����
          ��״̬λ���в���.(��Ӧ����Ӧ��ʹack=0)     
           ��������������ack=1; ack=0��ʾ��������Ӧ����𻵡�
********************************************************************/
void SendByte(uchar c)
{
   uchar  BitCnt;

   for(BitCnt=0;BitCnt<8;BitCnt++)  /*Ҫ���͵����ݳ���Ϊ8λ*/
   {
	 if((c<<BitCnt)&0x80)SDA=1;   /*�жϷ���λ*/
     else  SDA=0;                
	_nop_();
	SCL=1;               /*��ʱ����Ϊ�ߣ�֪ͨ��������ʼ��������λ*/
	_nop_(); 
	_nop_();             /*��֤ʱ�Ӹߵ�ƽ���ڴ���4��s*/
	_nop_();
	_nop_();
	_nop_();
	 SCL=0; 
   }

	_nop_();
	_nop_();
	SDA=1;                /*8λ��������ͷ������ߣ�׼������Ӧ��λ*/
	_nop_();
	_nop_();   
	SCL=1;
	_nop_();
	_nop_();
	_nop_();
	if(SDA==1)ack=0;     
	   else ack=1;        /*�ж��Ƿ���յ�Ӧ���ź�*/
	SCL=0;
	_nop_();
	_nop_();
}
/*******************************************************************
                 �ֽ����ݽ��պ���               
����ԭ��: UCHAR  RcvByte();
����:        �������մ���������������,���ж����ߴ���(����Ӧ���ź�)��
          ���������Ӧ����Ӧ��ӻ���  
********************************************************************/    
uchar RcvByte(void )
{
	uchar  retc;
	uchar  BitCnt;

	retc=0; 
	SDA=1;                     /*��������Ϊ���뷽ʽ*/
	for(BitCnt=0;BitCnt<8;BitCnt++)
	{
	_nop_();           
	SCL=0;                  /*��ʱ����Ϊ�ͣ�׼����������λ*/
	_nop_();
	_nop_();                 /*ʱ�ӵ͵�ƽ���ڴ���4.7��s*/
	_nop_();
	_nop_();
	_nop_();
	SCL=1;                  /*��ʱ����Ϊ��ʹ��������������Ч*/
	_nop_();
	_nop_();
	retc=retc<<1;
	if(SDA==1)retc=retc+1;  /*������λ,���յ�����λ����retc�� */
	_nop_();
	_nop_(); 
	}
	SCL=0;    
	_nop_();
	_nop_();
	return(retc);
}
/********************************************************************
                     Ӧ���Ӻ���
����ԭ��:  void Ack_I2c(bit a);
����:      ����������Ӧ���ź�(������Ӧ����Ӧ���źţ���λ����a����)
********************************************************************/
void Ack_I2c(bit a)
{
	if(a==0)SDA=0;              /*�ڴ˷���Ӧ����Ӧ���ź� */
	else SDA=1;
	_nop_();
	_nop_();
	_nop_();      
	SCL=1;
	_nop_();
	_nop_();                   /*ʱ�ӵ͵�ƽ���ڴ���4��s*/
	_nop_();
	_nop_();
	_nop_();  
	SCL=0;                     /*��ʱ���ߣ�ǯסI2C�����Ա��������*/
	_nop_();
	_nop_();    
}
/*******************************************************************
DAC �任, ת������               
*******************************************************************/
bit DACconversion(uchar sla,uchar c,uchar Val)
{
   Start_I2c();              //��������
   SendByte(sla);            //����������ַ
   if(ack==0)return(0);
   SendByte(c);              //���Ϳ����ֽ�
   if(ack==0)return(0);
   SendByte(Val);            //����DAC����ֵ  
   if(ack==0)return(0);
   Stop_I2c();               //��������
   return(1);
}
/*******************************************************************
ADC�����ֽ�[����]���ݺ���               
*******************************************************************/
bit ISendByte(uchar sla,uchar c)
{
   Start_I2c();              //��������
   SendByte(sla);            //����������ַ
   if(ack==0)return(0);
   SendByte(c);              //��������
   if(ack==0)return(0);
   Stop_I2c();               //��������
   return(1);
}
/*******************************************************************
ADC���ֽ����ݺ���               
*******************************************************************/
uchar IRcvByte(uchar sla)
{  
	 uchar c;
   Start_I2c();          //��������
   SendByte(sla+1);      //����������ַ
   if(ack==0)return(0);
   c=RcvByte();          //��ȡ����0
   Ack_I2c(1);           //���ͷǾʹ�λ
   Stop_I2c();           //��������
   return(c);
}
/*//��ʱ��2��ʼ��
void Init_timer2(void )
{
	 RCAP2H=(65536-15000)/256;//��T2��ʼֵ0x0bdc�����ʱ��Ϊ50ms
	 RCAP2L=(65536-15000)%256;
	 TR2=1;	     //������ʱ��2
	 ET2=1;		 //�򿪶�ʱ��2�ж�
	 EA=1;		 //�����ж�
}
//��ʱ���жϺ���
void Timer2(void ) interrupt 5	  //��ʱ��2��5���ж�
{	
	 TF2=0;
}*/
#endif