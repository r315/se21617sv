/**
 * \addtogroup Tasks
 * @{
 */
#ifndef __task_button_h__
#define __task_button_h__

#include  <button.h>

#ifdef DEBUG
#define TASK_BUTTON_DEBUG ON
#endif

#define TASK_BUTTON_MAX_TICKS 100
#define BTN_QUEUE_MAX_ELEMENTS 	10

typedef struct{
	uint32_t value;
	uint8_t event;
}BTN_Event;

/**
 * @brief obtem um botão da fila, não é bloquente
 */
BTN_Event BUTTON_QueueGet(void);

/**
 * @brief Remove todos os eventos na fila
 */
BTN_Event BUTTON_QueueClear(void);

/**
 * @brief Funcao chamada pela task que efectua a leitura dos botões
 */
void Task_Button(void *ptr);

#endif /* __task_button_h__ */
