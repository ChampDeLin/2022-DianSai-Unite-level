#include "vmsgexec.h"
#include "vqueue.h"

//消息队列定义

//窗口管理类
typedef struct vwm_t {
	msg_t 			msg_internel_array[MSG_QUEUE_SIZE];
	vqueue_t 		msg_queue;
	vwm_callback_t 	msg_handle_callback;
	msg_t 			handling_msg;
}vwm_t;

vwm_t g_wm = {0};

void vwm_init(vwm_callback_t msg_handle_callback)
{
	vqueue_init(&g_wm.msg_queue, (uint8_t*)&g_wm.msg_internel_array[0], MSG_QUEUE_SIZE, sizeof(msg_t));
	g_wm.msg_handle_callback = msg_handle_callback;
}


void vwm_sendmessage(msg_id_t msgid, Date msgdata)
{
	msg_t tmpmsg;
	tmpmsg.msgid = msgid;
	tmpmsg.msgdata = msgdata;

	VMSGEXEC_ACQUIRE_LOCK();
	vqueue_push(&g_wm.msg_queue, (uint8_t*)&tmpmsg);
	VMSGEXEC_RELEASE_LOCK();
}


/**
* @brief 执行消息处理，一般在消息处理线程中执行
* @return 0 执行成功 1 还有消息没有执行完毕
*/
void vwm_exec(void)
{	
	uint8_t popresult;
	//看看消息队列是否有消息，如果有消息对其进行处理
	VMSGEXEC_ACQUIRE_LOCK();
	popresult = vqueue_pop(&g_wm.msg_queue, (uint8_t*)&g_wm.handling_msg);
	VMSGEXEC_RELEASE_LOCK();
	if (popresult == 1) {
		if (g_wm.msg_handle_callback != NULL) {
			g_wm.msg_handle_callback(&g_wm.handling_msg);
		}		
	}
}
