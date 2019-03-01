#ifndef __ISD_h__
#define __ISD_h__

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

sbit ISD_SS   = P1^0;              	//ISD4002Ƭѡ
sbit ISD_SCLK = P1^1;              	//ISD4002ʱ��
sbit ISD_MOSI = P1^2;              	//ISD4002��������
sbit ISD_MISO = P1^3;              	//ISD4002�������
sbit ISD_INT  =	P3^3;             	//ISD4002�������������ж�
sbit PR   =  P1^4;              	  //PR=1¼��   PR=0����
sbit RUN  =  P1^5;              	  //ִ��¼�������
sbit STOP =  P1^6;               	  //��λ
sbit LED  =  P1^7;              	  //ָʾ��
//**************  ��������   ********************************************* 
void ISD_WRITE(uchar isdx); 			  //SPI���з���һ���ֽ��ӳ���
void ISD_STOP(void);					      //����stopָ�����
void ISD_PU(void);						      //�����ϵ�ָ�����
void ISD_PD(void);					      	//����ʡ��ָ�����
void ISD_PLAY(void);					      //����playָ�����
void ISD_REC(void);						      //����recָ�����
void ISD_SETPLAY(uchar adl,uchar adh);	//����setplayָ�����
void ISD_SETREC(uchar adl,uchar adh);	  //����setrecָ�����
uchar READ_ISD_OVF(void);				    //�������־OVF����
void ISD_OVF(void);					      	//LED��˸����
//************************************************************************
//                            ������
//���ܣ�1.¼��ʱ,��סRUN��,LED������ʼ¼��,�ɿ�RUN����ֹͣ¼��
//        �ٴΰ���RUN��,LED����,��ʼ¼�ڶ�����,��������,ֱ��оƬ�����
//      2.����ʱ,��һ��RUN��,������һ������,��stop��оƬ��λ��
//************************************************************************
void ISDpro(void)
{
	P1=0xff;  					//��ʼ��
	while(Isd)
	{	
		while(RUN==1)     				//�ȴ�RUN������
		{
			if(RUN==0)              //����������
			{
				delayms(20);
			}
		}
		ISD_PU();        				//RUN������Ϊ0��ISD�ϵ粢�ӳ�50ms
		ISD_PD();       				//ֹͣ���β���������ʡ��״̬
		ISD_PU();						    //SD�ϵ粢�ӳ�50ms
		if(PR==1)     					//���PR=1��ת��¼������
		{
			delayms(500);     		//�ӳ�¼�� ָ����ַ¼��
			ISD_SETREC(0x00,0x00);//����0x0000h��ַ��setplayָ�� 
			do
			{ 
				ISD_REC();    			//����recָ�� ��һ����ַ��ʼ¼��
				while(RUN==0)    		//��δ�ɿ� �ȴ�¼����� 
				{
					if (ISD_INT==0)   //���оƬ���
					ISD_OVF();    		//����LED��˸��ʾ
				}
				if(ISD_INT==0)break;//¼�����������RUN�ͷ� ���˳�¼������оƬ��λ
				LED=1;         			//�����ͷţ�δ����¼����ϣ�LEDϨ��
				ISD_STOP();     		//����ֹͣ����
				while(RUN==1)       //���RUNδ���� ��ѯ RUN��STOP��
				{
					if(STOP==0)    		//�������STOP��������оƬ��λ
					break;
					if (RUN==0)			  //���RUN�ٴΰ��£���ʼ¼����һ������
					delayms(500);
				}
			}while(RUN==0);
		}
		else             				//���PR==0��ת���������
		{
			while(RUN==0);			//δ�ͷŰ���ʱ�ȴ�
			ISD_SETPLAY(0x00,0x00);     //����setplayָ���0x0000��ַ��ʼ����
			do
			{ 
				ISD_PLAY();               //���ͷ���ָ��
				delayus(20);
				while(ISD_INT==1);        //�ȴ�������ϵ�EOM�ж��ź�
				LED=1;					          //LEDϨ��
				ISD_STOP();         	    //������ϣ�����stopָ��
				if (READ_ISD_OVF()==1)   	//���оƬ�Ƿ���� ���������ֹͣ������оƬ��λ
				break;
				while(RUN==1)     		    //�ȴ�RUN���ٴΰ���
				{
					if (STOP==0)  		      //�������STOP��������оƬ��λ
					break;
					if(RUN==0)			        //���RUN�ٴΰ��£���ʼ������һ������
					delayms(20);
				}
			}while(RUN==0);     		    //RUN���ٴΰ��£�������һ������
		}
		ISD_STOP();       	          //����stopָ��
		ISD_PD();						          //ֹͣ���β���������ʡ��״̬
	}
}
//************************************************************************
//ISD4002 SPI���з���һ���ֽ��ӳ���8λ����
//************************************************************************
void ISD_WRITE(uchar isdx) 	//SPI���з���һ���ֽ��ӳ���
{   
	ISD_SS=0;     						//Ƭѡʹ��,��SPIͨ�Ŷ�
	ISD_SCLK=0;							  //ʱ����0
	for(i=0;i<8;i++) 					//8λ���η���
	{ 
		if ((isdx&0x01)==1)			//ȡ���λ��Ϊ1����������1
		{
			ISD_MOSI=1;
		}
		else							    //Ϊ0����������0
		{
			ISD_MOSI=0;
		}		
		isdx=isdx>>1;					//�ȷ���λ�ٷ���λ�����η���
		ISD_SCLK=1;						//ʱ�Ӳ��������� �������������
		delayus(2);						  //��ʱʹ�����ȶ�
		ISD_SCLK=0;						//�ظ�ʱ�ӵ͵�ƽ
		delayus(2);
	}
}
//***********************************************************************
//����stopָ��
//***********************************************************************
void ISD_STOP(void)					//����stopָ�����
{   
	delayus(10);
	ISD_WRITE(0x30);					//ֹͣ¼�������������ָ�����0X11 0
	ISD_SS=1;
	delayms(50);
}
//***********************************************************************
//�����ϵ�ָ����ӳ�50ms
//***********************************************************************
void ISD_PU(void)						//�����ϵ�ָ�����
{  
	delayus(10);
	ISD_SS=0;
	ISD_WRITE(0x20);					//�����ϵ磬ָ�����0010 0
	ISD_SS=1;
	delayms(50);
}
//**********************************************************************
//����ʡ��ָ����ӳ�50ms
//************************************************************************
void ISD_PD(void)						//����ʡ��ָ�����
{ 
	delayus(10);
  ISD_WRITE(0x10);				//ֹͣ���β���������ʡ��״̬��ָ�����0X01 X
	ISD_SS=1;
	delayms(50);
}
//************************************************************************
//����playָ��
//**************************************************************************
void ISD_PLAY(void)					//����playָ�����
{
	LED=0;								    //ָʾ����
	ISD_WRITE(0xf0);					//����һ����ַ��ʼ������ָ�����1111 1/0 
	ISD_SS=1;
}
//*************************************************************************
//����recָ��
//*************************************************************************
void ISD_REC(void)					//����recָ�����
{
	LED=0;								    //ָʾ����
	ISD_WRITE(0xb0);					//����һ����ַ��ʼ¼��
	ISD_SS=1;
}
//**************************************************************************
//����setplayָ��
//**************************************************************************
void ISD_SETPLAY(uchar adl,uchar adh)  	//����setplayָ�����
{
	ISD_WRITE(adl);    					//���ͷ�����ʼ��ַ��λ
	adh=adh||0xe0;    					//�ӵ�ַ<A9~A0>��ʼ����
	ISD_WRITE(adh);   					//���ͷ�����ʼ��ַ��λ
	ISD_SS=1;
}
//****************************************************************************
//����setrecָ��
//****************************************************************************
void ISD_SETREC(uchar adl,uchar adh)	//����setrecָ�����
{
	ISD_WRITE(adl);    					//����¼����ʼ��ַ��λ
	adh=adh||0xa0;   					  //�ӵ�ַ<A9~A0>��ʼ¼��
	ISD_WRITE(adh);    					//����¼����ʼ��ַ��λ
	ISD_SS=1;
}
//*****************************************************************************
//���оƬ�Ƿ����(��OVF,������OVFֵ)
//*****************************************************************************
uchar READ_ISD_OVF(void)		//�������־OVF����
{
	ISD_SS=0; 							  //Ƭѡ��Ч
	delayus(2);
	ISD_SCLK=0;							  //�Ȳ���ʱ��������
	delayus(2);
	ISD_SCLK=1;
	ISD_SCLK=0;   						//ʱ���ź��½���ʱ�������
	delayus(2);
	if (ISD_MISO==1) 					//ֻ����1λ����
	{ 
		ISD_SS =1;     					//�ر�SPIͨ�Ŷ�
		ISD_STOP();     				//����stopָ��
		return 1;     					//OVFΪ1������1
	}
	else
	{ 
		ISD_SS =1;         			//�ر�SPIͨ�Ŷ�
		ISD_STOP();     				//����stopָ��
		return 0;     					//OVFΪ0������0
	}
}
//***************************************************************************
//оƬ�����LED��˸����ֹͣ¼��
//***************************************************************************
void ISD_OVF(void)					//LED��˸����
{
	while(RUN==0)						  //Ϊ0 ��ʾ���ڰ��� ��δ�ͷ� 
	{
		LED=1;   						    //ָʾ����
		delayms(300);					  //��ʱָʾ����˸
		LED=0;   						    //ָʾ����
		delayms(300);
	}
}
#endif