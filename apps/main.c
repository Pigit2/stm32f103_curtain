#include "stm32f10x.h"
#include "usart.h"
#include "stdio.h"
#include "string.h"
#include "adc.h"
#include "dht.h"
#include "sg90.h"
#include "OLED.h"
#include "delay.h"
#include "bmp.h"


#define LIGHT 1
u8 t,h;
u8 envi = 1;
u8 mode = 1;
u8 swi = 9;
u8 uad = 0;
u8 at ;
u8 Tab1 = 9;
u8 Tab2 = 1;
u8 Tab3 = 4;
u8 Tab5 = 1;
u8 sw = 0;
u8 Tabud;
u8 LZZ;
char data[2][30];	
float Light_val[2];
int it;
char Temp[30];
char Humi[30];
char Time[30];


int main(void)
{
	OLED_Clear();
	delay_init();			
	OLED_Init();
	USART1_GPIO_INIT();
	USART1_INIT();
	USART1_IT_INIT();
	USART2_GPIO_INIT();
	USART2_INIT();
	WIFI_LOGIN();
	Light_AIR_ADC1_INIT();
	PA8_INIT();
	Motor_Init();	//电机初始化
  while (1)
  {
		int L=ADC_AIR_LIGHT_VAL(LIGHT);
		int tim = uad*5;
		swi = 9;
		DHT_READ_DATA(&t,&h);
		sprintf(Temp,"%d",t);
		sprintf(Humi,"%d",h);
		
		//光环境检测
		if(250<L&&L<=330)
		  {
		  	LZZ = 1;//夜晚
		  }
		  if(160<L&&L<=250)
		  {
		  	LZZ = 2;//日落
	  	}
	  	if(70<L&&L<=160)
	  	{
		  	LZZ = 3;//晴天
		  }
		  if(0<=L&&L<=70)
		  {
			  LZZ = 4;//晴朗
	  	}
			
		
		//定时开关计时
		if(mode == 3&&Tab1 == 2)
		{
			if(it<=tim)
			{
			  it++;
		    tim=tim-it;
			  delay_ms(1000);
				OLED_Clear();
			}
			if(tim==0)
			{
			  Tab1=9;
				uad=0;
				it=0;
				if(sw==0)
				{
				  printf("dingshion\n");
					TIM_SetCompare1(TIM8,17);//2.5ms --> 反向转速
	        SysTick_TIMES(50);
		      TIM_SetCompare1(TIM8,15);//1.5ms --> 速度为0
					sw++;
				}
			}
		}
		if(mode == 3&&Tab1 == 1)
		{
			if(it<=tim)
			{
			  it++;
		    tim=tim-it;
			  delay_ms(1000);
				OLED_Clear();
			}
			if(tim==0)
			{
			  Tab1=9;
				uad=0;
				it=0;
				if(sw==1)
				{
				  printf("dingshioff\n");
					TIM_SetCompare1(TIM8,13);//0.5ms --> 正向转速
	        SysTick_TIMES(50);
		      TIM_SetCompare1(TIM8,15);//1.5ms --> 速度为0
					sw--;
				}
			}
		}
		sprintf(Time,"%d",tim);
		
		
		
		//
		if(mode == 2)
		{
			//设置打开的所有情况
		  if(Tabud == 1&&Tab2 == 1)
			{
			  Tab3 = 1;//夜晚打开
			}
			if(Tabud == 1&&Tab2 == 2)
			{
			  Tab3 = 2;//日落打开
			}
			if(Tabud == 1&&Tab2 == 3)
			{
			  Tab3 = 3;//晴天打开
			}
			if(Tabud == 1&&Tab2 == 4)
			{
			  Tab3 = 4;//晴朗打开
			}
			//设置关闭的所有情况
			if(Tabud == 2&&Tab2 == 1)
			{
			  Tab5 = 1;//夜晚关闭
			}
			if(Tabud == 2&&Tab2 == 2)
			{
			  Tab5 = 2;//日落关闭
			}
			if(Tabud == 2&&Tab2 ==3 )
			{
			  Tab5 = 3;//晴天关闭
			}
			if(Tabud == 2&&Tab2 ==4 )
			{
			  Tab5 = 4;//晴朗关闭
			}
			
			//
			if(Tab3 == LZZ)
			{
			  at = 1;
			}
			if(Tab5 == LZZ)
			{
				at = 2;
			}
		}
		
		
		if(at == 1)
		{
			if(sw == 0)
			{
		    printf("zidongon\n");
				TIM_SetCompare1(TIM8,17);//2.5ms --> 反向转速
	      SysTick_TIMES(50);
		    TIM_SetCompare1(TIM8,15);//1.5ms --> 速度为0
				sw++;
			}
			at = 9;
		}
		if(at == 2)
		{
			if(sw == 1)
			{
		    printf("zidongoff\n");
				TIM_SetCompare1(TIM8,13);//0.5ms --> 正向转速
	      SysTick_TIMES(50);
		    TIM_SetCompare1(TIM8,15);//1.5ms --> 速度为0
				sw--;
			}
			at = 9;
		}
		
		
//		printf("T=%d H=%d\n",t,h);
//		printf("air:%.2f  light:%.2f\n",ADC_AIR_LIGHT_VAL(AIR),ADC_AIR_LIGHT_VAL(LIGHT));
    //SysTick_TIMES(10);
		//printf("tab3=%d  tab5=%d\n",Tab3,Tab5);
		
		

		
		
		if(envi == 0)
		{
			OLED_Clear();
		}
		
		//第二页，环境
		if(envi == 2)
		{
			OLED_ShowCHinese2(28,0,13,4);//标题
			OLED_ShowCHinese2(16,3,17,3);//温度
			OLED_ShowCHinese2(16,5,20,3);//湿度
			OLED_ShowCHinese(81,3,23);
			OLED_ShowString(91,3,(u8 *)"C",16);
			OLED_ShowString(86,5,(u8 *)"%",16);
			OLED_ShowString(64,3,Temp,16);
			OLED_ShowString(64,5,Humi,16);
		}
		
		//第一页，主页
		if(envi == 1)
		{
		  OLED_ShowCHinese2(0,0,0,8);//标题
			OLED_ShowCHinese2(36,3,11,2);//信息
			OLED_ShowString(70,3,(u8 *)"196",16);
			//案件继续
			OLED_ShowCHinese2(18,6,73,6);
		}
		
		//第三页，模式页
		if(envi == 3)
		{
		  OLED_ShowCHinese2(10,5,40,5);//当前模式
		  OLED_ShowCHinese2(16,1,37,3);//目前
			if(mode == 1)
			{
			  OLED_ShowCHinese2(85,5,45,2);//手动
			}
			if(mode == 2)
			{
			  OLED_ShowCHinese2(85,5,47,2);//自动
			}
			if(mode == 3)
			{
			  OLED_ShowCHinese2(85,5,49,2);//定时
			}
		  if(LZZ == 1)
		  {
		  	OLED_ShowCHinese2(64,1,29,2);//夜晚
		  }
		  if(LZZ == 2)
		  {
		  	OLED_ShowCHinese2(64,1,31,2);//日落
	  	}
	  	if(LZZ == 3)
	  	{
		  	OLED_ShowCHinese2(64,1,33,2);//晴天
		  }
		  if(LZZ == 4)
		  {
			  OLED_ShowCHinese2(64,1,35,2);//晴朗
	  	}
		}
		
		//第四页，定时模式设置
		if(envi == 4)
		{
		  OLED_ShowCHinese2(16,1,49,6);//标题
			OLED_ShowString(48,5,(u8 *)"s",16);//s
			OLED_ShowString(24,5,Time,16);//
			OLED_ShowCHinese2(60,5,65,1);//后
			if(Tab1 == 1)
			{
			  OLED_ShowCHinese2(76,5,63,2);//关闭
			}
			if(Tab1 == 2)
			{
			  OLED_ShowCHinese2(76,5,61,2);//打开
			}
		}
		
		//第五页，自动模式设置
		if(envi == 5)
		{
		  OLED_ShowCHinese2(16,0,55,6);//标题
			OLED_ShowCHinese2(68,3,61,2);//打开
			OLED_ShowCHinese2(68,6,63,2);//关闭
			if(Tab3==1&&Tabud==1)
			{
			  OLED_ShowCHinese2(24,3,29,2);//夜晚
			}
			if(Tab3==2&&Tabud==1)
			{
			  OLED_ShowCHinese2(24,3,31,2);//日落
			}
			if(Tab3==3&&Tabud==1)
			{
			  OLED_ShowCHinese2(24,3,33,2);//晴天
			}
			if(Tab3==4&&Tabud==1)
			{
			  OLED_ShowCHinese2(24,3,35,2);//晴朗
			}
			if(Tab5==1&&Tabud==2)
			{
			  OLED_ShowCHinese2(24,6,29,2);//夜晚
			}
			if(Tab5==2&&Tabud==2)
			{
			  OLED_ShowCHinese2(24,6,31,2);//日落
			}
			if(Tab5==3&&Tabud==2)
			{
			  OLED_ShowCHinese2(24,6,33,2);//晴天
			}
			if(Tab5==4&&Tabud==2)
			{
			  OLED_ShowCHinese2(24,6,35,2);//晴朗
			}
		}
		
		
		
		if(mainbuf_flag)//是否进入到控制状态
		{
			
			//第一页，不需要
//			if('1'== mainbuf[3])//1XXX,XXXX
//			{
//				OLED_Clear();
//				envi = 1;
//			}

			//第二页，环境
			if('2'== mainbuf[3])//2XXX,XXXX
			{
				OLED_Clear();
				envi = 2;
			}
			
			//清屏
			if('3'== mainbuf[3])//3XXX,XXXX
			{
				OLED_Clear();
				envi = 0;
			}
			
			//第三页，模式选择
			if('4'== mainbuf[3])//4XXX,XXXX
			{
				OLED_Clear();
				envi = 3;
				if('4'== mainbuf[4])//44XX,XXXX
				{
				  mode = 1;
				}
				if('5'== mainbuf[4])//45XX,XXXX
				{
				  mode = 2;
				}
				if('6'== mainbuf[4])//46XX,XXXX
				{
				  mode = 3;
				}
			}
			
			//手动开关
			if('5'== mainbuf[3])//5XXX,XXXX
			{
			  swi = 0;
			}
			if('6'== mainbuf[3])//6XXX,XXXX
			{
			  swi = 1;
			}
			
			//定时设置
			if('7'== mainbuf[3])//7XXX,XXXX
			{
				OLED_Clear();
				envi = 4;
			}
			
			//自动设置
			if('2'== mainbuf[8])//XXXX,2XXX
			{
			  OLED_Clear();
				envi = 5;
			}
			
			//关于
			if('3'== mainbuf[8])//XXXX,3XXX
			{
			  OLED_Clear();
				envi = 6;
			}
			
			
			//定时加减键
			if(envi == 4)
			{
			  if('8'== mainbuf[3])//8XXX,XXXX
			  {
			    OLED_Clear();
					uad++;
		  	}
		  	if('9'== mainbuf[3]&&uad>0)//9XXX,XXXX
		  	{
		  	  OLED_Clear();
					uad--;
		  	}
		  }
			
			//定时tab键
			if(envi == 4)
			{
			  if('1'== mainbuf[8])//XXXX,1XXX
			  {
			    Tab1++;
			  	if(Tab1>2)
			  	{
			  	  Tab1=1;
			  	}
		  	}
			}
			
			
			//自动加减键
			if(envi == 5)
			{
			  if('8'== mainbuf[3])//8XXX,XXXX
			  {
			    Tabud=1;
		  	}
		  	if('9'== mainbuf[3])//9XXX,XXXX
		  	{
		  	  Tabud=2;
		  	}
		  }
			
			//自动tab键
			if(envi == 5)
			{
			  if('1'== mainbuf[8])//XXXX,1XXX
			  {
			    Tab2++;
			  	if(Tab2>4)
			  	{
			  	  Tab2=1;
			  	}
		  	}
			}
			
			
			//关于
			if(envi == 6)
			{
			  OLED_ShowCHinese2(28,0,79,5);//标题
			  OLED_ShowCHinese2(0,3,66,2);//成员1
				OLED_ShowString(32,3,(u8 *)"  ",16);
				OLED_ShowCHinese2(40,3,68,2);//成员2
				OLED_ShowString(72,3,(u8 *)"  ",16);
				OLED_ShowCHinese2(80,3,70,3);//成员3
				OLED_ShowString(24,6,(u8 *)"2021/12/12",16);
			}
			
			 memset(mainbuf,0,sizeof(mainbuf));
			 mainbuf_flag=0;
		}
		
		//手动模式
		if(mode == 1)
		{
			if(swi == 0)
			{
				if(sw==0)
				{
		      printf("shoudongon\n");
					TIM_SetCompare1(TIM8,17);//2.5ms --> 反向转速
	        SysTick_TIMES(50);
		      TIM_SetCompare1(TIM8,15);//1.5ms --> 速度为0
					sw++;
				}
				swi = 9;
			}
			if(swi == 1)
			{
				if(sw==1)
				{
		      printf("shoudongoff\n");
					TIM_SetCompare1(TIM8,13);//0.5ms --> 正向转速
	        SysTick_TIMES(50);
		      TIM_SetCompare1(TIM8,15);//1.5ms --> 速度为0
					sw--;
				}
				swi = 9;
			}
		}
	}
}

