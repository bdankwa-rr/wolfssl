 #include <errno.h>
 #include <sys/stat.h>
 #include <sys/times.h>
 #include <sys/unistd.h>
 #include <sys/types.h>

 #include "uart_driver.h"
 
 static volatile unsigned *const UART_Control_Reg = (unsigned *) 0x20003918;
 static volatile unsigned *const tp_tx_fifo_buf   = (unsigned *) 0x2000391C;
 static volatile unsigned *const tp_tx_qs         = (unsigned *) 0x20003924;
 
 void UART_init(){
    *(volatile uint8_t *)UART_Control_Reg = UART_BAUD_RATE_115200;
 }

 void delay(u_int32_t time){
   for (volatile int i=0; i<DELAY_TIME; i++){
      __asm__ volatile ("nop");
   }
 }
 
 int UART_tx_byte(uint8_t out_byte){
    
    *(volatile uint8_t *)tp_tx_fifo_buf = out_byte;
    //TODO: error check
    return 0;  
 }

 int UART_tx_fifo_full(void) {
    // Read UART status register and return 1 if TX FIFO not full
    return (*tp_tx_qs == 0x40)? 1 : 0;
 }