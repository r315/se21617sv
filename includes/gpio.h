

#ifndef _gpio_h_
#define _gpio_h_

#define GPIO_P00

#define ON 1
#define OFF 0
#define GPIO_OUTPUT 1
#define GPIO_HIGH   1
#define GPIO_LOW    0

/**
* @brief Initialyze pin and set his state
**/
void GPIO_Init(uint32_t pin, uint8_t dir, uint8_t state);

/**
* @brief Set state of given pin
**/
void GPIO_SetState(uint32_t pin, uint8_t state);

#endif
