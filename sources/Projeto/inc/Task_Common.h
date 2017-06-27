/**
 * \addtogroup Tasks
 * @{
 */
#ifndef _task_common_h_
#define _task_common_h_

#include <time.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <Task_Button.h>
#include <Task_Net.h>
#include <Task_Space.h>
#include <Task_Config.h>
#include <Task_Mem.h>

#define ON 1
#define OFF 0

#define SCORES_FORMAT (4<<8) | 10
#define ENTER_CONFIG_TIME 2000 //2 seconds

typedef enum Mstates{
  IDLE,
  CONFIG,  
  GAME,
  SAVE    
}State;

typedef struct{
    uint8_t checksum;
    GameData spaceInvaders;
    struct tm rtc;
}SaveData;

#define MAX_TOPSCORE_LEN 9      //AAA 0000\0

typedef struct{
	char name[MAX_TOPSCORE_LEN];
}RtopScore;



#define TASK_BUTTON_HEAP 		128		//128 * 4 bytes
#define TASK_BUTTON_PRIORITY	5		// range 1:30

#define TASK_MAIN_HEAP			256
#define TASK_MAIN_PRIORITY		20

#define TASK_CONFIG_HEAP		128
#define TASK_CONFIG_PRIORITY	10

#define TASK_SPACE_HEAP			128
#define TASK_SPACE_PRIORITY		20

#define TASK_NET_HEAP			128
#define TASK_NET_PRIORITY		7

#define TASK_SAVE_HEAP			128
#define TASK_SAVE_PRIORITY		5


#ifdef DEBUG
#define TASK_BUTTON_DEBUG ON
#define TASK_MAIN_DEBUG ON
#define TASK_SPACE_DEBUG ON
#define TASK_SAVE_DEBUG ON
#define TASK_STARTUP_DEBUG ON
#endif

/**
 * @ handle da Task main
 */
extern xTaskHandle *resumeTaskHandle;
#define TASK_EXIT                                                     \
		vTaskResume(resumeTaskHandle);	/* Retoma a Task Main */      \
		vTaskDelete(NULL);				/* Termina a Task corrente */ \

/**
 * @brief Semaforo e fila com os topscores recebidos do servidor
 */
extern xQueueHandle *topscore_queue;
extern xSemaphoreHandle *topscore_semaphore;


/**
 * @brief Funcao chamada pela task que executa após o startup
 */
void Task_Main(void *ptr);


/*
 *@brief Funcao chamada pela task idle, para activar esta
 *			funcionalidade o simbolo "configUSE_IDLE_HOOK 1" no ficheiro
 *			de configuracao do FreeRTOS tem de ser definido
 */
void vApplicationIdleHook(void);


#endif
