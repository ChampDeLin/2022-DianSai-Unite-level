/***
 * 51单片机版的环形队列
 * author		: 庞胜利
 * updatetime	: 2022-03-24
 * */


#include <string.h>
#include "vqueue.h"
#include "stdint.h"


/**
 * @brief  初始化环形队列
 * @note   
 * @param  queue: 队列对象指针
 * @param  *buff: 队列缓冲区指针
 * @param  qsize: 队列最大条目个数
 * @retval None
 */
void vqueue_init(vqueue_t* queue, uint8_t *buff, queue_size_t qsize, queue_item_size_t item_size)
{
    memset(queue, 0, sizeof(vqueue_t));
	queue->buff = buff;
	memset(queue->buff, 0, qsize*item_size);
	queue->queue_size = qsize;
	queue->item_size = item_size;
	queue->count = 0;
}

/**
 * @brief  环形队列复位
 * @note   
 * @retval None
 */
void vqueue_reset(vqueue_t* queue)
{
	queue->readi = 0;
	queue->writei = 0;
	queue->count = 0;
}

/**
 * @brief  环形队列入队
 * @note   
 * @param  queue: 
 * @param  da: 
 * @retval 0 队列满 1 成功
 */
uint8_t vqueue_push(vqueue_t* queue, uint8_t* da)
{
    //queue_size_t i;

	if (queue->count >= queue->queue_size) {
		//beep_on();delay_ms(1);beep_off();
		return 0;
	}

	// if (queue->writei - queue->readi >= queue->queue_size) {
	// 	beep_on();
	// 	return 0;
	// }
    //i = queue->writei % queue->queue_size;
    
	memcpy(&queue->buff[queue->writei*queue->item_size], da, queue->item_size);
	queue->writei++;
	queue->count++;
	if (queue->writei >= queue->queue_size) {
		queue->writei = 0;
	}
	return 1;
}

/**
 * @brief  环形队列出队
 * @note   
 * @param  queue: 环形队列对象指针
 * @param  *da: 要弹出的条目
 * @retval 0 队列空 1 正确弹出
 */
uint8_t vqueue_pop(vqueue_t* queue, uint8_t *da)
{
    
	if (queue->count == 0) {
		return 0;
	}
    // if (queue->writei == queue->readi)
    //     return 0;
    
	memcpy(da, &queue->buff[queue->readi*queue->item_size], queue->item_size);
	queue->readi++;
	if (queue->readi >= queue->queue_size) {
		queue->readi = 0;
	}
	
	queue->count--;
    
    return 1;
}

/**
 * @brief  返回环形队列空闲的条目数
 * @note   
 * @param  queue: 环形队列对象指针
 * @retval 
 */
queue_size_t vqueue_restsize(vqueue_t* queue)
{
	return queue->queue_size - queue->count;
}


/**
 * @brief  返回环形队列现在都长度，即队列中有多少条目
 * @note   
 * @param  queue: 环形队列对象指针
 * @retval 
 */
queue_size_t vqueue_count(vqueue_t* queue)
{
	return queue->count;
}

/**
 * @brief  虚拟弹出
 * @note   
 * @param  queue: 环形队列对象指针
 * @param  *da: 要虚拟弹出的数据指针
 * @param  ifsetvirtualindex: 是否从当前队列首开始弹出
 * @retval 0 队列空 1 正确虚拟弹出
 */

uint8_t vqueue_virtualpop(vqueue_t* queue, uint8_t *da, uint8_t ifsetvirtualindex)
{
    if (queue->count == 0)
        return 0;

	if (ifsetvirtualindex == 1)
		queue->virtual_readi = queue->readi;
	
    queue->virtual_readi++;
    //*da = queue->buff[i];
	memcpy(da, &queue->buff[queue->virtual_readi*queue->item_size], queue->item_size);
    queue->virtual_readi++;
	queue->virtual_readi = queue->virtual_readi % queue->queue_size;

    return 1;
}

