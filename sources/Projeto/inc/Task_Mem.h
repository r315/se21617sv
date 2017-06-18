/**
 * \addtogroup Tasks
 * @{
 */
#ifndef __task_save_h__
#define __task_save_h__

#include <stdint.h>

typedef struct{
	void *data;
	uint32_t size;
	uint8_t operation;
}SAVE_Item;


#define SAVE_BASE_ADDRESS 0
#define TASK_SAVE_WAIT_TICKS 1000
#define SAVE_QUEUE_MAX_ELEMENTS 2

enum SAVE_OPER{
	SAVE_READ,
	SAVE_WRITE
};



/**
 * @brief coloca dados na fila para serem escritos
 */
void SAVE_QueuePut(void* data, uint32_t size);

/**
 * @brief obtem dados da fila
 */
void SAVE_QueueGet(void *data, uint32_t size);

/**
 * @brief Funcao chamada pela task
 *
 */
void Task_Save(void *ptr);

#endif /* __task_save_h__ */
