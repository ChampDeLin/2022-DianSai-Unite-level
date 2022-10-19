/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "laser.h"
#include "vmsgexec.h"
#include <string.h>
#include <stdio.h>
#include "duoji.h"
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void beep_on(void)
{
  HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET);
}

void beep_off(void)
{
  HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET);
}


#define KEY_DELAY_TIME 30
#define KEY_NULL 0
#define KEY_1 1
#define KEY_2 2
#define KEY_3 3
#define KEY_4 4
#define KEY_5 5
/**
 * @brief 扫描按键
 * 
 * @return uint8_t 返回按键的�?�，0 为没有按下按�??? 
 */
uint8_t key_scan()
{
  if (HAL_GPIO_ReadPin(KEY_1_GPIO_Port, KEY_1_Pin) == GPIO_PIN_RESET) {
    HAL_Delay(KEY_DELAY_TIME);
    if (HAL_GPIO_ReadPin(KEY_1_GPIO_Port, KEY_1_Pin) == GPIO_PIN_RESET) {
      return KEY_1;
    }
  }
  else if (HAL_GPIO_ReadPin(KEY_2_GPIO_Port, KEY_2_Pin) == GPIO_PIN_RESET) {
    HAL_Delay(KEY_DELAY_TIME);
    if (HAL_GPIO_ReadPin(KEY_2_GPIO_Port, KEY_2_Pin) == GPIO_PIN_RESET) {
      return KEY_2;
    }
  }
   else if (HAL_GPIO_ReadPin(KEY_3_GPIO_Port, KEY_3_Pin) == GPIO_PIN_RESET) {
    HAL_Delay(KEY_DELAY_TIME);
    if (HAL_GPIO_ReadPin(KEY_3_GPIO_Port, KEY_3_Pin) == GPIO_PIN_RESET) {
      return KEY_3;
    }
  }
  else if (HAL_GPIO_ReadPin(KEY_4_GPIO_Port, KEY_4_Pin) == GPIO_PIN_RESET) {
    HAL_Delay(KEY_DELAY_TIME);
    if (HAL_GPIO_ReadPin(KEY_4_GPIO_Port, KEY_4_Pin) == GPIO_PIN_RESET) {
      return KEY_4;
    }
  }
  else if (HAL_GPIO_ReadPin(KEY_5_GPIO_Port, KEY_5_Pin) == GPIO_PIN_RESET) {
    HAL_Delay(KEY_DELAY_TIME);
    if (HAL_GPIO_ReadPin(KEY_5_GPIO_Port, KEY_5_Pin) == GPIO_PIN_RESET) {
      return KEY_5;
    }
  }

  return KEY_NULL;
}


#define myprintf(...)  HAL_UART_Transmit((UART_HandleTypeDef *)&huart3, (uint8_t *)u_buf,\
		                                sprintf((char*)u_buf,__VA_ARGS__), 0xFFFF);
uint8_t u_buf[256];

enum find_status  {FINDING_OBJ = 0, FIND_SIT = 1};
enum way_to_go  {ZHEN = 0, FAN = 1, JINZHI = 2};
volatile enum find_status nom_find_status = FINDING_OBJ;
enum way_to_go nom_way_to_go = JINZHI;
volatile uint8_t way_flag = 0; // 0 正转 1 反转

/**
 * 如果发现了对象
 *  根据当前的电机状态，如果正在正转，则状态为搜索对象边沿
 * 
 *  如果搜索到对象边沿，反向，记录，继续运行
 * 
 */

typedef struct scan_object_t{
  volatile	uint8_t fundobject;
  volatile	float min_len;
  volatile	float min_angle;
  volatile	uint8_t count;
	Date date[6];
}scan_object_t;

//此结构体用于第四问追踪物体
typedef struct scan_find_object_t{
  volatile	uint8_t fundobject;
  volatile	float min_len;
  volatile	float min_angle;
  volatile	uint8_t count;
  Date date_old;        //最少11
	Date date_nom;        //最少11
}scan_find_object_t;

scan_object_t scan_object={0};
scan_find_object_t scan_find_object={0};

void scan_init()
{
  memset(&scan_object, 0, sizeof(scan_object_t));
}

/**
 * @brief  处理消息队列中的消息，已经pop到了nowmsg�????
 * @note   
 * @retval None
 */
