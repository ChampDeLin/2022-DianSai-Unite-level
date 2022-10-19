#ifndef  DUOJO_H
#define DUOJO_H
#include "main.h"
#include "stdbool.h"

#define TEST_DELAY_TIME  4
#define DUOJI_XIFEN   3200   // 转一圈的脉冲数

void duoji_calibration(void);
//void duoji_to_angle(int16_t angle);
//void duoji_to_pulse_count(bool dir, uint16_t count);  // dir 方向
extern volatile int32_t realtime_pulse_count;
void fanzhuan(uint16_t delay_time, uint16_t pulse_count);
void zhengzhuan(uint16_t delay_time, uint16_t pulse_count);
void baidong(uint16_t delay_time, uint16_t add_pulse_count);
float real_time_angle(void);

void duoji_runto_zero(void);
#endif
