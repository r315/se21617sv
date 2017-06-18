#include <string.h>
#include <stdint.h>
#include <flash.h>
#include <eeprom.h>
#include <Task_Common.h>
#include <queue.h>
#include <stdio.h>
#include <led.h>

xQueueHandle *sendqueue = 0;
xSemaphoreHandle *rdy;

#if TASK_SAVE_DEBUG
static void LOG(char *msg){
	printf("Task SAVE: %s", msg);
}
#else
#define LOG(x)
#endif

void queuePut(void* data, uint32_t size, uint8_t oper){
SAVE_Item svi;
	svi.data = data;
	svi.size = size;
	svi.operation = oper;

	if(!sendqueue){// TODO fix concurrency issue
		sendqueue = xQueueCreate(SAVE_QUEUE_MAX_ELEMENTS, sizeof(SAVE_Item));
		vSemaphoreCreateBinary(rdy);
	}

	if(xQueueSendToBack(sendqueue, &svi, TASK_SAVE_WAIT_TICKS) == errQUEUE_FULL)
		LOG("Save QUEUE Full\n");
	else
		LED_SetState(LED_ON);
}

void SAVE_QueuePut(void *data, uint32_t size){
	queuePut(data,size, SAVE_WRITE);
}

void SAVE_QueueGet(void *data, uint32_t size){
	queuePut(data,size, SAVE_READ);
	xSemaphoreTake(rdy,TASK_SAVE_WAIT_TICKS); // block until data is ready or timeout
}

void Task_Save(void *ptr){
SAVE_Item itm;
	while(1){
			if(xQueueReceive(sendqueue, &itm, portMAX_DELAY) == pdPASS){		//blocking call
				if(itm.operation == SAVE_READ){
					EEPROM_Read(SAVE_BASE_ADDRESS, itm.data, itm.size);
				}else{
					EEPROM_Write(SAVE_BASE_ADDRESS, itm.data , itm.size);
				}
				xSemaphoreGive(rdy);
			LED_SetState(LED_OFF);
		}
	}
}