void step1_handle_msg(msg_t * pmsg)
{
	if(scan_object.fundobject == 0){		//第一
    if(pmsg->msgdata.len *100  < 81){
			myprintf("scan_object.fundobject------------------------------\r\n");
			scan_object.min_len = pmsg->msgdata.len;
			scan_object.min_angle = pmsg->msgdata.angle;
			scan_object.fundobject = 1;
      
		}
		// else{
		// 	if(scan_object.min_len > pmsg->msgdata.len){
		// 		scan_object.min_len = pmsg->msgdata.len;
		// 	}
		// }
	}
  else {
    if(scan_object.min_len > pmsg->msgdata.len){
      scan_object.min_len = pmsg->msgdata.len;
    }

    if (pmsg->msgdata.len *100  >= 81){
      //接口---返回距离
      scan_object.fundobject = 0;
      scan_object.date[scan_object.count].len = scan_object.min_len;
      scan_object.date[scan_object.count].angle = scan_object.min_angle;
      myprintf("The count = %d, len = %f, angle=%f\r\n ", 
        scan_object.count,
        scan_object.date[scan_object.count].len * 100, 
        scan_object.min_angle);
      myprintf("leave object+++++++++++++++++\r\n");

      scan_object.count++;

      sprintf((char*)u_buf, "obj:%d, len:%.1f, angle:%.2f", scan_object.count,  scan_object.min_len*100, scan_object.min_angle);
      LCD_ShowString(20,60,200,16,16, u_buf);

      if(scan_object.count >= 10){
        scan_object.count = 0;
      }
      beep_on(); HAL_Delay(10); beep_off();
    }
	}

	myprintf("len = %f\r\n", pmsg->msgdata.len);

}


#define TEST_STEP1 2
void test1()
{
  int32_t nowstep = 0;
  uint8_t nowkey;


  vwm_init(step1_handle_msg);

  //蜂鸣器叫
  beep_on();
  HAL_Delay(500);
  beep_off();

  LCD_Clear(WHITE);
  POINT_COLOR = BLACK;
  LCD_ShowString(20,30,200,16,16,"Return to Zero");

  duoji_runto_zero();
  scan_init();
  LCD_ShowString(20,30,200,16,16,"Test1 Start Press OK");
  vwm_init(NULL);
  while (1) {
    nowkey = key_scan();
    if (nowkey == KEY_3) { //确定
      beep_on(); HAL_Delay(20); beep_off();
      break;
    }
    else if (nowkey == KEY_4) {//取消
      beep_on(); HAL_Delay(20); beep_off();
      return;
    }

    vwm_exec();
  }

  vwm_init(step1_handle_msg);

  LCD_ShowString(20,30,200,16,16,"Test1 Process on");

  while (1) {
    if (nowstep < DUOJI_XIFEN / 2) {
      zhengzhuan(TEST_DELAY_TIME, TEST_STEP1);
      nowstep += TEST_STEP1;

      if (nowstep >= DUOJI_XIFEN / 2) {
        //显示结果
        beep_on(); HAL_Delay(300); beep_off();
      }
    }
    else {
      //显示结果
      nowkey = key_scan();

      if (nowkey == KEY_4) {//取消
        beep_on(); HAL_Delay(20); beep_off();
        LCD_ShowString(20,30,200,16,16,"Return to Zero");

        duoji_runto_zero();
        return;
      }
    }
    
    vwm_exec();
  }

}


