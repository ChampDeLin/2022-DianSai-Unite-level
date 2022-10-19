#include "vdelay.h"
#include "stm32f4xx_hal.h"

//extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim5;
//定时器分频到1Mhz
//定时器ARR设为0xFFFF
//向上计数，计数值为0~ARR

void delay_us(uint32_t nus)
{
//	uint32_t differ;
//	differ = 0xffff-nus-10;
	
	//__HAL_TIM_ENABLE(&htim5);
	HAL_TIM_Base_Start(&htim5);
	__HAL_TIM_SET_COUNTER(&htim5, 0);
//	while(differ<0xffff-10)
//    {
//        differ=__HAL_TIM_GET_COUNTER(&htim5);
//    }
	while(__HAL_TIM_GET_COUNTER(&htim5) <= ( nus));//72是系统时钟，更改这里
	/* Disable the Peripheral */
	//__HAL_TIM_DISABLE(&htim5);
	HAL_TIM_Base_Stop(&htim5);
}
