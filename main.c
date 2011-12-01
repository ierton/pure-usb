#include <common.h>

void main( void )
{
	int ret;
	console_init();

	while(1) {
		printf("Hello, Arm\n");

		ret = usb_init();
		if(ret < 0) {
			printf("USB init has exited with error %d\n", ret);
			goto out;
		}
	}

out:
	hang();
}