#define TEST_STEP2 2
void test2()
{
  int32_t nowstep = 0;
  uint8_t nowkey;

  uint8_t maxlen = 0;
  uint8_t editindex = 0;
  uint8_t i;

  char maxlen_char[2] = {'6', '7'};

  scan_init();
  vwm_init(step1_handle_msg);

  //蜂鸣器叫
  beep_on();
  HAL_Delay(100);
  beep_off();

  LCD_Clear(WHITE);
  POINT_COLOR = BLACK;
  
  LCD_ShowString(20,30,200,16,16,"Return to Zero");
  duoji_runto_zero();
  LCD_ShowString(20,30,200,16,16,"Test2 Input Max Len");

  LCD_ShowChar(20, 90, maxlen_char[0], 16, 0);
  LCD_ShowChar(40, 90, maxlen_char[1], 16, 0);
  vwm_init(NULL);
  scan_init();

  while (1) {
    nowkey = key_scan();
    if (nowkey == KEY_1) {
      beep_on(); HAL_Delay(20); beep_off();
      if (editindex == 0) {
        if (maxlen_char[0] < '6') {
          maxlen_char[0]++;
        }
        else {
          maxlen_char[0] = '0';
        }

        LCD_ShowChar(20, 90, maxlen_char[0], 16, 0);
      }
      else {
        if (maxlen_char[1] < '9') {
          maxlen_char[1]++;
        }
        else {
          maxlen_char[1] = '0';
        }
        
        LCD_ShowChar(40, 90, maxlen_char[1], 16, 0);
      }
      while(key_scan() != KEY_NULL) {};
    }
    else if (nowkey == KEY_2) {
      beep_on(); HAL_Delay(20); beep_off();
      if (editindex == 0) {
        if (maxlen_char[0] > '0') {
          maxlen_char[0]--;
        }
        else {
          maxlen_char[0] = '6';
        }

        LCD_ShowChar(20, 90, maxlen_char[0], 16, 0);
      }
      else {
        if (maxlen_char[1] > '0') {
          maxlen_char[1]--;
        }
        else {
          maxlen_char[1] = '9';
        }

        LCD_ShowChar(40, 90, maxlen_char[1], 16, 0);
      }
      while(key_scan() != KEY_NULL) {};
    }
    else if (nowkey == KEY_3) { //确定
      beep_on(); HAL_Delay(20); beep_off();
      editindex++;
      if (editindex >= 2) {
        //完成，准备测�?
        LCD_ShowString(20,30,200,16,16,"Test2 Start Press OK");
        break;
      }
      while(key_scan() != KEY_NULL) {};
    }
    else if (nowkey == KEY_4) {//取消
      return;
    }
    vwm_exec();
  }

  while (1) {
    nowkey = key_scan();
    if (nowkey == KEY_3) { //确定
      break;
    }
    else if (nowkey == KEY_4) {//取消
      return;
    }
    vwm_exec();
  }

  LCD_ShowString(20,30,200,16,16,"Test2 Process on");
  vwm_init(step1_handle_msg);
  while (1) {
    if (nowstep < DUOJI_XIFEN / 2) {
      zhengzhuan(TEST_DELAY_TIME, TEST_STEP2);
      nowstep += TEST_STEP2;

      if (nowstep >= DUOJI_XIFEN / 2) {
        //显示结果
        nowkey = 0;
        maxlen = (maxlen_char[0] - '0') * 10 + (maxlen_char[1] - '0');
        for (i=0; i<scan_object.count; i++) {
          if (scan_object.date[i].len*100 < maxlen) {
            nowkey++;
          }
        }
        
        sprintf((char*)u_buf, "ObjCount:%d", nowkey);
        LCD_ShowString(20,120,200,16,16,u_buf);
        beep_on(); HAL_Delay(300); beep_off();
      }
    }
    else {
      //显示结果
      nowkey = key_scan();

      if (nowkey == KEY_4) {//取消
        beep_on(); HAL_Delay(20); beep_off();
        LCD_ShowString(20,30,200,16,16,"Return to Zero");

        duoji_runto_zero();
        return;
      }
    }
    
    vwm_exec();
  }

}

#define PI 3.141592653589793

typedef struct task3_t{
    float shortest_distence;
    int index1, index2;
}task3_t;
task3_t task3 = {0};


void test3_calc_shortest(void)
{
    float distance = 0.0;
    int i, j;
    memset(&task3, 0, sizeof(task3_t));
    if (scan_object.count > 1) {
      task3.shortest_distence = 3000;

      for(i = 0; i < scan_object.count - 1; i++){
          for(j = i + 1; j < scan_object.count; j++){
              distance = sqrt(scan_object.date[i].len * scan_object.date[i].len + 
                  scan_object.date[j].len * scan_object.date[j].len - 
                  2 * scan_object.date[i].len * scan_object.date[j].len * 
                  cos((scan_object.date[i].angle - scan_object.date[j].angle)*PI/180));
              if(distance < task3.shortest_distence){
                  task3.shortest_distence = distance;
                  task3.index1 = i;
                  task3.index2 = j;
              }
          }
      }

    }
}

