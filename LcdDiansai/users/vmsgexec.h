#ifndef VMSG_EXEC_H
#define VMSG_EXEC_H
#include "stdint.h"
#include "stm32f4xx_hal.h"
#include "laser.h"
//----------------------------------------------------------------
//全局配置定义
#define MSG_QUEUE_SIZE 100
 #define VMSGEXEC_ACQUIRE_LOCK() HAL_NVIC_DisableIRQ(USART1_IRQn);
 #define VMSGEXEC_RELEASE_LOCK() HAL_NVIC_EnableIRQ(USART1_IRQn);

//全局配置定义结束
//----------------------------------------------------------------

//消息引擎开始
typedef enum {
	MSG_NULL,
	MSG_KEY_PRESS,
	MSG_KEY_RELEASE,
	MSG_TIMER,
	MSG_KEY_LONG_PRESS
}msg_id_t;

typedef struct msg_t {
	msg_id_t msgid;
	Date msgdata;
}msg_t;


typedef void (*vwm_callback_t)(msg_t * pmsg);

void vwm_init(vwm_callback_t msg_handle_callback);
void vwm_sendmessage(msg_id_t msgid, Date msgdata);
void vwm_exec(void);

#endif
