#ifndef __misc_h__
#define __misc_h__

/**
 * @brief gera o checksum para um buffer de dados
 */
uint8_t generateChecksum(void *data, uint32_t size);

/**
 * @brief calcula o checksum de um buffer de dados
 */
uint8_t checksumData(void *data, uint32_t size, uint8_t checksum);

/**
  * @brief Calcula o numero de pixels que uma string ocupa no display
  * @param str: String
  * @return Tamanho em pixels da string
  * */
uint32_t strlenInPixels(char *str);

/**
  * @brief Medicao do tempo de escrita do dispplay inteiro,
  * Apenas para testes de performance
  * */
void mesureDisplayDraw(void);

/**
 * @ Gera um numero pseudo aleatorio, usando o timer
 */
uint16_t rand(void);

/**
  * @brief Contagem de frames por segundo
  * Apenas para testes de performance
  * */
void MISC_countFps(void);

/**
 * @brief mostra o endereço ip no lcd
 */
void MISC_PrintIp(uint32_t *ip);

#endif /* __misc_h__ */