#define TEST_STEP3 2
void test3()
{
  int32_t nowstep = 0;
  uint8_t nowkey;


  vwm_init(step1_handle_msg);

  //蜂鸣器叫
  beep_on();
  HAL_Delay(300);
  beep_off();

  LCD_Clear(WHITE);
  POINT_COLOR = BLACK;
  LCD_ShowString(20,30,200,16,16,"Return to Zero");

  duoji_runto_zero();
  scan_init();
  LCD_ShowString(20,30,200,16,16,"Test3 Start Press OK");

  vwm_init(NULL);
  while (1) {
    nowkey = key_scan();
    if (nowkey == KEY_3) { //确定
      beep_on(); HAL_Delay(20); beep_off();
      break;
    }
    else if (nowkey == KEY_4) {//取消
      beep_on(); HAL_Delay(20); beep_off();
      return;
    }

    vwm_exec();
  }

  scan_init();
  vwm_init(step1_handle_msg);

  LCD_ShowString(20,30,200,16,16,"Test3 Process on");

  while (1) {
    if (nowstep < DUOJI_XIFEN / 2) {
      zhengzhuan(TEST_DELAY_TIME, TEST_STEP3);
      nowstep += TEST_STEP3;

      if (nowstep >= DUOJI_XIFEN / 2) {
        //显示结果
        test3_calc_shortest();

        //显示结果
        LCD_ShowString(20,30,200,16,16,"Test3 Finish OK!!!");

        sprintf((char*)u_buf, "ID1:%d,ID2:%d,Dis:%.2fcm", task3.index1, task3.index2, task3.shortest_distence*100);
        LCD_ShowString(20,60,200,16,16,u_buf);
        beep_on(); HAL_Delay(300); beep_off();

      }
    }
    else {
      //显示结果
      nowkey = key_scan();

      if (nowkey == KEY_4) {//取消
        beep_on(); HAL_Delay(20); beep_off();
        LCD_ShowString(20,30,200,16,16,"Return to Zero");

        duoji_runto_zero();
        return;
      }
    }
    
    vwm_exec();
  }
}

