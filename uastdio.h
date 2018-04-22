
/* Author, Copyright: Oleg Borodin <onborodin@gmail.com> 2018 */

#ifndef UART_STDIO_H_ITU 
#define UART_STDIO_H_ITU

#define BUFFER_SIZE 64

extern buffer_t stdin_buffer;
extern buffer_t stdout_buffer;

void io_setup(void);

#endif

/* EOF */
