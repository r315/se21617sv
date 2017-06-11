#ifndef _proj_h_
#define _proj_h_

#include <time.h>
#include "space.h"

#include <FreeRTOS.h>
#include <task.h>

#include <Task_Button.h>
#include <Task_Save.h>

#define ON 1
#define OFF 0

#define MAX_TOP_SCORES 3
#define SCORES_FORMAT (4<<8) | 10
#define ENTER_CONFIG_TIME 2000 //2 seconds

typedef enum Mstates{
  IDLE,
  CONFIG,  
  GAME,
  SAVE    
}State;

typedef struct _SaveData{
    uint32_t topscores[MAX_TOP_SCORES];    
    uint8_t checksum;
    GameData spaceInvaders;
    struct tm rtc;
}SaveData;


#define TASK_BUTTON_HEAP 		128		//128 * 4 bytes
#define TASK_BUTTON_PRIORITY	5		// range 1:30
#define BTN_QUEUE_MAX_ELEMENTS 	10

#define TASK_MAIN_HEAP			128
#define TASK_MAIN_PRIORITY		10

#define TASK_CONFIG_HEAP		128
#define TASK_CONFIG_PRIORITY	10

#define TASK_SPACE_HEAP			128
#define TASK_SPACE_PRIORITY		20

#define TASK_NET_HEAP			128
#define TASK_NET_PRIORITY		10

#define TASK_SAVE_HEAP			32
#define TASK_SAVE_PRIORITY		1


#ifdef DEBUG
#define TASK_BUTTON_DEBUG ON
#define TASK_MAIN_DEBUG ON
#define TASK_SPACE_DEBUG ON
#define TASK_SAVE_DEBUG ON
#endif

extern xTaskHandle *taskMainHandle;
#define TASK_EXIT                                                     \
		vTaskResume(taskMainHandle);	/* Retoma a Task Main */      \
		vTaskDelete(NULL);				/* Termina a Task corrente */ \


/**
 * @brief Nome das funcoes atribuidas a tasks
 */
void Task_Main(void *ptr);
void Task_Config(void *ptr);
void Task_Button(void *ptr);
void Task_Net(void *ptr);
void Task_Save(void *ptr);


/*
 *@brief Funcao chamada pela task idle, para activar esta
 *			funcionalidade o simbolo "configUSE_IDLE_HOOK 1" no ficheiro
 *			de configuracao do FreeRTOS tem de ser definido
 */
void vApplicationIdleHook(void);


#endif
