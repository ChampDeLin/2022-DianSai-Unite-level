#include <stdint.h>
#include "main.h"
#include "laser.h"
#include <stdbool.h>
#include <stdlib.h>
#include "stdio.h"
#include "duoji.h"
#include "vmsgexec.h"


//float len[5000];

laser_lens_t g_laser;

void laser_init(UART_HandleTypeDef *huart)
{
	uint8_t sendbuff[5];

	g_laser.huart = huart;	


	/*发送帧FA 04 0A 14 E4*/

	
	HAL_Delay(40);
	//5m量程
	sendbuff[0] = 0xFA;
	sendbuff[1] = 0x04;
	sendbuff[2] = 0x09;
	sendbuff[3] = 0x05;	
	sendbuff[4] = 0xF4;	
	HAL_UART_Transmit(&huart1, sendbuff, 5, 5*10);
	HAL_Delay(40);

	//分辨率1mm
	sendbuff[0] = 0xFA;
	sendbuff[1] = 0x04;
	sendbuff[2] = 0x0C;
	sendbuff[3] = 0x01;	
	sendbuff[4] = 0xF5;	
	HAL_UART_Transmit(&huart1, sendbuff, 5, 5*10);
	HAL_Delay(40);

	// //10Hz
	// sendbuff[0] = 0xFA;
	// sendbuff[1] = 0x04;
	// sendbuff[2] = 0x0A;
	// sendbuff[3] = 0x0A;
	// sendbuff[4] = 0xEE;
	// HAL_UART_Transmit(&huart1, sendbuff, 5, 5*10);

	//5Hz
	sendbuff[0] = 0xFA;
	sendbuff[1] = 0x04;
	sendbuff[2] = 0x0A;
	sendbuff[3] = 0x05;
	sendbuff[4] = 0xF3;
	HAL_UART_Transmit(&huart1, sendbuff, 5, 5*10);

	HAL_Delay(40);
	sendbuff[0] = 0x80;
	sendbuff[1] = 0x06;
	sendbuff[2] = 0x03;
	sendbuff[3] = 0x77;	
	HAL_UART_Transmit(&huart1, sendbuff, 4, 4*10);

	__HAL_UART_CLEAR_IDLEFLAG(&huart1);
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);	
	HAL_UART_Receive_DMA(&huart1, (uint8_t*)g_laser.rev_buffer, 22);
}

void laser_20Hz()
{
	uint8_t sendbuff[5];

/*发送帧FA 04 0A 14 E4*/
	// //关机
	// HAL_Delay(40);
	// sendbuff[0] = 0x80;
	// sendbuff[1] = 0x04;
	// sendbuff[2] = 0x02;
	// sendbuff[3] = 0x7A;	
	// HAL_UART_Transmit(&huart1, sendbuff, 4, 4*10);

	// HAL_Delay(40);
	// sendbuff[0] = 0x80;
	// sendbuff[1] = 0x04;
	// sendbuff[2] = 0x02;
	// sendbuff[3] = 0x7A;	
	// HAL_UART_Transmit(&huart1, sendbuff, 4, 4*10);


	HAL_Delay(40);
	//5m量程
	sendbuff[0] = 0xFA;
	sendbuff[1] = 0x04;
	sendbuff[2] = 0x09;
	sendbuff[3] = 0x05;	
	sendbuff[4] = 0xF4;	
	HAL_UART_Transmit(&huart1, sendbuff, 5, 5*10);

	HAL_Delay(40);
	//分辨率1mm
	sendbuff[0] = 0xFA;
	sendbuff[1] = 0x04;
	sendbuff[2] = 0x0C;
	sendbuff[3] = 0x01;	
	sendbuff[4] = 0xF5;	
	HAL_UART_Transmit(&huart1, sendbuff, 5, 5*10);
	HAL_Delay(40);

	//5Hz
	sendbuff[0] = 0xFA;
	sendbuff[1] = 0x04;
	sendbuff[2] = 0x0A;
	sendbuff[3] = 0x05;
	sendbuff[4] = 0xF3;
	HAL_UART_Transmit(&huart1, sendbuff, 5, 5*10);

	HAL_Delay(40);
	sendbuff[0] = 0x80;
	sendbuff[1] = 0x06;
	sendbuff[2] = 0x03;
	sendbuff[3] = 0x77;	
	HAL_UART_Transmit(&huart1, sendbuff, 4, 4*10);
}


void laser_10Hz()
{
	uint8_t sendbuff[5];
/*发送帧FA 04 0A 14 E4*/
	sendbuff[0] = 0xFA;
	sendbuff[1] = 0x04;
	sendbuff[2] = 0x0A;
	sendbuff[3] = 0x0A;
	sendbuff[4] = 0xEE;
	HAL_UART_Transmit(&huart1, sendbuff, 5, 5*10);

	HAL_Delay(40);
	sendbuff[0] = 0x80;
	sendbuff[1] = 0x06;
	sendbuff[2] = 0x03;
	sendbuff[3] = 0x77;	
	HAL_UART_Transmit(&huart1, sendbuff, 4, 4*10);
}
void laser_dma_open(){
	__HAL_UART_CLEAR_IDLEFLAG(&huart1);
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart1, (uint8_t*)g_laser.rev_buffer, 22);
}

static Date laser_sending_msg;
void laser_receive(){
	uint8_t *pdata = NULL;

	
	if (g_laser.rev_buffer[0] == 0x80) {
		pdata = (uint8_t*)&g_laser.rev_buffer[0];
	}
	else if (g_laser.rev_buffer[1] == 0x80) {
		pdata = (uint8_t*)&g_laser.rev_buffer[1];
	}
	if (pdata != NULL) {			
		if (pdata[0] == 0x80 &&
			pdata[1] == 0x06 &&
			pdata[2] == 0x83) {
			//帧头正确，解析内容

			if (pdata[3] != 'E') {
				pdata[10] = 0;

				g_laser.distanceM = atof((char*)&pdata[3]) +0.083;
				g_laser.distanceCM = (int32_t)(g_laser.distanceM * 100);
				laser_sending_msg.angle = real_time_angle();
				laser_sending_msg.len = g_laser.distanceM;
				vwm_sendmessage(1, laser_sending_msg);

				laser_dma_open();
				//return true;
			}else{laser_dma_open();}
		}else{laser_dma_open();}
	}else{laser_dma_open();}
}
