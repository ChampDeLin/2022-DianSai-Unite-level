#ifndef LASER_HH
#define LASER_HH
#include <stdint.h>
#include <stdbool.h>
#include "usart.h"

#define LASER_REV_BUFFER_SIZE 11

typedef struct Date{
	float len;
	float angle;
}Date;

typedef struct laser_lens_t {
	UART_HandleTypeDef *huart;
	volatile uint8_t	rev_buffer[LASER_REV_BUFFER_SIZE*2];
	volatile uint16_t	payload_length;
	volatile int32_t	distanceCM;
	volatile float		distanceM;
	char				distance_char_cm[3];
	volatile bool		collect_finished;
}laser_lens_t;

extern laser_lens_t g_laser;

void laser_init(UART_HandleTypeDef *huart);

//bool laser_sample_one(void);
void laser_receive(void);
void laser_10Hz();
void laser_20Hz();

#endif

