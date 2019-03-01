//Core program
#include <reg51.h>
#include "KeyTool.h"
#include "WAVE.h"
#include "Global.h"
/*******************************************************************************
* 函 数 名 : main
* 函数功能 : 主函数
*******************************************************************************/
int main(void )
{
	 Test();
	 while(1)
	 {
		 if(Ulsign){
			 	if(K2==0){      	//主目录下的功能转换键
						delayms(10);
						if(K2==0){
							 while(!K2);
							 Ulsign=0;
						 }
						delayms(500);
			  }
  	    Ultrasonic();   	//含温度矫正的超声波测距
		 }
		 else
		 {
		   Key4();           //多功能音乐盒
		   Matrix();         //可存储电子琴
		 }
	 }
	 return 0;
}
