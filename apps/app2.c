#include "ch.h"
#include "hal.h"
#include "appcfg.h"

#include "../print.h"

msg_t AppThread2(void *arg) {
	uint32_t cnt;

	(void) arg;

	cnt = 0xFFFFFFFF;
	print("Hello from APP2");
	printnl();

	while (TRUE) {
		palTogglePad(LED_GPIO, LED4);
		cnt--;
		print("APP2: ");
		printn(cnt);
		printnl();
		chThdSleepMilliseconds(500);
	}
	return 0;
}
