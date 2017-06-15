
#include <Task_Common.h>
#include <queue.h>
#include <stdio.h>


xQueueHandle *btn_queue;

#if TASK_BUTTON_DEBUG
static void LOG(char *msg){
	printf("Task Button: %s", msg);
}
#else
#define LOG(x)
#endif

BTN_Event BUTTON_QueueClear(void){
BTN_Event button;
	while(xQueueReceive(btn_queue, &button, 0) == pdPASS);
	button.value = BUTTON_EMPTY;
	button.event = BUTTON_EMPTY;
	return button;
}

BTN_Event BUTTON_QueueGet(void){
BTN_Event button;
	if(xQueueReceive(btn_queue, &button, 0) != pdPASS){
		button.value = BUTTON_EMPTY;
		button.event = BUTTON_EMPTY;
	}
	return button;
}

void Task_Button(void *ptr){
	BTN_Event button;
	btn_queue = xQueueCreate(BTN_QUEUE_MAX_ELEMENTS, sizeof(BTN_Event));

	if(btn_queue == NULL){
		LOG("Queue Creation fail\n");
		vTaskDelete(NULL);
	}

	while(1){
		BUTTON_Hit();
		if(BUTTON_GetEvents() != BUTTON_EMPTY){
			button.value = BUTTON_GetValue();
			button.event = BUTTON_GetEvents();
			if(xQueueSendToBack(btn_queue,&button, TASK_BUTTON_MAX_TICKS) == errQUEUE_FULL)
				LOG("Button QUEUE Full\n");
			taskYIELD();  //botão lido cede cpu para outra task
		}
	}
}
