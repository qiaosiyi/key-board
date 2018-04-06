#include "sys.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "wakeup.h"
#include "adc.h"
#include "wkup.h"
#include "usart.h"
#include "usart3.h"	
#define OPENBLT 1

u8 KeyDown(void)
{	

	if(COL0 | COL1 | COL2 | COL3 | COL4 | COL5 | COL6 | COL7 )//判断是否有键按下
	{ 
		// u3_printf("KeyDown.\r\n");
		return 1;//keydown
	}
	else {
		return 0;
	}

}


void ScanKeyBoard(void){
	if(KeyDown())
	{
		delay_ms(10);

		if(KeyDown())
		{	
			u3_printf("KeyDown.\r\n");
			send_key();
			//u3_printf("\r\n\r\n");
		}
		else 
		{
			// u3_printf("nothing\r\n");

		}
	} 
	 
}



int main(void){
	u8 key=0;

	u32 waittime=0;
	u16 adcx=0;
	u32 times=0;
	u8 keymap_pre[128];
	u8 keymap_now[128];
	u8 * resault;
	u8 byte[7] = {0,0,0,0,0,0,0};
	float temp=0.0;

	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	uart_init(9600);
	usart3_init(115200);
	LED_Init();
	WKUP_Init();
	WAKEUP_BLT_Init();
	KEY_Init();
	Adc_Init();
	//low_power();
	LED0=1;
	if(OPENBLT){
		WAKEUP=1;//蓝牙开启
	}else{ 
		WAKEUP=0;//不开启蓝牙
	}
	setrow0_0();
	setrow16_1();
	while(1){
		if(KeyDown() && WAKEUP == 1){
			waittime = 0;//将等待时间清零
		}else if(KeyDown() && WAKEUP == 0){//如果有按键按下,而且蓝牙待机
			waittime = 0;//将等待时间清零
			if(OPENBLT){
				WAKEUP=1;//蓝牙开启
			}else{
				WAKEUP=0;//不开启蓝牙
			}	
			delay_ms(20);//等待蓝牙启动成功
		}else if(!KeyDown()){//如果没有按键被按下，等待时间加1个单位，1个单位按10ms计算
			if(WAKEUP==1){//如果蓝牙开机，才开始算等待时间
				waittime++;
			}
		}
		if(waittime%100==1){
			u3_printf("runing..\r\n");
			u3_printf("t=%d\r\n",waittime);
		}
		if(waittime>=60000){//X10ms进入休眠，如果进入休眠比蓝牙待机时间早，则蓝牙同时也会被待机
			u3_printf("Enter standby..\r\n");
			Sys_Enter_Standby();
		}
		send_key();
		delay_ms(10);
		//u3_printf("times:%d\r\n", times);

	}
}






		// if(key != 0 && WAKEUP == 1){//如果有按键按下,而且蓝牙正在工作
		// 	waittime = 0;//将等待时间清零
		// }else if(key != 0 && WAKEUP == 0){//如果有按键按下,而且蓝牙待机
		// 	waittime = 0;//将等待时间清零
		// 	if(OPENBLT){
		// 		WAKEUP=1;//蓝牙开启
		// 	}else{
		// 		WAKEUP=0;//不开启蓝牙
		// 	}
		// 	LED0=1;
		// 	delay_ms(20);//等待蓝牙启动成功
		// }else if(key == 0){//如果没有按键被按下，等待时间加1个单位，1个单位按10ms计算
		// 	if(WAKEUP==1){//如果蓝牙开机，才开始算等待时间
		// 		waittime++;
		// 	}
		// }

		// if(waittime>=30000){//如果等了超过300秒
		// 	WAKEUP=0;//关闭蓝牙
		// 	LED0=1;//
		// 	waittime=0;
		// }
		// if(key == KEY0_PRES){
		// 	delay_ms(400);
		// 	//key=KEY_Scan(1);
		// 	if(key==0){
		// 		password();
		// 		//TP_a2z();
		// 		// tmp();
		// 	}else{
		// 		mailbox();
		// 	}
		// 	delay_ms(1000);
		// 	times=0;
		// }

		// if(times%50==0){
		// 	u3_printf("runing..\r\n");
		// 	u3_printf("t=%d\r\n",times);
		// }
		// if(times%300==0){//每3秒检测一次电源电压
		// 	adcx=Get_Adc_Average(ADC_Channel_1,10);
		// 	temp=(float)adcx*(2*3.3/4096);//电源被分压一半，所以要乘以2
		// 	u3_printf("%.3f\r\n",temp);
		// }
		// if(times>=6000){//X10ms进入休眠，如果进入休眠比蓝牙待机时间早，则蓝牙同时也会被待机
		// 	u3_printf("Enter standby..\r\n");
		// 	Sys_Enter_Standby();
		// }
