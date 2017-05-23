#include <uip.h>
#include <lcd.h>

void UIP_UTIL_PrintIp(uip_ipaddr_t ip){
	LCD_WriteString("IP: ");
	LCD_WriteInt((uint8_t)(HTONS(ip[0])>>8), 10); LCD_WriteChar('.');
	LCD_WriteInt((uint8_t)HTONS(ip[0]), 10); LCD_WriteChar('.');
	LCD_WriteInt((uint8_t)(HTONS(ip[1])>>8), 10); LCD_WriteChar('.');
	LCD_WriteInt((uint8_t)HTONS(ip[1]), 10); LCD_WriteChar('\n');
}
