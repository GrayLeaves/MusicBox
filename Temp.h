#ifndef __Temp_h_
#define __Temp_h_

#include <intrins.h>
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

//--����ʹ�õ�IO��--//
sbit DSPORT=P3^6;

//--����ȫ�ֺ���--//
uchar Ds18b20Init();
void  Ds18b20WriteByte(uchar com);
uchar Ds18b20ReadByte();
void  Ds18b20ChangTemp();
void  Ds18b20ReadTempCom();
int   Ds18b20ReadTemp();
/*******************************************************************************
* �� �� ��      : Ds18b20Init
* ��������		  : ��ʼ��
* ��    ��      : �ɹ�����1,ʧ�ܷ���0
*******************************************************************************/
uchar Ds18b20Init()
{
	DSPORT = 0;			 //����������480us~960us
	i = 70;	
	while(i--);     //��ʱ642us
	DSPORT = 1;			//Ȼ����������,���DS18B20������Ӧ�Ὣ��15us~60us����������
	i = 0;
	while(DSPORT)	  //�ȴ�DS18B20��������
	{
		delayms(1);
		i++;
		if(i>5)       //�ȴ�>5MS
		{
			return 0;   //��ʼ��ʧ��
		}
	}
	return 1;       //��ʼ���ɹ�
}
/*******************************************************************************
* �� �� ��    : Ds18b20WriteByte
* ��������    : ��18B20д��һ���ֽ�
*******************************************************************************/
void Ds18b20WriteByte(uchar dat)
{
	for(j=0; j<8; j++)
	{
		DSPORT = 0;	     	    //ÿд��һλ����֮ǰ�Ȱ���������1us
		i++;
		DSPORT = dat & 0x01;  //Ȼ��д��һ������,�����λ��ʼ
		i=6;
		while(i--); //��ʱ68us,����ʱ������60us
		DSPORT = 1;	//Ȼ���ͷ�����,����1us�����߻ָ�ʱ����ܽ���д��ڶ�����ֵ
		dat >>= 1;
	}
}
/*******************************************************************************
* �� �� ��    : Ds18b20ReadByte
* ��������		: ��ȡһ���ֽ�
*******************************************************************************/
uchar Ds18b20ReadByte()
{
	uchar byte, bi;
	i=0;
	for(j=8; j>0; j--)
	{
		DSPORT = 0;//�Ƚ���������1us
		i++;
		DSPORT = 1;//Ȼ���ͷ�����
		i++;
		i++;       //��ʱ6us�ȴ������ȶ�
		bi = DSPORT;	 //��ȡ���ݣ������λ��ʼ��ȡ
		/*��byte����һλ��Ȼ����������7λ���bi��ע���ƶ�֮���Ƶ���λ��0*/
		byte = (byte >> 1) | (bi << 7);						  
		i = 4;		//��ȡ��֮��ȴ�48us�ٽ��Ŷ�ȡ��һ����
		while(i--);
	}				
	return byte;
}
/*******************************************************************************
* �� �� ��   : Ds18b20ChangTemp
* ��������   : ��18b20��ʼת���¶�
*******************************************************************************/
void  Ds18b20ChangTemp()
{
	Ds18b20Init();
	delayms(1);
	Ds18b20WriteByte(0xcc);		//����ROM��������		 
	Ds18b20WriteByte(0x44);	    //�¶�ת������
}
/*******************************************************************************
* �� �� ��  : Ds18b20ReadTempCom
* ��������	: ���Ͷ�ȡ�¶�����
*******************************************************************************/
void  Ds18b20ReadTempCom()
{	
	Ds18b20Init();
	delayms(1);
	Ds18b20WriteByte(0xcc);	 //����ROM��������
	Ds18b20WriteByte(0xbe);	 //���Ͷ�ȡ�¶�����
}
/*******************************************************************************
* �� �� ��  : Ds18b20ReadTemp
* ��������  : ��ȡ�¶�
*******************************************************************************/
int Ds18b20ReadTemp()
{
	int temp = 0;
	uchar tmh, tml;
	Ds18b20ChangTemp();			 	//��д��ת������
	Ds18b20ReadTempCom();			//Ȼ��ȴ�ת������Ͷ�ȡ�¶�����
	tml = Ds18b20ReadByte();		//��ȡ�¶�ֵ��16λ���ȶ����ֽ�
	tmh = Ds18b20ReadByte();		//�ٶ����ֽ�
	temp = tmh;
	temp <<= 8;
	temp |= tml;
	return temp;
}
#endif