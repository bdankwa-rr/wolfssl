
#include <sys/types.h>

#define UART_BAUD_RATE_9600     0b000
#define UART_BAUD_RATE_19200    0b001
#define UART_BAUD_RATE_38400    0b010
#define UART_BAUD_RATE_115200   0b011
#define UART_BAUD_RATE_230400   0b100
#define UART_BAUD_RATE_460800   0b101
#define UART_BAUD_RATE_921600   0b110
#define UART_BAUD_RATE_4615385  0b111

void UART_init();
int UART_tx_byte(uint8_t out_byte);
int UART_tx_fifo_can_accept(void); 