#if 0
void step4_handle_msg(msg_t * pmsg)
{
  if(nom_find_status == FINDING_OBJ){
    if(scan_find_object.fundobject == 0){		//如果还没有检测到物体
      if(pmsg->msgdata.len *100  <= 81){
        myprintf("scan_object.fundobject------------------------------\r\n");
        scan_find_object.min_len = pmsg->msgdata.len;
        scan_find_object.min_angle = pmsg->msgdata.angle;
        scan_find_object.fundobject = 1;
        nom_find_status = FIND_SIT;
      }
	  }
    else {
      nom_find_status = FIND_SIT;
    }
  }
  else if(nom_find_status == FIND_SIT){
      if (scan_find_object.fundobject == 1) { //以及找到物体，找出去的边沿
        if(scan_find_object.min_len > pmsg->msgdata.len){
          scan_find_object.min_len = pmsg->msgdata.len;
        }

        if (pmsg->msgdata.len *100  > 81){
          //接口---返回距离
          //如果已经出去了，换向并重新进行物体搜寻
          scan_find_object.fundobject = 0;

          //换向

          way_flag = way_flag + 1;
          way_flag = way_flag % 2;

          LCD_DrawPoint_angle(scan_find_object.date_nom.len, scan_find_object.date_nom.angle);
          // beep_on(); HAL_Delay(2); beep_off();
        }
      }
      else if (scan_find_object.fundobject == 0) { //正在搜寻物体，找进去的边沿 
        if(pmsg->msgdata.len *100  <= 81){
          myprintf("scan_object.fundobject------------------------------\r\n");
          scan_find_object.min_len = pmsg->msgdata.len;
          scan_find_object.min_angle = pmsg->msgdata.angle;
          scan_find_object.fundobject = 1;
        }


      }

    //if(scan_find_object.fundobject == 0){		//第一
      if(pmsg->msgdata.len *100  < 81 ){
        scan_find_object.min_len = pmsg->msgdata.len;
        scan_find_object.min_angle = pmsg->msgdata.angle;

        scan_find_object.date_nom.len = scan_find_object.min_len;
        scan_find_object.date_nom.angle = scan_find_object.min_angle;

        if(scan_find_object.date_nom.angle > scan_find_object.date_old.angle )
        {
          nom_way_to_go = ZHEN;
          LCD_DrawLine_angles(scan_find_object.date_old.len,scan_find_object.date_old.angle, 
                              scan_find_object.date_nom.len, scan_find_object.date_nom.angle);
          scan_find_object.date_old.len = scan_find_object.date_nom.len;
          scan_find_object.date_old.angle = scan_find_object.date_nom.angle;
          beep_on(); HAL_Delay(2); beep_off();
        }
        else if(scan_find_object.date_nom.angle < scan_find_object.date_old.angle )
        {
          nom_way_to_go = FAN;
          LCD_DrawLine_angles(scan_find_object.date_old.len,scan_find_object.date_old.angle, 
                              scan_find_object.date_nom.len, scan_find_object.date_nom.angle);
          scan_find_object.date_old.len = scan_find_object.date_nom.len;
          scan_find_object.date_old.angle = scan_find_object.date_nom.angle;
          beep_on(); HAL_Delay(2); beep_off();
        }
        else {
          nom_way_to_go = JINZHI;
          beep_on(); HAL_Delay(2); beep_off();
        }
        LCD_DrawPoint_angle(scan_find_object.date_nom.len, scan_find_object.date_nom.angle);
        
        }
        else {

        }

    //  }
  }



}
#endif
uint8_t test4_delay_time = 4;
void step4_handle_msg(msg_t * pmsg)
{
    if(scan_find_object.fundobject == 0){		//如果还没有检测到物体
      if(pmsg->msgdata.len *100  <= 81){
        //myprintf("scan_object.fundobject, way: %d\r\n", way_flag);
        scan_find_object.min_len = pmsg->msgdata.len;
        scan_find_object.min_angle = pmsg->msgdata.angle;
        scan_find_object.fundobject = 1;
        test4_delay_time = 5;
        
        beep_on(); HAL_Delay(5); beep_off();
        POINT_COLOR = RED;
        //LCD_DrawLine_angles(scan_find_object.min_len,scan_find_object.min_angle,  scan_find_object.date_old.len, scan_find_object.date_old.angle);
        LCD_DrawPoint_angle(scan_find_object.min_len, scan_find_object.min_angle);
        scan_find_object.date_old.angle = scan_find_object.min_angle;
        scan_find_object.date_old.len = scan_find_object.min_len;
        
      }
	  }
    else if (scan_find_object.fundobject == 1) { //以及找到物体，找出去的边沿
      if(scan_find_object.min_len > pmsg->msgdata.len){
        scan_find_object.min_len = pmsg->msgdata.len;
        scan_find_object.min_angle = pmsg->msgdata.angle;
        POINT_COLOR = RED;
        //LCD_DrawLine_angles(scan_find_object.min_len,scan_find_object.min_angle,  scan_find_object.date_old.len, scan_find_object.date_old.angle);
        LCD_DrawPoint_angle(scan_find_object.min_len, scan_find_object.min_angle);
        scan_find_object.date_old.angle = scan_find_object.min_angle;
        scan_find_object.date_old.len = scan_find_object.min_len;
      }

      if (pmsg->msgdata.len *100  > 81){
        //如果已经出去了，换向并重新进行物体搜寻
        scan_find_object.fundobject = 0;
        //test4_delay_time = 4;

        //换向

        way_flag = way_flag + 1;
        way_flag = way_flag % 2;
        beep_on(); HAL_Delay(5); beep_off();

        //LCD_DrawPoint_angle(scan_find_object.min_len, scan_find_object.min_angle);
        // beep_on(); HAL_Delay(2); beep_off();
        //myprintf("scan_object.outobject，way:%d\r\n", way_flag);
      }
    }
    
    //myprintf("len = %f\r\n", pmsg->msgdata.len);

}

#define ADD_PAUSED 70

