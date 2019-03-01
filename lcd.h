#ifndef __LCD_H_
#define __LCD_H_
/**********************************
��ʹ�õ���4λ���ݴ����ʱ���壬
ʹ��8λȡ���������
**********************************/
//#define LCD1602_4PINS
/**********************************
����ͷ�ļ�
**********************************/
//---�ض���ؼ���---//
#ifndef uchar
#define uchar unsigned char
#endif
#ifndef uint 
#define uint unsigned int
#endif
#ifndef ulong
#define ulong unsigned long
/**********************************
PIN�ڶ���
**********************************/
#define LCD1602_DB  P0      //data bus ��������
sbit LCD1602_RS = P2^5;
sbit LCD1602_RW = P2^6;
sbit LCD1602_EN = P2^7;	 
/**********************************
*��ʾ����
***********************************/
uchar code tempstr[]=" Temp is:--.--C ";
uchar code lenstr[] =" Dist is:0-.--M ";
uchar code failstr[]="  Out-Of-Range  ";

uchar code welcome[]="  My pleasure!  ";
uchar code piano[]  ="Electronic organ";
uchar code leave[]  ="Quit See u later";
uchar code udstr  []="U:      D:      ";
uchar code Playing[]="N:      Playing.";
uchar code Pause[]  ="N:      Pause...";
uchar code Null[]   ="                ";
uchar code ASCII[]={'0','1','2','3','4','5','6','7', '8','9','A','B','C','D','E','F'};
uchar code *YF[]={ "  ",
									 "do","re","mi","fa","so","la","si",
									 "Do","Re","Mi","Fa","So","La","Si",
									 "DO","RE","MI","FA","SO","LA","SI",
                 };
/**************************************************************************
*��������
**************************************************************************/
/*��51��Ƭ��12MHZʱ���µ���ʱ����*/
void Read_Busy(void );                             //æ��⺯�����ж�bit7��0,����ִ�У�1��ֹ
void InitLcd1602(void );                           //1602��ʼ��
void Lcd1602_Write_Cmd(uchar cmd);                 //д����
void Lcd1602_Write_Data(uchar dat);                //д����
void LcdSetCursor(uchar x,uchar y);                //������ʾ
void LcdShowStr(uchar x,uchar y,uchar *str);       //��ʾ�ַ���
void DisplayOneChar(uchar X, uchar Y, uchar DData);
//***************************************************************************
void Read_Busy(void )           //æ��⺯�����ж�bit7��0������ִ�У�1��ֹ
{
	unsigned char sta;      //
	LCD1602_DB = 0xff;
	LCD1602_RS = 0;
	LCD1602_RW = 1;
	do
	{
			LCD1602_EN = 1;
			sta = LCD1602_DB;
			LCD1602_EN = 0;    //ʹ�ܣ���������ͣ��ͷ�����
	}while(sta & 0x80);
}

void Lcd1602_Write_Cmd(uchar cmd)     //д����
{
	Read_Busy();
	LCD1602_RS = 0;
	LCD1602_RW = 0;
	LCD1602_DB = cmd;
	LCD1602_EN = 1;
	LCD1602_EN = 0;    
}

void Lcd1602_Write_Data(uchar dat)   //д����
{
	Read_Busy();
	LCD1602_RS = 1;
	LCD1602_RW = 0;
	LCD1602_DB = dat;
	LCD1602_EN = 1;
	LCD1602_EN = 0;
}

void LcdSetCursor(uchar x,uchar y)  //������ʾ
{
	unsigned char addr;
	if(y == 0)
			addr = 0x00 + x;
	else
			addr = 0x40 + x;

	Lcd1602_Write_Cmd(addr|0x80);
}

//��ָ��λ����ʾһ���ַ�
void DisplayOneChar(uchar X, uchar Y, uchar DData)
{
	Y &= 0x1;
	X &= 0xF; //����X���ܴ���15��Y���ܴ���1
	if (Y) X |= 0x40; //��Ҫ��ʾ�ڶ���ʱ��ַ��+0x40;
	X |= 0x80; //���ָ����
	Lcd1602_Write_Cmd(X); //��������
	Lcd1602_Write_Data(DData); //������
}

void LcdShowStr(uchar x,uchar y,uchar *str)     //��ʾ�ַ���
{
	LcdSetCursor(x,y);      //��ǰ�ַ�������
	while(*str != '\0')
	 {
	  Lcd1602_Write_Data(*str++);
	 }
}

void InitLcd1602(void )              //1602��ʼ��
{
	Lcd1602_Write_Cmd(0x38);    //�򿪣�5*8,8λ����
	Lcd1602_Write_Cmd(0x0c);
	Lcd1602_Write_Cmd(0x06);
	Lcd1602_Write_Cmd(0x01);    //����   
}
#endif
