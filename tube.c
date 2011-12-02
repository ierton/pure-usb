#include <common.h>
#include <serial.h>

#define TUBE_BASE 0x20038000

void serial_init(void)
{
}

void serial_putc(char c)
{
	char str[2] = {c, '\0'};
	serial_puts(str);
}

void serial_puts(const char* c) 
{
	volatile int* tube = (volatile int*)TUBE_BASE;
	while(1){
		int i;
		char tube_word[4]={13,13,13,13};
		char *t=tube_word+3;
		for(i=0; i<4; i++){
			if (*c==0){
				*tube = *((int*)tube_word);
				return;
			}
			else{ 
				*t=*c;
				t--;
				c++;
			}
		}
		*tube = *((int*)tube_word);
	}
}

char serial_getc(void)
{
	serial_puts("serial_getc: not implemented. stopping the program.\n");
	hang();
	return 0;
}

int	serial_tstc(void)
{
	return 0;
}

void hang(void)
{
	volatile int* tube = (volatile int*)TUBE_BASE;
	*tube = 0x20202004;
}

