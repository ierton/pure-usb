#ifndef SERIAL_H
#define SERIAL_H

void serial_init(void);

void serial_putc(char c);

char serial_getc(void);

int	serial_tstc(void);

#endif 
