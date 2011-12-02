#include <common.h>
#include <ehci.h>
#include <ohci.h>
#include <usb.h>

void main( void )
{
	int i;
	int ret;

	console_init();

	for(i=0;;i++) {
		struct usb_device *roothub = NULL;
		struct usb_device *dev = NULL;

		printf("usb: scanning iteration %d\n", i);

		ret = ehci_init();
		if(ret < 0) {
			printf("ehci_init failed, errcode %d\n", ret);
			goto out;
		}

		/* fake OHCI initialization, the one which breaks everything */
		ohci_check();

		ret = usb_init();
		if(ret < 0) {
			printf("USB init has exited with error %d\n", ret);
			goto out;
		}

		ret = usb_scan_roothub(&roothub);
		if(ret < 0) {
			printf("failed to scan for roothub, errcode %d\n", ret);
			goto out;
		}
		if(roothub == NULL) {
			printf("unable to find roothub, this is likely an error\n");
			goto out;
		}

		ret = usb_hub_init(roothub);
		if(ret < 0) {
			printf("failed to init the roothub, errcode %d\n", ret);
			goto out;
		}

		ret = usb_hub_scan(roothub, 0, &dev);
		if(ret < 0) {
			printf("failed to scan the hub, errcode %d\n", ret);
			goto out;
		}
		if(dev == NULL) {
			printf("no usb device found on port 0\n");
			continue;
		}
	}

out:
	hang();
}
