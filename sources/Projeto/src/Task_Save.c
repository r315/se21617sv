#include <string.h>
#include <stdint.h>
#include <flash.h>
#include <eeprom.h>
#include <Task_Common.h>
#include <queue.h>
#include <stdio.h>

xQueueHandle *save_queue;
SAVE_Item svi;

#if TASK_SAVE_DEBUG
static void LOG(char *msg){
	printf("Task Button: %s", msg);
}
#endif

portBASE_TYPE SAVE_QueuePut(void *data, uint32_t size){
	//return EEPROM_Write(0, data, dataSize);
portBASE_TYPE res;
	svi.data = data;
	svi.size = size;
	res = xQueueSendToBack(save_queue, &svi, TASK_SAVE_MAX_TICKS);
	if(res == errQUEUE_FULL)
		LOG("Save QUEUE Full\n");
	return res;
}

portBASE_TYPE SAVE_QueueGet(void *data, uint32_t size){
	//return EEPROM_Read(0, data, dataSize);
	portBASE_TYPE res = 0;
	return res;
}

void Task_Save(void *ptr){

	save_queue = xQueueCreate(SAVE_QUEUE_MAX_ELEMENTS, sizeof(SAVE_Item));

	while(1){
		if(uxQueueMessagesWaiting(save_queue) != SAVE_QUEUE_MAX_ELEMENTS){

		}

	}

}
