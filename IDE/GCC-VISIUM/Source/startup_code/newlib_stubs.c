/*
 * newlib_stubs.c
 *
 * created on: 5 Jul 2016
 *      Author: STU123
 *
 */

 #include <errno.h>
 #include <sys/stat.h>
 #include <sys/times.h>
 #include <sys/unistd.h>
 #include <sys/types.h>

 #include "uart_driver.h"

int fstat(int file, struct stat *st) {
    return 0;
 }

int isatty(int file) {
    return 1;
 }

off_t lseek (int __fildes, off_t __offset, int __whence ) {
    return 0;
 }

int read (int __fd, void *__buf, size_t __nbyte ) {
    return 0;
 }

int write (int __fd, const void *__buf, size_t __nbyte ) {
   (void)__fd;
   const uint8_t *buf = (const uint8_t *)__buf;

   for (size_t i = 0; i < __nbyte; i++) {
      while (UART_tx_fifo_full()) {
            __asm__ volatile ("nop");
      }
      UART_tx_byte(buf[i]);    
      delay(DELAY_TIME); 
   }
   return (int)__nbyte;
   
   /* int todo;

    char *buf;

    buf = (char *)__buf;

    for (todo = 0; todo < __nbyte; todo++) {
        *tp_tx_fifo_buf = (*buf++);
    }
    return __nbyte; */
}

int close (int __fildes ) {
    return 0;
}

// Minimal implementation of _write
ssize_t _write(int file, const void *ptr, size_t len) {
    return write(file, ptr, len); // Pretend all bytes were written
}

// Minimal implementation of _sbrk (for memory allocation)
void *_sbrk(ptrdiff_t incr) {
  extern char _sheap;
  extern char _eheap;
  static char* heap_ptr = &_sheap;
  char* prev_heap_ptr = heap_ptr;

  if((heap_ptr + incr) > &_eheap){
    return (void*)-1;
  }

  heap_ptr += incr;
  return prev_heap_ptr;
}

// Define errno for Newlib compatibility
int errno;

// Minimal implementation of _fstat
int _fstat(int file, struct stat *st) {
    st->st_mode = S_IFCHR; // Character device
    return 0;
}

// Minimal implementation of _isatty
int _isatty(int file) {
    return 1; // Always true for standard streams
}

// Minimal implementation of _lseek
off_t _lseek(int file, off_t ptr, int dir) {
    return 0;
}

// Minimal implementation of _close
int _close(int file) {
    return -1; // Not supported
}

// Minimal implementation of _read
ssize_t _read(int file, void *ptr, size_t len) {
    return 0; // Not supported
}