//Core program
#include <reg51.h>
#include "KeyTool.h"
#include "WAVE.h"
#include "Global.h"
/*******************************************************************************
* �� �� �� : main
* �������� : ������
*******************************************************************************/
int main(void )
{
	 Test();
	 while(1)
	 {
		 if(Ulsign){
			 	if(K2==0){      	//��Ŀ¼�µĹ���ת����
						delayms(10);
						if(K2==0){
							 while(!K2);
							 Ulsign=0;
						 }
						delayms(500);
			  }
  	    Ultrasonic();   	//���¶Ƚ����ĳ��������
		 }
		 else
		 {
		   Key4();           //�๦�����ֺ�
		   Matrix();         //�ɴ洢������
		 }
	 }
	 return 0;
}
