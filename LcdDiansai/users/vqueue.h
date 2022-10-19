#ifndef SQUEUE_H
#define	SQUEUE_H
#include "stdint.h"
#define queue_size_t uint8_t //环形队列的缓冲池大小
#define queue_item_size_t uint8_t
typedef struct vqueue_t{
    volatile queue_size_t readi;
    volatile queue_size_t writei;
	queue_size_t queue_size;
	volatile queue_size_t virtual_readi;
	queue_item_size_t item_size;//每个条目的大小
	volatile queue_size_t count;//现在有的条目
    uint8_t *buff;
}vqueue_t;

void vqueue_init(vqueue_t* queue, uint8_t *buff, queue_size_t qsize, queue_item_size_t item_size);
void vqueue_reset(vqueue_t* queue);
uint8_t vqueue_push(vqueue_t* queue, uint8_t * da);
uint8_t vqueue_pop(vqueue_t* queue, uint8_t * da);
queue_size_t vqueue_restsize(vqueue_t* queue);
queue_size_t vqueue_length(vqueue_t* queue);
uint8_t vqueue_virtualpop(vqueue_t* queue, uint8_t *da, uint8_t ifsetvirtualindex);
#endif	/* SQUEUE_H */

