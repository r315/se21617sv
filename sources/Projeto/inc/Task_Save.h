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
}SAVE_Item;


#define SAVE_BASE_ADDRESS 0
#define TASK_SAVE_WAIT_TICKS 1000
#define SAVE_QUEUE_MAX_ELEMENTS 10



/**
 * @brief coloca dados na fila para serem escritos
 */
portBASE_TYPE SAVE_QueuePut(void *data, uint32_t size);

/**
 * @brief obtem dados da fila
 */
portBASE_TYPE SAVE_QueueGet(void *data, uint32_t size);

/**
 * @brief Funcao chamada pela task que efectua as leituras e escrita na memoria eeprom
 */
void Task_Save(void *ptr);

#endif /* __task_save_h__ */
