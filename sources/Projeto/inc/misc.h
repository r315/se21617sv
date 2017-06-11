#ifndef __misc_h__
#define __misc_h__

/**
 * @brief gera o checksum
 */
uint8_t generateChecksum(void *data, uint32_t size);

/**
 * @brief calcula o checksum
 */
uint8_t checksumData(void *data, uint32_t size, uint8_t checksum);

/**
  * @brief Calcula o numero de pixels que uma string ocupa no display
  * @param str: String
  * @return Tamanho em pixels da string
  * */
uint32_t strlenInPixels(char *str);

/**
  * @brief Apenas para testes de performance
  * */
void mesureDisplayDraw(void);

/**
 * @ Gera um numero pseudo aleatorio
 */
uint16_t rand(void);

/**
  * @brief Contagem de frames por segundo
  * */
void MISC_countFps(void);

#endif /* __misc_h__ */
