#ifndef __LCD_H_
#define __LCD_H_
/**********************************
当使用的是4位数据传输的时候定义，
使用8位取消这个定义
**********************************/
//#define LCD1602_4PINS
/**********************************
包含头文件
**********************************/
//---重定义关键词---//
#ifndef uchar
#define uchar unsigned char
#endif
#ifndef uint 
#define uint unsigned int
#endif
#ifndef ulong
#define ulong unsigned long
/**********************************
PIN口定义
**********************************/
#define LCD1602_DB  P0      //data bus 数据总线
sbit LCD1602_RS = P2^5;
sbit LCD1602_RW = P2^6;
sbit LCD1602_EN = P2^7;	 
/**********************************
*显示声明
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
*函数声明
**************************************************************************/
/*在51单片机12MHZ时钟下的延时函数*/
void Read_Busy(void );                             //忙检测函数，判断bit7是0,允许执行；1禁止
void InitLcd1602(void );                           //1602初始化
void Lcd1602_Write_Cmd(uchar cmd);                 //写命令
void Lcd1602_Write_Data(uchar dat);                //写数据
void LcdSetCursor(uchar x,uchar y);                //坐标显示
void LcdShowStr(uchar x,uchar y,uchar *str);       //显示字符串
void DisplayOneChar(uchar X, uchar Y, uchar DData);
//***************************************************************************
void Read_Busy(void )           //忙检测函数，判断bit7是0，允许执行；1禁止
{
	unsigned char sta;      //
	LCD1602_DB = 0xff;
	LCD1602_RS = 0;
	LCD1602_RW = 1;
	do
	{
			LCD1602_EN = 1;
			sta = LCD1602_DB;
			LCD1602_EN = 0;    //使能，用完就拉低，释放总线
	}while(sta & 0x80);
}

void Lcd1602_Write_Cmd(uchar cmd)     //写命令
{
	Read_Busy();
	LCD1602_RS = 0;
	LCD1602_RW = 0;
	LCD1602_DB = cmd;
	LCD1602_EN = 1;
	LCD1602_EN = 0;    
}

void Lcd1602_Write_Data(uchar dat)   //写数据
{
	Read_Busy();
	LCD1602_RS = 1;
	LCD1602_RW = 0;
	LCD1602_DB = dat;
	LCD1602_EN = 1;
	LCD1602_EN = 0;
}

void LcdSetCursor(uchar x,uchar y)  //坐标显示
{
	unsigned char addr;
	if(y == 0)
			addr = 0x00 + x;
	else
			addr = 0x40 + x;

	Lcd1602_Write_Cmd(addr|0x80);
}

//按指定位置显示一个字符
void DisplayOneChar(uchar X, uchar Y, uchar DData)
{
	Y &= 0x1;
	X &= 0xF; //限制X不能大于15，Y不能大于1
	if (Y) X |= 0x40; //当要显示第二行时地址码+0x40;
	X |= 0x80; //算出指令码
	Lcd1602_Write_Cmd(X); //发命令字
	Lcd1602_Write_Data(DData); //发数据
}

void LcdShowStr(uchar x,uchar y,uchar *str)     //显示字符串
{
	LcdSetCursor(x,y);      //当前字符的坐标
	while(*str != '\0')
	 {
	  Lcd1602_Write_Data(*str++);
	 }
}

void InitLcd1602(void )              //1602初始化
{
	Lcd1602_Write_Cmd(0x38);    //打开，5*8,8位数据
	Lcd1602_Write_Cmd(0x0c);
	Lcd1602_Write_Cmd(0x06);
	Lcd1602_Write_Cmd(0x01);    //清屏   
}
#endif
