/*
 * @Author: lkx 1658228948@qq.com
 * @Date: 2022-05-27 11:34:44
 * @LastEditors: lkx 1658228948@qq.com
 * @LastEditTime: 2022-05-27 21:59:05
 * @FilePath: \2022_five_complete\user\duoji.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "duoji.h"
#include "main.h"
#include "stdbool.h"
#include "math.h"
#include <stdint.h>
#include "vdelay.h"

#define one_angle_to_pulse_count (xifen/360.0)
//extern volatile int32_t tim_count;
//extern TIM_HandleTypeDef htim1;
volatile int32_t realtime_pulse_count = 0;
static void zhengzhuan_api(uint16_t delay_time);
static void fanzhuan_api(uint16_t delay_time);


float real_time_angle(void)
{
    int n = realtime_pulse_count;
	float re = fabs(n) * (360.0 / DUOJI_XIFEN);
    
	return n > 0 ? re : 360.0 - re;
}

void zhengzhuan(uint16_t delay_time, uint16_t pulse_count)
{
	uint16_t uu = pulse_count;
	realtime_pulse_count += pulse_count;
	while(uu--){
		zhengzhuan_api(delay_time);
	}
}

void fanzhuan(uint16_t delay_time, uint16_t pulse_count)
{
	uint16_t uu = pulse_count;
	realtime_pulse_count -= pulse_count;
	while(uu--){
		fanzhuan_api(delay_time);
	}
}

static void zhengzhuan_api(uint16_t delay_time){

    HAL_GPIO_WritePin(dir_GPIO_Port, dir_Pin, GPIO_PIN_SET);

    HAL_GPIO_WritePin(pul_GPIO_Port, pul_Pin, GPIO_PIN_SET);  //
    //HAL_Delay(delay_time);
    delay_us(delay_time*1150);
    HAL_GPIO_WritePin(pul_GPIO_Port, pul_Pin, GPIO_PIN_RESET);  //
    //HAL_Delay(delay_time);
    delay_us(delay_time*1150);
}

static void fanzhuan_api(uint16_t delay_time){

    HAL_GPIO_WritePin(dir_GPIO_Port, dir_Pin, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(pul_GPIO_Port, pul_Pin, GPIO_PIN_SET);  //
    //HAL_Delay(delay_time);
    delay_us(delay_time*1150);
    HAL_GPIO_WritePin(pul_GPIO_Port, pul_Pin, GPIO_PIN_RESET);  //
    //HAL_Delay(delay_time);
    delay_us(delay_time*1150);
}

//50摆动6du
void baidong(uint16_t delay_time, uint16_t add_pulse_count)
{
    fanzhuan(delay_time, add_pulse_count*2);

    zhengzhuan(delay_time, add_pulse_count*2);    
}

void duoji_calibration()
{
    HAL_GPIO_WritePin(en_GPIO_Port, en_Pin, GPIO_PIN_SET);  // 步进电机失能
}


void duoji_runto_zero()
{
    if (realtime_pulse_count > 0) {
        fanzhuan(TEST_DELAY_TIME, realtime_pulse_count);
    }
    else if (realtime_pulse_count < 0) {
        zhengzhuan(TEST_DELAY_TIME, -1 * realtime_pulse_count);
    }
}
//void duoji_to_angle(int16_t angle)
//{
//    int count = fabs(angle) * one_angle_to_pulse_count;  // 脉冲个数

//    if(angle > 0){
//        duoji_to_pulse_count(true, count);   // 正转
//    }else{
//        duoji_to_pulse_count(false, count);  // 反转
//    }
//}

//void duoji_to_pulse_count(bool dir, uint16_t count)  // dir 方向
//{
//    if(dir == true){
//        HAL_GPIO_WritePin(dir_GPIO_Port, dir_Pin, GPIO_PIN_SET);
//    }else{
//        HAL_GPIO_WritePin(dir_GPIO_Port, dir_Pin, GPIO_PIN_RESET);
//    }

//	tim_count = count * 2;
//    HAL_GPIO_WritePin(en_GPIO_Port, en_Pin, GPIO_PIN_RESET);  // 步进电机使能
//    HAL_TIM_Base_Start_IT( &htim1 );
//}
