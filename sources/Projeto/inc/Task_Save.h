#ifndef __task_save_h__
#define __task_save_h__

#include <stdint.h>

typedef struct _SAVE_Item{
	void *data;
	uint32_t size;
}SAVE_Item;

#define SAVE_QUEUE_MAX_ELEMENTS 2
#define TASK_SAVE_MAX_TICKS 100

/*
 * @brief coloca dados na fila para serem escritos
 */
portBASE_TYPE SAVE_QueuePut(void *data, uint32_t size);
portBASE_TYPE SAVE_QueueGet(void *data, uint32_t size);

#endif /* __task_save_h__ */
