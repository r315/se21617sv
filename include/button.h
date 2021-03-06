/**
* @file		button.h
* @brief	Contains API header for reading buttons.
*     		
* @version	1.0
* @date		3 Nov. 2016
* @author	Hugo Reis
**********************************************************************/

#ifndef _button_h_
#define _button_h_

#include <stdint.h>

/*
 * @brief estrutura que detem o estado e o valor do botão
 * de modo a simplificar a logica da maquina de estados do botão
 * foi separado o estado e o valor do botao em duas variaveis
 * assim a variavel cur contem a mascara correspondente ao botão precionado
 * e a variavel events o evento do botão. Para teste de uma tecla premida
 * basta verificar o evento e testar cur com a mascara do botão pretendido
 */
typedef struct{
	uint32_t cur;
	uint32_t last;
	uint32_t counter;
	uint32_t events;
    uint32_t htime;
}BUTTON_Controller;

enum Bevent{
    BUTTON_EMPTY = 0,
    BUTTON_PRESSED,
    BUTTON_TIMING,
    BUTTON_HOLD,
    BUTTON_RELEASED	
};

#if defined(__LPCXpresso__)
#include <LPC17xx.h>
/**
* @brief Button connections
*		P0.26  Center
*		P0.3   Up
*		P0.2   Right
*       P0.21  Left
*       P0.1   Down
**/
#define BUTTON_F (1<<26)
#define BUTTON_S (1<<3)
#define BUTTON_R (1<<2)
#define BUTTON_L (1<<21)
#define BUTTON_D (1<<1)

#define BUTTON_A BUTTON_F
#define BUTTON_U BUTTON_S
#define BUTTON_C BUTTON_F

#define BUTTON_MASK (BUTTON_U | BUTTON_D | BUTTON_L | BUTTON_R | BUTTON_C)

#define BUTTON_Capture()   (~(LPC_GPIO0->FIOPIN) & BUTTON_MASK);
#define BUTTON_SetInput(x) LPC_GPIO0->FIODIR &= ~(x)

#define BUTTON_UP    BUTTON_U
#define BUTTON_DOWN  BUTTON_D
#define BUTTON_LEFT  BUTTON_L
#define BUTTON_RIGHT BUTTON_R

#elif defined(__LPC1768_H__)
/**
* @brief Button connections
*		P0.26  Left
*		P0.2  Fire
*		P0.3  Right
*       P0.21  Save
**/
#define BUTTON_L (1<<26)
#define BUTTON_R (1<<3)
#define BUTTON_F (1<<2)
#define BUTTON_S (1<<21)

#define BUTTON_MASK (BUTTON_L | BUTTON_R | BUTTON_F | BUTTON_S)

#define BUTTON_GetValue()  LPC_GPIO0->FIOPIN
#define BUTTON_SetInput(x) LPC_GPIO0->FIODIR &= ~(x)

#else
/**
* @brief Button connections
*		P0.11  Left
*		P0.12  Fire
*		P0.13  Right
*       P0.15  Save
**/
#define BUTTON_L (1<<11)
#define BUTTON_R (1<<13)
#define BUTTON_F (1<<12)
#define BUTTON_S (1<<15)
#define BUTTON_U (1<<16)
#define BUTTON_D (1<<17)

#define BUTTON_MASK (BUTTON_L | BUTTON_R | BUTTON_F | BUTTON_S)

#if !defined(__EMU__)
#define BUTTON_GetValue()  GPIO_Read()
#define BUTTON_SetInput(x) GPIO_SetInputN(x)
#endif
#endif /* __LPCXpresso__ */

#if defined(__EMU__)
#define loop BUTTON_GetEvents() != 256 //SDL_QUIT
#else
#define loop 1
#endif 

#define BUTTON_DEFAULT_HOLD_TIME 2000   //2 seconds


/**
* @brief Faz a iniciação do sistema para permitir o acesso aos botões
* @param ht : mask bitmap com os bits correspondentes aos pinos ligados nos botões
**/
void BUTTON_Init(int ht);

/**
* @brief Devolve o codigo (bitmap) do botão pressionado ou 0 no caso de não existir 
*        qualquer botão pressionado. Não é bloqueante.
**/
int BUTTON_Hit(void);

/**
* @brief Devolve o código (bitmap) do botão pressionado. É bloqueante.
*
**/
int BUTTON_Read(void);

/**
* @brief Devolve o código (bitmap) do estado do botão: 
* 		 pressionado (transição),
*        libertado (transição),
*        repetição (mantido pressionado)
**/
int BUTTON_GetEvents(void);

/**
 * @brief Devolve o codigo (bitmap) da tecla corrente
 *        EMPTY se nenhuma tecla pressionada
 **/
int BUTTON_GetButton(void);
#define BUTTON_GetValue BUTTON_GetButton

/**
* @brief Efectua uma espera pelo evento dado no parametro*        
**/
void BUTTON_WaitEvent(int event);

/**
* @brief configura o tempo minimo para que uma tecla 
*        seja considerada como mantida pressionada 
**/
void BUTTON_SetHoldTime(int t);
#endif

