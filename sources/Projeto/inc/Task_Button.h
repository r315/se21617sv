#ifndef __task_button_h__
#define __task_button_h__

#include  <button.h>

#ifdef DEBUG
#define TASK_BUTTON_DEBUG ON
#endif

#define TASK_BUTTON_MAX_TICKS 100

typedef struct _BTN_Event{
	uint32_t value;
	uint8_t event;
}BTN_Event;

/*
 * @brief obtem um botão da fila
 */
BTN_Event BUTTON_QueueGet(void);

#endif /* __task_button_h__ */
