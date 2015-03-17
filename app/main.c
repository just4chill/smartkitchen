
#include "LPC17xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>
#include "system_LPC17xx.h"
#include "uart.h"
#include "lcd.h"
#include "delay.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char testModem[] = "AT\r\n";

void prvSetupHardware( void );

void sendSms(char *num, char *msg)
{
	char cr = 0x1A;
	uart_print(1, "AT+CMGF=1\r\n");
	uart_print(1, "AT+CMGS=\"");
	uart_print(1, num);
	uart_print(1, "\"\r\n");
	uart_print(1, msg);
	uart_print(1, "\r\n");
	uart_print(1, &cr);
}

static void lcdTask(void * pvParameters)
{
	uint32_t count = 0;
	uint8_t buff[10];
	for(;;)
	{
		sprintf(buff, "H:%d,T:%d", uart1_rx_head, uart1_rx_tail);
		lcd_print(buff);
		vTaskDelay(1000);
		lcdClear();
		vTaskDelay(1);
	}
}

static void monitorModem(void * pvParameters)
{
	uint8_t result1, result2;
	char buff[75];
	for(;;)
	{
		 uart_print(1, (char *)testModem);
		// result1 = radio_read_status(TI_CCxxx0_PARTNUM);
		// result2 = radio_read_status(TI_CCxxx0_VERSION);
		// sprintf(buff, "%#02x,%#02x,%d,%d \r\n", result1, result2, uart1_rx_head, uart1_rx_tail);
		// uart_print(0, buff);
		vTaskDelay(2000);
	}
}

static void monitorWeight(void * pvParameters)
{
	char buff[75];
	uint16_t weight = 0;
	for(;;)
	{
		sprintf(buff, "%d, %d\r\n", uart3_rx_head, uart3_rx_tail);
		uart_print(0, buff);
		uart_puts(0, uart3_rx_fifo, uart3_rx_tail);
		//uart_print(3, buff);
		// if(weight > 16)
		// {
		// 	sendSms("123","123");
		// }
		vTaskDelay(100);
	}
}

int main(void)
{
	/* Setup the Hardware */
	prvSetupHardware();

	/* Create the Tasks */
	// xTaskCreate(lcdTask,
	// 		(signed portCHAR *)"lcdTask",
	// 		128,
	// 		NULL,
	// 		3,
	// 		NULL);

	// xTaskCreate(monitorModem,
	// 		(signed portCHAR *)"monitorModem",
	// 		128,
	// 		NULL,
	// 		4,
	// 		NULL);

	xTaskCreate(monitorWeight,
			(signed portCHAR *)"monitorWeight",
			128,
			NULL,
			5,
			NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Never reach here */
	return 0;
}

void prvSetupHardware( void )
{
	/* Initialte Clock */
	SystemInit();
	SystemCoreClockUpdate();
	uart_init(0, 9600);			// Debug port
	uart_print(0, "System started\r\n");
	uart_init(1, 9600);				// Modem
	uart_print(1, "AT\r\n");
	uart_init(3, 9600);				// Loadcell
	uart_print(3, "System started\r\n");
	lcd_init();
	lcd_print("Booting..");
	_delay_ms(100);
	lcdClear();
	LPC_GPIO0->FIODIR |= (1 << 21);
}