void test4()
{
  int32_t nowstep = 0;
  uint8_t nowkey;
 
  vwm_init(NULL);
  //蜂鸣器叫
  beep_on();
  HAL_Delay(300);
  beep_off();

  LCD_Clear(WHITE);
  POINT_COLOR = BLACK;
  LCD_ShowString(20,30,200,16,16,"Return to Zero");

  duoji_runto_zero();

  LCD_ShowString(20,30,200,16,16,"Test4 Start Press OK");

  //laser_20Hz();
  scan_init();
  vwm_init(step4_handle_msg);

  way_flag = 0; //正转
  nowstep = 0;
  LCD_Clear(WHITE);
  POINT_COLOR = BLACK;
  LCD_DrawLine(0,160,160,160);
  LCD_DrawLine(160,0,160,160);
  LCD_DrawLine(0,80,160,80);
  LCD_DrawLine(80,0,80,160);
  while (1) {
    if (way_flag == 0) { //正转
      if (nowstep < DUOJI_XIFEN / 2) {
        zhengzhuan(test4_delay_time, TEST_STEP1);
        nowstep += TEST_STEP1;

        if (nowstep >= DUOJI_XIFEN / 2) {
          //显示结果
          beep_on(); HAL_Delay(10); beep_off();
          way_flag = 1; //反转
        }
      }
      else {
        way_flag = 1; //反转
      }
    }
    else if(way_flag == 1){
      if (nowstep > 0) {
        fanzhuan(test4_delay_time, TEST_STEP1);
        nowstep -= TEST_STEP1;

        if (nowstep <= 0) {
          //显示结果
          beep_on(); HAL_Delay(10); beep_off();
          way_flag = 0; //反转
        }
      }
      else {
        way_flag = 0; //反转
      }
    }

    //显示结果
    nowkey = key_scan();

    if (nowkey == KEY_4) {//取消
      beep_on(); HAL_Delay(20); beep_off();
      LCD_ShowString(20,30,200,16,16,"Return to Zero");

      duoji_runto_zero();
      return;
    }

    vwm_exec();
  }

}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	uint8_t x=0;
	uint8_t nowkey;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_FSMC_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM5_Init();
  /* USER CODE BEGIN 2 */
  beep_on(); HAL_Delay(200); beep_off();
	HAL_Delay(168);
	LCD_Init();
	LCD_Clear(BLUE);
	POINT_COLOR = RED;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);

	// __HAL_UART_CLEAR_IDLEFLAG(&huart1);
	// __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);	
	// HAL_UART_Receive_DMA(&huart1, (uint8_t*)g_laser.rev_buffer, 22);

  laser_init(&huart1);

  LCD_Clear(WHITE);
  POINT_COLOR = BLACK;
  
  // LCD_DrawLine_angles(40,60,60,150);
  // while (1);
  //LCD_ShowString(20,30,200,16,16,"Select Test Item");

  // while (1) {
  //   zhengzhuan(5, 800);
  // }

  //接收按键选择
  while (1) {
    nowkey = key_scan();
    if (nowkey != KEY_NULL) {
      switch (nowkey) {
        case KEY_1:
          test1();
          break;

        case KEY_2:
          test2();
        break;

        case KEY_3:
          test3();
          break;

        case KEY_4:
          test4();
          break;
      }

      vwm_init(NULL);
      //刷新主界
      LCD_Clear(WHITE);
      POINT_COLOR = BLACK;
      LCD_ShowString(20,30,200,16,16,"Select Test Item");
    }
  }
  test1();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	//LCD_ShowString(0,0,120,20,24,"WWW");
  while (1)
  {
//		switch(x)
//		{
//			case 0:LCD_Clear(WHITE);break;
//			case 1:LCD_Clear(BLACK);break;
//			case 2:LCD_Clear(BLUE);break;
//			case 3:LCD_Clear(RED);break;
//			case 4:LCD_Clear(MAGENTA);break;
//			case 5:LCD_Clear(GREEN);break;
//			case 6:LCD_Clear(CYAN);break; 
//			case 7:LCD_Clear(YELLOW);break;
//			case 8:LCD_Clear(BRRED);break;
//			case 9:LCD_Clear(GRAY);break;
//			case 10:LCD_Clear(LGRAY);break;
//			case 11:LCD_Clear(BROWN);break;
//		}
//		POINT_COLOR=RED;	  
//		LCD_ShowString(30,40,210,24,24,"Explorer STM32F4");	
//		LCD_ShowString(30,70,200,16,16,"TFTLCD TEST");
//		LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
// 		//LCD_ShowString(30,110,200,16,16,lcd_id);		//��ʾLCD ID	      					 
//		LCD_ShowString(30,130,200,12,12,"2017/4/8");	      					 
//		x++;
//		if(x==12)x=0;
//		
//		HAL_Delay(1000);	
//		
//		key = matrix_keyboard_scan(&g_keyboard);
//		duoji_to_angle(180);
		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
